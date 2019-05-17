# SDDDST::init_config_gen
The initial configuration generator tool for Simple Discrete Dislocation Dynamics Simulation Toolkit

## Short description
With this tool one can create dislocation configurations to use as an input of the simulation program 2D_DDD_simulation. It uses a platform independent implementation provided by [`boost.random`](https://www.boost.org/doc/libs/1_70_0/doc/html/boost_random.html).

## Dependencies
There is 1 new dependency: [`boost.random`](https://www.boost.org/doc/libs/1_70_0/doc/html/boost_random.html). To compile this project you need boost's program options and random libraries. The first is already a requirement for the project *2D_DDD_simulation*.

## Usage
Type `init_config_gen --help` to get the help. The program accepts the following arguments:
* `--N arg` or `-N arg`: **mandatory** argument. The number of dislocations to simulate. Must be an even number, because the same amount of positive and negative dislocations must be found in the system.
* `--seed-start arg` or `-S arg`: optional argument with default value `1000`. It is an integer used to populate the seed value of the random number generator engine.
* `--seed-end arg` or `-E arg`: optional argument. If set, must be larger than `seed_start` and the program will create configuration files starting from seed value `seed-start`, increasing it one by one till it reaches `seed-end` (closed interval).
* `--unsorted` or `-U`: optional switch. If set, dislocations will not printed out in order starting with positive Burger's vector and highest value in y, but with alternating Burger's vector and uncorrelated x and y coordinates.
* `--bare` or `-B`: optional swtich. If set, the configuration file name will not conatin the number of dislocations.

## Output
The program creates the initial conditions in the folder `dislocation-configurations` relative to the binary, with filenames `ic_SEEDVALUE_N.txt`. The file format is as described in [SDSST](https://github.com/danieltuzes/sdddst). The output is a simple text file in the format
```
pos_1_x pos_1_y Burgers_vector_1
pos_2_x pos_2_y Burgers_vector_2
...
pos_N_x pos_N_y Burgers_vector_N

```
If the default value of parameter `sorted` is kept `true`, then dislocations with positive Burger's vector come first with decreasing y coordinate. If `false` is set, dislocations comes with alternating Burger's vector and random coordinate values. Please keep in mind that line endings (`CR` and `LF`) will depend on your operating system.