/* csvreader - class to read simple csv files
 */
#ifndef __CSVREADER_H__
#define __CSVREADER_H__

#include <vector>
#include <string>

 // a row of data values
class CSVRow {
private:
    std::string getNextField(std::stringstream& ss, char separatorChar, char quoteChar);
public:
    std::vector<std::string> values;
    CSVRow(char commentChar, char separatorChar, char quoteChar, std::string& line);
    ~CSVRow();

    const std::string& operator[](std::size_t index) const {
        return values[index];
    }

    std::size_t size() const {
        return values.size();
    }
};


// a list of rows
class CSVFile {
public:
    char commentChar = '#';
    char separatorChar = ',';
    char quoteChar = '"';

    std::vector<CSVRow> rows;
    // maximum dimensions, can be accessed with getValue()
    size_t maxColCount; // longest row


    CSVFile(char _commentChar, char _separatorChar, char _quoteChar, std::string full_path) {
        commentChar = _commentChar;
        separatorChar = _separatorChar;
        quoteChar = _quoteChar;

        load(full_path);
    }

    ~CSVFile() {
        clear();
    }

    void clear();
    void load(std::string full_path);
    unsigned getRowCount() {
        return (unsigned)rows.size();
    }
    unsigned getColCount() {
        return (unsigned)maxColCount;
    }

    std::string getValue(unsigned row, unsigned col);

};

#endif // __CSVREADER_H__
