# thesis-ba

### Abstract

This thesis aims to analyze new programming languages in the context of high-performance computing. In contrast to many other evaluations the focus is not only on **performance** but also on developer **productivity metrics**. The two new languages Go and Rust are compared with C as it is one of the two commonly used languages in HPC next to Fortran.

The base for the evaluation is a shortest path calculation based on real world geographical data which is parallelized for shared memory concurrency. An implementation of this concept was written in all three languages to compare multiple productivity and performance metrics like **execution time**, **tooling support**, **memory consumption** and **development time** across different phases.

Although the results are not comprehensive enough to invalidate C as a leading language in HPC they clearly show that both Rust and Go offer tremendous **productivity gain** compared to C with **similar performance**. Additional work is required to further validate these results as future reseach topics are listed at the end of the thesis.

### Building instructions

The thesis is written in LaTex so you will need a working Tex installation (usually TexLive on *nix systems) to compile it successfully. For convenience there is a shell script in the tex folder `build.sh` which will compile everything (including sources) and output a single file `thesis.pdf`.

TL;DR

    $ git clone http://github.com/mrfloya/thesis-ba.git
    $ cd thesis-ba/tex
    $ ./build.sh
    $ <
    pdfviewer> thesis.pdf

### LICENSE

All code written is licensed under the MIT license unless stated otherwise.
