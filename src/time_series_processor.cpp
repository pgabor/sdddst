#include "time_series_processor.h"
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <StressProtocols/fixed_rate_protocol.h>
#include "spectral_decompositor.h"
#include "Fields/AnalyticField.h"
#include "simulation.h"
#include "utility.h"
sdddstEV::TimeSeriesProcessor::TimeSeriesProcessor(std::shared_ptr<sdddstCore::SimulationData> simData, std::shared_ptr<DataTimeSeries> dateTimeSeries, sdddstCore::StressProtocol *stressProtocol):
    sD(simData),
    dTS(dateTimeSeries)
{
    sD->tau.reset(new sdddstCore::AnalyticField);
    sD->externalStressProtocol.reset(stressProtocol);
    sD->standardOutputLog = std::ofstream("/dev/null");
    sim.reset(new sdddstCore::Simulation(sD));
}

void sdddstEV::TimeSeriesProcessor::run()
{
    sdddstCore::Field * field = new sdddstCore::AnalyticField();
    sdddstEV::SpectralDecompositor decomposer(field);
    std::ofstream file(sD->eVAnalResultFile, std::ios_base::out | std::ios_base::binary);
    boost::iostreams::filtering_streambuf<boost::iostreams::output> outbuf;
    outbuf.push(boost::iostreams::gzip_compressor());
    outbuf.push(file);
    std::ostream out(&outbuf);

    std::vector<double> speeds;

    while (dTS->next(sD)) {
        speeds.resize(sD->dc);
        sD->externalStressProtocol->calculateStress(sD->simTime, sD->dislocations, sdddstCore::StressProtocolStepType::Original);

        std::cout << sD->simTime << "\n";
        decomposer.decompose(sD->dislocations, sD->points);
        out << sD->simTime;

        if (sD->writeCorrelMatrices == 0) {
            // EV
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                out << " " << decomposer.getEigenValue(i);
            }

            // PN
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                double sum = 0;
                for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                    sum += pow(decomposer.getEigenVectorElement(i, j), 4.0);
                }
                out << " " << 1.0 / sum;
            }

            // 1/Gael (GAN)
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                double sum = 0;
                for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                    sum += decomposer.getEigenVectorElement(i, j) * sD->dislocations[j].b;
                }
                out << " " << pow(sum, 2.0) / decomposer.getEigenValue(i);
            }

            // Speeds
            sim->calculateSpeeds(sD->dislocations, speeds, true);
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                out << " " << speeds[i];
            }

            // X coordinates
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                out << " " << sD->dislocations[i].x;
            }

            // SUM
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                double sum = 0;
                for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                    sum += decomposer.getEigenVectorElement(i, j) * sD->dislocations[j].b;
                }
                out << " " << sum;
            }

            // Eigen vectors
            for (int i = 0; i < sD->numberOfEigenVecToWrite && i < decomposer.getDislocationCount(); i++) {
                for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                    out << " " << decomposer.getEigenVectorElement(i, j);
                }
            }

            if (sD->calculateDerivativeEVAnal) {
                // Eigen value derivatives
                for (int n = 0; n < decomposer.getDislocationCount(); n++) {
                    double sum = 0;
                    if (sD->externalStressProtocol->getType() == "fixed-rate-stress") {
                        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                            for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                                double multiplier = decomposer.getEigenVectorElement(n, i) * decomposer.getEigenVectorElement(n, j) / sD->externalStressProtocol->getStressDerivative(sD->simTime);
                                double subsum = 0;
                                for (int k = 0; k < decomposer.getDislocationCount(); k++) {
                                    subsum += speeds[k] * calculateHessianDerivative(i, j, k, sD->dislocations);
                                }
                                sum += subsum * multiplier;
                            }
                        }
                    }
                    out << " " << sum;
                }
            }
            out << "\n";
        } else {
            // EV
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                out << " " << decomposer.getEigenValue(i);
            }

            // PN
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                double sum = 0;
                for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                    sum += pow(decomposer.getEigenVectorElement(i, j), 4.0);
                }
                out << " " << 1.0 / sum;
            }

            // 1/Gael (GAN)
            for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                double sum = 0;
                for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                    sum += decomposer.getEigenVectorElement(i, j) * sD->dislocations[j].b;
                }
                out << " " << pow(sum, 2.0) / decomposer.getEigenValue(i);
            }
            out << "\n";

            std::vector<std::vector<double>> pp;
            std::vector<std::vector<double>> pn;
            std::vector<std::vector<double>> pppn;
            int ppc = 0;
            int pnc = 0;
            int pppnc = 0;

            pp.resize(sD->writeCorrelMatrices);
            pn.resize(sD->writeCorrelMatrices);
            pppn.resize(sD->writeCorrelMatrices);

            for (int i = 0; i < sD->writeCorrelMatrices; i++) {
                pp[i].resize(sD->writeCorrelMatrices);
                pn[i].resize(sD->writeCorrelMatrices);
                pppn[i].resize(sD->writeCorrelMatrices);
            }
            for (int mode = 0; mode < decomposer.getDislocationCount(); mode++) {
                for (int i = 0; i < sD->writeCorrelMatrices; i++) {
                    for (int j = 0; j < sD->writeCorrelMatrices; j++) {
                        pp[i][j] = 0.0;
                        pn[i][j] = 0.0;
                        pppn[i][j] = 0.0;
                    }
                }
                for (int i = 0; i < decomposer.getDislocationCount(); i++) {
                    for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                        if (i == j) continue;
                        double dx = sD->dislocations[i].x - sD->dislocations[j].x;
                        double dy = sD->dislocations[i].y - sD->dislocations[j].y;
                        normalize(dx);
                        normalize(dy);
                        int t_x = int(floor((dx+0.5) * sD->writeCorrelMatrices));
                        int t_y = int(floor((dy+0.5) * sD->writeCorrelMatrices));
                        double s1 = sD->dislocations[i].b;
                        double s2 = sD->dislocations[j].b;
                        if (s1 != s2) {
                            pn[t_x][t_y] += pow(decomposer.getEigenVectorElement(mode, j), 2.0) * pow(decomposer.getEigenVectorElement(mode, i), 2.0);
                            pnc++;
                        }
                        if (s1 == s2) {
                            pp[t_x][t_y] += pow(decomposer.getEigenVectorElement(mode, j), 2.0) * pow(decomposer.getEigenVectorElement(mode, i), 2.0);
                            ppc++;
                        }
                        pppn[t_x][t_y] += pow(decomposer.getEigenVectorElement(mode, j), 2.0) * pow(decomposer.getEigenVectorElement(mode, i), 2.0);
                        pppnc++;
                    }
                }
                for (int i = 0; i < pp.size(); i++) {
                    for (int j =0; j < pp.size(); j++) {
                        out << pp[i][j] << " ";
                    }
                    out << "\n";
                }

                for (int i = 0; i < pp.size(); i++) {
                    for (int j =0; j < pp.size(); j++) {
                        out << pn[i][j] << " ";
                    }
                    out << "\n";
                }

                for (int i = 0; i < pp.size(); i++) {
                    for (int j =0; j < pp.size(); j++) {
                        out << pppn[i][j] << " ";
                    }
                    out << "\n";
                }
            }
        }
    }

    boost::iostreams::close(outbuf);
    file.close();
}

