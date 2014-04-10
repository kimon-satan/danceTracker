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

void bankManager::saveSettings(ofxXmlSettings & XML){

    saveScenes(XML);
    saveBanks(XML);
   
}



void bankManager::saveScenes(ofxXmlSettings & XML){
    
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

void bankManager::saveScene(ofxXmlSettings & XML, ofPtr<scene> sn){
    
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
            
            saveZone(XML, z);
                        
            count++;
            
            //zone tag
            XML.popTag();
        }
        
        
    }

    
}


void bankManager::saveZone(ofxXmlSettings & XML, ofPtr<triggerZone> z){
    
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
    XML.addValue("INVERTED", z->getIsOccInvert()); //left as is for compatability
    XML.addValue("MOV_ENABLED", z->getIsMovEnabled());
    XML.addValue("MOV_INVERTED", z->getIsMovInvert());
    XML.addValue("ENABLED", z->getIsEnabled());
    //XML.addValue("SENSITIVITY", z->getSensitivity());
    XML.addValue("MIN_REPLAY", z->getMinReplaySecs());
    XML.addValue("SYNTH_TYPE", z->getSynthType());
    XML.addValue("SELECTOR_TYPE", z->getSelectorType());
    
    saveSynth(XML, z);
    

}

void bankManager::saveSynth(ofxXmlSettings & XML, ofPtr<triggerZone> z){
    
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

void bankManager::saveBanks(ofxXmlSettings & XML){
    
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

void bankManager::loadSettings(ofxXmlSettings & XML){
    
    currentScene->unTriggerAll();
    
    currentZone.reset();
    currentScene.reset();
    
    loadScenes(XML);
    loadBanks(XML);

    
}


void bankManager::loadScenes(ofxXmlSettings & XML){
    
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
        
        currentScene = (*allScenes.begin());
        
        if(currentScene->getNumTriggerZones() > 0)currentZone = currentScene->getFirstTriggerZone();
             
        //scene settings tag
        XML.popTag();
    }
    
    
}


void bankManager::loadScene(ofxXmlSettings & XML, ofPtr<scene> nScene){
    
    
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

void bankManager::loadZone(ofxXmlSettings & XML, ofPtr<triggerZone> z){
    
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
    z->setIsOccInvert(XML.getValue("INVERTED", false));
    z->setIsMovEnabled(XML.getValue("MOV_ENABLED", false));
    z->setIsMovInvert(XML.getValue("MOV_INVERTED", false));
    z->setIsLoop(XML.getValue("IS_LOOP", true));
    z->setIsEnabled(XML.getValue("ENABLED", false), true);
    //z->setSensitivity(XML.getValue("SENSITIVITY", 1.0));
    z->setMinReplaySecs(XML.getValue("MIN_REPLAY", 0.0));
    z->setSynthType(XML.getValue("SYNTH_TYPE", 0));
    z->setSelectorType(XML.getValue("SELECTOR_TYPE", z->getSelectorType()));
    
    loadSynth(XML, z);

}

void bankManager::loadSynth(ofxXmlSettings & XML, ofPtr<triggerZone> z){
    
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

void bankManager::loadBanks(ofxXmlSettings & XML){
    
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
                    if(it != allScenes.end())tb->scenes.push_back((*it)); //if the scene doesn't exist no reference is pushed back
                    
                }
                
                allBanks.push_back(tb);
                //bank tag
                XML.popTag();
            }
            
        }
        
        
        if(allBanks.size() > 0)currentBank = allBanks[0];

        
        //Bank Settings tag
        XML.popTag();
        
        
        
    }else{
        
        
        allBanks.clear();
        ofPtr<bank> tb = ofPtr<bank>(new bank());
        allBanks.push_back(tb);
        currentBank = tb;
        
    }

    
    
}

void bankManager::update(ofPtr<dancer> d){
    currentScene->update(d);
}


void bankManager::incrementBank(){currentBank = selectNextBank(currentBank);}
void bankManager::decrementBank(){currentBank = selectPrevBank(currentBank);}

