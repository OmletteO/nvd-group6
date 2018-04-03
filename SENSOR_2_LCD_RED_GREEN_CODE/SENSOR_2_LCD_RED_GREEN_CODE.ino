#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SD.h>

#define THRESHOLD_VALUE 200
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_RST  9  // Reset line for TFT (or see below...)
#define TFT_DC   8  // Data/command line for TFT
#define SD_CS    4  // Chip select line for SD card

void bmpDraw(char *filename, uint8_t x, uint8_t y);
int count=0;

/* BLOCK 1B */
/* Following block is code that came with the sensor to initialzie the sensor and check if connected properly */
/*---------------------------------------------------------------------------------------------------------------------*/

Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setupSensor()
{
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
}

void setup(void) 
{

#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif
  Serial.begin(9600);

    /*Intro message */
  Serial.println("Welcome to DRIVEN!");
  Serial.println("Make sure you have calibrated the threshold value before you begin testing.");
  Serial.println("Sensor is wired correctly.");
  Serial.println("The code is currently running so test away!");
  Serial.println("_________________________________________________________");
  Serial.print('\n');
  
// Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... Sensor not connected properly. Check your wiring!");
    while (1);
  }

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  tft.initR(INITR_144GREENTAB);

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  tft.fillScreen(ST7735_GREEN);
}
/*---------------------------------------------------------------------------------------------------------------------*/

/* Main loop begins */



void loop() 
{ 
  /* BLOCK 2 */
  /* Following block is code that came with the sensor to convert the coordinate values to degrees */
  /*---------------------------------------------------------------------------------------------------------------------*/
  sensors_event_t accel, mag, gyro, temp; /* Get a new sensor event */
  lsm.getEvent(&accel, &mag, &gyro, &temp);

  float Pi = 3.14159;
  float sensor_output = (atan2(lsm.magData.y,lsm.magData.x) * 180) / Pi; /* Calculate the angle of the vector y,x */

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
  Serial.print("Sensor Output: ");
  Serial.println(sensor_output);
  delay(1000);
*/
  /*---------------------------------------------------------------------------------------------------------------------*/

/* BLOCK 4 */
  /* Following block is code for the actual run. It constantly compares the output value to the threshold value, starts/stops the timer and outputs values accordingly.  */
  /*---------------------------------------------------------------------------------------------------------------------*/
  /* When car has parked */

  if (sensor_output > THRESHOLD_VALUE && count == 0)
  {
    Serial.println("A car has parked. Spot occupied. Timer initiated.");
    Serial.print('\n'); 
    //bmpDraw("sprk24.bmp",0,0);
      tft.fillScreen(ST7735_RED);
    Serial.println("A.");
    count = count + 1;
  }

  /* When car has left*/
  if (sensor_output < THRESHOLD_VALUE && count >= 1 && sensor_output > 1) /* Remember to ask me why I added the "sensor_output >1" condition */
  {
    Serial.print('\n');
    Serial.println("The car has left. Timer terminated.");
    //bmpDraw("nprk24.bmp",0,0);
    tft.fillScreen(ST7735_GREEN);
    count = 0;
   /* Serial.println("Spot vacant. Waiting for the next car to arrive...");
    Serial.println("_________________________________________________________");
    Serial.print('\n');*/
  }
  /*---------------------------------------------------------------------------------------------------------------------*/

}

/* REST OF LCD CODE */
  // uncomment these lines to draw bitmaps in different locations/rotations!
/*
  tft.fillScreen(ST7735_BLACK); // Clear display
  for(uint8_t i=0; i<4; i++)    // Draw 4 parrots
    bmpDraw("parrot.bmp", tft.width() / 4 * i, tft.height() / 4 * i);
  delay(1000);
  tft.setRotation(tft.getRotation() + 1); // Inc rotation 90 degrees
*/

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
