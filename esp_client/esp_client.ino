#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <sstream>


float lati;
float lngi;

// Receiver MAC address for ESP-NOW protocol
uint8_t receiverAddress[] = {0x94, 0xB5, 0x55, 0xF3, 0x2C, 0x94};

// Structure to send data
typedef struct struct_message {
    char a[32];
    float lat;
    float lng;
    char b[32];
} struct_message;

// Array to hold direction tuples
#define MAX_TUPLES 30  // Adjust the size as needed
struct_message directionTuples[MAX_TUPLES];
int tupleCount = 0;

// Create a struct_message to hold outgoing data
struct_message outgoingData;

// Create a struct_message to hold incoming data
struct_message incomingData;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Handle send status if needed
    if(status == ESP_NOW_SEND_SUCCESS){

    }else{

    }
}

// Callback when data is received
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len == sizeof(struct_message)) {
        memcpy(&incomingData, data, sizeof(struct_message));
        char khalaf;
        if ((strcmp(incomingData.b, "on") == 0)) {
          khalaf = 'D';
          Serial.println(khalaf);
        }
    }
}

// Define the Wi-Fi credentials
const char* ssid = "subhi";
const char* password = "123456789";

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <style>

    .button1 {
      display: grid;
    /grid-template-columns: repeat(3, 1fr); ! Adjust to three buttons per row !/
      gap: 10px;
      margin-top: 10px;
      color: black;
      gap: 20px;
    }

    .button2 {

      background-color: white;

      padding: 3px;
      font-size: 15px;
      text-align: center;
      border-radius: 8px; /* Gives the button rounded corners but not fully circular */
      border: 1.5px  ; /* Adds a white border with a thickness of 2 pixels */
      cursor: pointer; /* Changes the cursor to a pointer on hover */
    }


    .arrows {
      font-size:40px;
      color: #b29c9c;
    }
    .circularArrows {
      font-size:50px;
      color:blue;
    }
    td.button {
      background-color:black;
      border-radius:25%;
      box-shadow: 5px 5px #888888;
    }
    td.button:active {
      transform: translate(5px,5px);
      box-shadow: none;
    }

    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
      -webkit-user-select: none; /* Safari */
      -khtml-user-select: none; /* Konqueror HTML */
      -moz-user-select: none; /* Firefox */
      -ms-user-select: none; /* Internet Explorer/Edge */
      user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }

    .slidecontainer {
      width: 100%;
    }

    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 15px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider:hover {
      opacity: 1;
    }

    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }
    .title{
      color: #cccccc;
    }
  </style>

</head>
<body class="noselect" align="center" style="background-color:black">

<div class="title">
  <h1>AgriBotix Control </h1>
</div>
<!--h2 style="color: teal;text-align:center;">Wi-Fi Camera &#128663; Control</h2-->

<table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
  <tr>
    <img id="cameraImage" src="http://192.168.4.2:81/stream" style="width:400px;height:250px ;  border: 2px solid #fff; /* Optional: add a border for visual appeal */
