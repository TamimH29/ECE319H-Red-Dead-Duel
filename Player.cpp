#include <iostream>
#include "Player.h"
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
#include "images/images.h"

//Player Functions

//move up the screen
void Player::MoveUp(int vx){
    x -= vx;
    lastMove = 1;
}

//move down the screen
void Player::MoveDown(int vx){
    x += vx;
    lastMove = -1;
}

//checks to see if player is hit by opponent's bullet through coordinate analysis
bool Player::isHit(Bullet* opp){
    int bull_bot = opp->x + (opp->height-1), bull_top = opp->x, bull_left = opp->y, bull_right = opp->y + (opp->width-1);
    int play_top = x-3, play_bot = x + (height+3), play_left = y+4, play_right = y + (width-5);
    if(iframe == 0){
    if(!right){
        if(opp->active && (bull_left<play_right) && (bull_bot<play_bot) && (bull_top>play_top)){
                return true;
        }
    }
    else{
        if(opp->active && (bull_right>play_left) && (bull_bot<play_bot) && (bull_top>play_top)){
                return true;
        }
    }
    }

    return false;
}

//need to draw
bool Player::isDraw(){
    return needDraw == 1;
}

//draw function for player sprites
void Player::Draw(){

    ST7735_DrawBitmap(x, y, image, width, height);

    if(lastMove > 0){
        ST7735_FillRect(x + height, 136-y, width, height, 0xCC66);
    }
    if(lastMove < 0){
        ST7735_FillRect(x - (height-3), 136-y, width, height, 0xCC66);
    }

    lastMove = 0;
}

//checks if player has invincibility in order to prevent double counting for a single hit
bool Player::isIV(){
    if(iframe != 0){
        return true;
    }
    return false;
}

//checks if player is dead
bool Player::isDead(){
    return health <= 0;
}

//decrements health if no shield
void Player::DecrementHealth(){
    iframe = 15;
    if(shieldStatus == true){
        return;
    }
    health -= 1;
}

//decrements ammo
void Player::DecrementAmmo(){
    if(rapid){
        return;
    }
    ammo-=1;
}

//reloads ammo to full
void Player::Reload(){
    ammo = 9;
}

//increases health by 2 up to max health
void Player::HealthBoost(){
    if(health > 7){
        health = 9;
    }
    else{
        health += 2;
    }
}

//bullet cooldown after shooting depending on bullet motion speed
void Player::setCooldown(){
    if(slowed){
        shootCooldown = 30;
    }
    else if(rapid){
        shootCooldown = 10;
    }
    else{
        shootCooldown = 20;
    }
}

//checks if cooldown is done so player can shoot
bool Player::canShoot(){
    if(shootCooldown == 0){
        return true;
    }
    return false;
}

//decrements cooldown time
void Player::decrementCooldown(){
    if(shootCooldown > 0){
        shootCooldown--;
    }
}

//decrements invincibility time
void Player::DecrementIV(){
    if(iframe > 0){
        iframe--;
    }
}

//outputs colored line for corresponding powerup that the player has
void Player::PWIndicate(){
    if(right){
        if(DEindicator){
            ST7735_DrawFastVLine(3, 138-y, 10, ST7735_RED);
        } else{
            ST7735_DrawFastVLine(3, 138-y, 10, 0xCC66);
        }
        if(SHindicator){
            ST7735_DrawFastVLine(5, 138-y, 10, ST7735_BLUE);
        } else{
            ST7735_DrawFastVLine(5, 138-y, 10, 0xCC66);
        }
        if(RFindicator){
            ST7735_DrawFastVLine(7, 138-y, 10, ST7735_YELLOW);
        } else{
            ST7735_DrawFastVLine(7, 138-y, 10, 0xCC66);
        }
    } else{
        if(DEindicator){
            ST7735_DrawFastVLine(3, 138-y, 10, ST7735_RED);
        } else{
            ST7735_DrawFastVLine(3, 138-y, 10, 0xCC66);
        }
        if(SHindicator){
            ST7735_DrawFastVLine(5, 138-y, 10, ST7735_BLUE);
        } else{
            ST7735_DrawFastVLine(5, 138-y, 10, 0xCC66);
        }
        if(RFindicator){
            ST7735_DrawFastVLine(7, 138-y, 10, ST7735_YELLOW);
        } else{
            ST7735_DrawFastVLine(7, 138-y, 10, 0xCC66);
        }
    }
}

