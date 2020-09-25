#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_player = nullptr; //start this as nullptr, so if you quit immmediately, clean up won't erase some random memory
}


int StudentWorld::init()
{
    m_player = new Socrates(this); //we just need to pass in what student world it exists in
    int level = getLevel();
    int numbDirt = max(180-(20 * level),20); //getting the number of Dirt,Food, and Bacteria need to be created
    int numbFood = min(5*level, 25);
    int numbBacteria = level;
    for (int i = 0; i != numbBacteria; i++){
        int x, y;
        makeValidPoint(x, y);
        while (!checkOverlapInit(x,y)){ //Keeps checking that the point won't cause any overlap. If it does it will make new coordinates
            makeValidPoint(x, y);
        }
        Actor* pit = new BPit(this, x, y);
        m_actorLists.push_back(pit); //push the object to the List of Actor pointers numbBacteria times
    }
    for (int f = 0; f != numbFood; f++){
        int x, y;
        makeValidPoint(x, y);
        while (!checkOverlapInit(x,y)){
            makeValidPoint(x, y);
        }
        addFood(x,y); //push the object to the List of Actor pointers numbFood times
    }
    
    for (int i = 0; i != numbDirt; i++){ //initializing dirt
        int x, y;
        makeValidPoint(x, y);
        while (!checkOverlapInit(x,y)){
            makeValidPoint(x, y);
        }
        Actor* dirt = new Dirt(this, x, y);
        m_actorLists.push_back(dirt); //push the object to the List of Actor pointers numbDirt times
    }
    return GWSTATUS_CONTINUE_GAME; //continue Game because init is done

}


bool StudentWorld::movementOverlapDamage(int x, int y, Actor* start){
    for (auto it = m_actorLists.begin(); it != m_actorLists.end(); it++){
        if (start == *it){ //we don't want to accidentally damage ourselves
            continue;
        }
        if (!(*it)->getDamageable()){ //make sure the object can be damaged
            continue;
        }
        int c = (*it)->getX() - x;
        int d = (*it)->getY() - y;
        int e = c*c + d*d;
        if (e <= (SPRITE_WIDTH * SPRITE_WIDTH)){ //check that the object does overlap with an object not itself in the list of actors
            return true;
        }
    }
    
    return false;
    }

bool StudentWorld::movementOverlapSocrates(int x, int y){
    int c = (m_player)->getX() - x;
    int d = (m_player)->getY() - y;
    int e = c*c + d*d;
    if (e <= (SPRITE_WIDTH * SPRITE_WIDTH)) //checks if you overlap with Socrates(Meant for an object to call)
        return true;
return false;
}

void StudentWorld::damageObject(int x, int y, int amount){ //finds the oldest element in list that is Damageable and at the point(x,y)
    for (auto it = m_actorLists.begin(); it != m_actorLists.end(); it++){
        if (!(*it)->getDamageable()){
            continue;
        }
        int c = (*it)->getX() - x;
        int d = (*it)->getY() - y;
        int e = c*c + d*d;
        if (e <= (SPRITE_WIDTH * SPRITE_WIDTH)){
            if (!(*it)->gethasHP()){ //if there is no HP, we don't want to change HP
                (*it)->setAlive(false); //we just kill the object
            }
            else {
                (*it)->changeHP(amount);
                if ((*it)->getHP() <= 0)
                   (*it)->setAlive(false);
                else{
                    if ((*it)->getEnemy()) // if there is HP and it's an enemy that is not dead, we want it to play the sound that says it got hurt
                        (*it)->playSound();
                }
            }
            break; //gets out of the loop (return could also work here)
        }
    }
}


bool StudentWorld::checkOverlapInit(int x, int y){ //Checks for all Other Overlap in the init stage. Excludes dirt or any other future form of blockers
    for (auto it = m_actorLists.begin(); it != m_actorLists.end(); it++){
        if ((*it)->getBlocker()){
            continue;
        }
        int c = (*it)->getX() - x; //calculate change in x
        int d = (*it)->getY() - y; //calculate change in y
        int e = c*c + d*d; //calculate d squared
        if (e <= (SPRITE_WIDTH * SPRITE_WIDTH)) //see if d squared is less than the spriteWidth squared
            return false;
    }
    return true;
}



void StudentWorld::makeValidPoint(int& x, int& y){ //initializing dirt
        x = randInt(0, VIEW_WIDTH); //Creates a random point
        y = randInt(0, VIEW_HEIGHT);
        while (!validDistance(x, y)){ //Sees if the point is valid distance away from center
            x = randInt(0, VIEW_WIDTH); //Makes new point if necessary
            y = randInt(0, VIEW_HEIGHT);
        }
}


