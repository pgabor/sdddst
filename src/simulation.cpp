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

#include "simulation.h"
#include "utility.h"

#ifdef BUILD_PYTHON_BINDINGS
#include "simulation_data_wrapper.h"
#endif

#include <umfpack.h>

#include <iostream>
#include <iomanip>
#include <numeric>
#include <cstdlib>
#include <sstream>

using namespace sdddstCore;

Simulation::Simulation(std::shared_ptr<SimulationData> _sD) :
    succesfulStep(true),
    lastWriteTimeFinished(0),
    initSpeedCalculationIsNeeded(true),
    firstStepRequest(true),
    energy(0),
    energyAccum(0),
    vsquare(0),
    sD(_sD),
    pH(new PrecisionHandler)
{

    // Format setting
    sD->standardOutputLog << std::scientific << std::setprecision(16);

    pH->setMinPrecisity(sD->prec);
    pH->setSize(sD->dc);
}

Simulation::~Simulation()
{
}


void Simulation::integrate(const double &stepsize, std::vector<Dislocation> &newDislocation, const std::vector<Dislocation> & old,
                           bool useSpeed2, bool calculateInitSpeed, StressProtocolStepType origin, StressProtocolStepType end)
{
    calculateJacobian(stepsize, newDislocation);
    calculateSparseFormForJacobian();
    for (size_t i = 0; i < sD->ic; i++)
    {
        if (i > 0)
        {
            calculateG(stepsize, newDislocation, old, useSpeed2, false, false, origin, end);
        }
        else
        {
            calculateG(stepsize, newDislocation, old, useSpeed2, calculateInitSpeed, sD->externalStressProtocol->getType() == "zero-stress" ? true : false, origin, end);
        }
        solveEQSys();
        for (size_t j = 0; j < sD->dc; j++)
        {
            newDislocation[j].x -= sD->x[j];
        }
    }
    umfpack_di_free_numeric (&sD->Numeric) ;
}

void Simulation::calculateSpeeds(const std::vector<Dislocation> &dis, std::vector<double> &res)
{
    std::fill(res.begin(), res.end(), 0);

    for (unsigned int i = 0; i < sD->dc; i++)
    {
        for (unsigned int j = i+1; j < sD->dc; j++)
        {
            double dx = dis[i].x - dis[j].x;
            normalize(dx);

            double dy = dis[i].y - dis[j].y;
            normalize(dy);

            double tmp = dis[i].b * dis[j].b * sD->tau->xy(dx, dy);

            double r2 = dx*dx+dy*dy;
            pH->updateTolerance(r2, i);
            pH->updateTolerance(r2, j);

            res[i] +=  tmp;
            res[j] -=  tmp;
        }

        for (size_t j = 0; j < sD->pc; j++)
        {
            double dx = dis[i].x - sD->points[j].x;
            normalize(dx);

            double dy = dis[i].y - sD->points[j].y;
            normalize(dy);

            double xSqr = X2(dx);
            double ySqr = X2(dy);
            double rSqr = xSqr + ySqr;
            double expXY = exp(-sD->KASQR * rSqr);
            res[i] -= 2.0 * sD->A * X(dx) * X(dy) * ((1.-expXY)/rSqr- sD->KASQR * expXY) / rSqr * dis[i].b;

            pH->updateTolerance(rSqr, i);
        }
        res[i] += dis[i].b * sD->externalStressProtocol->getStress(sD->currentStressStateType);
    }
}

void Simulation::calculateG(const double &stepsize, std::vector<Dislocation> &newDislocation, const std::vector<Dislocation> &old,
                            bool useSpeed2, bool calculateInitSpeed, bool useInitSpeedForFirstStep, sdddstCore::StressProtocolStepType origin, sdddstCore::StressProtocolStepType end)
{
    std::vector<double> * isp = &(sD->initSpeed);
    std::vector<double> * csp = &(sD->speed);
    if (useSpeed2)
    {
        isp = &(sD->initSpeed2);
        csp = &(sD->speed2);
    }

    if (calculateInitSpeed)
    {
        double t = sD->simTime;
        if (origin == sdddstCore::StressProtocolStepType::EndOfFirstSmallStep)
        {
            t += sD->stepSize * 0.5;
        }

        sD->externalStressProtocol->calculateStress(t, old, origin);
        sD->currentStressStateType = origin;
        calculateSpeeds(old, *isp);
    }

    if (useInitSpeedForFirstStep)
    {
        csp = isp;
    }
    else
    {
        double t = sD->simTime + sD->stepSize;
        if (end == EndOfFirstSmallStep)
        {
            t -= sD->stepSize * 0.5;
        }
        sD->externalStressProtocol->calculateStress(t, newDislocation, end);
        sD->currentStressStateType = end;
        calculateSpeeds(newDislocation, *csp);
    }

    for (size_t i = 0; i < sD->dc; i++)
    {
        sD->g[i] = newDislocation[i].x - (1.0+sD->dVec[i]) * 0.5 * stepsize * (*csp)[i] - old[i].x - (1.0 - sD->dVec[i]) * 0.5 * stepsize * (*isp)[i];
    }
}

