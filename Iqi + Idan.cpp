#define BLYNK_TEMPLATE_ID "TMPL6255uu64b"
#define BLYNK_TEMPLATE_NAME "Time Control Pump"
#define BLYNK_AUTH_TOKEN "fnG7v_YwF6cx_t5U1cFdZeyFEju2EbAI"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Setup Definisi PIN Virtual
#define VPIN_waterLevel V6
#define VPIN_tinggiAir V7
#define VPIN_soilMoisture V8

// Setup Definisi PIN ESP32
#define IN1_A 19             // Pin Kontrol Arah Motor
#define IN2_A 18             // Pin Kontrol Arah Motor
#define ENA_PIN 21           // Pin Kontrol Speed Motor
#define SOIL_MOISTURE_PIN 34 // Gunakan pin analog yang tersedia di ESP32
#define TRIGPIN 16           // Pin UltraSonic
#define ECHOPIN 17           // Pin UltraSonic

// Setup Konfirgurasi nilai ANALOG
// Kalibrasi sensor tanah (sesuaikan dengan sensor Anda)
#define dry_soil_value 4095   // Nilai ADC saat sensor dalam keadaan kering
#define wet_soil_value 1500   // Nilai ADC saat sensor dalam keadaan basah

//Setup untuk PWM ( Kontrol Speed Motor )
const int pwmChannel = 0;      // PWM Channel
const int pwmFrequency = 5000; // Frequency in Hz
const int pwmResolution = 8;   // Resolution: 8 bits (0-255)

//Konfigurasi Wifi
const char *ssid = "Hernandos_Bawah";
const char *password = "bellarosa";

// Variable untuk Speed pada awal ( MAX, range 0 - 255)
int speed = 255;
// Setup Waktu untuk GMT 7+
const long IndoTime = 7 * 3600; 

// Untuk Timer Measure Soil
BlynkTimer timer;


//Function untuk Mengontrol Pump
void controlPump(bool state);
// NTP Untuk Set Waktu
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", IndoTime);

int targetHour = -1;   // Initial value
int targetMinute = -1; // Initial value

bool Mode_For_On = false; // True = Pakai Clock untuk nyala, False = Makai button untuk Nyala

//Boolean untuk Indikator Air dan Soil
bool LowWater = false;
bool LowMoisture = false;

//Variabel untuk Ultrasonik dan Sensor Tanah
float duration;
float distance;
int   waterLevelPer;
int   soilMoistureValue = 0;
int   soilMoisturePercentage = 0;

// Threshold untuk Kondisi Tank
int emptyTankDistance = 70;
int fullTankDistance = 30;

//Sinkronisasi nilai Pin dengan PIN VIRTUAL 
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_waterLevel);
  Blynk.syncVirtual(VPIN_tinggiAir);
  Blynk.syncVirtual(VPIN_soilMoisture);  
} 

// v0 untuk Pilih mode
BLYNK_WRITE(V0)
{
  int value = param.asInt();
  Mode_For_On = (value == 1);
  Serial.println(Mode_For_On ? "Mode: Clock" : "Mode: Button");
}

// V1 untuk input Waktu via blynk, string
BLYNK_WRITE(V1)
{
  String input = param.asString();
  int separator = input.indexOf(':');
  if (separator != -1)
  {
    targetHour = input.substring(0, separator).toInt();
    targetMinute = input.substring(separator + 1).toInt();
    if (targetHour >= 0 && targetHour < 24 && targetMinute >= 0 && targetMinute < 60)
    {
      Serial.printf("Sprinkler Menyala pada jam %02d:%02d.\n", targetHour, targetMinute);
    }
    else
    {
      Serial.println("Input Invalid.");
      targetHour = -1;
      targetMinute = -1;
    }
  }
  else
  {
    Serial.println("Format Invalid.");
  }
}

// v2 untuk button nyalain
BLYNK_WRITE(V2)
{
  int value = param.asInt();
  if (!Mode_For_On)
  {
    controlPump(value == 1);
    Serial.println(value == 1 ? "Pump Nyala" : "Pump Mati");
  }
  else
  {
    Serial.println("Manual control disabled in automatic mode.");
  }
}

