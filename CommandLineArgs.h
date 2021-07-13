#ifndef COMMANDLINEARGS_H
#define COMMANDLINEARGS_H

#include <boost/program_options.hpp>
#include <iostream>
#include "Cons.h"

using namespace boost::program_options;

bool CheckCommandLineArgs(int ac, char* av[], variables_map& vm ){

    bool bContinueExecution = false;
    try
    {
        options_description desc("INF> Allowed options");
        desc.add_options()
                ("help", "Produce help message")
                ("path_dir", value<std::string>(), "Include path")
                ("dry_run", "Dry_run")
                ("ask", "Ask delete")
                ("one_line_out", "Format output: one_line")
                ("errors_out", "Format output: only errors")
                ("file_path_out", "Format output: delete files")
                ("percent_out", "Format output: percent of delete files")
                ;

        parsed_options parsed = command_line_parser(ac, av).options(desc).allow_unregistered().run();
        store(parsed, vm);

        if( vm.count("help") || ( vm.size() == 0 ) )
        {
            std::cout << desc;
        }else
        {
            notify(vm);
            bContinueExecution = true;
        }
    }catch(std::exception& e)
    {
        std::cerr << Cons("ERR> ") << e.what() << std::endl;
    }catch(...)
    {
        std::cerr << Cons("ERR> Exception of unknown type!") << std::endl;
    }

    return bContinueExecution;
}

#endif // COMMANDLINEARGS_H