//updates bullet position based on bullet speed (normal, slowed, rapid)
void Player::UpdateBullet(){
    if(b != nullptr && b->moved == true){
    if(right){
        if(b->y > 5){
            if(slowed){
                b->y -= 3;
            }
            else if(rapid){
                b->y -= 12;
            }
            else{
                b->y -= 7;
            }
        }
    }
    else{
        if(b->y < 150){
            if(slowed){
                b->y += 3;
            }
            else if(rapid){
                b->y += 12;
            }
            else{
                b->y += 7;
            }
        }
    }

    b->moved = false;
    }
}

//draws bullet sprite by checking travel limits and bullet speed
void Player::DrawBullet(){
    if(b != nullptr){
    if(right){
        ST7735_DrawBitmap(b->x, b->y, Bullet1, 8, 7);
        if(slowed){ST7735_FillRect(b->x,  138-b->y + 8, 8, 7, 0xCC66);}
        else if(rapid){ST7735_FillRect(b->x,  138-b->y + 3, 8, 10, 0xCC66);}
        else{ST7735_FillRect(b->x,  138-b->y, 8, 15, 0xCC66);}
        if(b->y <= 5){
            ST7735_FillRect(b->x, 138 - b->y + 16, 8, 7, 0xCC66);
            delete b;
            b = nullptr;
        }
    }
    else{
        ST7735_DrawBitmap(b->x, b->y, Bullet1, 8, 7);
        if(slowed){ST7735_FillRect(b->x,(138-b->y) + 20, 8, 7, 0xCC66);}
        else if(rapid){ST7735_FillRect(b->x,  138-b->y + 29, 8, 14, 0xCC66);}
        else{ST7735_FillRect(b->x,(138-b->y) + 24, 8, 15, 0xCC66);}
        if(b->y >= 150){
            ST7735_FillRect(b->x, 155 - b->y, 8, 7, 0xCC66);
            delete b;
            b = nullptr;
        }
    }

    b->moved = true;
    }
}

//getter for x
int Player::getX(){
    return x;
}

//getter for y
int Player::getY(){
    return y;
}

//setter for x
void Player::setX(int x){
    this->x = x;
}

//setter for y
void Player::setY(int y){
    this->y = y;
}

//checks to ensure player does not leave borders of gameplay
void Player::BorderCheck(){
    // screen borders
    if(x<30){
        x = 30;
    } else if((x+height)>127){
        x = 95;
    }
    if(y<0){
        y = 0;
    } else if(y+width>159){
        y = 159-width;
    }
}

//getter for health
int Player::getHealth(){
    return health;
}

//getter for ammo
int Player::getAmmo(){
    return ammo;
}

//used to reset player default settings at the start of each new game
void Player::restart(){
    if(right){
        x = 90;
        y = 130;
    }
    else{
        x = 30;
        y = 10;
    }
    iframe = 30;
    health = 9;
    ammo = 9;
    rapid = false;
    shieldStatus = false;
    slowed = false;
}

//original slowed function, not used
void Player::slow(){
    if(lastUp > 0){
        x--;
    } else if(lastUp < 0){
        x++;
    }
}

// toggle shieldStatus
void Player::shield(){
    if(shieldStatus == false && powerup[Shield] != 1){
        shieldStatus = true;
    } else{
        shieldStatus = false;
    }
}

//checks if player has collided with the ammo reload or health boost powerups
bool Player::gotPowerUp(Powerup* p){
    int play_bot = x+height-1;
    if(p->active && play_bot > p->x){
        return true;
    }
    return false;
}

//Player constructor
Player::Player(int x, int y, const unsigned short* sprite, bool right){
    this->x = x;
    this->y = y;
    alive = true;
    slowed = false;
    rapid = true;
    width = 25;
    height = 28;
    health = 9;
    this->right = right;
    ammo = 9;
    image = sprite;
    b = nullptr;
    DEindicator = false;
    SHindicator = false;
    RFindicator = false;
    for(int i=0; i<5; i++){
        powerup[i] = 0;
    }
}

//Bullet functions

//Bullet move left original, not used
void Bullet::MoveLeft(){
    for(uint32_t t=y-32;t>5;t=t-5){
     ST7735_DrawBitmap(x+12, t, Bullet1, 8, 7);
     ST7735_FillRect(x+12,  138-t + 8, 8, 7, 0xCC66);
     Clock_Delay1ms(20);
   }
   ST7735_FillRect(x+12, y+15, 8, 7, 0xCC66);
}

//Bullet move right original, not used
void Bullet::MoveRight(){
    for(uint32_t t=y+32;t<150;t=t+5){
      ST7735_DrawBitmap(x+12, t, Bullet1, 8, 7);
      ST7735_FillRect(x+12,(138-t) + 22, 8, 7, 0xCC66);
      Clock_Delay1ms(20);
    }
    ST7735_FillRect(x+12, y-5, 8, 7, 0xCC66);
}

