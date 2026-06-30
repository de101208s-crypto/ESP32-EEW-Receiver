#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 16
#define BUZZER_PIN 17

String lastEventID = "";
int lastSerial = 0;

unsigned long cancelStart = 0;

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

const char* ssid = "your WI-FI SSID";
const char* password = "yourWI-FI Pass";

const char* url =
"https://api.wolfx.jp/jma_eew.json";

struct GSMap {
  const char* hypo;
  const char* code;
};

GSMap gsTable[] = {

  {"根室半島南東沖","NMU"},
  {"北海道東方沖","HEO"},
  {"釧路沖","KRO"},
  {"十勝沖","TOK"},
  {"浦河沖","URK"},
  {"国後島付近","KNR"},
  {"択捉島付近","ETR"},
  {"北海道南西沖","HSO"},

  {"青森県東方沖","AOE"},
  {"青森県西方沖","AOW"},
  {"津軽海峡","TSG"},
  {"岩手県沖","IWO"},
  {"三陸沖","SAN"},
  {"宮城県沖","MYO"},
  {"宮城県北部","MYN"},
  {"宮城県南部","MYS"},
  {"福島県沖","FKS"},
  {"福島県浜通り","FKH"},
  {"福島県中通り","FKN"},

  {"茨城県沖","IBR"},
  {"茨城県南部","IBS"},
  {"千葉県東方沖","CNB"},
  {"千葉県北東部","CNE"},
  {"千葉県北西部","CNW"},
  {"千葉県南部","CNS"},
  {"関東東方沖","KTO"},
  {"房総半島南方沖","BSO"},
  {"東京湾","TYO"},
  {"東京都23区","T23"},
  {"相模湾","SGW"},
  {"神奈川県東部","KNE"},
  {"神奈川県西部","KNW"},

  {"伊豆大島近海","IOK"},
  {"伊豆半島東方沖","IZE"},
  {"伊豆半島南方沖","IZS"},
  {"三宅島近海","MYJ"},
  {"八丈島近海","HCJ"},
  {"八丈島東方沖","HAO"},
  {"硫黄島東方沖","IEO"},
  {"小笠原諸島西方沖","OGW"},
  {"小笠原諸島東方沖","OGE"},

  {"新潟県中越地方","NCG"},
  {"新潟県上越地方","NJJ"},
  {"新潟県下越地方","NKG"},
  {"新潟県沖","NJO"},
  {"佐渡付近","SDF"},
  {"日本海北部","NKN"},
  {"日本海中部","NNN"},
  {"石川県能登地方","NOT"},
  {"能登半島沖","NHO"},
  {"富山湾","TYW"},
  {"長野県北部","NGN"},
  {"長野県中部","NGC"},
  {"長野県南部","NGS"},
  {"福井県嶺北","FKI"},
  {"福井県嶺南","FKR"},
  {"岐阜県美濃中西部","GIF"},

  {"京都府北部","KYN"},
  {"京都府南部","KYT"},
  {"大阪府北部","OSK"},
  {"大阪府南部","OSS"},
  {"奈良県","NAR"},
  {"奈良県北部","NAR"},
  {"奈良県南部","NAR"},
  {"兵庫県北部","HYG"},
  {"兵庫県南東部","HSN"},
  {"兵庫県南西部","HSW"},
  {"淡路島付近","AWD"},
  {"滋賀県北部","SGN"},
  {"滋賀県南部","SIG"},
  {"和歌山県北部","WKN"},
  {"和歌山県南方沖","WKM"},
  {"紀伊水道","KIS"},
  {"三重県南東沖","MNE"},
  {"熊野灘","KMN"},

  {"鳥取県東部","TTE"},
  {"鳥取県西部","TTB"},
  {"島根県東部","SME"},
  {"島根県西部","SMW"},
  {"島根県沖","SMO"},
  {"安芸灘","AKN"},
  {"伊予灘","IYN"},
  {"愛媛県東予","EHT"},
  {"愛媛県中予","EHC"},
  {"愛媛県南予","EHN"},
  {"徳島県北部","TKS"},
  {"高知県東部","KCE"},
  {"高知県西部","KCW"},
  {"土佐湾","TSW"},
  {"四国沖","SHO"},

  {"福岡県西方沖","FKO"},
  {"熊本県熊本地方","KMT"},
  {"大分県中部","OIC"},
  {"大分県南部","OIS"},
  {"豊後水道","BNG"},
  {"日向灘","HNG"},
  {"宮崎県北部平野部","MYP"},
  {"宮崎県北部山沿い","MYM"},
  {"鹿児島県薩摩地方","KGS"},
  {"鹿児島県大隅地方","KGO"},
  {"薩摩半島西方沖","SST"},
  {"種子島近海","TNS"},
  {"トカラ列島近海","TKR"},
  {"奄美大島近海","AMM"},
  {"奄美大島北東沖","AMO"},
  {"沖縄本島近海","OKA"},
  {"沖縄本島北西沖","ONO"},
  {"沖縄南方沖","OSO"},
  {"宮古島近海","MYK"},
  {"与那国島近海","YNG"}
};

