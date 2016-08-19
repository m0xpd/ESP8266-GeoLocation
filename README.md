# ESP8266-GeoLocation
This demonstrates methods to GeoLocate using the ESP8266 device. 

They are presented as sketches for the Arduino IDE.

The first method, ESP8266_GeoLocation_1, obtains a location estimate from the IP address using the freegeoip service (freegeoip.net), 
using code copied almost verbatim from the Adafruit CC3000 soution.

The second method, ESP_GeoLocation_2, obtains a location estimate from the results of a scan of nearby Wireless Access Points, 
using GeoLocation APIs such as those offered by Google or Mozilla. 

It includes an explicit example of how to build the json string which is required to submit the WAP scan result to these services.

For more information and description of applications, see http://m0xpd.blogspot.com/2016/08/esp8266-geolocation.html
