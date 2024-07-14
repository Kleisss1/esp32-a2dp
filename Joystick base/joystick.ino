/* Test de joystick
Conexiones joystick - Arduino UNO:
GND - GND
Vcc - 5v
VRx - A0: Canal 0 del ADC // ADC1_6
VRy - A1: Canal 1 del ADC // ADC1_7
SW  - D9                  // TX2
*/

#define LED    2
#define JX     35
#define JY     34
#define JB     17

void setup() 
{
     pinMode(JB, INPUT_PULLUP);  //activar resistencia pull up  
	 pinMode(LED, OUTPUT); // LED
     
	 Serial.begin(9600);
}

void loop() 
{
     int pos_X = 0;
     int pos_Y = 0;
     bool boton = false;
     
     // Lectura del joystick
     pos_X = analogRead(JX);
     delay(20);            // pausa entre lecturas analógicas (necesaria)
     pos_Y = analogRead(JY);
     boton = digitalRead(JB);
	   //digitalWrite(LED, JB); // controla el led con el estado del botón

     if (boton == HIGH) {
    digitalWrite(LED, HIGH);
      }
     else {
    digitalWrite(LED, LOW);
    }
        
	 
     
     // Mostrar valores por consola
     Serial.print("X, Y; botón= " );
     Serial.print(pos_X);
     Serial.print(", ");
     Serial.print(pos_Y);
     Serial.print("; ");
     Serial.println(boton);
     delay(500);
}