//
//  bankManager.cpp
//  danceTracker
//
//  Created by Simon Katan on 23/03/2014.
//
//

#include "bankManager.h"


bankManager::bankManager(ofPtr<oscManager> t){

    pOsc = t;
    ofPtr<scene>  s = ofPtr<scene>(new scene(pOsc));
    allScenes.push_back(s);
    currentScene = s;
    
    ofPtr<bank> b = ofPtr<bank>(new bank());
    allBanks.push_back(b);
    currentBank = b;


}

void bankManager::saveSettings(ofxXmlSettings XML){

    saveScenes(XML);
    saveBanks(XML);
   
}



void bankManager::saveScenes(ofxXmlSettings XML){
    
    XML.addTag("SCENE_SETTINGS");
    
    if(XML.pushTag("SCENE_SETTINGS")){
        
        vector<ofPtr<scene> >::iterator it;
        int counter = 0;
        
        for(it = allScenes.begin(); it != allScenes.end(); it++){
            
            XML.addTag("SCENE");
            ofPtr<scene> sn = (*it);
            
            if(XML.pushTag("SCENE", counter)){
                
                saveScene(XML, sn);
                
                counter++;
                
                //scene tag
                XML.popTag();
            }
            
            
        }
        
        //scene settings tag
        XML.popTag();
    }

}

void bankManager::saveScene(ofxXmlSettings XML, ofPtr<scene> sn){
    
    XML.addValue("NAME", sn->getName());
    XML.addValue("UID", sn->getUid());
    XML.addValue("FADE_IN", sn->getFadeIn());
    XML.addValue("FADE_OUT", sn->getFadeOut());
    
    vector <ofPtr<triggerZone> > t_tzs = sn->getTriggerZones();
    vector <ofPtr<triggerZone> > ::iterator it;
    int count = 0;
    
    for(it = t_tzs.begin(); it != t_tzs.end(); it++){
        
        XML.addTag("ZONE");
        
        ofPtr<triggerZone> z = (*it);
        
        if(XML.pushTag("ZONE", count)){
            
                        
            count++;
            
            //zone tag
            XML.popTag();
        }
        
        
    }

    
}


void bankManager::saveZone(ofxXmlSettings XML, ofPtr<triggerZone> z){
    
    XML.addValue("NAME", z->getName());
    
    
    XML.addValue("POS_X", z->getPos().x);
    XML.addValue("POS_Y", z->getPos().y);
    XML.addValue("POS_Z", z->getPos().z);
    
    XML.addValue("SHAPE", z->getShape());
    
    XML.addValue("DIM_X", z->getBoxDims().x);
    XML.addValue("DIM_Y", z->getBoxDims().y);
    XML.addValue("DIM_Z", z->getBoxDims().z);
    XML.addValue("RADIUS", z->getRadius());
    XML.addValue("SOUNDFILE", z->getSoundFileName());
    XML.addValue("IS_LOOP", z->getIsLoop());
    XML.addValue("IS_PLAY_TO_END", z->getIsPlayToEnd());
    XML.addValue("INVERTED", z->getIsInverted());
    XML.addValue("ENABLED", z->getIsEnabled());
    //XML.addValue("SENSITIVITY", z->getSensitivity());
    XML.addValue("MIN_REPLAY", z->getMinReplaySecs());
    XML.addValue("SYNTH_TYPE", z->getSynthType());
    
    saveSynth(XML, z);
    

}

void bankManager::saveSynth(ofxXmlSettings XML, ofPtr<triggerZone> z){
    
    vector<synthParam> defSp = synthDictionary::getSynthParams(z->getSynthType());
    
    
    for(int i = 0; i < defSp.size(); i++){
        
        synthParam sp = z->getSynthParam(i);
        
        XML.addTag(defSp[i].name);
        
        if(XML.pushTag(defSp[i].name)){
            
            XML.addValue("ABS_VAL", sp.abs_val);
            XML.addValue("MIN_VAL", sp.min_val);
            XML.addValue("MAX_VAL", sp.max_val);
            XML.addValue("MAP", (int)sp.map);
            
            XML.popTag();
        }
        
    }
    

}

void bankManager::saveBanks(ofxXmlSettings XML){
    
    XML.addTag("BANK_SETTINGS");
    
    if(XML.pushTag("BANK_SETTINGS")){
        
        for(int i = 0; i < allBanks.size(); i ++){
            
            XML.addTag("BANK");
            
            if(XML.pushTag("BANK", i)){
                
                XML.setValue("NAME", allBanks[i]->name);
                
                int ns = allBanks[i]->scenes.size();
                XML.setValue("NUM_ITEMS", ns);
                
                for(int j = 0; j < ns; j ++){
                    
                    XML.setValue("ITEM", allBanks[i]->scenes[j]->getUid(), j);
                    
                }
                
                
                //bank tag
                XML.popTag();
            }
            
        }
        
        //Bank Settings tag
        XML.popTag();
        
    }

    
}

