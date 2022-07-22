#include <LiquidCrystal_I2C.h>

//#include <LiquidCrystal_I2C.h>


#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

//LiquidCrystal_I2C lcd(0X3F, 16, 2);
LiquidCrystal_I2C lcd(0X27, 16, 2);



const char* ssid[] = {"rfid", "www.hadiryaa.com", "FRITTO LAND", "frittoland", "frittoland_wifi", "fritto", "frittoland_sumarosno", "fritto_sumarsono", "fritto_wifi", "fc_wifi_1", "fc_sumarsono_1", "sumarsono_fc", "fritto_wifi_1"} ;
const char* pass[] = {"12345566", "222222223", "chickenforeveryone", "frittoland2022", "fcland12345678", "fritto12345", "sumarsono12345", "chickenwhole", "superfritto2", "wholemadu", "pakethemat", "tetapirit", "favoritsemua"};

const int   ssid_count = sizeof(ssid) / sizeof(ssid[0]); // number of known networks


//const char* ssid = "rfid";
//const char* pass = "12345566";
//const char* ssid0 = "www.hadiryaa.com";
//const char* pass0 = "222222223";
//
////
//const char* ssid1 = "frittochicken_1";
//const char* pass1 = "superfritto1";
//
//
//const char* ssid2 = "fc_ayahanda";
//const char* pass2 = "fcayahanda";
//const char* ssid3 = "fritto_chicken";
//const char* pass3 = "fritto12345";
//const char* ssid4 = "fritto_ayahanda";
//const char* pass4 = "ayahanda12345";
//const char* ssid5 = "fritto_1";
//const char* pass5 = "chickenwhole";
//const char* ssid6 = "fritto_wifi";
//const char* pass6 = "superfritto2";
//const char* ssid7 = "fc_wifi_1";
//const char* pass7 = "wholemadu";
//const char* ssid8 = "fc_ayahanda_1";
//const char* pass8 = "pakethemat";
//const char* ssid9 = "ayahanda_fc";
//const char* pass9 = "tetapirit";
//const char* ssid10 = "fritto_wifi_1";
//const char* pass10 = "favoritsemua";






const char* host = "frittocard.frittochicken.com";
//const char* host = "192.168.217.247";



WiFiClient client;

constexpr uint8_t RST_PIN = 0;
constexpr uint8_t SS_PIN = 2;

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;
String id;
String data;
int nbVisibleNetworks;


//const int speaker = 15;
const int speaker = 9;
int i, n;





void setup() {

  boolean wifiFound = false;


  lcd.init();

  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("DEVICE OK");
  delay(1000);
  lcd.clear();


  // put your setup code here, to run once:
  Serial.begin(9600);

  SPI.begin();
  pinMode(speaker, OUTPUT);

  rfid.PCD_Init();


  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  nbVisibleNetworks = WiFi.scanNetworks();
  Serial.println(F("scan done"));
  if (nbVisibleNetworks == 0) {
    Serial.println(F("no ntwrk found"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  for (i = 0; i < nbVisibleNetworks; ++i) {
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < ssid_count; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(ssid[n], WiFi.SSID(i).c_str())) {
        Serial.print(F("\tNot matching "));
        Serial.println(ssid[n]);
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  } // end for each visible network

  WiFi.begin(ssid[n], pass[n]);




  while ((!(WiFi.status() == WL_CONNECTED))) {
    Serial.print(".");
    lcd.print("Menghubungkan ...");
    delay(300);
    lcd.clear();
  }



  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Wifi Terhubung");
    digitalWrite(speaker, HIGH);
    delay(500);
    digitalWrite(speaker, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WIFI CONNECT");
    lcd.setCursor(0, 1);
    lcd.print("Ready to TAP");
    delay(3000);
    lcd.clear();

  }




}


void loop() {



  while ((!(WiFi.status() == WL_CONNECTED))) {
    Serial.print(".");
    lcd.print("Menghubungkan ...");
    delay(300);
    lcd.clear();




  }


  if (WiFi.status() == WL_CONNECTED)
  {
    //    digitalWrite(speaker, HIGH);
    //    delay(500);
    //    digitalWrite(speaker, LOW);
    lcd.setCursor(0, 0);
    lcd.print("Ready to TAP");
  }

  if (rfid.PICC_IsNewCardPresent()) {
    koneksi_database();

    readRFID();

    Serial.print("ID CARD: ");
    Serial.println(id);

    Serial.print("Terhubung Ke : ");
    Serial.println(host);

    //    String LinkId;
    //    HTTPClient httprelay;

    String Link;
    HTTPClient http;
    Link = "http://" + String(host) + "/read.php?outlet=8&card=" + String(id);
    http.begin(client, Link);
    http.GET();
    String responWeb = http.getString();



    //    String LinkId;
    //    HTTPClient httprelay;
    //    LinkId = "http://" + String(host) + "/read.php?outlet=8&card=" + String(id);
    //    httprelay.begin(client, LinkId);
    //    //ambil isi data
    //    httprelay.GET();
    //    String responWeb = httprelay.getString();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sisa Saldo:");
    lcd.setCursor(0, 1);
    //    lcd.clear();
    Serial.println(responWeb);
    //
    lcd.print(responWeb);
    delay(3000);
    http.end();
    //    httprelay.end();
    lcd.clear();

  }
}



void readRFID() {

  rfid.PICC_ReadCardSerial();
  id = String(rfid.uid.uidByte[0]) + String(rfid.uid.uidByte[1]) + String(rfid.uid.uidByte[2]) + String(rfid.uid.uidByte[3]);

  if (id)
  {
    digitalWrite(speaker, HIGH);
    delay(500);
    digitalWrite(speaker, LOW);
  }
  delay(500);
}

void koneksi_database()
{
  if (!client.connect(host, 80)) {
    Serial.println("Gagal Konek");
    return;
  }
  else {
    Serial.println("Berhasil Konek");
    return;
  }
}
