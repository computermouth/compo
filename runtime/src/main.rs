use wasmtime::component::*;
use wasmtime::{Engine, Store};

mod bindings {
    wasmtime::component::bindgen!({
        world: "rcmp-world",
        path: "../components/rcmp/wit"
    });
}

struct MyState {
    name: String,
}

// Imports into the world, like the `name` import for this world, are
// satisfied through traits.
impl bindings::RcmpWorldImports for MyState {
    fn name(&mut self) -> String {
        self.name.clone()
    }
}

fn main() -> wasmtime::Result<()> {
    // Compile the `Component` that is being run for the application.
    let engine = Engine::default();
    let component = Component::from_file(&engine, "target/wasm32-wasip1/debug/rcmp.wasm")?;

    // Instantiation of bindings always happens through a `Linker`.
    // Configuration of the linker is done through a generated `add_to_linker`
    // method on the bindings structure.
    //
    // Note that the closure provided here is a projection from `T` in
    // `Store<T>` to `&mut U` where `U` implements the `HelloWorldImports`
    // trait. In this case the `T`, `MyState`, is stored directly in the
    // structure so no projection is necessary here.
    let mut linker = Linker::new(&engine);
    bindings::RcmpWorld::add_to_linker(&mut linker, |state: &mut MyState| state)?;

    // As with the core wasm API of Wasmtime instantiation occurs within a
    // `Store`. The bindings structure contains an `instantiate` method which
    // takes the store, component, and linker. This returns the `bindings`
    // structure which is an instance of `HelloWorld` and supports typed access
    // to the exports of the component.
    let mut store = Store::new(
        &engine,
        MyState {
            name: "Ben".to_string(),
        },
    );
    let rcmp = bindings::RcmpWorld::instantiate(&mut store, &component, &linker)?;

    // Here our `greet` function doesn't take any parameters for the component,
    // but in the Wasmtime embedding API the first argument is always a `Store`.
    rcmp.call_greet(&mut store)?;
    Ok(())
}
