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
#include "oscManager.h"
#include "synthDictionary.h"

enum tzShape{

    TZ_SPHERE,
    TZ_BOX

};



class triggerZone{
    
public:

    triggerZone(ofPtr<oscManager> o);
    
    void draw(ofVec3f camPos);
    
    bool checkInRange(ofVec3f com, float userHeight);
    void checkPoints(vector<ofVec3f> & pc);
    
    void updateAllAudio();
    
    void update();
    
    void reloadSound();
    
    
    //getters and setters
    
    void deselect();
    
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
    bool getIsLoop();
    
    void setIsPlayToEnd(bool b);
    bool getIsPlayToEnd();
    
    bool getIsAudioLoaded();
    
    void setSensitivity(float s);
    float getSensitivity();
    
    void setMinReplaySecs(float s);
    float getMinReplaySecs();
    
    int getSynthType();
    void setSynthType(int i);
    
    int getIndex();
    

    
    
private:
    
    void updateSynthParams();
    
    tzShape shape;
    ofVec3f center;
    ofVec3f boxDims;

    float radius;
    ofVec3f corners[2];
    ofSoundPlayer mSound;
    ofTrueTypeFont font;
    
    synthType synth;
    vector<synthParam> synthParams;
    
    float sensitivity, minReplaySecs;
    
    string mName, mSoundFileName;
    
    bool isOccupied, isEnabled, isSelected;
    
    bool isLoop, isPlayToEnd;
    int occupyCount, emptyCount;
    
    ofPtr<oscManager> mOsc;
    
    ofVec3f mCom;
    ofVec3f iCom;
    int numUp, inTotal;
    
    int mIndex;
    
    static int index;
    
    
    
    
};


#endif /* defined(__danceTracker__triggerZone__) */
