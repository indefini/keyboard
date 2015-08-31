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

pub enum KeyKind
{
    Normal(String),
    Func(elm::RustCb),
    //Modifier(String)
}

pub struct Key
{
    eo : *mut elm::Evas_Object,
    name : String,
    kind : KeyKind,
    down : bool,
    device : i32
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
    shift : bool
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
        rows :&Vec<Vec<&str>>,
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
        max_col = max_col.max(get_real_len(r));
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

fn main() {
    unsafe { elm::init() };

    let win = unsafe {elm::window_new()};
    let (dpix, dpiy, w, h) = elm::get_dpi_size(win);

    let config = calc_config(dpix, dpiy);

    //let rows = rows4();
    let rows = rowsnum();

    let mut container = Container {
        keys : Vec::new(),
        touch : [Touch {x:0, y:0, down:false}; 10],
        shift : false
    };

    create_keyboard_with_rects(&rows, &mut container, win);

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


fn calc_keyboard_size(dpix : usize, dpiy : usize, w : usize, h : usize, rows :&Vec<Vec<&str>>)
    -> (usize, usize, usize, usize, usize, usize)
{
    let mut max_col = 0f32;
    for r in rows.iter() {
        //max_col = cmp::max(max_col, get_real_len(r));
        max_col = max_col.max(get_real_len(r));
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

fn create_keyboard_with_rects(
    rows : &Vec<Vec<&str>>,
    container : &mut Container,
    win : *const elm::Evas_Object
    )
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
    let k = unsafe {elm::keyboard_new(win, px as i32, py as i32, kx as i32, ky as i32, ksx as i32, ksy as i32)};

    create_keys(k, rows, container);
    unsafe {elm::keyboard_bg_add(
            input_down,
            input_up,
            input_move,
            mem::transmute(container))
    };
}

fn create_keys(k: *mut elm::Keyboard, rows : &Vec<Vec<&str>>, container : &mut Container)
{
    let mut row = 0;
    for r in rows.iter() {

        let mut col_keys = Vec::new();

        for c in (*r).iter() {

            let s : Vec<&str> = c.split(',').collect();

            let w = if s.len() > 1 {
                //s[1].parse::<usize>().unwrap()
                s[1].parse::<f32>().unwrap()
            }
            else {
                1f32
            };

            let realkey = if s.len() > 2 {
                //s[1].parse::<usize>().unwrap()
                s[2]
            }
            else {
                s[0]
            };

            if c.starts_with("__reduce") {
                /*
                unsafe {
                    elm::keyboard_fn_add(
                        k,
                        cstring_new(&c[2..]),
                        close,
                        ptr::null(),
                        pos as i32,
                        row,
                        width as i32,
                        1);
                }
                */
                let r = unsafe {elm::keyboard_rect_add(
                    k,
                    cstring_new(&c[2..]),
                    row,
                    w)};

                let key = Key {
                    eo : r,
                    name : String::from(s[0]),
                    kind : KeyKind::Func(reduce),
                    down : false,
                    device : 0
                };
                col_keys.push(key);
            }
            else if c.starts_with("__close") {
                let r = unsafe {elm::keyboard_rect_add(
                    k,
                    cstring_new(&c[2..]),
                    row,
                    w)};

                let key = Key {
                    eo : r,
                    name : String::from(s[0]),
                    kind : KeyKind::Func(close),
                    down : false,
                    device : 0
                };
                col_keys.push(key);
            }
            else {
                unsafe {
                    /*
                    elm::keyboard_add(
                        k,
                        cstring_new(s[0]),
                        pos as i32,
                        row,
                        (width*w) as i32,
                        1);
                        */
                let keyname = if c.starts_with("comma") {
                    ","
                }
                else {
                    s[0]
                };

                    let r = elm::keyboard_rect_add(
                        k,
                        cstring_new(keyname),
                        row,
                        w);

                    let key = Key {
                        eo : r,
                        name : String::from(s[0]),
                        kind : KeyKind::Normal(String::from(realkey)),
                        down : false,
                        device : 0
                    };
                    col_keys.push(key);
                }
            }
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
                match k.kind {
                    KeyKind::Normal(ref s) => {
                        //con.handle_normal_key(s);
                        if s == "Shift_L" && !con.shift {
                            con.shift = true;
                            unsafe {
                             elm::ecore_x_test_fake_key_down(cstring_new(s));
                            }
                        }
                        else if s == "BackSpace" {
                            unsafe {
                             elm::ecore_x_test_fake_key_down(cstring_new(s));
                            }
                        }
                        else {
                            unsafe {
                             elm::ecore_x_test_fake_key_press(cstring_new(s));
                            }
                        }

                    },
                    KeyKind::Func(ref cb) => {
                        unsafe {
                            cb(data);
                        }
                    }
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

    for c in con.keys.iter_mut() {
        for k in c.iter_mut() {
            if k.down && unsafe {elm::is_point_inside(k.eo, x, y)} {
                if k.name == "Shift_L" {
                     con.shift = false;
                    unsafe {
                     elm::ecore_x_test_fake_key_up(cstring_new(&k.name));
                     }
                }
                else if k.name == "BackSpace" {
                    unsafe {
                        elm::ecore_x_test_fake_key_up(cstring_new(&k.name));
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
                    if k.name == "Shift_L" {
                         con.shift = false;
                        unsafe {
                         elm::ecore_x_test_fake_key_up(cstring_new(&k.name));
                        }
                    }
                }
            }
            else if false && unsafe {elm::is_point_inside(k.eo, x, y)} {
                k.down = true;
                k.device = device;
                unsafe { elm::evas_object_color_set(k.eo, 150, 150, 150, 255)};
                match k.kind {
                    KeyKind::Normal(ref s) => {
                        if s == "Shift_L" && !con.shift {
                            con.shift = true;
                            unsafe {
                             elm::ecore_x_test_fake_key_down(cstring_new(s));
                            }
                        }
                        else {
                        unsafe {
                            elm::ecore_x_test_fake_key_press(cstring_new(s));
                        }
                        }

                    },
                    KeyKind::Func(ref cb) => {
                        unsafe {
                            cb(data);
                        }
                    }
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

fn mm_to_px(mm : f32, dpi : usize) -> usize
{
    let f = (dpi as f32) / 25.4f32 * mm;
    f as usize
}

