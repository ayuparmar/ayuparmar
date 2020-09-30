

#include <stdlib.h>

#include "scrc_node.h"
#include "scrc_wifi.h"
#include "scrc_time.h"

#include "DHT.h"

#define DHTPIN D3                       // what pin we're connected to
#define DHTTYPE DHT22                   // DHT 22  (AM2302)


// sds011 sensor
#include <SDS011.h>


// CO2

#include <Wire.h>

//#define version "T6700-R15 I2C v1.0"
#define ADDR_6700  0x15 // default I2C slave address is 0x15

//Noise
const uint32_t sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
uint32_t sample;
unsigned long startMillis = millis();                  // Start of sample window
uint32_t peakToPeak = 0;                                  // peak-to-peak level

uint32_t signalMax = 0;                            //minimum value
uint32_t signalMin = 1024;                         //maximum value



// GAS
#include <MiCS6814-I2C.h>
MiCS6814 sensor;
#define SENSOR_ADDR     0X04        // default to 0x04



#define TIME_INTERVAL_15_SEC  15 * 1000
// #define TIME_INTERVAL_10_MIN  10 * 60 * 1000
// #define TIME_INTERVAL_1_HR    1 * 60* 60 * 1000
#define TIME_INTERVAL_10_MIN  30000
#define TIME_INTERVAL_1_HR    60000

// count of sensor reading in 1 hour @ 15 sec, i.e., 4 reading per min, (240)
#define COUNT_15_SEC_DATA  4 * 60

// count of 10 min avg sensor reading in 2 days, i.e., 6 reading per hour, (288 reading)
#define COUNT_10_MIN_DATA  6 * 24 * 2

// buffer to store above readings
struct sensors_data data_15_sec[COUNT_15_SEC_DATA];
struct sensors_data data_10_min[COUNT_10_MIN_DATA];

uint16_t idx_data_15_sec = 0;
uint16_t idx_data_10_min = 0;


DHT dht(DHTPIN, DHTTYPE);

uint16_t sum_temp = 0.0;
uint16_t sum_rh = 0.0;


SDS011 my_sds;
float p25, p10;
uint32_t P10, P25;


uint32_t data[4];

bool sensorConnected;

static bool wifi_connected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("***** Setup *****");

  int timeout = 10000;    // wifi connection timeout
  int retry_delay = 500; //wifi reconnection delay

  Serial.println(" ");
  Serial.println("d-1");
  nw_setup();

  Serial.println("d-2");
  wifi_connected = nw_start(timeout, retry_delay);
  //check if connected

  Serial.println("d-3");
  if (wifi_connected)
    Serial.println("wifi connected");
  else
    Serial.println("wifi not connected");

  Serial.println("d-4");
  sync_time(19800);
  uint64_t ts = get_timestamp();
  Serial.println("timestamp: " + get_timestamp_str(ts));
  Serial.print("timestamp uint64_t: ");
  print_uint64_t(ts);
  Serial.println("Date :" + get_datestamp_str(ts));
  Serial.print("timestamp uint64_t: ");
  print_uint64_t(ts);
  Serial.println("");

  dht.begin();
  my_sds.begin(D7, D8);
  Wire.begin();

  // GAS SETUP
  sensorConnected = sensor.begin(SENSOR_ADDR);

  if (sensorConnected == true) {
    // Print status message
    Serial.println("Connected to MiCS-6814 sensor");

    // Turn heater element on
    sensor.powerOn();

    // Print header for live values
    Serial.println("Current concentrations:");
    Serial.println("CO\tNO2\tNH3\tC3H8\tC4H10\tCH4\tH2\tC2H5OH");
  } else {
    // Print error message on failed connection
    Serial.println("Couldn't connect to MiCS-6814 sensor");
  }

  Serial.println("***** Endof setup *****");
}


void loop() {
  //read_data_from_hw();
  //calc_10_min_avg();
  //pub_15_sec_data();
  //pub_10_min_data();

  uint64_t ts = get_timestamp();
  Serial.println("timestamp: " + get_timestamp_str(ts));
  Serial.println("Date :" + get_datestamp_str(ts));
  delay(2000);
}

