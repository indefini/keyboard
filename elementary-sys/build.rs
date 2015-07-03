extern crate pkg_config;
extern crate gcc;

use std::env;
use std::path::PathBuf;

fn main() {
    if let Ok(info) = pkg_config::find_library("elementary") {
        build_elementary_key(&info.include_paths);
        return;
    }
}

fn build_elementary_key(include_paths: &[PathBuf]) {
    let mut config = gcc::Config::new();

    for path in include_paths {
        config.include(path);
    }

    config.file("src/elementary.c")
        .compile("libelementary_key.a");
}

