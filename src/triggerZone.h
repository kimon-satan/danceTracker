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
#include "dancer.h"
#include "dt_utils.h"

enum tzShape{

    TZ_SPHERE,
    TZ_BOX,
    TZ_CYLINDER,
    TZ_COUNT

};


class triggerZone{
    
public:

    triggerZone(ofPtr<oscManager> o);
    
    void draw(ofVec3f camPos);
    
    bool checkInRange(ofPtr<dancer> d);
    void checkPoints(ofPtr<dancer> d);
    
    void updateAllAudio();
    
    void update(ofPtr<dancer> d);
    
    void reloadSound();
    
    
    //getters and setters
    
    void deselect();
    
    void setIsEnabled(bool b, bool isLoading  = false);
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
    string getShapeString();
    
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
    
    int getSelectorType();
    void setSelectorType(int i);

    synthParam getSynthParam(int i);
    void setSynthParam(int i, synthParam p);
    
    bool getIsOccInvert();
    void setIsOccInvert(bool b);
    
    bool getIsMovEnabled();
    void setIsMovEnabled(bool b);
    
    bool getIsMovInvert();
    void setIsMovInvert(bool b);
    
    void setChangeBuff(int i);
    int getChangeBuff();
    
    void newIndex();
    string getUid();

    
private:
    
    void evaluate();
    void forceStop();
    void updateSynthParams(bool isInit = false);
    
    tzShape shape;
    ofVec3f center;
    ofVec3f boxDims;

    float radius;
    ofVec3f corners[2];
    
    synthType synth;
    selectorType selector;
    vector<synthParam> synthParams;
    
    float sensitivity, minReplaySecs;
    
    string mName, mSoundFileName;
    
    bool isOccupied, isIntersect, isEnabled, isSelected, isSound;
    
    bool isLoop, isPlayToEnd, isOccInvert, isMovEnabled, isMovInvert, isMoving;
    int  silentCount, changeBuff, changeCount;
    
    ofPtr<oscManager> mOsc;
    
    ofVec3f mCom;
    ofVec3f iCom;
    int numUp, inTotal;
    
    string u_id;
    
    
    
};


#endif /* defined(__danceTracker__triggerZone__) */
