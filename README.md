# IOT-Smart-Home System
This project is an IoT-based Smart Home System designed to monitor environmental conditions and control devices. It uses an ESP8266 E12 microcontroller, a DHT11 sensor, ultrasonic sensor, LEDs, and a buzzer to perform edge computing and communicate with ThingSpeak for cloud computing to perform data logging and analysis.

**Features** <br>
Environmental Monitoring: Measures temperature and humidity using a DHT11 sensor.<br>
Distance Measurement: Uses an ultrasonic sensor to detect obstacles and measure distance.<br>
Alert System: Includes an LED and buzzer for notifications based on threshold values.<br>
Data Logging: Sends sensor data to ThingSpeak for real-time monitoring and visualization.<br>
WiFi Connectivity: Connects to a WiFi network for data communication.<br>

**Hardware Components** <br>
ESP8266 microcontroller<br>
DHT11 Temperature and Humidity Sensor<br>
Ultrasonic Sensor (HC-SR04)<br>
LEDs (Red, Green, Blue, Alert)<br>
Buzzer<br>
Miscellaneous: Breadboard, resistors, wires

**Software Requirements** <br>
Arduino IDE<br>
ESP8266 WiFi Library<br>
ThingSpeak Library<br>
DHT Library<br>

**Circuit Setup**<br>
Connect the DHT11 sensor to the ESP8266 as follows:<br>
Data pin to GPIO2 (D4)<br>
Power and Ground to VCC and GND<br>
Connect the ultrasonic sensor:<br>
Trig pin to GPIO14 (D5)<br>
Echo pin to GPIO12 (D6)<br>
Connect the LEDs:<br>
Red LED to GPIO5 (D1)<br>
Green LED to GPIO4 (D2)<br>
Blue LED to GPIO0 (D3)<br>
Alert LED to GPIO15 (D8)<br>
Connect the buzzer to GPIO13 (D7).<br>

**Configuration**<br>
Update WiFi credentials and Thingspeak configurations in the code:<br>

**Usage**<br>
Power on the ESP8266 module.<br>
The system will connect to the specified WiFi network.<br>
Sensor readings are displayed on the serial monitor and sent to ThingSpeak.<br>
Alerts are triggered when values exceed predefined thresholds.<br>

**Edge Computing**<br>
Implements smoothing algorithms to average sensor readings.<br>
Maintains a rolling buffer for efficient data processing.<br>

**Future Improvements**<br>
Integration with a mobile app for remote monitoring.<br>
Expandable support for additional sensors or devices.<br>