double Simulation::getElement(int j, int si, int ei)
{
    int len = ei - si;
    if (len > 1)
    {
        int tmp = len /2;
        double a;

        if (sD->Ai[si+tmp] > j)
        {
            a = getElement(j, si, si + tmp);
            if (a != 0.0)
            {
                return a;
            }
        }
        else
        {
            a = getElement(j, si + tmp, ei);
            if (a != 0.0)
            {
                return a;
            }
        }
    }
    else
    {
        if (sD->Ai[si] == j)
        {
            return sD->Ax[si];
        }
        return 0;
    }
    return 0;
}

double Simulation::getSimTime()
{
    return sD->simTime;
}

void Simulation::calculateJacobian(const double & stepsize, const std::vector<Dislocation> & data)
{
    int totalElementCounter = 0;

    for (unsigned int j = 0; j < sD->dc; j++)
    {
        if (sD->currentStorageSize - totalElementCounter < sD->dc)
        {
               double * tmp = static_cast<double*>(realloc(sD->Ax, (sD->currentStorageSize + sD->dc) * sizeof(double)));
               //std::cout << "Used percent: " << double(sD->currentStorageSize) / double(sD->dc) / double(sD->dc) << std::endl;
               if (tmp == nullptr)
               {
                   std::cerr << "Out of memory to allocate more memory. Exit to prevent corrupted data." << std::endl;
                   exit(-4);
               }

               sD->Ax = tmp;
               sD->currentStorageSize += sD->dc;
               for (unsigned int i = totalElementCounter; i < sD->currentStorageSize; i++)
               {
                   sD->Ax[i] = 0.0;
               }
        }
        // Previously calculated part
        for (unsigned int i = 0; i < j; i++)
        {
            double v = getElement(j, sD->Ap[i], sD->Ap[i+1]);
            if (v != 0.0)
            {
                sD->Ai[totalElementCounter] = i;
                sD->Ax[totalElementCounter++] = v;
            }
        }
        // Add the diagonal element (it will be calculated later and the point defects now)
        sD->Ai[totalElementCounter] = j;
        double tmp = 0;
        double dx;
        double dy;
        for (size_t l = 0; l < sD->pc; l++)
        {
            dx = data[j].x - sD->points[l].x;
            normalize(dx);
            dy = data[j].y - sD->points[l].y;
            normalize(dy);

            if (pow(sqrt(dx * dx + dy * dy) - sD->cutOff, 2) < 36.8 * sD->cutOffSqr)
            {
                double multiplier = 1;
                if (dx * dx + dy * dy > sD->cutOffSqr)
                {
                    multiplier = exp(-pow(sqrt(dx*dx+dy*dy)-sD->cutOff, 2) * sD->onePerCutOffSqr);
                }
                tmp -= data[j].b * (- sD->A * cos(0.2e1 * M_PI * dx) / M_PI * sin(0.2e1 * M_PI * dy) * ((0.1e1 - pow(M_E, -sD->KASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                        (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1))) /
                                                                                                        ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                         pow(M_PI, -0.2e1) / 0.2e1) - sD->KASQR * pow(M_E, -sD->KASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                         pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                                         (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                         pow(M_PI, -0.2e1) / 0.2e1))) /
                                    ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1)
                                    - sD->A * sin(0.2e1 * M_PI * dx) * pow(M_PI, -0.2e1) * sin(0.2e1 * M_PI * dy) * (pow(M_E, -sD->KASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                            (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1)) *
                                                                                                                     sD->KASQR * sin(0.2e1 * M_PI * dx) / M_PI * log(M_E) / ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                             pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                                             (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                             pow(M_PI, -0.2e1) / 0.2e1) -
                                                                                                                     (0.1e1 - pow(M_E, -sD->KASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) /
                                                                                                                                                     0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                     pow(M_PI, -0.2e1) / 0.2e1))) *
                                                                                                                     pow((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                         (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1, -0.2e1) *
                                                                                                                     sin(0.2e1 * M_PI * dx) / M_PI + sD->KASQR * sD->KASQR * pow(M_E, -sD->KASQR *
                                                                                                                                                                                 ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                                  pow(M_PI, -0.2e1) /
                                                                                                                                                                                  0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                                  pow(M_PI, -0.2e1) / 0.2e1)) *
                                                                                                                     sin(0.2e1 * M_PI * dx) / M_PI * log(M_E)) / ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) /
                                                                                                                                                                  0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                  pow(M_PI, -0.2e1) / 0.2e1) / 0.2e1 + sD->A *
                                    pow(sin(0.2e1 * M_PI * dx), 0.2e1) * pow(M_PI, -0.3e1) * sin(0.2e1 * M_PI * dy) * ((0.1e1 - pow(M_E, -sD->KASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                       (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1))) /
                                                                                                                       ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                        pow(M_PI, -0.2e1) / 0.2e1) - sD->KASQR * pow(M_E, -sD->KASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                                        pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                                                        (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                                        pow(M_PI, -0.2e1) / 0.2e1))) *
                                    pow((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1, -0.2e1) / 0.2e1) *  multiplier;
            }
        }
        sD->Ax[totalElementCounter++] = - tmp * stepsize;
        // Totally new part
        for (unsigned int i = j+1; i < sD->dc; i++)
        {
            dx = data[i].x - data[j].x;
            normalize(dx);

            dy = data[i].y - data[j].y;
            normalize(dy);

            if (pow(sqrt(dx * dx + dy * dy) - sD->cutOff, 2) < 36.8 * sD->cutOffSqr)
            {
                double multiplier = 1;
                if (dx * dx + dy * dy > sD->cutOffSqr)
                {
                    multiplier = exp(-pow(sqrt(dx*dx+dy*dy)-sD->cutOff, 2) * sD->onePerCutOffSqr);
                }
                sD->Ai[totalElementCounter] = i;
                sD->Ax[totalElementCounter++] = stepsize * data[i].b * data[j].b * sD->tau->xy_diff_x(dx, dy) * multiplier;
            }
        }
        sD->Ap[j+1] = totalElementCounter;
    }

    for (unsigned int j = 0; j < sD->dc; j++)
    {
        double subSum = 0;
        for (int i = sD->Ap[j]; i < sD->Ap[j+1]; i++)
        {
            if (sD->Ai[i] == int(j))
            {
                sD->indexes[j] = i;
            }
            subSum += sD->Ax[i];
        }

        subSum *= -1.;
        sD->Ax[sD->indexes[j]] = subSum;
        if (subSum > 0)
        {
            subSum = 1./subSum;
            subSum += 1.;
            subSum *= subSum;
            sD->dVec[j] = 1./subSum;
        }
        else
        {
            sD->dVec[j] = 0.;
        }
    }

    for (unsigned int j = 0; j < sD->dc; j++)
    {
        for (int i = sD->Ap[j]; i < sD->Ap[j+1]; i++)
        {
            sD->Ax[i] *= (1.0+sD->dVec[sD->Ai[i]]) * 0.5;
        }
        sD->Ax[sD->indexes[j]] += 1.0;
    }
}

