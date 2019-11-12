#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <MHZ19.h>
#include <string.h>
#include <stdlib.h>

#define DHTPIN 2
#define DHTTYPE DHT11

SoftwareSerial ss(4, 5);
MHZ19 mhz(&ss);
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "";
const char* password = "";

// 
// 앞면
int Front_fan1 = 16;
int Front_fan2 = 0;
// 뒷면
int Back_fan1 = 13;
int Back_fan2 = 15;

int ledPin = 14; // GPIO14
WiFiServer server(80);

int h = 0; int t = 0;
int dust_sensor = A0;
float dust_value = 0;
float dustDensityug = 0;
int sensor_led = 12;
int sampling = 280;
int waiting = 40;
float stop_time = 9680;
int is_error = 0;
int is_auto = 0;
int is_led = 0;
int is_fan = 0;
int uv_read = 0;
int SA = 0;

void LED_Processing()
{
    WiFiClient client = server.available();
    if(!client) { return ; }
    
    int fan_value = LOW;
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("/AUTO") != -1)
    {
        is_auto = 1;
    }
    if (request.indexOf("/MANUAL") != -1)
    {
        is_auto = 0;
    }

    if (is_auto == 0)
    {
        int device = 0; // 10: LED, 20: FAN
        int led_value = LOW;
        if (request.indexOf("/LED=ON") != -1)
        {
            digitalWrite(ledPin, HIGH);
            led_value = HIGH;
            device = 10;
            is_led = 1;
        }
        if (request.indexOf("/LED=OFF") != -1)
        {
            digitalWrite(ledPin, LOW);
            led_value = LOW;
            device = 10;
            is_led = 0;
        }

        if (request.indexOf("/FAN=ON") != -1){
            digitalWrite(Front_fan1, HIGH);
            digitalWrite(Front_fan2, LOW);
            digitalWrite(Back_fan1, HIGH);
            digitalWrite(Back_fan2, LOW);
            fan_value = HIGH;
            device = 20;
            is_fan = 1;
        }
        if (request.indexOf("/FAN=OFF") != -1){
            digitalWrite(Front_fan1, LOW);
            digitalWrite(Front_fan2, LOW);
            digitalWrite(Back_fan1, LOW);
            digitalWrite(Back_fan2, LOW);
            fan_value = LOW;
            device = 20;
            is_fan = 0;
        }

        if(device == 10){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type : text/html");
          client.println("");
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("LED is turned ");
          if (led_value) { client.println("On"); }
          else { client.print("Off"); }
          client.println("<br><br>");
          client.println("<a href = \"/LED=ON\"\"><button>Turn On</button></a>");
          client.println("<a href = \"/LED=OFF\"\"><button>Turn Off</button></a>");
          client.println("</html>");
        }
        else if(device = 20){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type : text/html");
          client.println("");
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("FAN is turned ");
          if (fan_value) { client.println("On"); }
          else { client.print("Off"); }
          client.println("<br><br>");
          client.println("<a href = \"/FAN=ON\"\"><button>Turn On</button></a>");
          client.println("<a href = \"/FAN=OFF\"\"><button>Turn Off</button></a>");
          client.println("</html>");          
        }
    }
    delay(1);
}

void setup()
{
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT); // 14
    pinMode(sensor_led, OUTPUT); // 12
    pinMode(4, OUTPUT);
    
    ss.begin(9600);
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("connection Start");

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    pinMode(Front_fan1, OUTPUT);
    pinMode(Front_fan2, OUTPUT);
    pinMode(Back_fan1, OUTPUT);
    pinMode(Back_fan2, OUTPUT);

    Serial.setTimeout(30);
    server.begin();
}

