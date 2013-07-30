//
//  scene.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "scene.h"


scene::scene(){

    
    for(int i = 0; i < 6; i ++){
        
        triggerZone t;
        triggerZones.push_back(t);
        
    }
    
}


void scene::draw(){

    vector<triggerZone>::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
    
        it->draw();
        
    }
    

}

void scene::update(ofVec3f com, float userHeight, vector<ofVec3f> & pc){

    vector<triggerZone>::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
        
        if(it->getIsEnabled()){
            
            if(it->checkInRange(com, userHeight))it->checkPoints(pc);
            it->update();
            
        }
        
    }
    

}


ofVec3f scene::getTZPos(int tz){ return triggerZones[tz].getPos();}
float scene::getTZRadius(int tz){ return triggerZones[tz].getRadius();}

void scene::setTZRadius(int tz, float r){ triggerZones[tz].setRadius(r);}
void scene::setTZPosX(int tz, float x){ triggerZones[tz].setPosX(x);}
void scene::setTZPosY(int tz, float y){triggerZones[tz].setPosY(y);}
void scene::setTZPosZ(int tz, float z){triggerZones[tz].setPosZ(z);}

void scene::setTZEnabled(int tz, bool b){triggerZones[tz].setIsEnabled(b);}
bool scene::getTZEnabled(int tz){return triggerZones[tz].getIsEnabled();}