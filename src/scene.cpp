//
//  scene.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "scene.h"


scene::scene(){

    string fileNames[6] ={"indeterminacy.wav", "menAreMen.wav", "mindCanChange.wav", "noUse.wav", "purposeful.wav", "spring.wav"};
    
    for(int i = 0; i < 6; i ++){
        
        ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone());
        triggerZones.push_back(t);
        t->setSoundFile("sound/" + fileNames[i]);
        
    }
    
}


void scene::draw(){

    vector< ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
    
        (*it)->draw();
        
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

ofPtr<triggerZone> scene::getTriggerZone(int tz){return triggerZones[tz];}

