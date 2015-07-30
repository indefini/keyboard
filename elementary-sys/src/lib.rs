extern crate libc;

use libc::{c_void, c_int, c_char};//, c_ulong, c_long, c_uint, c_uchar, size_t};

pub type RustCb = extern fn(data : *mut c_void);
pub type PressedCb = extern fn(data : *mut c_void, device : c_int, x : c_int, y : c_int);

#[repr(C)]
pub struct Keyboard;
#[repr(C)]
pub struct Evas_Object;


extern "C" {
    pub fn init();
    pub fn run();
    pub fn kexit();
    pub fn reduce();

    pub fn keyboard_new() -> *mut Keyboard;

    pub fn keyboard_add(
        keyboard : *const Keyboard,
        keyname : *const c_char,
        col : c_int,
        row : c_int,
        width : c_int,
        height : c_int);

    pub fn keyboard_rect_add(
        keyboard : *const Keyboard,
        keyname : *const c_char,
        col : c_int,
        row : c_int,
        width : c_int,
        height : c_int) -> *mut Evas_Object;

    pub fn keyboard_fn_add(
        keyboard : *const Keyboard,
        name : *const c_char,
        cb : RustCb,
        cb_data : *const c_void,
        col : c_int,
        row : c_int,
        width : c_int,
        height : c_int);

    pub fn keyboard_bg_add(
        cb : PressedCb,
        cb_up : PressedCb,
        cb_move : PressedCb,
        cb_data : *const c_void);

    pub fn is_point_inside(o : *const Evas_Object, x : c_int, y : c_int) -> bool;
    //pub fn fake_key(keyname : *const c_char);
    pub fn ecore_x_test_fake_key_press(keyname : *const c_char);
    pub fn evas_object_color_set(o : *mut Evas_Object, r : c_int, g : c_int, b : c_int, a : c_int); 
}


