#define BLYNK_TEMPLATE_ID "TMPL6QOwCNBbd"
#define BLYNK_TEMPLATE_NAME "IOT 22 SIAP MENGGANYANG FINPRO ENAK YAHUD LEGIT"
#define BLYNK_AUTH_TOKEN "zAEjCpKp2yRDd70_E1OrC-gYMgtOPBG3"

#define VPIN_waterLevel V0
#define VPIN_tinggiAir V1
#define VPIN_soilMoisture V2  // Tambahan pin virtual untuk kelembaban tanah

char ssid[] = "Dane terlalu rawr";
char pass[] = "Unicaca123";

int emptyTankDistance = 70;
int fullTankDistance = 30;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Define connections to ultrasonic sensor
#define TRIGPIN    19
#define ECHOPIN    18

// Define connection to soil moisture sensor
#define SOIL_MOISTURE_PIN 34  // Gunakan pin analog yang tersedia di ESP32

float duration;
float distance;
int   waterLevelPer;
int   soilMoistureValue = 0;
int   soilMoisturePercentage = 0;

// Kalibrasi sensor tanah (sesuaikan dengan sensor Anda)
#define dry_soil_value 4095   // Nilai ADC saat sensor dalam keadaan kering
#define wet_soil_value 1500   // Nilai ADC saat sensor dalam keadaan basah

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_waterLevel);
  Blynk.syncVirtual(VPIN_tinggiAir);
  Blynk.syncVirtual(VPIN_soilMoisture);  // Sinkronkan pin virtual soil moisture
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
}
 
void setup() {
  // Set up serial monitor
  Serial.begin(115200);
 
  // Set pinmodes for sensor connections
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Set pin sensor tanah sebagai input

  WiFi.begin(ssid, pass);
  Blynk.config(auth);
  
  // Tambahkan timer untuk membaca sensor tanah setiap 5 detik
  timer.setInterval(5000L, measureSoilMoisture);
  
  delay(1000);
}
 
void loop() {
  measureDistance();

  Blynk.run();
  timer.run(); // Initiates SimpleTimer   
}