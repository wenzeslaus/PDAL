/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
* Copyright (c) 2014-2015, Bradley J Chambers (brad.chambers@gmail.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include "GroundKernel.hpp"

#include <pdal/KernelFactory.hpp>
#include <pdal/KernelSupport.hpp>
#include <pdal/Options.hpp>
#include <pdal/pdal_macros.hpp>
#include <pdal/PointBuffer.hpp>
#include <pdal/PointContext.hpp>
#include <pdal/Stage.hpp>
#include <pdal/StageFactory.hpp>

#include <memory>
#include <string>
#include <vector>

CREATE_KERNEL_PLUGIN(ground, pdal::GroundKernel)

namespace pdal
{

namespace
{

static std::unique_ptr<Stage> makeReader(Options options, std::string filename, uint32_t verbosity, bool debug=true)
{
    if (debug)
    {
        options.add<bool>("debug", true);
        if (!verbosity)
            verbosity = 1;

        options.add<uint32_t>("verbose", verbosity);
        options.add<std::string>("log", "STDERR");
    }

    Stage* stage = KernelSupport::makeReader(filename);
    stage->setOptions(options);
    std::unique_ptr<Stage> reader_stage(stage);

    return reader_stage;
}

static std::string s_inputFile;
static std::string s_outputFile;
static double s_maxWindowSize = 33;
static double s_slope = 1;
static double s_maxDistance = 2.5;
static double s_initialDistance = 0.15;
static double s_cellSize = 1;
static bool s_classify = false;
static bool s_extract = false;

}

void GroundKernel::validateSwitches()
{
    if (s_inputFile == "")
    {
        throw app_usage_error("--input/-i required");
    }

    if (s_outputFile == "")
    {
        throw app_usage_error("--output/-o required");
    }
}

void GroundKernel::addSwitches()
{
    po::options_description* file_options = new po::options_description("file options");

    file_options->add_options()
    ("input,i", po::value<std::string>(&s_inputFile)->default_value(""), "input file name")
    ("output,o", po::value<std::string>(&s_outputFile)->default_value(""), "output file name")
    ("maxWindowSize", po::value<double>(&s_maxWindowSize)->default_value(33), "max window size")
    ("slope", po::value<double>(&s_slope)->default_value(1), "slope")
    ("maxDistance", po::value<double>(&s_maxDistance)->default_value(2.5), "max distance")
    ("initialDistance", po::value<double>(&s_initialDistance)->default_value(0.15, "0.15"), "initial distance")
    ("cellSize", po::value<double>(&s_cellSize)->default_value(1), "cell size")
    ("classify", po::bool_switch(&s_classify), "apply classification labels?")
    ("extract", po::bool_switch(&s_extract), "extract ground returns?")
    ;

    addSwitchSet(file_options);

    addPositionalSwitch("input", 1);
    addPositionalSwitch("output", 1);
}

int GroundKernel::execute()
{
    PointContext ctx;

    Options readerOptions;
    readerOptions.add<std::string>("filename", s_inputFile);
    readerOptions.add<bool>("debug", isDebug());
    readerOptions.add<uint32_t>("verbose", getVerboseLevel());

    std::unique_ptr<Stage> readerStage = makeReader(readerOptions, s_inputFile, getVerboseLevel(), isDebug());

    Options groundOptions;
    groundOptions.add<double>("maxWindowSize", s_maxWindowSize);
    groundOptions.add<double>("slope", s_slope);
    groundOptions.add<double>("maxDistance", s_maxDistance);
    groundOptions.add<double>("initialDistance", s_initialDistance);
    groundOptions.add<double>("cellSize", s_cellSize);
    groundOptions.add<bool>("classify", s_classify);
    groundOptions.add<bool>("extract", s_extract);

    StageFactory f;
    std::unique_ptr<Filter> groundStage(f.createFilter("filters.ground"));
    groundStage->setOptions(groundOptions);
    groundStage->setInput(readerStage.get());

    // setup the Writer and write the results
    Options writerOptions;
    writerOptions.add<std::string>("filename", s_outputFile);
    setCommonOptions(writerOptions);

    std::unique_ptr<Writer> writer(KernelSupport::makeWriter(s_outputFile, groundStage.get()));
    writer->setOptions(writerOptions);

    std::vector<std::string> cmd = getProgressShellCommand();
    UserCallback *callback =
        cmd.size() ? (UserCallback *)new ShellScriptCallback(cmd) :
        (UserCallback *)new HeartbeatCallback();

    writer->setUserCallback(callback);

    for (const auto& pi: getExtraStageOptions())
    {
        std::string name = pi.first;
        Options options = pi.second;
        std::vector<Stage*> stages = writer->findStage(name);
        for (const auto& s : stages)
        {
            Options opts = s->getOptions();
            for (const auto& o : options.getOptions())
                opts.add(o);
            s->setOptions(opts);
        }
    }

    writer->prepare(ctx);

    // process the data, grabbing the PointBufferSet for visualization of the
    // resulting PointBuffer
    PointBufferSet pbSetOut = writer->execute(ctx);

    if (isVisualize())
        visualize(*pbSetOut.begin());
    //visualize(*pbSetIn.begin(), *pbSetOut.begin());

    return 0;
}

} // namespace pdal

