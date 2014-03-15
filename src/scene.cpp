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
    
    newIndex();
    
    fadeIn = 0.01;
    fadeOut = 0.01;
    
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
        //addZone to SC
        mOsc->addZone(t->getIndex(), t->getName());
        t->updateAllAudio();
    }else{
        triggerZones.push_back(t);
         mOsc->addZone(t->getIndex(), t->getName());
        t->updateAllAudio();
    }
    
    return t;
}

ofPtr<triggerZone> scene::copyTriggerZone(int tz){
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*triggerZones[tz]));
    t->setName(t->getName() + "_copy");
    t->newIndex();
    mOsc->addZone(t->getIndex(), t->getName());
    t->reloadSound();
  
    triggerZones.insert(triggerZones.begin() + tz + 1, t);
  
    return t;
}

void scene::removeTriggerZone(int tz){

    mOsc->removeZone(triggerZones[tz]->getIndex());
    triggerZones.erase(triggerZones.begin() + tz);
    
}


void scene::deepCopyTriggerZones(){

    //make a deep copy of the trigger zones ... for when copying a scene
    vector<ofPtr<triggerZone> > ttz;
    
    for(int i = 0; i < triggerZones.size(); i++){
         ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*triggerZones[i]));
        t->newIndex();
        mOsc->addZone(t->getIndex(), t->getName());
        t->reloadSound();
        ttz.push_back(t);
    }
    
    triggerZones.clear();
    triggerZones = ttz;
    

}

void scene::setName(string s){

    mName = s;
}

string scene::getName(){

    return mName;
}


void scene::setFadeIn(float f){

    fadeIn = f;
}

float scene::getFadeIn(){

    return fadeIn;
}

void scene::setFadeOut(float f){ fadeOut = f;}
float scene::getFadeOut(){return fadeOut;}


void scene::setIndex(int i){
    
    mIndex = i;
    
}


void scene::newIndex(){

    mIndex = index;
    index += 1;
}


int scene::getIndex(){return mIndex;}

void scene::setStaticIndex(int i){

    index = i;

}