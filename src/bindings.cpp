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
#include "dislocation_wrapper.h"
#include "point_defect_wrapper.h"

#include <boost/python.hpp>

std::string hello_world()
{
    return "Hello World!";
}

BOOST_PYTHON_MODULE(PYTHON_LIB_NAME)
{
    using namespace boost::python;
    def("hello_world", hello_world);

    class_<PySdddstCore::PyDislocation>("Dislocation")
            .def_readwrite("x", &PySdddstCore::PyDislocation::x)
            .def_readwrite("y", &PySdddstCore::PyDislocation::y)
            .def_readwrite("b", &PySdddstCore::PyDislocation::b)
            .def("__repr__", &PySdddstCore::PyDislocation::__repr__)
            .def("__str__", &PySdddstCore::PyDislocation::__str__);

    class_<PySdddstCore::PyPointDefect>("PointDefect")
            .def_readwrite("x", &PySdddstCore::PyPointDefect::x)
            .def_readwrite("y", &PySdddstCore::PyPointDefect::y)
            .def("__repr__", &PySdddstCore::PyPointDefect::__repr__)
            .def("__str__", &PySdddstCore::PyPointDefect::__str__);
}
