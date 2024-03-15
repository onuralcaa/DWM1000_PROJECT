#include <deprecated.h>
#include <DW1000.h>
#include <DW1000CompileOptions.h>
#include <DW1000Constants.h>
#include <DW1000Device.h>
#include <DW1000Mac.h>
#include <DW1000Ranging.h>
#include <DW1000Time.h>
#include <require_cpp11.h>

/**

   @todo
    - move strings to flash (less RAM consumption)
    - fix deprecated convertation form string to char* startAsTag
    - give example description
*/
#include <SPI.h>
#include "DW1000Ranging.h"

// connection pins
const uint8_t PIN_RST = PB9; // reset pin
const uint8_t PIN_IRQ = PB8; // irq pin
const uint8_t PIN_SS = PA4; // spi select pin

float mesafe, dist;
int adres;
int power;

double nokta_1 = 0;
double nokta_2 = 0;
double nokta_3 = 0;

double nokta_1f, nokta_2f, nokta_3f;

double cos_a = 0;
double sin_a = 0;
double a, b, c;

double x, y;


//******************************************************************
float readings1[10]; // Buffer to store readings
int index1 = 0;      // Index of the last reading
int num_readings1 = 10; // Number of readings to use for median filter
float sorted_readings1[10]; // Buffer to store sorted readings
float median1 = 0;   // Median of the last num_readings readings
//*****************************************************************
float readings2[10]; // Buffer to store readings
int index2 = 0;      // Index of the last reading
int num_readings2 = 10; // Number of readings to use for median filter
float sorted_readings2[10]; // Buffer to store sorted readings
float median2 = 0;   // Median of the last num_readings readings
//*****************************************************************


void setup() {
  Serial.begin(57600);
  delay(1000);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  DW1000Ranging.useRangeFilter(true);

  //we start the module as a tag
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  //  Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  //  Serial.print("\t Range: "); Serial.print(DW1000Ranging.getDistantDevice()->getRange()); Serial.print(" m");
  //  Serial.print("\t RX power: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");

  adres = DW1000Ranging.getDistantDevice()->getShortAddress();
  dist = DW1000Ranging.getDistantDevice()->getRange();
  power = DW1000Ranging.getDistantDevice()->getRXPower();

  if (adres == 6018)
  {
    nokta_1 = dist;
    nokta_1 *= 100;
    nokta_1 -= 65;
  }
  else if (adres == 6020)
  {
    nokta_2 = dist;
    nokta_2 *= 100;
    nokta_2 -= 80;
  }
  else if (adres == 6022)
  {
    nokta_3 = dist;
    nokta_3 *= 100;
    nokta_3 -= 80;

  }

  //nokta değişkenlerine filtre uygulaması**********************************************************
  nokta_1f = (nokta_1f * 0.75) + (nokta_1 * 0.25);
  nokta_2f = (nokta_2f * 0.75) + (nokta_2 * 0.25);
  nokta_3f = (nokta_3f * 0.75) + (nokta_3 * 0.25);
  //**********************************************************************************************


  //**************************************************************************
  // Add the new measurement to the buffer
  readings1[index1] = nokta_1f - 23;
  index1 = (index1 + 1) % num_readings1;

  // Sort the readings
  for (int i = 0; i < num_readings1; i++) {
    sorted_readings1[i] = readings1[i];
  }
  for (int i = 0; i < num_readings1 - 1; i++) {
    for (int j = i + 1; j < num_readings1; j++) {
      if (sorted_readings1[i] > sorted_readings1[j]) {
        float temp1 = sorted_readings1[i];
        sorted_readings1[i] = sorted_readings1[j];
        sorted_readings1[j] = temp1;
      }
    }
  }

  // Compute the median
  if (num_readings1 % 2 == 0) {
    median1 = (sorted_readings1[num_readings1/2 - 1] + sorted_readings1[num_readings1/2]) / 2;
  } else {
    median1 = sorted_readings1[num_readings1/2];
  }
  //******************************************************************************

  //**************************************************************************
  // Add the new measurement to the buffer
  readings2[index2] = nokta_2f;
  index2 = (index2 + 1) % num_readings2;

  // Sort the readings
  for (int k = 0; k < num_readings2; k++) {
    sorted_readings2[k] = readings2[k];
  }
  for (int k = 0; k < num_readings2 - 1; k++) {
    for (int m = k + 1; m < num_readings2; m++) {
      if (sorted_readings2[k] > sorted_readings2[m]) {
        float temp2 = sorted_readings2[k];
        sorted_readings2[k] = sorted_readings2[m];
        sorted_readings2[m] = temp2;
      }
    }
  }

  // Compute the median
  if (num_readings2 % 2 == 0) {
    median2 = (sorted_readings2[num_readings2/2 - 1] + sorted_readings2[num_readings2/2]) / 2;
  } else {
    median2 = sorted_readings2[num_readings2/2];
  }
  //******************************************************************************
  

  a = median2;
  b = median1;
  c = 100;

  cos_a = (pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b * c);
  sin_a = sqrt(1 - pow(cos_a, 2));

  x = b * cos_a;
  y = b * sin_a;

  int son_x = round(x);
  int son_y = round(y);





  //-------------------------------------
  Serial.print(son_x);
  Serial.print("   ");
  Serial.print(son_y);

  Serial.print("    nokta 1: ");
  Serial.print((int)median1);
  Serial.print(" cm");

  Serial.print("    nokta 2: ");
  Serial.print((int)median2);
  Serial.print(" cm");

  Serial.print("    nokta 3: ");
  Serial.print((int)nokta_3);
  Serial.print(" cm");

  Serial.println();

  //-------------------------------------
}

void newDevice(DW1000Device* device) {
  Serial.print("ranging init; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}
