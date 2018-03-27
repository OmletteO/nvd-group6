#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
//#include <secTimer.h>
#include <MKRWAN.h>


/******************************************************************************************/
//LoRaModem modem;

// Uncomment if using the Murata chip as a module
LoRaModem modem(Serial1);

String appEui = String("70B3D57ED000A90A");
String appKey = String("2E22D893E74691A8CC7320F4EE35EEFF");
String devAddr = String("2601184E");
String nwkSKey = String("C82161190DAF65A8D4B33B489ED339F9");
String appSKey = String("F7C5BAD47690A8111E5157ED4036BD48");

/******************************************************************************************/
/* Following block is code that came with the sensor to initialzie the sensor and check if connected properly */
/*---------------------------------------------------------------------------------------------------------------------*/
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345); /* Assign a unique ID to this sensor at the same time */
/*---------------------------------------------------------------------------------------------------------------------*/

#define THRESHOLD_VALUE 150 /* This is the max/min value if exceeded indicates that a car is in the spot. This value varies from place to place and needs to be calibrated accordingly */

int count = 0;

/******************************************************************************************/

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(115200);
//  while (!Serial);
//  Serial.println("Welcome to MKRWAN1300 first configuration sketch");
//  Serial.println("Register to your favourite LoRa network and we are ready to go!");
  // change this to your regional band (eg. US915, AS923,EU868 ...)
  if (!modem.begin(US915_HYBRID)) {
    //Serial.println("Failed to start module");
    while (1) {}
  };
//  Serial.print("Your module version is: ");
//  Serial.println(modem.version());
//  Serial.print("Your device EUI is: ");
//  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);

  //modem.minPollInterval(60);
  delay(3000);

    /*Intro message */
//  Serial.println("Welcome to Omlette!");
//  Serial.println("Make sure you have calibrated the threshold value before you begin testing.");
//  Serial.println("Sensor is wired correctly.");
//  Serial.println("The code is currently running so test away!");
//  Serial.println("_________________________________________________________");
//  Serial.print('\n');

  /* Initialise the sensor */
  if (!mag.begin())
  {
    //Serial.println("Oops, no LSM303 detected. Check your wiring!"); /* Checks if sensor connected properly */
    while (1);
  }

}

void loop() {


    /* Following block is code that came with the sensor to convert the coordinate values to degrees */
  /*---------------------------------------------------------------------------------------------------------------------*/
  sensors_event_t event; /* Get a new sensor event */
  mag.getEvent(&event);

  float Pi = 3.14159;
  float sensor_output = (atan2(event.magnetic.y, event.magnetic.x) * 180) / Pi; /* Calculate the angle of the vector y,x */

  if (sensor_output < 0)
  {
    sensor_output = 360 + sensor_output; /* Normalize to 0-360 */
  }

 /* BLOCK 3 - CALIBRATION BLOCK */
  /* Following block is used ONLY for CALIBRATION of the THRESHOLD_VALUE. Use only at the beginning then turn back into comments for the actual run. */
  /*---------------------------------------------------------------------------------------------------------------------*/

/*
  Serial.println("CALIBRATION MODE");
  Serial.print("Compass Heading: ");
  Serial.println(sensor_output);
  /*Serial.print("Timer: ");
  time_total = myTimer.readTimer(); 
  Serial.println(time_total); */
  delay(500);
/*

  /*---------------------------------------------------------------------------------------------------------------------*/

  /* BLOCK 4 */
  /* Following block is code for the actual run. It constantly compares the output value to the threshold value, starts/stops the timer and outputs values accordingly.  */
  /*---------------------------------------------------------------------------------------------------------------------*/

  /* When car has parked */
  if (sensor_output > THRESHOLD_VALUE && count == 0)
  {
//    Serial.println("A car has parked. Spot occupied. Timer initiated.");
//    Serial.print('\n');
    count = count + 1;

    String msg = "in";
//    Serial.println();
//    Serial.print("Sending: " + msg + " - ");
//    for (unsigned int i = 0; i < msg.length(); i++) {
//      Serial.print(msg[i] >> 4, HEX);
//      Serial.print(msg[i] & 0xF, HEX);
//      Serial.print(" ");
//    }
//    Serial.println();

    modem.beginPacket();
    modem.print(msg);
    int err = modem.endPacket(true);
    delay(1000);
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(true);
    delay(1000);
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(true);
    delay(1000);
  }

  /* When car has left*/
  if (sensor_output < THRESHOLD_VALUE && count >= 1 && sensor_output > 1) /* Remember to ask me why I added the "sensor_output >1" condition */
  {
    count = 0;
//    Serial.print('\n');
//    Serial.println("The car has left. Timer terminated.");

    String msg = "out";
//    Serial.println();
//    Serial.print("Sending: " + msg + " - ");
//    for (unsigned int i = 0; i < msg.length(); i++) {
//      Serial.print(msg[i] >> 4, HEX);
//      Serial.print(msg[i] & 0xF, HEX);
//      Serial.print(" ");
//    }
//    Serial.println();
    modem.beginPacket();
    modem.print(msg);
    int err = modem.endPacket(true);
    delay(1000);
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(true);
    delay(1000);
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(true);
    delay(1000);
  }
}
