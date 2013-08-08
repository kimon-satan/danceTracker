//
//  scene.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "scene.h"

int scene::index = 0;

scene::scene(ofPtr<oscManager> o): mOsc(o){
    
    mName = "emptyScene_" + ofToString(index,0);
    
    mIndex = index;
    index += 1;
    
    
}


void scene::draw(ofVec3f camPos){

    vector< ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
    
        (*it)->draw(camPos);
        
    }
    

}

void scene::update(ofVec3f com, float userHeight, vector<ofVec3f> & pc){

    vector< ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
        
        if((*it)->getIsEnabled()){
            
            if((*it)->checkInRange(com, userHeight))(*it)->checkPoints(pc);
            (*it)->update();
            
        }
        
    }
    

}

void scene::deselectAll(){

    vector< ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
     
        (*it)->deselect();
    }
}

ofPtr<triggerZone> scene::getTriggerZone(int tz){return triggerZones[tz];}
int scene::getNumTriggerZones(){return triggerZones.size();}

ofPtr<triggerZone> scene::addTriggerZone(int tz){
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(mOsc));
    if(triggerZones.size() > 0){
        triggerZones.insert(triggerZones.begin() + tz + 1, t);
    }else{
        triggerZones.push_back(t);
    }
    
    return t;
}

void scene::removeTriggerZone(int tz){

    mOsc->removeZone(triggerZones[tz]->getIndex());
    triggerZones.erase(triggerZones.begin() + tz);
    
}


void scene::setName(string s){

    mName = s;
}

string scene::getName(){

    return mName;
}


void scene::setIndex(int i){
    
    if(i < 0)return; // -1 signifies no prev index recorded
    mIndex = i;
    index = i + 1;
}

int scene::getIndex(){return mIndex;}