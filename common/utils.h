//
#ifndef __UTILS_H__
#define __UTILS_H__

// Compatibility Arduino-Linux-Visual Studio

#if defined(PLATFORM_ARDUINO)

#include <Arduino.h>
// please use only types with well defined bit-width
typedef unsigned long uint32_t;
typedef word uint16_t;
typedef byte uint8_t;

#else
// Linux or Visual Studio. All C++ features and lots of memory avialbale.
// 
#include <algorithm>
#include <string>
#include <vector>


#if defined (WIN32)

#ifndef strcasecmp
#define strcasecmp _stricmp	// grmbl
#endif

#else // ! WIN32
#include <limits.h> // INT_MAX

#endif // WIN32


std::string stringVectorAsCommaList(std::vector<std::string>& stringList, std::string separator = ",");

// Thanks to https ://stackoverflow.com/a/44973498/524503 for bringing up the modern solution.
// trim from start (in place)
inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

#endif // PLATFORM_ARDUINO

#if !defined(UNREFERENCED_PARAMETER)
#define UNREFERENCED_PARAMETER(p) (void)(p)
#endif


#endif // __UTILS_H__


