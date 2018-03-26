////////////////////////////////Declaration for Wifi////////////////////////////////////
/*
 * This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
 * using the CC3000 WiFi chip. See the README file for more details.
 * 
 * Written in 2014 by Marco Schwartz under a GPL license.
 */

// Import required libraries
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <string.h>
#include <ccspi.h>
#include <avr/wdt.h>

// These are the pins for the CC3000 chip if you are using a breakout board
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

// Create CC3000 instance
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
SPI_CLOCK_DIV2);

// WiFi SSID and password
#define WLAN_SSID       "AndroidAP"
#define WLAN_PASS       "team1234"
#define WLAN_SECURITY   WLAN_SEC_WPA2

// Set up cc3000
uint32_t output;

#define PROCESSING_IP "192.168.43.12" // IP
// TCP port
uint16_t PORT = 5204;
#define IDLE_TIMEOUT_MS  3000

Adafruit_CC3000_Client client;

const int multiPin = A1;

#define DEBUG 1

void setup() {
    // Start Serial
    if (DEBUG) {
    Serial.begin(115200);
    }
      
    // Initialize wifi breakout
    wifiInitialize();
}

char *ptr = "0000000000";
/*
 * Read data until either the connection is closed, or the idle timeout is reached.
 */
void loop() {
    
   if (client.connected()) {
    
      sprintf(ptr, "Hello");
      client.write(ptr, 10, 0);
  }

  else {
      // Disconnect and close the client
      client.close();
      //Serial.println("\n\nDisconnecting");
      cc3000.disconnect();
  }
}

//////////////////////EXTERNAL FUNCTIONS////////////////////////////////////

/////////////////////PRINT FUNCTIONS/////////////////////////////////////////

/*
 * Print connection details of the CC3000 chip
 */
bool displayConnectionDetails(void) {
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if (!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    //Serial.println(("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    //Serial.print(("\nIP Addr: ")); 
    cc3000.printIPdotsRev(ipAddress);
    //Serial.print(("\nNetmask: ")); 
    cc3000.printIPdotsRev(netmask);
    //Serial.print(("\nGateway: ")); 
    cc3000.printIPdotsRev(gateway);
    //Serial.print(("\nDHCPsrv: ")); 
    cc3000.printIPdotsRev(dhcpserv);
    //Serial.print(("\nDNSserv: ")); 
    cc3000.printIPdotsRev(dnsserv);
    //Serial.println();
    return true;
  }
}
/////////////////////INITIALIZATION FUNCTIONS///////////////////////////////////

/*
 * Initialize the wifi
 * Code was taken from the webclient example from the adafruit cc3000 library and edited
 */
void wifiInitialize(void)
{
    // Set up CC3000
    if (!cc3000.begin())
    {
        //Serial.println("Couldn't begin! Check your wiring?");
        while (1);
    }
    Serial.println("Begin successful!");
  
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
        //Serial.println("Failed to connect to AP");
        while (1);
    }
    //Serial.println("Connection to AP is successful!");
   
    while (!cc3000.checkDHCP())
    {
        //Serial.println("checking DHP");
        delay(100);
    }
    //Serial.println();
    //Serial.println("Connected succesfully!!");
    // Send sound to indicate successful connection
    client.write("1000000000", 10, 0);
    // Display the IP address DNS, Gateway, etc.
    while (! displayConnectionDetails()) {
        delay(1000);
    }
  
    output = 0;
  
    // Checking the server's IP address
    //Serial.print(PROCESSING_IP);
    //Serial.print(" -> ");
    while (output == 0) {
        if (! cc3000.getHostByName(PROCESSING_IP, &output)) {
            //Serial.println("Couldn't find server at IP!!");
        }
        delay(500);
    }
    cc3000.printIPdotsRev(output);
  
    // Client instance
    client = cc3000.connectTCP(output, PORT);
    Serial.println("Instantiated client");
    // Try connecting to the website.
    if (client.connected()) {
        //Serial.print("Server:");
        //Serial.println(PROCESSING_IP);
    } 
    else {
        //Serial.println("Connection failed");
        return;
    }
}