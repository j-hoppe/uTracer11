/* keypatterns - recognizes words in free text via regex
   and returns a list of keys for graphical annotations
   Regex definitions contain a dataFieldIdx, which specifies
   against which input datafield to match


*/
#ifndef __KEYWORDS_H__
#define __KEYWORDS_H__

#include  "wx/regex.h"
#include  "wx/xml/xml.h"
#include  "wx/filename.h"
#include <vector>

// single pattern definition:
// 1st = a regex, 2nd the resulting keyowrds on match
// example: "<-((PC)|(R11)|(R17))"  , "SPAM,SPM"
// when a uflow action descriptions conatins <-PC, in datapth is SPAM and SPM higlighted
class StringPair {
public:
    // "advanced wxRegEx" format
    const std::string regexText;
    // strict comma list, no spaces allowed
    const std::string resultKeyListCommaText;
};

// compiled pattern->ID list
class KeyPattern {
public:
    unsigned dataFieldIdx = 0; // select one of several input fields to parse, starts with 0
    // text representation
    std::string regexText;
    std::string resultKeyListCommaText;
    // compiled
    wxRegEx* regex = nullptr;
    std::vector<std::string>    keyList; // list of parsed Keys, returned on match

    std::string info;

    KeyPattern(int _dataFieldIdx, std::string _regexText, std::string _resultKeyListText);
    ~KeyPattern() {
        if (regex != nullptr) delete regex;
    }
    void compile();

    int addKeysToListByMatch(std::string searchMultiLineText, std::vector<std::string>& resultKeyList);

};

// multiple patterns, whit load, compile and match operators
class KeyPatternList {
public:
    std::vector<KeyPattern> keyPatterns;

    void loadXml(wxFileName resourcePath, std::string subDir, std::string fileName);
    void assertKeysAgainst(std::vector < std::string>& controlKeyList, std::string info);

    void getKeyListByMatch(std::string searchText, std::vector<std::string>& resultKeyList);
    void getKeyListByMatch(std::vector<std::string>& dataFields, std::vector<std::string>& resultKeyList);
};

#endif // __KEYWORDS_H__
