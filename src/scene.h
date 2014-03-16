//
//  scene.h
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#ifndef __danceTracker__scene__
#define __danceTracker__scene__

#include "triggerZone.h"

class scene{

    public:
    
    scene(ofPtr<oscManager> o);
    void draw(ofVec3f camPos);
    void update(ofVec3f com, float userHeight,vector<ofVec3f> & pc);
    
    ofPtr<triggerZone> addTriggerZone(ofPtr<triggerZone> tz);
    ofPtr<triggerZone> copyTriggerZone(ofPtr<triggerZone> tz);
    ofPtr<triggerZone> getFirstTriggerZone();
    ofPtr<triggerZone> getNextTriggerZone(ofPtr<triggerZone> tz);
    ofPtr<triggerZone> getPrevTriggerZone(ofPtr<triggerZone> tz);
    
    void removeTriggerZone(ofPtr<triggerZone> tz);
    void deepCopyTriggerZones();
    
    void checkUniqueId(ofPtr<triggerZone> tz);
    vector<ofPtr<triggerZone> >::iterator getInsertIt(ofPtr<triggerZone> tz);
    
    void deselectAll();
    
    //getters and setters
    
    vector <ofPtr<triggerZone> > getTriggerZones();
    //ofPtr<triggerZone> getTriggerZone(string tz);
    int getNumTriggerZones();
    void setName(string s);
    string getName();
    
    string getUid();
    
    void newIndex();
    
    void setFadeIn(float f);
    float getFadeIn();
    
    void setFadeOut(float f);
    float getFadeOut();
    
    private:
    
    vector <ofPtr<triggerZone> > triggerZones;
    ofPtr<oscManager> mOsc;
    
    float fadeIn, fadeOut;
    
    string mName;
    string u_id;
    

};


#endif /* defined(__danceTracker__scene__) */
