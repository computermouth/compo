#![no_std]

extern crate alloc;
use alloc::string::String;
use alloc::string::ToString;

#[global_allocator]
static A: dlmalloc::GlobalDlmalloc = dlmalloc::GlobalDlmalloc;

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

mod bindings {
    wit_bindgen_rust_macro::generate!({
        path: "./wit",
        world: "rcmp-world",
    });
}

struct Component;

impl bindings::Guest for Component {
    fn greet() -> String {
        "Howdy".to_string()
    }
}

bindings::export!(Component with_types_in bindings);
