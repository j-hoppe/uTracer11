/* csvreader - classes to read simple csv files

Format like
# addr   type        group        symbol      info
0777500; DEVICE_REG; "Cassette"; "TA11.TACS"; "command & status"
addr >= 160000: assume physical iopage

 */
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

#include "utils.h"
#include "csvreader.h"


 // return text until separatorChar. Tread text with in quotes as atomic
 // spaces are part of the result
 // return: ss has read the separator, or ss.eof()
 // Example: ss =  ' "hello" ;2nd word; ready '
 // call 1:  return ' "hello" ', ss = '2nd word; ready '
 // call 2:  return '2nd word', ss = ' ready '
 // call 3:  return ' ready ', ss.eof()
 // call 4:  return '', ss.eof()
std::string CSVRow::getNextField(std::stringstream& ss, char separatorChar, char quoteChar) {

    std::stringstream result;
    char nextChar; // next unprocessed char
    ss >> nextChar;
    while (!ss.eof() && nextChar != separatorChar) {
        if (nextChar == quoteChar) { // add "...." sequence to result
            ss >> nextChar; // skip first "
            // read into item until next "
            while (!ss.eof() && nextChar != quoteChar) {
                result << nextChar;
                ss >> nextChar;
            }
            ss >> nextChar; // skip trailing ", even on eof
        }
        else { // add single char to result
            result << nextChar;
            ss >> nextChar;
        }
    }
    return result.str();
}


// parse a line into field values
// comments are stripped off, surrounding space is removed
// and empty line results as row with a single value ""
CSVRow::CSVRow(char commentChar, char separatorChar, char quoteChar, std::string& line)
{
    std::string trimmed_line = line;
    size_t pos = trimmed_line.find(commentChar);
    if (pos != std::string::npos) {
        trimmed_line = trimmed_line.substr(0, pos); // trunc off line comment
    }

    std::stringstream ss(trimmed_line);
    std::string token; // next unprocessed text between separators
    values.clear();
    while (!ss.eof()) {
        token = getNextField(ss, separatorChar, quoteChar);
        trim(token);
        values.push_back(token);
    }
}


CSVRow::~CSVRow()
{
    values.clear();
}

// remove all rows, recusive all values
void CSVFile::clear()
{
    rows.clear();
}


// read file
// commentChar, separtorChar set
void CSVFile::load(std::string full_path)
{
    clear();

    std::ifstream inputFile(full_path);

    if (!inputFile.is_open()) {
        throw std::invalid_argument("Couldn't read CSV file: '" + full_path + '"');
    }
    maxColCount = 0;

    std::string line;
    while (getline(inputFile, line)) {
        auto row = CSVRow(commentChar, separatorChar, quoteChar, line);
        // filter "no fields", or a single empty field"
        bool emptyRow = (row.size() == 0) || (row.size() == 1 && row.values[0].empty());
        if (!emptyRow) {
            // add row to file, update columns in longest row
            rows.push_back(row);
            maxColCount = std::max(maxColCount, rows.size());
        }
    }
    inputFile.close();
}

// access a row and a value within.
// colIdx is allo3wed up to the amx of all field counts,
// return ing"" when the indexrow is shorter
std::string CSVFile::getValue(unsigned rowIdx, unsigned colIdx) {
    if (rowIdx >= rows.size())
        throw std::out_of_range("CSVfile: illegal row");
    auto row = rows[rowIdx];
    if (colIdx >= maxColCount)
        throw std::out_of_range("CSVfile: illegal column");
    if (colIdx >= row.size())
        return "";
    else return row[rowIdx];
}
