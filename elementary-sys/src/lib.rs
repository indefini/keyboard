#![feature(libc)]
extern crate libc;

use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use std::mem;

pub type RustCb = extern fn(data : *mut c_void);

#[repr(C)]
pub struct Keyboard;

extern "C" {
    pub fn init();
    pub fn run();
    pub fn kexit();
    pub fn reduce();
    pub fn keyboard_new() -> *const Keyboard;
    pub fn keyboard_add(
        keyboard : *const Keyboard,
        keyname : *const c_char,
        col : c_int,
        row : c_int,
        width : c_int,
        height : c_int);

    pub fn keyboard_fn_add(
        keyboard : *const Keyboard,
        name : *const c_char,
        cb : RustCb,
        cb_data : *const c_void,
        col : c_int,
        row : c_int,
        width : c_int,
        height : c_int);
}


