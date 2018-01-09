# Boolean Satisifiability Problem Compiler for D-Wave Quantum Annealer
Author: Juexiao Su
Email: juexiao.su@gmail.com

# Introduction
This is a C++ software that compiles SAT problem on to D-Wave quantum annealer. D-Wave has successfully built the first commericalized Quantum Annealer, however, one can hardly harness the computational power due to the lack of problem compiler. This project servers as an example program of D-Wave Boolean satisfiability problem compiler, which compiles a CNF format to D-Wave configuration file.

# Installation & Usage
## Pre-requisite library:
    Boost library 1.53.0
    Tcl/Tk 8.4
    Python
    [ABC synthesis tool](http://people.eecs.berkeley.edu/~alanmi/abc/)
## Install
    1) git clone to your own space
    2) cd <path>/sat_compil
    3) ./build/makes
    4) make
    5) source qsat_env.sh or mannually set QSAT_HOME to <path>/sat_compil and add <path>/sat_compil/bin to PATH env
## Run Regression
    1) cd $QSAT_HOME/regression/reg_infra/
    2) ./move_and_copy.py
    3) ./create_tcl.py
    4) ./run_test.py <log file name> <thread num>
    5) regression result is stored at $QSAT_HOME/regression/testcases
## File Type
1) [Blif File](http://www.cs.columbia.edu/~cs6861/sis/blif/index.html), we use blif file to represent the input netlist. A Boolean satisfiability problem can be seen as a netlist with single output. In our internal netlist data structure, we use the output wire name to name the driver gate, and wire is named by the driver pin name.
2) Placement File: *.place, indicating the placement of each gate.
3) Routing File: *.route, inidicating how the qubit chain are routed.
3) Dwave Configuration: *.dwave, using D-Wave format to configure the qubit and coupler weight on the real device.
4) Tcl File: *.tcl, controlling the tool flow.
  
# Open the hood
The implementation is based on two papers:
1) [A quantum annealing approach for boolean satisfiability problem](https://dl.acm.org/citation.cfm?id=2897973)
2) [Fast embedding of constrained satisfaction problem to quantum annealer with minimizing chain lengt](http://ieeexplore.ieee.org/document/8060449/)

# Example Configuration
1) ![](https://github.com/QuantumSAT/sat_compil/blob/master/example/and-gate2.PNG)
2) ![](https://github.com/QuantumSAT/sat_compil/blob/master/example/and-gate.PNG)
3) ![](https://github.com/QuantumSAT/sat_compil/blob/master/example/routed.PNG)

  



TODO:
1) Replacement of front-end (Synthesis) flow
2) Redevelope logic Netlist datastructure
