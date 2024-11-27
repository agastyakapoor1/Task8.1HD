#include <Wire.h>
#include <BH1750.h>
#include <ArduinoBLE.h>  // Bluetooth library for Nano 33 IoT

// Create BH1750 object
BH1750 lightMeter;

// Bluetooth characteristics
BLEService lightSensorService("12345678-1234-5678-1234-56789abcdef0");  // Custom service UUID
BLEIntCharacteristic luxCharacteristic("abcdef01-1234-5678-1234-56789abcdef1", BLERead | BLENotify);  // Characteristic UUID for light intensity

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);
  delay(1000);  // Give some time for Serial Monitor to start
  Serial.println("Starting Setup...");

  // Initialize I2C communication
  Serial.println("Initializing BH1750 sensor...");
  Wire.begin();
  
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Error initializing BH1750!");
    while (1);
  }
  Serial.println("BH1750 initialized.");

  // Initialize BLE
  Serial.println("Initializing BLE...");
  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® failed!");
    while (1);
  }
  Serial.println("BLE initialized successfully.");

  // Set device name and advertise the service
  BLE.setLocalName("Nano33IOT_LightSensor");
  BLE.setAdvertisedService(lightSensorService);

  // Add characteristic to the service
  lightSensorService.addCharacteristic(luxCharacteristic);

  // Add service
  BLE.addService(lightSensorService);

  // Start advertising
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // Poll for Bluetooth device
  BLEDevice central = BLE.central();

  // If a central device is connected
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // While central device is still connected
    while (central.connected()) {
      // Read the BH1750 light sensor data
      float lux = lightMeter.readLightLevel();
      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");

      // Update the Bluetooth characteristic with the sensor data
      luxCharacteristic.writeValue((int)lux);

      // Small delay before reading again
      delay(1000);
    }
    
    Serial.println("Disconnected from central.");
  }

  // Small delay if no central is connected
  delay(500);
}
