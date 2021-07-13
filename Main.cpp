#include <QCoreApplication>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <iomanip>

#include "CommandLineArgs.h"

using std::cout;
using std::endl;
using std::cin;
using namespace boost::filesystem;
using namespace boost::system;

void OutputAndDeletFile(std::vector<path>& vFilesPath, int& iCount, bool bDeliting, bool bErrorsOut, bool bFilePathOut, bool bOneLineOut, bool bPrecentOut){
    char chMyEnd = '\n';
    if(bOneLineOut)
    {
        chMyEnd = ' ';
    }
    for(int i = 0; i<iCount; i++)
    {
        if( !bErrorsOut && bPrecentOut)
        {
            cout << (i*100/iCount) << Cons("% Уже удалено ");
        }
        if( !bErrorsOut && bFilePathOut )
        {
            cout << vFilesPath.at(i) << chMyEnd;
        }
        if(bDeliting){
            try
            {
                remove(vFilesPath.at(i));
            }catch (const filesystem_error& ex)
            {
                std::cerr << "ERR> " <<ex.what() << chMyEnd;
            }
        }
    }
    if( !bErrorsOut && bPrecentOut)
    {
        cout << Cons("100% Уже удалено") << '\n';
    }
}

void ChoiceAskAndDryRun(std::vector<path>& vFilesPath, bool bDryRun, bool bAsk, bool bErrorsOut , bool bFilePathOut, bool bOneLineOut, bool bPrecentOut, path& pathDir){
    int iCountFiles = vFilesPath.size();
    int iSizeAllFiles = 0;
    if(bAsk){
        for(int i = 0; i<iCountFiles; i++)
        {
            try
            {
                iSizeAllFiles += file_size(vFilesPath.at(i));
            }catch (const filesystem_error& ex)
            {
                std::cerr << Cons("ERR> ") <<ex.what() << endl;
            }
        }
        cout << Cons("Удалить каталог с количеством файлов равным ") << iCountFiles << Cons(" и весом ") << iSizeAllFiles << Cons(". Y/N") << endl;
        char ch;
        cin >> ch;
        if(ch == 'Y')
        {
            if(bDryRun)
            {
                OutputAndDeletFile(vFilesPath, iCountFiles, false, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
            }else
            {
                OutputAndDeletFile(vFilesPath, iCountFiles, true, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
                remove_all(pathDir);
            }
        }else
        {
            return;
        }
    }else
    {
        if(bDryRun)
        {
            OutputAndDeletFile(vFilesPath, iCountFiles, false, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
        }else
        {
            OutputAndDeletFile(vFilesPath, iCountFiles, true, bErrorsOut, bFilePathOut, bOneLineOut, bPrecentOut);
            remove_all(pathDir);
        }
    }
}



int main(int argc, char *argv[])
{
#ifdef WIN32
    SetConsoleOutputCP(1251);
#endif//WIN32

    cout << Cons("INF> ___ Запуск ___") << endl;

    variables_map vm;
    if( CheckCommandLineArgs( argc, argv, vm ) )
    {

        path pathDir(vm["path_dir"].as<std::string>());
        if( is_directory(pathDir) )
        {
            recursive_directory_iterator begin(pathDir);
            recursive_directory_iterator end;
            std::vector<path> vFilesPath;
            std::copy_if(begin, end, std::back_inserter(vFilesPath), [](const path& path)
            {
                return is_regular_file(path);
            });
            ChoiceAskAndDryRun(vFilesPath, vm.count("dry_run"), vm.count("ask"), vm.count("errors_out"), vm.count("file_path_out"), vm.count("one_line_out"), vm.count("percent_out"), pathDir);
        }else
        {
            std::cerr << Cons("ERR> Данный путь не существует или не ведет к каталогу") << endl;
        }
    }

    cout << Cons("INF> ___ Останов ___") << endl;
}
