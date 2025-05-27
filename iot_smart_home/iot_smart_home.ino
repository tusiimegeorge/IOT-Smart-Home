#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Your wifi network name";
const char* password = "Your wifi password";

// ThingSpeak settings
unsigned long myChannelNumber = 2777711; // Replace with your channel ID
const char* myWriteAPIKey = "Your api key"; // Replace with your Write API Key

WiFiClient client;

// DHT settings
#define DHTPIN D4 // GPIO2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED pins
#define RED_LED D1   // GPIO5
#define GREEN_LED D2 // GPIO4
#define BLUE_LED D3  // GPIO0
#define ALERT_LED D8 // GPIO15 (New LED for alert)

// Ultrasonic sensor pins
#define TRIG_PIN D5 // GPIO14
#define ECHO_PIN D6 // GPIO12

// Buzzer pin
#define BUZZER_PIN D7 // GPIO13

// Edge computing variables
const int numReadings = 5;
float temperatureReadings[numReadings] = {0};
float humidityReadings[numReadings] = {0};
float distanceReadings[numReadings] = {0};
int readIndex = 0;
int readingsCount = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LEDs as outputs
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(ALERT_LED, OUTPUT); // Initialize ALERT_LED as output

  // Initialize ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer initially

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the time for the echo
  long duration = pulseIn(ECHO_PIN, HIGH);
  // Calculate the distance (in cm)
  float distance = (duration * 0.034) / 2; // Speed of sound: 340 m/s
  return distance;
}

void loop() {
  // Read temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Measure distance from ultrasonic sensor
  float distance = measureDistance();

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (distance == 0 || distance > 400) {
    Serial.println("Invalid distance reading!");
    return;
  }

  // **Activate buzzer and ALERT_LED if the distance is below 7 cm**
  if (distance < 7.0) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
    digitalWrite(ALERT_LED, HIGH); // Turn on ALERT_LED
    Serial.println("Warning: Object detected within 7 cm!");
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
    digitalWrite(ALERT_LED, LOW); // Turn off ALERT_LED
  }

  // Store readings in arrays for edge computing
  temperatureReadings[readIndex] = temperature;
  humidityReadings[readIndex] = humidity;
  distanceReadings[readIndex] = distance;
  readIndex = (readIndex + 1) % numReadings;

  if (readingsCount < numReadings) {
    readingsCount++;
  }

  // Compute averages when we have enough readings
  if (readingsCount == numReadings) {
    float avgTemperature = 0;
    float avgHumidity = 0;
    float avgDistance = 0;

    for (int i = 0; i < numReadings; i++) {
      avgTemperature += temperatureReadings[i];
      avgHumidity += humidityReadings[i];
      avgDistance += distanceReadings[i];
    }

    avgTemperature /= numReadings;
    avgHumidity /= numReadings;
    avgDistance /= numReadings;

    // Print averages to Serial Monitor
    Serial.print("Average Temperature: ");
    Serial.print(avgTemperature);
    Serial.println(" \u00B0C");
    Serial.print("Average Humidity: ");
    Serial.print(avgHumidity);
    Serial.println(" %");
    Serial.print("Average Distance: ");
    Serial.print(avgDistance);
    Serial.println(" cm");

    // LED Control based on average temperature
    if (avgTemperature > 28) {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BLUE_LED, LOW);
    } else if (avgTemperature < 20) {
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BLUE_LED, HIGH);
    } else {
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(BLUE_LED, LOW);
    }

    // Send averaged data to ThingSpeak
    ThingSpeak.setField(1, avgTemperature); // Field 1: Temperature
    ThingSpeak.setField(2, avgHumidity);    // Field 2: Humidity
    ThingSpeak.setField(3, avgDistance);    // Field 3: Distance
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Averaged data successfully sent to ThingSpeak!");
    } else {
      Serial.println("Error sending averaged data to ThingSpeak");
    }

    // Reset readings count for the next batch
    readingsCount = 0;
  }

  // Wait before collecting the next reading
  delay(1000); // -second delay for each loop iteration
}
