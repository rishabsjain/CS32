#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>


//Any functions needed explaining are explained in a base class or in the .cpp

const int ANGLE_CHANGE = 5;

using Direction = int;
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject{
public:
    Actor(StudentWorld* world, int imageID, double startX, double startY, Direction dir, int depth, bool HP = false, bool isAlive = true, bool enemy = false, bool blocker = false, bool damageable = false, int initialHP = 0, bool proj = false, bool Goodie = false, double size = 1.0):
    GraphObject(imageID, startX, startY, dir, depth, size), m_world(world), x(startX), y(startY),hasHP(HP), isAlive(isAlive), isEnemy(enemy), isBlocker(blocker), isDamageable(damageable), m_HP(initialHP), maxHP(initialHP), isProjectile(proj), isGoodie(Goodie)
    {
        
    } //Initiliazer LIST!!!!
    
    
    virtual ~Actor(){}
    
    virtual void doSomething() = 0; //no actor class should be created
    
    virtual void playSound(){
    }
    
    void setAlive(bool alive){
        isAlive = alive;
    }
    bool getAlive() const {
        return isAlive;
    }
    
    bool getEnemy()  const {
        return isEnemy;
    }
    
    bool getBlocker()  const {
        return isBlocker;
    }
    bool getDamageable() const {
        return isDamageable;
    }
    bool gethasHP() const {
        return hasHP;
    }
    int getHP() const {
        return m_HP;
    }
    bool getProj() const {
        return isProjectile;
    }
    
    bool getGoodie() const {
        return isGoodie;
    }
    void changeHP(int amount){
        if (m_HP + amount > maxHP)
            m_HP = maxHP;
        else
            m_HP += amount;
    }
    
    StudentWorld* getWorld() const {
        return m_world;
    }
    
    int getMaxHP() const {
        return maxHP;
    }
    
private:
    StudentWorld* m_world;
    int x;
    int y;
    bool hasHP;
    bool isAlive;
    bool isEnemy;
    bool isGoodie;
    bool isBlocker;
    bool isDamageable;
    bool isProjectile;
    int m_HP;
    int maxHP;
    //May want to add more later
    
};


class Dirt:public Actor{
public:
    Dirt(StudentWorld* world, double startX, double startY, Direction dir = right, int depth = 1):
    Actor(world, IID_DIRT, startX, startY, dir, depth, false, true, false, true, true, 0, false)
    {
    }
    virtual void doSomething(){
    //like a USC Student: it does nothing
        return;
    }
    virtual ~Dirt(){}
};

class Socrates: public Actor{
public:
    Socrates(StudentWorld* world, int imageID = IID_PLAYER, double startX = 0, double startY = (VIEW_HEIGHT/2), Direction dir = right, int depth = 0, int initialHP = 100): Actor(world, imageID, startX, startY, dir, depth, true, true, false, false, true, initialHP, false), m_flameCharges(5), m_sprayCharges(20), m_angle(180)
    
    
    {
    } //Initializer List with 100 HP, 5 flameCharges, 20 SprayCharges, and is alive to begin
    
    virtual ~Socrates(){}
    
    int getAngle() const {
        return m_angle;
    }
    
    void changeFlameCharges(int amount){
        m_flameCharges += amount;
    }
    
    int nFCharges() const {
        return m_flameCharges;
    }
    int nSCharges() const {
        return m_sprayCharges;
    }
    virtual void doSomething();
private:
    int m_flameCharges;
    int m_sprayCharges;
    int m_angle;
    
};

class Food: public Actor{
public:
    Food(StudentWorld* world, double startX, double startY, Direction dir = up, int depth = 1, double size = 1.0): Actor(world, IID_FOOD, startX, startY, dir, depth, false, true, false, false, false, 0, false)
        {}
    
        virtual void doSomething(){
            return;
        }
    
        virtual ~Food(){}
    
};


class Projectile: public Actor{
public:
    Projectile(StudentWorld* world, int ImageID ,double startX, double startY, Direction dir, int maxDistance):
    Actor(world, ImageID, startX, startY, dir, 1, false, true, false, false, false, 0, true), m_maxDistance(maxDistance)
    {
        m_DistanceTraveled = 0;
    }
    
    void increaseDistance(int amount){
        m_DistanceTraveled += amount;
    }
    
    int maxD() const {
        return m_maxDistance;
    }
    
    int DTraveled()  const
    {
        return m_DistanceTraveled;
    }
    
    virtual ~Projectile(){}
    
    virtual void doSomething();
    
    virtual bool damageDone() = 0;

private:
    int m_maxDistance;
    int m_DistanceTraveled;
};

class Flame: public Projectile{
public:
    Flame(StudentWorld* world, double startX, double startY, Direction dir, int ImageID = IID_FLAME, int maxDistance = 32):
    Projectile(world, ImageID, startX, startY, dir, maxDistance){
        
    }
    
