extern crate libc;
extern crate elementary_sys as elm;

use std::cmp;
use std::ptr;
use std::ffi::CString;
//use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use libc::{c_char, c_void, c_int};
use std::mem;

const KEY_X_MM : f32 = 17f32;
const KEY_Y_MM : f32 = 17f32;
const KEYSPACE_X_MM : f32 = 0.8f32;
const KEYSPACE_Y_MM : f32 = 0.8f32;

#[derive(Clone)]
enum Special
{
    Esc,
    Tab,
    Caps,
    Backspace,
    Enter,
    Space,

    Up,
    Down,
    Left,
    Right,

    PageUp,
    PageDown
}

impl Special
{
    fn get_fake(&self) -> &str
    {
        match *self {
            Special::Esc => "Escape",
            Special::Tab => "Tab",
            Special::Caps => "Caps_Lock",
            Special::Backspace => "BackSpace",
            Special::Enter => "Return",
            Special::Space => "space",
            Special::Up => "Up",
            Special::Down => "Down",
            Special::Left => "Left",
            Special::Right => "Right",
            Special::PageUp => "Page_Up",
            Special::PageDown => "Page_Down",
        }
    }
}

#[derive(Clone)]
enum Modifier
{
    Shift,
    Control,
    Alt,
}

impl Modifier
{
    fn get_fake(&self) -> &str
    {
        match *self {
            Modifier::Shift => "Shift_L",
            Modifier::Control => "Control_L",
            Modifier::Alt => "Alt_L",
        }
    }
}

#[derive(Clone)]
enum KeyT
{
    //Normal(String),
    Modifier(Modifier),
    Func(elm::RustCb),
    Special(Special),
    //Modifier(String)
    Ch(Ch),
    Empty
}

#[derive(Clone)]
struct KeyDef
{
    kind : KeyT,
    width : f32,
    height : f32,
    name : String,
}

impl KeyDef{
    fn new(kind : KeyT, name : &str ) -> KeyDef
    {
        KeyDef {
            kind : kind,
            width : 1f32,
            height : 1f32,
            name : String::from(name),
        }
    }

    fn with_size(kind : KeyT , name : &str, w : f32, h : f32)  -> KeyDef
    {
        KeyDef {
            kind : kind,
            width : w,
            height : h,
            name : String::from(name)
        }
    }

    fn get_name(&self) -> (&str, Option<&str>)
    {
        match self.kind {
            KeyT::Ch(ref c) => {
                match c.shift {
                    Some(ref s) => (c.click.1.as_ref(), Some(s.1.as_ref())),
                    None => (c.click.1.as_ref(), None)
                 }
            },
            _ => (self.name.as_ref(), None)

        }
    }
}

#[derive(Clone)]
struct Ch
{
    click : (String,String),
    shift : Option<(String,String)>,
    long : Vec<String>,
}

impl Ch
{
    fn new(c : &str) -> Ch
    {
        Ch {
            click : (String::from(c), String::from(c)),
            shift : None,
            long : Vec::new()
        }
    }

    fn with_shift(c : &str, shift : &str, disp1 : &str, disp2 : &str) -> Ch
    {
        Ch {
            click : (String::from(c),String::from(disp1)),
            shift : Some((String::from(shift),String::from(disp2))),
            long : Vec::new()
        }
    }

    fn with_shift_long(
        c : &str,
        shift : Option<&str>,
        long : Vec<String>,
        ) -> Ch
    {
        Ch {
            click : (String::from(c),String::from(c)),
            shift : match shift {
                    Some(s) => Some((String::from(s), String::from(c))),
                    None => None
            },
            long : long
        }
    }
}

pub struct Key
{
    eo : *mut elm::Evas_Object,
    def : KeyDef,
    down : bool,
    device : i32
}

