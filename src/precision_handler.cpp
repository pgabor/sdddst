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
#include "precision_handler.h"

#include <cmath>
#include <iostream>

using namespace sdddstCore;

PrecisionHandler::PrecisionHandler():
    minPrecisity(1e-6),
    minPrecisitySqr(1e-12),
    maxErrorRatioSqr(0),
    selectedID(0)
{
    //Nothing to do
}

PrecisionHandler::~PrecisionHandler()
{
    // Nothing to do
}

void PrecisionHandler::setSize(unsigned int size)
{
    if (toleranceAndError.size() < size)
    {
        while (toleranceAndError.size() != size)
        {
            toleranceAndError.push_back(std::make_pair(minPrecisitySqr, 0));
        }
    }
    else
    {
        toleranceAndError.resize(size);
    }
}

unsigned long PrecisionHandler::getSize() const
{
    return toleranceAndError.size();
}

void PrecisionHandler::reset()
{
    maxErrorRatioSqr = 0;
    for (auto & i : toleranceAndError)
    {
        i.first = minPrecisitySqr;
        i.second = 0;
    }
}

void PrecisionHandler::updateTolerance(const double &distanceSqr, const unsigned int &ID)
{
    double tmp = distanceSqr * 0.25 * 1e-2;
    if (tmp < minPrecisitySqr && tmp < toleranceAndError[ID].first)
    {
        if (tmp == 0.0)
        {
            toleranceAndError[ID].first = EPS;
            std::cout << "Two dislocations are in the same place!\n";
        }
        else
        {
            toleranceAndError[ID].first = tmp;
        }
    }
}

void PrecisionHandler::updateError(const double &error, const unsigned int &ID)
{
    if (toleranceAndError[ID].second < error)
    {
        toleranceAndError[ID].second = error;
        double tmp = error * error / toleranceAndError[ID].first;
        if (tmp > maxErrorRatioSqr)
        {
            maxErrorRatioSqr = tmp;
            selectedID = ID;
        }
    }
}

double PrecisionHandler::getNewStepSize(const double &oldStepSize)
{
    if(0.0 == maxErrorRatioSqr)
    {
        return oldStepSize * 2.0;
    }

    double tmp = 1./sqrt(maxErrorRatioSqr);

    tmp = pow(tmp, 1./3.);
    if (tmp > 2.0)
    {
        tmp = 2.0;
    }

    return 0.9 * oldStepSize * tmp;
}

double PrecisionHandler::getMinPrecisity() const
{
    return minPrecisity;
}

void PrecisionHandler::setMinPrecisity(double value)
{
    minPrecisity = value;
    minPrecisitySqr = value * value;
}

double PrecisionHandler::getMaxErrorRatioSqr() const
{
    return maxErrorRatioSqr;
}
