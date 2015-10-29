extern crate libc;

use libc::{c_void, c_int, c_char, c_float};//, c_ulong, c_long, c_uint, c_uchar, size_t};

pub type AnimCb = extern fn(data : *mut c_void) -> bool;
pub type RustCb = extern fn(data : *mut c_void);
pub type PressedCb = extern fn(data : *mut c_void, device : c_int, x : c_int, y : c_int);

#[repr(C)]
pub struct Keyboard;
#[repr(C)]
pub struct Evas_Object;
#[repr(C)]
pub struct Ecore_Animator;


extern "C" {
    pub fn init();
    pub fn run();
    pub fn kexit();
    pub fn reduce();

    pub fn window_new() -> *const Evas_Object;
    pub fn keyboard_new(
        win : *const Evas_Object,
        px : c_int,
        py : c_int,
        kx : c_int,
        ky : c_int,
        ksx : c_int,
        ksy : c_int,
        ) -> *mut Keyboard;

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
        row : c_int,
        width : c_float) -> *mut Evas_Object;

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

    pub fn keyboard_popup_show(
        keyboard : *const Keyboard,
        eo : *const Evas_Object,
        name : *const c_char);

    pub fn keyboard_popup_hide(
        keyboard : *const Keyboard);

    pub fn is_point_inside(o : *const Evas_Object, x : c_int, y : c_int) -> bool;
    //pub fn fake_key(keyname : *const c_char);
    pub fn ecore_x_test_fake_key_press(keyname : *const c_char);
    pub fn ecore_x_test_fake_key_down(keyname : *const c_char);
    pub fn ecore_x_test_fake_key_up(keyname : *const c_char);
    pub fn evas_object_color_set(o : *mut Evas_Object, r : c_int, g : c_int, b : c_int, a : c_int);
    fn elm_win_screen_dpi_get(win : *const Evas_Object, dpix : *mut c_int, dpiy : *mut c_int);
    fn elm_win_screen_size_get(win : *const Evas_Object, x : *mut c_int, y : *mut c_int, w : *mut c_int, h : *mut c_int);

    pub fn ecore_animator_add(cb : AnimCb, data :*const c_void) -> *const Ecore_Animator;
    pub fn ecore_animator_del(animator : *const Ecore_Animator);
}

pub fn get_dpi_size(win : *const Evas_Object) -> (usize, usize, usize, usize)
{
    let mut dpix =0;
    let mut dpiy =0;
    unsafe { elm_win_screen_dpi_get(win, &mut dpix, &mut dpiy); }

    let mut x =0;
    let mut y =0;
    let mut w =0;
    let mut h =0;
    unsafe { elm_win_screen_size_get(win, &mut x, &mut y, &mut w, &mut h); }

    (dpix as usize, dpiy as usize, w as usize, h as usize)
}


