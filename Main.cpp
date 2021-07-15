#include <QCoreApplication>
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

void OutputAndDeletFile(std::vector<bfs::path>& vFilesPath, int& iCount, bool bDeliting, bool bErrorsOut, bool bFilePathOut, bool bOneLineOut, bool bPrecentOut){

    for(int i = 0; i<iCount; i++)
    {
        if( !bErrorsOut && bFilePathOut )
        {
            if(bOneLineOut)
            {
                if(bPrecentOut)
                {
                    std::cout << vFilesPath.at(i) << Cons(" ") << (i*100/iCount) << Cons("% Уже удалено                          ") << '\r' << std::flush;
                    if(i == iCount-1)
                    {
                        Sleep(1000);
                        std::cout << Cons("100% Уже удалено                          ") << std::endl;
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

void ChoiceAskAndDryRun(std::vector<bfs::path>& vFilesPath, bool bDryRun, bool bAsk, bool bErrorsOut , bool bFilePathOut, bool bOneLineOut, bool bPrecentOut, bfs::path& pathDir){
    int iCountFiles = vFilesPath.size();
    int iSizeAllFiles = 0;
    if(bAsk){
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
            if(bDryRun)
            {
                OutputAndDeletFile(vFilesPath, iCountFiles, false, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
            }
            else
            {
                OutputAndDeletFile(vFilesPath, iCountFiles, true, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
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
        if(bDryRun)
        {
            OutputAndDeletFile(vFilesPath, iCountFiles, false, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
        }
        else
        {
            OutputAndDeletFile(vFilesPath, iCountFiles, true, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
            bfs::remove_all(pathDir);
        }
    }
}



int main(int argc, char *argv[])
{
#ifdef WIN32
    SetConsoleOutputCP(1251);
#endif//WIN32

    std::cout << Cons("INF> ___ Запуск ___") << std::endl;

    boost::program_options::variables_map vm;
    if( CheckCommandLineArgs( argc, argv, vm ) )
    {

        bfs::path pathDir(vm["path_dir"].as<std::string>());
        if( bfs::is_directory(pathDir) )
        {
            bfs::recursive_directory_iterator begin(pathDir);
            bfs::recursive_directory_iterator end;
            std::vector<bfs::path> vFilesPath;

            std::cout << "Start Copy" << std::endl;
            while(begin != end)
            {
                if(bfs::is_regular_file(begin->path()))
                {
                    vFilesPath.push_back(begin->path());
                }
                begin++;
            }

            ChoiceAskAndDryRun(vFilesPath, vm.count("dry_run"), vm.count("ask"), vm.count("errors_out"), vm.count("file_path_out"), vm.count("one_line_out"), vm.count("percent_out"), pathDir);
        }
        else
        {
            std::cerr << Cons("ERR> Данный путь не существует или не ведет к каталогу") << std::endl;
        }
    }

    std::cout << Cons("INF> ___ Останов ___") << std::endl;
}