void bankManager::perfBankIncrement(){
    
    ofPtr<bank> b = selectNextBank(currentBank);
    
    if(currentBank != b){
        currentBank = b;
        currentBank->firstScene();
        if(currentBank->scenes.size() > 0){
            currentScene = currentBank->selScene;
            currentScene->unTriggerAll();
            pOsc->newScene(currentScene->getFadeIn(), currentScene->getFadeOut());
        }
    }

}

void bankManager::perfBankDecrement(){
    
    ofPtr<bank> b = selectPrevBank(currentBank);
    
    if(currentBank != b){
        currentBank = b;
        currentBank->firstScene();
        if(currentBank->scenes.size() > 0){
            currentScene = currentBank->selScene;
            currentScene->unTriggerAll();
            pOsc->newScene(currentScene->getFadeIn(), currentScene->getFadeOut());
        }
    }

    
}

void bankManager::perfSceneIncrement(){
    
    if(currentBank->scenes.size() > 0){
        currentBank->nextScene();
        currentScene = currentBank->selScene;
        currentScene->unTriggerAll();
        pOsc->newScene(currentScene->getFadeIn(), currentScene->getFadeOut());
    }
}

void bankManager::perfSceneDecrement(){
    if(currentBank->scenes.size() > 0){
        currentBank->prevScene();
        currentScene = currentBank->selScene;
        currentScene->unTriggerAll();
        pOsc->newScene(currentScene->getFadeIn(), currentScene->getFadeOut());
    }

}


void bankManager::createBank(){
    ofPtr<bank> b = ofPtr<bank>(new bank());
    allBanks.insert(getInsertIt(currentBank), b);
    currentBank = b;
}

void bankManager::deleteBank(){
    if(allBanks.size() > 1){
        ofPtr<bank> db = currentBank;
        currentBank = selectPrevBank(currentBank, true);
        allBanks.erase(find(allBanks.begin(), allBanks.end(), db));
    }
}


void bankManager::bankAddScene(){
    currentBank->insertScene(currentScene);
}

void bankManager::bankRemoveScene(){
     currentBank->removeScene();
}

void bankManager::bankIncrementScene(){
    currentBank->nextScene();
}

void bankManager::bankDecrementScene(){
    currentBank->prevScene();
}

void bankManager::incrementScene(){
    
    currentScene->unTriggerAll();
    currentScene->deselectAll();
    currentScene = selectNextScene(currentScene);
    currentScene->unTriggerAll();
    currentZone = currentScene->getFirstTriggerZone();
    
}

void bankManager::decrementScene(){
    
    currentScene->unTriggerAll();
    currentScene->deselectAll();
    currentScene = selectPrevScene(currentScene);
    currentScene->unTriggerAll();
    currentZone = currentScene->getFirstTriggerZone();
}

void bankManager::createScene(){
    
    currentScene->unTriggerAll();
    currentScene->deselectAll();
    currentZone.reset();
    
    ofPtr<scene> t = ofPtr<scene>(new scene(pOsc));
    checkUniqueId(t);
    allScenes.insert(getInsertIt(currentScene), t);
    currentScene = t;

}

void bankManager::deleteScene(){
    
    if(allScenes.size() <= 1)return;
    
    ofPtr<scene> t = currentScene;
    removeAllSceneReferences(t);
    currentScene = selectPrevScene(currentScene, true); //TO DO : might need wrap function
    vector <ofPtr<scene> > :: iterator it = find(allScenes.begin(),allScenes.end(), t);
    allScenes.erase(it);
    currentZone = currentScene->getFirstTriggerZone();

}

void bankManager::removeAllSceneReferences(ofPtr<scene> s){
    
    vector<ofPtr<bank> >::iterator b_it = allBanks.begin();
    
    while(b_it != allBanks.end()){
        
        vector<ofPtr<scene> >::iterator s_it;
        
        s_it = remove((*b_it)->scenes.begin(), (*b_it)->scenes.end(), s);
        if(s_it != (*b_it)->scenes.end())(*b_it)->scenes.erase(s_it, (*b_it)->scenes.end());
        
        b_it++;
    }
    
    
}

