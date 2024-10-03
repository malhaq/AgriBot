#include <esp_now.h>
#include <WiFi.h>

// Sensors Pins
const int soil_sensor1 = 32;
const int soil_sensor2 = 33;
const int soil_sensor3 = 34;
const int soil_sensor4 = 35;

// Structure to hold incoming data
typedef struct struct_message {
    char a[32];
    float lat;
    float lng;
    char b[32];
} struct_message;

struct_message incomingData;

struct_message response;

// Function to handle incoming data
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    struct_message receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));  // Copy incoming data to a struct_message instance

    if (strcmp(receivedData.a, "humidity") == 0) { 
        // Simulate sensor reading (replace with actual sensor code)
        
        strcpy(response.a, "humidity");
        response.lat = receivedData.lat;
        response.lng = receivedData.lng;
        String res = readHumi();
        strcpy(response.b, res.c_str());  // Convert String to const char* before copying
        esp_now_send(mac_addr, (uint8_t *)&response, sizeof(response));
    }
}

// Callback function when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
//   struct_message receivedData;
//   memcpy(&receivedData, incomingData, sizeof(receivedData));  // Copy incoming data to a struct_message instance

//   if (strcmp(incomingData.a, "direction") == 0) {
//     // Save the incoming latitude and longitude to the array
//     if (tupleCount < MAX_TUPLES) {
//       directionTuples[tupleCount] = incomingData;
//       tupleCount++;
//     }
//   } else if (strcmp(incomingData.a, "get_direction") == 0) {
//     // Compare the incoming latitude and longitude with stored tuples
//     bool found = false;
//     for (int i = 0; i < tupleCount; i++) {
//       if (incomingData.lat == directionTuples[i].lat && incomingData.lng == directionTuples[i].lng) {
//         // Send the matching direction back to the sender
//         struct_message response;
//         strcpy(response.a, directionTuples[i].a);
//         response.lat = directionTuples[i].lat;
//         response.lng = directionTuples[i].lng;
//         strcpy(response.b,directionTuples[i].b);
        
//         esp_now_send(mac_addr, (uint8_t *)&response, sizeof(response));
//         found = true;
//         break;
//       }
//     }
//     if (!found) {
//       // If no match is found, send a "not found" message
//       struct_message response;
//       strcpy(response.a, "not_found");
//       response.lat = 0;
//       response.lng = 0;
//       strcpy(response.b,"straight");
//       esp_now_send(mac_addr, (uint8_t *)&response, sizeof(response));
//     }
//   } else if (strcmp(incomingData.a, "humidity") == 0) {//TODO create function to read all sensors and return on or off
//       // Simulate sensor reading (replace with actual sensor code)
      
//       struct_message response;
//       strcpy(response.a, "humidity");
//       response.lat = incomingData.lat;
//       response.lng = incomingData.lng;
//       String res = readHumi();
//       strcpy(response.b,res);
//       esp_now_send(mac_addr, (uint8_t *)&response, sizeof(response));
//   }
// }

// Setup ESP-NOW and Wi-Fi
void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register the receive callback
    esp_now_register_recv_cb(OnDataRecv);

    // Register the send callback
    esp_now_register_send_cb(onDataSent);
}

void loop() {
    
}
String readHumi() {
  int m1 = analogRead(soil_sensor1);
  int m2 = analogRead(soil_sensor2);
  int m3 = analogRead(soil_sensor3);
  int m4 = analogRead(soil_sensor4);
  Serial.print("sensor 1:");
  Serial.println(m1);
  Serial.print("sensor 2:");
  Serial.println(m2);
  Serial.print("sensor 3:");
  Serial.println(m3);
  Serial.print("sensor 4:");
  Serial.println(m4);
  int count=0;
  if(m1<=2000)count++;
  if(m2<=2000)count++;
  if(m3<=2000)count++;
  if(m4<=2000)count++;
  if(count>=2){
    return "on";
  }else{
    return "off";
  }
}