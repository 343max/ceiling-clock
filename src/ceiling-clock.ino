#include "config.h"

#include <ESP8266WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <ESPNtpClient.h>
#include <SPI.h>
#include <TimeLib.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 4
#define CLK_PIN   D2
#define DATA_PIN  D4
#define CS_PIN    D3

uint8_t space[] = { 1, 0x00 }; // Space same width as colon ':'

#if NARROW_DIGITS
uint8_t digit0[] = { 4,0x3e,0x41,0x41,0x3e };
uint8_t digit1[] = { 4,0x04,0x42,0x7f,0x40 };
uint8_t digit2[] = { 4,0x62,0x51,0x49,0x46 };
uint8_t digit3[] = { 4,0x22,0x41,0x49,0x36 };
uint8_t digit4[] = { 4,0x0c,0x0b,0x08,0x7e };
uint8_t digit5[] = { 4,0x2F,0x49,0x49,0x31 };
uint8_t digit6[] = { 4,0x3e,0x49,0x49,0x32 };
uint8_t digit7[] = { 4,0x01,0x79,0x05,0x03 };
uint8_t digit8[] = { 4,0x36,0x49,0x49,0x36 };
uint8_t digit9[] = { 4,0x26,0x49,0x49,0x3e };
#endif

#if HOLLOW_ZERO
uint8_t hollow_digit0[] = { 5,0x3e,0x41,0x41,0x41,0x3e };
#endif


// Hardware SPI connection
// MD_Parola Display = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
MD_Parola Display = MD_Parola(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void wifi_connect(void)
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void display_ip(void)
{
  char charBuf[100];
  IPAddress localIP = WiFi.localIP();
  sprintf(charBuf, "%u.%u.%u.%u", localIP[0], localIP[1], localIP[2], localIP[3]);
  Display.displayText(charBuf, PA_LEFT, 25, 0, PA_SCROLL_LEFT, PA_NO_EFFECT);
  while(!Display.displayAnimate());
  delay(500);
  Display.displayReset();
}

void setup(void)
{
  Serial.begin(9600);
  delay(10);
  // Serial << endl << endl;

  Display.begin();
#if FLIP_DISPLAY
  Display.setZoneEffect(0, true, PA_FLIP_UD);
#endif

  Display.addChar(' ', space);
#if NARROW_DIGITS
  Display.addChar('0', digit0);
  Display.addChar('1', digit1);
  Display.addChar('2', digit2);
  Display.addChar('3', digit3);
  Display.addChar('4', digit4);
  Display.addChar('5', digit5);
  Display.addChar('6', digit6);
  Display.addChar('7', digit7);
  Display.addChar('8', digit8);
  Display.addChar('9', digit9);
#endif

#if HOLLOW_ZERO
  Display.addChar('0', hollow_digit0);
#endif
  
  Display.displayText("Hallo!", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  while(!Display.displayAnimate()) {}

  wifi_connect();
  display_ip();

  NTP.onNTPSyncEvent([](NTPEvent_t event) {
    Serial.printf ("[NTP-event] %s\n", NTP.ntpEvent2str(event));
    setTime(NTP.getLastNTPSync());
  });
  NTP.setTimeZone(TZ_Europe_Berlin);
  NTP.setInterval(3600 * 12);
  NTP.begin("pool.ntp.org", true);

  pinMode(LED_BUILTIN, OUTPUT);
}

static char charBuf[100];

char *getTimeStr() {
  time_t currentTime = time(NULL);
  tm *local_tm = localtime(&currentTime);
  if (local_tm->tm_sec % 2 == 0) {
    strftime(charBuf, sizeof(charBuf), "%H:%M", local_tm);
  } else {
    strftime(charBuf, sizeof(charBuf), "%H %M", local_tm);
  }
  return charBuf;
}

void update_time(void)
{
  static time_t last_t;
  time_t t = now();

  if (t != last_t) {
    last_t = t;
    const char* timeStr = getTimeStr();
    Serial.println(timeStr);
    Display.displayText(timeStr, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT );
    while (!Display.displayAnimate());
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void loop(void)
{
  update_time();
}