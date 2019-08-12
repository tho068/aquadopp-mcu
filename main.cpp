#include "mbed.h"
#include "aquadopp.h"
#include "nbiot.h"
#include "ArduinoJson.h"

#include <stdlib.h>

/* Create a JSON string to transmit over nb-iot */
static char *create_json(measurement_t m, int *length) {
    cell_t                   current;
    char                     *payload;
    DynamicJsonDocument      doc(1024);

    JsonArray cells = doc.createNestedArray("cells");

    payload = (char *) calloc(1024, sizeof(char));

    for(unsigned int i = 0; i < m.cells.size(); i++) {
        current = m.cells[i];

        JsonObject o = cells.createNestedObject();

        o["#"] = current.cell;
        o["s"] = current.speed;
        o["d"] = current.direction;
    }

    *length = serializeJson(doc, (char *)payload, 1024);
    
    return payload;
}

int main(int argc, char **argv) {

    Nbiot                   nbiot;
    Aquadopp                aquadopp;

    measurement_t           measurement;
    int                     data_len;

    data_len = 0;

    /* Register with the network */
    nbiot.Connect();

    while(true) {
        measurement = aquadopp.Read();
        if (!measurement.error) {
            /* No error detected, transmit the data */
            char *data = create_json(measurement, &data_len);

            printf("length of data: %d \n");
            printf("Data: %s \n", data);

            nbiot.TransmitData(data, data_len);

            free(data);
        }
    }

    return 1;
}
