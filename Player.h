#include <iostream>
#ifndef PLAYER_H
#define PLAYER_H

extern int bgColor;

extern uint32_t M;
uint32_t Random32(void);
uint32_t Random(uint32_t n);

//player, bullet, powerup, powerupqueue class template

//powerup definitions
typedef enum{
    DeadEye,
    Shield,
    RapidFire,
    numPowerUps
}PowerUp;

//Bullet class
class Bullet{
friend class Player;
friend class Powerup;
friend class PowerupQueue;
private: 
    int x;
    int y;
    int v;
    bool active;
    const unsigned short* image;
    int width;
    int height;
public:
    bool moved;
    void MoveRight();
    void MoveLeft();
    Bullet(int, int);
};

//Powerup class
class Powerup{
friend class Bullet;
friend class Player;
friend class PowerupQueue;
private:
    int x;
    int y;
    const unsigned short* image;
    int width;
    int height;
    bool active;
public:
    int getX();
    int getY();
    int currentPowerUp;
    bool PisHit(Bullet*);
    void Draw();
    void DrawS();
    void Delete();
    void switchStatus(bool);
    bool getStatus();
    Powerup(int, int, const unsigned short*, int, int, int);
    Powerup(int, int, const unsigned short*, int, int);
};

//Player class
class Player{
friend class Bullet;
friend class Powerup;
friend class PowerupQueue;
private:
    int x;
    int y;
    bool alive; //alive status
    int iframe; //invincibility time frame
    int width;
    int height;
    int health;
    int ammo;
    int shootCooldown;  //wait time period between shots
    bool right; //right or left side player
    const unsigned short* image;
    const unsigned short* black;
    bool needDraw;
public:
    bool shieldStatus; //has shield or not
    bool rapid; //has rapidfire or not
    bool slowed; //affect by deadeye or not
    int lastUp;
    int lastMove;
    //indicators for powerups
    bool DEindicator;
    bool SHindicator;
    bool RFindicator;
    // timer for powerups
    int powerup[numPowerUps];
    void slow();
    void shield();
    void rapidfire();
    bool gotPowerUp(Powerup* p);
    Bullet* b;
    void MoveUp(int);
    void MoveDown(int);
    // void Shoot();
    void UpdateBullet();
    void DrawBullet();
    bool isIV();
    bool isHit(Bullet*);
    bool isDraw();
    bool isDead();
    void DecrementHealth();
    void DecrementAmmo();
    void DecrementIV();
    void Reload();
    void HealthBoost();
    void PWIndicate();
    Player(int, int, const unsigned short*, bool);
    void BorderCheck();
    void Draw();
    int getX();
    int getY();
    void setX(int x);
    void setY(int y);
    int getHealth();
    int getAmmo();
    void setCooldown();
    bool canShoot();
    void decrementCooldown();
    void restart();
};

const int MAX_POWERUPS = 3;  // Maximum number of Powerups

//PowerupQueue class
class PowerupQueue{
friend class Powerup;
friend class Player;
friend class Bullet;
private:
    Powerup* PWarray[MAX_POWERUPS] = {nullptr};  // Initialize all to nullptr
public:
    bool QueueIsValid(int*, int*);
    void addPowerup(Powerup*);
    void removePowerup(int);
    void check(Player*);
    void drawPowerups();
    bool isHit(Powerup*, int*, int*);
};

#endif
