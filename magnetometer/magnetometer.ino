/*
  First Configuration
  This sketch demonstrates the usage of MKR WAN 1300 LoRa module.
  This example code is in the public domain.
*/

#include <MKRWAN.h>

//LoRaModem modem;

// Uncomment if using the Murata chip as a module
LoRaModem modem(Serial1);

String appEui = String("70B3D57ED000A90A");
String appKey = String("2E22D893E74691A8CC7320F4EE35EEFF");
String devAddr = String("2601184E");
String nwkSKey = String("C82161190DAF65A8D4B33B489ED339F9");
String appSKey = String("F7C5BAD47690A8111E5157ED4036BD48");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Welcome to MKRWAN1300 first configuration sketch");
  Serial.println("Register to your favourite LoRa network and we are ready to go!");
  // change this to your regional band (eg. US915, AS923,EU868 ...)
  if (!modem.begin(US915_HYBRID)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int mode = 0;
  while (mode != 1 && mode != 2) {
    Serial.println("Are you connecting via OTAA (1) or ABP (2)?");
    while (!Serial.available());
    mode = Serial.readStringUntil('\n').toInt();
  }

  int connected;
  if (mode == 1) {
//    Serial.println("Enter your APP EUI");
//    while (!Serial.available());
//    appEui = Serial.readStringUntil('\n');
//
//    Serial.println("Enter your APP KEY");
//    while (!Serial.available());
//    appKey = Serial.readStringUntil('\n');
//
//    appKey.trim();
//    appEui.trim();

    connected = modem.joinOTAA(appEui, appKey);
  } else if (mode == 2) {

//    Serial.println("Enter your Device Address");
//    while (!Serial.available());
//    devAddr = Serial.readStringUntil('\n');
//
//    Serial.println("Enter your NWS KEY");
//    while (!Serial.available());
//    nwkSKey = Serial.readStringUntil('\n');
//
//    Serial.println("Enter your APP SKEY");
//    while (!Serial.available());
//    appSKey = Serial.readStringUntil('\n');
//
//    devAddr.trim();
//    nwkSKey.trim();
//    appSKey.trim();

    connected = modem.joinABP(devAddr, nwkSKey, appSKey);
  }

  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  modem.minPollInterval(60);

  delay(5000);

//  int err;
//  //modem.setPort(3);
//  modem.beginPacket();
//  modem.print("HeLoRA world!/n");
//  err = modem.endPacket(true);
//  if (err > 0) {
//    Serial.println("Message sent correctly!");
//  } else {
//    Serial.println("Error sending message :(");
//  }
}

void loop() {

  Serial.println();
  Serial.println("Enter a message to send to network");
  Serial.println("(make sure that end-of-line 'NL' is enabled)");

  while (!Serial.available());
  String msg = Serial.readStringUntil('\n');

  Serial.println();
  Serial.print("Sending: " + msg + " - ");
  for (unsigned int i = 0; i < msg.length(); i++) {
    Serial.print(msg[i] >> 4, HEX);
    Serial.print(msg[i] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
  int err;
  modem.beginPacket();
  modem.print(msg);
  err = modem.endPacket(true);
//  if (err > 0) {
//    Serial.println("Message sent correctly!");
//  } else {
//    Serial.println("Error sending message :(");
//    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
//    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
//  }
  delay(1000);
  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    return;
  }
  String rcv;
  rcv.reserve(64);
  while (modem.available()) {
    rcv += (char)modem.read();
  }
  Serial.print("Received: " + rcv + " - ");
  for (unsigned int i = 0; i < rcv.length(); i++) {
    Serial.print(rcv[i] >> 4, HEX);
    Serial.print(rcv[i] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
}
