//
//  ExpandableHashMap.h
//  Project4Final
//
//  Created by Rishab Jain on 3/4/20.
//  Copyright © 2020 Rishab Jain. All rights reserved.
//

#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h
#include <list>
#include <vector>
#include <utility>


// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    double m_loadfactor;
    typename std::list<std::pair<KeyType,ValueType>>* m_hashmap;
    int m_size;
    int m_associations;
    
    
    //private member functions
    void insertOrUpdate(const KeyType& key, const ValueType& value);
    void rehash();
    
    
    
};


template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    if (maximumLoadFactor < 0)
        m_loadfactor = 0.5;
    else
        m_loadfactor = maximumLoadFactor;
    m_size = 8;
    m_associations = 0;
    m_hashmap = new std::list<std::pair<KeyType,ValueType>>[m_size];
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    reset();
    delete [] m_hashmap;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    for (int i = 0; i != m_size; i++){
        for (auto it2 = (m_hashmap[i]).begin(); it2 != (m_hashmap[i]).end();){
            it2 = (m_hashmap[i]).erase(it2);
        }
    }
    
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_associations;  // Delete this line and implement this function correctly
}



template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    double cLoadfactor = m_associations;
    if (find(key) == nullptr)
        cLoadfactor++;
    cLoadfactor = cLoadfactor/m_size;
    if (m_loadfactor <= cLoadfactor){
        rehash();
    }
    insertOrUpdate(key, value);
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const{
    unsigned int hasher(const KeyType& key);
    unsigned int x = hasher(key); //get the integer equivalent
    x = x % m_size;
    const auto it = m_hashmap[x];
    for (auto it2 = (m_hashmap[x]).begin(); it2 != (m_hashmap[x]).end(); it2++){
        if (it2->first == key){
            const ValueType* val = &(it2->second);
            return val;
        }
    }
    return nullptr;  // Delete this line and implement this function correctly
}


template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::insertOrUpdate(const KeyType& key, const ValueType& value){
    unsigned int hasher(const KeyType& key);
    unsigned int x = hasher(key); //get the integer equivalent
    x = x % m_size;
    if (find(key) == nullptr){
        m_hashmap[x].push_back(std::pair<KeyType, ValueType>(key,value));
        m_associations++;
    }
    else
    {
        auto it = m_hashmap[x];
        for (auto it2 = (m_hashmap[x]).begin(); it2 != (m_hashmap[x]).end(); it2++){
            if (it2->first == key){
                it2->second = value;
                return;
            }
        }
    }
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::rehash(){
    //find each element and put into container(MAKE SURE TO ERASE Each element)
    typename std::list<std::pair<KeyType, ValueType>> temp;
    for (int i = 0; i != m_size;i++){
        if (m_associations == 0)
            break;
        for (auto it = m_hashmap[i].begin();it != m_hashmap[i].end();it++){
            temp.push_back(*it);
            auto toBeDeleted = it;
            it--;
            m_hashmap[i].erase(toBeDeleted);
            m_associations--;
        }
    }
    delete [] m_hashmap;
    m_size *= 2;
    m_hashmap = new std::list<std::pair<KeyType,ValueType>>[m_size];
    //Rehash all items and return
    for (auto it = temp.begin(); it != temp.end();){
        KeyType key = (*it).first;
        ValueType val = (*it).second;
        insertOrUpdate(key, val);
        it = temp.erase(it);
    }
}

#endif /* ExpandableHashMap_h */