void bankManager::loadSettings(ofxXmlSettings XML){
    
    currentZone.reset();
    currentScene.reset();
    
    loadScenes(XML);
    loadBanks(XML);

    
}


void bankManager::loadScenes(ofxXmlSettings XML){
    
    if(XML.pushTag("SCENE_SETTINGS")){
        
        allScenes.clear();
        
        int numScenes = XML.getNumTags("SCENE");
        
        for(int sn = 0; sn < numScenes; sn++){
            
            if(XML.pushTag("SCENE", sn)){
                
                ofPtr<scene> nScene = ofPtr<scene>(new scene(pOsc));
                
                loadScene(XML, nScene);
                
                //scene tag
                XML.popTag();
            }
            
            
            
            
        }
        
        //TODO GUI methods back into testApp
        
        currentScene = (*allScenes.begin());
        //sc2TextInput[0]->setTextString(currentScene->getName());
        //there will probably be more to update here later
        
        if(currentScene->getNumTriggerZones() > 0){
            currentZone = currentScene->getFirstTriggerZone();
            //updateTZGuiElements();
        }else{
            //potentially refactor into a disableZoneElements
            //sc2TextInput[1]->setTextString("none");
            //sc2TextInput[2]->setTextString("none");
        }
        
        //scene settings tag
        XML.popTag();
    }
    
    
}


void bankManager::loadScene(ofxXmlSettings XML, ofPtr<scene> nScene){
    
    
    nScene->setName(XML.getValue("NAME", ""));
    nScene->setUid(XML.getValue("UID", nScene->getUid()));
    nScene->setFadeIn(XML.getValue("FADE_IN", 0.01));
    nScene->setFadeOut(XML.getValue("FADE_OUT", 0.01));
    
    int numZones = XML.getNumTags("ZONE");
    
    for(int tz = 0; tz < numZones; tz++){
        
        if(XML.pushTag("ZONE", tz)){
            
            ofPtr<triggerZone> z = nScene->addTriggerZone(currentZone);
            
            loadZone(XML, z);
            
            
            //zone tag
            XML.popTag();
            
        }
        
    }
    
    
    allScenes.push_back(nScene);

}

void bankManager::loadZone(ofxXmlSettings XML, ofPtr<triggerZone> z){
    
    z->setName(XML.getValue("NAME", ""));
    z->setShape(XML.getValue("SHAPE", 0));
    z->setPosX(XML.getValue("POS_X", 0.0));
    z->setPosY(XML.getValue("POS_Y", 0.0));
    z->setPosZ(XML.getValue("POS_Z", 0.0));
    z->setBoxDimsX( XML.getValue("DIM_X", 0.5));
    z->setBoxDimsY( XML.getValue("DIM_Y", 0.5));
    z->setBoxDimsZ( XML.getValue("DIM_Z", 0.5));
    z->setRadius(XML.getValue("RADIUS",1.0));
    z->setSoundFile(XML.getValue("SOUNDFILE", ""));
    z->setIsPlayToEnd(XML.getValue("IS_PLAY_TO_END", false));
    z->setIsInverted(XML.getValue("INVERTED", false));
    z->setIsLoop(XML.getValue("IS_LOOP", true));
    z->setIsEnabled(XML.getValue("ENABLED", false));
    //z->setSensitivity(XML.getValue("SENSITIVITY", 1.0));
    z->setMinReplaySecs(XML.getValue("MIN_REPLAY", 0.0));
    z->setSynthType(XML.getValue("SYNTH_TYPE", 0));
    
    loadSynth(XML, z);

}

void bankManager::loadSynth(ofxXmlSettings XML, ofPtr<triggerZone> z){
    
    vector<synthParam> defSp = synthDictionary::getSynthParams(z->getSynthType());
    
    
    for(int i = 0; i < defSp.size(); i++){
        
        
        synthParam sp = defSp[i];
        
        if(XML.pushTag(defSp[i].name)){
            
            sp.abs_val = XML.getValue("ABS_VAL", defSp[i].abs_val);
            sp.min_val = XML.getValue("MIN_VAL", defSp[i].min_val);
            sp.max_val = XML.getValue("MAX_VAL", defSp[i].max_val);
            sp.map = mapType(XML.getValue("MAP", defSp[i].map));
            
            z->setSynthParam(i, sp);
            
            XML.popTag();
        }
        
    }


}

