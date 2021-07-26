#ifndef CONS_H
#define CONS_H

#include <iostream>

std::string inline Cons(const char* pText)
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

#endif // CONS_H
