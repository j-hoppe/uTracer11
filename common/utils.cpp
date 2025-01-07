//

#include <sstream>
#include <cstdio>

#include "utils.h"


// make a comma list
std::string stringVectorAsCommaList(std::vector<std::string>&stringVector, std::string separator)
{
    std::ostringstream oss;
    for (size_t i = 0; i < stringVector.size(); ++i) {
        oss << stringVector[i];
        if (i != stringVector.size() - 1) {
            oss << separator;
        }
    }
    return oss.str();
}


std::string format_string(const char* fmt, ...) {
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args) ; // vsnprintf() modifies 'args', need copy

    int size = std::vsnprintf(nullptr, 0, fmt, args);
    std::string result(size + 1, '\0');
    std::vsnprintf(&result[0], size + 1, fmt, args_copy);

    va_end(args_copy);
    va_end(args);
    result.pop_back(); // Remove null terminator
    return result;
}