impl Key
{
    fn key_up(&mut self, con : &mut Container)
    {
        match self.def.kind {
            KeyT::Special(ref s) => {
                unsafe {
                    elm::ecore_x_test_fake_key_up(cstring_new(s.get_fake()));
                }
            },
            KeyT::Modifier(ref m) => {
                match *m {
                    Modifier::Shift => {
                        con.shift = false;
                    },
                    Modifier::Control => {
                        con.ctrl = false;
                    },
                    Modifier::Alt => {
                        println!("todo");
                    }
                }
                unsafe {
                    elm::ecore_x_test_fake_key_up(cstring_new(m.get_fake()));
                }
            }
            _ => {
            }
        }

        self.down = false;
        //println!("found object {} ", self.name);
        unsafe { elm::evas_object_color_set(self.eo, 80, 80, 80, 255)};
    }
}


#[derive(Copy, Clone)]
pub struct Touch
{
    x : i32,
    y : i32,
    down : bool
}

pub struct Container
{
    keys : Vec<Vec<Key>>,
    touch : [Touch;10],
    shift : bool,
    ctrl : bool,
    keyboard : *mut elm::Keyboard
}

pub struct Config
{
   key_width : usize,
   key_height : usize,

   key_space_minx : usize,
   key_space_miny : usize,
   key_space_maxx : usize,
   key_space_maxy : usize,

   paddingx : usize,
   paddingy : usize,

   //ideal_width;
   //ideal_index;
   //ideal_factor_width;
   //int more;
}

fn get_size(
    rows :&Vec<Vec<KeyDef>>,
    //config : &Config,
    screenw : usize,
    screenh : usize,
    dpix : usize,
    dpiy : usize
    ) -> (usize, usize)
{
    let mut max_col = 0f32;
    for r in  rows.iter() {
        //max_col = cmp::max(max_col, get_real_len(r));
        max_col = max_col.max(get_real_len2(r));
    }

    let rows_num = rows.len() as f32;

    let width_mm = max_col * KEY_X_MM + (max_col - 1f32) * KEYSPACE_X_MM;
    let height_mm = rows_num * KEY_Y_MM + (rows_num - 1f32) * KEYSPACE_Y_MM;

    let width_px = mm_to_px(width_mm, dpix);
    let height_px = mm_to_px(height_mm, dpiy);

    if width_px <= screenw {
        if height_px <= screenh {
            return (width_px, height_px);
        }
        else {
            //TODO recalc height
            return (width_px, height_px);
        }
    }
    else {
        let ratio = screenw as f32 / width_px  as f32;
        return (screenw, (height_px as f32 * ratio) as usize);

    }



    (
        width_px, height_px
    )
}


/*
impl Container
{
    fn handle_normal_key(&mut self, s : &str)
    {
        unsafe {
            elm::ecore_x_test_fake_key_press(cstring_new(s));
        }
    }
}
*/

