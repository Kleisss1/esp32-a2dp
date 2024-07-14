/* LCD_I2C demo
 *  - Place cursor
 *  - Custom characters
 *  - Custom colors (for GROVE LCD RGB)  
 *  For Grove LCD RGB define GROVE_LCDRGB
 *  lmnieto2@ujaen.es, 2019
*/
#define GROVE_LCDRGB

#include <Wire.h>   // https://www.arduino.cc/en/Reference/Wire
#ifndef GROVE_LCDRGB
  #include <LiquidCrystal_I2C.h>  //https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
#else
  #include <rgb_lcd.h> // https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight/archive/master.zip
// See library function list at: https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight/blob/master/rgb_lcd.h
#endif

#ifndef GROVE_LCDRGB
  LiquidCrystal_I2C lcd(0x3F, 16, 2);  // I2C adress (check!!!), cols, rows
#else
  rgb_lcd lcd; // I2C address not needed for GROVE LCD RGB
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 255;
#endif

uint8_t col = 0; // column

void setup()
{
#ifndef GROVE_LCDRGB
  lcd.init(); // Init LCD
  lcd.backlight(); // switch-on backlight
#else
  lcd.begin(16, 2);
  lcd.setRGB(red, green, blue);
#endif

  // Create custom characters
  uint8_t pacman1[8]= 
  {
      0b00000,
      0b01110,
      0b11011,
      0b11111,
      0b11111,
      0b01110,
      0b00000,
      0b00000
  };
  uint8_t pacman2[8]= 
  {
      0b00000,
      0b01110,
      0b10100,
      0b11000,
      0b11100,
      0b01110,
      0b00000,
      0b00000
  };
  lcd.createChar(1, pacman1);
  lcd.createChar(2, pacman2);
  
  lcd.home();
  
  // Two lines message
  lcd.print("Hello...");
  lcd.setCursor(5, 1); // 5,1: 6th colum, 2nd row
  lcd.print("PACMAN!");

  delay(2000);
}

void loop()
{
  // Write character
  lcd.setCursor(col, 1); // second row
  if (col & 1) // odd column?
  {
    lcd.write((unsigned char)1);
  }
  else // even column
  {
    lcd.write((unsigned char)2);
  }
  
  delay(200);

  // Clear character
  lcd.setCursor(col, 1);
  lcd.print(" ");
  
  // Update column and scroll
  col = (col + 1) % 16;

#ifdef GROVE_LCDRGB  
  // Update backlight color
  red += 5;
  blue -= 5;
  green += 10;
  lcd.setRGB(red, green, blue); 
#endif
}
