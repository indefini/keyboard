extern crate libc;
extern crate elementary_sys as elm;

use std::cmp;
use std::ptr;
use std::ffi::CString;
//use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use libc::{c_char, c_void, c_int};
use std::mem;

pub enum KeyKind
{
    Normal(String),
    Func(elm::RustCb)
}


pub struct Key
{
    eo : *mut elm::Evas_Object,
    name : String,
    kind : KeyKind,
    down : bool
}

pub struct Container
{
    keys : Vec<Vec<Key>>,

}

fn main() {
    unsafe { elm::init() };

    let row0 = vec![ "q", "w", "e", "r", "t", "y", "u", "i", "o", "p" ];
    let row1 = vec![ "a", "s", "d", "f", "g", "h", "j", "k", "l" ];
    let row2 = vec![ "z", "x", "c", "v", "b", "n", "m"];
    let row3 = vec![ "__close", "__empty,2", "space,4", "__empty,1", "Return", "BackSpace"];

    let rows = vec![row0, row1, row2, row3];

    let mut container = Container {keys : Vec::new()};

    create_keyboard_with_rects(&rows, &mut container);

    //create_keyboard_with_table_buttons(&rows);

    unsafe {
        elm::run();
    }
}

fn create_keyboard_with_rects(rows : &Vec<Vec<&str>>, container : &mut Container)
{
    let k = unsafe {elm::keyboard_new()};
    create_keys(k, rows, container);
    unsafe {elm::keyboard_bg_add(
            pressed,
            mem::transmute(container))
    };
}

fn create_keys(k: *mut elm::Keyboard, rows : &Vec<Vec<&str>>, container : &mut Container)
{
    let width = 2;

    let mut max_col = 0;
    for r in rows.iter() {
        max_col = cmp::max(max_col, get_real_len(r));
    }


    let mut row = 0;
    for r in rows.iter() {

        let mut col = 0;
        let first_pos = (max_col*width - width*get_real_len(r))/2;

        let mut col_keys = Vec::new();

        for c in (*r).iter() {

            let pos = first_pos + col*width;
            let s : Vec<&str> = c.split(',').collect();

            let w = if s.len() > 1 {
                s[1].parse::<usize>().unwrap()
            }
            else {
                1
            };

            if c.starts_with("__empty") {
                //do nothing
            }
            else if c.starts_with("__close") {
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
                    let r = elm::keyboard_rect_add(
                        k,
                        cstring_new(s[0]),
                        pos as i32,
                        row,
                        (width*w) as i32,
                        1);

                    let key = Key {
                        eo : r,
                        name : String::from(s[0]), 
                        kind : KeyKind::Normal(String::from(s[0])),
                        down : false
                    };
                    col_keys.push(key);
                }
            }
            col = col +w;
        }
        row = row + 1;
        container.keys.push(col_keys);
    }

}

fn create_keyboard_with_table_buttons(rows : &Vec<Vec<&str>>)
{
    let k = unsafe {elm::keyboard_new()};
    //create_keys(k, rows);
}

fn cstring_new(s : &str) -> *const c_char
{
    //let to_print = &b"Hello, world!"[..];
    CString::new(s).unwrap().as_ptr()
}

extern fn close(data : *mut c_void) {
    unsafe { elm::reduce() };
}

extern fn pressed(data : *mut c_void, x : c_int, y : c_int) {
    //println!("pressed {}, {}", x, y);
    let con : &mut Container = unsafe { mem::transmute(data) };

    for c in con.keys.iter_mut() {
        for k in c.iter_mut() {
            if unsafe {elm::is_point_inside(k.eo, x, y)} {
                k.down = true;
                //println!("found object {} ", k.name);
                unsafe { elm::evas_object_color_set(k.eo, 150, 150, 150, 255)};
                match k.kind {
                    KeyKind::Normal(ref s) => {
                        unsafe {
                            elm::ecore_x_test_fake_key_press(cstring_new(s));
                        }
                        
                    },
                    _ => {}
                }
                break;
            }
        }
    }

}


fn get_button_count(v : &Vec<&str>) -> usize
{
    let l = v.iter().filter(|s| !s.starts_with("__empty")).count();
    l
}

fn get_real_len(v : &Vec<&str>) -> usize
{
    let mut l = 0;
    for c in v.iter() {
        let s : Vec<&str> = c.split(',').collect();
        let w = if s.len() > 1 {
            s[1].parse::<usize>().unwrap()
        }
        else {
            1
        };
        l += w;
    }

    l
}


