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

#include "sdddstCMakeConfig.h"

#include "dislocation.h"
#include "point_defect.h"
#include "simulation_data.h"
#include "utility_wrapper.h"
#include "precision_handler.h"

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
            .def("updateTolerance", &sdddstCore::PrecisionHandler::updateTolerance)
            .def("updateError", &sdddstCore::PrecisionHandler::updateError)
            .def("get_new_stepsize", &sdddstCore::PrecisionHandler::getNewStepSize)
            .def("get_min_precisity", &sdddstCore::PrecisionHandler::getMinPrecisity)
            .def("set_min_precisity", &sdddstCore::PrecisionHandler::setMinPrecisity)
            .def("get_max_error_ratio_square", &sdddstCore::PrecisionHandler::getMaxErrorRatioSqr)
            .def("__repr__", &sdddstCore::PrecisionHandler::__repr__)
            .def("__str__", &sdddstCore::PrecisionHandler::__str__);
}
