# SDDDST
Simple Discrete Dislocation Dynamics Simulation Toolkit

## Short description
This toolkit contains tools to simulate 2D single slip edge dislocation systems under periodic boundary conditions. The integrator is based on an implicit numerical scheme which makes it possible to keep the 
O(*N*<sup>2</sup>) complexity which arise from the pair interactions while no dislocation annihilation is required and the runtime is greatly decreased.

SDDDST is highly modular, it can be easily modified and extended based on the use case where it is needed.

The detailed publication of the numerical scheme and the implementation with the achived results will be soon available.

### Tools
The toolkit contains the following tools
1. The **simulation program** *2D_DDD_simulation* that evols the dislocation configuration under the prescribed external stress.
2. [**Dislocation system generator** *init_config_gen*](https://github.com/danieltuzes/sdddst/init_config_gen) to create the initial configuration.
3. **Evaulation programs** to do perform analysis on the simulations obatined.

The rest of this file belongs to the simulation program 2D_DDD_simulation.

## Build & run
This project uses several external libraries, such as **umfpack** and **boost** and furthermore, to keep the code simple, some additional include libraries must be set up for your compiler. A convenient Linux-gcc-cmake built procedure (scenario A) is provided along with a Windows-VS-vcpkg built procedure (scenario B).

### Dependencies: scenario A
This is the Linux-gcc-cmake case. To be able to build the simulator, you will need the following dependencies on your computer:

* g++ or clang
* cmake (at least version 3.8)
* make
* pyhton3 with python3-dev
  * this is only needed if python interface is required in `CMakeLists.txt`
  * this can be found at line 23: `option(BUILD_PYTHON_BINDINGS "Build python interface package" OFF)`
  * a system-wide installation is required by default, otherwise, cmake will not find the libs
  * using a local (user) version of python is also possible: define paths for cmake with the switch `-DPYTHON_LIBRARY=~/.local/lib/libpython` and `-DPYTHON_INCLUDE_DIR=~/.local/include/python3.7m` if you installed python3-dev into your `$HOME/.local` directory
* umfpack from suitesparse
* boost (the program options, and python libraries, if python is required)
* FFTW

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
You can safely **delete** the files corresponding to scenario B:
* 2D_DDD_simulation.vcxproj
* 2D_DDD_simulation.vcxproj.filters
* 2D_DDD_simulation.vcxproj.user
* SDDST.sln

### Dependencies: scenario B
This is the Windows-VS-vcpkg case. The project can be compiled for x64 compatible machines only [due to umfpacks's openblas dependency](https://github.com/microsoft/vcpkg/issues/2095), which is available only for x64. This case comes with no python interface yet. Follow these instructions to be able build this project.
1. Install [vcpkg](https://github.com/microsoft/vcpkg)
   1. Open a PowerShell (abbreviated as PS) and first create the directory where you would like to place it. Let's say, `C:\local`, so in PS, execute `mkdir C:\local`. Move there by executing `cd C:\local`.
   2. Download the vcpkg package with git or by downloading it with a browser from [its website](https://github.com/Microsoft/vcpkg).
	3. Extract the files with their parent folder vcpkg-master from the compressed file vcpkg-master.zip. Move it to `C:\local`. Navigate your PS there by `cd vcpkg-master`.
   3. Install the program by executing `.\bootstrap-vcpkg.bat`. The installar may ask for admin privileges.
   4. To use the installed packages automatically, execute `.\vcpkg integrate install`.
2. Install the required dependencies with *vcpkg* by executing
      
      1. `.\vcpkg install boost-program-options:x64-windows` for boost program options, and
      2. `.\vcpkg install suitesparse:x64-windows`, this installs the umfpack.
      
	These steps may take at least around 10 minutes.
3. Use the solution file or set up a new one for the project. Due to an [issue](https://github.com/microsoft/vcpkg/issues/3417), your compiler probably need be informed about umpack's directory.
	1. With the provided solution the project files, it has been already added by pointing to `%VCPKG_ROOT%`. All you need to do is to add a system variable called VCPKG_ROOT with value pointing to the root directory of vcpkg. In our previous example, it was `C:\local\vcpkg-master`.
	2. If you do not want to or cannot create the system variable, or want to start your solution and project files from zero, you can manually add it to your project settings. Open project 2D_DDD_simulation in the solution SDDST, then open your project properties under Project, \<Projectname\> properties. Go to VC++ Directories and in the Include Directories add `C:\local\vcpkg-master\packages\suitesparse_x64-windows\include\suitesparse`.
	
You can safely **delete** building files and folders for scenario A:
* cmake/
* CMakeLists.txt
* sdddstCMakeConfig.h.in
### Configuration files
For the available parameters, check out the help!

```bash
./sdddst --help
```

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
* cutoff (used in the implicit scheme)
* order parameter
* value of the external stress
* computation time between the last two successful steps
* accumulated strain
* average *v*<sup>2</sup>
* energy of the system

### Cutoff multiplier
A cutoff parameter is needed for this implicit method. The meaning of the parameter is that if it is infinite the calculation goes like an implicit method was used, but if it is zero, it is like an explicit method. The multiplier multiplied with one on square root N (where N is the number of the dislocations) results in the actual cutoff.

## Python interface
A minimalistic Python interface is also available which makes it possible to run simulations directly from python. A simple description about how to run a simulation can be found below:
### Compile the python module
Just like before, cmake is responsible for the compilation process. The dependencies:

* Just like above
* Boost.Python (already covered by boost)
* Python3 and the developer libraries as well

After the dependencies are in place, the following should be issued in the previously created `build` folder:

```bash
cmake .. -DBUILD_PYTHON_BINDINGS On
make
```

As a result `PySdddstCore.so` named shared library going to be created in the build folder. This should be placed into your path and after that it can be imported into python by issueing the following command (a python package can be created with it as well):

```python
import PySdddstCore as psc
```

### Example
First we need to import the library into python:

```python
import PySdddstCore as psc
```

The next step is to prepare the simulation data. For that we need to use a `SimulationDataObject`. Let's say we are going to simulate the motion of some dislocations, and their data is in `dislocation.dconf` in the current working directory:

```python
simulation_data = psc.SimulationDataObject("dislocation.dconf", "")
```

(Caution: If the paths do not point to a valid file, the program will crash. This will be fixed later on.)

The properties of the simulation will be available through this object during the whole simulation. Set up the simulation parameters:

```python
simulation_data().cutoff_multiplier = 0.5
simulation_data().precision = 1e-6
simulation_data().time_limit = 100
simulation_data().time_limited = True
simulation_data().calculate_strain_during_simulation = True
simulation_data().final_dislocation_configuration_path = "result.dconf"
simulation_data().update_cutoff()
```

The actual values can be obtained by simply invoking the parameters:

```python
simulation_data().cutoff_multiplier
simulation_data().precision
simulation_data().time_limit
simulation_data().time_limited
simulation_data().calculate_strain_during_simulation
simulation_data().final_dislocation_configuration_path
```

Two important thing is still missing. The external stress field (even if it is zero) and the dislocation field model has to be specified. First we specify a zero external stress field:

```python
external_stress = psc.StressProtocolObject()
```

This is not a valid object right now. We should initialize it first:

```python
external_stress.init()
```

Validity can be checked by:

```python
external_stress.valid()
```

We need to provide this object to the simulation as well by:

```python
simulation_data().external_stress = external_stress
```

After that the `StressProtocolObject` becomes invalid again.

The same should be applied in case of the dislocation model:

```python
field = psc.AnalyticFieldObject()
field.init()
simulation_data().tau = field
```

After we are ready, the simulation can be created and run by:

```python
simulation = psc.Simulation.create(simulation_data)
simulation.run()
```
