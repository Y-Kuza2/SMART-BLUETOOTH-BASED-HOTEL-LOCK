#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>

const char* ssid = "Prathmesh's";    
const char* password = "hello0112"; 


// RX2 = GPIO 16, TX2 = GPIO 17
HardwareSerial STM32Serial(2); 


// Web Server on port 80
WebServer server(80);

// HTML PAGE
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Smart Hotel Lock</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background-color: #f2f2f2; margin: 0; padding: 20px; }
    h1 { color: #333; }
    .card { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); max-width: 400px; margin: auto; }
    button { width: 100%; padding: 15px; font-size: 18px; margin: 10px 0; border: none; border-radius: 5px; cursor: pointer; color: white; }
    .btn-enable { background-color: #4CAF50; }
    .btn-disable { background-color: #f44336; }
    .btn-name { background-color: #2196F3; }
    input { width: calc(100% - 22px); padding: 10px; font-size: 16px; margin-bottom: 10px; border: 1px solid #ddd; border-radius: 5px; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Smart Lock Admin APP</h1>
    <hr><p>Project by Prathmesh, Niket, Kshitij, Priyanjol & Srishti</p><hr>
    
    <button class="btn-enable" onclick="fetch('/enable')">ENABLE BT KEY (Welcome)</button>
    <button class="btn-disable" onclick="fetch('/disable')">DISABLE BT KEY (Locked)</button>
    
    <hr>
    
    <h3>Guest Setup</h3>
    <input type="text" id="guestName" placeholder="Enter Guest Name">
    <button class="btn-name" onclick="sendName()">SET GUEST NAME</button>
  </div>

  <script>
    function sendName() {
      var name = document.getElementById('guestName').value;
      if(name) {
        fetch('/setname?name=' + encodeURIComponent(name));
        alert("Name sent: " + name);
      } else {
        alert("Please enter a name first.");
      }
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // connecting to STM32
  STM32Serial.begin(9600, SERIAL_8N1, 16, 17);

  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 

  
  
  // WEB SERVER HANDLERS
  
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });

  
  server.on("/enable", []() {
    STM32Serial.write('1');
    server.send(200, "text/plain", "OK");
    Serial.println("Web Request: ENABLE");
  });

  
  server.on("/disable", []() {
    STM32Serial.write('0');
    server.send(200, "text/plain", "OK");
    Serial.println("Web Request: DISABLE");
  });

  
  server.on("/setname", []() {
    if (server.hasArg("name")) {
      String name = server.arg("name");
      
      // Send Protocol to STM32
      STM32Serial.write('$');   
      STM32Serial.print(name);  
      STM32Serial.write('\n');  
      
      server.send(200, "text/plain", "OK");
      Serial.print("Web Request: Set Name -> ");
      Serial.println(name);
    }
  });

  server.begin();
}

void loop() {
  server.handleClient(); 
}