#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


void Socrates::doSomething(){
    int ch;
    if (getHP() <= 0){
        getWorld()->playSound(SOUND_PLAYER_DIE);
        // if HP is less than 0, then Socrates died, so we need to tell the game he did via setAlive and player with the sound
        setAlive(false);
        return;
    }
    
    if (getWorld()->getKey(ch)){
        switch(ch){ //Checks to see what Key was pressed
            case KEY_PRESS_RIGHT:
                moveTo(VIEW_WIDTH/2, VIEW_HEIGHT/2); //moves to center
                m_angle -= ANGLE_CHANGE; //changes angle
                setDirection(m_angle - 180); //changes Direction
                moveAngle(m_angle, VIEW_RADIUS); //change angle in unit circle by 5 degrees and then send him to border again
                //move CW 5 degrees
                break;
            case KEY_PRESS_LEFT:
                moveTo(VIEW_WIDTH/2, VIEW_HEIGHT/2); //moving left is CCW
                m_angle += ANGLE_CHANGE;
                moveAngle(m_angle, VIEW_RADIUS); //change angle in unit circle by 5 degrees and then send him to border again
                setDirection(m_angle - 180);
                //move CCW 5 degrees
                break;
            case KEY_PRESS_SPACE:
                if (m_sprayCharges < 1)
                    break;
                getWorld()->playSound(SOUND_PLAYER_SPRAY); //playSound of firing Spray if valid
                m_sprayCharges--;
                getWorld()->fireSpray(getX(),getY());
                break;
            case KEY_PRESS_ENTER:
                if (m_flameCharges <= 0)
                    break;
                getWorld()->playSound(SOUND_PLAYER_FIRE); //play sound of Firing Fire if valid
                m_flameCharges--;
                getWorld()->fireFlame(getX(), getY());
                break;
        }
   }
    else //since getKey returns false if no key was pressed, we need to make an else statement to increment spray Chargres
        if (nSCharges() < 20)
            m_sprayCharges++;
}



bool Spray::damageDone(){
    if (getWorld()->movementOverlapDamage(getX(), getY(), this)){
        getWorld()->damageObject(getX(), getY(), -2); //if Spray, we damage Objects with 2 damages
        setAlive(false);
        return true;
    }
    return false;
}
bool Flame::damageDone(){
    if (getWorld()->movementOverlapDamage(getX(), getY(), this)){
        getWorld()->damageObject(getX(), getY(), -5); //if spray, we damage object with 5 damage
        setAlive(false);
        return true;
    }
    return false;
}



void Projectile::doSomething(){ // we can have general doSomething
    if (!getAlive()){
        return;
    }
    if (damageDone()){ //only damageDone differs between projectiles based on the damage they do
        return;
    }
    int angle = getDirection();
    const double PI = 4 * atan(1);
    double nx = getX() + SPRITE_WIDTH*(cos(angle*1.0 / 360 * 2 * PI));
    double ny = getY() + SPRITE_WIDTH*(sin(angle*1.0 / 360 * 2 * PI));
    moveTo(nx, ny);
    increaseDistance(SPRITE_WIDTH);
    if (maxD() <= DTraveled()){ //check that it traveled its max distance
        setAlive(false);
    }
}


Goodie::Goodie(StudentWorld* world, int ImageID, double startX, double startY, Direction dir, int depth):Actor(world, ImageID, startX, startY, dir, depth, false, true, false, false, true, 0, false, true)
{
    int level = getWorld()->getLevel();
    m_lifetime = (std::max(randInt(0,300 - 5 * level), 50)); //calculate how many ticks the Goodie needs to stay alive for
    m_timeLived = 0;
}

void Goodie::doSomething(){
    if (!getAlive())
        return;
    if (getWorld()->movementOverlapSocrates(getX(), getY())){
        Effect(); //Effect changes based on each goodie.
        return;
    }
    if (getLifetime() <= getTimeLived())
        setAlive(false);
    m_timeLived++;
    
}

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* world, double startX, double startY, int ImageID): Goodie(world, ImageID, startX, startY)
{
}

