 #include "ESP8266WiFi.h"
 #include "ESP8266HTTPClient.h"
 
const char* ssid = "Makerspace-iLab";
const char* password = "";

long counter;

long readIntFromSerial()
{
  long message;
  if(Serial.available() > 0) //Check serial buffer for available data.
  {
    message = Serial.parseInt(); //Look for integer in serial buffer, if timeout after 60seconds, Serial.parseInt will return 0.
    return message;
  }
  else
  {
    return -1;
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial)
  {
    ; //Wait for serial port to connect. Needed for native USB
  }
  Serial.setTimeout(60000L); //Set timeout to 60 seconds.

 
// Connect to WiFi network

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(300);
    //Wait for Wi-Fi to connect.
  }

 
}

void loop()
{
  counter = readIntFromSerial();
  if(counter >= 0)
  {
    if(WiFi.status() == WL_CONNECTED) //Check WiFi connection status
    {
      String jsonCounter =  "{\"traffic\": \"" + String(counter) + "\"}"; //Package into json format.
      HTTPClient http;    //Declare object of class HTTPClient
      http.begin("http://192.168.0.188:5000/");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
      http.POST(jsonCounter);   //Send the request
      http.end();  //Close connection
    }
  }
}
 