"></td>
  </tr>
  <tr>
    <td></td>
    <td class="button" ontouchstart='sendButtonInput("Forward","1")'><span class="arrows" >&#8679;</span></td>
  </tr>
  <tr>
    <td class="button" ontouchstart='sendButtonInput("LeftTurn","2")' ><span class="arrows" >&#8678;</span></td>
    <td></td>
    <td class="button" ontouchstart='sendButtonInput("RightTurn","3")'><span class="arrows" >&#8680;</span></td>
  </tr>
  <tr>
    <td></td>
    <td class="button" ontouchstart='sendButtonInput("Backward","4")' ><span class="arrows" >&#8681;</span></td>
  </tr>
  <tr>
    <td class="button2" ontouchstart='sendButtonInput("WaterOn","5")' ><span class="button1" >Water on</span></td>
    <td class="button2" ontouchstart='sendButtonInput("WaterOff","6")'><span class="button1" >Water off</span></td>
    <td class="button2" ontouchstart='sendButtonInput("InsticideMode","7")'><span class="button1" >Insecticide mode</span></td>

  </tr>
  <tr>
    <td class="button2" ontouchstart='sendButtonInput("SetRight","8")' ><span class="button1" >Set right turn</span></td>
    <td class="button2" ontouchstart='sendButtonInput("SetLeft","9")'><span class="button1" >Set left turn</span></td>
    <td class="button2" ontouchstart='sendButtonInput("SetFOn","10")'> <span class="button1" >Set fluid on</span></td>

  </tr>

  <tr>

    <td class="button2"  ontouchstart='sendButtonInput("SetFOff","11")'><span class="button1" >Set fluid off</span></td>
    <td class="button2" ontouchstart='sendButtonInput("InsticideOn","12")' ><span class="button1" >Insecticide on</span></td>
    <td class="button2"  ontouchstart='sendButtonInput("InsticideOFF","13")'><span class="button1" >Insticide OFF</span></td>

  </tr>

  <tr>

    <td class="button2"  ontouchstart='sendButtonInput("Set BASE","14")'><span class="button1" >Set BASE</span></td>
    <td></td>
    <td class="button2"  ontouchstart='sendButtonInput("Manual","15")'><span class="button1" >Manual</span></td>

  </tr>
</table>

<script>
  var webSocketCameraUrl = "ws:\/\/" + window.location.hostname + "/Camera";
  var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";
  var websocketCamera;
  var websocketCarInput;

  function initCameraWebSocket()
  {
    websocketCamera = new WebSocket(webSocketCameraUrl);
    websocketCamera.binaryType = 'blob';
    websocketCamera.onopen    = function(event){};
    websocketCamera.onclose   = function(event){setTimeout(initCameraWebSocket, 2000);};
    websocketCamera.onmessage = function(event)
    {
      var imageId = document.getElementById("cameraImage");
      imageId.src = URL.createObjectURL(event.data);
    };
  }

  function initCarInputWebSocket()
  {
    websocketCarInput = new WebSocket(webSocketCarInputUrl);
    websocketCarInput.onopen    = function(event)
    {
      var speedButton = document.getElementById("Speed");
      sendButtonInput("Speed", speedButton.value);
      var lightButton = document.getElementById("Light");
      sendButtonInput("Light", lightButton.value);
    };
    websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
    websocketCarInput.onmessage = function(event){};
  }

  function initWebSocket()
  {
    initCameraWebSocket ();
    initCarInputWebSocket();
  }

  function sendButtonInput(key, value)
  {
    var data = key + "," + value;
    websocketCarInput.send(data);
  }

  window.onload = initWebSocket;
  document.getElementById("mainTable").addEventListener("touchend", function(event){
    event.preventDefault()
  });
</script>
</body>
</html>
)HTMLHOMEPAGE";

void sendCarCommands(std::string inputCommand) {
    Serial.println(inputCommand.c_str());
}