void RestoreHealthGoodie::Effect(){
    getWorld()->playSound(SOUND_GOT_GOODIE); //play sound for Goodie
    getWorld()->changeSocratesHP(100); //max out Socrates health
    setAlive(false);
    getWorld()->increaseScore(250); //increase score by 250
}

Fungus::Fungus(StudentWorld* world, double startX, double startY, int ImageID):
Goodie(world, ImageID, startX, startY){
    
}

void Fungus::Effect(){
    getWorld()->changeSocratesHP(-20); //max out Socrates -20
    setAlive(false);
    getWorld()->increaseScore(-50);
}

FlamethrowerGoodie::FlamethrowerGoodie(StudentWorld* world, double startX, double startY, int ImageID):
Goodie(world, ImageID, startX, startY)
{
}

void FlamethrowerGoodie::Effect(){
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->changeFlameCharges(5); //change Flame Charges by 5
    setAlive(false);
    getWorld()->increaseScore(300); //increase score my 300
}


ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, double startX, double startY, int ImageID):
Goodie(world, ImageID, startX, startY)
{
}

void ExtraLifeGoodie::Effect(){
    getWorld()->playSound(SOUND_GOT_GOODIE);
    setAlive(false);
    getWorld()->increaseScore(500);
    getWorld()->incLives(); //increase live
}

BPit::BPit(StudentWorld* world, double startX, double startY, int depth, bool damageable, int imageID, Direction dir):Actor(world, imageID, startX, startY, dir, depth, false, true, true, false, false, 0, false)
{
    nEColi = 2;
    nRSalmonella = 5;
    nASalmonella = 3;
} //we need the BPit to have these many bacteria


void BPit::doSomething(){
    if ((nEColi + nASalmonella + nRSalmonella) == 0) //Check that all bacteria have left.
        setAlive(false);
    int chanceSpawn = randInt(0,50); //chance of a spawn is 1/50
    if (chanceSpawn == 0){
        int typeBac = randInt(0, 2);
        //all bacteria have same chance of spawning
        while(true){
            if (typeBac == 0 && nASalmonella != 0){ //only spawn if there are some of that type of bacteria left of that kind in the pit
                nASalmonella--;
                getWorld()->addBacteria(getX(), getY(), typeBac);
                getWorld()->playSound(SOUND_BACTERIUM_BORN);
                    break;
            }
            else if (typeBac == 1 && nRSalmonella != 0){
                nRSalmonella--;
                getWorld()->addBacteria(getX(), getY(), typeBac);
                getWorld()->playSound(SOUND_BACTERIUM_BORN);
                break;
            }
            else if (typeBac == 2 && nEColi != 0){
                nEColi--;
                getWorld()->addBacteria(getX(), getY(), typeBac);
                getWorld()->playSound(SOUND_BACTERIUM_BORN);
                break;
            }
            else {
               typeBac = randInt(0, 2);
            }
        }
    }
}


void Bacteria::computeXY(int v, int& newV){ //computes new coordinate for duplicating a bacteria based on the spec.
    if (v < VIEW_RADIUS)
        newV = v + (SPRITE_WIDTH/2);
    else if (v == VIEW_RADIUS)
        newV = v;
    else
        newV = v - (SPRITE_WIDTH/2);
    
    
}

void Bacteria::duplicateBacteria(){
    int newX, newY;
    computeXY(getX(), newX); //compute new x coordinate
    computeXY(getY(), newY); //compute new y coordinate
    
    addBac(newX, newY); //adds bacteria based on x,y coordinate.
}


void RSalmonella::addBac(int newX, int newY){ //derived function overwrites base function
    getWorld()->addBacteria(newX, newY, 1); //addBacteria
}

void ASalmonella::addBac(int newX, int newY){ //derived function overwrites base function
    getWorld()->addBacteria(newX, newY, 0);
}
void EColi::addBac(int newX, int newY){ //derived function overwrites base function
    getWorld()->addBacteria(newX, newY, 2);
}