void Simulation::calculateSparseFormForJacobian()
{
    (void) umfpack_di_symbolic (sD->dc, sD->dc, sD->Ap, sD->Ai, sD->Ax, &(sD->Symbolic), sD->null, sD->null);
    (void) umfpack_di_numeric (sD->Ap, sD->Ai, sD->Ax, sD->Symbolic, &(sD->Numeric), sD->null, sD->null);
    umfpack_di_free_symbolic (&(sD->Symbolic));
}

void Simulation::solveEQSys()
{
    (void) umfpack_di_solve (UMFPACK_A, sD->Ap, sD->Ai, sD->Ax, sD->x, sD->g.data(), sD->Numeric, sD->null, sD->null) ;
}

void Simulation::calculateXError()
{
    for (size_t i = 0; i < sD->dc; i++)
    {
        double tmp = fabs(sD->bigStep[i].x - sD->secondSmall[i].x);
        pH->updateError(tmp, i);
    }
}

double Simulation::calculateOrderParameter(const std::vector<double> &speeds)
{
    double orderParameter = 0;
    for (size_t i = 0; i < sD->dc; i++)
    {
        orderParameter += sD->dislocations[i].b * speeds[i];
    }
    return orderParameter;
}

double Simulation::calculateStrainIncrement(const std::vector<Dislocation> &old, const std::vector<Dislocation> &newD)
{
    double result = 0;
    for (size_t i = 0; i < old.size(); i++)
    {
        result += newD[i].b * (newD[i].x - old[i].x);
    }
    return result;
}

