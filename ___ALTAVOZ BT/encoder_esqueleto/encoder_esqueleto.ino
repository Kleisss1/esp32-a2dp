/* Esqueleto encoder para ESP32. Muchas cosas en ingles ya que son explicaciones intuitivas directas del github.
https://github.com/igorantolic/ai-esp32-rotary-encoder/blob/master/examples/Esp32RotaryEncoderBasics/Esp32RotaryEncoderBasics.ino
*/ 

#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

#define ROTARY_ENCODER_A_PIN 14 // Pin CLK.
                                // * NOTA: fijarse en el pinout del esp. 14 es el puerto SPI, 23 el I2C.
#define ROTARY_ENCODER_B_PIN 22 // Pin DATA (DT).
#define ROTARY_ENCODER_BUTTON_PIN 21 // Pin SW para boton.
#define ROTARY_ENCODER_VCC_PIN -1 // Pin alimentación. -1 si te lo llevas a Vin de una placa como el ESP. 
                                  // Usualmente son 5 voltios... 3.3 puede dar errores. 
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
            Serial.println(rotaryEncoder.readEncoder()); // recomendable guardar rotaryEncoder.readEncoder() en una variable int.
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

void setup()
{
    Serial.begin(115200);

    // startup encoder
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    //set boundaries and if values should cycle or not
    //in this example we will set possible values between 0 and 1000.
    //puedes limitar los valores de esta forma o utilizar un ciclo while o if. a veces, es deseable.
    
    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, 1000, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
                                                        //circlevalues -> si lectura 1000 y subes a 1001 automaticamente pasar a 0. loop.
    
    /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
    //rotaryEncoder.disableAcceleration(); // descomentar si quieres desactivarla.
    rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

    // esta es la aceleración. básicamente a cuanto más rapido gires, más rapida sera la lectura.
    // cuanto menor sea setAcceleration(), más lento llegarás a tu set point (por ej 100)

}

void loop()
{
    // aquí haces lo que quieras con las lecturas del encoder.
    rotary_loop();
    delay(50); //
}