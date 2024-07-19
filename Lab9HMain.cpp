// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Jimmy & Tamim
// Last Modified: 4/16/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "Player.h"
#include "images/images.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

//random generator functions
uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

//initialize slidepot and players
SlidePot Sensor(1500,0); // copy calibration from Lab 7
Player Arthur(90, 130, Cowboy1, true);
Player John(30, 10, Cowboy2, false);
Language_t myLanguage=English;
int bgColor = 0xCC66;

int powerupRate = 842;
//enums for game state names
typedef enum {
    StartMenu,
    InGame,
    GameOver,
    NumStates
} GameState;

// define state struct for FSM
typedef struct State State_t;
struct State {
    GameState state;
    uint32_t time;
    State_t* next[NumStates];
};

// define struct ptrs
#define startMenu &FSM[0]
#define inGame &FSM[1]
#define gameOver &FSM[2]

// initialize game FSM with 3 states
State_t FSM[NumStates] = {
    {StartMenu, 300, {inGame, inGame, inGame}},
    {InGame, 50, {gameOver, gameOver, gameOver}},
    {GameOver, 150, {startMenu, startMenu, startMenu}}
};

//initialize first state
State_t* Pt = startMenu; // state ptr
//initialize powerup queue
PowerupQueue PWs;

//launchpad leds
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}



