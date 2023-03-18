#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <FS.h>
#include <FTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

HTTPClient http;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const long utcOffsetInSeconds = 3600;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const char* ssid     ="IFM";
const char* password = "engine923";
String deviceID = String(WiFi.macAddress());
String formU = "http://eucalyptus.iq-joy.com/joseph1/smcppowersave.php?deviceID="+deviceID;
String networPublicIPx = "http://checkip.amazonaws.com";
//const char* url

//FTP server details
char* xhost = "ftp.iq-joy.com";
char* xusername = "joseph1@eucalyptus.iq-joy.com";
char* xpassword = "Angel1985#20X";
char* xfolder = "/"; 

char* __dataFileName="";

  
PZEM004Tv30 pzem(D1, D2); // (RX,TX)connect to TX,RX of PZEM
PZEM004Tv30 pzem2(D5, D6);  // (RX,TX) connect to TX,RX of PZEM
PZEM004Tv30 pzem3(D7, D3);  // (RX,TX) connect to TX,RX of PZEM

short FTPresult; //outcome of FTP upload
//Function prototype - required if folder is an optional argument
short doFTP(char* , char* , char* , char* , char* = "");
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change:
const long interval = 30000; 

boolean gotPublicIP = false;

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 251);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  
  //WiFi.mode(WIFI_STA);
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

}

void loop() {
  // wait for WiFi connection
unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you passed
    previousMillis = currentMillis;
    if(WiFi.status()== WL_CONNECTED){
                    Serial.println("Connecting to IQ-JOY Server");
                    String data = getPowerInfo();
                    String url = formU+"&ip=unknown&readings="+data;
                   
                    Serial.println(url);
                    http.begin(url); //Specify the URL
                    int httpCode = http.GET();                                        //Make the request
                 
                    if (httpCode > 0) { //Check for the returning code
                        String response = http.getString();
                        Serial.println(httpCode);
                        Serial.println(response);
                      }else {
                      Serial.println("Error on HTTP request");
                      }
                      http.end(); //Free the resources
                    
                  }else{
                     Serial.println("Wifi disconnected");
                     //try reconnecting
                     WiFi.reconnect();
                     String data = getPowerInfo();
                  }
                }
}

String getPowerInfo(){
      float voltagex = pzem.voltage();
      float currentx = pzem.current();
      float powerx = pzem.power();
      float energyx = pzem.energy();
      float frequencyx = pzem.frequency();
      float pfx = pzem.pf(); 
    
      //energymeter 2
      float voltage2 = pzem2.voltage();
      float current2 = pzem2.current();
      float power2 = pzem2.power();
      float energy2 = pzem2.energy();
      float frequency2 = pzem2.frequency();
      float pf2 = pzem2.pf();
     
      // energy meter 3
      float voltage3 = pzem3.voltage();
      float current3 = pzem3.current();
      float power3 = pzem3.power();
      float energy3 = pzem3.energy();
      float frequency3 = pzem3.frequency();
      float pf3 = pzem3.pf();
    
    
      //write to file
    String Stringdata=String(voltagex)+"|"+String(currentx)+"|"+String(powerx)+"|"+String(energyx)
    +"|"+String(frequencyx)+"|"+String(pfx)+"|"+String(voltage2)+"|"+String(current2)
    +"|"+String(power2)+"|"+String(energy2)
    +"|"+String(frequency2)+"|"+String(pf2)+"|"+String(voltage3)+"|"
    +String(current3)+"|"+String(power3)+"|"+String(energy3)
    +"|"+String(frequency3)+"|"+String(pf3);
    Serial.println("Data= "+Stringdata);
    
    return Stringdata;
}
