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
    
    ofPtr<triggerZone> addTriggerZone(int tz);
     ofPtr<triggerZone> copyTriggerZone(int tz);
    void removeTriggerZone(int tz);
    void deepCopyTriggerZones();
    
    
    void deselectAll();
    
    //getters and setters
    
    ofPtr<triggerZone> getTriggerZone(int tz);
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
    
    vector < ofPtr<triggerZone> > triggerZones;
    ofPtr<oscManager> mOsc;
    
    float fadeIn, fadeOut;
    
    string mName;
    int mIndex;
    static int index;
    

};


#endif /* defined(__danceTracker__scene__) */
