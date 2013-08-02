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
    TZ_BOX

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
    ofVec3f getBoxDims();
    float getRadius();
    
    void setRadius(float r);
    void setPosX(float x);
    void setPosY(float y);
    void setPosZ(float z);
    
    void setBoxDimsX(float x);
    void setBoxDimsY(float y);
    void setBoxDimsZ(float z);

    void setShape(int t);
    int getShape();
    
    void setName(string s);
    string getName();
    
    void toggleSelected();
    void setIsSelected(bool b);
    
    void setIsLoop(bool b);
    void getIsLoop();
    
    void setIsPlayToEnd(bool b);
    void getIsPlayToEnd();

    
    
private:
    
    tzShape shape;
    ofVec3f center;
    ofVec3f boxDims;
    ofVec3f intersect;

    float radius;
    ofVec3f corners[2];
    ofSoundPlayer mSound;
    ofTrueTypeFont font;
    
    string mName, mSoundFileName;
    
    bool isOccupied, isEnabled, isSelected;
    
    bool isLoop, isPlayToEnd;
    int occupyCount;
    
    static int index;
    
    
    
    
};


#endif /* defined(__danceTracker__triggerZone__) */