fn rows4<'a>() -> Vec<Vec<&'a str>>
{
    let row0 = vec![ "Escape","q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "@,1,at","[,1,bracketleft", "BackSpace,1.3" ];//, r"\" ];
    let row1 = vec![ "Tab,1.3", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";,1,semicolon", ":,1,colon","],1,bracketright", "Return" ];
    let row2 = vec![ "Shift_L,1.6","z", "x", "c", "v", "b", "n", "m", "comma,1,comma", ".,1,period", r"/,1,slash", r"\,1,backslash", "up,1,Up"];
    let row3 = vec![ "Control_L,1.6", "__empty,2", "space,7", "__empty,1.4", "__reduce", "__close"];
    //let row3 = vec![ "Control_L,1.6", "__empty,2", "space,6", "__empty,1", "__close", "left,1,Left", "down,1,Down", "right,1,Right"];

    vec![row0, row1, row2, row3]
}

fn rowsnum<'a>() -> Vec<Vec<&'a str>>
{
    let rownum = vec![ "Escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-,1,minus","^,1,asciicircum", r"\,1,yen" ,"BackSpace" ];
    //let row0 = vec![ "Tab,1.3","q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "@","[" ];//, r"\" ];
    //let row1 = vec![ "__empty,1.6", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", ":" ];
    //let row2 = vec![ "Shift_L,1.9","z", "x", "c", "v", "b", "n", "m", "<", ">", "?", r"\"];
    //let row3 = vec![ "Control_L,1.9", "__empty,3", "space,4", "__empty,2", "__reduce", "__close"];

    let row0 = vec![ "Tab,1.3","q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "@,1,at","[,1,bracketleft", "Return,1.7" ];//, r"\" ];
    let row1 = vec![ "Kanji,1.6", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";,1,semicolon", ":,1,colon","],1,bracketright", "Return,1.4" ];
    let row2 = vec![ "Shift_L,1.9","z", "x", "c", "v", "b", "n", "m", "comma,1,comma", ".,1,period", r"/,1,slash", r"\,1,backslash", "up,1,Up"];
    //let row3 = vec![ "Control_L,2.2", "__empty,2", "space,7", "__empty,1.4", "__reduce", "__close"];
    //let row3 = vec![ "Control_L,2.2", "__empty,1.8", "space,7", "__empty,1", "__reduce", "__close", "left,1,Left", "down,1,Down", "right,1,Right"];
    let row3 = vec![ "Control_L,2.2", "__empty,1.7", "space,6", "__close", "__empty,1", "left,1,Left", "down,1,Down", "right,1,Right"];

    vec![rownum, row0, row1, row2, row3]
}

macro_rules! ch {
    ($l:expr) => (KeyDef::new(KeyT::Ch(Ch::new($l)),$l));
    ($l:expr, $shift:expr, $disp1:expr, $disp2:expr) => 
        (KeyDef::new(KeyT::Ch(Ch::with_shift($l,$shift,$disp1, $disp2)),$disp1))
}

macro_rules! sp {
    ($l:expr,$name:expr) => (KeyDef::new(KeyT::Special($l),$name));
    ($l:expr,$name:expr, $w:expr) => (KeyDef::with_size(KeyT::Special($l),$name,$w,1f32));
    ($l:expr,$name:expr, $w:expr, $h:expr) => (KeyDef::with_size(KeyT::Special($l),$name,$w,$h));
}

macro_rules! modi {
    ($l:expr,$name:expr) => (KeyDef::new(KeyT::Modifier($l),$name));
    ($l:expr,$name:expr, $w:expr) => (KeyDef::with_size(KeyT::Modifier($l),$name,$w,1f32));
}

macro_rules! empty {
    ($w:expr) => (KeyDef::with_size(KeyT::Empty,"",$w,1f32));
}

macro_rules! func {
    ($f:expr,$name:expr) => (KeyDef::new(KeyT::Func($f),$name));
}



fn rows_test() -> Vec<Vec<KeyDef>>
{
    let row0 = vec![
        sp!(Special::Esc, "Esc"),
        ch!("q"),
        ch!("w"),
        ch!("e"),
        ch!("r"),
        ch!("t"),
        ch!("y"),
        ch!("u"),
        ch!("i"),
        ch!("o"),
        ch!("p"),
        ch!("at", "grave", "@", "`"),
        ch!("bracketleft", "braceleft", "[","{"),
        sp!(Special::Backspace, "Backspace", 1.3f32),
    ];

    let row1 = vec![
        sp!(Special::Tab, "Tab", 1.3f32),
        ch!("a"),
        ch!("s"),
        ch!("d"),
        ch!("f"),
        ch!("g"),
        ch!("h"),
        ch!("j"),
        ch!("k"),
        ch!("l"),
        ch!("semicolon", "plus", ";","+"),
        ch!("colon", "asterisk",":","*"),
        ch!("bracketright", "braceright","]","}"),
        sp!(Special::Enter, "Enter")
    ];

    let row2 = vec![
        modi!(Modifier::Shift, "Shift", 1.6f32),
        ch!("z"),
        ch!("x"),
        ch!("c"),
        ch!("v"),
        ch!("b"),
        ch!("n"),
        ch!("m"),
        ch!("comma","less",",","<"),
        ch!("period", "greater",".",">"),
        ch!("slash", "question", "/","?"),
        ch!("backslash", "underscore","\\","_"),
        sp!(Special::Up, "Up" )
    ];

    let row3 = vec![
        modi!(Modifier::Control, "Ctrl", 1.6f32),
        empty!(2f32),
        sp!(Special::Space, "", 7f32),
        empty!(1.4f32),
        func!(reduce, "reduce"),
        func!(close, "close"),
        //mov!(Move::Left,"left"),
        //mov!(Move::Down, "down"),
        //mov!(Move::Right, "right"),
    ];

    vec![row0, row1, row2, row3]
}


fn main() {
    unsafe { elm::init() };

    let win = unsafe {elm::window_new()};
    let (dpix, dpiy, w, h) = elm::get_dpi_size(win);

    let config = calc_config(dpix, dpiy);

    //let rows = rows4();
    //let rows = rowsnum();

    let rows = rows_test();

    let keyboard = create_keyboard(&rows, win);

    let mut container = Container {
        keys : Vec::new(),
        touch : [Touch {x:0, y:0, down:false}; 10],
        shift : false,
        ctrl : false,
        keyboard : keyboard
    };

    create_keys_bg(&rows, &mut container);

    //create_keyboard_with_table_buttons(&rows);

    unsafe {
        elm::run();
    }
}

fn calc_config(dpix : usize, dpiy : usize) //, rows :&Vec<Vec<&str>>)
    -> Config
{
    Config {
        key_width : mm_to_px(KEY_X_MM, dpix),
        key_height : mm_to_px(KEY_Y_MM, dpiy),

        key_space_minx : mm_to_px(KEYSPACE_X_MM, dpix),
        key_space_miny : mm_to_px(KEYSPACE_Y_MM, dpiy),
        key_space_maxx : mm_to_px(KEYSPACE_X_MM, dpix),
        key_space_maxy : mm_to_px(KEYSPACE_Y_MM, dpiy),

        paddingx : 0,
        paddingy : 0,
    }
}

fn calc_keyboard_size(dpix : usize, dpiy : usize, w : usize, h : usize, rows :&Vec<Vec<KeyDef>>)
    -> (usize, usize, usize, usize, usize, usize)
{
    let mut max_col = 0f32;
    for r in rows.iter() {
        //max_col = cmp::max(max_col, get_real_len(r));
        max_col = max_col.max(get_real_len2(r));
    }

    let rows_num = rows.len() as f32;

    let width_mm = max_col * KEY_X_MM + (max_col - 1f32) * KEYSPACE_X_MM;
    let height_mm = rows_num * KEY_Y_MM + (rows_num - 1f32) * KEYSPACE_Y_MM;

    let width_px = mm_to_px(width_mm, dpix);
    let height_px = mm_to_px(height_mm, dpiy);


    (
        width_px, height_px,
        mm_to_px(KEY_X_MM, dpix),
        mm_to_px(KEY_Y_MM, dpiy),
        mm_to_px(KEYSPACE_X_MM, dpix),
        mm_to_px(KEYSPACE_Y_MM, dpiy),
     )

}

fn create_keyboard(
    rows : &Vec<Vec<KeyDef>>,
    win : *const elm::Evas_Object)
    -> *mut elm::Keyboard
{
    let (dpix, dpiy, w, h) = elm::get_dpi_size(win);
    println!("dpix, dpiy {}, {}", dpix, dpiy);
    let (mut px, mut py, kx, ky, ksx, ksy) = calc_keyboard_size(dpix, dpiy, w, h, rows);
    //px = cmp::min(px, w);
    //py = cmp::min(py, h);

    let (sizex, sizey) = get_size(rows, w, h, dpix, dpiy);
    px = sizex;
    py = sizey;

    println!("px, py {}, {}, key space {}, {}", px, py, ksx, ksy);
    let keyboard = unsafe {elm::keyboard_new(win, px as i32, py as i32, kx as i32, ky as i32, ksx as i32, ksy as i32)};

    keyboard
}

fn create_keys_bg(
    rows : &Vec<Vec<KeyDef>>,
    container : &mut Container,
    )
{
    create_keys(rows, container);
    unsafe {elm::keyboard_bg_add(
            input_down,
            input_up,
            input_move,
            mem::transmute(container))
    };
}

fn create_keys(rows : &Vec<Vec<KeyDef>>, container : &mut Container)
{
    let k = container.keyboard;
    let mut row = 0;
    for r in rows.iter() {

        let mut col_keys = Vec::new();

        for c in (*r).iter() {
            let w = c.width;

            //TODO display text better than "text1,text2"
            let mut name = String::new();
            match c.get_name() {
                (s1, Some(s2)) => {
                    name.push_str(s1);
                    name.push_str(",");
                    name.push_str(s2);
                },
                (s1, None) => {
                    name.push_str(s1);
                }
            }

            let r = unsafe {elm::keyboard_rect_add(
                    k,
                    cstring_new(name.as_ref()),
                    row,
                    w)};


            let key = Key {
                eo : r,
                def : (*c).clone(),
                down : false,
                device : 0
            };
            col_keys.push(key);
        }
        row = row + 1;
        container.keys.push(col_keys);
    }

}

fn create_keyboard_with_table_buttons(rows : &Vec<Vec<&str>>)
{
    let win = unsafe {elm::window_new()};
    //let k = unsafe {elm::keyboard_new(win)};
    //create_keys(k, rows);
}

fn cstring_new(s : &str) -> *const c_char
{
    //let to_print = &b"Hello, world!"[..];
    CString::new(s).unwrap().as_ptr()
}

extern fn reduce(data : *mut c_void) {
    let con : &mut Container = unsafe { mem::transmute(data) };

    for t in con.touch.iter_mut() {
      t.down = false;
    }

    for c in con.keys.iter_mut() {
        for k in c.iter_mut() {
          k.down = false;
          unsafe { elm::evas_object_color_set(k.eo, 80, 80, 80, 255)};
        }
    }

  unsafe { elm::reduce() };
}

extern fn close(data : *mut c_void) {
    unsafe { elm::kexit() };
}


extern fn input_down(data : *mut c_void, device : c_int, x : c_int, y : c_int) {
    //println!("pressed {}, {}", x, y);
    let con : &mut Container = unsafe { mem::transmute(data) };
    con.touch[device as usize].down = true;

    for c in con.keys.iter_mut() {
        for k in c.iter_mut() {
            if !k.down && unsafe {elm::is_point_inside(k.eo, x, y)} {
                k.down = true;
                k.device = device;
                unsafe { elm::evas_object_color_set(k.eo, 150, 150, 150, 255)};

                match k.def.kind {
                    KeyT::Ch(ref s) => {
                        unsafe {
                            elm::ecore_x_test_fake_key_press(cstring_new(&*s.click.0));
                            let display = if con.shift {
                                if let Some(ref ss) = s.shift {
                                    ss.1.as_ref()
                                }
                                else {
                                    s.click.1.as_ref()
                                }
                            }
                            else {
                                s.click.1.as_ref()
                            };
                            elm::keyboard_popup_show(
                                con.keyboard,
                                k.eo,
                                cstring_new(display));
                        }
                    },
                    KeyT::Special(ref f) => {
                        unsafe {
                            elm::ecore_x_test_fake_key_down(cstring_new(f.get_fake()));
                        }
                    },
                    KeyT::Modifier(ref m) => {
                        let key = cstring_new(m.get_fake());
                        match *m {
                            Modifier::Shift => if !con.shift {
                                con.shift = true;
                                unsafe {
                                    elm::ecore_x_test_fake_key_down(key);
                                }
                            },
                            Modifier::Control => if !con.ctrl {
                                con.ctrl = true;
                                unsafe {
                                    elm::ecore_x_test_fake_key_down(key);
                                }
                            },
                            Modifier::Alt => {
                                println!("Todo");
                            },
                        }
                    },
                    KeyT::Func(ref cb) => {
                        unsafe {
                            cb(data);
                        }
                    },
                    _ => {}
                }
                break;
            }
        }
    }
}

extern fn input_up(data : *mut c_void, device : c_int, x : c_int, y : c_int)
{
    let con : &mut Container = unsafe { mem::transmute(data) };
    con.touch[device as usize].down = false;

    unsafe { elm::keyboard_popup_hide(con.keyboard);}

    for c in con.keys.iter_mut() {
        for k in c.iter_mut() {
            if k.down && unsafe {elm::is_point_inside(k.eo, x, y)} {

                match k.def.kind {
                    KeyT::Special(ref s) => {
                        unsafe {
                            elm::ecore_x_test_fake_key_up(cstring_new(s.get_fake()));
                        }
                    },
                    KeyT::Modifier(ref m) => {
                        match *m {
                            Modifier::Shift => {
                                con.shift = false;
                            },
                            Modifier::Control => {
                                con.ctrl = false;
                            },
                            Modifier::Alt => {
                                println!("todo");
                            }
                        }
                        unsafe {
                            elm::ecore_x_test_fake_key_up(cstring_new(m.get_fake()));
                        }
                    }
                    _ => {
                    }
                }

                k.down = false;
                //println!("found object {} ", k.name);
                unsafe { elm::evas_object_color_set(k.eo, 80, 80, 80, 255)};
            }
        }
    }
}

extern fn input_move(data : *mut c_void, device : c_int, x : c_int, y : c_int)
{
    let con : &mut Container = unsafe { mem::transmute(data) };
    if !con.touch[device as usize].down {
        return;
    }

    for c in con.keys.iter_mut() {
        for k in c.iter_mut() {
            if k.down {
                if k.device == device && unsafe {!elm::is_point_inside(k.eo, x, y)} {
                    k.down = false;
                    //println!("found object {} ", k.name);
                    unsafe { elm::evas_object_color_set(k.eo, 80, 80, 80, 255)};
                    unsafe { elm::keyboard_popup_hide(con.keyboard);}

                    match k.def.kind {
                        KeyT::Special(ref s) => {
                            unsafe {
                                elm::ecore_x_test_fake_key_up(cstring_new(s.get_fake()));
                            }
                        },
                        KeyT::Modifier(ref m) => {
                            match *m {
                                Modifier::Shift => {
                                    con.shift = false;
                                },
                                Modifier::Control => {
                                    con.ctrl = false;
                                },
                                Modifier::Alt => {
                                    println!("todo");
                                }
                            }
                            unsafe {
                                elm::ecore_x_test_fake_key_up(cstring_new(m.get_fake()));
                            }
                        }
                        _ => {
                        }
                    }

                }
            }
            else if false && unsafe {elm::is_point_inside(k.eo, x, y)} {
                k.down = true;
                k.device = device;
                unsafe { elm::evas_object_color_set(k.eo, 150, 150, 150, 255)};
                match k.def.kind {
                    KeyT::Ch(ref s) => {
                        unsafe {
                            elm::ecore_x_test_fake_key_press(cstring_new(&k.def.name));
                            elm::keyboard_popup_show(
                                con.keyboard,
                                k.eo,
                                cstring_new(&k.def.name));
                        }
                    },
                    KeyT::Special(ref f) => {
                        unsafe {
                            elm::ecore_x_test_fake_key_down(cstring_new(f.get_fake()));
                        }
                    },
                    KeyT::Modifier(ref m) => {
                        let key = cstring_new(m.get_fake());
                        match *m {
                            Modifier::Shift => if !con.shift {
                                con.shift = true;
                                unsafe {
                                    elm::ecore_x_test_fake_key_down(key);
                                }
                            },
                            Modifier::Control => if !con.ctrl {
                                con.ctrl = true;
                                unsafe {
                                    elm::ecore_x_test_fake_key_down(key);
                                }
                            },
                            Modifier::Alt => {
                                println!("Todo");
                            },
                        }
                    },
                    KeyT::Func(ref cb) => {
                        unsafe {
                            cb(data);
                        }
                    },
                    _ => {}
                }
            }
        }
    }
}

extern fn input_update(data : *mut c_void) -> bool
{
    println!("update");
    true
}

fn get_button_count(v : &Vec<&str>) -> usize
{
    let l = v.iter().filter(|s| !s.starts_with("__empty")).count();
    l
}

fn get_real_len(v : &Vec<&str>) -> f32
{
    let mut l = 0f32;
    for c in v.iter() {
        let s : Vec<&str> = c.split(',').collect();
        let w = if s.len() > 1 {
            println!("process : {}", s[1]);
            s[1].parse::<f32>().unwrap()
        }
        else {
            1f32
        };
        l += w;
    }

    l
}

fn get_real_len2(v : &Vec<KeyDef>) -> f32
{
    let mut l = 0f32;
    for c in v.iter() {
        l += c.width;
    }

    l
}

fn mm_to_px(mm : f32, dpi : usize) -> usize
{
    let f = (dpi as f32) / 25.4f32 * mm;
    f as usize
}

