// Program to demonstrate the MD_Parola library
//
// Simplest program that does something useful - Hello World!
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#include <ESP8266WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <NtpClientLib.h>
#include <SPI.h>
#include <TimeLib.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 4
#define CLK_PIN   D3
#define DATA_PIN  D4
#define CS_PIN    D2

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

// Hardware SPI connection
// MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void wifi_connect(void)
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void setup(void)
{
  Serial.begin(115200);
  delay(10);
  // Serial << endl << endl;

  P.begin();
  P.displayText("...", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  while(!P.displayAnimate()) {}

  wifi_connect();

  P.displayText("20:58", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
    if (error) {
      Serial.print("Time Sync error: ");
      if (error == noResponse)
        Serial.println("NTP server not reachable");
      else if (error == invalidAddress)
        Serial.println("Invalid NTP server address");
    }
    else {
      Serial.print("Got NTP time: ");
      Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
  });
  NTP.begin("pool.ntp.org", 1, true);
  NTP.setInterval(1800);
}

void loop(void)
{
  time_t t = now() - 3600 * 9;
  char charBuf[100];
  char delimiter = second(t) % 2 == 0 ? ':' : ' ';
  sprintf(charBuf, "%02u%c%02u", hour(t), delimiter, minute(t));
  P.displayText(charBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT );
  while (!P.displayAnimate());
}