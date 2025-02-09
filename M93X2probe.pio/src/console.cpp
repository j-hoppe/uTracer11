/* console.cpp - handling of user RS232, buffered background operation

Serial console driver
sents and receives in background, by service() thread

Arduino Serial does buffer on its own, but we need to
rcv/xmt complete strings terminated with \n.
So own buffer code, like:
https://www.codeproject.com/Articles/1255400/Processing-Data-from-Serial-Line-in-Arduino
*/

#include "console.h"
#include <stdarg.h>
#include <stdio.h>


// Singletons
Console theConsole;
Logger theLogger;

void Console::setup(UartClass *_serial, unsigned long baudrate) {
    // first, for debug output
    serial = _serial;
    serial->begin(baudrate, SERIAL_8N1);

    rcvCharIndex = 0;
    stringReceived = false;
    // xmtCharIndex = 0 ;
    stringTransmitted = true;
}

void Console::printChar(char c) {
    serial->print(c);
    serial->flush(); // immediately visible
}

// printStr()
// write a string to serial
// manages correct CR/CRLF
// Place strings in flash memory:
// 	 call printStr(F("my string"))
//   see c:\Users\joerg\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino\Print.cpp
void Console::printStr(const __FlashStringHelper *str) {
    PGM_P p = reinterpret_cast<PGM_P>(str);
    // replace \n with \n\r
    while (1) {
        char c = pgm_read_byte(p++);
        if (c == 0)
            break;
        // print.cpp::: print(unsigned char) "unsigned char" as long ???
        serial->print(c);
        if (c == '\n')           // linefeed
            serial->print('\r'); // carriage return
    }
    serial->flush();
}

void Console::printStr(const char *str) {
    // replace \n with \n\r
    for (const char *c = str; *c; c++) {
        serial->print(*c);
        if (*c == '\n')          // linefeed
            serial->print('\r'); // carriage return
    }
    serial->flush();
}

void Console::printStrF(const char *msg, ...) {
    char buffer[256];
    va_list argptr;
    va_start(argptr, msg);
    vsnprintf(buffer, sizeof(buffer), msg, argptr);
    printStr(buffer);
    va_end(argptr);
}

// force format string to come from flash
void Console::printStrF(const __FlashStringHelper *flashmsg, ...) {
    // format string from flash to temporary ram
    char formatbuffer[128];
    PGM_P p = reinterpret_cast<PGM_P>(flashmsg);
    strncpy_P(formatbuffer, p, sizeof(formatbuffer));
    formatbuffer[sizeof(formatbuffer) - 1] = 0; // ensures formatbuffer is null terminated in the event that the string is longer than the buffer
    va_list argptr;
    va_start(argptr, flashmsg);
    char buffer[128];
    vsnprintf(buffer, sizeof(buffer), formatbuffer, argptr);
    printStr(buffer);
    va_end(argptr);
    /**/
}

// returns single char. -1 = none there
int Console::getChar() {
    if (!serial->available())
        return -1; // EOF
    return serial->read();
}

// get a user string from serial.
// Blocking, only exits when '\r' = "RETURN key" typed.
// '\r' not part of result string.
// echoes input, handles Backspace.
// consoleIdleFunc called on polling loop
void Console::getStr(char *buffer, int bufferSize, ConsoleIdleFunc consoleIdleFunc) {
    int wi = 0; // write index in buffer
    bool ready = false;

    while (!ready) {

        if (consoleIdleFunc != nullptr)
            consoleIdleFunc();

        // read a single char, wait endless blocking
        if (!serial->available())
            continue; // busy waiting

        // at least 1 char there, never returns by timeout
        int c = serial->read();
        switch (c) {
        case EOF: // should not happen
            break;
        case 0x08:
        case 0x7f: // not clear what the backspace key geenrates ... catch both.
            if (wi > 0) {
                // BACKSPACE and already something in buffer
                // erase visual and in buffer
                wi--;
                printStr("\b \b"); // terminal backspace sequence settable ?
            }
            break;
        case '\r': // terminal emulator carriage return 0x0d = "Enter" key
        case '\n': // Arduino IDE serial monitor
            // ready. Terminating char not part of string
            buffer[wi] = 0;
            ready = true;
            break;
        default: // add chars, when buffer not yet full
            if (wi < (bufferSize - 1)) {
                buffer[wi++] = c;
                printChar(c); // visual echo
            }
        }
    }
}

void Console::getStr(char *buffer, int bufferSize) {
    getStr(buffer, bufferSize, nullptr);
}

// probe wether a char waits
bool Console::kbhit() {
    return serial->available();
}

// Buffered Background Operation
// check for receive, add to receive buffer
void Console::loopRcv() {
    int c;
    // put next received chars into receive buffer

    // lock: rcv string must be processed and emptied by caller first
    if (stringReceived)
        return ;

    // poll until interrupt buffer emptied or CR/LF
    do {
        c = getChar();
        if (c != EOF) {
			//	  LOG("%c", (int)c);
            // still data there
            if (c == '\n' || c == '\r')
                stringReceived = true; // must be emptied by caller
            else if (rcvCharIndex < rcvBufferLen)
                rcvBuffer[rcvCharIndex++] = (char)c;
            rcvBuffer[rcvCharIndex] = 0; // trunc long receive data?!
        }
    } while(!stringReceived && c != EOF) ;
}

// Buffered Background Operation
// send a char, if possible and something in xmtBuffer
void Console::loopXmt() {
    // check if all chars have been transmitted
    if (serial->availableForWrite() >= (SERIAL_TX_BUFFER_SIZE - 1)) {
        stringTransmitted = true;
    }
}

// result valid until next loop()
char *Console::getAndClearRcvString() {
    stringReceived = false;
    rcvCharIndex = 0;
    return rcvBuffer;
}

// start new xmt buffer. Add new line \n
// use serial buffering, no own logic.
// only call when stringTransmitted
// can sent max SERIAL_TX_BUFFER_SIZE

void Console::xmtLine(const char *message) {
    strncpy(xmtBuffer, message, xmtBufferLen - 2);
    xmtBuffer[xmtBufferLen - 2] = 0; // terminate if trunced
    strcat(xmtBuffer, "\n\r");
    serial->write(xmtBuffer); // non blocking if len < SERIAL_TX_BUFFER_SIZE
    // LOG("%s", xmtBuffer) ;
    stringTransmitted = false;
}
