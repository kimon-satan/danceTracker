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
    
    scene();
    void draw();
    void update(ofVec3f com, float userHeight,vector<ofVec3f> & pc);
    
    
    //getters and setters
    
    ofPtr<triggerZone> getTriggerZone(int tz);
    

    
    private:
    
    vector < ofPtr<triggerZone> > triggerZones;
    
    
    

};


#endif /* defined(__danceTracker__scene__) */
