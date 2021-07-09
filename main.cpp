#include <QCoreApplication>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <iomanip>
\
#include "CommandLineArgs.h"

using std::cout;
using std::endl;
using std::cin;
using namespace boost::filesystem;
using namespace boost::system;

std::string CONS(const char* pText)
{
    std::string sRes;
#ifdef WIN32
    try
    {
        sRes = boost::locale::conv::between(pText, "cp-1251", "utf-8");
    }
    catch(const std::exception& e)
    {
        std::cout << "EXC> " << e.what() << std::endl;
    }
#else //WIN32
    sRes = pText;
#endif//WIN32

    return sRes;
}

bool dry_run(path& p,int& count,int& size,variables_map& vm){

    try{
        if( ( !exists(p) ) && ( !is_directory(p) ) ){
            cout << p << "ERR> Данный путь не существует или не ведет к каталогу" << endl;
            return true;
        }

        char Myend = '\n';
        if(vm.count("one_line")){
            Myend = ' ';
        }

        recursive_directory_iterator iter (p) ;
        recursive_directory_iterator end;
        cout <<"Выбранная директория содержитв себе: " << endl;
        while(iter != end){
            if(is_regular_file(iter->path())){
                count++;
                size = size + file_size(iter->path());
                if( (!vm.count("errors")) && (vm.count("file")) ){
                    cout << "      Файл: " << iter->path() << Myend;
                }
            }
            if(is_directory(iter->path())){
                if( (!vm.count("errors")) && (vm.count("file")) ){
                    cout << "Директория: " << iter->path() << Myend;
                }
            }
            iter++;
        }
    }catch (const filesystem_error& ex){
        cout << ex.what() << endl;
    }
    return false;
}

int main(int argc, char *argv[])
{
#ifdef WIN32
    SetConsoleOutputCP(1251);
#endif//WIN32

    cout << CONS("INF> ___ Запуск ___") << endl;

    variables_map vm;
    if( !CheckCommandLineArgs( argc, argv, vm ) ){
        return 1;
    }

    //Модификаторы вывода внутри фунции + мод в одну строку
    path p(vm["path"].as<std::string>());
    int count = 0;
    int size = 0;
    if(dry_run(p,count,size,vm)){
        return 1;
    }

    if(vm.count("ask")){
        cout << "\nВы уверены что хотите удалить " << count << " файлов. Общим объемом " << size << " : Y/N" << endl;
        char ch;
        cin >> ch;
        if(ch == 'Y'){
            if(vm.count("dry_run")){
                remove_all(p);
            }
            cout << "INF> Удалениe произведено успешно" << endl;
        }else{
            cout << "INF> Отмена удаления произведена успешно" << endl;
        }
    }else{
        if(vm.count("dry_run")){
            remove_all(p);
        }
        cout << "INF> Удалениe произведено успешно" << endl;
    }

    cout << CONS("\n>>>> ___ Останов ___") << endl;
}