void bankManager::loadBanks(ofxXmlSettings XML){
    
    if(XML.pushTag("BANK_SETTINGS")){
        
        allBanks.clear();
        
        int nb = XML.getNumTags("BANK");
        
        for(int i = 0; i < nb; i ++){
            
            
            if(XML.pushTag("BANK", i)){
                
                ofPtr<bank> tb = ofPtr<bank>(new bank());
                
                tb->name = XML.getValue("NAME","none");
                
                int ns = XML.getValue("NUM_ITEMS", 0);
                
                
                for(int j = 0; j < ns; j ++){
                    
                    string item = XML.getValue("ITEM", "", j);
                    
                    vector<ofPtr<scene> >::iterator it = find_if(allScenes.begin(), allScenes.end(), matchSceneIndex(item));
                    tb->scenes.push_back((*it));
                    
                }
                
                allBanks.push_back(tb);
                //bank tag
                XML.popTag();
            }
            
        }
        
        //TODO sort out updateBankElements in GUI
        
        if(allBanks.size() > 0)currentBank = allBanks[0];
        
        //updateBankElements();
        
        //Bank Settings tag
        XML.popTag();
        
        
        
    }else{
        
        
        allBanks.clear();
        ofPtr<bank> tb = ofPtr<bank>(new bank());
        allBanks.push_back(tb);
        currentBank = tb;
       // updateBankElements();
        
    }

    
    
}


void bankManager::checkUniqueId(ofPtr<scene> sn){
    
    bool isUnique = false;
    
    while (!isUnique) {
        
        bool isFound = false;
        vector<ofPtr<scene> >::iterator it = allScenes.begin();
        for(it = allScenes.begin(); it != allScenes.end(); it++){
            if(sn->getUid() == (*it)->getUid())isFound = true;
        }
        
        if(isFound){
            sn->newIndex();
        }else{
            isUnique = true;
        }
    }
    
}

vector<ofPtr<scene> >::iterator bankManager::getInsertIt(ofPtr<scene> sn){
    
    vector<ofPtr<scene> >::iterator it;
    if(allScenes.size() == 1)
        return allScenes.end();
    else
        it = find(allScenes.begin(), allScenes.end(), sn);
    it ++;
    return it;
    
}

vector<ofPtr<bank> >::iterator bankManager::getInsertIt(ofPtr<bank> bk){
    
    vector<ofPtr<bank> >::iterator it;
    if(allBanks.size() == 1)
        return allBanks.end();
    else
        it = find(allBanks.begin(), allBanks.end(), bk);
    it ++;
    return it;
    
}


void bankManager::selectFirstZone(){
    currentZone = currentScene->getFirstTriggerZone();
}

void bankManager::deselectCurrentZone(){
    if(currentZone)currentZone->setIsSelected(false);
    currentZone.reset();
}

ofPtr<scene> bankManager::selectNextScene(ofPtr<scene> sn){
    
    if(allScenes.size() == 1)return sn;
    vector< ofPtr<scene> >::iterator it = find(allScenes.begin(), allScenes.end(), sn);
    it++;
    if(it == allScenes.end())it--;
    return(*it);
    
}

ofPtr<scene> bankManager::selectPrevScene(ofPtr<scene> sn){
    
    if(allScenes.size() == 1)return sn;
    vector< ofPtr<scene> >::iterator it = find(allScenes.begin(), allScenes.end(), sn);
    if(it != allScenes.begin())it--;
    return(*it);
    
}

ofPtr<bank> bankManager::selectNextBank(ofPtr<bank> bk){
    
    if(allBanks.size() == 1)return bk;
    vector< ofPtr<bank> >::iterator it = find(allBanks.begin(), allBanks.end(), bk);
    it++;
    if(it == allBanks.end())it--;
    return(*it);
    
}

ofPtr<bank> bankManager::selectPrevBank(ofPtr<bank> bk){
    
    if(allBanks.size() == 1)return bk;
    vector< ofPtr<bank> >::iterator it = find(allBanks.begin(), allBanks.end(), bk);
    if(it != allBanks.begin())it--;
    return(*it);
    
}



//getters and setters

const ofPtr<triggerZone> bankManager::getCurrentZone(){return currentZone;}
const ofPtr<scene> bankManager::getCurrentScene(){return currentScene;}
const ofPtr<bank> bankManager::getCurrentBank(){return currentBank;}

void bankManager::setOsc(ofPtr<oscManager> t){
    pOsc = t;
}


