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

#include "simulation_data.h"
#include "constants.h"
#include "StressProtocols/stress_protocol.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace sdddstCore;

SimulationData::SimulationData(const std::string &dislocationDataFilePath, const std::string &fixpointsDataFilePath):
    cutOffMultiplier(DEFAULT_CUTOFF_MULTIPLIER),
    cutOff(DEFAULT_CUTOFF),
    cutOffSqr(cutOff*cutOff),
    onePerCutOffSqr(1./cutOffSqr),
    prec(DEFAULT_PRECISION),
    pc(0),
    dc(0),
    ic(DEFAULT_ITERATION_COUNT),
    timeLimit(DEFAULT_TIME_LIMIT),
    stepSize(DEFAULT_STEP_SIZE),
    simTime(DEFAULT_SIM_TIME),
    KASQR(DEFAULT_KASQR),
    A(DEFAULT_A),
    tau(nullptr),
    Ap(nullptr),
    Ai(nullptr),
    Ax(nullptr),
    x(nullptr),
    null(NULL),
    Symbolic(nullptr),
    Numeric(nullptr),
    succesfulSteps(0),
    failedSteps(0),
    totalAccumulatedStrainIncrease(0),
    isStrainIncreaseLimit(false),
    totalAccumulatedStrainIncreaseLimit(0),
    isMaxStepSizeLimit(false),
    maxStepSizeLimit(0),
    isTimeLimit(false),
    isStepCountLimit(false),
    stepCountLimit(0),
    calculateStrainDuringSimulation(false),
    orderParameterCalculationIsOn(false),
    standardOutputLog(),
    endDislocationConfigurationPath(""),
    externalStressProtocol(nullptr),
    countAvalanches(false),
    avalancheSpeedThreshold(0),
    avalancheTriggerLimit(0),
    avalancheCount(0),
    inAvalanche(false),
    isSaveSubConfigs(false),
    subConfigPath(""),
    subConfigDelay(0),
    subConfigDelayDuringAvalanche(0),
    subconfigDistanceCounter(0),
    currentStressStateType(sdddstCore::StressProtocolStepType::Original),
    dislocationDataIsLoaded(false)
{
    readDislocationDataFromFile(dislocationDataFilePath);
    readPointDefectDataFromFile(fixpointsDataFilePath);
    initSimulationVariables();
}

void SimulationData::readDislocationDataFromFile(const std::string &dislocationDataFilePath)
{
    if (dislocationDataIsLoaded)
    {
        std::cerr << "Dislocation data was already loaded! Exiting...\n";
        exit(-10);
    }
    if (dislocationDataFilePath.empty())
    {
        return;
    }

    dislocationDataIsLoaded = true;

    std::ifstream in(dislocationDataFilePath);
    assert(in.is_open() && "Cannot open dislocation data file!");

    // Iterating through the file
    while(!in.eof())
    {
        std::string data;
        in >> data;
        if (data == "")
        {
            break;
        }
        Dislocation tmp;
        tmp.x = std::stod(data);
        in >> tmp.y;
        in >> tmp.b;
        dislocations.push_back(tmp);
        dc++;
    }
    updateMemoryUsageAccordingToDislocationCount();
}

void SimulationData::writeDislocationDataToFile(const std::string &dislocationDataFilePath)
{
    std::ofstream out(dislocationDataFilePath);
    assert(out.is_open() && "Cannot open the data file to write!");
    out << std::scientific << std::setprecision(16);
    for (auto & i: dislocations)
    {
        out << i.x << " " << i.y << " " << i.b << "\n";
    }
}

void SimulationData::readPointDefectDataFromFile(const std::string &pointDefectDataFilePath)
{
    if (pointDefectDataFilePath.empty())
    {
        pc = 0;
        return;
    }
    pc = 0;
    points.resize(0);
    std::ifstream in(pointDefectDataFilePath);
    assert(in.is_open() && "Cannot open fixpoints data file!");

    // Iterating through the file
    while (!in.eof())
    {
        std::string data;
        in >> data;
        if (data == "")
        {
            break;
        }
        PointDefect tmp;
        tmp.x = std::stod(data);
        in >> tmp.y;
        points.push_back(tmp);
        pc++;
    }
}

void SimulationData::writePointDefectDataToFile(const std::string &pointDefectDataFilePath)
{
    std::ofstream out(pointDefectDataFilePath);
    assert(out.is_open() && "Cannot open the data file to write!");
    out << std::scientific << std::setprecision(16);
    for (auto & i: points)
    {
        out << i.x << " " << i.y << "\n";
    }
}

void SimulationData::initSimulationVariables()
{
    updateCutOff();
}

void SimulationData::updateCutOff()
{
    double multiplier = cutOffMultiplier;
    if (cutOffMultiplier >=  1./12. * sqrt(2.0 * double(dc)))
    {
        multiplier = 1e20;
    }
    cutOff = 1./sqrt(dc) * multiplier;
    cutOffSqr = cutOff * cutOff;
    onePerCutOffSqr = 1./cutOffSqr;
}

void SimulationData::deleteDislocationCountRelatedData()
{
    dc = 0;
    dislocations.resize(0);
    g.resize(0);
    initSpeed.resize(0);
    initSpeed2.resize(0);
    speed.resize(0);
    speed2.resize(0);
    dVec.resize(0);
    bigStep.resize(0);
    firstSmall.resize(0);
    secondSmall.resize(0);
    free(Ap);
    Ap = nullptr;
    free(Ai);
    Ai = nullptr;
    free(Ax);
    Ax = nullptr;
    free(x);
    x = nullptr;
    indexes.resize(0);
}

void SimulationData::updateMemoryUsageAccordingToDislocationCount()
{
    g.resize(dc);
    initSpeed.resize(dc);
    initSpeed2.resize(dc);
    speed.resize(dc);
    speed2.resize(dc);
    dVec.resize(dc);
    bigStep.resize(dc);
    firstSmall.resize(dc);
    secondSmall.resize(dc);
    Ap = (int*) calloc(dc+1, sizeof(int));
    Ai = (int*) calloc(dc*dc, sizeof(int));
    Ax = (double*) calloc(dc*dc, sizeof(double));
    x = (double*) calloc(dc, sizeof(double));
    assert(Ap && "Memory allication for Ap failed!");
    assert(Ai && "Memory allocation for Ai failed!");
    assert(Ax && "Memory allocation for Ax failed!");
    assert(x && "Memory allocation for x failed!");
    indexes.resize(dc);
}
