#define BLYNK_TEMPLATE_ID "TMPL6yzcqnQwu"
#define BLYNK_TEMPLATE_NAME "Fire and Gas"
#define BLYNK_AUTH_TOKEN "cqgZan-eUMEmUAoSmTre5_VdNnb1h5p-"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

// WiFi
char ssid[] = "loooo";
char pass[] = "12345679";
// char ssid[] = "TP-Link_9430";
// char pass[] = "51563103";

// GPIO
#define CB_chay  27  // Cảm biến cháy
#define CB_khoi  14  // Cảm biến khói
#define buzz     26  // Còi báo
#define quat     33  // Quạt
#define bom      25  // Bơm

// Trạng thái
bool gui1 = false;
bool gui2 = false;
bool gui3 = false;
bool canh_bao_bat = false;
unsigned long lastBuzzTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("MO HINH BAO CHAY - ESP32");

  Wire.begin(21, 22);  // SDA, SCL
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MO HINH BAO CHAY");
  lcd.setCursor(0, 1);
  lcd.print("HT BINH THUONG");

  pinMode(CB_khoi, INPUT_PULLUP);
  pinMode(CB_chay, INPUT_PULLUP);
  pinMode(bom, OUTPUT);
  pinMode(quat, OUTPUT);
  pinMode(buzz, OUTPUT);

  digitalWrite(buzz, LOW);
  digitalWrite(bom, LOW);  // Bơm tắt
  digitalWrite(quat, LOW); // Quạt tắt

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  int GT_cbkhoi = digitalRead(CB_khoi);
  int GT_cbchay = digitalRead(CB_chay);


 // Phát hiện có khói
if (GT_cbchay == LOW && GT_cbkhoi == HIGH) {
  Serial.println("PHAT HIEN KHOI");
  lcd_hien_thi("PHAT HIEN KHOI");
  canh_bao_bat = true;
  quatchay();

  if (!gui1) {
    Blynk.logEvent("smoke_detected", "PHAT HIEN CO KHOI");
    gui1 = true;
  }
}
// Phát hiện có chay
else if (GT_cbkhoi == LOW && GT_cbchay == HIGH) {
  Serial.println("PHAT HIEN CHAY");
  lcd_hien_thi("PHAT HIEN CHAY");
  canh_bao_bat = true;
  quatchay();
  bomchay();

  if (!gui2) {
    Blynk.logEvent("fire_detected", "PHAT HIEN CO CHAY");
    gui2 = true;
  }
}
// Phát hiện cháy + khói
else if (GT_cbkhoi == LOW && GT_cbchay == LOW) {
  Serial.println("CO CHAY VA KHOI");
  lcd_hien_thi("CO CHAY VA KHOI");
  canh_bao_bat = true;
  quatchay();
  bomchay();

  if (!gui3) {
    Blynk.logEvent("fire_and_smoke", "PHAT HIEN CO CHAY VA KHOI");
    gui3 = true;
  }
}

  // Hệ thống bình thường
  else {
    if (canh_bao_bat) {
      Serial.println("HT BINH THUONG");
      lcd_hien_thi("HT BINH THUONG");
      quattat();
      bomtat();
      digitalWrite(buzz, LOW);
      gui1 = gui2 = gui3 = false;
      canh_bao_bat = false;
    }
  }

  // Còi báo nhấp nháy
  if (canh_bao_bat) {
    unsigned long now = millis();
    if (now - lastBuzzTime >= 500) {
      digitalWrite(buzz, !digitalRead(buzz)); // Đảo trạng thái còi
      lastBuzzTime = now;
    }
  }
}

// ===== Hàm điều khiển =====
void lcd_hien_thi(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MO HINH BAO CHAY");
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

void bomchay() {
  digitalWrite(bom, HIGH);  // Bật bơm
}
void bomtat() {
  digitalWrite(bom, LOW); // Tắt bơm
}
void quatchay() {
  digitalWrite(quat, HIGH);  // Bật quạt
}
void quattat() {
  digitalWrite(quat, LOW); // Tắt quạt
}
