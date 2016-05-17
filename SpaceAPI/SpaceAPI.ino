#include <EtherCard.h>
#include "secrets.h"

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x42, 0x42 };

// CONFIG:
#define BAUDRATE 57600
const char api[]      PROGMEM = "spaceapi.net";
const char endpoint[] PROGMEM = "new/space/losfuzzyslab/sensor/set";
// define TOKEN in secrets.h

static byte session;
byte Ethernet::buffer[700];
Stash stash;
bool oldDoorstatus = false;

static void updateDoorstatus(bool isOpen) {
  Serial.println("[>] Sending new doorstatus ...");
  
  byte sd = stash.create();

  const char s_open[]  = "%7B%22state%22%3A%7B%22open%22%3Atrue%7D%7D";
  const char s_close[] = "%7B%22state%22%3A%7B%22open%22%3Afalse%7D%7D";
  stash.print("key=");
  stash.print(TOKEN);
  stash.print("&sensors=");
  stash.println(isOpen ? s_open : s_close);
  stash.save();
  int stash_size = stash.size();

  Stash::prepare(PSTR("POST /$F HTTP/1.0" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-type: application/x-www-form-urlencoded" "\r\n"
                      "Content-length: $D" "\r\n"
                      "\r\n"
                      "$H"),
                 endpoint, api, stash_size, sd);

  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();

  Serial.println("[>] Done sending doorstatus!");
}

void setup () {
  Serial.begin(BAUDRATE);
  Serial.println("\n[Stefan's SpaceAPI Publisher]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("[*] DHCP failed"));

  ether.printIp("[*] IP:  ", ether.myip);
  ether.printIp("[*] GW:  ", ether.gwip);
  ether.printIp("[*] DNS: ", ether.dnsip);

  if (!ether.dnsLookup(api))
    Serial.println(F("[*] DNS failed"));

  ether.printIp("[*] SRV: ", ether.hisip);

  updateDoorstatus(oldDoorstatus);
}

void loop () {

  // TODO: check doorsensor
  bool newDoorstatus = false;

  if(newDoorstatus != oldDoorstatus) {
    Serial.println(newDoorstatus ? "[!] DOOR OPENED!" : "[!] DOOR CLOSED!");
    updateDoorstatus(newDoorstatus);
    oldDoorstatus = newDoorstatus;
  }

  // HTTP response handling (mostly for debug):
  ether.packetLoop(ether.packetReceive());
  const char* reply = ether.tcpReply(session);
  if (reply != 0) {
    Serial.println("[<] Got a response:");
    Serial.println(reply);
  }
}

