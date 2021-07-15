#ifndef COMMANDLINEARGS_H
#define COMMANDLINEARGS_H

#include <boost/program_options.hpp>
#include <iostream>
#include "Cons.h"



inline bool CheckCommandLineArgs(int ac, char* av[], boost::program_options::variables_map& vm ){

    namespace po = boost::program_options;
    bool bContinueExecution = false;
    try
    {
        po::options_description desc("INF> Allowed options");
        desc.add_options()
                ("help", "Produce help message")
                ("path_dir", po::value<std::string>(), "Include path")
                ("dry_run", "Dry_run")
                ("ask", "Ask delete")
                ("one_line_out", "Format output: one_line")
                ("errors_out", "Format output: only errors")
                ("file_path_out", "Format output: delete files")
                ("percent_out", "Format output: percent of delete files")
                ;

        po::parsed_options parsed = po::command_line_parser(ac, av).options(desc).allow_unregistered().run();
        po::store(parsed, vm);

        if( vm.count("help") || ( vm.size() == 0 ) )
        {
            std::cout << desc;
        }
        else
        {
            notify(vm);
            bContinueExecution = true;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << Cons("ERR> ") << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << Cons("ERR> Exception of unknown type!") << std::endl;
    }

    return bContinueExecution;
}

#endif // COMMANDLINEARGS_H