int StudentWorld::move()
{
    ostringstream oss; //This is to output the top bar giving details about how your program works.
    if (getScore() >= 0)
        oss << "Score: " << setfill('0') << std::setw(6) << getScore() << "  "; //If score is positive
    else
        oss << "Score: -" << setfill('0') << std::setw(5) << getScore()*-1 << "  "; //if score is negative.
    
    oss << "Level: " << getLevel() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_player->getHP() << "  ";
    oss << "Sprays: " << m_player->nSCharges() << "  ";
    oss << "Flames: " << m_player->nFCharges();
    
    string gameStat = oss.str();
    setGameStatText(gameStat);
    for(auto it = m_actorLists.begin(); it != m_actorLists.end();)
    {
        if((*it)->getAlive()){ //check if alive
            (*it)->doSomething(); //make alive objects do something

            if (!(m_player->getAlive())){ //if player dies end game and decrement lives
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            if (finishedLevel()){ //if you finish the level Play that you finished the Level
                playSound(SOUND_FINISHED_LEVEL); //Play sound that you finished level
                return GWSTATUS_FINISHED_LEVEL; //return
            }
            it++; //increment to next object
        }
        else{
            delete *it; //if object isn't alive we deleted it
            it = m_actorLists.erase(it); //need to make sure we iterate through the list properly
        }
    }
    m_player->doSomething(); //Player does something as soon as all other objects have done something
    addGoodies(); //sees if any Goodies needed to be added
    
    return GWSTATUS_CONTINUE_GAME;
}



void StudentWorld::addGoodies(){
    int L = getLevel();
    const double PI = 4 * atan(1);
    //Need Pi constant and level to calculate the chances of a Fungus Spawning
    int ChanceFungus = max(510 - L * 10, 200);
    int FungusSpawn = randInt(0, ChanceFungus-1); //we don't want the top bracket to be inclusive
    if (FungusSpawn == 0){
        double angle = randInt(0,360) * PI* 2 * 1.0 / 360;
        int x = VIEW_RADIUS+VIEW_RADIUS * cos(angle);
        int y = VIEW_RADIUS+VIEW_RADIUS * sin(angle);
        Actor* fungus = new Fungus(this, x,y);
        m_actorLists.push_back(fungus);
    }

    int ChanceGoodie = max(510 - L * 10, 250);
    Actor* goodie = nullptr; //we need to allocate this outside, so if any of the Goodies do need to be created, we have a spot create the goodie
    int GoodieSpawn = randInt(0, ChanceGoodie-1); //we don't want the top bracket to be inclusive
    if (GoodieSpawn == 0){
        int goodietype = randInt(0,9);
        double angle = randInt(0,360) * PI* 2 * 1.0 / 360;
        int x = VIEW_RADIUS+VIEW_RADIUS * cos(angle);
        int y = VIEW_RADIUS+ VIEW_RADIUS * sin(angle);
        if (goodietype < 1){
            goodie = new ExtraLifeGoodie(this,x,y);
        }
        else if (goodietype < 4){
            goodie = new FlamethrowerGoodie(this, x, y);
        }
        else
            goodie = new RestoreHealthGoodie(this, x, y);
        m_actorLists.push_back(goodie); //if we are suppose to create a goodie, it will the goodie to the list.
    }
}


void StudentWorld::cleanUp()
{
    for (auto it = m_actorLists.begin(); it != m_actorLists.end();){ //erase all items in the list
        delete *it;
        it = m_actorLists.erase(it);
    }
    delete m_player; //delete player, so no memory leaks
    m_player = nullptr; //This ensures that we won't ever try to delete m_player with a bad address
    return;
}

bool StudentWorld::validDistance(const int& x, const int& y){ //check if this is 120^2 away from the center.
    int xFromCenter = (x-(VIEW_WIDTH/2));
    int yFromCenter = (y-(VIEW_HEIGHT/2));
    return (14400 > ((xFromCenter*xFromCenter) + (yFromCenter*yFromCenter)));
}


bool StudentWorld::finishedLevel(){ //sees if there are any Bacteria or Bacteria pits still alive
    for (auto it = m_actorLists.begin(); it != m_actorLists.end(); it++){
        if ((*it)->getEnemy() && (*it)->getAlive())
            return false;
    }
    return true;
}

void StudentWorld::fireFlame(int x, int y){ //Creates 16 new Flame objects that fire in 22 degree increments per spec
    const double PI = 4 * atan(1);
    for(int i = 0; i !=  16; i++){
        int angle = m_player->getAngle()-180 + 22*i;
        //makes sure that the flames will form a circle around Socrates
        double c = x+SPRITE_WIDTH*(cos(angle*1.0 / 360 * 2 * PI));
        double d = y+SPRITE_WIDTH*(sin(angle*1.0 / 360 * 2 * PI));
        Actor* flame = new Flame(this, c, d, angle);
        m_actorLists.push_back(flame);
    }
}


void StudentWorld::fireSpray(int x, int y){ //Spray goes same direction as Socrates
    const double PI = 4 * atan(1);
    int angle = m_player->getAngle() - 180; //needs to shoot same direction way.
    double c = x+SPRITE_WIDTH*(cos(angle*1.0 / 360 * 2 * PI));
    double d = y+SPRITE_WIDTH*(sin(angle*1.0 / 360 * 2 * PI));
    Actor* flame = new Spray(this, c, d, angle);
    m_actorLists.push_back(flame);
    
}

void StudentWorld::changeSocratesHP(int amount){
    if (m_player->getHP()+ amount >= m_player->getMaxHP()){ //makes sure HP will never go amount or below the MaxHP or 0
        int change = m_player->getMaxHP() - m_player->getHP();
        m_player->changeHP(change);
    }
    if (m_player->getHP()+ amount < 0)
        return;
    m_player->changeHP(amount);
    
}

int StudentWorld::SocratesHealth(){ //access the Socrates's HP
    return m_player->getHP();
    
}


void StudentWorld::addBacteria(int x, int y, int type){ //add Bacteria and each type has a number associated with it to make sure that bacteria is created
    if (type == 0){
        Actor* aSal = new ASalmonella(this, x, y);
        m_actorLists.push_back(aSal);

    }
    else if (type == 1){
        Actor* rSal = new RSalmonella(this, x, y);
        m_actorLists.push_back(rSal);
    }
    else{
        Actor* eColi = new EColi(this, x, y);
        m_actorLists.push_back(eColi);
    }
}



bool StudentWorld::isFood(Actor* bac){ //needed to check for Overlap and make code cleaner
    if (bac->getAlive() && !bac->getDamageable() && !bac->getEnemy() && !bac->getProj() && !bac->getGoodie()){
        return true;
    }
    return false;
}

bool StudentWorld::checkOverlapFood(Actor* bac){ //if the actor overlaps with the Bacteria it needs to delete itself
    for (auto it = m_actorLists.begin(); it != m_actorLists.end();){
        if (isFood(*it)){
            int c = (*it)->getX() - bac->getX();
            int d = (*it)->getY() - bac->getY();
            int e = c*c + d*d;
            if (e <= (SPRITE_WIDTH * SPRITE_WIDTH)){
                delete (*it);
                m_actorLists.erase(it);
                return true;
            }
            else
                it++; //increment to next object if no overlap
        }
        else
            it++; //increment to next element
    }
    return false;
            
}


bool StudentWorld::checkBactMovement(double x, double y){
for (auto it = m_actorLists.begin(); it != m_actorLists.end(); it++){
    if ((*it)->getBlocker()){ //Checks if Dirt stop bacteria from moving around
        int c = (*it)->getX() - x;
        int d = (*it)->getY() - y;
        int e = c*c + d*d;
        if (e <= (SPRITE_WIDTH/2 * SPRITE_WIDTH/2)){ //THIS IS TO CHECK WITH DIRT
            return false;
        }
    }
}
return true;
}


int StudentWorld::AngleBacteria(Actor* bac, int dis){
    double angle = randInt(0,359); //start with a random direction
    int distanceToBeat = dis; //sets the distance to beat
    for (auto it = m_actorLists.begin(); it != m_actorLists.end(); it++){
        if (!isFood(*it))
            continue;
        if (distance(bac->getX(), bac->getY(), (*it)->getX(), (*it)->getY()) <= distanceToBeat){
            distanceToBeat = distance((*it)->getX(), (*it)->getY(), bac->getX(), bac->getY()); // becomes the new distance to beat for other food
            double dy = (*it)->getY() - bac->getY();
            double dx = (*it)->getX() - bac->getX();
            angle = calcAngle(dy, dx);
        }
    }
    double ddx, ddy;
    int units = 3;
    bac->getPositionInThisDirection(angle, units, ddx, ddy);
    if (!checkBactMovement(ddx,ddy)){ //check if moving in this direction for the closest food is even possible to do without hitting dirt
        angle = randInt(0,359); //if not we just move in a random angle instead
    }
    return angle;
}


int StudentWorld::distance(int x1, int y1, int x2, int y2){ //distance formula
    return (sqrt((x1-x2)*(x1-x2)+ (y1-y2)*(y1-y2)));
}

bool StudentWorld::distancetoSocrates(Actor* bac, int dis){ //distance formula with socrates
    return distance(m_player->getX(), m_player->getY(), bac->getX(), bac->getY()) < dis;
    
}


int StudentWorld::angleToSocrates(Actor* bac){ //calculate angle to Socrates
    double dy = (m_player)->getY() - bac->getY();
    double dx = (m_player)->getX() - bac->getX();
    return calcAngle(dy,dx);
}

void StudentWorld::addFood(int x, int y){ //addFood is called by Bacteria Destructor and init
    Actor* food = new Food(this, x, y);
    m_actorLists.push_back(food);
}

int StudentWorld::calcAngle(double dy, double dx){
    const int PI = atan(1) * 4;
   int angle = (atan(dy/dx) * 180 / PI); //arctan is limited to (-pi/2, pi/2) or Quadrant 1,4
   if (dy < 0 && dx < 0)
       angle += 180; //this accounts for Quadrant 3
   if (dy > 0 && dx < 0)
       angle +=180; //This account for Quadrant 2
   return angle;
}
