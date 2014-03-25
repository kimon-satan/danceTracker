//
//  kinectManager.h
//  danceTracker
//
//  Created by Simon Katan on 23/03/2014.
//
//

#ifndef __danceTracker__kinectManager__
#define __danceTracker__kinectManager__

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

#define USE_KINECT true
#define OFX_KINECT_GRAVITY 9.80665

class dancer {

    public:
    
    ofVec3f com;
    vector<ofVec3f> pixels;

};


class findOutliers{
    
    ofVec3f com;
    float userHeight;
    
public:
    
    findOutliers(ofVec3f com, float userHeight): com(com), userHeight(userHeight){};
    
    bool operator()(ofVec3f t) const{
        
        return (t.distance(com) > userHeight/2);
        
    };
    
};


class kinectManager{

    public:
    
    kinectManager();
    void saveSettings(ofxXmlSettings & XML);
    void loadSettings(ofxXmlSettings & XML);
    void update();
    void recordBg();
    void drawScenePointCloud();
    void drawUserPointCloud();
    void exit();

    //getters and setters
    
    void toggleFake();
    bool getIsFake();
    
    const ofPtr<dancer> getDancer();
    ofVec3f getFakePos();
    void setFakePos(ofVec3f p);
    
    
    int getKinectAngle();
    void setKinectAngle(int f);
    
    float getSegThresh();
    void setSegThresh(float f);
    
    float getNearThresh();
    void setNearThresh(float f);
    
    float getFarThresh();
    void setFarThresh(float f);
    
    float getMinBlob();
    void setMinBlob(float f);
    
    float getMaxBlob();
    void setMaxBlob(float f);
    
    float getFloorY();
    void setFloorY(float f);
    
    float getDancerHeight();
    void setDancerHeight(float f);
    
    float getQAngle();
    ofVec3f getQAxis();
    
    ofxCvGrayscaleImage * getLiveImg();
    ofxCvGrayscaleImage * getSegMask();
    ofxCvContourFinder * getCfFinder();
    
    private:
    
    void calcQ();
    void segment();
    void analyseUser();
    
    ofxKinect 			kinect;
    int					kinectAngle;
    int                 numBlankFrames;
    
    int pointCloudRotation;
    float pitch, roll;
    
    vector<float> mPitches;
    vector<float> mRolls;
    
    ofVec3f qaxis;
    float qangle;
    
    float rRange, pRange;
    
    vector<ofVec3f> bgDepths;
    vector<ofVec3f> curDepths;
    
    ofxCvGrayscaleImage liveImg;
    ofxCvGrayscaleImage segImg;
    ofxCvGrayscaleImage segMask;
    ofxCvContourFinder cfFinder;
    
    int segRes;
    float segThresh, nearThresh, farThresh, minBlob, maxBlob;
    
    bool isUser;
    bool isBgRecorded;
    
    float floorY;
    
    float dancerHeight;
    
    ofPtr<dancer> mDancer;
    
    //for fake users
    
    bool isFakeUser;
    ofVec3f fakePos;
    float fakeRadius;
    int numFakePoints;
    
    
};

#endif /* defined(__danceTracker__kinectManager__) */
