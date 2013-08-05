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
    void draw();
    void update(ofVec3f com, float userHeight,vector<ofVec3f> & pc);
    
    ofPtr<triggerZone> addTriggerZone(int tz);
    void removeTriggerZone(int tz);
    
    void deselectAll();
    
    //getters and setters
    
    ofPtr<triggerZone> getTriggerZone(int tz);
    int getNumTriggerZones();
    void setName(string s);
    string getName();
    

    
    private:
    
    vector < ofPtr<triggerZone> > triggerZones;
    ofPtr<oscManager> mOsc;
    
    
    string mName;
    
    static int index;
    

};


#endif /* defined(__danceTracker__scene__) */
