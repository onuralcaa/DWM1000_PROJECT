
#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"



// connection pins
const uint8_t PIN_RST = PB9; // reset pin
const uint8_t PIN_IRQ = PB8; // irq pin
const uint8_t PIN_SS = PA4;   // spi select pin

// TAG antenna delay defaults to 16384
// leftmost two bytes below will become the "short address"
char tag_addr[] = "7D:00:22:EA:82:60:3B:9C";

float mesafe, dist;
byte adres;

double nokta_1 = 0;
double nokta_2 = 0;

double nokta_1f = 0;
double nokta_2f = 0;

double cos_a = 0;
double sin_a = 0;
double a, b, c;

double x, y;


//******************************************************************
float readings1[20]; // Buffer to store readings
int index1 = 0;      // Index of the last reading
int num_readings1 = 10; // Number of readings to use for median filter
float sorted_readings1[10]; // Buffer to store sorted readings
float median1 = 0;   // Median of the last num_readings readings
//*****************************************************************
float readings2[20]; // Buffer to store readings
int index2 = 0;      // Index of the last reading
int num_readings2 = 10; // Number of readings to use for median filter
float sorted_readings2[10]; // Buffer to store sorted readings
float median2 = 0;   // Median of the last num_readings readings
//*****************************************************************


void setup()
{
  Serial.begin(57600);
  delay(1000);

  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  // start as tag, do not assign random short address

  DW1000Ranging.startAsTag(tag_addr, DW1000.MODE_LONGDATA_RANGE_ACCURACY, false);
}

void loop()
{
  DW1000Ranging.loop();
}



void newRange()
{
  //Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  adres = DW1000Ranging.getDistantDevice()->getShortAddress();
  //Serial.print(",");
  dist = DW1000Ranging.getDistantDevice()->getRange();
  //Serial.print(dist);
  //Serial.println(" | ");

  if (adres == 130)
  {
    nokta_1 = dist;
        nokta_1 *= 100;
  }
  else if (adres == 132)
  {
    nokta_2 = dist;
      nokta_2 *= 100;
  }


//filter application**********************************************************
  nokta_1f = (nokta_1f * 0.75) + (nokta_1 * 0.25);
  nokta_2f = (nokta_2f * 0.75) + (nokta_2 * 0.25);
  //**********************************************************************************************
  
   //**************************************************************************
  
  readings1[index1] = nokta_1f;
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

  
  if (num_readings1 % 2 == 0) {
    median1 = (sorted_readings1[num_readings1/2 - 1] + sorted_readings1[num_readings1/2]) / 2;
  } else {
    median1 = sorted_readings1[num_readings1/2];
  }
  //******************************************************************************

  //**************************************************************************
  
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
  c = 300;

  cos_a = (pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b * c);
  sin_a = sqrt(abs(1 - pow(cos_a, 2)));

  x = b * cos_a;
  y = b * sin_a;

  int son_x = round(x);
  int son_y = round(y);



  Serial.print(son_x);
  Serial.print("-");
  Serial.print(son_y);
  Serial.print("-");

  Serial.print(" Nokta-1 ad:82 ");
  Serial.print((int)median1); //offset not assigned
  Serial.print(" cm");

  Serial.print(" Nokta-1 ad:84 ");
  Serial.print((int)median2); //offset not assigned
  Serial.print(" cm");

  Serial.println();
  delay(1);
}

void newDevice(DW1000Device *device)
{
//  Serial.print("Device added: ");
//  Serial.print(device->getShortAddress(), HEX);
//  Serial.println("   ");
}

void inactiveDevice(DW1000Device *device)
{
//  Serial.print("delete inactive device: ");
//  Serial.println(device->getShortAddress(), HEX);
}
