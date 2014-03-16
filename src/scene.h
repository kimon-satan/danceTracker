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
    
    ofPtr<triggerZone> addTriggerZone();
    ofPtr<triggerZone> copyTriggerZone(string tz);
    ofPtr<triggerZone> getFirstTriggerZone();
    ofPtr<triggerZone> getNextTriggerZone(ofPtr<triggerZone>);
    ofPtr<triggerZone> getPrevTriggerZone(ofPtr<triggerZone> tz);
    
    void removeTriggerZone(string tz);
    void deepCopyTriggerZones();
    
    void checkUniqueId(ofPtr<triggerZone> tz);
    
    void deselectAll();
    
    //getters and setters
    
    map < string, ofPtr<triggerZone> > getTriggerZones();
    ofPtr<triggerZone> getTriggerZone(string tz);
    int getNumTriggerZones();
    void setName(string s);
    string getName();
    
    void setIndex(int i);
    int getIndex();
    void newIndex();
    static void setStaticIndex(int i);
    
    void setFadeIn(float f);
    float getFadeIn();
    
    void setFadeOut(float f);
    float getFadeOut();
    
    private:
    
    map < string, ofPtr<triggerZone> > triggerZones;
    ofPtr<oscManager> mOsc;
    
    float fadeIn, fadeOut;
    
    string mName;
    int mIndex;
    static int index;
    

};


#endif /* defined(__danceTracker__scene__) */
