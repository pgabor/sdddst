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
#include "StressProtocols/stress_protocol.h"
#include "StressProtocols/fixed_rate_protocol.h"
#include "StressProtocols/spring_protocol.h"

#include <iostream>

sdddstCore::ProjectParser::ProjectParser(int argc, char **argv):
    sD(nullptr),
    pType(NONE)
{
    boost::program_options::options_description operationModeOptions("Operation");
    boost::program_options::options_description requiredOptions("Required Options");
    boost::program_options::options_description optionalOptions("Optional Options");
    boost::program_options::options_description fieldOptions("Available dislocation fields");
    boost::program_options::options_description externalStressProtocolOptions("External stress protocols (optional)");

    operationModeOptions.add_options()
            ("simulation", "run a simulation (default)")
            ("ev-analyzation", "run eigen value analysation");

    requiredOptions.add_options()
            ("dislocation-configuration", boost::program_options::value<std::string>(), "plain text file path containing dislocation data in {x y b} triplets")
            ("result-dislocation-configuration", boost::program_options::value<std::string>(), "path where the result configuration will be stored at the end of the simulation")
    ;

    optionalOptions.add_options()
            ("point-defect-configuration", boost::program_options::value<std::string>(), "plain text file path containing point defect data in {x y} pairs")
            ("logfile-path", boost::program_options::value<std::string>(), "path for the plain text log file (it will be overwritten if it already exists)")
            ("time-limit", boost::program_options::value<double>(), "in simulation time limit, if reached the simulation stops")
            ("speed-limit", boost::program_options::value<double>(), "in simulation units, if |v| falls below, the simulation stops")
            ("step-count-limit", boost::program_options::value<unsigned int>(), "the simulation will stop after successful N steps")
            ("strain-increase-limit", boost::program_options::value<double>(), "the simulation will stop after arg total strain increase is reached")
            ("stress-limit", boost::program_options::value<double>(), "the simulation will stop after arg external stress is reached")
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
            ("change-cutoff-to-inf-under-threshold", boost::program_options::value<double>(), "if the avg speed decreases once under this threshold during the simulation the applied cutoff multiplier will be 1e20")
            ;

    fieldOptions.add_options()
            ("periodic-stress-field-analytic", ("analytic periodic stress field (default), number of images in each direction: " + std::to_string(ANALYTIC_FIELD_N)).c_str())
            ;

    externalStressProtocolOptions.add_options()
            ("no-external-stress", "no external stress during the simulation (default)")
            ("fixed-rate-external-stress", boost::program_options::value<double>(), "external stress is linear with time, rate should be specified as an arg")
            ("spring-constant", boost::program_options::value<double>(), "simple model of an experiment where a spring is used, the arg should be the spring constant (it is valid only with the previous option together)")
            ;

    boost::program_options::options_description ev_options("EV analysis related options");
    ev_options.add_options()
            ("file-list", boost::program_options::value<std::string>(), "file list with timestamps int timestamp file_path format")
            ("pd-configuration", boost::program_options::value<std::string>(), "file path to the point defect config file")
            ("result-ev-file", boost::program_options::value<std::string>(), "file path where to save result");

    boost::program_options::options_description options("Extra options");

    options.add(operationModeOptions).add(requiredOptions).add(optionalOptions).add(fieldOptions).add(externalStressProtocolOptions).add(ev_options).add_options()
            ("help", "show this help")
            ("hide-copyright,c", "hides the copyright notice from the standard output");

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
                 "Copyright (C) 2015-2022 Gábor Péterffy <gabor.peterffy@ttk.elte.hu>\n"
                 "This program comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it under certain conditions; see the license for details\n";
}

void sdddstCore::ProjectParser::processInput(boost::program_options::variables_map &vm)
{
    if (0 == vm.count("ev-analyzation"))
    {
        pType = SIMULATION;
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
        else if (vm.count("point-defect-configuration") > 0) {
            std::cerr << "Multiple definition of point-defect-configuration\n";
            exit(2);
        } else
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

        if (vm.count("stress-limit"))
        {
            sD->stressLimit = vm["stress-limit"].as<double>();
            sD->isStressLimit = true;
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
            sD->isMaxStepSizeLimit = true;
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

        if (vm.count("speed-limit"))
        {
            sD->isSpeedLimit = true;
            sD->speedLimit = vm["speed-limit"].as<double>();
        }

        sD->endDislocationConfigurationPath = vm["result-dislocation-configuration"].as<std::string>();

        sD->tau = std::unique_ptr<Field>(new AnalyticField());
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
            std::unique_ptr<FixedRateProtocol> tmp;
            if (vm.count("spring-constant") > 0) {
                std::unique_ptr<SpringProtocol> tmp2(new SpringProtocol);
                tmp2->setSpringConstant(vm["spring-constant"].as<double>());
                tmp.reset(tmp2.release());
            } else {
                tmp.reset(new FixedRateProtocol);
            }
            tmp->setRate(vm["fixed-rate-external-stress"].as<double>());

            sD->externalStressProtocol = std::unique_ptr<StressProtocol>(std::move(tmp));
        }
        else
        {
            sD->externalStressProtocol = std::unique_ptr<StressProtocol>(new StressProtocol());
        }

        if (vm.count("change-cutoff-to-inf-under-threshold"))
        {
            sD->speedThresholdForCutoffChange = vm["change-cutoff-to-inf-under-threshold"].as<double>();
            sD->isSpeedThresholdForCutoffChange = true;
        }
    } else if (1 == vm.count("ev-analyzation") && 0 == vm.count("simulation")) {
        sD = std::shared_ptr<SimulationData>(new SimulationData());
        if (vm.count("result-ev-file") != 1) {
            std::cerr << "result-ev-file is not defined! Exiting..." << std::endl;
            exit(1);
        }
        sD->eVAnalResultFile = vm["result-ev-file"].as<std::string>();
        pType = EV_ANALYZATION;
    }
}



namespace sdddstCore {
ProjectType ProjectParser::getPType() const
{
    return pType;
}

std::shared_ptr<DataTimeSeries> ProjectParser::getDataTimeSeries()
{
    if (getPType() == EV_ANALYZATION) {
        if (vm.count("pd-configuration") == 1 && vm.count("file-list") == 1) {
            return std::shared_ptr<DataTimeSeries>(new DataTimeSeries(vm["file-list"].as<std::string>(), vm["pd-configuration"].as<std::string>()));
        }
    }
    return nullptr;
}

StressProtocol *ProjectParser::getStressProtocol()
{
    if (vm.count("fixed-rate-external-stress") == 1) {
        auto fpr = new sdddstCore::FixedRateProtocol();
        fpr->setRate(vm["fixed-rate-external-stress"].as<double>());
        return fpr;
    } else {
        abort();
    }
}

}