bool Bacteria::OnDish(int x, int y){ //checks that the bacteria doesn't fall out of the petri dish
    return ((VIEW_RADIUS*VIEW_RADIUS) >= ((x-VIEW_RADIUS)*(x-VIEW_RADIUS)+ (y-VIEW_RADIUS)*(y-VIEW_RADIUS)));
}


Bacteria::~Bacteria(){
    if (getHP() <= 0 && gethasHP()){ //if the bacteria actually died than we play sound and increase score, otherwise we are just destructing it
        getWorld()->increaseScore(100);
        getWorld()->playSound(m_sound);
        int randchance = randInt(1,2);
        if (randchance == 1)
            getWorld()->addFood(getX(),getY());
    }
}

Bacteria::Bacteria(StudentWorld* world, double startX, double startY, int imageID, int initialHP, int sound): Actor(world, imageID, startX, startY, up, 0, true, true, true, false, true, initialHP, false), m_sound(sound)
{
    movementPlanDistance = 0;
    foodEaten = 0;
}

void EColi::playSound(){ //playSound
    getWorld()->playSound(SOUND_ECOLI_HURT);
}




void RSalmonella::playSound(){
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}


void Bacteria::checkSocratesFoodOverlap(int amount){ //this does steps 2-4 in Regular Salmonella
if(getWorld()->movementOverlapSocrates(getX(), getY())){
       getWorld()->playSound(SOUND_PLAYER_HURT);
       getWorld()->changeSocratesHP(-amount); //socrates HP needs to decrease by this amount
   }
   else if(getFood() == 3){
       duplicateBacteria();
       resetFood();
   }
   else if(getWorld()->checkOverlapFood(this)){
       IncFood();
   }
}

void Bacteria::movementSalmonella(){ //this is how both aggressive and regular salmonella move
    if(getmovePlan() > 0){
        dmovePlan();
        double dx, dy;
        int units = 3;
        getPositionInThisDirection(getDirection(), units, dx, dy);
        if ((getWorld()->checkBactMovement(dx,dy)) && OnDish(dx, dy)){
            moveTo(dx, dy);
            return;
        }
        else{
            int nDirection = randInt(0,359);
            setDirection(nDirection);
            resetMovePlan();
        }
    }
    else{
        int angletoClosestFood = getWorld()->AngleBacteria(this, VIEW_RADIUS);
        setDirection(angletoClosestFood);
        resetMovePlan();
        return;
    }
}

void RSalmonella::doSomethingSpecial(){ //this is the difference in the functions of a bacteria
   checkSocratesFoodOverlap(1);
    movementSalmonella();
}

void ASalmonella::doSomethingSpecial(){ //this is the difference in the functions of a bacteria
    if (getWorld()->distancetoSocrates(this ,72)){
        double dx, dy;
        getPositionInThisDirection(getWorld()->angleToSocrates(this), 3, dx, dy);
        setDirection(getWorld()->angleToSocrates(this));
        if ((getWorld()->checkBactMovement(dx,dy)) && OnDish(dx, dy)){
            moveTo(dx, dy);
        }
        checkSocratesFoodOverlap(2);
        return;
    }
    checkSocratesFoodOverlap(2);
    movementSalmonella();
    
        
}


void EColi::doSomethingSpecial(){ //this is the difference in the functions of a bacteria
    checkSocratesFoodOverlap(4);
    if (getWorld()->distancetoSocrates(this,VIEW_HEIGHT)){
        int angle = getWorld()->angleToSocrates(this);
        setDirection(angle);
        for(int i = 0; i != 10; i++){
            double dx, dy;
            getPositionInThisDirection(angle, 2, dx, dy);
            if(getWorld()->checkBactMovement(dx,dy)){
                moveTo(dx, dy);
                return;
            }
            angle = angle+10;
        }
    }
    
}

void Bacteria::doSomething(){ //Bacteria do something for all Bacteria
    if (!getAlive())
        return;
    doSomethingSpecial();
    return;
}
