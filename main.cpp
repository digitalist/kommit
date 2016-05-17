#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "./git.h"
using  std::string;
using  std::vector;
using  std::cout;

namespace po = boost::program_options;


namespace git = commonUtils::v1::git;
int main(int argc, char *argv[]) {
    po::options_description desc("Allowed options"); //init options descriptions
    desc.add_options()
            ("help", "produce help message")
//            ("compression", po::value<int>() "set compression level")
            ("path", po::value< vector<string> >(), "path")
            ("m", po::value<string>(), "message") //message
    ;

    po::positional_options_description p;
    p.add("path", -1);

    //finally parse the command line to a map
    po::variables_map vm; //options are mapped here
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);  //positional options mapped
    po::notify(vm);

    std::string path = ".";
    std::string defaultCommitMessage = "doing";

    if (vm.count("path")) {
        path = vm["path"].as< vector<string> >().at(0);
    }
    if (vm.count("m")) {
        defaultCommitMessage = vm["m"].as<string>();
    }

    auto branch = git::get_branch(path, git::formats::SHORT);
    if(branch.size()){
        git::commit(path, branch + " " + defaultCommitMessage);
    }else{
        std::cout << "No branch here, sorry\n";
    }


    return 0;
}
