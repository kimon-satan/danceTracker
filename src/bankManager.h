//
//  bankManager.h
//  danceTracker
//
//  Created by Simon Katan on 23/03/2014.
//
//

#ifndef __danceTracker__bankManager__
#define __danceTracker__bankManager__

#include "bank.h"
#include "ofxXmlSettings.h"

class matchSceneIndex{
    
    string searchIndex;
    
    
public:
    
    matchSceneIndex(string searchIndex):searchIndex(searchIndex){};
    
    bool operator()(ofPtr<scene> s) const{
        
        return (s->getUid() == searchIndex);
    };
    
};



class bankManager{

    public:
    
    bankManager(ofPtr<oscManager> t);
    
    void saveSettings(ofxXmlSettings & XML);
    void loadSettings(ofxXmlSettings & XML);
    
    void update(ofVec3f com, float userHeight,vector<ofVec3f> & pc);
    void unTriggerAll();
    void resetForPerformance();
    void resetScenes();
    
    //selection tools
    
    void incrementBank();
    void decrementBank();
    void createBank();
    void deleteBank();
    
    void bankAddScene();
    void bankRemoveScene();
    void bankIncrementScene();
    void bankDecrementScene();
    
    void perfBankIncrement();
    void perfBankDecrement();
    void perfSceneIncrement();
    void perfSceneDecrement();
    
    
    void incrementScene();
    void decrementScene();
    void createScene();
    void deleteScene();
    void copyScene();
    
    void selectFirstZone();
    
    void incrementZone();
    void decrementZone();
    void createZone();
    void copyZone();
    void deleteZone();
    
    void deselectCurrentZone();
    
    
    vector<ofPtr<scene> >::iterator getInsertIt(ofPtr<scene> sn);
    vector<ofPtr<bank> >::iterator getInsertIt(ofPtr<bank> bk);
    
    void checkUniqueId(ofPtr<scene> sn);
    
    //getters and setters
    void setOsc(ofPtr<oscManager> t);
    const ofPtr<triggerZone> getCurrentZone();
    const ofPtr<scene> getCurrentScene();
    const ofPtr<bank> getCurrentBank();
    
    void setCSceneName(string s);
    void setCZoneName(string s);
    void setCZoneSoundFile(string s);
    
    void incCZoneSynthType();
    void decCZoneSynthType();
    
    void setCZoneShape(int i);
    void setCZoneRadius(float f);
    void setCZonePosition(ofVec3f p);
    void setCZoneBoxDims(ofVec3f bd);
    bool setCZoneEnabled(bool b);
    void setCZoneLoop(bool b);
    void setCZonePlayToEnd(bool b);
    void setCZoneInverted(bool b);
    void setCZoneMinReplay(float f);
    
    void setCZoneSynthParam(int id, synthParam sp);
    
    void setCSceneFadeIn(float f);
    void setCSceneFadeOut(float f);
    
    
    private:
    
    void saveScenes(ofxXmlSettings & XML);
    void saveBanks(ofxXmlSettings & XML);
    
    void saveScene(ofxXmlSettings & XML, ofPtr<scene> sn);
    void saveZone(ofxXmlSettings & XML, ofPtr<triggerZone> zn);
    void saveSynth(ofxXmlSettings & XML, ofPtr<triggerZone> zn);
    
    void loadScenes(ofxXmlSettings & XML);
    void loadBanks(ofxXmlSettings & XML);
    
    void loadScene(ofxXmlSettings & XML, ofPtr<scene> sn);
    void loadZone(ofxXmlSettings & XML, ofPtr<triggerZone> zn);
    void loadSynth(ofxXmlSettings & XML, ofPtr<triggerZone> zn);
    
    //selection tools
    ofPtr<scene> selectNextScene(ofPtr<scene> sn);
    ofPtr<scene> selectPrevScene(ofPtr<scene> sn);
    ofPtr<bank> selectNextBank(ofPtr<bank> bk);
    ofPtr<bank> selectPrevBank(ofPtr<bank> bk);
    
    
    ofPtr<oscManager>  pOsc;
    
    //Banks, Scenes, Zones
    
    vector<ofPtr<scene> > allScenes;
    vector<ofPtr<bank> >allBanks;
    
    ofPtr<scene> currentScene;
    ofPtr<triggerZone> currentZone;
    ofPtr<bank>currentBank;
    

};

#endif /* defined(__danceTracker__bankManager__) */