void loop()
{
    is_error = 0;
    WiFiClient client;
    if (!client.connect("", ))
    { // Fail to access
        Serial.println("connection failed");
        delay(1000);
    }
    else // Success to access to rasp server
    {
        char result[1024] = "";
        // H / T
        h = dht.readHumidity();
        t = dht.readTemperature();
        char Temper[100];
        char Humidity[100];
        sprintf(Temper, "%d", t);
        sprintf(Humidity, "%d", h);
        if (strlen(Temper) >= 3) { is_error = 1; }
        if (strlen(Humidity) >= 3) { is_error = 1; }
        if (is_error == 0)
        {
            strcat(Temper, " ");
            strcat(Temper, Humidity);
            strcat(result, Temper);

            // UV
            SA = Serial.available();
            //while(SA == 0){ Serial.println("Waiting UV"); delay(500); }
            String str = ""; char ch; int cnt = 0;
            while(SA > 0){
              if(cnt > 1){
                uv_read = str.toInt();
                break;
              }
              ch = Serial.read();
              if(ch > 47) { str.concat(ch); cnt++; }
            }
            //else { is_error = 1; }
            if (uv_read > 300) { is_error = 1; }
            else
            {
                if (is_auto == 1)
                {
                    if (uv_read < 10)
                    {
                        digitalWrite(ledPin, HIGH);
                        is_led = 1;
                    }
                    else if (uv_read >= 10)
                    {
                        digitalWrite(ledPin, LOW);
                        is_led = 0;
                    }
                }
                char UVstr1[100];
                sprintf(UVstr1, "%d", uv_read);
                strcat(result, " ");
                strcat(result, UVstr1);
            }

            // CO2
            MHZ19_RESULT response = mhz.retrieveData();
            if (response == MHZ19_RESULT_OK)
            {
                char CO2str1[10];
                int CO2_result = (int)(mhz.getCO2());
                sprintf(CO2str1, "%d", CO2_result);
                strcat(result, " ");
                strcat(result, CO2str1);
            }
            else { is_error = 1; }

            // Air Quality
            char Microstr1[100];
            digitalWrite(sensor_led, LOW);
            delayMicroseconds(sampling);
            dust_value = 0;
            for (int i = 0; i < 7; i++)
            {
                dust_value += analogRead(dust_sensor); // analogRead digitalRead
                delay(2);
            }
            dust_value = dust_value / 7;
            delayMicroseconds(waiting);
            digitalWrite(sensor_led, HIGH);
            delayMicroseconds(stop_time);
            dustDensityug = -(0.17 * (dust_value * (5.0 / 1024)) - 0.1) * 1000;

            if(is_auto == 1){
              if(dustDensityug > 70){
                digitalWrite(Front_fan1, HIGH);
                digitalWrite(Front_fan2, LOW);
                digitalWrite(Back_fan1, HIGH);
                digitalWrite(Back_fan2, LOW);
                is_fan = 1;
              }
              else if(dustDensityug <= 70 && dustDensityug > 0 ){
                digitalWrite(Front_fan1, LOW);
                digitalWrite(Front_fan2, LOW);
                digitalWrite(Back_fan1, LOW);
                digitalWrite(Back_fan2, LOW);
                is_fan = 0;
              }
            }
            
            //Serial.println(dustDensityug);
            if (dustDensityug <= 0) { is_error = 1; }
            else if (dustDensityug > 0)
            {
                sprintf(Microstr1, "%d", (int)dustDensityug);
                strcat(result, " ");
                strcat(result, Microstr1);

                // IP
                char addr[20];
                strcat(result, " ");
                sprintf(addr, "%s", WiFi.localIP().toString().c_str());
                strcat(result, addr); // enter address

                // is_auto
                char am[2];
                strcat(result, " ");
                sprintf(am, "%d", is_auto);
                strcat(result, am);

                // is_led
                char IL[2];
                strcat(result, " ");
                sprintf(IL, "%d", is_led);
                strcat(result, IL);

                // is_fan
                char IF[2];
                strcat(result, " ");
                sprintf(IF, "%d", is_fan);
                strcat(result, IF);

                if (is_error == 0)
                {
                    Serial.print("Send: ");
                    Serial.println(result);
                    client.write(result);

                    String recevbline = client.readStringUntil('\r');
                    Serial.print("Recv: ");
                    Serial.println(recevbline);
                    Serial.println();
                    delay(10);
                }
            }
        }
        LED_Processing();
    }
}