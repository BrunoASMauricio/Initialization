# Initialization

This exercise provides a basic set of primitives to ensure constructors only run
after other constructors they depend on had the chance to run.

In heterogeneous code bases, the "main" application has no obligation to know
 what initialization needs to be done and in what order.

In order to avoid having a centralized file littered with `ifdefs` containing
the ordered calls for each handler, the primitives presented here exchange a bit
of initial processing (very low overhead) to provide decentralized
initialization.

So far the only "con" of this method is that all relevant projects must have
this code as an extra dependency

## Usage

### Independent handlers

An independent handler has no dependency and will be one of the first to run

```C
REGISTER_INDEPENDENT_CONSTRUCTOR(MyConstructor1);
```


### Dependent handlers

A dependent handler will only run after its' dependencies run all the way.

```C
REGISTER_DEPENDENT_CONSTRUCTOR(MyConstructor3, MyConstructor1, MyConstructor2);
```

### Running all handlers

```C
int main(void) {
    RunInitializationFunctions();

    // ... initialization complete, do whatever now ...

    return 0;
}
```

## Concept

This exercise makes use of constructors that run without any specific order.
Each constructor simply registers into a global structure the handlers and their
dependencies.

A dependency is simply a handler that must run first. This function/address
should be present in the header file, possibly hidden behind some macro

When a call to `RunInitializationFunctions` is performed, the handlers are
organized and run in the appropriate order, taking their dependencies into
account.

There are two assumptions made here:

1. The handlers belong to the same memory address space and therefore their
values are unique.
If this assumption cannot be maintained, then the initialization responsibility
shifs either into a network protocol, or some operating system primitive

2. A handler has access to the address of the handlers it depends on.
Often this is achieved by using the respective header file

## RoadMap / Project Status

This exercises' main core is complete!

Valgrind shows no memory leaks if everything is properly shutdown!