// use main1 to observe special characters
int main1(void){ // main1
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();

  ST7735_FillScreen(ST7735_WHITE);            // set screen to black
  ST7735_SetTextColor(ST7735_BLACK);
  while(1){
      // State 1
      ST7735_SetCursor(0, 1);
      OutPhrase(HELLO_E);
      OutPhrase(HELLO_S);
      OutPhrase(SW_E);
      OutPhrase(SW_S);
    do{
        if(Switch_In() == 1){
            myLanguage=English;
            Pt = Pt->next[0];
        }
        if(Switch_In() == 2){
            myLanguage=Spanish;
            Pt = Pt->next[0];
        }
    } while(Pt->state == 0);
    Clock_Delay1ms(500);
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for


  ST7735_FillScreen(0xCC66);

    ST7735_DrawBitmap(90, 10, Cowboy2, 25, 28); // player ship bottom
    ST7735_DrawBitmap(102, 34, Bullet1, 8, 7);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED

  Sensor.Init(ADC0, 0, 6, ADCVREF_VDDA);
  ST7735_InitPrintf();
  int32_t z1 = 0, z2 = 0;
  ST7735_FillScreen(ST7735_BLACK);
  uint32_t x1 = 0, y1 = 30, x2 = 9, y2 = 20;
  uint32_t w1 = 18, h1 = 8, w2 = 16, h2 = 10;
  //ST7735_DrawBitmap(x1, y1, PlayerShip0, w1, h1); // player ship bottom
  //ST7735_DrawBitmap(x2, y2, SmallEnemy10pointA, w2, h2);
  while(1){
    // write code to test switches and LEDs

//   Sensor.In(ADC0, &z2, &z1);
   if(z1<1900){
       // top left x y
       ST7735_FillRect(x1, y1-8, w1, h1, ST7735_BLACK);
       y1-=3;
   } else if(z1>2200){
       ST7735_FillRect(x1, y1-8, w1, h1, ST7735_BLACK);
       y1+=3;
   }
   if(z2<1900){
       ST7735_FillRect(x2, y2-10, w2, h2, ST7735_BLACK);
       y2-=2;
   } else if(z2>2200){
       ST7735_FillRect(x2, y2-10, w2, h2, ST7735_BLACK);
       y2+=2;
   }
   //ST7735_DrawBitmap(x1, y1, PlayerShip0, w1, h1); // player ship bottom
   //ST7735_DrawBitmap(x2, y2, SmallEnemy10pointA, w2, h2);
   Clock_Delay1ms(50);

  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
      Sound_Shoot();
      Clock_Delay1ms(5000);
//      Sound_Killed();
      Clock_Delay1ms(5000);
//      Sound_Explosion();

  }
}

const unsigned short* getRandomSprite(int index) {
    const unsigned short* sprites[] = {deadeye, shield, rapidfire};
    return sprites[index];
}

uint8_t semaphore = 0;
uint32_t move2=0, move1=0;
// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
  GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
  // 1) sample slide pot
  Sensor.In(ADC0, &move2, &move1);

  // 3) move sprites
  Sensor.Move(&John, move1);
  Sensor.Move(&Arthur, move2);
  John.BorderCheck();
  Arthur.BorderCheck();

  // move bullets if b != nullptr && moved == true
  John.UpdateBullet();
  Arthur.UpdateBullet();

  // decrement bullet cooldown
  John.decrementCooldown();
  Arthur.decrementCooldown();

  // check if player is shooting and execute a shoot
  int Switch = Switch_In();
  //if John shoots, create new bullet for John, shoot sound, decrement ammo
  if(Switch == 1 && John.getAmmo() > 0 && John.canShoot()){
      Bullet* b2 = new Bullet(John.getX()+12, John.getY()+32);
      John.b = b2;
      Sound_Shoot();
      John.DecrementAmmo();
      John.setCooldown();
  }
  //if Arthur shoots, create new bullet for Arthur, shoot sound, decrement ammo
  if(Switch == 2 && Arthur.getAmmo() > 0 && Arthur.canShoot()){
      Bullet* b1 = new Bullet(Arthur.getX()+12, Arthur.getY()-16);
      Arthur.b = b1;
      Sound_Shoot();
      Arthur.DecrementAmmo();
      Arthur.setCooldown();
  }

  //if both shoots, create new bullet for both, shoot sound, decrement ammo for both
  if(Switch == 3 && Arthur.getAmmo() > 0 && Arthur.canShoot() && John.getAmmo() > 0 && John.canShoot()){
      Bullet* b1 = new Bullet(Arthur.getX()+12, Arthur.getY()-16);
      Arthur.b = b1;
      Bullet* b2 = new Bullet(John.getX()+12, John.getY()+32);
      John.b = b2;
      Sound_Shoot();
      Arthur.DecrementAmmo();
      Arthur.setCooldown();
      John.DecrementAmmo();
      John.setCooldown();
  }

  // check if bullets hit + set iframes
  if(Arthur.isHit(John.b)){
      Arthur.DecrementHealth();
      LED_On(2);
      Sound_Hit();
  }
  if(John.isHit(Arthur.b)){
      John.DecrementHealth();
      LED_On(1);
      Sound_Hit();
  }

  // decrement iframes
  John.DecrementIV();
  Arthur.DecrementIV();

  //turn of leds once hit is indicated
  if(John.isIV() == false){
      LED_Off(1);
  }
  if(Arthur.isIV() == false){
      LED_Off(2);
  }

  //check powerup queues for players
  PWs.check(&Arthur);
  PWs.check(&John);

  // deadeye powerup check and initialize
  if(Arthur.powerup[DeadEye] != 0){
      John.slowed = true;
      Arthur.powerup[DeadEye]--;
  }
  if(John.powerup[DeadEye] != 0){
      Arthur.slowed = true;;
      John.powerup[DeadEye]--;
  }

  //reset after deadeye finish
  if(Arthur.powerup[DeadEye] == 0){
      John.slowed = false;
      Arthur.DEindicator = false;
  }
  if(John.powerup[DeadEye] == 0){
      Arthur.slowed = false;
      John.DEindicator = false;
  }

  //shield powerup check and initialize
  if(Arthur.powerup[Shield] != 0){
      if(Arthur.powerup[Shield] == 120 || Arthur.powerup[Shield] == 1){
          Arthur.shield();
      }
      Arthur.powerup[Shield]--;
  }
  else{
      Arthur.SHindicator = false;
  }
  if(John.powerup[Shield] != 0){
      if(John.powerup[Shield] == 120 || John.powerup[Shield] == 1){
          John.shield();
      }
      John.powerup[Shield]--;
  }
  else{
      John.SHindicator = false;
  }

  // rapidfire powerup check and initialize
  if(Arthur.powerup[RapidFire] != 0){
      Arthur.rapid = true;
      Arthur.powerup[RapidFire]--;
  }
  if(John.powerup[RapidFire] != 0){
      John.rapid = true;
      John.powerup[RapidFire]--;
  }

  //rapidfire powerup reset
  if(Arthur.powerup[RapidFire] == 0){
      Arthur.rapid = false;
      Arthur.RFindicator = false;
  }
  if(John.powerup[RapidFire] == 0){
      John.rapid = false;
      John.RFindicator = false;
  }
    semaphore = 1;
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}


