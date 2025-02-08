/* console.h - handling of user RS232, buffered background operation


 */
#if !defined(__CONSOLE_H__)
#define __CONSOLE_H__
#include <UART.h>
#include "messages.h"

// function to be called on idle input loop
typedef void (*ConsoleIdleFunc)(void);

class Console {
  public:
    UartClass *serial; // interface to use
    void setup(UartClass *_serial, unsigned long baudrate);
    void printChar(char c);
    void printStr(const char *str);
    void printStr(const __FlashStringHelper *str);

    void printStrF(const char *msg, ...);
    void printStrF(const __FlashStringHelper *msg, ...);
    bool kbhit(); // probe wether a char waits
    int getChar();
    void getStr(char *buffer, int bufferSize);
    void getStr(char *buffer, int bufferSize, ConsoleIdleFunc consoleIdleFunc);

  private:
    // buffer sizes: longest message + reserve
    static const int rcvBufferLen = Message::txtBufferSize +10 ;// 128;
    int rcvCharIndex;
    char rcvBuffer[rcvBufferLen];

    static const int xmtBufferLen = Message::txtBufferSize +10 ;// 128;
    char xmtBuffer[xmtBufferLen];
    // int 	xmtCharIndex ;

  public:
    void loop(); // background line string buffering
    bool stringReceived;

    char *getAndClearRcvString();

    bool stringTransmitted;

    void xmtLine(const char *message);
};

// logger is output to the USB RS232, caught in IDE, std settings 8-N-1,
class Logger : private Console {
  public:
    void setup() // initialzation
    {
        // see platform.io monitor_speed = 115200
        Console::setup(&Serial, 115200);
    }

    // make just printStr() public
    using Console::printStr;
    using Console::printStrF;
};

// Singletons
extern Console theConsole;
extern Logger theLogger;

///// compact access to Logger
#define LOG(...) theLogger.printStrF(__VA_ARGS__)

#endif // __CONSOLE_H__