void bankManager::copyScene(){

    currentScene->unTriggerAll();
    currentScene->deselectAll();
    
    ofPtr<scene> t = ofPtr<scene>(new scene(*currentScene));
    t->newIndex();
    checkUniqueId(t);
    string s = t->getName();
    string s_a = "_" + t->getUid() + "_copy";
    if(s.length() > 5){
        if(s.substr(s.length()- 5,5) != "_copy"){
            t->setName(s + s_a);
        }else{
            t->setName(s.substr(0,s.length()-16) + s_a);
        }
    }else{
        t->setName(s + s_a);
    }
    
    t->deepCopyTriggerZones();
    allScenes.insert(getInsertIt(currentScene), t);
    currentScene = t;
    
    currentZone = currentScene->getFirstTriggerZone();

}

void bankManager::incrementZone(){
    if(!currentZone)return;
    ofPtr<triggerZone> tz = currentScene->getNextTriggerZone(currentZone);
    currentZone->setIsSelected(false);
    currentZone = tz;
}

void bankManager::decrementZone(){
    if(!currentZone)return;
    ofPtr<triggerZone> tz = currentScene->getPrevTriggerZone(currentZone);
    currentZone->setIsSelected(false);
    currentZone = tz;
}

void bankManager::createZone(){
    currentScene->deselectAll();
    currentZone = currentScene->addTriggerZone(currentZone); 
}

void bankManager::copyZone(){
    if(!currentZone)return;
    currentZone->setIsSelected(false);
    currentZone = currentScene->copyTriggerZone(currentZone);
}

void bankManager::deleteZone(){
    
    if(!currentZone)return;
    ofPtr<triggerZone> tz = currentZone;
    if(currentScene->getNumTriggerZones() > 1){
        currentZone = currentScene->getPrevTriggerZone(currentZone, true); //2nd argument isWrap
        currentZone->setIsSelected(true);
    }else{
        currentZone.reset();
    }
    currentScene->removeTriggerZone(tz);
}


void bankManager::unTriggerAll(){
    currentScene->unTriggerAll();
}

void bankManager::resetForPerformance(){
    
    resetScenes();
    
    currentBank = allBanks[0];
    currentBank->firstScene();
    currentZone.reset();
    
    if(currentBank->scenes.size() > 0){
        currentScene = currentBank->selScene;
    }else{
        currentScene = allScenes[0];
    }
    
    pOsc->newScene(currentScene->getFadeIn(), currentScene->getFadeOut());

}

void bankManager::resetCurrentZone(){
    
    if(currentZone)pOsc->resetZone(currentZone->getUid());
    
}