void handleRoot(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server, 
                              AsyncWebSocketClient *client, 
                              AwsEventType type,
                              void *arg, 
                              uint8_t *data, 
                              size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            // Handle WebSocket connection
            break;
        case WS_EVT_DISCONNECT:
            // Handle WebSocket disconnection
            break;
        case WS_EVT_DATA:
            AwsFrameInfo *info;
            info = (AwsFrameInfo*)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                char command;
                std::string myData = "";
                myData.assign((char *)data, len);
                std::istringstream ss(myData);
                std::string key, value;
                std::getline(ss, key, ',');
                std::getline(ss, value, ',');
                int valueInt = atoi(value.c_str());
                
                if (key == "WaterOn") {
                    command = 'C';
                    Serial.println(command);
                } else if (key == "WaterOff") {
                    command = 'P';
                    Serial.println(command);
                } else if (key == "SetBASE") {
                    strcpy(outgoingData.a, "direction");
                    strcpy(outgoingData.b, "base");
                    if(tupleCount < MAX_TUPLES){
                      directionTuples[tupleCount] = outgoingData;
                      tupleCount++;
                    }  
                } else if (key == "SetRight") {
                    strcpy(outgoingData.a, "direction");
                    strcpy(outgoingData.b, "right");
                    if(tupleCount < MAX_TUPLES){
                      directionTuples[tupleCount] = outgoingData;
                      tupleCount++;
                    }  
                } else if (key == "SetLeft") {
                    strcpy(outgoingData.a, "direction");
                    strcpy(outgoingData.b, "left");
                    if(tupleCount < MAX_TUPLES){
                      directionTuples[tupleCount] = outgoingData;
                      tupleCount++;
                    }
                } else if (key == "SetFOn") {
                    strcpy(outgoingData.a, "direction");
                    strcpy(outgoingData.b, "fluidon");
                    if(tupleCount < MAX_TUPLES){
                      directionTuples[tupleCount] = outgoingData;
                      tupleCount++;
                    }
                } else if (key == "SetFOff") {
                    strcpy(outgoingData.a, "direction");
                    strcpy(outgoingData.b, "fluidoff");
                    if(tupleCount < MAX_TUPLES){
                      directionTuples[tupleCount] = outgoingData;
                      tupleCount++;
                    }
                } else if (key == "Manual") {
                    command = 'M';
                    Serial.println(command);
                } else if (key == "InsticideMode") {
                    command = 'I';
                    Serial.println(command);
                } else if (key == "InsticideOn") {
                    command = 'N';
                    Serial.println(command);
                } else if (key == "InsticideOFF") {
                    command = 'X';
                    Serial.println(command);
                } else if (key == "Forward") {
                    command = 'F';
                    Serial.println(command);
                } else if (key == "Backward") {
                    command = 'A';
                    Serial.println(command);
                } else if (key == "RightTurn") {
                    command = 'R';
                    Serial.println(command);
                } else if (key == "LeftTurn") {
                    command = 'L';
                    Serial.println(command);
                }
            }
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
        default:
            break;  
    }
}


void setup() {
    WiFi.mode(WIFI_AP_STA);  // Set both AP and STA mode

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    Serial.begin(115200);
    Serial2.begin(115200);
    // Register send and receive callbacks
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.println(IP);

    server.on("/", HTTP_GET, handleRoot);
    server.onNotFound(handleNotFound);

    wsCarInput.onEvent(onCarInputWebSocketEvent);
    server.addHandler(&wsCarInput);

    server.begin();

    // Register peer
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
  wsCarInput.cleanupClients();
  if (Serial2.available() > 0) {
    String lat_lng = Serial.readStringUntil('\n');
    lat_lng.trim();
    int commaIndex = lat_lng.indexOf(',');
    if (commaIndex > 0) {
        String latstr = lat_lng.substring(0, commaIndex);
        String lngstr = lat_lng.substring(commaIndex + 1);
        float latitude = latstr.toFloat();
        float longitude = lngstr.toFloat();
        outgoingData.lat = latitude;
        outgoingData.lng = longitude;
        for(int i = 0; tupleCount<i;i++){
        if(directionTuples[i].lat == latitude && directionTuples[i].lng == longitude){
          char khalaf;
          if((strcmp(directionTuples[i].b,"on")==0)){
            khalaf = 'D';
            Serial.println(khalaf);
          } else if((strcmp(directionTuples[i].b,"left")==0)){
            khalaf = 'L';
            Serial.println(khalaf);
          } else if((strcmp(directionTuples[i].b,"right")==0)){
            khalaf = 'R';
            Serial.println(khalaf);
          } else if((strcmp(directionTuples[i].b,"fluidoff")==0)){
            khalaf = 'E';
            Serial.println(khalaf);
          } else if((strcmp(directionTuples[i].b,"fluidon")==0)){
            khalaf = 'Q';
            Serial.println(khalaf);
          } else if((strcmp(directionTuples[i].b,"base")==0)){
            khalaf = 'B';
            Serial.println(khalaf);
          }
            
        }
      }
    }
  }
  strcpy(outgoingData.a , "humidity");
  outgoingData.lng = 15.333256;
  outgoingData.lat = 15.235689;
  strcpy(outgoingData.b , "fathi");
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));
  if(result != ESP_OK){
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));
  }else{
    
  }
  delay(1000);
}