// v4 untuk indikasi LED pump nyala / mati
void controlPump(bool state)
{
  if (state && !LowWater)
  {
    Serial.println("Pump Nyala");
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    Blynk.virtualWrite(V4, 1); // Update pump state to ON
  }
  else
  {
    Serial.println("Pump Mati");
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, LOW);
    Blynk.virtualWrite(V4, 0); // Update pump state to OFF
  }
}
void handleWiFiStatusChange(bool isConnected)
{
  if (isConnected)
  {
    Serial.println("WiFi Connected");
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
  Blynk.virtualWrite(V3, isConnected ? 1 : 0); // Update Blynk LED widget
}

BLYNK_WRITE(V5)
{
  int speed = param.asInt(); // Get the speed value from Blynk
  if (speed >= 0 && speed <= 255)
  {
    ledcWrite(pwmChannel, speed);
    Serial.printf("Motor speed set to: %d (from Blynk)\n", speed);
  }
  else
  {
    Serial.println("Invalid speed. Blynk value must be between 0 and 255.");
  }
}

//
void checkTimeTask(void *parameter)
{
  while (true)
  {
    if (Mode_For_On)
    {
      timeClient.update();
      int currentHour = timeClient.getHours();
      int currentMinute = timeClient.getMinutes();
      if (targetHour != -1 && currentHour == targetHour && currentMinute == targetMinute)
      {
        controlPump(true);                      // pump nyala
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Delay
      }
      else
      {
        controlPump(false); // Turn off the pump
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay lagi
  }
}

// Task for updating WiFi status
void updateWiFiStatusTask(void *parameter)
{
  static bool Statuslast = false;
  while (true)
  {
    bool isConnected = (WiFi.status() == WL_CONNECTED);
    if (isConnected != Statuslast)
    {
      Statuslast = isConnected;
      handleWiFiStatusChange(isConnected);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void measureDistance(){
  // Set the trigger pin LOW for 2uS
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  // Set the trigger pin HIGH for 20us to send pulse
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(20);
  // Return the trigger pin to LOW
  digitalWrite(TRIGPIN, LOW);
  // Measure the width of the incoming pulse
  duration = pulseIn(ECHOPIN, HIGH);
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  // Divide by 1000 as we want millimeters
  distance = ((duration / 2) * 0.343)/10;
  if (distance > (fullTankDistance - 10) && distance < emptyTankDistance){
    waterLevelPer = map((int)distance, emptyTankDistance, fullTankDistance, 0, 100);
    Blynk.virtualWrite(VPIN_waterLevel, waterLevelPer);
    Blynk.virtualWrite(VPIN_tinggiAir, (String(distance) + " cm"));
    // Print result to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm"); 
    // If Function untuk Check Air Rendah Or No  
    if ( distance > 50){
      LowWater = true;
    }
    else{
      LowWater = false;
    }
  }
  else 
    // Delay before repeating measurement
    delay(100);
}

void measureSoilMoisture() {
  // Baca nilai analog dari sensor tanah
  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  // Konversi ke persentase
  // Semakin rendah nilainya, semakin tinggi kelembaban
  soilMoisturePercentage = map(soilMoistureValue, dry_soil_value, wet_soil_value, 0, 100);
  // Batasi persentase antara 0-100
  soilMoisturePercentage = constrain(soilMoisturePercentage, 0, 100);
  // Kirim data ke Blynk
  Blynk.virtualWrite(VPIN_soilMoisture, soilMoisturePercentage);
  // Cetak ke serial monitor untuk debugging
  Serial.print("Soil Moisture Value: ");
  Serial.print(soilMoistureValue);
  Serial.print(" | Soil Moisture Percentage: ");
  Serial.print(soilMoisturePercentage);
  Serial.println("%");

  // Tolong Cek lagi, arti Persen makin tinggi itu makin basah atau makin kering
  // kode dibawah dibikin dengan asumsi persen tinggi == tanah basah
  if (soilMoisturePercentage < 20 ){
      LowMoisture = true;
      controlPump(LowMoisture);
      Serial.println(" Kadar Air rendah, Sprinkler Akan Dinyalakan.");
  }
}

void setup()
{
  Serial.begin(9600);
  // Definisi Pin MODE
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT); 
  
  // Jangan Hapus, incase Heap kurang. Untuk Logging
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timeClient.begin();
  Blynk.virtualWrite(V5, speed);
  timer.setInterval(5000L, measureSoilMoisture);

  // PWM setup for ENA
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(ENA_PIN, pwmChannel);
  ledcWrite(pwmChannel, speed);

  xTaskCreatePinnedToCore(checkTimeTask, "Time Check Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(updateWiFiStatusTask, "WiFi Status Task", 4096, NULL, 1, NULL, 1);
}

void loop()
{
  measureDistance();
  Blynk.run();
  timer.run() // Handle Blynk operations
}