#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    double calcDistance(const GeoCoord& start,const vector<DeliveryRequest>& Nodes) const;
    void bestNeighbor(GeoCoord start, vector<DeliveryRequest>& nodes, int index) const;
};

void DeliveryOptimizerImpl::bestNeighbor(GeoCoord start, vector<DeliveryRequest>& nodes, int index) const{
    double best = distanceEarthMiles(start, nodes[index].location);
    int bestindex = index;
    double current = distanceEarthMiles(start, nodes[index].location);
    for (int i = index; i != nodes.size(); i++){
        current = distanceEarthMiles(start, nodes[i].location);
        if (current < best){
            best = current;
            bestindex = i;
        }
    }
    if (best < distanceEarthMiles(start, nodes[index].location)){
        DeliveryRequest s = nodes[index];
        nodes[index] = nodes[bestindex];
        nodes[bestindex] = s;
    }
}

double DeliveryOptimizerImpl::calcDistance(const GeoCoord& start,const vector<DeliveryRequest>& Nodes) const{
    double x = 0;
    GeoCoord depot = start;
    for (int i = 0; i != Nodes.size(); i++){
        x += distanceEarthMiles(depot, Nodes[i].location);
        depot = Nodes[i].location;
    }
    x += distanceEarthMiles(depot, start);
    return x;
    
}

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = calcDistance(depot, deliveries);
    if (deliveries.size() < 2){
        newCrowDistance = oldCrowDistance;
        cout << oldCrowDistance << 'v' << newCrowDistance << endl;
        return;
    }
    if (deliveries.size() == 2){
        int x1 = distanceEarthMiles(depot, deliveries[1].location);
        int x2 = distanceEarthMiles(depot, deliveries[0].location);
        if (x1 > x2){
            DeliveryRequest temp = deliveries[0];
            deliveries[0] = deliveries[1];
            deliveries[1] = temp;
            
        }
        newCrowDistance = oldCrowDistance;
        cout << oldCrowDistance << 'v' << newCrowDistance << endl;
        return;
    }
    vector<DeliveryRequest> bestway = deliveries;
    GeoCoord start = depot;
    for(int i = 0; i != bestway.size(); i++){
        bestNeighbor(start, bestway, i);
        start = bestway[i].location;
    }
    
    double bestneighbor = calcDistance(depot, bestway);
    oldCrowDistance = calcDistance(depot, deliveries);
    vector<DeliveryRequest> best = deliveries; //create a vector to keep the best deliveries
    vector<DeliveryRequest> current = best; //create a vector to look at the deliveries we are using
    double bestDistance = oldCrowDistance; //stores best distance we have
    double lookAt = bestDistance; //stores distance we are considering
    if (bestneighbor < oldCrowDistance){
        best = bestway;
        current = bestway;
        bestDistance = bestneighbor;
        lookAt = bestneighbor;
    }
    double probability = 1.00; //starting probability
    double changeinProb = 0.001;
    while (probability > 0.00){
        int x = rand() % deliveries.size(); //random number that describes an index
        int y = rand() % deliveries.size();
        while (y == x){
            y = rand() % deliveries.size(); //keep finding new random number if same as x
        }
        
        DeliveryRequest temp = current[x];
        current[x] = current[y];
        current[y] = temp; //swap two index
        double newD = calcDistance(depot, current); //calculate distance based on this

        if (lookAt > newD){
            
            if (bestDistance > newD){
                changeinProb *= (1.000 + changeinProb);
                bestDistance = newD;
                best = current;
            }
            probability -= changeinProb;
            lookAt = newD;
        }
        else if (probability > 0.00){
            probability -= changeinProb;
            lookAt = newD;
        }
        else{
            current = deliveries;
            probability -= changeinProb;
        }
    }
    
    deliveries = best;
    newCrowDistance = bestDistance;
    cout << oldCrowDistance << 'v' << bestneighbor << endl;
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
/*
int main(){
    StreetMap x;
        x.load("/Users/rishab/Desktop/Project4Final/Project4Final/mapdata.txt");
        DeliveryOptimizer optimusPrime(&x);
        DeliveryRequest first("a", GeoCoord("34.0712323", "-118.4505969"));
            DeliveryRequest second("b", GeoCoord("34.0616291", "-118.4416199"));
            DeliveryRequest third("c", GeoCoord("34.0685657", "-118.4489289"));
            DeliveryRequest fourth("d", GeoCoord("34.0687443","-118.4449195"));
            DeliveryRequest fifth("e", GeoCoord("34.0636860", "-118.4453568"));
            DeliveryRequest sixth("f", GeoCoord("34.0564109", "-118.4805269"));
            DeliveryRequest seventh("g", GeoCoord("34.0592030", "-118.4756143"));
            vector<DeliveryRequest> deliveries;
            deliveries.push_back(first);
            deliveries.push_back(second);
            deliveries.push_back(third);
            deliveries.push_back(fourth);
            deliveries.push_back(fifth);
            deliveries.push_back(sixth);
            deliveries.push_back(seventh);
        double old;
        double better;
        optimusPrime.optimizeDeliveryOrder(GeoCoord("34.0671816", "-118.3975476"), deliveries, old, better);
            cerr << "\nold: " << old << endl;
            cerr << "new: " << better << endl;
//        cerr << "deliveries change check(distance): " << getOrderTravelDistance(GeoCoord("34.0671816", "-118.3975476"), deliveries) << endl;
        cerr << "NewDeliveryOrder: " << endl;
        for(int i = 0; i < deliveries.size(); i++)
            cerr << deliveries[i].item << endl;
}
*/
