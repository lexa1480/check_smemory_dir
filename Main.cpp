#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <iomanip>
#include <stdio.h>
#include <cstdio>

#include "CommandLineArgs.h"

namespace bfs = boost::filesystem;


#ifndef  WIN32
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    inline void Sleep( int iTimeout_ms )
    {
        usleep( iTimeout_ms * 1000 );
    }
#endif//WIN32

void OutputAndDeletFile(std::vector<bfs::path>& vFilesPath, int& iCount, bool bDeliting, boost::program_options::variables_map& vm)
{

    for(int i = 0; i<iCount; i++)
    {
        if( !(vm.count(c_szArgErrorsOut)) && (vm.count(c_szArgFilePathOut)) )
        {
            if(vm.count(c_szArgOneLineOut))
            {
                if(vm.count(c_szArgPercentOut))
                {
                    std::cout << std::setfill(' ') << std::setw(3) << (i*100/iCount) << Cons("% :") << vFilesPath.at(i) << Cons("                    ") << '\r' << std::flush;
                    if(i == iCount-1)
                    {
                        Sleep(1000);
                        std::cout << Cons("100%                          ") << std::endl;
                    }
                }
                else
                {
                    std::cout << vFilesPath.at(i) << Cons("                          ") << '\r' << std::flush;
                    if(i == iCount-1)
                    {
                        std::cout << std::endl;
                    }
                }
                Sleep(1000);
            }
            else
            {
                std::cout << vFilesPath.at(i) << std::endl;
            }
        }
        if(bDeliting)
        {
            try
            {
                remove(vFilesPath.at(i));
            }
            catch (const bfs::filesystem_error& ex)
            {
                std::cerr << "ERR> " <<ex.what();
            }
        }
    }
}

void ChoiceAskAndDryRun(std::vector<bfs::path>& vFilesPath, boost::program_options::variables_map& vm)
{

    bfs::path pathDir(vm[c_szArgPathDir].as<std::string>());

    int iCountFiles = vFilesPath.size();
    int iSizeAllFiles = 0;
    if(vm.count(c_szArgAsk))
    {
        for(int i = 0; i<iCountFiles; i++)
        {
            try
            {
                iSizeAllFiles += bfs::file_size(vFilesPath.at(i));
            }
            catch (const bfs::filesystem_error& ex)
            {
                std::cerr << Cons("ERR> ") <<ex.what() << std::endl;
            }
        }
        std::cout << Cons("Удалить каталог с количеством файлов равным ") << iCountFiles << Cons(" и весом ") << iSizeAllFiles << Cons(". Y/N") << std::endl;
        char ch;
        std::cin >> ch;
        if(ch == 'Y')
        {
            if(vm.count(c_szArgDryRun))
            {
                OutputAndDeletFile(vFilesPath, iCountFiles, false, vm);
            }
            else
            {
                OutputAndDeletFile(vFilesPath, iCountFiles, true, vm);
                bfs::remove_all(pathDir);
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        if(vm.count(c_szArgDryRun))
        {
            OutputAndDeletFile(vFilesPath, iCountFiles, false, vm);
        }
        else
        {
            OutputAndDeletFile(vFilesPath, iCountFiles, true, vm);
            bfs::remove_all(pathDir);
        }
    }
}



int main(int argc, char *argv[])
{
#ifdef WIN32
    SetConsoleOutputCP(1251);
#endif//WIN32


    boost::program_options::variables_map vm;
    if( CheckCommandLineArgs( argc, argv, vm ) )
    {

        bfs::path pathDir(vm[c_szArgPathDir].as<std::string>());
        if( bfs::is_directory(pathDir) )
        {
            bfs::recursive_directory_iterator itPath(pathDir);
            bfs::recursive_directory_iterator end;
            std::vector<bfs::path> vFilesPath;

            while(itPath != end)
            {
                if(bfs::is_regular_file(itPath->path()))
                {
                    vFilesPath.push_back(itPath->path());
                }
                itPath++;
            }

            ChoiceAskAndDryRun(vFilesPath, vm);
        }
        else
        {
            std::cerr << Cons("ERR> Данный путь не существует или не ведет к каталогу") << std::endl;
        }
    }
}
