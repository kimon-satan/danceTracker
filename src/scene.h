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
    
    ofVec3f getTZPos(int tz);
    float getTZRadius(int tz);
    
    void setTZRadius(int tz, float r);
    void setTZPosX(int tz, float x);
    void setTZPosY(int tz, float y);
    void setTZPosZ(int tz, float z);
    
    void setTZEnabled(int tz, bool b);
    bool getTZEnabled(int tz);
    
    private:
    
    vector <triggerZone> triggerZones;
    
    
    

};


#endif /* defined(__danceTracker__scene__) */