void read_data_from_hw() {
  static uint32_t last_ms_read_data_15 = 0;
  if (millis() - last_ms_read_data_15 > TIME_INTERVAL_15_SEC) {
    last_ms_read_data_15 = millis();
    uint64_t time_stamp = get_timestamp();
    print_uint64_t(time_stamp);
    // Serial.print(time_stamp);
    data_15_sec[idx_data_15_sec].time_stamp = time_stamp;

    


    // read data from hardware
    hw_read_dht();
    hw_read_sds();
    hw_read_co2();
    hw_read_noise();
    hw_read_gas();
    hw_read_so2();
    //    compute_index_aqi();
    //    save_timestamp();

    if (idx_data_15_sec == COUNT_15_SEC_DATA)
      idx_data_15_sec = 0;
    else
      idx_data_15_sec++;
  }

}

void calc_10_min_avg() {
  static uint32_t last_ms_read_data_10 = 0;
  if (millis() - last_ms_read_data_10 > TIME_INTERVAL_15_SEC) {
    last_ms_read_data_10 = millis();

    uint16_t sum_temp = 0;
    uint16_t sum_rh = 0;
    // compute 10 min avg
    for (int i = idx_data_15_sec; i <= COUNT_15_SEC_DATA ; i++) {

      sum_temp += data_15_sec[i].temp;
      sum_rh += data_15_sec[i].rh;

      if (i > COUNT_15_SEC_DATA)
        i = 0;
    }

    data_10_min[idx_data_10_min].temp = sum_temp / COUNT_15_SEC_DATA ;
    data_10_min[idx_data_10_min].rh = sum_rh / COUNT_15_SEC_DATA ;


    if (idx_data_10_min == TIME_INTERVAL_10_MIN)
      idx_data_10_min = 0;
    else
      idx_data_10_min++;
  }
}
void pub_15_sec_data() {
  static uint32_t last_ms_pub_15 = 0;
  if (millis() - last_ms_pub_15 > TIME_INTERVAL_1_HR) {
    last_ms_pub_15 = millis();

    for (int i = idx_data_15_sec;  i <= COUNT_15_SEC_DATA ; i++) {

      // publish_onem2m(data_15_sec);
      publish_onem2m();
      long int tim = millis() - last_ms_pub_15 ;
      Serial.println(tim);
      if (i > COUNT_15_SEC_DATA)
        i = 0;
    }
  }
}

void pub_10_min_data() {

  static uint32_t last_ms_pub_10 = 0;
  if (millis() - last_ms_pub_10 > TIME_INTERVAL_1_HR) {
    last_ms_pub_10 = millis();

    for (int i = idx_data_10_min;  i <= COUNT_10_MIN_DATA ; i++) {

      // publish_onem2m(data_10_min);
      publish_onem2m();

      if (i > COUNT_10_MIN_DATA)
        i = 0;
    }
  }

}

void hw_read_dht() {
  Serial.print("...hw_read_dht");
  delay(200);
  for (int i = idx_data_15_sec; i <= COUNT_15_SEC_DATA; i++)
  {
    uint16_t temp = dht.readTemperature();
    data_15_sec[i].temp = temp;
    // Serial.print("data_15_sec[" + i );
    Serial.println(data_15_sec[i].temp);

    // Serial.println(temp);
    i++;

    uint16_t hum = dht.readHumidity();
    data_15_sec[i].rh = hum;
    Serial.println(hum);
  }
  /* temp = dht_read_temp()
    rh = dht_read_rh()

    data_15_sec[idx_data_15_sec]->temp = temp
    data_15_sec[idx_data_15_sec]->rh = rh */
}

void hw_read_sds() {
  Serial.print("...hw_read_sds");
  delay(200);

  for (int i = idx_data_15_sec; i <= COUNT_15_SEC_DATA; i++)
  {
    uint32_t error = my_sds.read(&p25, &p10);
    if (!error)
    {
      P10 = p10;
      data_15_sec[i].pm10 = P10;
      P25 = p25;
      data_15_sec[i].pm25 = P25;
      Serial.println("P10:");
      Serial.print( data_15_sec[i].pm10);
      Serial.println("P25:");
      Serial.print(data_15_sec[i].pm25);
    }
    else
      Serial.println("SDS not connected");

  }

}

