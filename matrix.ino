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
#define CLK_PIN   D2
#define DATA_PIN  D4
#define CS_PIN    D3

#define TIMEZONE -8

#define HOLLOW_ZERO 0
#define NARROW_DIGITS 1

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

uint8_t space[] = { 1, 0x00 }; // Space same width as colon ':'

#if NARROW_DIGITS
uint8_t digit0[] = { 4,0x3e,0x41,0x41,0x3e };
uint8_t digit1[] = { 4,0x04,0x42,0x7f,0x40 };
uint8_t digit2[] = { 4,0x62,0x51,0x49,0x46 };
uint8_t digit3[] = { 4,0x22,0x41,0x49,0x36 };
uint8_t digit4[] = { 4,0x0c,0x0b,0x08,0x7e };
uint8_t digit5[] = { 4,0x27,0x45,0x45,0x39 };
uint8_t digit6[] = { 4,0x3e,0x49,0x49,0x32 };
uint8_t digit7[] = { 4,0x01,0x79,0x05,0x03 };
uint8_t digit8[] = { 4,0x36,0x49,0x49,0x36 };
uint8_t digit9[] = { 4,0x26,0x49,0x49,0x3e };
#endif

#if HOLLOW_ZERO
uint8_t hollow_digit0[] = { 5,0x3e,0x41,0x41,0x41,0x3e };
#endif


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

  char charBuf[100];
  IPAddress localIP = WiFi.localIP();
  sprintf(charBuf, "%u.%u.%u.%u", localIP[0], localIP[1], localIP[2], localIP[3]);
  P.displayText(charBuf, PA_CENTER, 0, 0, PA_SCROLL_LEFT, PA_NO_EFFECT);
  while(!P.displayAnimate());
  delay(3000);
}

void setup(void)
{
  Serial.begin(115200);
  delay(10);
  // Serial << endl << endl;

  P.begin();
  P.addChar(' ', space);
#if NARROW_DIGITS
  P.addChar('0', digit0);
  P.addChar('1', digit1);
  P.addChar('2', digit2);
  P.addChar('3', digit3);
  P.addChar('4', digit4);
  P.addChar('5', digit5);
  P.addChar('6', digit6);
  P.addChar('7', digit7);
  P.addChar('8', digit8);
  P.addChar('9', digit9);
#endif

#if HOLLOW_ZERO
  P.addChar('0', hollow_digit0);
#endif
  
  P.displayText("Kolja", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  while(!P.displayAnimate()) {}

  wifi_connect();

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
  NTP.begin("pool.ntp.org", TIMEZONE, true);
  NTP.setInterval(1800);
}

void loop(void)
{
  char charBuf[100];
  char delimiter = second() % 2 == 0 ? ':' : ' ';
  sprintf(charBuf, "%u%c%02u", hour(), delimiter, minute());
  P.displayText(charBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT );
  while (!P.displayAnimate());
}