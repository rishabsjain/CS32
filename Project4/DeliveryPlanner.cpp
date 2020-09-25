#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    string angleConverter(const double& angle) const;
    const StreetMap* m_street;
    const PointToPointRouter* m_routeMaker;
    const DeliveryOptimizer* m_optimizer;
};

string DeliveryPlannerImpl::angleConverter(const double& angle) const{
    if (angle >= 0 && angle < 22.5)
        return "east";
    else if (angle >= 22.5 && angle < 67.5)
        return "northeast";
    else if (angle >= 67.5 && angle < 112.5)
        return "north";
    else if (angle >= 112.5 && angle < 157.5)
        return "northwest";
    else if (angle >= 157.5 && angle < 202.5)
        return "west";
    else if (angle >= 202.5 && angle < 247.5)
        return "southwest";
    else if (angle >= 247.5 && angle < 292.5)
        return "south";
    else if (angle >= 292.5 && angle < 337.5)
        return "southeast";
    else
        return "east";
}

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_street = sm;
    m_routeMaker = new PointToPointRouter(sm);
    m_optimizer = new DeliveryOptimizer(sm);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
    delete m_routeMaker;
    delete m_optimizer;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    double oldDistance;
    double newDistance;
    vector<DeliveryRequest> optimized = deliveries;
    m_optimizer->optimizeDeliveryOrder(depot, optimized, oldDistance, newDistance);
    //Optimizer code
    list<StreetSegment> route;
    double totalD = 0;
    GeoCoord location = depot;
    if (deliveries.size() == 0){
        return DELIVERY_SUCCESS;
    }
    
    for (int i = 0; i != optimized.size()+1; i++){
        //change from depot to last location
        route.clear();
        DeliveryResult result;
        if (i == optimized.size()){
            result = m_routeMaker->generatePointToPointRoute(location, depot, route, totalDistanceTravelled);
        }
        else{
            result = m_routeMaker->generatePointToPointRoute(location, optimized[i].location, route, totalDistanceTravelled);
        }
        if (result == DELIVERY_SUCCESS){
            totalD += totalDistanceTravelled;
            auto it = route.begin();
            auto next = it;
            next++;
            while (it != route.end() && next != route.end()){
                double angleb2 = angleBetween2Lines(*it, *next);
                double angle = angleOfLine(*it);
                if (it->name == next->name){
                    DeliveryCommand x;
                    double distance = 0;
                    bool incremented = false;
                    while (it != route.end() && next!= route.end() && it->name == next->name){
                        distance += distanceEarthMiles(it->start, it->end);
                        incremented = true;
                        it++;
                        next++;
                    }
                    distance += distanceEarthMiles(it->start, it->end);
                    if (!incremented){
                        it++;
                        next++;
                    }
                        
                    string s = angleConverter(angle);
                    x.initAsProceedCommand(s, it->name, distance);
                    if (distance != 0.00)
                        commands.push_back(x);
                    if (it == route.end())
                        break;
                }
                else if (angleb2 >= 1 && angleb2 < 180){
                    DeliveryCommand x;
                    x.initAsTurnCommand("left", next->name);
                    commands.push_back(x);
                    it++;
                    next++;
                    if (it != route.end() && next != route.end() && it->name != next->name){
                        string s = angleConverter(angle);
                        DeliveryCommand p;
                        p.initAsProceedCommand(s, it->name, distanceEarthMiles(it->start, it->end));
                        commands.push_back(p);
                    }
                }
                else if(angleb2 >= 180 && angleb2 <= 359){
                    DeliveryCommand x;
                    x.initAsTurnCommand("right", next->name);
                    commands.push_back(x);
                    it++;
                    next++;
                    if (it != route.end() && next != route.end() && it->name != next->name){
                        string s = angleConverter(angle);
                        DeliveryCommand p;
                        p.initAsProceedCommand(s, it->name, distanceEarthMiles(it->start, it->end));
                        commands.push_back(p);
                    }
                }
                else{
                    it++;
                    next++;
                }
            }
            DeliveryCommand x;
            if (i != optimized.size()){
            x.initAsDeliverCommand(optimized[i].item);
            commands.push_back(x);
            location = optimized[i].location;
            }
            
        }
        else if (result == BAD_COORD){
            return BAD_COORD;
        }
        else
            return NO_ROUTE;
    }
    totalDistanceTravelled = totalD;
    return DELIVERY_SUCCESS;  // Delete this line and implement this function correctly
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}



