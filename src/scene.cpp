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

    map< string, ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
    
        (*it).second->draw(camPos);
        
    }
    

}

void scene::update(ofVec3f com, float userHeight, vector<ofVec3f> & pc){

    map<string, ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
        
        if((*it).second->getIsEnabled()){
            
            if((*it).second->checkInRange(com, userHeight))(*it).second->checkPoints(pc);
            (*it).second->update();
            
        }
        
    }
    

}

void scene::deselectAll(){

    map< string, ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
     
        (*it).second->deselect();
    }
}

ofPtr<triggerZone> scene::getTriggerZone(string tz){return triggerZones[tz];} 

ofPtr<triggerZone> scene::getNextTriggerZone(ofPtr<triggerZone> tz){
    
    map< string, ofPtr<triggerZone> >::iterator it = triggerZones.find(tz->getUid());
    it++;
    if(it == triggerZones.end())it = triggerZones.begin();
    return (*it).second;
    
    
}

ofPtr<triggerZone> scene::getFirstTriggerZone(){
    return (*triggerZones.begin()).second;
}


map < string, ofPtr<triggerZone> > scene::getTriggerZones(){ return triggerZones;}
int scene::getNumTriggerZones(){return triggerZones.size();}

ofPtr<triggerZone> scene::addTriggerZone(){
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(mOsc));
 
        triggerZones[t->getUid()] = t;
        //addZone to SC
        mOsc->addZone(t->getIndex(), t->getName()); //replace index
        t->updateAllAudio();
  
    
    return t;
}

ofPtr<triggerZone> scene::copyTriggerZone(string tz){
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*triggerZones[tz]));
    t->setName(t->getName() + "_copy");
    t->newIndex();
    mOsc->addZone(t->getIndex(), t->getName());
    t->reloadSound();
  
    triggerZones[t->getUid()] = t;
  
    return t;
}

void scene::removeTriggerZone(string tz){

    mOsc->removeZone(triggerZones[tz]->getIndex());
    triggerZones.erase(tz);
    
}


void scene::deepCopyTriggerZones(){

    //make a deep copy of the trigger zones ... for when copying a scene
    map <string, ofPtr<triggerZone> > ttz;
    
    for(int i = 0; i < triggerZones.size(); i++){
        ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*triggerZones["index"]));
        t->newIndex();
        mOsc->addZone(t->getIndex(), t->getName());
        t->reloadSound();
        ttz["index"] = t;
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