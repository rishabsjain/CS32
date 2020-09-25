#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <ostream>
#include <iostream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}



class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap <GeoCoord, vector<StreetSegment>> m_map;
};

StreetMapImpl::StreetMapImpl():m_map()
{
    
}

StreetMapImpl::~StreetMapImpl()
{
    
    
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);    // infile is a name of our choosing
    if (!infile)                // Did opening the file fail?
    {
        cerr << "Error: Cannot open data.txt!" << endl;
        return false;
    }
    string s;
    while (getline(infile, s)){
        int x;
        infile >> x;
        infile.ignore(10000, '\n');
        for (int i = 0; i != x; i++){
            string y; string z;
            infile >> y;
            infile >> z;
            GeoCoord start(y, z);
            
            infile >> y;
            infile >> z;
            GeoCoord end(y, z);
            
            StreetSegment segment1(start, end, s);
            StreetSegment segment2(end, start, s);
            vector<StreetSegment>* s1 = m_map.find(start);
            if (s1 == nullptr)
                s1 = new vector<StreetSegment>;
            s1->push_back(segment1);
            m_map.associate(start, *s1);
            vector<StreetSegment>* q = m_map.find(end);
            if (q == nullptr)
                q = new vector<StreetSegment>;
            q->push_back(segment2);
            m_map.associate(end, *q);
            
        }
        infile.ignore(10000, '\n');
        
    }
    return true;  // Delete this line and implement this function correctly
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    if (m_map.find(gc) == nullptr)
        return false;
    const vector<StreetSegment> temp = *(m_map.find(gc));
    segs.clear();
    for (auto it = temp.begin(); it != temp.end(); it++){
        StreetSegment stay (it->start, it->end, it->name);
        segs.push_back(stay);
    }
        
    
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}

/*
int main(){
    StreetMapImpl test;
    test.load("/Users/rishab/Desktop/Project4Final/Project4Final/mapdata.txt");
    vector<StreetSegment> sup;
    sup.clear();
    cout << "All worked" << endl;
    test.getSegmentsThatStartWith(GeoCoord("34.0732851", "-118.4931016"), sup);
    for (int i =0; i != sup.size(); i++){
        cout << sup[i].name << endl;
    }
    
}

*/
