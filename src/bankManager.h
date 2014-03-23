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
    
    void saveSettings(ofxXmlSettings XML);
    void loadSettings(ofxXmlSettings XML);
    
    //selection tools
    void selectFirstZone();
    
    ofPtr<scene> selectNextScene(ofPtr<scene> sn);
    ofPtr<scene> selectPrevScene(ofPtr<scene> sn);
    ofPtr<bank> selectNextBank(ofPtr<bank> bk);
    ofPtr<bank> selectPrevBank(ofPtr<bank> bk);
    
    void deselectCurrentZone();
    
    
    vector<ofPtr<scene> >::iterator getInsertIt(ofPtr<scene> sn);
    vector<ofPtr<bank> >::iterator getInsertIt(ofPtr<bank> bk);
    
    void checkUniqueId(ofPtr<scene> sn);
    
    //getters and setters
    void setOsc(ofPtr<oscManager> t);
    const ofPtr<triggerZone> getCurrentZone();
    const ofPtr<scene> getCurrentScene();
    const ofPtr<bank> getCurrentBank();
    
    private:
    
    void saveScenes(ofxXmlSettings XML);
    void saveBanks(ofxXmlSettings XML);
    
    void saveScene(ofxXmlSettings XML, ofPtr<scene> sn);
    void saveZone(ofxXmlSettings XML, ofPtr<triggerZone> zn);
    void saveSynth(ofxXmlSettings XML, ofPtr<triggerZone> zn);
    
    void loadScenes(ofxXmlSettings XML);
    void loadBanks(ofxXmlSettings XML);
    
    void loadScene(ofxXmlSettings XML, ofPtr<scene> sn);
    void loadZone(ofxXmlSettings XML, ofPtr<triggerZone> zn);
    void loadSynth(ofxXmlSettings XML, ofPtr<triggerZone> zn);
    
    
    ofPtr<oscManager>  pOsc;
    
    //Banks, Scenes, Zones
    
    vector<ofPtr<scene> > allScenes;
    vector<ofPtr<bank> >allBanks;
    
    ofPtr<scene> currentScene;
    ofPtr<triggerZone> currentZone;
    ofPtr<bank>currentBank;
    

};

#endif /* defined(__danceTracker__bankManager__) */
