#include "client/audio_sender_and_splitter.cpp"
#include "client/client_utils.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace vad_client
{
    void runMain(int argc, char *argv[])
    {
        po::options_description desc("speech to text tool, asr_tool <model-path> <file-to-split-path> <output-directory> [options]");
        desc.add_options()
            ("help", "produce help message")
            ("verbose", "run in verbose mode")
            ("chunk-size-msec", po::value<int>(), "audio chunk size to push")
            ;

        // these next two lines get all the non-named options.
        po::parsed_options parsed =
            po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();

        std::vector<string> args = po::collect_unrecognized(parsed.options, po::include_positional);
        // this section populates the named args
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        // we need to call notify, or else errors in options will not be noticed.
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return;
        }
        int chunkSizeMsec = 200;
        if ( vm.count("chunk-size-msec") )
        {
            chunkSizeMsec = vm["chunk-size-msec"].as<int>();
        }
        if (args.size() < 3)
        {
            std::stringstream ss;
            ss << "Incorrect Command-line Usage. " << desc;
            throw std::runtime_error(ss.str());
        }
        const string modelPath = args[0];
        const string fileToSplit = args[1];
        const string outputDir = args[2];

        checkApiReturn(Api_NewModel(modelPath.c_str(), modelPath.c_str()));
        AudioSenderAndSplitter senderAndSplitter(fileToSplit,
                modelPath,
                outputDir,
                chunkSizeMsec);
        senderAndSplitter.sendAndSplit();
        checkApiReturn(Api_DeleteModel(modelPath.c_str()));
    }
}
int main(int argc, char *argv[])
{
    try
    {
        vad_client::runMain(argc, argv);
    }
    catch (std::runtime_error &e)
    {
        cout << "Caught C++ runtime error: " << e.what() << endl;
        return 1;
    }
    catch ( ... )
    {
        cout << "Caught unknown error!!";
        return 2;
    }
    return 0;
}
