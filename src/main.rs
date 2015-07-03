extern crate elementary_sys as elm;
fn main() {
    println!("my keyboard");
    unsafe {
        elm::init();
        elm::create_simple_window();
    }

    create_key("a");
    create_key("b");

    unsafe {
        elm::run();
    }
}

fn create_key(name : &str)
{
    //TODO
    // add rect with the name
    println!("create key {}", name);
}

