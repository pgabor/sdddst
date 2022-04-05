#include "time_series_processor.h"
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "spectral_decompositor.h"
#include "Fields/AnalyticField.h"
#include "simulation.h"
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
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            out << " " << decomposer.getEigenValue(i);
        }
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            double sum = 0;
            for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                sum += pow(decomposer.getEigenVectorElement(i, j), 4.0);
            }
            out << " " << 1.0 / sum;
        }
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            double sum = 0;
            for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                sum += decomposer.getEigenVectorElement(i, j);
            }
            out << " " << decomposer.getEigenValue(i) / pow(sum, 2.0);
        }
        sim->calculateSpeeds(sD->dislocations, speeds, true);
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            out << " " << speeds[i];
        }
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            out << " " << sD->dislocations[i].x;
        }
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            double sum = 0;
            for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                sum += decomposer.getEigenVectorElement(i, j);
            }
            out << " " << sum;
        }
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < decomposer.getDislocationCount(); j++) {
                out << " " << decomposer.getEigenVectorElement(i, j);
            }
        }
        out << "\n";
    }

    boost::iostreams::close(outbuf);
    file.close();
}
