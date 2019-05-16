// init_config_gen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <boost/program_options.hpp> // to read in program call arguments
#include <boost/program_options/options_description.hpp> // to add descriptions of the program call arguments
#include <boost/random/mersenne_twister.hpp> // random number generator
#include <boost/random/uniform_real_distribution.hpp> // uniform distribution generator

#include <iostream>
#include <fstream>

namespace bpo = boost::program_options;
namespace br = boost::random;
int main(int argc, char** argv)
{
#pragma region read in variables
    std::cout << 1 + 1;
    bpo::options_description requiredOptions("Required options"); // must be set from command line or file
    bpo::options_description optionalOptions("Seed options"); // must be set from command line or file

    requiredOptions.add_options()
        ("N", bpo::value<int>(), "The number of dislocations to generate. Must be even, because N/2 number of positive and negative dislocations will be created.");

    optionalOptions.add_options()
        ("seed-start", bpo::value<int>()->default_value(1000), "An integer used as an initial seed value for the random number generator.")
        ("seed-end", bpo::value<int>(), "An integer used as the last seed value for the random number generator, seed-end > seed_start must hold. If set, seed-end - seed-start number of initial configurations will be created.")
        ;

    bpo::options_description options; // the superior container of the options

    options.add(requiredOptions).add(optionalOptions).add_options()
        ("help", "show this help")
        ("hide-copyright,c", "hides the copyright notice from the standard output");

    bpo::variables_map vm; // the storage for the variables

    try {
        bpo::store(bpo::parse_command_line(argc, argv, options), vm, false);
    }
    catch (bpo::error & e)
    {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }

    if (!vm.count("hide-copyright"))
    {
        std::cout << "init_config_gen from the SDDDST - Simple Discrete Dislocation Dynamics Toolkit\n"
        "Copyright (C) 2015-2019 Gabor Peterffy <peterffy95@gmail.com>, Daniel Tuzes <tuzes@metal.elte.hu> and their friends.\n"
        "This program comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it under certain conditions; see the license for details\n";
    }
#pragma endregion

#pragma region process variables
    if (vm.count("help")) // if the user is curious 
    {
        std::cout << options << std::endl;
        exit(0);
    }
    else // real process is needed
    {
        // N
        if (0 == vm.count("N")) // to test if N is present
        {
            std::cerr << "N is missing! Program terminates.\n";
            exit(-1);
        }
        if (vm["N"].as<int>() % 2 != 0)  // to test if N is even
        {
            std::cerr << "N must be even. Program terminates." << std::endl;
            exit(-1);
        }

        // seed-end >? seed-start
        if (vm.count("seed-end") != 0 && vm["seed-end"].as<int>() <= vm["seed-start"].as<int>())
        {
            std::cerr << "seed-end > seed-start must hold. Program terminates." << std::endl;
            exit(-1);
        }
    }
#pragma endregion

#pragma region generate and write out configuration
    int seed_val = vm["seed-start"].as<int>();
    int seed_end = (vm.count("seed-end") == 0 ? seed_val : vm["seed-end"].as<int>());
    for (;seed_val <= seed_end; ++seed_val)
    {
        std::string ofname = "dislocation-configurations/init_config_" +std::to_string(seed_val) + ".txt";
        std::ofstream ofile(ofname);
        if (!ofile)
        {
            std::cerr << "Cannot write " << ofname << ". Program terminates." << std::endl;
            exit(-1);
        }
        std::cout << "Generating configuration with seed value " << seed_val << "." << std::endl;
        ofile.precision(std::numeric_limits<double>::max_digits10); // print out every digits

        br::mt19937 generator(seed_val);
        br::uniform_real_distribution<> distribution(-0.5, 0.5);
        for (int n = 0; n < vm["N"].as<int>(); ++n)
            ofile << distribution(generator)
            << "\t" << distribution(generator)
            << "\t" << (n % 2) * 2 - 1
            << "\n";
    }
#pragma endregion


    std::cout << "Done.\n";
    return 0;
}