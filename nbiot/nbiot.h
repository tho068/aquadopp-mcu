#include "mbed.h"
#include "ATCmdParser.h"

#define IP_ADDRESS "213.239.106.42"
#define PORT 8080

class Nbiot {
    public:
        Nbiot();
        bool Connect();
        bool TransmitData(char *data, int len);

    private:
        UARTSerial              *serial;
        ATCmdParser         *parser;
        unsigned int         fd;
};