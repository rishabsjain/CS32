#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld(){
        cleanUp();

    }
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool checkOverlapInit(int x, int y);
    bool movementOverlapDamage(int x, int y, Actor* start);
    bool movementOverlapSocrates(int x, int y);
    void damageObject(int x, int y, int amount);
    void fireSpray(int x, int y);
    void fireFlame(int x, int y);
    void changeSocratesHP(int amount);
    void changeFlameCharges(int amount){
        m_player->changeFlameCharges(amount);
    }
    void addGoodies();
    int SocratesHealth();
    void addBacteria(int x, int y, int type);
    bool checkOverlapFood(Actor* bac);
    bool isFood(Actor* bac);
    bool checkBactMovement(double x, double y);
    int AngleBacteria(Actor* bac, int dis);
    bool distancetoSocrates(Actor* bac, int distance);
    int angleToSocrates(Actor* bac);
    void addFood(int x, int y);
    
private:
    bool validDistance(const int& x, const int& y);
    int distance(int x1, int y1, int x2, int y2);
    bool finishedLevel();
    Socrates* m_player; //separate pointer to m_player
    std::list<Actor*> m_actorLists;  //container of Nodes
    void makeValidPoint(int& x, int& y);
    int calcAngle(double dy, double dx);
    
};

#endif // STUDENTWORLD_H_
