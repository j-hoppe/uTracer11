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


#if defined(__WXGTK__) || defined(__WXMSW__)
// running under wxWindows, Ubuntu or MS Windows
#include <wx/wx.h>
#define LOGERROR(fmt, ...)      wxLogError(fmt, __VA_ARGS__)
#define LOGWARNING(fmt, ...)    wxLogWarning(fmt, __VA_ARGS__)
#define LOGINFO(fmt, ...)       wxLogInfo(fmt, __VA_ARGS__)
#else
#include <stdio.h>
#define LOGERROR(fmt, ...)      fprintf(stderr, "ERROR: " fmt, __VA_ARGS__)
#define LOGWARNING(fmt, ...)    fprintf(stderr, "WARNING: " fmt, __VA_ARGS__)
#define LOGINFO(fmt, ...)       fprintf(stderr, "INFO: " fmt, __VA_ARGS__)
#endif


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

// all args "char *" !
std::string format_string(const char* fmt, ...) ;

inline std::string uppercase(std::string &s) {
    std::string result = s ;
    transform(result.begin(), result.end(), result.begin(), ::toupper) ;
    return result ;
}

#endif // PLATFORM_ARDUINO

#if !defined(UNREFERENCED_PARAMETER)
#define UNREFERENCED_PARAMETER(p) (void)(p)
#endif


// get a KM11 signal bit from the matrix andd shift it
#define GETBIT(val,shift) (!!( (val) & (1 << (shift) ) ) )  // extract a bit as bool
#define SETBIT(bitmask,shift) ( (bitmask) ? 1 << (shift):0 )


// make integer to decimal string, historic K&R
void itoa10(int n, char s[]) ;

// convert '0'..'9', 'A'..'F' to numerical value. -1 on error
inline int hexchar2val(char c) {
    if ((c >= '0') && (c <= '9'))
        return (c - '0');
    else if ((c >= 'A') && (c <= 'F'))
        return (c - 'A' + 10);
    else if ((c >= 'a') && (c <= 'f'))
        return (c - 'a' + 10);
    else 
		return -1 ;
}


#endif // __UTILS_H__


