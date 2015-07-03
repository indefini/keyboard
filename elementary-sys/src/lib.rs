#![feature(libc)]
extern crate libc;

use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use std::mem;

extern "C" {
    pub fn create_simple_window();
    pub fn init();
    pub fn run();
}

