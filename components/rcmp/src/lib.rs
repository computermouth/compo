mod bindings {
    wit_bindgen_rust_macro::generate!({
        path: "./wit",
        world: "rcmp-world",
        std_feature,
        generate_all,
    });
}

struct Component;

impl bindings::Guest for Component {
    fn greet() -> String {
        "Howdy".to_string()
    }
}

bindings::export!(Component with_types_in bindings);
