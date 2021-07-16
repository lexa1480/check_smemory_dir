#ifndef COMMANDLINEARGS_H
#define COMMANDLINEARGS_H

#include <boost/program_options.hpp>
#include <iostream>
#include "Cons.h"

const char c_szArgHelp[] = "help";
const char c_szArgPathDir[] = "path_dir";
const char c_szArgDryRun[] = "dry_run";
const char c_szArgAsk[] = "ask";
const char c_szArgOneLineOut[] = "one_line_out";
const char c_szArgErrorsOut[] = "errors_out";
const char c_szArgFilePathOut[] = "file_path_out";
const char c_szArgPercentOut[] = "percent_out";

inline bool CheckCommandLineArgs(int ac, char* av[], boost::program_options::variables_map& vm )
{

    namespace po = boost::program_options;
    bool bContinueExecution = false;
    try
    {
        po::options_description desc("INF> Allowed options");
        desc.add_options()
                (c_szArgHelp, "Produce help message")
                (c_szArgPathDir, po::value<std::string>(), "Include path")
                (c_szArgDryRun, "Dry_run")
                (c_szArgAsk, "Ask delete")
                (c_szArgOneLineOut, "Format output: one_line")
                (c_szArgErrorsOut, "Format output: only errors")
                (c_szArgFilePathOut, "Format output: delete files")
                (c_szArgPercentOut, "Format output: percent of delete files")
                ;

        po::parsed_options parsed = po::command_line_parser(ac, av).options(desc).allow_unregistered().run();
        po::store(parsed, vm);

        if( vm.count(c_szArgHelp) || ( vm.size() == 0 ) )
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