// initialize all data structures


// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(ADC0, 0, 6, ADCVREF_VDDA); // PB18 = ADC1 channel 5, slidepot (NO)
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TimerG12_IntArm(80000000/30,2);  // initialize interrupts on TimerG12 at 30 Hz
  while(1){
      // State 1: Main Menu
      if(Pt->state == StartMenu){
          myLanguage=English;
          ST7735_FillScreen(ST7735_DARKGREY);
          ST7735_DrawBitmap(30, 40, OpenScreen, 80, 80);
          Clock_Delay1ms(2500);
          ST7735_FillScreen(ST7735_WHITE);            // set screen to black
          ST7735_SetTextColor(ST7735_BLACK);
          Clock_Delay1ms(500);
          ST7735_SetCursor(0, 1);
          OutPhrase(HELLO_E);
          OutPhrase(HELLO_S);
          ST7735_OutChar(13);
          OutPhrase(SW_E);
          OutPhrase(SW_S);

      do{
          //wait for language select, then move to ingame state
          int SW = Switch_In();

          if(SW == 1){
              myLanguage=English;
              while(Switch_In() != 0){};
              powerupRate = 842;
              Pt = Pt->next[InGame];
          }
          if(SW == 2){
              myLanguage=Spanish;
              while(Switch_In() != 0){};
              powerupRate = 842;
              Pt = Pt->next[InGame];
          }
          if(SW == 3){
              ST7735_FillScreen(bgColor);
              OutPhrase(MAYHEM);
              powerupRate = 50;
              Clock_Delay1ms(1000);
              Pt = Pt->next[InGame];
          }
      } while(Pt->state == StartMenu);
      }

      // State 2: InGame
      if(Pt->state == InGame){
          ST7735_FillScreen(bgColor);
          Clock_Delay1ms(500);

          //reset default settings for players for each new game
          Arthur.restart();
          John.restart();

          //initial sprite draw
          John.Draw();
          Arthur.Draw();

          //ammo reload and health boost powerup declarations
          Powerup healthboost1(100, 17, heart, 10, 10);
          Powerup healthboost2(100, 140, heart, 10, 10);
          Powerup ammoboost1(100, 17, ammo, 10, 10);
          Powerup ammoboost2(100, 140, ammo, 10, 10);

          __enable_irq();

      do{
          //static ints to periodically display powerups
          static int pUP = 0; //for deadeye, rapid, and shield
          static int cycles = 0; //all ingame cycles
          cycles++; //increment cycles

          //time period for health boost periodic spawn
          if(cycles % 681 == 0){
              healthboost1.switchStatus(true);
              healthboost2.switchStatus(true);
          }

          //time period for ammo reload periodic spawn
          if(cycles % 301 == 0){
              ammoboost1.switchStatus(true);
              ammoboost2.switchStatus(true);
          }

          //other powerups periodic spawn, appear in middle of screen for players to shoot
          if(cycles % powerupRate == 0){
              if(pUP % 3 == 0){
                  Powerup* newPW = new Powerup(40, 75, deadeye, 15, 15, 0);
                  PWs.addPowerup(newPW);
              }
              if(pUP % 3 == 1){
                  Powerup* newPW = new Powerup(60, 75, shield, 15, 15, 1);
                  PWs.addPowerup(newPW);
              }
              if(pUP % 3 == 2){
                  Powerup* newPW = new Powerup(80, 75, rapidfire, 15, 15, 2);
                  PWs.addPowerup(newPW);
              }
              pUP ++;
          }
          // wait for semaphore
          while(semaphore==0){}
          // clear semaphore
          semaphore = 0;

          //draw player health and ammo amounts during the game
          ST7735_DrawBitmap(10, 40, heart, 10, 10);
          ST7735_DrawCharS(10, 51, John.getHealth() + 48, ST7735_BLACK, bgColor, 1);
          ST7735_DrawBitmap(10, 60, ammo, 10, 10);
          ST7735_DrawCharS(10, 71, John.getAmmo() + 48, ST7735_BLACK, bgColor, 1);
          ST7735_DrawBitmap(10, 90, heart, 10, 10);
          ST7735_DrawCharS(10, 101, Arthur.getHealth() + 48, ST7735_BLACK, bgColor, 1);
          ST7735_DrawBitmap(10, 110, ammo, 10, 10);
          ST7735_DrawCharS(10, 121, Arthur.getAmmo() + 48, ST7735_BLACK, bgColor, 1);

          //draw player names below player stats
          ST7735_DrawCharS(25, 45, 'J', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 50, 'o', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 55, 'h', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 60, 'n', ST7735_BLACK, bgColor, 1);

          ST7735_DrawCharS(25, 90, 'A', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 95, 'r', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 100, 't', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 105, 'h', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 110, 'u', ST7735_BLACK, bgColor, 1);
          ST7735_DrawCharS(25, 115, 'r', ST7735_BLACK, bgColor, 1);

          //check if either player is dead, if yes, move to endgame state
          if((John.isDead()) || (Arthur.isDead())){
              Clock_Delay1ms(500);
              Pt = Pt->next[GameOver];
          }

          // update ST7735R player sprites
          John.Draw();
          Arthur.Draw();

          // draw bullet if b != nullptr
          if(cycles > 5){
              John.DrawBullet();
              Arthur.DrawBullet();
          }

          //draw active powerups
          PWs.drawPowerups();

          // draw powerup indicators
          John.PWIndicate();
          Arthur.PWIndicate();

          //check is healthboost is active for both players and spawn healthboost icon
          if(healthboost1.getStatus()){
              healthboost1.Draw();
          }

          if(healthboost2.getStatus()){
              healthboost2.Draw();
          }

          //check is ammoboost is active for both players and spawn ammoboost icon
          if(ammoboost1.getStatus()){
              ammoboost1.Draw();
          }

          if(ammoboost2.getStatus()){
              ammoboost2.Draw();
          }

          //check if Arthur has gotten healthboost or ammoboost powerup and deactivate
          if(Arthur.gotPowerUp(&healthboost2)){
              Arthur.HealthBoost();
              healthboost2.switchStatus(false);
          }

          if(Arthur.gotPowerUp(&ammoboost2)){
              Arthur.Reload();
              ammoboost2.switchStatus(false);
          }

          //check if John has gotten healthboost or ammoboost powerup and deactivate
          if(John.gotPowerUp(&healthboost1)){
              John.HealthBoost();
              healthboost1.switchStatus(false);
          }

          if(John.gotPowerUp(&ammoboost1)){
              John.Reload();
              ammoboost1.switchStatus(false);
          }


      } while(Pt->state == InGame);
      }


      // State 3: GameOver/Results
      if(Pt->state == GameOver){
          __disable_irq();
          ST7735_FillScreen(0xFFFF);
          ST7735_SetCursor(0, 1);
          OutPhrase(GAMEOVER);
          //display Arthur win
          if(John.isDead() && !(Arthur.isDead())){
              OutPhrase(P2_WIN);
              ST7735_DrawBitmap(70, 70, ArthurWin, 45, 45);

          }
          //display John win
          else if(Arthur.isDead() && !(John.isDead())){
              OutPhrase(P1_WIN);
              ST7735_DrawBitmap(70, 70, JohnWin, 45, 47);
          }
          //display tie
          else{
              OutPhrase(TIE);
          }
          OutPhrase(END);
          Clock_Delay1ms(500);
      do{
          // Wait until switch is clicked to play again
          if(Switch_In() == 1 | Switch_In() == 2){
              Pt = Pt->next[StartMenu];
          }
      } while(Pt->state == GameOver);
      }
  }
}
