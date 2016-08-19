/*
ESP8266_GeoLocate_1

GeoLocation using IP Address

This is a copy of the Adafruit CC3000 Solution at 
https://github.com/adafruit/Adafruit_CC3000_Library/blob/master/examples/GeoLocation/GeoLocation.ino
by Limor Fried, Kevin Townsend and Phil Burgess

presented as a contrast to the alternative 
ESP8266_GeoLocate_2 using WAP 'sniffing'

m0xpd

August2016

For further information:
http://m0xpd.blogspot.co.uk/
=======================================================
*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
// variables for location service...
char
  country[20],
  region[20],
  city[20],
  name[13],  // Temp space for name:value parsing
  value[64]; // Temp space for name:value parsing
float
  longitude, latitude;
char server[] = "freegeoip.net";    //  (using DNS)

char ssid[] = <ssid>;  //  your network SSID (name)
char pass[] = <pass>;            // your network password


// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;


void setup() {
  // put your setup code here, to run once:  
  uint32_t ip = 0L;
  DynamicJsonBuffer jsonBuffer;
  Serial.begin(115200);
    Serial.println("Start");
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  ip=WiFi.localIP();
  Serial.println(ip);    
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
  client.println("GET /json/ HTTP/1.1\r\nHost: freegeoip.net\r\n\r\n");
  Serial.print("\r\nReading response...");
  country[0] = region[0] = city[0] = 0; // Clear data
  jsonParse(0, 0);
  client.println("Connection: close");
  Serial.println(F("OK"));
  Serial.println(F("\nDisconnecting"));
  client.stop();
  }
  Serial.print(F("\r\nRESULTS:\r\n  Country: "));
  Serial.println(country);
  Serial.print(F("  Region: "));
  Serial.println(region);
  Serial.print(F("  City: "));
  Serial.println(city);
  Serial.print(F("  Longitude: "));
  Serial.println(longitude);
  Serial.print(F("  Latitude: "));
  Serial.println(latitude);
   
}

void loop() {
  // put your main code here, to run repeatedly:

}

boolean jsonParse(int depth, byte endChar) {
  int     c, i;
  boolean readName = true;
  for(;;) {
    while(isspace(c = timedRead())); // Scan past whitespace
    if(c < 0)        return false;   // Timeout
    if(c == endChar) return true;    // EOD

    if(c == '{') { // Object follows
      if(!jsonParse(depth + 1, '}')) return false;
      if(!depth)                     return true; // End of file
    } else if(c == '[') { // Array follows
      if(!jsonParse(depth + 1,']')) return false;
    } else if((c == '"') || (c == '\'')) { // String follows
      if(readName) { // Name-reading mode
        if(!readString(name, sizeof(name)-1, c)) return false;
      } else { // Value-reading mode
        if(!readString(value, sizeof(value)-1, c)) return false;
        // Process name and value strings:
        if       (!strcasecmp(name, "country_name")) {
          strncpy(country, value, sizeof(country)-1);
        } else if(!strcasecmp(name, "region_name")) {
          strncpy(region, value, sizeof(region)-1);
        } else if(!strcasecmp(name, "city")) {
          strncpy(city, value, sizeof(city)-1);
        }
      }
    } else if(c == ':') { // Separator between name:value
      readName = false; // Now in value-reading mode
      value[0] = 0;     // Clear existing value data
    } else if(c == ',') { // Separator between name/value pairs
      readName = true; // Now in name-reading mode
      name[0]  = 0;    // Clear existing name data
    } else {
      // Else true/false/null or a number follows.
      value[0] = c;
      if(!strcasecmp(name, "longitude")) {
        if(!readString(value+1, sizeof(value)-1, ',')) return false;
        longitude = atof(value);
      } else if(!strcasecmp(name, "latitude")) {
        if(!readString(value+1, sizeof(value)-1, ',')) return false;
        latitude = atof(value);
      }
      readName = true; // Now in name-reading mode
      name[0]  = 0;    // Clear existing name data
    }
  }
}

// Read from client stream with a 5 second timeout.  Although an
// essentially identical method already exists in the Stream() class,
// it's declared private there...so this is a local copy.
int timedRead(void) {
  unsigned long start = millis();
  while((!client.available()) && ((millis() - start) < 5000L));
  return client.read();  // -1 on timeout
}

// Read string from client stream into destination buffer, up to a maximum
// requested length.  Buffer should be at least 1 byte larger than this to
// accommodate NUL terminator.  Opening quote is assumed already read,
// closing quote will be discarded, and stream will be positioned
// immediately following the closing quote (regardless whether max length
// is reached -- excess chars are discarded).  Returns true on success
// (including zero-length string), false on timeout/read error.
boolean readString(char *dest, int maxLen, char quote) {
  int c, len = 0;

  while((c = timedRead()) != quote) { // Read until closing quote
    if(c == '\\') {    // Escaped char follows
      c = timedRead(); // Read it
      // Certain escaped values are for cursor control --
      // there might be more suitable printer codes for each.
      if     (c == 'b') c = '\b'; // Backspace
      else if(c == 'f') c = '\f'; // Form feed
      else if(c == 'n') c = '\n'; // Newline
      else if(c == 'r') c = '\r'; // Carriage return
      else if(c == 't') c = '\t'; // Tab
      else if(c == 'u') c = unidecode(4);
      else if(c == 'U') c = unidecode(8);
      // else c is unaltered -- an escaped char such as \ or "
    } // else c is a normal unescaped char

    if(c < 0) return false; // Timeout

    // In order to properly position the client stream at the end of
    // the string, characters are read to the end quote, even if the max
    // string length is reached...the extra chars are simply discarded.
    if(len < maxLen) dest[len++] = c;
  }

  dest[len] = 0;
  return true; // Success (even if empty string)
}

// Read a given number of hexadecimal characters from client stream,
// representing a Unicode symbol.  Return -1 on error, else return nearest
// equivalent glyph in printer's charset.  (See notes below -- for now,
// always returns '-' or -1.)
int unidecode(byte len) {
  int c, v, result = 0;
  while(len--) {
    if((c = timedRead()) < 0) return -1; // Stream timeout
    if     ((c >= '0') && (c <= '9')) v =      c - '0';
    else if((c >= 'A') && (c <= 'F')) v = 10 + c - 'A';
    else if((c >= 'a') && (c <= 'f')) v = 10 + c - 'a';
    else return '-'; // garbage
    result = (result << 4) | v;
  }

  return '?';
}
