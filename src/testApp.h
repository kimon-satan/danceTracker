#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"

#include "bank.h"

#define NUM_SETTINGS_CANVASES 5
#define USE_KINECT false

enum dtDisplayMode{
    
    DT_DM_2D,
    DT_DM_3D,
    DT_DM_COUNT
    
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


//FIXME
/*class matchSceneIndex{

    int searchIndex;
    
    
public:
    
    matchSceneIndex(int searchIndex):searchIndex(searchIndex){};
    
    bool operator()(ofPtr<scene> s) const{
    
        return (s->getIndex() == searchIndex);
    };

};*/




class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void settingsEvents(ofxUIEventArgs &e);
    void dispEvents(ofxUIEventArgs &e);
    
    void s0Events(ofxUIEventArgs &e);
    void s1Events(ofxUIEventArgs &e);
    void s2Events(ofxUIEventArgs &e);
    void s3Events(ofxUIEventArgs &e);
    void s4Events(ofxUIEventArgs &e);
    
    void fEvents(ofxUIEventArgs &e);
    void synthEvents(ofxUIEventArgs &e);
    
    void exit();
    
    void setLightOri(ofLight &light, ofVec3f rot);
    
    void setupGui();
    
    void setupGeneralSettings();
    void setupZonePanels();
    void setupDisplaySettings();
    void setupSynthPanels();
    
    void saveSettings(string fn);
    void loadSettings(string fn);
    
    void calcQ();
    void recordBg();
    void segment();
    void analyseUser();
    
    void drawScenePointCloud();
    void drawUserPointCloud();
    void drawFloor();
    
    
    void updateZoneControls();
    void updateTZGuiElements();
    void updateBankElements();
    
    void populateSynthCanvas();
    void hideSynthCanvas();
    
    void perfChange(string name);
    void cleanUpBanks();
    
    ofPtr<scene> selectNextScene(ofPtr<scene> sn);
    ofPtr<scene> selectPrevScene(ofPtr<scene> sn);
    //kinect variables
    
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
    vector<ofVec3f> userPixels;
    
    ofxCvGrayscaleImage liveImg;
    ofxCvGrayscaleImage segImg;
    ofxCvGrayscaleImage segMask;
    ofxCvContourFinder cfFinder;
    
    int segRes;
    float segThresh, nearThresh, farThresh, minBlob, maxBlob;
    
    bool isUser;
    bool isBgRecorded;
    
    float floorY;
    ofVec3f com;
    float userHeight;
    
    
    ofPtr<oscManager>  mOsc;
    ofEasyCam  cm;
    ofLight topLight, ambientLight;
    
    
    //for fake users
    
    bool isFakeUser;
    ofVec3f fakePos;
    float fakeRadius;
    int numFakePoints;
    
    //Banks, Scenes, Zones
    
    int selScene, selBank, bSelScene;
    
    map<string, ofPtr<scene> > allScenes;
    vector<ofPtr<bank> >allBanks;
    
    ofPtr<scene> currentScene;
    ofPtr<triggerZone> currentZone;
    ofPtr<bank>currentBank;
    
    
    //UI Variables
    
    bool isSettingsGui, isDisplayGui, isCamMouse, isCamKey, isTextFocus, isPerfMode;
    bool isMouseDown, isSaveDialog, isLoadDialog;
    
    bool isViewCom, isViewCScene, isViewSegPoints, isSynthView;
    
    dtDisplayMode       displayMode;
    
    int tabBarWidth, tabBarHeight;
    
    //UI Elements
    
    ofxUITabBar * settingsTabBar;
    ofxUITabBar * displayTabBar;
    ofxUICanvas * settingsCanvases[NUM_SETTINGS_CANVASES];
    ofxUICanvas * displayCanvases[2];
    ofxUICanvas * zoneCanvases[3];
    ofxUIScrollableCanvas * synthCanvas;
    ofxUIScrollableCanvas * bankCanvas;
    ofxUICanvas * fakeCanvas;
    ofxUICanvas * dialog;
    
    
    //canvas 0
    
    ofxUITextInput * fileNameInput;
    ofxUILabel * confLab;
    
    //canvas 1
    
    ofxUISlider * sc1Sliders[8];
    
    //canvas 2
    
    ofxUITextInput * sc2TextInput[3];
    
    ofxUISlider * fInSlid, * fOutSlid;
    
    ofxUIRadio * shapeRad;
    
    ofxUISlider * sensSlider, * repSlider;
    ofxUISlider * radSlid, * tPosX, * tPosY, * tPosZ;
    ofxUISlider * xDimSlid, * yDimSlid, * zDimSlid;
    ofxUIToggle * eblTog, *loopTog, *playToEndTog, *dispSynthTog, *invertedTog;
    ofxUILabel * synthTypeDisp;
    
    
    //canvas 3
    
    ofxUITextInput * sc3TextInput;
    ofxUITextArea * sceneText;
    ofxUITextArea * bankText;
    
    //canvas 4
    
    ofxUITextArea * perfBankText;
    ofxUITextArea * perfSceneText;
    
    //synthCanvas
    
    vector<ofxUITextArea *> mapTypeLabels;
    
    //displayCanvas 3d
    
    ofxUIToggle * mouseTog;
    
    
    
};
