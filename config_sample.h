#include <ESPNtpClient.h>

// a zero without the strike through in the middle
#define HOLLOW_ZERO 0
// narrower, somewhat friendlier digits 
#define NARROW_DIGITS 1
// flip the display. neccessary for projection mode
#define FLIP_DISPLAY 1

// wifi credentials
const char* ssid = "<wifi ssid>";
const char* password = "<wifi password>";
const char* hostname = "<hostname for this device>";