void Simulation::run()
{
    while( ((sD->isTimeLimit && sD->simTime < sD->timeLimit) || !sD->isTimeLimit) &&
           ((sD->isStrainIncreaseLimit && sD->totalAccumulatedStrainIncrease < sD->totalAccumulatedStrainIncreaseLimit) || !sD->isStrainIncreaseLimit) &&
           ((sD->isStepCountLimit && sD->succesfulSteps < sD->stepCountLimit) || !sD->isStepCountLimit) &&
           ((sD->countAvalanches && sD->avalancheCount < sD->avalancheTriggerLimit) || !sD->countAvalanches) &&
           ((sD->isSpeedLimit && sD->sumAvgSpeed < sD->speedLimit) || !sD->isSpeedLimit)
           )
    {
        step();
    }
    sD->writeDislocationDataToFile(sD->endDislocationConfigurationPath);
}

bool Simulation::step()
{
    stepStageI();
    stepStageII();
    stepStageIII();
    return succesfulStep;
}

void Simulation::stepStageI()
{
    energyAccum = 0;

    double sumAvgSp = 0;
    vsquare = 0;
    sD->currentStressStateType = sdddstCore::StressProtocolStepType::Original;
    if (firstStepRequest)
    {
        lastWriteTimeFinished = get_wall_time();
        sD->externalStressProtocol->calculateStress(sD->simTime, sD->dislocations, sdddstCore::StressProtocolStepType::Original);
        calculateSpeeds(sD->dislocations, sD->initSpeed);
        initSpeedCalculationIsNeeded = false;
        sumAvgSp = std::accumulate(sD->initSpeed.begin(), sD->initSpeed.end(), 0.0, [](double a, double b){return a + fabs(b);}) / double(sD->dc);
        vsquare = std::accumulate(sD->initSpeed.begin(), sD->initSpeed.end(), 0.0, [](double a, double b){return a + b*b;});

        // First log line
        sD->standardOutputLog <<
                                 sD->simTime << " " <<
                                 sD->succesfulSteps << " " <<
                                 sD->failedSteps << " " <<
                                 0 << " " <<
                                 sumAvgSp << " " <<
                                 sD->cutOff << " " <<
                                 "-" << " " <<
                                 sD->externalStressProtocol->getStress(sD->currentStressStateType) << " " <<
                                 "-" << " " <<
                                 sD->totalAccumulatedStrainIncrease << " " <<
                                 vsquare << " " <<
                                 energy << "\n";

        firstStepRequest = false;
    }

    // Reset the variables for the integration
    sD->bigStep = sD->dislocations;


    /////////////////////////////////
    /// Integrating procedure begins

    integrate(sD->stepSize, sD->bigStep, sD->dislocations, false, initSpeedCalculationIsNeeded, Original, EndOfBigStep);

    // This can not get before the first integration step
    succesfulStep = false;
}

void Simulation::stepStageII()
{
    sD->firstSmall = sD->dislocations;

    integrate(0.5*sD->stepSize, sD->firstSmall, sD->dislocations, false, false, Original, EndOfFirstSmallStep);
}

