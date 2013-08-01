//
//  triggerZone.h
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#ifndef __danceTracker__triggerZone__
#define __danceTracker__triggerZone__

#include "ofMain.h"


enum tzShape{

    TZ_SPHERE,
    TZ_CUBE

};

class triggerZone{
    
public:

    triggerZone();
    
    void draw();
    
    bool checkInRange(ofVec3f com, float userHeight);
    void checkPoints(vector<ofVec3f> & pc);
    
    void update();
    
    void reloadSound();
    
    
    //getters and setters
    
    void setIsEnabled(bool b);
    bool getIsEnabled();
    
    void setSoundFile(string s);
    string getSoundFileName();
    
    ofVec3f getPos();
    float getRadius();
    
    void setRadius(float r);
    void setPosX(float x);
    void setPosY(float y);
    void setPosZ(float z);

    void setShape(int t);
    int getShape();
    
    void setName(string s);
    string getName();
    
    void toggleSelected();
    void setIsSelected(bool b);

    
    
private:
    
    tzShape shape;
    ofVec3f center;
    float radius;
    ofVec3f corners[2];
    ofSoundPlayer mSound;
    ofTrueTypeFont font;
    
    string mName, mSoundFileName;
    
    bool isOccupied, isEnabled, isSelected;
    
    
    static int index;
    
    
    
    
};


#endif /* defined(__danceTracker__triggerZone__) */
