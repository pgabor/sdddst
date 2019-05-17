
// init_config_gen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/*
 * SDDDST Simple Discrete Dislocation Dynamics Toolkit
 * Copyright (C) 2015-2019 Gábor Péterffy <peterffy95@gmail.com>, Dániel Tüzes <tuzes@metal.elte.hu> and their friends.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */


#include <boost/program_options.hpp> // to read in program call arguments
#include <boost/program_options/options_description.hpp> // to add descriptions of the program call arguments
#include <boost/random/mersenne_twister.hpp> // random number generator
#include <boost/random/uniform_real_distribution.hpp> // uniform distribution generator

#include <iostream>
#include <fstream>
#include <algorithm>
#include <tuple>

namespace bpo = boost::program_options;
namespace br = boost::random;
int main(int argc, char** argv)
{
#pragma region read in variables
    std::cout << 1 + 1;
    bpo::options_description requiredOptions("Required options"); // must be set from command line or file
    bpo::options_description optionalOptions("Optional options"); // must be set from command line or file

    requiredOptions.add_options()
        ("N,N", bpo::value<int>(), "The number of dislocations to generate. Must be even, because N/2 number of positive and negative dislocations will be created.");

    optionalOptions.add_options()
        ("seed-start,S", bpo::value<int>()->default_value(1000), "An integer used as an initial seed value for the random number generator.")
        ("seed-end,E", bpo::value<int>(), "An integer used as the last seed value for the random number generator, seed-end > seed_start must hold. If set, seed-end - seed-start number of initial configurations will be created.")
        ("unsorted,U", "If set, dislocations will not printed out in order starting with positive Burger's vector and highest value in y, but with alternating Burger's vector and uncorrelated x and y coordinates.")
        ("bare,B", "If set, filenames will not contain the value of the parameter N.")
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
            std::cerr << "seed-end >= seed-start must hold. Program terminates." << std::endl;
            exit(-1);
        }
        if (vm.count("seed-end") == 0) // label: seed-end set
        {
            vm.insert(std::make_pair("seed-end", bpo::variable_value()));
        }
        vm.at("seed-end").value() = vm["seed-start"].as<int>();
    }
#pragma endregion

#pragma region generate and write out configuration
    for (int seed_val = vm["seed-start"].as<int>(); seed_val <= vm["seed-end"].as<int>(); ++seed_val) // generate configurations with seeds in the range of [seed-start; seed-end]; seed-end has been set at label: seed-end set
    {
        std::string ofname = "dislocation-configurations/ic_" + std::to_string(seed_val);
        if(vm.count("bare") == 0)
            ofname += "_" + std::to_string(vm["N"].as<int>());
        ofname += ".txt"; // output filename; the file is inside a folder

        std::ofstream ofile(ofname); // the filestream
        if (!ofile) // evaluates to false if file cannot be opened
        {
            std::cerr << "Cannot write " << ofname << ". Program terminates." << std::endl;
            exit(-1);
        }
        std::cout << "Generating configuration with seed value " << seed_val << "." << std::endl;
        ofile.precision(std::numeric_limits<double>::max_digits10); // print out every digits

        br::mt19937 engine(seed_val); // Mersenne twister is a good and expensive random number generator
        br::uniform_real_distribution<> distr(-0.5, 0.5); // uniform distribution on [-0.5; 0.5)
        using disl = std::tuple<double, double, int>; // a dislocation is a (double, double, int) tuple
        std::vector<disl> dislocs; // container of the N number of dislocations
        
        for (int n = 0; n < vm["N"].as<int>(); ++n) // generate the N number of dislocations
            dislocs.push_back(disl(distr(engine), distr(engine), (n % 2) * 2 - 1)); // x, y coordinates, and the Burger's vector
        
        if (vm.count("unsorted") == 0) // sorting if not told otherwise
            std::sort(dislocs.begin(), dislocs.end(), [](const disl& a, const disl& b) {return (std::get<1>(a) + std::get<2>(a)) > (std::get<1>(b) + std::get<2>(b)); });

        for_each(dislocs.begin(), dislocs.end(), [&ofile](const disl& a) {ofile << std::get<0>(a) << "\t" << std::get<1>(a) << "\t" << std::get<2>(a) << "\n"; }); // print out to ofile
    }
#pragma endregion


    std::cout << "Done.\n";
    return 0;
}