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

#include "spectral_decompositor.h"
#include "utility.h"

#include <iostream>
#include <algorithm>
#include <chrono>
#include <lapacke.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

sdddstEV::SpectralDecompositor::SpectralDecompositor(sdddstCore::Field *f):
    dislocationCount(0),
    fpCount(0),
    field(f),
    sdA(0),
    sdKASQR(0),
    matrix(nullptr),
    matrixSize(0),
    w(nullptr)
{
    // Nothing to do
}

sdddstEV::SpectralDecompositor::~SpectralDecompositor()
{
    if (matrix)
    {
        delete[] matrix;
    }
    matrix = nullptr;

    if (w)
    {
        delete[] w;
    }
    w = nullptr;
}

void sdddstEV::SpectralDecompositor::decompose(std::vector<sdddstCore::Dislocation> &dislocations, std::vector<sdddstCore::PointDefect> & pointDefects)
{
    if (dislocationCount != dislocations.size())
    {
        if (w)
        {
            delete[] w;
        }
        w = new double[dislocations.size()];
        eigenValues.resize(dislocations.size());
    }

    dislocationCount = dislocations.size();
    fpCount = pointDefects.size();

    if (fpCount > 0)
    {
        sdA = 1e-4 * 16.0 * 1./sqrt(fpCount);
        sdKASQR = 1.65*1.65*1e6 / 256.0 * double(dislocationCount);
    }
    else {
        sdA = 0;
        sdKASQR = 0;
    }

    if (matrixSize != dislocationCount * dislocationCount)
    {
        if (matrix)
        {
            delete[] matrix;
        }

        matrixSize = dislocationCount * dislocationCount;
        matrix = new double[matrixSize];
    }


    std::fill_n(matrix, matrixSize, 0);

    for (size_t i = 0; i < dislocationCount; i++)
    {
        double subSum = 0;
        for (size_t j = 0; j < i; j++)
        {
            double dx = dislocations[i].x - dislocations[j].x;
            normalize(dx);
            double dy = dislocations[i].y - dislocations[j].y;
            normalize(dy);

            double tmp = 0;
            tmp = dislocations[i].b * dislocations[j].b * field->xy_diff_x(dx, dy);

            subSum += tmp;
            matrix[i*dislocationCount+j] = tmp;
            matrix[j*dislocationCount+i] = tmp;
        }
        matrix[i*dislocationCount+i] -= subSum;
        double j = i;
        for (auto & fp: pointDefects)
        {
            double dx = dislocations[i].x - fp.x;
            normalize(dx);
            double dy = dislocations[i].y - fp.y;
            normalize(dy);

                matrix[i*dislocationCount+i] -= dislocations[j].b * (- sdA * cos(0.2e1 * M_PI * dx) / M_PI * sin(0.2e1 * M_PI * dy) * ((0.1e1 - pow(M_E, -sdKASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                        (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1))) /
                                                                                                        ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                         pow(M_PI, -0.2e1) / 0.2e1) - sdKASQR * pow(M_E, -sdKASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                         pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                                         (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                         pow(M_PI, -0.2e1) / 0.2e1))) /
                                    ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1)
                                    - sdA * sin(0.2e1 * M_PI * dx) * pow(M_PI, -0.2e1) * sin(0.2e1 * M_PI * dy) * (pow(M_E, -sdKASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                            (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1)) *
                                                                                                                     sdKASQR * sin(0.2e1 * M_PI * dx) / M_PI * log(M_E) / ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                             pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                                             (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                             pow(M_PI, -0.2e1) / 0.2e1) -
                                                                                                                     (0.1e1 - pow(M_E, -sdKASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) /
                                                                                                                                                     0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                     pow(M_PI, -0.2e1) / 0.2e1))) *
                                                                                                                     pow((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                         (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1, -0.2e1) *
                                                                                                                     sin(0.2e1 * M_PI * dx) / M_PI + sdKASQR * sdKASQR * pow(M_E, -sdKASQR *
                                                                                                                                                                                 ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                                  pow(M_PI, -0.2e1) /
                                                                                                                                                                                  0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                                  pow(M_PI, -0.2e1) / 0.2e1)) *
                                                                                                                     sin(0.2e1 * M_PI * dx) / M_PI * log(M_E)) / ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) /
                                                                                                                                                                  0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                  pow(M_PI, -0.2e1) / 0.2e1) / 0.2e1 + sdA *
                                    pow(sin(0.2e1 * M_PI * dx), 0.2e1) * pow(M_PI, -0.3e1) * sin(0.2e1 * M_PI * dy) * ((0.1e1 - pow(M_E, -sdKASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                       (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1))) /
                                                                                                                       ((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                        pow(M_PI, -0.2e1) / 0.2e1) - sdKASQR * pow(M_E, -sdKASQR * ((0.1e1 - cos(0.2e1 * M_PI * dx)) *
                                                                                                                                                                                        pow(M_PI, -0.2e1) / 0.2e1 +
                                                                                                                                                                                        (0.1e1 - cos(0.2e1 * M_PI * dy)) *
                                                                                                                                                                                        pow(M_PI, -0.2e1) / 0.2e1))) *
                                    pow((0.1e1 - cos(0.2e1 * M_PI * dx)) * pow(M_PI, -0.2e1) / 0.2e1 + (0.1e1 - cos(0.2e1 * M_PI * dy)) * pow(M_PI, -0.2e1) / 0.2e1, -0.2e1) / 0.2e1);
        }
    }

    for (size_t i = 0; i < dislocationCount; i++)
    {
        for (size_t j = i+1; j < dislocationCount; j++)
        {
            matrix[i*dislocationCount+i] -= matrix[j*dislocationCount+i];
        }
    }

   /* for (size_t i = 0; i < dislocationCount; i++)
    {
        for (size_t j = 0; j < dislocationCount; j++)
        {
            std::cout << matrix[i*dislocationCount+j] << " ";
        }
        std::cout << std::endl;
    }*/

    /** LAPACKE_dsyevd leaks memory :/ **/

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int fd[2];
    if (pipe(fd) < 0)
    {
        std::cerr << "Pipe can not be opened.\n";
        exit(-2);
    }

    int pid = fork();
    if (pid < 0)
    {
        std::cerr << "Error during forking" << std::endl;
        exit(-3);
    }

    /** CHILD **/
    if (pid == 0)
    {
        int info = LAPACKE_dsyevd(LAPACK_COL_MAJOR, 'v', 'U', dislocationCount, matrix, dislocationCount, w);
        if (info != 0) {
            std::cerr << "Lapacke error: " << info << std::endl;
            exit(2);
        }
        int n = write(fd[1], matrix, sizeof(double) * dislocationCount * dislocationCount);
        n = write(fd[1], w, sizeof(double) * dislocationCount);
        exit(0);
    }
    /** PARENT **/
    else if (pid > 0)
    {
        signal(SIGCHLD,SIG_IGN);
        close(fd[1]);
        int n = read(fd[0], matrix, sizeof(double) * dislocationCount * dislocationCount);
        n = read(fd[0], w, sizeof(double) * dislocationCount);
        for (size_t i = 0; i < dislocationCount; i++)
        {
            eigenValues[i].eigen = w[i];
            eigenValues[i].index = i;
        }
    }

    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
//    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;
}

double sdddstEV::SpectralDecompositor::getEigenValue(unsigned int i)
{
    return eigenValues[i].eigen;
}

double sdddstEV::SpectralDecompositor::getEigenVectorElement(int i, int j)
{
    return matrix[eigenValues[i].index*dislocationCount+j];
}

unsigned long sdddstEV::SpectralDecompositor::getFpCount() const
{
    return fpCount;
}

unsigned long sdddstEV::SpectralDecompositor::getDislocationCount() const
{
    return dislocationCount;
}
 
