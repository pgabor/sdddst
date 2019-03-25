/*
 * SDDDST Simple Discrete Dislocation Dynamics Toolkit
 * Copyright (C) 2015-2019  Gábor Péterffy <peterffy95@gmail.com>
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

#include "constants.h"
#include "project_parser.h"
#include "Fields/AnalyticField.h"
#include "Fields/PeriodicShearStressELTE.h"
#include "StressProtocols/stress_protocol.h"
#include "StressProtocols/fixed_rate_protocol.h"

#include <iostream>

sdddstCore::ProjectParser::ProjectParser(int argc, char **argv):
    sD(nullptr)
{
    boost::program_options::options_description requiredOptions("Required Options");
    boost::program_options::options_description optionalOptions("Optional Options");
    boost::program_options::options_description fieldOptions("Available dislocation fields");
    boost::program_options::options_description externalStressProtocolOptions("External stress protocols (optional)");

    requiredOptions.add_options()
            ("dislocation-configuration", boost::program_options::value<std::string>(), "plain text file path containing dislocation data in {x y b} triplets")
            ("result-dislocation-configuration", boost::program_options::value<std::string>(), "path where the result configuration will be stored at the end of the simulation")
    ;

    optionalOptions.add_options()
            ("point-defect-configuration", boost::program_options::value<std::string>(), "plain text file path containing point defect data in {x y} pairs")
            ("logfile-path", boost::program_options::value<std::string>(), "path for the plain text log file (it will be overwritten if it already exists)")
            ("time-limit", boost::program_options::value<double>(), "in simulation time limit, if reached the simulation stops")
            ("step-count-limit", boost::program_options::value<unsigned int>(), "the simulation will stop after successful N steps")
            ("strain-increase-limit", boost::program_options::value<double>(), "the simulation will stop after arg total strain increase is reached")
            ("avalanche-detection-limit", boost::program_options::value<unsigned int>(), "the simulation will stop after the threshold was reached with the given numer of events from above")
            ("avalanche-speed-threshold", boost::program_options::value<double>()->default_value(1e-3), "speed threshold for counting avalanches")
            ("initial-stepsize", boost::program_options::value<double>()->default_value(1e-6), "first tried step size for the simulation")
            ("cutoff-multiplier", boost::program_options::value<double>()->default_value(1e20), "multiplier of the 1/sqrt(N) cutoff parameter")
            ("max-stepsize", boost::program_options::value<double>(), "the stepsize can not overleap this value")
            ("calculate-strain", "turns on strain calculation for the simulation")
            ("calculate-order-parameter", "turns on order parameter calculation during the simulation")
            ("position-precision", boost::program_options::value<double>()->default_value(1e-5), "minimum precision for the positions for the adaptive step size protocol")
            ("save-sub-configurations", boost::program_options::value<std::string>(), "saves the current configuration after every N successful step to the given destination")
            ("sub-configuration-delay", boost::program_options::value<unsigned int>()->default_value(5), "number of successful steps between the sub configurations written out")
            ("sub-configuration-delay-during-avalanche", boost::program_options::value<unsigned int>()->default_value(1), "number of successful steps between the sub configurations written out during avalanche if avalanche detection is on")
            ;

    fieldOptions.add_options()
            ("periodic-stress-field-elte", boost::program_options::value<std::string>()->default_value("."), "periodic stress field based on ELTE library, the argument should be the folder where the compressed numerical data can be found (default)")
            ("periodic-stress-field-analytic", ("analytic periodic stress field, number of images in each direction: " + std::to_string(ANALYTIC_FIELD_N)).c_str())
            ;

    externalStressProtocolOptions.add_options()
            ("no-external-stress", "no external stress during the simulation (default)")
            ("fixed-rate-external-stress", boost::program_options::value<double>(), "external stress is linear with time, rate should be specified as an arg")
            ("spring-constant", boost::program_options::value<double>(), "simple model of an experiment where a spring is used, the arg should be the spring constant (it is valid only with the previous option together)")
            ;

    boost::program_options::options_description options;

    options.add(requiredOptions).add(optionalOptions).add(fieldOptions).add(externalStressProtocolOptions).add_options()
            ("help", "show this help")
            ("hide-copyright,c", "hides the copyright notice from the standard output");

    boost::program_options::variables_map vm;

    try{
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm, true);
    }
    catch(boost::program_options::error & e)
    {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }

    if (!vm.count("hide-copyright"))
    {
        printLicense();
    }
    if (vm.count("help"))
    {
        std::cout << options << std::endl;
        exit(0);
    }
    else
    {
        processInput(vm);
    }
}

sdddstCore::ProjectParser::~ProjectParser()
{

}

std::shared_ptr<sdddstCore::SimulationData> sdddstCore::ProjectParser::getSimulationData()
{
    return sD;
}

void sdddstCore::ProjectParser::printLicense()
{
    std::cout << "SDDDST - Simple Discrete Dislocation Dynamics Toolkit\n"
                 "Copyright (C) 2015-2019 Gábor Péterffy <peterffy95@gmail.com>\n"
                 "This program comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it under certain conditions; see the license for details\n";
}

void sdddstCore::ProjectParser::processInput(boost::program_options::variables_map &vm)
{
    // Check for required options
    if (0 == vm.count("dislocation-configuration"))
    {
        std::cerr << "dislocation-configuration is missing!\n";
        exit(-1);
    }

    if (0 == vm.count("result-dislocation-configuration"))
    {
        std::cerr << "result-dislocation-configuration is missing!\n";
        exit(-1);
    }

    if (vm.count("point-defect-configuration") == 1)
    {
        sD = std::shared_ptr<SimulationData>(new SimulationData(vm["dislocation-configuration"].as<std::string>(),
                vm["point-defect-configuration"].as<std::string>()));
    }
    else
    {
        sD = std::shared_ptr<SimulationData>(new SimulationData(vm["dislocation-configuration"].as<std::string>(), ""));
    }

    if (vm.count("time-limit"))
    {
        sD->isTimeLimit = true;
        sD->timeLimit = vm["time-limit"].as<double>();
    }

    if (vm.count("initial-stepsize"))
    {
        sD->stepSize = vm["initial-stepsize"].as<double>();
    }

    if (vm.count("position-precision"))
    {
        sD->prec = vm["position-precision"].as<double>();
    }

    if (vm.count("cutoff-multiplier"))
    {
        sD->cutOffMultiplier = vm["cutoff-multiplier"].as<double>();
        sD->updateCutOff();
    }

    if (vm.count("strain-increase-limit"))
    {
        sD->isStrainIncreaseLimit = true;
        sD->totalAccumulatedStrainIncreaseLimit = vm["strain-increase-limit"].as<double>();
    }

    if (vm.count("max-stepsize"))
    {
        sD->maxStepSizeLimit = vm["max-stepsize"].as<double>();
    }

    if (vm.count("point-defect-configuration"))
    {
        sD->A *= 1./sqrt(sD->dc);
        sD->KASQR *= double(sD->dc);
    }

    if (vm.count("calculate-strain"))
    {
        sD->calculateStrainDuringSimulation = true;
    }

    if (vm.count("calculate-order-parameter"))
    {
        sD->orderParameterCalculationIsOn = true;
    }

    if (vm.count("logfile-path"))
    {
        sD->standardOutputLog = std::ofstream(vm["logfile-path"].as<std::string>());
    }

    if (vm.count("step-count-limit"))
    {
        sD->isStepCountLimit = true;
        sD->stepCountLimit = vm["step-count-limit"].as<unsigned int>();
    }

    if (vm.count("avalanche-detection-limit"))
    {
        sD->countAvalanches = true;
        sD->avalancheTriggerLimit = vm["avalanche-detection-limit"].as<unsigned int>();
        sD->avalancheSpeedThreshold = vm["avalanche-speed-threshold"].as<double>();
    }

    if (vm.count("save-sub-configurations"))
    {
        sD->isSaveSubConfigs = true;
        sD->subConfigPath = vm["save-sub-configurations"].as<std::string>();
        sD->subConfigDelay = vm["sub-configuration-delay"].as<unsigned int>();
        sD->subConfigDelayDuringAvalanche = vm["sub-configuration-delay-during-avalanche"].as<unsigned int>();
    }

    sD->endDislocationConfigurationPath = vm["result-dislocation-configuration"].as<std::string>();

    if (vm.count("periodic-stress-field-analytic"))
    {
        sD->tau = std::unique_ptr<Field>(new AnalyticField());
    }
    else
    {
        std::unique_ptr<sdddstCoreELTE::PeriodicShearStressELTE> tmp(new sdddstCoreELTE::PeriodicShearStressELTE());
        tmp->loadStress(vm["periodic-stress-field-elte"].as<std::string>(), "xy", 1024);
        tmp->loadStress(vm["periodic-stress-field-elte"].as<std::string>(), "xy_diff_x", 1024);
        sD->tau = std::move(tmp);
    }
  /*
    "no-external-stress", "no external stress during the simulation (default)")
                ("fixed-rate-external-stress", boost::program_options::value<double>(), "external stress is linear with time, rate should be specified as an arg")
                ("spring-constant"

                 ;*/
    if (vm.count("no-external-stress") && vm.count("fixed-rate-external-stress"))
    {
        std::cerr << "Only one stress protocol can be defined at the same time!\n";
        exit(-20);
    }
    else if (vm.count("fixed-rate-external-stress"))
    {
        std::unique_ptr<FixedRateProtocol> tmp(new FixedRateProtocol);
        tmp->setRate(vm["fixed-rate-external-stress"].as<double>());

        sD->externalStressProtocol = std::unique_ptr<StressProtocol>(std::move(tmp));
    }
    else
    {
        sD->externalStressProtocol = std::unique_ptr<StressProtocol>(new StressProtocol());
    }
}

