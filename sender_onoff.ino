
#include <esp_now.h>
#include <WiFi.h>
#define BUTTON_PIN 9



// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {"34:85:18:03:D2:F8"};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    int x;
    int y;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
volatile int angle = 0; // use volatile for variables that can be changed by an interrupt
int deg = 90;
void IRAM_ATTR button_isr() {
  if (angle == deg) {
    angle = 0;
  } else {
    angle = deg;
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_isr, FALLING);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }



  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
}

void loop() {
// print out the angle and button state
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(", Button: ");
  Serial.println(digitalRead(BUTTON_PIN));

  delay(100); // add a delay to prevent spamming the serial port


  // Set values to send
  myData.id = 1;
  myData.x = angle;
  myData.y = random(0,50);

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   

}