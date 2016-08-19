# ESP8266-GeoLocation
This demonstrates methods to GeoLocate using the ESP8266 device.

The first method, ESP8266_GeoLocation_1, obtains a location estimate from the IP address using the freegeoip service (freegeoip.net), 
using code copied almost verbatim from the Adafruit CC3000 soution.

The second method, ESP_GeoLocation_2, obtains a location estimate from a scan of nearby Wireless Access Points, using GeoLocation APIs,
such as those offered by Google or Mozilla. 

It includes an explicit example of how to build the json string which is required to submit the WAP scan result to these services.

For more information and description of applications, see m0xpd.blogspot.com
