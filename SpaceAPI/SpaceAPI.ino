#include <OneWire.h>
#include <DallasTemperature.h>
#include <EtherCard.h>
#include "secrets.h"

#define DEBUG false // ignore network if true

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

// CONFIG:
#define BAUDRATE 9600
//const char api[]      PROGMEM = "spaceapi.net";
//const char endpoint[] PROGMEM = "/new/space/losfuzzyslab/sensor/set";
const char api[]      PROGMEM = "think.2904.cc";
const char endpoint[] PROGMEM = "/losfuzzys/index.php";

#define PIN_ONEWIRE 2
#define TEMPERATURE_PRECISION 9 // Lower resolution

// define TOKEN in secrets.h

#define DOOR_OPEN 1
#define DOOR_CLOSED 0
#define DOOR_ERROR 2

static byte session;
byte Ethernet::buffer[700];
char sendBuffer[128];

float oldTemp = 0;
int numberOfDevices;
DeviceAddress tempDeviceAddress;
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);

int oldDoorstatus = DOOR_ERROR;

static void updateTemperature(float temp) {
  Serial.println("[>] Sending new temperature ...");

  if (DEBUG) return;

  snprintf(sendBuffer, sizeof sendBuffer, "key=%s&temp=%d \0", TOKEN, (int)temp);
  ether.httpPost(endpoint, api, NULL, sendBuffer, NULL);

//  Serial.println("[>] Done sending temperature!");
}

static void updateDoorstatus(int isOpen) {
  Serial.println("[>] Sending new doorstatus ...");
  
  if (DEBUG) return;

  snprintf(sendBuffer, sizeof sendBuffer, "key=%s&door=%d \0", TOKEN, isOpen);
  ether.httpPost(endpoint, api, NULL, sendBuffer, NULL);
  
//  Serial.println("[>] Done sending doorstatus!");
}

void findTempSensor() {
  while (numberOfDevices < 1) {
    // Grab a count of devices on the wire
    numberOfDevices = sensors.getDeviceCount();

    // locate devices on the bus
    //    Serial.print("[T] Locating one-wire devices on port ");
    //    Serial.println(PIN_ONEWIRE);
    //
    Serial.print("[T] Found ");
    Serial.print(numberOfDevices, DEC);
    Serial.println(" one-wire devices.");
    //
    //    // report parasite power requirements
    //    Serial.print("[T] Parasite power is: ");
    //    if (sensors.isParasitePowerMode()) Serial.println("ON");
    //    else Serial.println("OFF");

    if (numberOfDevices < 1) delay(500);
  }
  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      //      Serial.print("[T] Found device ");
      //      Serial.println(i, DEC);
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    } else
    {
      //      Serial.print("[T] Found ghost device at ");
      //      Serial.print(i, DEC);
      //      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

void setup () {
  // Setup Serial:
  Serial.begin(BAUDRATE);
  Serial.println("\n[Stefan's SpaceAPI Publisher]");

  // Setup temp sensor:
  sensors.begin();
  findTempSensor();

  // Setup network:
  if (!DEBUG)
  {
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
      Serial.println(F("[N] Failed to access Ethernet controller"));
    if (!ether.dhcpSetup())
      Serial.println(F("[N] DHCP failed"));

    //    ether.printIp("[N] IP:  ", ether.myip);
    ether.printIp("[N] GW:  ", ether.gwip);
    ether.printIp("[N] DNS: ", ether.dnsip);

    if (!ether.dnsLookup(api))
      Serial.println(F("[N] DNS failed"));

    ether.printIp("[N] SRV: ", ether.hisip);
  }
}

float getTemp() {
  sensors.requestTemperatures();
  sensors.getAddress(tempDeviceAddress, 0);
  float temp = sensors.getTempC(tempDeviceAddress);
  return temp;
}

static uint32_t timer;
void loop () {
  ether.packetLoop(ether.packetReceive());

  if (millis() > timer) {
    timer = millis() + 5000;

    // TODO: check doorsensor
    int newDoorstatus = DOOR_ERROR;
    float newTemp = getTemp();

    if (newDoorstatus != oldDoorstatus) {
      Serial.println(newDoorstatus == DOOR_OPEN ? "[!] DOOR OPENED!" : "[!] DOOR CLOSED!");
      updateDoorstatus(newDoorstatus);
      oldDoorstatus = newDoorstatus;

    } else if (newTemp != oldTemp) { // comparing floats? well ...
      Serial.print("[!] Temp updated: ");
      Serial.println(newTemp);
      updateTemperature(newTemp);
      //oldTemp = newTemp;
    } else Serial.println("[*] Nothing to do.");

  }
}

