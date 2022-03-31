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

#ifndef SDDDST_CORE_SPECTRAL_CALCULATION_H
#define SDDDST_CORE_SPECTRAL_CALCULATION_H

#include "Fields/Field.h"
#include "dislocation.h"
#include "point_defect.h"

#include <vector>

namespace sdddstEV {

struct Pair
{
    double eigen;
    unsigned long index;
};

class SpectralDecompositor
{
public:
    SpectralDecompositor(sdddstCore::Field * f);
    ~SpectralDecompositor();

    void decompose(std::vector<sdddstCore::Dislocation> & dislocations, std::vector<sdddstCore::PointDefect> &pointDefects);

    double getEigenValue(unsigned int i);
    double getEigenVectorElement(int i, int j);

    unsigned long getDislocationCount() const;

    unsigned long getFpCount() const;

private:
    unsigned long dislocationCount;
    unsigned long fpCount;
    std::unique_ptr<sdddstCore::Field> field;

    double sdA;
    double sdKASQR;
    double * matrix;
    unsigned long matrixSize;
    double * w;

    std::vector<Pair> eigenValues;
};


}

#endif

