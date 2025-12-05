# Z3-Noodler-Mocha

Welcome to the homepage of Z3-Noodler-Mocha!

Z3-Noodler-Mocha is an SMT solver for string constraint which is based on [Z3-Noodler](https://github.com/VeriFIT/z3-noodler).

### Building
1) Build the [Mata](https://github.com/VeriFIT/mata/) library
    ```shell
    git clone 'https://github.com/VeriFIT/mata.git'
    cd mata
    make release
    sudo make install
    ```
2) Build Z3-Noodler-Mocha
    ```shell
    git clone 'https://github.com/Bazoka13/Z3-Noodler-Mocha.git'
    mkdir Z3-Noodler-Mocha/build
    cd Z3-Noodler-Mocha/build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    ```

### Running Z3-Noodler-Mocha
To run Z3-Noodler-Mocha, use:
    ```shell
    ./build/z3 xxx.smt2
    ```

### Acknowledgment
We would like to thank the developers of the Z3-Noodler solver and all the authors who contributed to the modifications that have been integrated. Their solver is the foundation of our improvement.