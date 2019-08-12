#include "aquadopp.h"

Aquadopp::Aquadopp() {
  this->serial = new RawSerial(PC0, PC1, 9600);
}

/* Remove spaces from char array */
void Aquadopp::remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

/* Parse Aquadopp ascii format */
measurement_t Aquadopp::Parse(char *d, int len) {
  char              *token;
  char              *line;
  char              *tmp;
  int               lineCount, headerCount;
  int               dataLineCount;
  measurement_t     measurement;
  
  if (d == NULL) {
    printf("Input to parse is NULL \n");
    measurement.error = true;
    return measurement;
  }

  lineCount = 0;
  headerCount = 0;
  dataLineCount = 0;

  while ((token = strsep(&d, "\n")) != NULL) {
    line = strdup(token);
    if (lineCount == 0) {
      /* Parse header */
      while((tmp = strsep(&line, " ")) != NULL) {
        this->remove_spaces(tmp);

        if(strlen(tmp) == 0) {
          continue;
        }

        switch (headerCount)
        {
          case MONTH: {
            measurement.month = atoi(tmp);
            headerCount++;
            break;
          }
          case DAY: {
            measurement.day = atoi(tmp);
            headerCount++;
            break;
          }
          case YEAR: {
            measurement.year = atoi(tmp);
            headerCount++;
            break;
          }
          case MINUTE: {
            measurement.minute = atoi(tmp);
            headerCount++;
            break;
          }
          case HOUR: {
            measurement.hour = atoi(tmp);
            headerCount++;
            break;
          }
          case SECOND: {
            measurement.second = atoi(tmp);
            headerCount++;
            break;
          }
        }
      }
    }
    else {
      /* Parse data lines */
      cell_t cell;

      while((tmp = strsep(&line, " ")) != NULL) {
        this->remove_spaces(tmp);
      
        if (strlen(tmp) == 0) {
          continue;
        }

        switch (dataLineCount)
        {
          case CELL_NUMBER: {
            cell.cell = atoi(tmp);
            break;
          }
          case SPEED: {
            cell.speed = atoi(tmp);
            break;
          }
          case DIRECTION: {
            cell.direction = atoi(tmp);
            break;
          }
        }

        dataLineCount++;
      }
      measurement.cells.push_back(cell);
    }

    free(line);
    lineCount++;
    dataLineCount = 0;
  }

  measurement.error = false;

  if (measurement.cells.size() <= 0) {
    measurement.error = true;
  }

  return measurement;
}

/* Read data from serial port */
measurement_t Aquadopp::Read() {
  char                    data[1024] = {0};
  measurement_t           measurement;
  int                     i = 0;
  Timer                   t;

  printf("Waiting for data \n");

  while(true) {
      if (this->serial->readable()) {
          t.start();
          uint8_t rx = this->serial->getc();
          data[i] += rx;
          i++;
      }

      if(t.read() > 2.0) {
          break;
      }
  }

  measurement = this->Parse(data, i);

  return measurement;
}