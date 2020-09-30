#ifndef scrc_node_h
#define scrc_node_h

#include <unistd.h>

struct sensors_data {
    // DHT22
    uint16_t temp;    // confirmed
    uint16_t rh;      // confirmed
    
    // SDS011
    uint32_t pm25;
    uint32_t pm10;

    // T6713-5K
    uint16_t CO2;     // confirmed
    
    // MAX4466
    uint16_t noise;   // confirmed

    // Multi Channel Groove Gas 
    uint32_t no2;        // confirmed
    uint32_t nh3;        // confirmed
    uint32_t co;         // confirmed
    uint32_t so2;

    // soft sensors
    uint32_t aqi;
    uint32_t aql;
    uint32_t aqimp;
    
    uint64_t time_stamp;  // timestamp number
};

#endif /* scrc_node_h */