double sdddstEV::TimeSeriesProcessor::calculateHessianDerivative(int i, int j, int k, std::vector<sdddstCore::Dislocation> & dislocations)
{
    if (i == j && j == k) {
        double sum = 0;
        for (int l = 0; l < dislocations.size(); l++) {
            if (l != i) {
                double dx = dislocations[i].x - dislocations[l].x;
                double dy = dislocations[i].y - dislocations[l].y;
                normalize(dx);
                normalize(dy);
                sum += dislocations[l].b * sD->tau->xy_diff_x2(dx, dy);
            }
        }
        return - dislocations[i].b * sum;
    } else if (i == j && j != k)
    {
        double dx = dislocations[i].x - dislocations[k].x;
        double dy = dislocations[i].y - dislocations[k].y;
        normalize(dx);
        normalize(dy);
        return dislocations[i].b * dislocations[k].b * sD->tau->xy_diff_x2(dx, dy);
    } else if (i == k && k != j) {
        double dx = dislocations[i].x - dislocations[j].x;
        double dy = dislocations[i].y - dislocations[j].y;
        normalize(dx);
        normalize(dy);
        return dislocations[i].b * dislocations[j].b * sD->tau->xy_diff_x2(dx, dy);
    } else if (j == k && k != i) {
        double dx = dislocations[i].x - dislocations[j].x;
        double dy = dislocations[i].y - dislocations[j].y;
        normalize(dx);
        normalize(dy);
        return - dislocations[i].b * dislocations[j].b * sD->tau->xy_diff_x2(dx, dy);
    }
    return 0;
}
