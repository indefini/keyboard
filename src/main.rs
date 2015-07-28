#![feature(libc)]
extern crate libc;
extern crate elementary_sys as elm;

use std::cmp;
use std::ffi::CString;
//use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use libc::{c_char};

fn main() {
    unsafe { elm::init() };

    let rows = [ "qwertyuiop", "asdfghjkl", "zxcvbnm" ];
    let width = 10;

    let mut max_col = 0;
    for r in rows.iter() {
        max_col = cmp::max(max_col, r.len());
    }

    let k = unsafe {elm::keyboard_new()};

    let mut row = 0;
    for r in rows.iter() {
        let mut col = 0;
        let first_pos = (max_col*width - width*r.len())/2;
        for c in (*r).chars() {
            let pos = first_pos + col*width;
            unsafe { 
                elm::keyboard_add(k, cstring_new(&c.to_string()), pos as i32, row, width as i32, 1);
            }
            col = col +1;
        }
        row = row + 1;
    }

    unsafe {
        elm::run();
    }
}

fn cstring_new(s : &str) -> *const c_char
{
    //let to_print = &b"Hello, world!"[..];
    CString::new(s).unwrap().as_ptr()
}

