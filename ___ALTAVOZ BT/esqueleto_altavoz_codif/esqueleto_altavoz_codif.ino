#include "BluetoothA2DPSink.h" // Incluimos todas las librerias necesarias. 
#include <SPI.h>               // * NO NECESARIA SI USAS I2C. En teoria.
#include <Wire.h>
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

#define ROTARY_ENCODER_A_PIN 14 // Pin CLK.
                                // HSPI CLK GPIO si quieres evitar conflicto con OLED SPI * NOTA: mejor fijarse en el pinout del esp.
#define ROTARY_ENCODER_B_PIN 22 // Pin DATA (DT).
#define ROTARY_ENCODER_BUTTON_PIN 21 // Pin SW para boton.
#define ROTARY_ENCODER_VCC_PIN -1 // Pin alimentación. -1 si te lo llevas a Vin de una placa como el ESP.
#define ROTARY_ENCODER_STEPS 4 // saltos encoder.

// En vez de cambiarlos aquí cambialos arriba.
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

void rotary_onButtonClick()
{
    static unsigned long lastTimePressed = 0; // Debouncing suave para que la señal alcance el valor deseado.
    if (millis() - lastTimePressed < 500)
    {
            return;
    }
    lastTimePressed = millis();
    Serial.print("button pressed "); 
    Serial.print(millis());
    Serial.println(" milliseconds after restart");
}

void rotary_loop()
{
    // no printear nada si no cambia el valor leido.
    if (rotaryEncoder.encoderChanged())
    {
            Serial.print("Value: ");
            Serial.println(rotaryEncoder.readEncoder());
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {
            rotary_onButtonClick();
    }
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

BluetoothA2DPSink a2dp_sink;


void setup()
{
    Serial.begin(115200);

    //we must initialize rotary encoder
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    //set boundaries and if values should cycle or not
    //in this example we will set possible values between 0 and 1000;
    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, 1000, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

    /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
    //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

    // configuramos i2s del dac

    i2s_pin_config_t my_pin_config = {
        .bck_io_num = 27, //// BCLK 
        .ws_io_num = 26,  //// LRC pin 
        .data_out_num = 25, ///Din pin
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    a2dp_sink.set_pin_config(my_pin_config);
    a2dp_sink.start("ESP32a2dp_guts");
    //a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
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

}

void loop()
{
    //in loop call your custom function which will process rotary encoder values
    rotary_loop();
          a2dp_sink.set_volume(rotaryEncoder.readEncoder());

    delay(50); //or do whatever you need to do...
}