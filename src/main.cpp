/**
 * @file   main.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2016.11.24
 *
 * @brief  Program entry point.
 */

#include "core/main.h"
#include <fstream>
#include <iostream>
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include "core/g3log/filesink.h"
#include "core/ApplicationNodeInternal.h"
#include "core/initialize.h"


int main(int argc, char** argv)
{
    const std::string directory = "./";
    const std::string name = "viscomlabfw.log";
    auto worker = g3::LogWorker::createLogWorker();
    auto handle = worker->addSink(std::make_unique<vku::FileSink>(name, directory, false), &vku::FileSink::fileWrite);

    g3::log_levels::disable(WARNING);

#ifdef WIN_NO_ERROR_REPORT
    //don't open error reporting, let it crash. kthx
    DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
    SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);
#endif
#ifndef NDEBUG
    g3::log_levels::enable(WARNING);
#endif

    initializeLogging(worker.get());

    LOG(INFO) << "Log created.";

    viscom::FWConfiguration config;
    if (argc > 1) config = viscom::LoadConfiguration(argv[1]);
    else config = viscom::LoadConfiguration("framework.cfg");
    config.resourceSearchPaths_.emplace_back(config.baseDirectory_ + "extern/fwenh/resources/");
    if(config.openglProfile_.at(0) != '4') {
        LOG(WARNING) << "at least opengl v4 necessary";
        std::cout << "at least opengl v4 necessary";
        return 0;
    }
    auto appNode = Application_Init(config);

    // Main loop
    LOG(INFO) << "Started Rendering.";
    appNode->Render();

    return 0;
}
