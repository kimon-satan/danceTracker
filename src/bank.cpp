//
//  bank.cpp
//  danceTracker
//
//  Created by Simon Katan on 07/08/2013.
//
//

#include "bank.h"

bank::bank(){

    newIndex();
    name = "bank_" + u_id;
    selSceneIndex = 0;

}

void bank::newIndex(){
    
    u_id = dt_utils::getRandom(10);
}


void bank::nextScene(){
    
    if(scenes.size() <= 1)return;
    selSceneIndex = min(selSceneIndex + 1, (int)scenes.size() - 1);
    selScene = scenes[selSceneIndex];

}

void bank::prevScene(){

    if(scenes.size() <= 1)return;
    selSceneIndex = max(selSceneIndex - 1, 0);
    selScene = scenes[selSceneIndex];

}

void bank::firstScene(){
    
    if(scenes.size() == 0){
        selScene.reset();
    }else{
        selScene = scenes[0];
        selSceneIndex = 0;
    }
    
    
}


void bank::insertScene(ofPtr<scene> sn){
    
    if(scenes.size() == 0){
        scenes.push_back(sn);
        selScene = sn;
        return;
    }
    
    
    scenes.insert(scenes.begin() + selSceneIndex + 1, sn);
    
    selScene = sn;
    selSceneIndex += 1;
    
}


void bank::removeScene(){
    
    if(scenes.size() == 0)return;
    scenes.erase(scenes.begin() + selSceneIndex);
    selSceneIndex = max(0, selSceneIndex - 1);
    
    if(scenes.size() == 0){
        selScene.reset();
    }else{
        selScene= scenes[selSceneIndex];
    }
    
}