#include "time_series_processor.h"
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "spectral_decompositor.h"
#include "Fields/AnalyticField.h"
sdddstEV::TimeSeriesProcessor::TimeSeriesProcessor(std::shared_ptr<sdddstCore::SimulationData> simData, std::shared_ptr<DataTimeSeries> dateTimeSeries):
    sD(simData),
    dTS(dateTimeSeries)
{

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

    int tmpc = 0;
    while (dTS->next(sD)) {
        std::cout << sD->simTime << "\n";
        decomposer.decompose(sD->dislocations, sD->points);
        out << sD->simTime;
        for (int i = 0; i < decomposer.getDislocationCount(); i++) {
            std::cout << " " << decomposer.getEigenValue(i);
        }
        out << "\n";
        if (tmpc++ == 5) {
            break;
        }
    }

    boost::iostreams::close(outbuf);
    file.close();
}
