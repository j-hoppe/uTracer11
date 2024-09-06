//

#include <sstream>

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

