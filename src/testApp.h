#pragma once

#include "ofMain.h"
#include "ofxUI.h"

#include "kinectManager.h"
#include "bankManager.h"

#define NUM_SETTINGS_CANVASES 5

enum dtDisplayMode{
    
    DT_DM_2D,
    DT_DM_3D,
    DT_DM_COUNT
    
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
    
    void drawFloor();
    
    void updateMainSettingsGui();
    void updateBankElements(ofPtr<bank> b, ofPtr<scene> s);
    void updateSceneControls(ofPtr<scene> s, ofPtr<triggerZone> zn);
    void updateTZGuiElements(ofPtr<triggerZone> zn);
   
    void populateSynthCanvas(ofPtr<triggerZone> zn);
    void hideSynthCanvas();
    
    void perfChange(string name);
    void sendFeatureNames();
    
    //goldsmiths hack to bypass OSC manager
    ofxOscSender weki_sender;
    ofxOscReceiver weki_receiver;
    ofxOscSender remote_sender;
    
    ofPtr<bankManager> m_bankManager;
    kinectManager m_kinectManager;
        
    
    ofPtr<oscManager>  mOsc;
 
    ofEasyCam  cm;
    bool isUntriggered;
    float accumulator;
    
    
    //UI Variables
    
    bool isFullscreenMode;
    
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
    ofxUICanvas * zoneCanvases[4];
    ofxUIScrollableCanvas * synthCanvas;
    ofxUIScrollableCanvas * bankCanvas;
    ofxUICanvas * fakeCanvas;
    ofxUICanvas * dialog;
    
    
    //canvas 0
    
    ofxUITextInput * fileNameInput;
    ofxUILabel * confLab;
    
    //canvas 1
    
    ofxUISlider * sc1Sliders[10];
    
    //canvas 2
    
    ofxUITextInput * sc2TextInput[3];
    
    ofxUISlider * fInSlid, * fOutSlid;
    
    ofxUITextArea * shapeText;
    
    ofxUISlider * sensSlider, * repSlider, * cbSlider;
    ofxUISlider * radSlid, * tPosX, * tPosY, * tPosZ;
    ofxUISlider * xDimSlid, * yDimSlid, * zDimSlid;
    ofxUIToggle * eblTog, *loopTog, *playToEndTog, *dispSynthTog, *occInvTog, *movEnaTog, *movInvTog;
    ofxUILabel * synthTypeDisp, * selTypeDisp;
    
    
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
    
    int state;
    float sendStep;
    
    
    
};
