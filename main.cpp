#define BLYNK_TEMPLATE_ID "TMPL6255uu64b"
#define BLYNK_TEMPLATE_NAME "Time Control Pump"
#define BLYNK_AUTH_TOKEN "fnG7v_YwF6cx_t5U1cFdZeyFEju2EbAI"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


// Pin Arah Motor
#define IN1_A 19
#define IN2_A 18
#define ENA_PIN 21


const int pwmChannel = 0;       // PWM Channel
const int pwmFrequency = 5000;  // Frequency in Hz
const int pwmResolution = 8;    // Resolution: 8 bits (0-255)


const char* ssid = "Hernandos_Bawah";
const char* password = "bellarosa";


int speed = 255;


const long IndoTime = 7 * 3600; // Offset untuk GMT 7+
void controlPump(bool state);


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", IndoTime);


int targetHour = -1;   // Initial value
int targetMinute = -1; // Initial value


bool Mode_For_On = false; // True = Pakai Clock untuk nyala, False = Makai button untuk Nyala


// v0 untuk Pilih mode
BLYNK_WRITE(V0) {
 int value = param.asInt();
 Mode_For_On = (value == 1);
 Serial.println(Mode_For_On ? "Mode: Clock" : "Mode: Button");
}


// V1 untuk input Waktu via blynk, string
BLYNK_WRITE(V1) {
 String input = param.asString();
 int separator = input.indexOf(':');
 if (separator != -1) {
   targetHour = input.substring(0, separator).toInt();
   targetMinute = input.substring(separator + 1).toInt();
   if (targetHour >= 0 && targetHour < 24 && targetMinute >= 0 && targetMinute < 60) {
     Serial.printf("Sprinkler Menyala pada jam %02d:%02d.\n", targetHour, targetMinute);
   } else {
     Serial.println("Input Invalid.");
     targetHour = -1;
     targetMinute = -1;
   }
 } else {
   Serial.println("Format Invalid.");
 }
}


// v2 untuk button nyalain
BLYNK_WRITE(V2) {
 int value = param.asInt();
 if (!Mode_For_On) {
   controlPump(value == 1);
   Serial.println(value == 1 ? "Pump Nyala" : "Pump Mati");
 } else {
   Serial.println("Manual control disabled in automatic mode.");
 }
}


// v4 untuk indikasi LED pump nyala / mati
void controlPump(bool state) {
 if (state) {
   Serial.println("Pump Nyala");
   digitalWrite(IN1_A, HIGH);
   digitalWrite(IN2_A, LOW);
   Blynk.virtualWrite(V4, 1); // Update pump state to ON
 } else {
   Serial.println("Pump Mati");
   digitalWrite(IN1_A, LOW);
   digitalWrite(IN2_A, LOW);
   Blynk.virtualWrite(V4, 0); // Update pump state to OFF
 }
}
void handleWiFiStatusChange(bool isConnected) {
 if (isConnected) {
   Serial.println("WiFi Connected");
 } else {
   Serial.println("WiFi Disconnected");
 }
 Blynk.virtualWrite(V3, isConnected ? 1 : 0); // Update Blynk LED widget
}


BLYNK_WRITE(V5) {
 int speed = param.asInt(); // Get the speed value from Blynk
 if (speed >= 0 && speed <= 255) {
   ledcWrite(pwmChannel, speed);
   Serial.printf("Motor speed set to: %d (from Blynk)\n", speed);
 } else {
   Serial.println("Invalid speed. Blynk value must be between 0 and 255.");
 }
}


//
void checkTimeTask(void* parameter) {
 while (true) {
   if (Mode_For_On) {
     timeClient.update();
     int currentHour = timeClient.getHours();
     int currentMinute = timeClient.getMinutes();


     if (targetHour != -1 && currentHour == targetHour && currentMinute == targetMinute) {
       controlPump(true); // pump nyala
       vTaskDelay(60000 / portTICK_PERIOD_MS); // Delay
     } else {
       controlPump(false); // Turn off the pump
     }
   }
   vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay lagi
 }
}


// Task for updating WiFi status
void updateWiFiStatusTask(void* parameter) {
 static bool Statuslast = false;
 while (true) {
   bool isConnected = (WiFi.status() == WL_CONNECTED);
   if (isConnected != Statuslast) {
     Statuslast = isConnected;
     handleWiFiStatusChange(isConnected);
   }
   vTaskDelay(5000 / portTICK_PERIOD_MS);
 }
}


void setup() {
 pinMode(IN1_A, OUTPUT);
 pinMode(IN2_A, OUTPUT);


 Serial.begin(9600);


Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());




 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 Serial.println("\nWiFi Connected");


 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
 timeClient.begin();
 Blynk.virtualWrite(V5,speed);


   // PWM setup for ENA
 ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
 ledcAttachPin(ENA_PIN, pwmChannel);
 ledcWrite(pwmChannel, speed);




 xTaskCreatePinnedToCore(checkTimeTask, "Time Check Task", 4096, NULL, 1, NULL, 0);
 xTaskCreatePinnedToCore(updateWiFiStatusTask, "WiFi Status Task", 4096, NULL, 1, NULL, 1);
}


void loop() {
 Blynk.run(); // Handle Blynk operations
}