//
//  scene.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "scene.h"


scene::scene(ofPtr<oscManager> o): mOsc(o){
    
    newIndex();
    mName = "scene_" + u_id;
    
    
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
    
    if(!tz){
        return tz; //return the empty pointer
    }else{
        map< string, ofPtr<triggerZone> >::iterator it = triggerZones.find(tz->getUid());
        it++;
        if(it == triggerZones.end())it = triggerZones.begin();
        return (*it).second;
    }
    
}

ofPtr<triggerZone> scene::getPrevTriggerZone(ofPtr<triggerZone> tz){
    
    if(!tz){
        return tz; //return the empty pointer
    }else{
        
        map< string, ofPtr<triggerZone> >::iterator it = triggerZones.find(tz->getUid());
        if(it == triggerZones.begin())it = triggerZones.end();
        it--;
        
        return (*it).second;
    }
    
}

ofPtr<triggerZone> scene::getFirstTriggerZone(){
    ofPtr<triggerZone> tz;
    if(triggerZones.size() == 0)return tz; //an empty pointer
    return (*triggerZones.begin()).second;
}


map < string, ofPtr<triggerZone> > scene::getTriggerZones(){ return triggerZones;}
int scene::getNumTriggerZones(){return triggerZones.size();}

ofPtr<triggerZone> scene::addTriggerZone(){
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(mOsc));
    checkUniqueId(t);
    triggerZones[t->getUid()] = t;
    
    //addZone to SC
    mOsc->addZone(t->getUid(), t->getName());
    t->updateAllAudio();
    
    return t;
}

ofPtr<triggerZone> scene::copyTriggerZone(string tz){ //TODO for consistency this could be done with ptr
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*triggerZones[tz]));
    t->newIndex();
    checkUniqueId(t);
    
    string tn = t->getName();
    if(tn.substr(tn.length() - 5, 5) != "_copy")tn += "_copy";
    
    t->setName(tn);
    mOsc->addZone(t->getUid(), t->getName());
    t->reloadSound();
  
    triggerZones[t->getUid()] = t;
  
    return t;
    
}

void scene::removeTriggerZone(string tz){ //TODO for consistency this could be done with ptr

    mOsc->removeZone(triggerZones[tz]->getUid());
    triggerZones.erase(tz);
    
}


void scene::deepCopyTriggerZones(){

    //make a deep copy of the trigger zones ... for when copying a scene
    map <string, ofPtr<triggerZone> > ttz;
    
    for(int i = 0; i < triggerZones.size(); i++){
        ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*triggerZones["index"]));
        t->newIndex();
        checkUniqueId(t);
        mOsc->addZone(t->getUid(), t->getName());
        t->reloadSound();
        ttz["index"] = t;
    }
    
    triggerZones.clear();
    triggerZones = ttz;
    

}

void scene::checkUniqueId(ofPtr<triggerZone> tz){
    
    bool isUnique = false;
    
    while (!isUnique) {
        if(triggerZones.find(tz->getUid()) != triggerZones.end()){
            tz->newIndex();
        }else{
            isUnique = true;
        }
    }
    
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



void scene::newIndex(){

    u_id = dt_utils::getRandom(10);
}


string scene::getUid(){return u_id;}

