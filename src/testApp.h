#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"

#include "scene.h"

#define NUM_CANVASES 3

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
    
        void dispEvents(ofxUIEventArgs &e);
    
        void s0Events(ofxUIEventArgs &e);
        void s1Events(ofxUIEventArgs &e);
        void s2Events(ofxUIEventArgs &e);
    
    
        void exit();
    
        void setupGui();
    
        void saveSettings(string fn);
        void loadSettings(string fn);
    
        void calcQ();
        void recordBg();
        void segment();
        void analyseUser();
    
        void drawScenePointCloud();
        void drawUserPointCloud();
        void drawFloor();
    
    
        void updateTZGuiElements();

        ofxUITabBar * settingsTabBar;
        ofxUITabBar * displayTabBar;
        ofxUICanvas * settingsCanvases[NUM_CANVASES];
        ofxUICanvas * displayCanvases[2];
    
        ofxKinect 			kinect;
        int					kinectAngle;
    
        dtDisplayMode       displayMode;
    
        bool isViewCom, isViewCScene, isViewSegPoints;
    
        int pointCloudRotation;
        float pitch, roll;
    
        ofVec3f qaxis;
        float qangle;
    
        vector<float> mPitches;
        vector<float> mRolls;
    
        float rRange;
        float pRange;
    
        ofEasyCam  cm;
    
        float floorY;
    
        ofxCvGrayscaleImage liveImg;
        ofxCvGrayscaleImage segImg;
        ofxCvGrayscaleImage segMask;
        ofxCvContourFinder cfFinder;
    
        int segRes;
        float segThresh, nearThresh, farThresh, minBlob, maxBlob;
    
        bool isUser;
        bool isBgRecorded;
    
        vector<ofVec3f> bgDepths;
        vector<ofVec3f> curDepths;
        vector<ofVec3f> userPixels;
    
        ofVec3f com;
        float userHeight;
        float cZone; //just a dummy sort out later
        int selZone, selScene;
    
        bool isSettingsGui, isDisplayGui, isCamMouse, isCamKey, isTextFocus;
        bool isMouseDown;
    
        int tabBarWidth, tabBarHeight;
    
        vector<ofPtr<scene> > allScenes;
    
        ofPtr<scene> currentScene;
        ofPtr<triggerZone> currentZone;
    
    
        //UI pointers
    
        //canvas 0
    
        ofxUITextInput * fileNameInput;
    
        //canvas 1
    
        ofxUISlider * sc1Sliders[8];
    
    
        //canvas 2
    
        ofxUITextInput * sc2TextInput[3];
    
        ofxUIDropDownList * sc2DropDowns[2];
    
        ofxUISlider * radSlid, * tPosX, * tPosY, * tPosZ;
        ofxUIToggle * eblTog;
    
        //displayCanvas 3d
    
        ofxUIToggle * mouseTog;
    
    
		
};
