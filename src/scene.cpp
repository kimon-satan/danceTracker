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

    vector<ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
    
        (*it)->draw(camPos);
        
    }
    

}

void scene::update(ofPtr<dancer> d){

    vector<ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
        
        if((*it)->getIsEnabled()){
            
            if((*it)->checkInRange(d))(*it)->checkPoints(d);
            (*it)->update(d);
            
        }
        
    }
    

}

void scene::deselectAll(){

    vector< ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
     
        (*it)->setIsSelected(false);
    }
}

void scene::unTriggerAll(){
    
    vector< ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
        
        (*it)->deselect();
        mOsc->resetZone((*it)->getUid());
    }
    
}

ofPtr<triggerZone> scene::getNextTriggerZone(ofPtr<triggerZone> tz, bool isWrap){
    
    if(!tz){
        return tz; //return the empty pointer
    }else{
        vector< ofPtr<triggerZone> >::iterator it = find(triggerZones.begin(), triggerZones.end(), tz);
        it++;
        if(!isWrap){
            if(it == triggerZones.end())it--;
        }else{
            if(it == triggerZones.end())it = triggerZones.begin();
        }
        return (*it);
    }
    
}

ofPtr<triggerZone> scene::getPrevTriggerZone(ofPtr<triggerZone> tz , bool isWrap){
    
    if(!tz){
        return tz; //return the empty pointer
    }else{
        
        vector< ofPtr<triggerZone> >::iterator it = find(triggerZones.begin(), triggerZones.end(), tz);
        if(!isWrap){
            if(it != triggerZones.begin())it--;
        }else{
            if(it == triggerZones.begin())it = triggerZones.end();
            it--;
        }
        return (*it);
    }
    
}

ofPtr<triggerZone> scene::getFirstTriggerZone(){
    ofPtr<triggerZone> tz;
    if(triggerZones.size() == 0)return tz; //an empty pointer
    return (*triggerZones.begin());
}


vector < ofPtr<triggerZone> > scene::getTriggerZones(){ return triggerZones;}
int scene::getNumTriggerZones(){return triggerZones.size();}

ofPtr<triggerZone> scene::addTriggerZone(ofPtr<triggerZone> tz){
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(mOsc));
    checkUniqueId(t);
    if(tz){
        triggerZones.insert(getInsertIt(tz), t);
    }else{
        triggerZones.push_back(t);
    }
    
    //addZone to SC
    mOsc->addZone(t->getUid(), t->getName());
    t->updateAllAudio();
    
    return t;
}

ofPtr<triggerZone> scene::copyTriggerZone(ofPtr<triggerZone> tz){ 
    
    ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*tz));
    t->newIndex();
    checkUniqueId(t);
    
    string tn = t->getName();
    if(tn.length() > 5){
        //otherwise it will crash
       if(tn.substr(tn.length() - 5, 5) != "_copy")tn += "_copy"; 
    }else{
        //it can't already have copy appended
        tn += "_copy"; 
    }
    
    t->setName(tn);
    mOsc->addZone(t->getUid(), t->getName());
    t->reloadSound(); //needs looking at
  
    triggerZones.insert(getInsertIt(tz), t);
  
    return t;
    
}

void scene::removeTriggerZone(ofPtr<triggerZone> tz){

    if(!tz)return;
    mOsc->removeZone(tz->getUid());
    triggerZones.erase(find(triggerZones.begin(), triggerZones.end(), tz));
    
}


void scene::deepCopyTriggerZones(){

    //make a deep copy of the trigger zones ... for when copying a scene
    vector <ofPtr<triggerZone> > ttz;
    vector <ofPtr<triggerZone> >::iterator it;
    
    for(it = triggerZones.begin(); it != triggerZones.end(); it++){
        ofPtr<triggerZone> tz = (*it);
        ofPtr <triggerZone> t = ofPtr<triggerZone>(new triggerZone(*tz));
        t->newIndex();
        checkUniqueId(t);
        mOsc->addZone(t->getUid(), t->getName());
        t->reloadSound();
        ttz.push_back(t); //no need for insert here
    }
    
    triggerZones.clear();
    triggerZones = ttz;
    

}

void scene::checkUniqueId(ofPtr<triggerZone> tz){
    
    bool isUnique = false;
    
    while (!isUnique) {
        
        bool isFound = false;
        vector<ofPtr<triggerZone> >::iterator it = triggerZones.begin();
        for(it = triggerZones.begin(); it != triggerZones.end(); it++){
            if(tz->getUid() == (*it)->getUid())isFound = true;
        }
        
        if(isFound){
            tz->newIndex();
        }else{
            isUnique = true;
        }
    }
    
   
    
}

vector<ofPtr<triggerZone> >::iterator scene::getInsertIt(ofPtr<triggerZone> tz){
    
    vector<ofPtr<triggerZone> >::iterator it;
    if(triggerZones.size() == 1)
        return triggerZones.end();
    else if(!tz)
        return triggerZones.end();
    else
        it = find(triggerZones.begin(), triggerZones.end(), tz);
    it ++;
    return it;
    
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
void scene::setUid(string s){u_id = s;}

