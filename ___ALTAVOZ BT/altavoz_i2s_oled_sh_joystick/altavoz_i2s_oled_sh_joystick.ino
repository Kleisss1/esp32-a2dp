#include "BluetoothA2DPSink.h" // Incluimos todas las librerias necesarias. 
#include <SPI.h>               // Usamos 4-Wire SPI porque la SH no tiene I2C.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>


#define OLED_MOSI     23 // (SDA (D1)) va al GPIO MOSI
#define OLED_CLK      18 // SCK GPIO
#define OLED_RST      17 // TX2 GPIO
#define OLED_DC       16 // RX2 GPIO
#define OLED_CS       5  // SS  GPIO


/*
Conexiones joystick - Arduino UNO:
GND - GND
Vcc - 5v
VRx - A0: Canal 0 del ADC // ADC1_6
VRy - A1: Canal 1 del ADC // ADC1_7
SW  - D9                  // TX0
*/

#define JX     35 //vrx
#define JY     34 //vry
#define JB     21 //boton

#define LEDB   19 //led emparejamiento
#define LEDG   21


bool isPlaying = true; // para control de canción pause/stop.

String songTitle;
String artistName;
String albumName;

/*
  Pantalla --> 0x3F  
*/

// Inicia la pantalla

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// Función para obtener metadata. Se puede ampliar

 void avrc_metadata_callback(uint8_t id, const uint8_t *text) {
    switch (id) {
        case ESP_AVRC_MD_ATTR_TITLE:
           songTitle = (char*) text;
            break;
        case ESP_AVRC_MD_ATTR_ARTIST:
            artistName = (char*) text;
            break;
        case ESP_AVRC_MD_ATTR_ALBUM:
            albumName = (char*) text;
            break;    
    } 
    /*Serial.println( songTitle);
    Serial.println(artistName);*/
}


BluetoothA2DPSink a2dp_sink;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  

  pinMode(JB, INPUT_PULLUP);  //activar resistencia pull up  
  pinMode(JX, INPUT_PULLUP);
  pinMode(JY, INPUT_PULLUP);

  Serial.begin(9600);


  // Inicia OLED

  display.begin(0,true); // reset sin I2C

  // Splashcreen Adafruit por estética

  display.display(); delay(2000); display.clearDisplay();

  // configuramos i2s del dac

    i2s_pin_config_t my_pin_config = {
        .bck_io_num = 27, //// BCLK 
        .ws_io_num = 26,  //// LRC pin 
        .data_out_num = 25, ///Din pin
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    a2dp_sink.set_pin_config(my_pin_config);
    a2dp_sink.start("ESP32a2dp_guts");
    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
    a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM);
  

    static const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = 44100, // corrected by info from bluetooth
        .bits_per_sample = (i2s_bits_per_sample_t) 16, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    //a2dp_sink.set_i2s_config(i2s_config);

  // Bienvenida a BT.
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("Iniciando ESP32...");
    display.println("Iniciando altavoz BT...");
    display.display(); 
    delay(2500); 
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Dispositivo BT activado");
    display.setCursor(0,24);
    display.println("Listo para emparejar."); 
    display.display();

    while (a2dp_sink.is_connected() == false){  // parpadeo emparejamiento

      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);

    }
     
}

void loop() {

// Check conectividad a BT.

    if (a2dp_sink.is_connected() == true) {

      display.clearDisplay();
      display.println("BT emparejado...");
      digitalWrite(LED_BUILTIN, a2dp_sink.is_connected());
      delay(2500); 
      display.clearDisplay(); 
      display.setTextSize(1);
      

      display.clearDisplay();
      display.setCursor(0,0); // Esquina superior izquierda
      display.println("Bluetooth conectado.");
      display.setCursor(0,20);             
      display.print("Cancion: ");
      display.println(songTitle);
      display.print("Artista: ");
      display.println(artistName);
      display.print("Album: ");
      display.println(albumName);
      display.display();

    // defino valores iniciales joystick

     int pos_x = 0;
     int pos_y = 0;
     bool boton = false;
     
     // Lectura del joystick
     pos_x = analogRead(JX);
     delay(20);            // pausa entre lecturas analógicas (necesaria)
     pos_y = analogRead(JY);
     boton = digitalRead(JB);
	   //digitalWrite(LED, JB); // controla el led con el estado del botón

     if (pos_x > 3000){

        a2dp_sink.next();
        delay(500);
        //break;
        
     }
     if (pos_x < 2500) {

        a2dp_sink.previous();
       delay(500);
       //break;

     }

     if (boton == LOW) {

        if (isPlaying == true){
        a2dp_sink.pause();
      
      }

      else{

       a2dp_sink.play();

      }
      
     }

    }


}