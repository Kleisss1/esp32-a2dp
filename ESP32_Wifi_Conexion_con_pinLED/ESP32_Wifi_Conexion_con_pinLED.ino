// ESP32 funciona como estación cualquiera y se conecta a un servidor WIFI.

#include <WiFi.h> 

const char* ssid = "";
const char* password = "";
WiFiServer server(80);
int LEDPIN = 2; // LED interno se encuentra en PIN 2 (9 del chip)

void setup(){
    Serial.begin(115200); // velocidad de bus (monitor_speed)
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network. Check LED for connection.");

    /// Confirmación conexión a WiFi con el LED.
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, HIGH);
    ///

    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP()); // IP Local del ESP32 necesitada para operar.
    server.begin(); // Inicia servidor.


    delay(5000); 
    digitalWrite(LEDPIN, LOW);

} 


void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Pulsa <a href=\"/H\">aqui</a> para encender el LED interno.<br>");
            client.print("Pulsa <a href=\"/L\">aqui</a> para apagar el LED interno.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LEDPIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LEDPIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
 

