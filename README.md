# SDDDST
Simple Discrete Dislocation Dynamics Simulation Toolkit

## Short description
This toolkit makes it possible to easily simulate 2D edge dislocation systems where the slip planes are parallel and periodic boundary conditions are applied. The integrator is based on a semi-implicit numerical scheme which makes it possible to keep the 
O(N<sup>2</sup>) complexity which arise from the pair interactions while no dislocation annihilation is required and the runtime is greatly decreased.

SDDDST is highly modular, it can be easily modified and extended based on the use case where it is needed.

The detailed publication of the numerical scheme and the implementation with the achived results will be soon available.

## Build & run
### Dependencies
In order to be able to build the simulator, you will need the following dependencies on your computer:

* g++ or clang
* cmake
* make
* boost
* umfpack from suitesparse
* gsl

The build is fairly simple from the root directory of the source:

```bash
mkdir build
cd build
cmake ..
make
```

The resulting binary which can be used to run the simulations:

```bash
build/src/sdddst
```

For the available parameters, check out the help!

```bash
./sdddst --help
```

### Configuration files
To be able to run a simulation, data has to be provided in plain text format. The slip planes of the dislocations are parallel with the x axis and the simulation area is between [-0.5, 0.5] in both directions. Based on that an example configuration file which contains dislocation data:

```
-0.1 0.3 1
0.4 0.2 1
-0.2 -0.3 -1
-0.3 0.3 1
...
```

Each line represent a dislocation: in the first column the x coordinates are present, while in the second one the y coordinates can be found. The last column can be either 1 or -1 based on in which direction the dislocation's Burgers vector point.
Point defects represented in files are the same in structure, but without the last column. Point defects are fixed in place during the simulations.

### Field of a dislocation
To be able to simulate dislocation interactions, a field need to be defined. These should be periodic and should reflect the size of the simulation cell. The current default one uses a binary datablob which contains precalculated data. The binary (periodic_stress_xy_1024x1024_bin.dat) need to be in the present working directory, or the path has to be defined with the corresponding option. Do not include the name of the binary at the end of the path!

### External stress
The default protocol for external stress gives 0 for every timestep, but other protocols can be choosen as well, see the help.

### Structure of the log file
If a log file is requested, a file will be continously updated during the simulation, but be aware that it is only written into the file when the buffer needs to be emptied. Each line represent a successful step in the simulation and in order the meaning of the columns:

* simulation time
* number of successful steps
* number of failed steps
* worst error ratio squared
* average speed of the dislocations
* cutoff (used in the semi implicit scheme)
* order parameter
* value of the external stress
* computation time between the last two successful steps
* accumulated strain
* average v<sup>2</sup>
* energy of the system

### Cutoff multiplier
A cutoff parameter is needed for this semi-implicit method. The meaning of the parameter is that if it is infinite the calculation goes like an implicit method was used, but if it is zero, it is like an explicit method. The multiplier multiplied with one on square root N (where N is the number of the dislocations) results in the actual cutoff.
