#![feature(libc)]
extern crate libc;

extern crate elementary_sys as elm;

use std::ffi::CString;
//use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use libc::{c_char};

fn main() {
    let rows = [ "qwertyuiop", "asdfghjkl", "zxcvbnm" ];

    unsafe {
        elm::init();
        //elm::create_simple_window();
        let k = elm::keyboard_new();
        let mut row = 0;
        for r in rows.iter() {
            let mut col = 0;
            for c in (*r).chars() {
                elm::keyboard_add(k, cstring_new(&c.to_string()), col, row);
                col = col +1;
            }
            row = row + 1;
        }
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