void bankManager::resetScenes(){

    vector<ofPtr<scene> >:: iterator it;
    
    for(it = allScenes.begin(); it != allScenes.end(); it++){
        (*it)->unTriggerAll();
        (*it)->deselectAll();
    }
    
    currentScene = allScenes[0];
    currentZone = currentScene->getFirstTriggerZone();
    
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

ofPtr<scene> bankManager::selectNextScene(ofPtr<scene> sn, bool isWrap){
    
    if(allScenes.size() == 1)return sn;
    vector< ofPtr<scene> >::iterator it = find(allScenes.begin(), allScenes.end(), sn);
    it++;
    if(!isWrap){
        if(it == allScenes.end())it--;
    }else{
        if(it == allScenes.end())it = allScenes.begin();
    }
    return(*it);
    
}

ofPtr<scene> bankManager::selectPrevScene(ofPtr<scene> sn, bool isWrap){
    
    if(allScenes.size() == 1)return sn;
    vector< ofPtr<scene> >::iterator it = find(allScenes.begin(), allScenes.end(), sn);
    if(!isWrap){
        if(it != allScenes.begin())it--;
    }else{
        if(it == allScenes.begin())it = allScenes.end();
        it--;
    }
    return(*it);
    
}

ofPtr<bank> bankManager::selectNextBank(ofPtr<bank> bk, bool isWrap){
    
    if(allBanks.size() == 1)return bk;
    vector< ofPtr<bank> >::iterator it = find(allBanks.begin(), allBanks.end(), bk);
    it++;
    if(!isWrap){
        if(it == allBanks.end())it--;
    }else{
        if(it == allBanks.end())it = allBanks.begin();
    }
    
    return(*it);
    
}

ofPtr<bank> bankManager::selectPrevBank(ofPtr<bank> bk, bool isWrap){
    
    if(allBanks.size() == 1)return bk;
    vector< ofPtr<bank> >::iterator it = find(allBanks.begin(), allBanks.end(), bk);
    if(!isWrap){
        if(it != allBanks.begin())it--;
    }else{
        if(it == allBanks.begin())it = allBanks.end();
        it--;
    }
    
    return(*it);
    
}



//getters and setters

const ofPtr<triggerZone> bankManager::getCurrentZone(){return currentZone;}
const ofPtr<scene> bankManager::getCurrentScene(){return currentScene;}
const ofPtr<bank> bankManager::getCurrentBank(){return currentBank;}

void bankManager::setOsc(ofPtr<oscManager> t){
    pOsc = t;
}


void bankManager::setCSceneName(string s){
    currentScene->setName(s);
}

void bankManager::setCZoneName(string s){
    if(currentZone)currentZone->setName(s);
}

void bankManager::setCZoneSoundFile(string s){
    
    if(!currentZone)return;
    
    currentZone->setSoundFile(s);
    if(currentZone->getIsAudioLoaded()){
        currentZone->setIsEnabled(currentZone->getIsEnabled());
    }else{
        currentZone->setIsEnabled(false);
    }


}

void bankManager::setCZoneShape(int i){currentZone->setShape(i);}
void bankManager::setCZoneRadius(float f){currentZone->setRadius(f);}

void bankManager::setCZonePosition(ofVec3f p){
    currentZone->setPosX(p.x);
    currentZone->setPosY(p.y);
    currentZone->setPosZ(p.z);
}
void bankManager::setCZoneBoxDims(ofVec3f bd){

    currentZone->setBoxDimsX(bd.x);
    currentZone->setBoxDimsY(bd.y);
    currentZone->setBoxDimsZ(bd.z);
}

bool bankManager::setCZoneEnabled(bool b){
    //returns false when can't enable bacuase of audio
    if(currentZone->getIsAudioLoaded()){
        currentZone->setIsEnabled(b);
        return b;
    }else{
        return false;
    }
    
}

void bankManager::setCZoneLoop(bool b){
    currentZone->setIsLoop(b);
}

void bankManager::setCZonePlayToEnd(bool b){
    currentZone->setIsPlayToEnd(b);
}

void bankManager::setCZoneOccInvert(bool b){
    currentZone->setIsOccInvert(b);
}

void bankManager::setCZoneMovInvert(bool b){
    currentZone->setIsMovInvert(b);
}

void bankManager::setCZoneMovEnabled(bool b){
    currentZone->setIsMovEnabled(b);
}

void bankManager::setCZoneMinReplay(float f){
    currentZone->setMinReplaySecs(f);
}

void bankManager::setCZoneSynthParam(int id, synthParam sp){
    currentZone->setSynthParam(id, sp);
}

void bankManager::setCSceneFadeIn(float f){
    currentScene->setFadeIn(f);
}

void bankManager::setCSceneFadeOut(float f){
    currentScene->setFadeOut(f);
}



void bankManager::incCZoneSynthType(){
    int i = min(ST_COUNT - 1, currentZone->getSynthType() + 1);
    currentZone->setSynthType(i);
}

void bankManager::decCZoneSynthType(){
    int i = max(0, currentZone->getSynthType() - 1);
    currentZone->setSynthType(i);

}

void bankManager::incCZoneSelectorType(){
    if(!currentZone)return;
    int i = min(SELT_COUNT - 1, currentZone->getSelectorType() + 1);
    currentZone->setSelectorType(i);
}

void bankManager::decCZoneSelectorType(){
    int i = max(0, currentZone->getSelectorType() - 1);
    currentZone->setSelectorType(i);
}