void hw_read_co2() {
  Serial.print("...hw_read_co2");
  delay(200);

  for (int i = idx_data_15_sec; i <= COUNT_15_SEC_DATA; i++)
  {
    Wire.beginTransmission(ADDR_6700);
    Wire.write(0x04); Wire.write(0x13); Wire.write(0x8B); Wire.write(0x00); Wire.write(0x01);
    // end transmission
    Wire.endTransmission();
    // read report of current gas measurement in ppm
    delay(2000);
    Wire.requestFrom(ADDR_6700, 5);    // request 4 bytes from slave device
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    uint16_t CO2ppmvalue = ((data[2] * 0xFF ) + data[3]);
    data_15_sec[i].CO2 = CO2ppmvalue;
    Serial.println("CO2:");
    Serial.print( data_15_sec[i].CO2);

  }

}

void hw_read_noise() {
  Serial.print("...hw_read_noise");
  delay(200);

  for (int i = idx_data_15_sec; i <= COUNT_15_SEC_DATA; i++)
  {

    while (millis() - startMillis < sampleWindow)
    {
      sample = analogRead(A0);                             //get reading from microphone
      if (sample < 1024)                                   // toss out spurious readings
      {
        if (sample > signalMax)
        {
          signalMax = sample;                              // save just the max levels
        }
        else if (sample < signalMin)
        {
          signalMin = sample;                              // save just the min levels
        }
      }
    }
    peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
    uint16_t noise = map(peakToPeak, 20, 900, 49.5, 90);         //calibrate for deciBels
    data_15_sec[i].noise = noise;
    Serial.print("Noise Value: ");
    Serial.print( data_15_sec[i].noise);

  }
}

void hw_read_so2() {
  Serial.print("...hw_read_so2");
  delay(200);

}

void hw_read_gas() {
  Serial.print("...hw_read_gas");
  delay(200);


  for (int i = idx_data_15_sec; i <= COUNT_15_SEC_DATA; i++)
  {
    uint32_t co = sensor.measureCO();
    data_15_sec[i].co = co;
    Serial.print("CO Value: ");
    Serial.print( data_15_sec[i].co);

    uint32_t no2 = sensor.measureNO2();
    data_15_sec[i].no2 = no2;
    Serial.print("NO2 Value: ");
    Serial.print( data_15_sec[i].no2);

    uint32_t nh3 = sensor.measureNH3();
    data_15_sec[i].nh3 = nh3;
    Serial.print("NH3 Value: ");
    Serial.print( data_15_sec[i].nh3);

  }




}


void publish_onem2m()
{
  Serial.println("...pub start");
/*
 publish function
 */


  
Serial.println("...pub done");
  
}




void print_uint64_t(uint64_t num) {

  char rev[128];
  char *p = rev + 1;

  while (num > 0) {
    *p++ = '0' + ( num % 10);
    num /= 10;
  }
  p--;
  /*Print the number which is now in reverse*/
  while (p > rev) {
    Serial.print(*p--);
  }
}

/*
  void loop() {
  read_sensors_data(&data_15_sec[data_15_sec_next])
  if idx_data_15_sec % READING_30_MIN {
    avg_data(idx_data_15_sec, idx_data_30_min);

    if idx_data_30_min == SIZE_30_MIN_DATA:
      idx_data_30_min = 0
    else
      idx_data_30_min++
  }
  if idx_data_15_sec == SIZE_15_SEC_DATA:
    idx_data_15_sec = 0
  else
    idx_data_15_sec++
  }
  void read_sensors_data(sensors_data *ptr){
  hw_read_dht(ptr);
  hw_read_sds(ptr);

  }
  void avg_data(int idx_data_15_sec, int idx_data_30_min){
  for (i = (idx_data_15_sec - READING_30_MIN); i++; i <= idx_data_15_sec) {

        data_30_min->temp += data_15_sec->temp;
        data_30_min->rb += data_15_sec->rh;
  }
  data_30_min->temp = data_30_min->temp / READING_30_MIN;
  data_30_min->rb += data_30_min->rh / READING_30_MIN;

  }
*/
