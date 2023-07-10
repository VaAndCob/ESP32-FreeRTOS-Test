#include <TFT_eSPI.h>
#include "image.h"
#include "music.h"
#include "nes_audio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_task_wdt.h>
//Pin configuration
#define BUZZER_PIN 26//speaker
#define SELECTOR_PIN 27//push button
// setting PWM properties
#define backlightChannel 0
#define buzzerChannel 2
#define array_length(x) (sizeof(x) / sizeof(x[0])) //macro to calculate array length
Cartridge player(BUZZER_PIN);//not specified 4 pin , use 1 pin at buzzer pin
bool press = false;

//DISPLAY
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite txt = TFT_eSprite(&tft);

#define  maxLine 12

//--- music task
TaskHandle_t TaskHandle0 = NULL;
uint8_t za, zb, zc, zx;
const String scrollText[maxLine] = {
    "[ Real Time OS + Sprite Testing ]",
    "Music runs on CORE-0 / Graphic runs on CORE-1",
    "SPRITE: Text scrolling / X-Wing Fighter",
    "=============================================",
    "A long time ago in a galaxy far, far away...",
    "The dead speak! The galaxy has heard a mysterious",
    "broadcast, a threat of REVENGE, in the",
    "sinister voice of the late EMPEROR PALPATINE.",
    "GENERAL LEIA ORGANA dispatches secret agents",
    "to gather intelligence, while REY, the last",
    "hope of the Jedi, trains for battle against",
    "the diabolical FIRST ORDER."

};


uint8_t rng() {
  zx++;
  za = (za^zc^zx);
  zb = (zb+za);
  zc = ((zc+(zb>>1))^za);
  return zc;
}
//----------------------------
void TaskPlayMusic(void *pvParameters) {
  esp_task_wdt_init(63, false);//63 sec wd timer / disable wd
  for(;;) {
 // player.frame_counter_cb(danceLight);//cal back fucntion (not use)
  player.play_nes(starwars , true, 0.5); //play nes music, loop, volume 0.5 
  }
}
//----------------------------
void StarField() {//start field effect
//#define roll
  img.createSprite(124,59);
  txt.createSprite(320,120);
#ifdef roll
#else  
  img.setSwapBytes(true);
  txt.setSwapBytes(true);
#endif  
  img.pushImage(2,2,120,55,xwing);//xwing spirte
  uint8_t curx = 98;
  uint8_t cury = 145;
  int scrolly = 80;
// With 1024 stars the update rate is ~65 frames per second
  #define NSTARS 512//no of stars
  uint8_t sx[NSTARS] = {};
  uint8_t sy[NSTARS] = {};
  uint8_t sz[NSTARS] = {};
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);
  tft.fillScreen(TFT_BLACK);//clear screen
  while (press) {//exit if press button
  uint8_t spawnDepthVariation = 127;//255
  for(int i = 0; i < NSTARS; ++i)  {
    if (sz[i] <= 1) {
      sx[i] = 160 - 120 + rng();
      sy[i] = rng();
      sz[i] = spawnDepthVariation--;
    } else {   
      int old_screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
      int old_screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;
      // This is a faster pixel drawing function for occassions where many single pixels must be drawn
      tft.drawPixel(old_screen_x, old_screen_y,TFT_BLACK); 
      sz[i] -= 2;
      if (sz[i] > 1) {
        int screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
        int screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;
        if (screen_x >= 0 && screen_y >= 0 && screen_x < 320 && screen_y < 240) {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          tft.drawPixel(screen_x, screen_y, tft.color565(r,g,b));
        } else sz[i] = 0; // Out of screen, die.
      }
    }
  }


  //draw x-wing
  int8_t rolls = random(-1,2);
  curx = curx + rolls;//114
  if (curx < 20) curx = 20; //limit left
  if (curx > 178) curx = 178;//limit right
  cury = cury + random(-1,2);
  if (cury < 130) cury = 130;//limit upper
  if (cury > 165) cury = 165;//limit lower
#ifdef roll  
  tft.setPivot(curx+62,cury+29);//set rotation origin
  img.pushRotated(rolls);//show sprite with rotation angle
#else  
  img.pushSprite(curx,cury);//show sprite

#endif  


  //draw text on screen
  tft.drawFastHLine(0,219,320,TFT_GREEN);
  tft.setTextColor(random(0xffff));//draw next text
  tft.drawString("RTOS TEST",0,221,4);
  tft.setTextColor(TFT_WHITE,TFT_RED);
  tft.drawRightString("[- Press button to pause -]",319,225,2);

  //draw scroll text on sprite
  txt.setTextColor(TFT_BLACK);//delete old text
  for (int i = 0;i<maxLine;i++) 
      if (scrolly+i*15 > 0) {
    txt.drawCentreString(scrollText[i],159,scrolly+i*15,2); 
    }

  scrolly--;
  if (scrolly < -200) scrolly = 120;

  txt.setTextColor(TFT_ORANGE);//draw next text
  for (int i = 0;i<maxLine;i++) {
    if (scrolly+i*15 > 0) {
    txt.drawCentreString(scrollText[i],159,scrolly+i*15,2); 
   // Serial.println(scrolly+i*15);
    }

  }
  txt.pushSprite(0,0);//put sprite 

  
  if (digitalRead(SELECTOR_PIN) == LOW) {
    press = false;
  } 
 }
  img.deleteSprite();
  txt.deleteSprite();
}//task star field
//----------------------------



// the setup function runs once when you press reset or power the board
void setup() {
  //pin configuration
  pinMode(BUZZER_PIN,OUTPUT);//speaker
  pinMode(SELECTOR_PIN,INPUT_PULLUP);//button

  //pwm setup
  ledcSetup(buzzerChannel, 1500, 10);//buzzer 10 bit
  ledcSetup(backlightChannel, 12000, 8);//backlight 8 bit
  ledcAttachPin(BUZZER_PIN, buzzerChannel);//attach buzzer

  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.drawCentreString("Push button to start",159,120,4);

  ledcWriteTone(buzzerChannel,0);
  Serial.println(array_length(starwars));//total 5 type of display page)

}
//----------------------------
void loop()
{

  if (digitalRead(SELECTOR_PIN) == LOW) {//button pressed
    press = true;
 
    if (press) {  
      delay(500);
      Serial.println("PLAY");
      xTaskCreatePinnedToCore(//start play animation
      &TaskPlayMusic
      , "play music in core 0"   // A name just for humans
      , 4096// This stack size can be checked & adjusted by reading the Stack Highwater
      , NULL// no variable passting to task
      , 2 //Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      , &TaskHandle0
      , 0);//core0      
      StarField();//play star field animation  
      tft.setTextColor(TFT_BLACK,TFT_VIOLET);
      tft.drawCentreString("PAUSE",159,110,4);    
      Serial.println("PAUSE");
      vTaskDelete(TaskHandle0);//stop starfield and text animation
      pinMode(BUZZER_PIN,OUTPUT);//set pin mode again unless speaker stop working
      ledcAttachPin(BUZZER_PIN, buzzerChannel);//reattach buzzer pin
    }
       
    
   delay(500);

}
 yield();
}
  