    virtual ~Flame(){
        
    }
    
    virtual bool damageDone();

    
};

class Spray: public Projectile{
public:
    Spray(StudentWorld* world, double startX, double startY, Direction dir, int ImageID = IID_SPRAY, int maxDistance = 112):
    Projectile(world, ImageID, startX, startY, dir, maxDistance){
        
    }
    
    virtual ~Spray(){
        
    }
    virtual bool damageDone();

    
    
};

class Goodie: public Actor{
public:
    Goodie(StudentWorld* world, int ImageID, double startX, double startY, Direction dir = right, int depth = 1);
    
    virtual ~Goodie(){
    }
    
    virtual void doSomething();
    
    int getLifetime() const {
        return m_lifetime;
    }

    int getTimeLived() const {
        return m_timeLived;
    }
    
    virtual void Effect() = 0;
    
private:
    int m_lifetime;
    int m_timeLived;
    };


class RestoreHealthGoodie: public Goodie{
public:
    RestoreHealthGoodie(StudentWorld* world, double startX, double startY, int ImageID = IID_RESTORE_HEALTH_GOODIE);
    
    virtual ~RestoreHealthGoodie(){
        
    }
    virtual void Effect();
};


class Fungus: public Goodie{
public:
    Fungus(StudentWorld* world, double startX, double startY, int ImageID = IID_FUNGUS);
    
    virtual ~Fungus(){
        
    }
    virtual void Effect();
};

class FlamethrowerGoodie: public Goodie{
public:
    FlamethrowerGoodie(StudentWorld* world, double startX, double startY, int ImageID = IID_FLAME_THROWER_GOODIE);
    
    virtual ~FlamethrowerGoodie(){
        
    }
    
    virtual void Effect();
};

class ExtraLifeGoodie: public Goodie{
public:
    ExtraLifeGoodie(StudentWorld* world, double startX, double startY, int ImageID = IID_EXTRA_LIFE_GOODIE);
    
    virtual ~ExtraLifeGoodie(){
        
    }
    
    virtual void Effect();
};

class BPit:public Actor{
public:
    BPit(StudentWorld* world, double startX, double startY, int depth = 1, bool damageable = false, int imageID = IID_PIT, Direction dir = right);
    virtual ~BPit(){
        
    }
    
    
    virtual void doSomething();
    
private:
    int nEColi;
    int nRSalmonella;
    int nASalmonella;
    };


class Bacteria: public Actor{
public:
    Bacteria(StudentWorld* world, double startX, double startY, int imageID, int initialHP, int sound);
    
    bool OnDish(int x, int y);
    
    
    virtual void doSomething();
    
    virtual ~Bacteria();
    
    void IncFood(){
        foodEaten++;
    }
    
    void resetFood(){
        foodEaten = 0;
    }
    int getFood() const {
        return foodEaten;
    }
    int getmovePlan() const {
        return movementPlanDistance;
    }
    
    void resetMovePlan(){
        movementPlanDistance = 10;
    }
    void dmovePlan(){
        movementPlanDistance--;
    }
    
    
    void computeXY(int v, int& newV);
    
    void duplicateBacteria();
    virtual void addBac(int newX, int newY) = 0;
    void movementSalmonella();
    virtual void doSomethingSpecial() = 0;
    
    void checkSocratesFoodOverlap(int amount);
    
    
    
private:
    int movementPlanDistance;
    int foodEaten;
    int m_sound;
    
    
    };

class RSalmonella: public Bacteria{
public:
    RSalmonella(StudentWorld* world, double startX, double startY, int initialHP = 4): Bacteria(world,startX,startY,IID_SALMONELLA,initialHP, SOUND_SALMONELLA_DIE){
        
    }
    
    virtual void doSomethingSpecial();
    
    virtual ~RSalmonella(){
        
    }
    void playSound();
    
    virtual void addBac(int newX, int newY);
};
    
class ASalmonella: public RSalmonella{
public:
    ASalmonella(StudentWorld* world, double startX, double startY, int initialHP = 10):RSalmonella(world, startX, startY, initialHP) {}
    
    
    
    virtual ~ASalmonella(){
        
    }
    virtual void doSomethingSpecial();
    virtual void addBac(int newX, int newY);
    
};

class EColi: public Bacteria{
public:
    EColi(StudentWorld* world, double startX, double startY, int imageID = IID_ECOLI, int initialHP = 5): Bacteria(world,startX,startY,imageID,initialHP,SOUND_ECOLI_DIE) {}
    
    virtual ~EColi(){
        
    }
    virtual void doSomethingSpecial();
    
    void playSound();
    virtual void addBac(int newX, int newY);
    
    
};
#endif // ACTOR_H_
