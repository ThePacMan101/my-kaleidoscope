# Kaleidoscope

Kaleidoscope is a toy functional programming language used in the tutorial for [LLVM](https://llvm.org/). The tutorial is explicitly not using best practices in software engineering principles, so my idea was to implement a version that does so. While at it, I also use this opportunity to practice building C++ systems with Cmake, which I'm not that familiar with at the time of writing this.

Usage of LLMs for code generation was kept to a mininum in this repo, as **the whole idea** is to learn LLVM.

Feel free to check the [official Kaleidoscope tutorial for LLVM](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend). 

## Build and Run:

You will need to install LLVM either by compiling locally or through a package manager. 

Once you do so, you can use make to build the project:

```shell
> make clean
```

And run using:

```shell
> make run
```
___
Once inside the REPL, just type a function definition or any expression to evaluate it and printhe generated and optimized IR:
```llvm
ready> 1+1;

define double @__anon_expr() {
entry:
  ret double 2.000000e+00
}
ready>
> 2.000000
```