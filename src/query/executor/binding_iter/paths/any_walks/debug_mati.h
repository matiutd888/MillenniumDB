// debug_mati.h
#ifndef DEBUG_MATI_H
#define DEBUG_MATI_H

#include <iostream>

// Use __PRETTY_FUNCTION__ for GCC/Clang or __FUNCSIG__ for MSVC
#if defined(_MSC_VER)
    #define _debug_mati() std::cout << "MATI: " << __FUNCSIG__ << ": "
#else
    #define _debug_mati() std::cout << "MATI: " << __PRETTY_FUNCTION__ << ": "
#endif

#endif // DEBUG_MATI_H