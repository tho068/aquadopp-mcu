#include "mbed.h"
#include <string>
#include <string.h>
#include <vector>

enum {
    MONTH = 0,
    DAY = 1,
    YEAR = 2,
    HOUR = 3,
    MINUTE = 4,
    SECOND = 5
};

enum {
    CELL_NUMBER = 0,
    SPEED = 1,
    DIRECTION = 2
};

typedef struct {
    int speed;
    int direction;
    int cell;
} cell_t;

typedef struct {
    int month;
    int day;
    int second;
    int minute;
    int hour;
    int year;
    bool error;
    std::vector<cell_t> cells;
} measurement_t;

class Aquadopp {
    public:
        Aquadopp();
        measurement_t Read();
        measurement_t Parse(char *d, int i);

        RawSerial *serial;

    private:
        void remove_spaces(char* s);
};