//Bullet constructor
Bullet::Bullet(int x, int y){
    active = true;
    this->x = x;
    this-> y = y;
    image = Bullet1;
    width = 8;
    height = 7;
    moved = false;
}

//Powerup functions

//checks if powerup icon is hit by player's bullet
bool Powerup::PisHit(Bullet* opp){
    int bull_bot = opp->x + (opp->height-1), bull_top = opp->x, bull_left = opp->y, bull_right = opp->y + (opp->width-1);
    int play_top = x, play_bot = x + (height-1), play_left = y+6, play_right = y + (width-1)-6;

    // check if new_p is to the left, right, above, or below other p
    if(bull_right < play_left || play_right < bull_left){
        return false;
    }
    if(bull_bot < play_top || play_bot < bull_top){
        return false;
    }

    return true;
}

//draw function for powerup
void Powerup::Draw(){
    ST7735_DrawBitmap(x, y, image, width, height);
}

//altenate powerp draw
void Powerup::DrawS(){
    if(active){
        ST7735_DrawBitmap(x, y, image, width, height);
    }
}

//removes powerup icon from screen
void Powerup::Delete(){
    ST7735_FillRect(x, y, width, height, bgColor);
}

//changes powerup status
void Powerup::switchStatus(bool input){
    active = input;
}

//getter for powerup status
bool Powerup::getStatus(){
    return active;
}

//getter for x
int Powerup::getX(){
    return x;
}

//getter for y
int Powerup::getY(){
    return y;
}

//Powerup constructor
Powerup::Powerup(int x, int y, const unsigned short* sprite, int w, int h, int p){
    this->x = x;
    this->y = y;
    image = sprite;
    width = w;
    height = h;
    active = false;
    currentPowerUp = p;
}

//alternative Powerup constructor
Powerup::Powerup(int x, int y, const unsigned short* sprite, int w, int h){
    this->x = x;
    this->y = y;
    image = sprite;
    width = w;
    height = h;
    active = false;
}

//checks if a queue is valid
bool PowerupQueue::QueueIsValid(int* x, int* y){
    for (int i = 0; i < MAX_POWERUPS; i++) {
        // PWarray is not full
        if (PWarray[i] == nullptr) {
            // generate x and y - check if location overlaps
            // try 10 different locations max
            for (int i = 0; i < 10; i++){
                *x = Random(55) + 25;
                *y = Random(41) + 60;
                if(isHit(PWarray[i], x, y) == false){
                    return true;
                }
            }
        }
    }
    return false;
}

//checks if powerupqueue has been hit
bool PowerupQueue::isHit(Powerup* p, int* x, int* y){
    int p_bot = p->x + (p->height-1), p_top = p->x, p_left = p->y, p_right = p->y + (p->width-1);

    int pn_top = *x, pn_bot = *x + (15-1), pn_left = *y, pn_right = *y + (15-1);

    // check if new_p is to the left, right, above, or below x and y
    if(pn_right < p_left || p_right < pn_left){
        return false;
    }
    if(pn_bot < p_top || p_bot < pn_top){
        return false;
    }

    return true;
}

// Function to add a Powerup
void PowerupQueue::addPowerup(Powerup* p) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (PWarray[i] == nullptr) {
            PWarray[i] = p;
            break;
        }
    }
}

// Function to remove a Powerup
void PowerupQueue::removePowerup(int index) {
    Powerup* p = PWarray[index];
    if (index >= 0 && index < MAX_POWERUPS && PWarray[index] != nullptr) {
        ST7735_FillRect(p->getX(),  138-p->getY() + 8, 15, 15, 0xCC66);
        delete PWarray[index];
        PWarray[index] = nullptr;
    }
}

//draw function for powerups
void PowerupQueue::drawPowerups(){
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (PWarray[i] != nullptr) {
            PWarray[i]->Draw();
        }
    }
}


//checks to see if powerup is active and change status of indicators
void PowerupQueue::check(Player* p){
    for(int i=0; i< MAX_POWERUPS; i++){
        if(PWarray[i] != nullptr){
            if(PWarray[i]->PisHit(p->b) == true){
                // set powerup in player to 4 seconds
                p->powerup[PWarray[i]->currentPowerUp] = 120;
                if(PWarray[i]->currentPowerUp == 0){
                    p->DEindicator = true;
                } else if(PWarray[i]->currentPowerUp == 1){
                    p->SHindicator = true;
                } else if(PWarray[i]->currentPowerUp == 2){
                    p->RFindicator = true;
                }
                removePowerup(i);
                Sound_Powerup();
            }
        }
    }
}