void Simulation::stepStageIII()
{
    sD->sumAvgSpeed = 0;
    sD->secondSmall = sD->firstSmall;

    integrate(0.5 * sD->stepSize, sD->secondSmall, sD->firstSmall, true, true, EndOfFirstSmallStep, EndOfSecondSmallStep);

    vsquare1 = std::accumulate(sD->initSpeed.begin(), sD->initSpeed.end(), 0.0, [](double a, double b){return a + b*b;});
    vsquare2 = std::accumulate(sD->initSpeed2.begin(), sD->initSpeed2.end(), 0.0, [](double a, double b){return a + b*b;});

    energyAccum = (vsquare1+vsquare2) * 0.5 * sD->stepSize * 0.5;

    calculateXError();

    /// Precision related error handling
    if (pH->getMaxErrorRatioSqr() < 1.0)
    {
        succesfulStep = true;
        initSpeedCalculationIsNeeded = true;

        if (sD->calculateStrainDuringSimulation)
        {
            sD->totalAccumulatedStrainIncrease += calculateStrainIncrement(sD->dislocations, sD->firstSmall);
            sD->totalAccumulatedStrainIncrease += calculateStrainIncrement(sD->firstSmall, sD->secondSmall);
        }

        sD->dislocations.swap(sD->secondSmall);

        sD->simTime += sD->stepSize;
        sD->succesfulSteps++;

        double orderParameter = 0;
        if (sD->orderParameterCalculationIsOn)
        {
            orderParameter = calculateOrderParameter(sD->speed);
        }

        double current_wall_time = get_wall_time();

        sD->currentStressStateType = Original;
        sD->externalStressProtocol->calculateStress(sD->simTime, sD->dislocations, Original);
        calculateSpeeds(sD->dislocations, sD->initSpeed);
        initSpeedCalculationIsNeeded = false;
        sD->sumAvgSpeed = std::accumulate(sD->initSpeed.begin(), sD->initSpeed.end(), 0.0, [](double a, double b){return a + fabs(b);}) / double(sD->dc);
        vsquare = std::accumulate(sD->initSpeed.begin(), sD->initSpeed.end(), 0.0, [](double a, double b){return a + b*b;});

        if (sD->countAvalanches)
        {
            if (sD->inAvalanche && sD->sumAvgSpeed < sD->avalancheSpeedThreshold)
            {
                sD->avalancheCount++;
                sD->inAvalanche = false;
            }
            else if (sD->sumAvgSpeed > sD->avalancheSpeedThreshold)
            {
                sD->inAvalanche = true;
            }
        }

        energyAccum += (vsquare2+vsquare)* 0.5 * sD->stepSize * 0.5;
        sD->standardOutputLog <<
                                 sD->simTime << " " <<
                                 sD->succesfulSteps << " " <<
                                 sD->failedSteps << " " <<
                                 pH->getMaxErrorRatioSqr() << " " <<
                                 sD->sumAvgSpeed << " " <<
                                 sD->cutOff << " ";

        if (sD->orderParameterCalculationIsOn)
        {
            sD->standardOutputLog << orderParameter;
        }
        else
        {
            sD->standardOutputLog << "-";
        }


        sD->standardOutputLog << " " << sD->externalStressProtocol->getStress(Original) << " " << current_wall_time - lastWriteTimeFinished;

        if (sD->calculateStrainDuringSimulation)
        {
            sD->standardOutputLog << " " << sD->totalAccumulatedStrainIncrease;
        }
        else
        {
            sD->standardOutputLog << " -";
        }

        if (sD->isSpeedThresholdForCutoffChange && sD->speedThresholdForCutoffChange > sD->sumAvgSpeed)
        {
            sD->cutOffMultiplier = 1e20;
            sD->updateCutOff();
        }

        energy += energyAccum;

        sD->standardOutputLog << " " << vsquare << " " << energy;

        sD->standardOutputLog << "\n";

        if (sD->isSaveSubConfigs)
        {
            if ((!sD->inAvalanche && sD->subConfigDelay >= sD->subconfigDistanceCounter) || (sD->inAvalanche && sD->subConfigDelayDuringAvalanche >= sD->subconfigDistanceCounter))
            {
                sD->subconfigDistanceCounter = 0;
                std::stringstream ss;
                ss << std::setprecision(16);
                ss << sD->simTime;
                sD->writeDislocationDataToFile(sD->subConfigPath + "/" + ss.str() + ".dconf");
            }
            else
            {
                sD->subconfigDistanceCounter++;
            }
        }

        lastWriteTimeFinished = get_wall_time();

    }
    else
    {
        sD->failedSteps++;
    }

    sD->stepSize = pH->getNewStepSize(sD->stepSize);
    pH->reset();

    if (sD->isMaxStepSizeLimit && sD->maxStepSizeLimit < sD->stepSize)
    {
        sD->stepSize = sD->maxStepSizeLimit;
    }
}

const std::vector<Dislocation> &Simulation::getStoredDislocationData()
{
    return sD->dislocations;
}

#ifdef BUILD_PYTHON_BINDINGS
Simulation *Simulation::create(boost::python::object simulationData)
{
    boost::python::extract<PySdddstCore::PySimulationData&> x(simulationData);
    if (x.check())
    {
        PySdddstCore::PySimulationData& tmp = x();
        return new Simulation{tmp.get()};
    }
    return nullptr;
}
#endif
