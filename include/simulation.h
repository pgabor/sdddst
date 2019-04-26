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

#ifndef SDDDST_CORE_SIMULATION_H
#define SDDDST_CORE_SIMULATION_H

#include "dislocation.h"
#include "precision_handler.h"
#include "simulation_data.h"
#include "StressProtocols/stress_protocol.h"

#include <memory>
#include <sstream>
#include <vector>

#ifdef BUILD_PYTHON_BINDINGS
#include <boost/python.hpp>
#endif

namespace sdddstCore {

class Simulation
{
public:
    Simulation(std::shared_ptr<SimulationData> _sD);
    ~Simulation();

    void integrate(const double & stepsize, std::vector<Dislocation> &newDislocation, const std::vector<Dislocation> &old, bool useSpeed2, bool calculateInitSpeed, sdddstCore::StressProtocolStepType origin, sdddstCore::StressProtocolStepType end);
    void calculateSpeeds(const std::vector<Dislocation> & dis, std::vector<double>  & res);
    void calculateG(const double & stepsize, std::vector<Dislocation> &newDislocation, const std::vector<Dislocation> &old, bool useSpeed2, bool calculateInitSpeed, bool useInitSpeedForFirstStep, StressProtocolStepType origin, StressProtocolStepType end);
    void calculateJacobian(const double &stepsize, const std::vector<Dislocation> &data);
    void calculateXError();

    void calculateSparseFormForJacobian();
    void solveEQSys();

    double calculateOrderParameter(const std::vector<double> & speeds);
    double calculateStrainIncrement(const std::vector<Dislocation> & old, const std::vector<Dislocation> & newD);

    double getElement(int j, int si, int ei);

    double getSimTime();

    void run();

    bool step();

    void stepStageI();
    void stepStageII();
    void stepStageIII();

    const std::vector<Dislocation> & getStoredDislocationData();

#ifdef BUILD_PYTHON_BINDINGS
    static Simulation * create(boost::python::object simulationData);
#endif

private:
    bool succesfulStep;
    double lastWriteTimeFinished;
    bool initSpeedCalculationIsNeeded;
    bool firstStepRequest;
    double energy;
    double energyAccum;
    double vsquare;
    double vsquare1;
    double vsquare2;

    std::shared_ptr<SimulationData> sD;
    std::unique_ptr<PrecisionHandler> pH;
};

}

#endif
