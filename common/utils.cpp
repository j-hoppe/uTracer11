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




// From the first edition of Kernighan and Ritchie's The C Programming Language, on page 60. 
 /* itoa:  convert n to characters in s */
 // The function reverse used above is implemented two pages earlier:
 
 #include <string.h>
 
  /* reverse:  reverse string s in place */
  void reverse(char s[])   {
	  int i, j;
	  char c;
	  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		  c = s[i];
		  s[i] = s[j];
		  s[j] = c;
	  }
 }	

// make integer decimal string
 void itoa10(int n, char s[]) {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}  