String getGSCode(String hypo){

  for(auto &e : gsTable){
    if(hypo == e.hypo){
      return e.code;
    }
  }

  return "???";
}

void showWaiting() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("WiFi OK");

  display.setTextSize(1);
  display.setCursor(0,35);
  display.println("Waiting...");

  display.display();
}

void warningAlert() {

  for(int i=0;i<3;i++) {

    for(int f=1000;f<=2200;f+=100){

      digitalWrite(LED_PIN,HIGH);

      tone(BUZZER_PIN,f);

      delay(25);
    }

    for(int f=2200;f>=1000;f-=100){

      tone(BUZZER_PIN,f);

      delay(25);
    }

    digitalWrite(LED_PIN,LOW);

    noTone(BUZZER_PIN);

    delay(150);
  }
}

void setup() {

  Serial.begin(115200);

  Wire.begin(21,22);

  if(!display.begin(
      SSD1306_SWITCHCAPVCC,
      0x3C
  )) {

    while(true);
  }

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setTextSize(2);

  display.setCursor(0,0);
  display.println("BOOT");

  display.display();

  delay(1000);

  WiFi.begin(
    ssid,
    password
  );

  int count = 0;

  while(
    WiFi.status() != WL_CONNECTED &&
    count < 20
  ) {

    count++;

    display.clearDisplay();

    display.setTextSize(2);

    display.setCursor(0,0);
    display.println("WIFI");

    display.setTextSize(1);

    display.setCursor(0,35);

    display.print("TRY ");
    display.println(count);

    display.display();

    delay(500);
  }

  if(
    WiFi.status() ==
    WL_CONNECTED
  ) {

    Serial.println(
      "WiFi OK"
    );

  } else {

    Serial.println(
      "WiFi FAIL"
    );
  }
  pinMode(LED_PIN, OUTPUT);
pinMode(BUZZER_PIN, OUTPUT);

digitalWrite(LED_PIN, LOW);
digitalWrite(BUZZER_PIN, LOW);
}

void loop() {

  if(WiFi.status()!=WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("NO WIFI");

    display.display();

    delay(1000);

    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  http.begin(client,url);

  int code=http.GET();

  if(code==200) {

    String payload=http.getString();

    JsonDocument doc;

    if(
      deserializeJson(
        doc,
        payload
      ) == DeserializationError::Ok
    ) {

      String eventID =
        doc["EventID"] | "";

      int serial =
        doc["Serial"] | 0;

      String intensity =
        doc["MaxIntensity"] | "?";
      if(intensity == "5弱") intensity = "5-";
      if(intensity == "5強") intensity = "5+";
      if(intensity == "6弱") intensity = "6-";
      if(intensity == "6強") intensity = "6+";
      if(
        intensity == "" ||
        intensity == "不明" ||
        intensity == "なし"
      ){
        intensity = "?";
      }

      float mag =
        doc["Magunitude"] | 0.0;

      bool isWarn =
        doc["isWarn"] | false;

      bool isFinal =
        doc["isFinal"] | false;

      bool isCancel =
        doc["isCancel"] | false;

      bool isPLUM =
        doc["isAssumption"] | false; 

      String hypo =
        doc["Hypocenter"] | "";

      String gsCode =
        getGSCode(hypo);

        Serial.print("HYPO=");
        Serial.println(hypo);

        Serial.print("GS=");
        Serial.println(gsCode);

     if(isCancel) {
        digitalWrite(LED_PIN, LOW);
        noTone(BUZZER_PIN);

        lastEventID = "";
        lastSerial = 0;

        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0,20);
        display.println("CANCEL");
        display.display();

      delay(5000);
      http.end();
      return;
    }

      bool updated = false;

      if(eventID != lastEventID) {

        updated = true;

        lastEventID =
          eventID;
      }

      if(serial > lastSerial) {

        updated = true;

        lastSerial =
          serial;
      }

        Serial.print("updated=");
        Serial.println(updated);

        Serial.print("isFinal=");
        Serial.println(isFinal);

        Serial.print("serial=");
        Serial.println(serial);

        Serial.print("lastSerial=");
        Serial.println(lastSerial);

      if(
        updated &&
        isWarn 
      ) {

        warningAlert();
      }

        if(isFinal && !updated) {

        delay(3500);

        showWaiting();

        http.end();

       delay(1000);

       return;
      }

      display.clearDisplay();

      display.setTextSize(2);

      display.setCursor(0,0);

      if(isPLUM){

      display.print("PLUM#");

    }else if(isFinal){

      display.print("FINAL#");

    }else{

      display.print("EEW!#");
    }

      display.println(serial);

      display.setTextSize(3);

      display.setCursor(0,22);

      display.println(intensity);

      display.setTextSize(1);

      display.setCursor(80,40);

      display.println(gsCode);

      display.setTextSize(1);

      display.setCursor(80,54);

      display.print("M");

      if(mag > 0){

      display.print(mag,1);

    }else{

      display.print("?");
    }

      display.display();

      http.end();

      delay(1000);
    }

} else {

  showWaiting();
  return;
}

http.end();

delay(1000);
}