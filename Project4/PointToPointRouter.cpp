#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <utility>
#include <vector>
#include <set>
#include <queue>
#include <map>
using namespace std;

unsigned int hasher(const StreetSegment& key){
    return std::hash<std::string>()(key.name);
}

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    struct Node{
        Node(GeoCoord point, double g1, double h2){
            m_coord = point;
            g = g1;
            h = h2;
            f = g1+h2;
        }
        bool operator< (const Node& other){
            return f > other.f;
        }
        GeoCoord m_coord;
        double g;
        double h;
        double f;
        GeoCoord m_parent;
    };
    class myComparator{
    public:
        int operator() (const Node& gc1, const Node& gc2){
        return gc1.f > gc2.f;
        }
    };
    const StreetMap* m_streetdata;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_streetdata = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    vector<StreetSegment> temp;
    if (!m_streetdata->getSegmentsThatStartWith(start, temp)){
        return BAD_COORD;
    }
    if (start == end){
        route.clear();
        totalDistanceTravelled = 0;
        return DELIVERY_SUCCESS;
    }
    priority_queue<Node, vector<Node>, myComparator> openHeap;
    ExpandableHashMap<GeoCoord, double> closedList;
    map<GeoCoord, double> openList;
    map<GeoCoord, StreetSegment> pathway;
    
    openHeap.push(Node(start, 0, distanceEarthMiles(start, end)));
    openList.insert(pair<GeoCoord, double> (start, distanceEarthMiles(start, end)));
    
    while(!openHeap.empty()){
        Node current = openHeap.top();
        openHeap.pop();
        auto it = openList.find(current.m_coord);
        if (it != openList.end())
            openList.erase(it);
        temp.clear();
        m_streetdata->getSegmentsThatStartWith(current.m_coord, temp);
        for(int i = 0; i != temp.size(); i++){
            if (temp[i].end == end){
                totalDistanceTravelled = current.g + distanceEarthMiles(temp[i].start, temp[i].end);
                route.push_back(temp[i]);
                GeoCoord* point = &current.m_coord;
                while (*point != start){
                    auto it = pathway.find(*point);
                    point = &it->second.start;
                    route.push_back(it->second);
                }
                
                
                route.reverse();
                return DELIVERY_SUCCESS;
            }
            double g = current.g + distanceEarthMiles(temp[i].start, temp[i].end);
            double h = distanceEarthMiles(temp[i].end, end);
            double f = g + h;
            auto openit = openList.find(temp[i].end);
            auto closedit = closedList.find(temp[i].end);
            if (openit != openList.end() && openit->second < f){
                continue;
            }
            else if (closedit != nullptr && *closedit < f){
                continue;
            }
            else{
                openHeap.push(Node(temp[i].end, g, h));
                openList.insert(pair<GeoCoord, double> (temp[i].end, f));
                pathway.insert(pair<GeoCoord,StreetSegment>(temp[i].end, temp[i]));
            }
        }
        closedList.associate(current.m_coord, current.f);
    }
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

/*
int main(){
      StreetMap* test = new StreetMap();
      test->load("/Users/rishab/Desktop/Project4Final/Project4Final/mapdata.txt");
      PointToPointRouter test2(test);
    list<StreetSegment> route;
    double total = 0;
    DeliveryResult x = test2.generatePointToPointRoute(GeoCoord("34.0625329", "-118.4470263"), GeoCoord( "34.0475778" ,"-118.4607909"), route, total);
    cout << "No issues" << endl;
    cout << total << endl;
    cout << route.size();
    for (auto it = route.begin(); it != route.end(); it++){
        if (it == route.begin())
            cout << it->start.latitudeText << ' ' << it->start.longitudeText << endl;
        cout << it->end.latitudeText << ' ' << it->end.longitudeText << endl;
    
    }
    
        
        
        
}

*/
