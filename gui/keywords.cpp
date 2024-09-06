

#include "wx/wx.h"
#include <sstream>
#include "utils.h"
#include "keywords.h"


// new pattern from parsed text strings
// split keylist, regex yet not compiled
KeyPattern::KeyPattern(int _dataFieldIdx, std::string _regexText, std::string _resultKeyListCommaText) {
    dataFieldIdx = _dataFieldIdx;
    // save original input
    regexText = _regexText;
    resultKeyListCommaText = _resultKeyListCommaText;
    regex = nullptr;
    // split result list into separated keys
    std::istringstream iss(resultKeyListCommaText);
    std::string item;
    while (std::getline(iss, item, ',')) {
        trim(item);
        keyList.push_back(item);
    }
}


// allocate and compile the regex
void KeyPattern::compile() {
    // compile. max syntax, ignore char case
    regex = new wxRegEx(regexText, wxRE_ADVANCED | wxRE_ICASE | wxRE_NOSUB);
    if (!regex->IsValid())
        wxLogError("Error compiling regex \"%s\"", regexText);
}


// adds matches for a single regex to the resultKeyList, unique
// ^ and $ have special meaning for "start of line", "end of line"
// searchMultilineText may contain liene breaks \n, each line is matched individually
// result: # of matches ... less new keys are added, if they were already in the list
int KeyPattern::addKeysToListByMatch(std::string searchMultilineText, std::vector<std::string>& resultKeyList) {
    int result = 0;
    // split searchMultilineText
    std::stringstream ss(searchMultilineText);
    std::string searchText;
    while (std::getline(ss, searchText, '\n')) {
        // match single line
        if (regex->Matches(searchText)) {
            result++;
            // pathing pattern: add all keys to result, if not there
            for (auto itKey = keyList.begin(); itKey != keyList.end(); itKey++) {
                // search in result
                if (std::find(resultKeyList.begin(), resultKeyList.end(), *itKey) == resultKeyList.end()) {
                    // not there: add
                    resultKeyList.push_back(*itKey);
                    std::string s = *itKey;
                    //wxLogDebug("addKeysToListByMatch() adding %s, size now %d", s, (int)resultKeyList.size());
                }
            }
        }
    }
    return result;
}

/* call like "...resources\pdp1134", "MP00082", "filename.xml"
 Format :
<document>
   <keypattern>
      <regex> (BA).*&lt;- </regex>
      <keylist> VBA,BA,DRIVER_ADDRESS,UNIBUS </keylist>
      <info> write bus address </info>
   </keypattern>
 </document>
*/
void KeyPatternList::loadXml(wxFileName resourcePath, std::string subDir, std::string fileName)
{
    wxFileName path = resourcePath; // to be extended

    // append components to path
    //subDir += wxFileName::GetPathSeparator();
    path.SetFullName(fileName);
    path.AppendDir(subDir);
    // now .../resource/subdir/filename.xml

    wxXmlDocument doc;
    if (!doc.Load(path.GetAbsolutePath())) {
        auto errormsg = wxString::Format(" XML file %s could not be opened", path.GetAbsolutePath());
        wxMessageBox(errormsg, "File error", wxICON_ERROR);
        wxLogError(errormsg);// gui not yet open
        abort();
        return;
    }
    wxLogInfo("%s opened", path.GetAbsolutePath());

    // iterate all <keypattern>
    wxXmlNode* rootChild = doc.GetRoot()->GetChildren();
    while (rootChild) {
        if (rootChild->GetName().IsSameAs("keypattern", false)) {
            wxXmlNode* keypatternChild = rootChild->GetChildren();
            std::string dataFieldIdxText, regexText, resultKeyListCommaText, infoText;

            while (keypatternChild) {
                // now inside <keypattern>. Content with extra spaces for readability
                auto val = keypatternChild->GetNodeContent().Trim(false).Trim(true);
                if (keypatternChild->GetName().IsSameAs("datafield", false)) {
                    dataFieldIdxText = val;
                }
                else if (keypatternChild->GetName().IsSameAs("regex", false)) {
                    regexText = val;
                }
                else if (keypatternChild->GetName().IsSameAs("keylist", false)) {
                    resultKeyListCommaText = val;
                }
                else if (keypatternChild->GetName().IsSameAs("info", false)) {
                    infoText = val;
                }
                keypatternChild = keypatternChild->GetNext();
            }
            unsigned dataFieldIdx = (unsigned)std::atoi(dataFieldIdxText.c_str());
            if (dataFieldIdx > 0)
                dataFieldIdx--; // internal from 0, in xml from 1
            KeyPattern kp(dataFieldIdx, regexText, resultKeyListCommaText);
            kp.info = infoText;
            keyPatterns.push_back(kp);
        }
        rootChild = rootChild->GetNext();
    }

    // now compile all regexes
    for (auto it = keyPatterns.begin(); it < keyPatterns.end(); it++) {
        it->compile();
    }

}

// verify, that each produced key appears in controlList. else error stop
// call after loadXml()
void KeyPatternList::assertKeysAgainst(std::vector<std::string>& controlKeyList, std::string info) {
    for (auto itPattern = keyPatterns.begin(); itPattern != keyPatterns.end(); itPattern++) {
        auto keyList = itPattern->keyList;
        for (auto itKey = keyList.begin(); itKey != keyList.end(); itKey++) {
            if (std::find(controlKeyList.begin(), controlKeyList.end(), *itKey) == controlKeyList.end())
                wxLogFatalError("%s: Pattern generates key %s, which is not in list", info, *itKey);
        }
    }
}


// match all regexes against 'searchText', keyList of matches returned in resultKeyList
// Example: input = info line from 11/34 micro code flow chart data
// inpput (uflow = 016), datafield = "BA<-PC,DATI , IR,B<-UDATA"
// result: objects in data path schematic "VBA,BA,UNIBUS,SPM,B_REG,IR"
void KeyPatternList::getKeyListByMatch(std::string searchText, std::vector<std::string>& resultKeyList) {
    resultKeyList.clear();
    // test all pattern regexes again searchtext
    for (auto itKeyPattern = keyPatterns.begin(); itKeyPattern != keyPatterns.end(); itKeyPattern++) {
        itKeyPattern->addKeysToListByMatch(searchText, resultKeyList);
    }
}

// match agains a list of input data fields, actual field selected by dataFieldIdx
void KeyPatternList::getKeyListByMatch(std::vector<std::string>& dataFields, std::vector<std::string>& resultKeyList) {
    resultKeyList.clear();
    // test all pattern regexes again searchtext
    for (auto itKeyPattern = keyPatterns.begin(); itKeyPattern != keyPatterns.end(); itKeyPattern++) {
        if (itKeyPattern->dataFieldIdx >= dataFields.size())
            continue; // this pattern referneces an input field not present in the input list
        auto searchText = dataFields[itKeyPattern->dataFieldIdx];
        itKeyPattern->addKeysToListByMatch(searchText, resultKeyList);
    }
}

