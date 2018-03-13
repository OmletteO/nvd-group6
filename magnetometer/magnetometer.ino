#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <secTimer.h> /* This is the library I downloaded to record time. It gives the output in seconds */

#define THRESHOLD_VALUE 150 /* This is the max/min value if exceeded indicates that a car is in the spot. This value varies from place to place and needs to be calibrated accordingly */

secTimer myTimer; 
unsigned long time_total = 0; /* variable where the time elapsed gets stored once a car is parked */
unsigned long time_min = 0;
unsigned long time_sec = 0;
int count = 0;

/* BLOCK 1 */
/* Following block is code that came with the sensor to initialzie the sensor and check if connected properly */
/*---------------------------------------------------------------------------------------------------------------------*/
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345); /* Assign a unique ID to this sensor at the same time */
 
void setup(void) 
{
  Serial.begin(9600);
  
  /*Intro message */
  Serial.println("Welcome to Omlette!");
  Serial.println("Make sure you have calibrated the threshold value before you begin testing.");
  Serial.println("Sensor is wired correctly.");
  Serial.println("The code is currently running so test away!");
  Serial.println("_________________________________________________________");
  Serial.print('\n');
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    Serial.println("Oops, no LSM303 detected. Check your wiring!"); /* Checks if sensor connected properly */
    while(1);
  }
}
/*---------------------------------------------------------------------------------------------------------------------*/

/* Main loop begins */

void loop(void) 
{

/* BLOCK 2 */
/* Following block is code that came with the sensor to convert the coordinate values to degrees */
/*---------------------------------------------------------------------------------------------------------------------*/  
  sensors_event_t event; /* Get a new sensor event */ 
  mag.getEvent(&event);
  
  float Pi = 3.14159;
  float sensor_output = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi; /* Calculate the angle of the vector y,x */
 
  if (sensor_output < 0)
  {
    sensor_output = 360 + sensor_output; /* Normalize to 0-360 */
  }
/*---------------------------------------------------------------------------------------------------------------------*/  
  
 
 /* BLOCK 3 - CALIBRATION BLOCK */
/* Following block is used ONLY for CALIBRATION of the THRESHOLD_VALUE. Use only at the beginning then turn back into comments for the actual run. */
/*---------------------------------------------------------------------------------------------------------------------*/  

/*
 Serial.println("CALIBRATION MODE");
 Serial.print("Compass Heading: ");
 Serial.println(sensor_output);
 delay(500);
*/

/*---------------------------------------------------------------------------------------------------------------------*/  

 /* BLOCK 4 */
/* Following block is code for the actual run. It constantly compares the output value to the threshold value, starts/stops the timer and outputs values accordingly.  */
/*---------------------------------------------------------------------------------------------------------------------*/  

/* When car has parked */
if(sensor_output > THRESHOLD_VALUE && count ==0)
{
  Serial.println("A car has parked. Spot occupied. Timer initiated.");
  Serial.print('\n');
  myTimer.startTimer();
  count = count + 1;
}

/* When car has left*/
if(sensor_output < THRESHOLD_VALUE && count >= 1 && sensor_output > 1) /* Remember to ask me why I added the "sensor_output >1" condition */
{
  Serial.println("The car has left. Timer terminated.");
  myTimer.stopTimer();
  count = 0;
  time_total = myTimer.readTimer();
  time_min = time_total/60;
  time_sec = time_total%60;
  Serial.print("The car was parked for ");
  Serial.print(time_min);
  Serial.print(" minutes and ");
  Serial.print(time_sec);
  Serial.println(" seconds.");
  Serial.print('\n');
  Serial.println("Spot vacant. Waiting for the next car to arrive...");
  Serial.println("_________________________________________________________");
  Serial.print('\n');
}
/*---------------------------------------------------------------------------------------------------------------------*/  
}
