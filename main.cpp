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

void Print(std::vector<path>& Files ,int&count ,bool deliting,bool errors ,bool file ,bool one_line ,bool precent){
    char MyEnd = '\n';
    if(one_line){
        MyEnd = ' ';
    }
    for(int i = 0; i<count ;i++){
        if( !errors && precent){
            cout << (i*100/count) << CONS("% Уже удалено ");
        }
        if( !errors && file ){
            cout << Files.at(i) << MyEnd;
        }
        if(deliting){
            try{
                remove(Files.at(i));
            }catch (const filesystem_error& ex){
                std::cerr << "ERR> " <<ex.what() << MyEnd;
            }
        }
    }
    if( !errors && precent){
        cout << CONS("100% Уже удалено") << '\n';
    }
}

void Fun(std::vector<path>& Files ,bool dry_run ,bool ask ,bool errors ,bool file ,bool one_line ,bool percent,path& p){
    int count = Files.size();
    int size = 0;
    if(ask){
        for(int i = 0; i<count ;i++){
            try{
                size += file_size(Files.at(i));
            }catch (const filesystem_error& ex){
                std::cerr << CONS("ERR> ") <<ex.what() << endl;
            }
        }
        cout << CONS("Удалить каталог с количеством файлов равным ") << count << CONS(" и весом ") << size << CONS(". Y/N") << endl;
        char ch;
        cin >> ch;
        if(ch == 'Y'){
            if(dry_run){
                Print(Files ,count ,false, errors, file, one_line, percent);
            }else{
                Print(Files ,count ,true , errors, file, one_line, percent);
                remove_all(p);
            }
        }else{
            return;
        }
    }else{
        if(dry_run){
            Print(Files ,count ,false , errors, file, one_line, percent);
        }else{
            Print(Files ,count ,true , errors, file, one_line, percent);
            remove_all(p);
        }
    }
}



int main(int argc, char *argv[])
{
#ifdef WIN32
    SetConsoleOutputCP(1251);
#endif//WIN32

    cout << CONS("INF> ___ Запуск ___") << endl;

    variables_map vm;
    if( CheckCommandLineArgs( argc, argv, vm ) ){

        path p(vm["path"].as<std::string>());
        if( is_directory(p) ){
            recursive_directory_iterator begin(p);
            recursive_directory_iterator end;
            std::vector<path> Files;
            std::copy_if(begin, end, std::back_inserter(Files), [](const path& path) {
                        return is_regular_file(path);
                    });
            Fun(Files,vm.count("dry_run") ,vm.count("ask") ,vm.count("errors") ,vm.count("file") ,vm.count("one_line") ,vm.count("percent"),p);
        }else{
            std::cerr << CONS("ERR> Данный путь не существует или не ведет к каталогу") << endl;
        }
    }

    cout << CONS("INF> ___ Останов ___") << endl;
}
