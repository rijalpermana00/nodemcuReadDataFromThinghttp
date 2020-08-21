#include <ESP8266WiFi.h>        //Use ESP8266 functions                                              
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <time.h>

const char* ssid = "SQR21";              //Your router SSID and password
const char* password =  "Satu2tiga";
const char* host = "api.thingspeak.com";  //We read the data from this host
const int httpPortRead = 80;

const char* url1 = "/apps/thinghttp/send_request?api_key=QJF4HCWSCDS200D4";     

int To_remove;      
//of those characters
int timezone = 7 * 3600;
int dst = 0;

String Data_Raw, Data_Raw_1, Data_Raw_2, Data_Raw_3,d,m,j,mi;

WiFiClient client;      
HTTPClient http;
LiquidCrystal_I2C lcd(0x27, 16, 2);
//sda d2;
//scl d1
void setup() {

  Wire.begin(2, 0);
  Serial.begin(9600);            
  WiFi.begin(ssid, password);

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for Internet time");
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("\nTime response....OK");
  lcd.init();
  lcd.backlight();
  lcd.clear();

}

void loop()
{
  //Reading 1: Reading of cases
  if ( http.begin(host, httpPortRead, url1)) {
    int httpCode = http.GET();                //Check feedback if there's a response
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

        Data_Raw = http.getString();   //Here we store the raw data string

        Data_Raw_1 = Data_Raw;

        To_remove = Data_Raw_1.indexOf(">");                   
        Data_Raw_1.remove(0, To_remove + 1);                   
        To_remove = Data_Raw_1.indexOf("<");                   
        Data_Raw_1.remove(To_remove, Data_Raw_1.length());     
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(Data_Raw_1);
        Serial.println(Data_Raw_1 );

        time_t now = time(nullptr);
        struct tm* p_tm = localtime(&now);
        Serial.print(p_tm->tm_mday);
        Serial.print("/");
        Serial.print(p_tm->tm_mon + 1);
        Serial.print("/");
        Serial.print(p_tm->tm_year + 1900);

        Serial.print(" ");

        Serial.print(p_tm->tm_hour);
        Serial.print(":");
        Serial.print(p_tm->tm_min);
        Serial.print(":");
        Serial.println(p_tm->tm_sec);

        d = p_tm->tm_mday;
        m = p_tm->tm_mon + 1;
        j = p_tm->tm_hour;
        mi = p_tm->tm_min;
        
        if((d).length() == 1){
          Data_Raw_2 += "0"; 
        }
        Data_Raw_2 = d;
        Data_Raw_2 += "/";
        if((m).length() == 1){
          Data_Raw_2 += "0"; 
        }
        Data_Raw_2 += m; 
        Data_Raw_2 += "/";
        Data_Raw_2 += p_tm->tm_year + 1900;
        Data_Raw_2 += " ";
        if((j).length() == 1){
          Data_Raw_2 += "0"; 
        }
        Data_Raw_2 += j;
        Data_Raw_2 += ":";
        if((mi).length() == 1){
          Data_Raw_2 += "0"; 
        }
        Data_Raw_2 += mi;

        lcd.setCursor(0, 1);
        lcd.print(Data_Raw_2);
        delay(1000);

      }
    } else {

      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      lcd.clear();

    }

    http.end();

  } else {

    Serial.printf("[HTTP} Unable to connect\n");
    lcd.clear();
  }

  while (WiFi.status() != WL_CONNECTED)     //In case the Wifi connexion is lost
  {

    WiFi.disconnect();
    delay(1000);

    WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi..");
    delay(10000);
    lcd.clear();
  }

}
