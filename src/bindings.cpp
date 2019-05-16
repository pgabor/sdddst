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

 /*
#include "sdddstCMakeConfig.h"

#include "dislocation.h"
#include "Fields/Field.h"

#include "field_wrapper.h"
#include "periodic_shear_stress_ELTE_wrapper.h"
#include "analytic_field_wrapper.h"
#include "stress_protocol_wrapper.h"
#include "fixed_rate_protocol_wrapper.h"
#include "simulation_data_wrapper.h"

#include "point_defect.h"
#include "simulation_data.h"
#include "utility_wrapper.h"
#include "precision_handler.h"
#include "StressProtocols/stress_protocol.h"
#include "simulation.h"

#include <memory>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

std::string hello_world()
{
    return "Hello World!";
}

BOOST_PYTHON_MODULE(PYTHON_LIB_NAME)
{
    using namespace boost::python;
    def("hello_world", hello_world);

/// Functions
///

// utility.h

def("normalize", &py_normalize);
def("X", &X);
def("X2", &X2);
def("E", &E);
def("X_dx", &X_dx);
def("X2_dx", &X2_dx);
def("E_dx", &E_dx);
def("get_wall_time", &get_wall_time);
def("get_cpu_time", &get_cpu_time);

/// Classes
///
    class_<sdddstCore::Dislocation>("Dislocation")
            .def_readwrite("x", &sdddstCore::Dislocation::x)
            .def_readwrite("y", &sdddstCore::Dislocation::y)
            .def_readwrite("b", &sdddstCore::Dislocation::b)
            .def("__eq__", &sdddstCore::Dislocation::operator==)
            .def("__repr__", &sdddstCore::Dislocation::__repr__)
            .def("__str__", &sdddstCore::Dislocation::__str__);

    class_<std::vector<sdddstCore::Dislocation>>("DislocationVector")
        .def(vector_indexing_suite<std::vector<sdddstCore::Dislocation> >());

    class_<sdddstCore::PointDefect>("PointDefect")
            .def_readwrite("x", &sdddstCore::PointDefect::x)
            .def_readwrite("y", &sdddstCore::PointDefect::y)
            .def("__repr__", &sdddstCore::PointDefect::__repr__)
            .def("__str__", &sdddstCore::PointDefect::__str__);

    class_<std::vector<sdddstCore::PointDefect>>("PointDefectVector")
        .def(vector_indexing_suite<std::vector<sdddstCore::PointDefect> >());

    class_<sdddstCore::PrecisionHandler>("PrecisionHandler")
            .def("set_size", &sdddstCore::PrecisionHandler::setSize)
            .def("get_size", &sdddstCore::PrecisionHandler::getSize)
            .def("reset", &sdddstCore::PrecisionHandler::reset)
            .def("update_tolerance", &sdddstCore::PrecisionHandler::updateTolerance)
            .def("update_error", &sdddstCore::PrecisionHandler::updateError)
            .def("get_new_stepsize", &sdddstCore::PrecisionHandler::getNewStepSize)
            .def("get_min_precisity", &sdddstCore::PrecisionHandler::getMinPrecisity)
            .def("set_min_precisity", &sdddstCore::PrecisionHandler::setMinPrecisity)
            .def("get_max_error_ratio_square", &sdddstCore::PrecisionHandler::getMaxErrorRatioSqr)
            .def("__repr__", &sdddstCore::PrecisionHandler::__repr__)
            .def("__str__", &sdddstCore::PrecisionHandler::__str__);

    class_<sdddstCore::Field, boost::noncopyable>("Field")
            .def("xy", &sdddstCore::Field::xy)
            .def("xy_diff_x", &sdddstCore::Field::xy_diff_x);

    class_<PySdddstCore::PyField, boost::noncopyable>("FieldObject")
            .def("init", &PySdddstCore::PyField::init)
            .def("valid", &PySdddstCore::PyField::valid)
            .def("name", &PySdddstCore::PyField::name)
            .def("__str__", &PySdddstCore::PyField::__str__)
            .def("__repr__", &PySdddstCore::PyField::__repr__);

    class_<PySdddstCore::PyAnalyticField, bases<PySdddstCore::PyField>, boost::noncopyable>("AnalyticFieldObject");

    class_<PySdddstCore::PyPeriodicShearStressFieldELTE, bases<PySdddstCore::PyField>, boost::noncopyable>("PeriodicShearStressFieldELTEObject")
            .def("set_pwd", &PySdddstCore::PyPeriodicShearStressFieldELTE::setPWD);

    enum_<sdddstCore::StressProtocolStepType>("StepType")
            .value("Original", sdddstCore::StressProtocolStepType::Original)
            .value("EndOfBigStep", sdddstCore::StressProtocolStepType::EndOfBigStep)
            .value("EndOfFirstSmallStep", sdddstCore::StressProtocolStepType::EndOfFirstSmallStep)
            .value("EndOfSecondSmallStep", sdddstCore::StressProtocolStepType::EndOfSecondSmallStep);

    class_<sdddstCore::StressProtocol, boost::noncopyable>("StressProtocol")
            .def("calculate_stress", &sdddstCore::StressProtocol::calculateStress)
            .def("get_stress", &sdddstCore::StressProtocol::getStress);

    class_<PySdddstCore::PyStressProtocol, boost::noncopyable>("StressProtocolObject")
            .def("init", &PySdddstCore::PyStressProtocol::init)
            .def("valid", &PySdddstCore::PyStressProtocol::valid)
            .def("name", &PySdddstCore::PyStressProtocol::name)
            .def("__repr__", &PySdddstCore::PyStressProtocol::__repr__)
            .def("__str__", &PySdddstCore::PyStressProtocol::__str__);

    class_<PySdddstCore::PyFixedRateProtocol, bases<PySdddstCore::PyStressProtocol>, boost::noncopyable>("FixedRateStressProtocolObject")
            .add_property("rate", &PySdddstCore::PyFixedRateProtocol::getRate, &PySdddstCore::PyFixedRateProtocol::setRate);

    class_<sdddstCore::SimulationData, boost::noncopyable>("SimulationData", init<std::string, std::string>())
            .def("read_dislocation_data_from_file", &sdddstCore::SimulationData::readDislocationDataFromFile)
            .def("write_dislocation_data_to_file", &sdddstCore::SimulationData::writeDislocationDataToFile)
            .def("read_point_defect_data_from_file", &sdddstCore::SimulationData::readPointDefectDataFromFile)
            .def("write_point_defect_data_to_file", &sdddstCore::SimulationData::writePointDefectDataToFile)
            .def("init_simulation_variables", &sdddstCore::SimulationData::initSimulationVariables)
            .def("update_cutoff", &sdddstCore::SimulationData::updateCutOff)
            .def_readwrite("dislocations", &sdddstCore::SimulationData::dislocations)
            .def_readwrite("point_defects", &sdddstCore::SimulationData::points)
            .def_readwrite("g_vec", &sdddstCore::SimulationData::g)
            .def_readwrite("init_speed", &sdddstCore::SimulationData::initSpeed)
            .def_readwrite("init_speed_2", &sdddstCore::SimulationData::initSpeed2)
            .def_readwrite("d_vec", &sdddstCore::SimulationData::dVec)
            .def_readwrite("cutoff_multiplier", &sdddstCore::SimulationData::cutOffMultiplier)
            .def_readwrite("cutoff", &sdddstCore::SimulationData::cutOff)
            .def_readwrite("cutoff_square", &sdddstCore::SimulationData::cutOffSqr)
            .def_readwrite("one_per_cutoff_square", &sdddstCore::SimulationData::onePerCutOffSqr)
            .def_readwrite("precision", &sdddstCore::SimulationData::prec)
            .def_readwrite("point_defect_count", &sdddstCore::SimulationData::pc)
            .def_readwrite("dislocation_count", &sdddstCore::SimulationData::dc)
            .def_readwrite("iteration_count", &sdddstCore::SimulationData::ic)
            .def_readwrite("time_limit", &sdddstCore::SimulationData::timeLimit)
            .def_readwrite("step_size", &sdddstCore::SimulationData::stepSize)
            .def_readwrite("simulation_time", &sdddstCore::SimulationData::simTime)
            .def_readwrite("KASQR", &sdddstCore::SimulationData::KASQR)
            .def_readwrite("A", &sdddstCore::SimulationData::A)
            .def_readwrite("big_step", &sdddstCore::SimulationData::bigStep)
            .def_readwrite("first_small_step", &sdddstCore::SimulationData::firstSmall)
            .def_readwrite("second_small_step", &sdddstCore::SimulationData::secondSmall)
            .def_readwrite("successful_steps", &sdddstCore::SimulationData::succesfulSteps)
            .def_readwrite("failed_steps", &sdddstCore::SimulationData::failedSteps)
            .def_readwrite("total_accumulated_strain_increase", &sdddstCore::SimulationData::totalAccumulatedStrainIncrease)
            .def_readwrite("strain_increase_limted", &sdddstCore::SimulationData::isStrainIncreaseLimit)
            .def_readwrite("total_accumulated_strain_increase_limit", &sdddstCore::SimulationData::totalAccumulatedStrainIncreaseLimit)
            .def_readwrite("max_step_size_limited", &sdddstCore::SimulationData::isMaxStepSizeLimit)
            .def_readwrite("max_stepsize_limit", &sdddstCore::SimulationData::maxStepSizeLimit)
            .def_readwrite("time_limited", &sdddstCore::SimulationData::isTimeLimit)
            .def_readwrite("step_count_limited", &sdddstCore::SimulationData::isStepCountLimit)
            .def_readwrite("step_count_limit", &sdddstCore::SimulationData::stepCountLimit)
            .def_readwrite("calculate_strain_during_simulation", &sdddstCore::SimulationData::calculateStrainDuringSimulation)
            .def_readwrite("calculate_order_parameter", &sdddstCore::SimulationData::orderParameterCalculationIsOn)
            .def_readwrite("final_dislocation_configuration_path", &sdddstCore::SimulationData::endDislocationConfigurationPath)
            .def_readwrite("count_avalanches", &sdddstCore::SimulationData::countAvalanches)
            .def_readwrite("avalanche_speed_threshold", &sdddstCore::SimulationData::avalancheSpeedThreshold)
            .def_readwrite("trigger_limit_for_avalanche_count", &sdddstCore::SimulationData::avalancheTriggerLimit)
            .def_readwrite("avalanche_count", &sdddstCore::SimulationData::avalancheCount)
            .def_readwrite("in_avalanche", &sdddstCore::SimulationData::inAvalanche)
            .def_readwrite("save_sub_configs", &sdddstCore::SimulationData::isSaveSubConfigs)
            .def_readwrite("sub_config_path", &sdddstCore::SimulationData::subConfigPath)
            .def_readwrite("sub_config_delay", &sdddstCore::SimulationData::subConfigDelay)
            .def_readwrite("sub_config_delay_during_avalanche", &sdddstCore::SimulationData::subConfigDelayDuringAvalanche)
            .def_readwrite("sub_config_distance_counter", &sdddstCore::SimulationData::subconfigDistanceCounter)
            .def_readonly("stress_state", &sdddstCore::SimulationData::currentStressStateType)
            .add_property("tau", make_function(&sdddstCore::SimulationData::getField, return_internal_reference<>()), &sdddstCore::SimulationData::setField)
            .add_property("external_stress", make_function(&sdddstCore::SimulationData::getStressProtocol, return_internal_reference<>()), &sdddstCore::SimulationData::setStressProtocol);

    class_<PySdddstCore::PySimulationData>("SimulationDataObject", init<std::string, std::string>())
            .def("__call__", &PySdddstCore::PySimulationData::resolve, return_internal_reference<>())
            .def("__repr__", &PySdddstCore::PySimulationData::__repr__)
            .def("__str__", &PySdddstCore::PySimulationData::__str__);

    class_<sdddstCore::Simulation, boost::noncopyable>("Simulation", no_init)
            .def("create", &sdddstCore::Simulation::create, return_value_policy<manage_new_object>())
            .staticmethod("create")
            .def("run", &sdddstCore::Simulation::run)
            .def("step", &sdddstCore::Simulation::step)
            .def("get_time", &sdddstCore::Simulation::getSimTime);

    class_<std::vector<double>>("DoubleVector")
            .def(vector_indexing_suite<std::vector<double>>());
}
*/