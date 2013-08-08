#include "testApp.h"

#define OFX_KINECT_GRAVITY 9.80665




//--------------------------------------------------------------
void testApp::setup(){
    
    
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    
    numBlankFrames = 0;
    
    if(USE_KINECT){
        kinect.clear();
        kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
        kinect.setVerbose(true);
        kinect.open();
        
        
        if(kinect.isConnected()){
            // zero the tilt on startup
            kinectAngle = 0;
            kinect.setCameraTiltAngle(kinectAngle);
            kinect.getCalibration().setClippingInCentimeters(50,1500);
        }
        
    }
    
    mOsc = ofPtr<oscManager>(new oscManager());
    
    pointCloudRotation = 0;
    
    calcQ();
    
    floorY = -1;
    
    liveImg.allocate(kinect.width,kinect.height);
    segMask.allocate(kinect.width, kinect.height);
    
    isBgRecorded = false;
    isUser = false;
    segThresh = 0.2;
    segRes = 2;
    nearThresh = 0.5;
    farThresh = 12;
    
    minBlob = 0.005;
    maxBlob = 0.5;
    
    userHeight = 1.80;
    
    fakePos.set(0,0,5);
    fakeRadius = 0.25;
    numFakePoints = 1000;
    
    isViewCom = false;
    isViewCScene = false;
    isViewSegPoints = false;
    isSynthView = false;
    
    cm.reset();
    cm.setPosition(0, 0, 1000);
    cm.setTarget(ofVec3f(0, 0, 500));
    
    isCamMouse = false; isCamKey = true; isTextFocus = false, isMouseDown = false, isPerfMode = false;
    
    cm.disableMouseInput();
    
    ofPtr<scene>  s = ofPtr<scene>(new scene(mOsc));
    allScenes.push_back(s);
    currentScene = s;
    
    ofPtr<bank> b = ofPtr<bank>(new bank());
    allBanks.push_back(b);
    currentBank = b;
    
    setupGui();
    updateZoneControls();
    
    isFakeUser = false;
    selBank = 0; selZone = 0; selScene = 0; bSelScene = 0;
    
}

void testApp::setupGui(){
    
    tabBarWidth = 320;
    tabBarHeight = 120;
    
    isSettingsGui = true;
    isDisplayGui = true;
    
    setupGeneralSettings();
    setupZonePanels();
    setupDisplaySettings();
    
    
    fakeCanvas = new ofxUICanvas(ofGetWidth() - tabBarWidth, 450, tabBarWidth, 125);
    fakeCanvas->setColorFill(ofxUIColor(200));
    fakeCanvas->setColorFillHighlight(ofxUIColor(255));
    fakeCanvas->setColorBack(ofxUIColor(20, 20, 20, 150));
    
    fakeCanvas->addSlider("F_POS_X", -5, 5, fakePos.x);
    fakeCanvas->addSlider("F_POS_Y", -2, 2, fakePos.y);
    fakeCanvas->addSlider("F_POS_Z", 0, 10, fakePos.z);
    
    ofAddListener(fakeCanvas->newGUIEvent,this,&testApp::fEvents);
    fakeCanvas->setVisible(false);
    
    
    synthCanvas = NULL;
    
    
}

void testApp::setupGeneralSettings(){
    
    float slHeight  = 24;
    float slLength = tabBarWidth - OFX_UI_GLOBAL_WIDGET_SPACING * 2;
    
    // ---
    settingsTabBar = new ofxUITabBar();
    
    settingsTabBar->setPosition(0, ofGetHeight() - tabBarHeight);
    settingsTabBar->setColorFill(ofxUIColor(200));
    settingsTabBar->setColorFillHighlight(ofxUIColor(255));
    settingsTabBar->setColorBack(ofxUIColor(255, 20, 20, 150));
    
    ofAddListener(settingsTabBar->newGUIEvent,this,&testApp::settingsEvents);
    
    for(int i = 0; i < NUM_SETTINGS_CANVASES; i ++){
        
        settingsCanvases[i] = new ofxUICanvas(0, 0, tabBarWidth, 500);
        settingsCanvases[i]->setColorFill(ofxUIColor(200));
        settingsCanvases[i]->setColorFillHighlight(ofxUIColor(255));
        settingsCanvases[i]->setColorBack(ofxUIColor(20, 20, 20, 150));
        
    }
    
    
    //--------------------------------------------
    
    settingsCanvases[0]->setName("Save/Load");
    
    settingsCanvases[0]->addLabel("Save/Load");
    settingsCanvases[0]->addSpacer();
    
    fileNameInput = settingsCanvases[0]->addTextInput("FILENAME", "none");
    settingsCanvases[0]->addButton("SAVE", false);
    settingsCanvases[0]->addButton("LOAD", false);
    
    
    ofAddListener(settingsCanvases[0]->newGUIEvent,this,&testApp::s0Events);
    settingsTabBar->addCanvas(settingsCanvases[0]);
    settingsCanvases[0]->autoSizeToFitWidgets();
    
    //---------------------------
    
	settingsCanvases[1]->setName("Initial Setup");
    
    sc1Sliders[0] = settingsCanvases[1]->addSlider("KN_TILT", -30, 30, kinectAngle, slLength, slHeight);
    
    settingsCanvases[1]->addSpacer();
    
    settingsCanvases[1]->addButton("RECORD_BACKGROUND", false);
    
    settingsCanvases[1]->addSpacer();
    
    sc1Sliders[1] = settingsCanvases[1]->addSlider("NEAR_THRESH", 0, 2, nearThresh, slLength, slHeight);
    sc1Sliders[2] = settingsCanvases[1]->addSlider("FAR_THRESH", 5, 15, farThresh, slLength, slHeight);
    sc1Sliders[3] =  settingsCanvases[1]->addSlider("SEG_THRESH", 0, 1, segThresh, slLength, slHeight);
    sc1Sliders[4] = settingsCanvases[1]->addSlider("MIN_BLOB", 0, 0.1, minBlob, slLength, slHeight);
    sc1Sliders[5] = settingsCanvases[1]->addSlider("MAX_BLOB", 0.25, 1, maxBlob, slLength, slHeight);
    
    
    settingsCanvases[1]->addSpacer();
    sc1Sliders[6] = settingsCanvases[1]->addSlider("FLOOR_Y", -10, -0.5, floorY, slLength, slHeight);
    sc1Sliders[7] = settingsCanvases[1]->addSlider("USER_HEIGHT", 1, 2, userHeight, slLength, slHeight);
    
    
    ofAddListener(settingsCanvases[1]->newGUIEvent,this,&testApp::s1Events);
    settingsTabBar->addCanvas(settingsCanvases[1]);
    settingsCanvases[1]->autoSizeToFitWidgets();
    
    //------------------------
    
    
	settingsCanvases[2]->setName("Scene Setup");
    
    
    settingsCanvases[2]->addLabel("SELECTED SCENE");
    sc2TextInput[0] = settingsCanvases[2]->addTextInput("S_NAME", "");
    sc2TextInput[0]->setTextString(currentScene->getName());
    
    ofxUILabelButton * sb = (ofxUILabelButton *)settingsCanvases[2]->addWidgetDown(new ofxUILabelButton("SCENE_MINUS", true, 25));
    ofxUILabelButton * sc = (ofxUILabelButton *)settingsCanvases[2]->addWidgetRight(new ofxUILabelButton("SCENE_PLUS", true, 25));
    
    sb->setLabelText("-");
    sc->setLabelText("+");
    
    
    settingsCanvases[2]->addButton("CREATE_SCENE", false);
    settingsCanvases[2]->addButton("DELETE_SCENE", false);
    
    
    settingsCanvases[2]->addSpacer();
    
    settingsCanvases[2]->addLabel("SELECTED_ZONE");
    sc2TextInput[1] = settingsCanvases[2]->addTextInput("Z_NAME", "none");
    
    ofxUILabelButton * zb = (ofxUILabelButton *)settingsCanvases[2]->addWidgetDown(new ofxUILabelButton("ZONE_MINUS", true, 25));
    ofxUILabelButton * zc = (ofxUILabelButton *)settingsCanvases[2]->addWidgetRight(new ofxUILabelButton("ZONE_PLUS", true, 25));
    
    zb->setLabelText("-");
    zc->setLabelText("+");
    
    settingsCanvases[2]->addButton("CREATE_ZONE", false);
    settingsCanvases[2]->addButton("DELETE_ZONE", false);
    
    
    ofAddListener(settingsCanvases[2]->newGUIEvent,this,&testApp::s2Events);
    settingsTabBar->addCanvas(settingsCanvases[2]);
    settingsCanvases[2]->autoSizeToFitWidgets();
    
    //----------------------------BANKS CANVASES ----------------------//
    
    settingsCanvases[3]->setName("Setup Banks");
    
    settingsCanvases[3]->addLabel("Setup Banks");
    
    
    settingsCanvases[3]->addLabel("CURRENT BANK: ", OFX_UI_FONT_SMALL);
    sc3TextInput = new ofxUITextInput ("B_NAME", "all scenes", 200);
    
    settingsCanvases[3]->addWidgetRight(sc3TextInput);
    
    ofxUILabelButton * bb = (ofxUILabelButton *)settingsCanvases[3]->addWidgetDown(new ofxUILabelButton("BANK_MINUS", true, 25));
    ofxUILabelButton * bc = (ofxUILabelButton *)settingsCanvases[3]->addWidgetRight(new ofxUILabelButton("BANK_PLUS", true, 25));
    
    bb->setLabelText("-");
    bc->setLabelText("+");
    
    settingsCanvases[3]->addWidgetRight(new ofxUIButton("CREATE_BANK", false, 20,20));
    settingsCanvases[3]->addWidgetRight(new ofxUIButton("DELETE_BANK", false, 20,20));
    
    
    settingsCanvases[3]->addSpacer();
    
    settingsCanvases[3]->addLabel("SELECTED SCENE: ", OFX_UI_FONT_SMALL);
    
    sceneText = new ofxUITextArea("SCENE_TEXT", "none", 200, 20,0,0,OFX_UI_FONT_SMALL);
    
    sceneText->setDrawBack(true);
    
    settingsCanvases[3]->addWidgetRight(sceneText);
    
    ofxUILabelButton * sbb = (ofxUILabelButton *)settingsCanvases[3]->addWidgetDown(new ofxUILabelButton("B_SCENE_MINUS", true, 25));
    ofxUILabelButton * sbc = (ofxUILabelButton *)settingsCanvases[3]->addWidgetRight(new ofxUILabelButton("B_SCENE_PLUS", true, 25));
    
    sbb->setLabelText("-");
    sbc->setLabelText("+");
    
    settingsCanvases[3]->addWidgetRight(new ofxUIButton("ADD_SCENE_TO_BANK", false, 20,20));
    
    bankText = new ofxUITextArea("BANK_TEXT", "bank items here", tabBarWidth, 200, 0,0,  OFX_UI_FONT_SMALL);
    
    bankText->setDrawOutlineHighLight(true);
    
    settingsCanvases[3]->addWidgetDown(bankText);
    
    ofxUILabelButton * sbb2 = (ofxUILabelButton *)settingsCanvases[3]->addWidgetRight(new ofxUILabelButton("B_ITEM_MINUS", true, 25));
    ofxUILabelButton * sbc2 = (ofxUILabelButton *)settingsCanvases[3]->addWidgetSouthOf(new ofxUILabelButton("B_ITEM_PLUS", true, 25), "B_ITEM_MINUS");
    
    sbb2->setLabelText("+");
    sbc2->setLabelText("-");
    
    settingsCanvases[3]->addWidgetSouthOf(new ofxUIButton("REMOVE_ITEM", false, 20,20), "BANK_TEXT");
    
    ofAddListener(settingsCanvases[3]->newGUIEvent,this,&testApp::s3Events);
    settingsTabBar->addCanvas(settingsCanvases[3]);
    settingsCanvases[3]->autoSizeToFitWidgets();
    
    //----------------------------PERFORMANCE CANVAS ----------------------//
    
    settingsCanvases[4]->setName("Performance Mode");
    
    settingsCanvases[4]->addLabel("Performance Mode");
    
    
    settingsCanvases[4]->addLabel("CURRENT BANK: ", OFX_UI_FONT_SMALL);
    perfBankText = new ofxUITextArea ("PERF_BANK_TEXT", "none", 150, 20, 0,0,  OFX_UI_FONT_SMALL);
    perfBankText->setDrawBack(true);
    
    settingsCanvases[4]->addWidgetRight(perfBankText);
    
    ofxUILabelButton * bplus = (ofxUILabelButton *)settingsCanvases[4]->addWidgetDown(new ofxUILabelButton("BANK_MINUS", true, 25));
    ofxUILabelButton * bmin = (ofxUILabelButton *)settingsCanvases[4]->addWidgetRight(new ofxUILabelButton("BANK_PLUS", true, 25));
    
    bplus->setLabelText("-");
    bmin->setLabelText("+");
    
    
    settingsCanvases[4]->addSpacer();
    
    perfSceneText = new ofxUITextArea("BANK_TEXT", "bank items here", tabBarWidth, 200, 0,0,  OFX_UI_FONT_SMALL);
    perfSceneText->setDrawOutlineHighLight(true);
    
    settingsCanvases[4]->addWidgetDown(perfSceneText);
    
    bplus = (ofxUILabelButton *)settingsCanvases[4]->addWidgetRight(new ofxUILabelButton("B_ITEM_MINUS", true, 25));
    bmin = (ofxUILabelButton *)settingsCanvases[4]->addWidgetSouthOf(new ofxUILabelButton("B_ITEM_PLUS", true, 25), "B_ITEM_MINUS");
    
    bplus->setLabelText("+");
    bmin->setLabelText("-");
    

    
    ofAddListener(settingsCanvases[4]->newGUIEvent,this,&testApp::s4Events);
    settingsTabBar->addCanvas(settingsCanvases[4]);
    settingsCanvases[4]->autoSizeToFitWidgets();

    
  
    
}

void testApp::setupZonePanels(){
    
    //----------------------ZONE CANVASES --------------//
    
    for(int i = 0; i < 3; i ++){
        
        if(i == 0)
            zoneCanvases[i] = new ofxUICanvas(ofGetWidth()/2 - 400, ofGetHeight() - 200, 600, 200);
        else
            zoneCanvases[i] = new ofxUICanvas(ofGetWidth()/2 + 210, ofGetHeight() - 200, 300, 200);
        
        zoneCanvases[i]->setColorFill(ofxUIColor(200));
        zoneCanvases[i]->setColorFillHighlight(ofxUIColor(255));
        zoneCanvases[i]->setColorBack(ofxUIColor(20, 20, 20, 150));
        
        
    }
    
    //zone c0 -------------------------------------------
    
    float slw = 200 - OFX_UI_GLOBAL_WIDGET_SPACING - 5;
    
    tPosX = new ofxUISlider("T_POS_X", -5, 5, 0.0, slw, 10);
    tPosY = new ofxUISlider("T_POS_Y", -2, 2, 0.0, slw, 10);
    tPosZ = new ofxUISlider("T_POS_Z", 0, 10, 0.0, slw, 10);
    
    zoneCanvases[0]->addWidgetDown(tPosX);
    zoneCanvases[0]->addWidgetRight(tPosY);
    zoneCanvases[0]->addWidgetRight(tPosZ);
    
    zoneCanvases[0]->addSpacer();
    
    vector<string> st;
    st.push_back("sphere");
    st.push_back("box");
    
    zoneCanvases[0]->addLabel("SHAPE TYPE", OFX_UI_FONT_SMALL);
    shapeRad = new ofxUIRadio("SHAPE_TYPE", st, OFX_UI_ORIENTATION_HORIZONTAL ,20, 20);
    shapeRad->activateToggle("sphere");
    
    zoneCanvases[0]->addWidgetRight(shapeRad);
    
    zoneCanvases[0]->addWidgetRight(new ofxUISpacer(1,20));
    
    eblTog = new ofxUIToggle("ENABLED", true, 20, 20, 0, 0, OFX_UI_FONT_SMALL);
    eblTog->setColorFill(ofxUIColor(255,0,0));
    zoneCanvases[0]->addWidgetRight(eblTog);
    
    zoneCanvases[0]->addWidgetRight(new ofxUISpacer(1,20));
    
    zoneCanvases[0]->addWidgetRight(new ofxUILabel("SOUNDFILE", OFX_UI_FONT_SMALL));
    sc2TextInput[2] = new ofxUITextInput("SOUNDFILE", "none", 200);
    zoneCanvases[0]->addWidgetRight(sc2TextInput[2]);
    
    zoneCanvases[0]->addSpacer();
    
    loopTog = new ofxUIToggle("LOOP", true, 20,20,0,0, OFX_UI_FONT_SMALL);
    playToEndTog = new ofxUIToggle("PLAY_TO_END", false, 20,20,0,0, OFX_UI_FONT_SMALL);
    invertedTog = new ofxUIToggle("INVERTED", false, 20,20,0,0, OFX_UI_FONT_SMALL);
    
    zoneCanvases[0]->addWidgetDown(loopTog);
    zoneCanvases[0]->addWidgetRight(playToEndTog);
    zoneCanvases[0]->addWidgetRight(invertedTog);
    
    // a bit crap
    //sensSlider = new ofxUISlider("SENSITIVITY", 0.75, 1.0, 1.0, slw, 10);
    //zoneCanvases[0]->addWidgetRight(sensSlider);
    
    repSlider = new ofxUISlider("MIN_REPLAY", 0, 5.0, 0.05, slw, 10);
    
    zoneCanvases[0]->addWidgetRight(repSlider);
    
    zoneCanvases[0]->addSpacer();
    
    synthTypeDisp = new ofxUILabel(0,0,200, "SYNTH TYPE: ", OFX_UI_FONT_SMALL);
    zoneCanvases[0]->addWidgetDown(synthTypeDisp);
    
    ofxUILabelButton * stb = (ofxUILabelButton *)zoneCanvases[0]->addWidgetRight(new ofxUILabelButton("ST_MINUS", true, 25));
    ofxUILabelButton * stc = (ofxUILabelButton *)zoneCanvases[0]->addWidgetRight(new ofxUILabelButton("ST_PLUS", true, 25));
    
    stb->setLabelText("-");
    stc->setLabelText("+");
    
    zoneCanvases[0]->addWidgetRight(new ofxUISpacer(1,20));
    
    dispSynthTog = new ofxUIToggle("EDIT_SYNTH_PARAMS", false, 20,20,0,0, OFX_UI_FONT_SMALL);
    
    zoneCanvases[0]->addWidgetRight(dispSynthTog);
    
    ofAddListener(zoneCanvases[0]->newGUIEvent,this,&testApp::s2Events);
    
    for(int i = 0; i < 3; i++)sc2TextInput[i]->setTriggerType(OFX_UI_TEXTINPUT_ON_FOCUS);
    
    
    //zone c1----------------------------------------------------
    
    radSlid = zoneCanvases[1]->addSlider("RADIUS", 0.05, 3.0, 0.1);
    
    ofAddListener(zoneCanvases[1]->newGUIEvent,this,&testApp::s2Events);
    
    
    //zone c2 ----------------------------------------------------------------
    
    xDimSlid= zoneCanvases[2]->addSlider("X_DIM", 0.05,10.0,0.5);
    yDimSlid= zoneCanvases[2]->addSlider("Y_DIM", 0.05,2.0,0.5);
    zDimSlid= zoneCanvases[2]->addSlider("Z_DIM", 0.05,10.0,0.5);
    
    ofAddListener(zoneCanvases[2]->newGUIEvent,this,&testApp::s2Events);
    
}

void testApp::setupDisplaySettings(){
    
    //--------------------------------------DISPLAY SETTINGS----------------------------------------------------------//
    
    displayTabBar = new ofxUITabBar();
    displayTabBar->setPosition(ofGetWidth() - 100, ofGetHeight() - tabBarHeight);
    displayTabBar->setColorFill(ofxUIColor(200));
    displayTabBar->setColorFillHighlight(ofxUIColor(255));
    displayTabBar->setColorBack(ofxUIColor(255, 20, 20, 150));
    ofAddListener(displayTabBar->newGUIEvent,this,&testApp::dispEvents);
    
    for(int i = 0; i < 2; i ++){
        
        displayCanvases[i] = new ofxUICanvas(ofGetWidth() - tabBarWidth, 0, tabBarWidth, 300);
        displayCanvases[i]->setColorFill(ofxUIColor(200));
        displayCanvases[i]->setColorFillHighlight(ofxUIColor(255));
        displayCanvases[i]->setColorBack(ofxUIColor(20, 20, 20, 150));
        
    }
    
    displayCanvases[0]->setName("2D");
    displayCanvases[0]->setVisible(false);
    
    ofAddListener(displayCanvases[0]->newGUIEvent,this,&testApp::dispEvents);
    displayTabBar->addCanvas(displayCanvases[0]);
    
    displayCanvases[1]->setName("3D");
    
    mouseTog = displayCanvases[1]->addToggle("MOUSE_INPUT", isCamMouse);
    
    displayCanvases[1]->addSpacer();
    
    displayCanvases[1]->addLabel("Z/X = ZOOM IN/OUT");
    displayCanvases[1]->addLabel("A/S = TRUCK LEFT/RIGHT");
    
    displayCanvases[1]->addButton("RESET_CAM", false);
    displayCanvases[1]->addButton("OVERHEAD", false);
    displayCanvases[1]->addButton("FROM_LEFT", false);
    displayCanvases[1]->addButton("FROM_RIGHT", false);
    
    displayCanvases[1]->addToggle("VIEW_USER", isViewSegPoints);
    displayCanvases[1]->addToggle("VIEW_COM", isViewCom);
    displayCanvases[1]->addToggle("VIEW_SCENE", isViewCScene);
    
    
    displayCanvases[1]->autoSizeToFitWidgets();
    ofAddListener(displayCanvases[1]->newGUIEvent,this,&testApp::dispEvents);
    displayTabBar->addCanvas(displayCanvases[1]);
    
    
}

//--------------------------------------------------------------

void testApp::saveSettings(string fn){
    
    ofxXmlSettings XML;
    
    if(fn.substr(fn.length()-4, 4) != ".xml")fn += ".xml";
    
    string filePath = "XMLs/" + fn;
    
    XML.addTag("DANCE_TRACKER");
    
    if(XML.pushTag("DANCE_TRACKER")){
        
        XML.addTag("MAIN_SETTINGS");
        
        if(XML.pushTag("MAIN_SETTINGS")){
            
            XML.addValue("KN_TILT", kinect.getTargetCameraTiltAngle());
            XML.addValue("FLOOR_Y", floorY);
            XML.addValue("SEG_FRESH", segThresh);
            XML.addValue("USER_HEIGHT", userHeight);
            XML.addValue("NEAR_THRESH", nearThresh);
            XML.addValue("FAR_THRESH", farThresh);
            XML.addValue("MIN_BLOB", minBlob);
            XML.addValue("MAX_BLOB", maxBlob);
            
            XML.popTag();
        }
        
        XML.addTag("SCENE_SETTINGS");
        
        if(XML.pushTag("SCENE_SETTINGS")){
            
            for(int sn = 0; sn < allScenes.size(); sn++){
                
                XML.addTag("SCENE");
                
                if(XML.pushTag("SCENE", sn)){
                    
                    XML.addValue("NAME", allScenes[sn]->getName());
                    XML.addValue("INDEX", allScenes[sn]->getIndex());
                    
                    int numZones = allScenes[sn]->getNumTriggerZones();
                    
                    for(int tz = 0; tz < numZones; tz++){
                        
                        ofPtr<triggerZone> z = allScenes[sn]->getTriggerZone(tz);
                        
                        XML.addTag("ZONE");
                        
                        if(XML.pushTag("ZONE", tz)){
                            
                            XML.addValue("NAME", z->getName());
                            
                            
                            XML.addValue("POS_X", z->getPos().x);
                            XML.addValue("POS_Y", z->getPos().y);
                            XML.addValue("POS_Z", z->getPos().z);
                            
                            XML.addValue("SHAPE", z->getShape());
                            
                            XML.addValue("DIM_X", z->getBoxDims().x);
                            XML.addValue("DIM_Y", z->getBoxDims().y);
                            XML.addValue("DIM_Z", z->getBoxDims().z);
                            XML.addValue("RADIUS", z->getRadius());
                            XML.addValue("SOUNDFILE", z->getSoundFileName());
                            XML.addValue("IS_LOOP", z->getIsLoop());
                            XML.addValue("IS_PLAY_TO_END", z->getIsPlayToEnd());
                            XML.addValue("INVERTED", z->getIsInverted());
                            XML.addValue("ENABLED", z->getIsEnabled());
                            //XML.addValue("SENSITIVITY", z->getSensitivity());
                            XML.addValue("MIN_REPLAY", z->getMinReplaySecs());
                            XML.addValue("SYNTH_TYPE", z->getSynthType());
                            
                            vector<synthParam> defSp = synthDictionary::getSynthParams(z->getSynthType());
                            
                            
                            for(int i = 0; i < defSp.size(); i++){
                                
                                synthParam sp = z->getSynthParam(i);
                                
                                XML.addTag(defSp[i].name);
                                
                                if(XML.pushTag(defSp[i].name)){
                                    
                                    XML.addValue("ABS_VAL", sp.abs_val);
                                    XML.addValue("MIN_VAL", sp.min_val);
                                    XML.addValue("MAX_VAL", sp.max_val);
                                    XML.addValue("MAP", (int)sp.map);
                                    
                                    XML.popTag();
                                }
                                
                            }
                            
                            
                            
                            //zone tag
                            XML.popTag();
                        }
                        
                        
                    }
                    
                    //scene tag
                    XML.popTag();
                }
                
                
            }
            
            //scene settings tag
            XML.popTag();
        }
        
        XML.addTag("BANK_SETTINGS");
        
        if(XML.pushTag("BANK_SETTINGS")){
            
            for(int i = 0; i < allBanks.size(); i ++){
            
                XML.addTag("BANK");
                
                if(XML.pushTag("BANK", i)){
                    
                    XML.setValue("NAME", allBanks[i]->name);
                    
                    int ns = allBanks[i]->scenes.size();
                    XML.setValue("NUM_ITEMS", ns);
                    
                    for(int j = 0; j < ns; j ++){
                    
                        XML.setValue("ITEM", allBanks[i]->scenes[j]->getIndex(), j);
                    
                    }
                    
                
                    //bank tag
                    XML.popTag();
                }
            
            }
            
            //Bank Settings tag
            XML.popTag();
        
        }
        
        //danceTracker tag
        XML.popTag();
    }
    
    XML.saveFile(filePath);
    
}

void testApp::loadSettings(string fn){
    
    ofxXmlSettings XML;
    
    if(fn.substr(fn.length()-4, 4) != ".xml")fn += ".xml";
    
    string filePath = "XMLs/" + fn;
    
    if(XML.loadFile(filePath)){
        
        if(XML.pushTag("DANCE_TRACKER")){
            
            if(XML.pushTag("MAIN_SETTINGS")){
                
                float kt = XML.getValue("KN_TILT", 0.0);
                kinect.setCameraTiltAngle(kt);
                sc1Sliders[0]->setValue(kt);
                
                nearThresh = XML.getValue("NEAR_THRESH", 0.5);
                sc1Sliders[1]->setValue(nearThresh);
                
                farThresh = XML.getValue("FAR_THRESH", 10.0);
                sc1Sliders[2]->setValue(farThresh);
                
                segThresh = XML.getValue("SEG_FRESH", 0.1);
                sc1Sliders[3]->setValue(segThresh);
                
                minBlob = XML.getValue("MIN_BLOB", 0.005);
                sc1Sliders[4]->setValue(minBlob);
                
                maxBlob = XML.getValue("MAX_BLOB", 10.0);
                sc1Sliders[5]->setValue(maxBlob);
                
                floorY = XML.getValue("FLOOR_Y", 1.0);
                sc1Sliders[6]->setValue(floorY);
                
                userHeight = XML.getValue("USER_HEIGHT", 1.8);
                sc1Sliders[7]->setValue(userHeight);
                
                XML.popTag();
            }
            
            
            if(XML.pushTag("SCENE_SETTINGS")){
                
                allScenes.clear();
                
                int numScenes = XML.getNumTags("SCENE");
                
                for(int sn = 0; sn < numScenes; sn++){
                    
                    if(XML.pushTag("SCENE", sn)){
                        
                        ofPtr<scene> nScene = ofPtr<scene>(new scene(mOsc));
                        
                        nScene->setName(XML.getValue("NAME", ""));
                        nScene->setIndex(XML.getValue("INDEX", -1));
                        
                        int numZones = XML.getNumTags("ZONE");
                        
                        for(int tz = 0; tz < numZones; tz++){
                            
                            if(XML.pushTag("ZONE", tz)){
                                
                                ofPtr<triggerZone> z = nScene->addTriggerZone(max(0,tz - 1));
                                
                                z->setName(XML.getValue("NAME", ""));
                                z->setShape(XML.getValue("SHAPE", 0));
                                z->setPosX(XML.getValue("POS_X", 0.0));
                                z->setPosY(XML.getValue("POS_Y", 0.0));
                                z->setPosZ(XML.getValue("POS_Z", 0.0));
                                z->setBoxDimsX( XML.getValue("DIM_X", 0.5));
                                z->setBoxDimsY( XML.getValue("DIM_Y", 0.5));
                                z->setBoxDimsZ( XML.getValue("DIM_Z", 0.5));
                                z->setRadius(XML.getValue("RADIUS",1.0));
                                z->setSoundFile(XML.getValue("SOUNDFILE", ""));
                                z->setIsPlayToEnd(XML.getValue("IS_PLAY_TO_END", false));
                                z->setIsInverted(XML.getValue("INVERTED", false));
                                z->setIsLoop(XML.getValue("IS_LOOP", true));
                                z->setIsEnabled(XML.getValue("ENABLED", false));
                                //z->setSensitivity(XML.getValue("SENSITIVITY", 1.0));
                                z->setMinReplaySecs(XML.getValue("MIN_REPLAY", 0.0));
                                z->setSynthType(XML.getValue("SYNTH_TYPE", 0));
                                
                                vector<synthParam> defSp = synthDictionary::getSynthParams(z->getSynthType());
                                
                                
                                for(int i = 0; i < defSp.size(); i++){
                                    
                                    
                                    synthParam sp = defSp[i];
                                    
                                    if(XML.pushTag(defSp[i].name)){
                                        
                                        sp.abs_val = XML.getValue("ABS_VAL", defSp[i].abs_val);
                                        sp.min_val = XML.getValue("MIN_VAL", defSp[i].min_val);
                                        sp.max_val = XML.getValue("MAX_VAL", defSp[i].max_val);
                                        sp.map = mapType(XML.getValue("MAP", defSp[i].map));
                                        
                                        z->setSynthParam(i, sp);
                                        
                                        XML.popTag();
                                    }
                                    
                                }
                                
                                
                                
                                //zone tag
                                XML.popTag();
                                
                                
                            }
                            
                            
                        }
                        
                        
                        allScenes.push_back(nScene);
                        
                        //scene tag
                        XML.popTag();
                    }
                    
                    
                    
                    
                }
                
                selScene = 0;
                currentScene = allScenes[selScene];
                sc2TextInput[0]->setTextString(currentScene->getName());
                //there will probably be more to update here later
                
                if(currentScene->getNumTriggerZones() > 0){
                    selZone = 0;
                    currentZone = currentScene->getTriggerZone(selZone);
                    updateTZGuiElements();
                }else{
                    //potentially refactor into a disableZoneElements
                    
                    selZone = 0;
                    sc2TextInput[1]->setTextString("none");
                    sc2TextInput[2]->setTextString("none");
                }
                
                //scene settings tag
                XML.popTag();
            }
            
            
            
            if(XML.pushTag("BANK_SETTINGS")){
                
                allBanks.clear();
                selBank = 0;
                bSelScene = 0;
                
                int nb = XML.getNumTags("BANK");
                
                for(int i = 0; i < nb; i ++){
                    
                    if(XML.pushTag("BANK", i)){
                        
                        ofPtr<bank> tb = ofPtr<bank>(new bank());
                        
                        tb->name = XML.getValue("NAME","none");
                        
                        int ns = XML.getValue("NUM_ITEMS", 0);
                        
                        
                        for(int j = 0; j < ns; j ++){
                            
                            int item = XML.getValue("ITEM", 0, j);
                            
                            vector<ofPtr<scene> >::iterator it = find_if(allScenes.begin(), allScenes.end(), matchSceneIndex(item));
                            tb->scenes.push_back((*it));
                            
                        }
                        
                        allBanks.push_back(tb);
                        //bank tag
                        XML.popTag();
                    }
                    
                }
                
                if(allBanks.size() > 0)currentBank = allBanks[0];
                
                updateBankElements();
                
                //Bank Settings tag
                XML.popTag();
                

     
            }else{
        
               
                allBanks.clear();
                ofPtr<bank> tb = ofPtr<bank>(new bank());
                allBanks.push_back(tb);
                currentBank = tb;
                selBank = 0;
                bSelScene = 0;
                updateBankElements();
            
            }
            

            
            //danceTracker Tag
            XML.popTag();
        }
        
        
        
        
        
    }else{
        
        cout << "failed /n";
        
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    mOsc->update();
    
    if(USE_KINECT){
        
        kinect.update();
        numBlankFrames += 1;
        
    }
    
    if(kinect.isFrameNew() || numBlankFrames == 10){
        
        numBlankFrames = 0;
        
        liveImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        calcQ();
        //rotate the pixels
        
        curDepths.clear();
        
        for(int y = 0; y < kinect.height; y += segRes) {
            
            for(int x = 0; x < kinect.width; x += segRes) {
                
                ofVec3f cur = kinect.getWorldCoordinateFor(x, y);
                
                ofVec3f curR = cur.getRotated(-qangle, qaxis);
                curR *= ofVec3f(1,-1,1);
                curDepths.push_back(curR);
                
            }
            
        }
        
        
        if(isBgRecorded){
            
            segment();
            if(isUser){
                
                analyseUser();
                currentScene->update(com, userHeight, userPixels);
            }else{
                currentScene->deselectAll();
            }
        }
        
    }else if(isFakeUser){
        
        userPixels.clear();
        
        for(int i = 0; i < numFakePoints; i ++){
            ofVec3f p(0,ofRandom(-fakeRadius, fakeRadius),0);
            p.rotate(ofRandom(0,360), ofRandom(0,360), ofRandom(0,360));
            p += fakePos;
            userPixels.push_back(p);
        }
        
        
        currentScene->update(fakePos, fakeRadius * 3, userPixels);
        
        
    }
    
    
    
    
    
}

void testApp::calcQ(){
    
    
    float cPitch = -ofRadToDeg(asin(kinect.getMksAccel().z/OFX_KINECT_GRAVITY));
    float cRoll = -ofRadToDeg(asin(kinect.getMksAccel().x/OFX_KINECT_GRAVITY));
    
    if(mPitches.size() > 10)mPitches.erase(mPitches.begin());
    if(mRolls.size() > 10)mRolls.erase(mRolls.begin());
    
    mPitches.push_back(cPitch);
    mRolls.push_back(cRoll);
    
    vector<float> tPitches = mPitches;
    vector<float> tRolls = mRolls;
    
    sort(tPitches.begin(), tPitches.end());
    sort(tRolls.begin(), tRolls.end());
    
    rRange = tRolls[tRolls.size()-1] - tRolls[0];
    pRange = tPitches[tPitches.size()-1] - tPitches[0];
    
    if(ofGetFrameNum() < 10){
        
        roll = cRoll;
        pitch = cPitch;
    }
    
    if(rRange > 4)roll = cRoll;
    if(pRange > 4)pitch = cPitch;
    
    ofVec3f Znormal(0, 0, 1);
    ofVec3f Xnormal(1, 0, 0);
    ofVec3f Ynormal(0, 1, 0);
    
    //Create the quaternions
    ofQuaternion qr(roll, Znormal);
    ofQuaternion qp(pitch, Xnormal);
    ofQuaternion qh(0, Ynormal);
    
    ofQuaternion qt = qr * qp * qh;
    
    qt.getRotate(qangle, qaxis);
    
    
}

void testApp::recordBg(){
    
    isBgRecorded = true;
    bgDepths = curDepths;
    
    float cFloor = 10;
    
    for(int i = 0 ; i < bgDepths.size(); i ++){
        
        if(bgDepths[i].z > nearThresh && bgDepths[i].z < farThresh){
            
            if(bgDepths[i].y < cFloor){
                cFloor = bgDepths[i].y;
            }
        }
    }
    
    floorY = cFloor;
    
}

void testApp::segment(){
    
    segImg.allocate(kinect.width/segRes, kinect.height/segRes);
    
    unsigned char * s_pix = segImg.getPixels();
    
    int counter = 0;
    
	for(int i = 0; i < curDepths.size(); i ++) {
        
        if(curDepths[i].z > nearThresh && curDepths[i].z < farThresh){
            
            //is inside range
            
            if(curDepths[i].z == 0){ //curDepth didn't record data
                s_pix[i] = 0;
            }else if(bgDepths[i].z == 0){ // bgDepth didn't record data
                s_pix[i] = 255;
            }else if(curDepths[i].z < bgDepths[i].z - segThresh){ //meets difference threshold
                s_pix[i] = 255;
            }else{
                s_pix[i] = 0; // doesn't meet thresholds
            }
            
        }else{
            s_pix[i] = 0;
        }
        
    }
    
    segImg.erode();
    segImg.flagImageChanged();
    segMask.allocate(segImg.getWidth(), segImg.getHeight());
    segMask = segImg;
    segMask.resize(kinect.width, kinect.height);
    
    int numPixels = kinect.width * kinect.height;
    
    cfFinder.findContours(segMask, numPixels * minBlob, numPixels * maxBlob, 1, false);
    segMask.set(0);
    
    //make a new mask based based on the contour
    
    if(cfFinder.blobs.size() > 0){
        
        isUser = true;
        
        CvPoint pts[cfFinder.blobs[0].nPts];
        
        for(int i = 0; i < cfFinder.blobs[0].nPts; i ++){
            
            pts[i].x = cfFinder.blobs[0].pts[i].x;
            pts[i].y = cfFinder.blobs[0].pts[i].y;
            
        }
        
        CvPoint * ppt[1] = { pts };
        
        int ppt_size[1] = { cfFinder.blobs[0].nPts };
        
        cvFillPoly(segMask.getCvImage(), ppt, ppt_size , 1, cvScalarAll(255));
        
    }else{
        isUser = false;
    }
    
    
}

void testApp::analyseUser(){
    
    userPixels.clear();
    
    //create a dense array of rotated userPixels
    
    unsigned char * s_pix = segMask.getPixels();
    
    ofVec3f total(0,0,0);
    
    for(int y = 0; y < kinect.height; y ++){
        
        for(int x = 0; x < kinect.width; x ++){
            
            if(s_pix[y * kinect.width + x] > 0){
                ofVec3f cur = kinect.getWorldCoordinateFor(x, y);
                ofVec3f curR = cur.getRotated(-qangle, qaxis);
                curR *= ofVec3f(1,-1,1);
                total += curR;
                userPixels.push_back(curR);
                
                
            }
            
        }
        
	}
    
    com = total/userPixels.size();
    
    
    
    vector<ofVec3f>::iterator it = remove_if(userPixels.begin(), userPixels.end(),findOutliers(com, userHeight));
    
    userPixels.erase(it, userPixels.end());
    
    
}

//---------------------------------------------------------------

void testApp::settingsEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    if(name == "Scene Setup"){
        
        updateZoneControls();
        
    }else{
        
        for(int i = 0; i < 3; i++)zoneCanvases[i]->setVisible(false);
        hideSynthCanvas();
        
    }
    
    if(name == "Setup Banks"){
        
        updateBankElements();
    
    }
    
    if(name == "Performance Mode"){
        updateBankElements();
        isPerfMode = true;
    }else{
    
        isPerfMode = false;
    }
    
    
    
}

void testApp::dispEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    if(name == "2D"){
        
        displayMode = DT_DM_2D;
        displayCanvases[0]->toggleVisible();
        cm.disableMouseInput();
        isCamMouse = false;
        isCamMouse = false;
        for(int i = 0; i < 3; i++)zoneCanvases[i]->setVisible(false);
        hideSynthCanvas();
        
    }
    if(name == "3D"){
        
        displayMode = DT_DM_3D;
        mouseTog->setValue(false);
        isCamKey = true;
        
        if(settingsTabBar->getActiveCanvas()){
            if(settingsTabBar->getActiveCanvas()->getName() == "Scene Setup")updateZoneControls();
        }
    }
    
    
    if(name == "MOUSE_INPUT"){
        (isCamMouse)? cm.disableMouseInput(): cm.enableMouseInput();
        isCamMouse = !isCamMouse;
    }
    
    if(name == "RESET_CAM"){
        
        cm.reset();
        cm.setTarget(ofVec3f(0, 0, 500));
        cm.setDistance(500);
        
        
    }
    
    
    if(name == "OVERHEAD"){
        
        cm.setPosition(1, 500, 500);
        cm.setTarget(ofVec3f(0, 0, 500));
    }
    
    
    if(name == "FROM_LEFT"){
        
        cm.setPosition(-500, 200, 500);
        cm.setTarget(ofVec3f(0, 0, 500));
        
    }
    
    if(name == "FROM_RIGHT"){
        
        cm.setPosition(500, 200, 500);
        cm.setTarget(ofVec3f(0, 0, 500));
        
    }
    
    
    
    if(name == "VIEW_USER"){
        ofxUIToggle *tog = (ofxUIToggle *) e.widget;
        isViewSegPoints = tog->getValue();
    }
    
    if(name == "VIEW_COM"){
        ofxUIToggle *tog = (ofxUIToggle *) e.widget;
        isViewCom = tog->getValue();
    }
    
    if(name == "VIEW_SCENE"){
        ofxUIToggle *tog = (ofxUIToggle *) e.widget;
        isViewCScene = tog->getValue();
    }
    
    
    
}

void testApp::s0Events(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    if(isMouseDown){
        if(name == "SAVE")saveSettings(fileNameInput->getTextString());
        if(name == "LOAD")loadSettings(fileNameInput->getTextString());
    }
    
	
}

void testApp::s1Events(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    if(name == "KN_TILT"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        kinectAngle = slider->getScaledValue();
        kinect.setCameraTiltAngle(kinectAngle);
    }
    
    if(name == "SEG_THRESH"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        segThresh = slider->getScaledValue();
    }
    
    if(name == "NEAR_THRESH"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        nearThresh = slider->getScaledValue();
    }
    
    if(name == "FAR_THRESH"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        farThresh = slider->getScaledValue();
    }
    
    if(name == "MIN_BLOB"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        minBlob = slider->getScaledValue();
    }
    
    if(name == "MAX_BLOB"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        maxBlob = slider->getScaledValue();
    }
    
    if(name == "MAX_BLOB"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        maxBlob = slider->getScaledValue();
    }
    
    if(name == "FLOOR_Y"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        floorY = slider->getScaledValue();
    }
    
    if(name == "USER_HEIGHT"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        userHeight = slider->getScaledValue();
    }
    
    if(name == "RECORD_BACKGROUND"){
        recordBg();
    }
    
    
    
}

void testApp::s2Events(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    isTextFocus = false;
    
    if(e.widget->getKind() == 12){ //no key events whilst text is focussed
        
        ofxUITextInput * t = (ofxUITextInput *) e.widget;
        
        if(t->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_UNFOCUS);
            isTextFocus = true;
        }else{
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_FOCUS);
            isTextFocus = false;
            
            if(name == "S_NAME"){
                currentScene->setName(t->getTextString());
            }
            
            if(currentScene->getNumTriggerZones() > 0){
                if(name == "Z_NAME"){
                    currentZone->setName(t->getTextString());
                }
                
                if(name == "SOUNDFILE"){
                    
                    currentZone->setSoundFile(t->getTextString());
                    if(currentZone->getIsAudioLoaded())
                        currentZone->setIsEnabled(currentZone->getIsEnabled());
                    else
                        currentZone->setIsEnabled(false);
                    
                    updateZoneControls();
                }
            }
            
            
            
        }
        
        
    }
    
    if(isMouseDown){
        
        if(name == "SCENE_PLUS"){
            
            currentScene->deselectAll();
            selScene = min(selScene + 1, (int)allScenes.size() - 1);
            currentScene = allScenes[selScene];
            sc2TextInput[0]->setTextString(currentScene->getName());
            selZone = 0;
            updateZoneControls();
            hideSynthCanvas();
            
        }
        
        if(name == "SCENE_MINUS"){
            
            currentScene->deselectAll();
            selScene = max(selScene - 1, 0);
            currentScene = allScenes[selScene];
            sc2TextInput[0]->setTextString(currentScene->getName());
            selZone = 0;
            updateZoneControls();
            hideSynthCanvas();
            
        }
        
        if(name == "CREATE_SCENE"){
            
            currentScene->deselectAll();
            ofPtr<scene> t = ofPtr<scene>(new scene(mOsc));
            allScenes.insert(allScenes.begin() + selScene + 1, t);
            selScene = min(selScene + 1, (int)allScenes.size() - 1);
            currentScene = t;
            selZone = 0;
            sc2TextInput[0]->setTextString(currentScene->getName());
            updateZoneControls();
            hideSynthCanvas();
        }
        
        if(name == "DELETE_SCENE"){
            
            if(allScenes.size() > 1){
                allScenes.erase(remove(allScenes.begin(), allScenes.end(), currentScene));
                selScene = max(selScene - 1, 0);
                currentScene = allScenes[selScene];
                sc2TextInput[0]->setTextString(currentScene->getName());
                selZone = 0;
                updateZoneControls();
                hideSynthCanvas();
                
            }
        }
        
        
        if(name == "CREATE_ZONE"){
            
            if(currentScene->getNumTriggerZones() > 0)currentZone->setIsSelected(false);
            currentZone = currentScene->addTriggerZone(selZone);
            selZone = min(selZone + 1, (int)currentScene->getNumTriggerZones() - 1);
            updateZoneControls();
            hideSynthCanvas();
        }
        
        if(currentScene->getNumTriggerZones() > 0){
            
            if(name == "ZONE_PLUS"){
                
                currentZone->setIsSelected(false);
                selZone = min(selZone + 1, (int)currentScene->getNumTriggerZones() - 1);
                currentZone = currentScene->getTriggerZone(selZone);
                updateZoneControls();
                hideSynthCanvas();
                
                
            }
            
            if(name == "ZONE_MINUS"){
                
                currentZone->setIsSelected(false);
                selZone = max(selZone - 1, 0);
                currentZone = currentScene->getTriggerZone(selZone);
                updateZoneControls();
                hideSynthCanvas();
                
            }
            
            
            if(name == "DELETE_ZONE"){
                
                currentScene->removeTriggerZone(selZone);
                selZone = max(selZone - 1, 0);
                if(currentScene->getNumTriggerZones() > 0)currentZone = currentScene->getTriggerZone(selZone);
                updateZoneControls();
                hideSynthCanvas();
                
            }
            
            
        }
        
        
    }
    
    if(currentScene->getNumTriggerZones() > 0 ){
        
        if(name == "sphere"){
            currentZone->setShape(0);
            updateZoneControls();
        }
        
        if(name == "box"){
            currentZone->setShape(1);
            updateZoneControls();
        }
        
        if(name == "EDIT_SYNTH_PARAMS"){
            
            //make synth panel visible and populate
            if(!isSynthView){
                populateSynthCanvas();
                isSynthView = true;
            }else{
                isSynthView = false;
            }
            
            synthCanvas->setVisible(isSynthView);
            
        }
        
        if(name == "RADIUS"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setRadius(slider->getScaledValue());
        }
        
        if(name == "T_POS_X"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setPosX(slider->getScaledValue());
        }
        
        if(name == "T_POS_Y"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setPosY(slider->getScaledValue());
        }
        
        if(name == "T_POS_Z"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setPosZ(slider->getScaledValue());
        }
        
        
        if(name == "X_DIM"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setBoxDimsX(slider->getScaledValue());
        }
        
        if(name == "Y_DIM"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setBoxDimsY(slider->getScaledValue());
        }
        
        if(name == "Z_DIM"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setBoxDimsZ(slider->getScaledValue());
        }
        
        if(name == "ENABLED"){
            ofxUIToggle *tog = (ofxUIToggle *) e.widget;
            if(currentZone->getIsAudioLoaded()){
                currentZone->setIsEnabled(tog->getValue());
            }else{
                tog->setValue(false);
            }
        }
        
        if(name == "LOOP"){
            ofxUIToggle *tog = (ofxUIToggle *) e.widget;
            currentZone->setIsLoop(tog->getValue());
        }
        
        if(name == "PLAY_TO_END"){
            ofxUIToggle *tog = (ofxUIToggle *) e.widget;
            currentZone->setIsPlayToEnd(tog->getValue());
        }
        
        if(name == "INVERTED"){
            ofxUIToggle *tog = (ofxUIToggle *) e.widget;
            currentZone->setIsInverted(tog->getValue());
        }
        
        if(name == "SENSITIVITY"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setSensitivity(slider->getScaledValue());
        }
        
        if(name == "MIN_REPLAY"){
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            currentZone->setMinReplaySecs(slider->getScaledValue());
        }
        
        
        if(isMouseDown){
            
            //buttons must go in here
            
            if(name == "ST_MINUS"){
                
                int i = max(0, currentZone->getSynthType() - 1);
                currentZone->setSynthType(i);
                updateZoneControls();
                hideSynthCanvas();
            }
            
            if(name == "ST_PLUS"){
                
                int i = min(ST_COUNT - 1, currentZone->getSynthType() + 1);
                currentZone->setSynthType(i);
                updateZoneControls();
                hideSynthCanvas();
                
            }
        }
        
        
    }
    
    
}

void testApp::s3Events(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    isTextFocus = false;
    
    if(e.widget->getKind() == 12){ //no key events whilst text is focussed
        
        ofxUITextInput * t = (ofxUITextInput *) e.widget;
        
        if(t->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_UNFOCUS);
            isTextFocus = true;
        }else{
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_FOCUS);
            isTextFocus = false;
            
            if(name == "B_NAME"){
                currentBank->name = t->getTextString();
            }
            
            
            
        }
        
        
    }
    
    if(isMouseDown){
    
        if(name == "BANK_MINUS"){
            
            selBank = max(selBank - 1, 0);
            currentBank = allBanks[selBank];
            bSelScene = 0;
            updateBankElements();
        
        }
        
        if(name == "BANK_PLUS"){
            
            selBank = min((int)allBanks.size() -1, selBank + 1);
            currentBank = allBanks[selBank];
            bSelScene = 0;
            updateBankElements();
        }
        
        if(name == "CREATE_BANK"){
            ofPtr<bank> b = ofPtr<bank>(new bank());
            allBanks.insert(allBanks.begin() + selBank + 1, b);
            currentBank = b;
            selBank += 1;
            bSelScene = 0;
            updateBankElements();
        }
        
        if(name == "DELETE_BANK"){
        
            if(allBanks.size() > 1){
                allBanks.erase(remove(allBanks.begin(), allBanks.end(), currentBank));
                selBank -= 1;
                currentBank = allBanks[selBank];
                bSelScene = 0;
                updateBankElements();
            }
        }
        
        if(name == "B_SCENE_PLUS"){
            
            currentScene->deselectAll();
            selScene = min(selScene + 1, (int)allScenes.size() - 1);
            currentScene = allScenes[selScene];
            selZone = 0;
            updateBankElements();
        
        }
        
        if(name == "B_SCENE_MINUS"){
        
            currentScene->deselectAll();
            selScene = max(selScene - 1, 0);
            currentScene = allScenes[selScene];
            selZone = 0;
            updateBankElements();

        }
        
        if(name == "ADD_SCENE_TO_BANK"){
        
            if(currentBank->scenes.size() > 0){
                currentBank->scenes.insert(currentBank->scenes.begin() + bSelScene + 1, currentScene);
                bSelScene += 1;
            }else{
                currentBank->scenes.push_back(currentScene);
            }
            updateBankElements();
        }
        
        if(name == "B_ITEM_MINUS"){
        
            bSelScene = max(0, bSelScene - 1);
             updateBankElements();
        }
        
        if(name == "B_ITEM_PLUS"){
            
            bSelScene = min((int)currentBank->scenes.size()- 1, bSelScene + 1);
             updateBankElements();
        
        }
        
        if(name == "REMOVE_ITEM"){
        
            if(currentBank->scenes.size() > 0){
            
                currentBank->scenes.erase(currentBank->scenes.begin()+ bSelScene);
                bSelScene = max(0, bSelScene - 1);
                updateBankElements();
            }
            
        }
        
        
    }
    
}

void testApp::s4Events(ofxUIEventArgs &e){

    string name = e.widget->getName();
    
    if(isMouseDown)perfChange(name);
    
    
}

void testApp::synthEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    int id = e.widget->getID();
    
    synthParam sp = currentZone->getSynthParam(id);
    
    if(name == "abs_val"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        sp.abs_val = slider->getScaledValue();
    }
    
    if(name == "min_val"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        sp.min_val = slider->getScaledValue();
    }
    
    if(name == "max_val"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        sp.max_val = slider->getScaledValue();
    }
    
    if(isMouseDown){
        if(name == "MINUS"){
            sp.map = mapType(max(0, sp.map - 1));
            mapTypeLabels[id]->setTextString("MAP_TYPE: " + synthDictionary::getMapString(sp.map));
            
        }
        
        if(name == "PLUS"){
            sp.map = mapType(min((int)MT_COUNT - 1, (int)sp.map + 1));
            mapTypeLabels[id]->setTextString("MAP_TYPE: " + synthDictionary::getMapString(sp.map));
        }
    }
    
    currentZone->setSynthParam(id, sp);
    
}

void testApp::fEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    ofxUISlider *slider = (ofxUISlider *) e.widget;
    
    if(name == "F_POS_X")fakePos.x = slider->getScaledValue();
    if(name == "F_POS_Y")fakePos.y = slider->getScaledValue();
    if(name == "F_POS_Z")fakePos.z = slider->getScaledValue();
    
    
}

//-----------------------------------------------------

void testApp::updateZoneControls(){
    
    if(currentScene->getNumTriggerZones() > 0){
        
        if(displayMode == DT_DM_3D){
            
            zoneCanvases[0]->setVisible(true);
            
            currentZone = currentScene->getTriggerZone(selZone);
            if(currentZone->getShape() == 0){
                zoneCanvases[1]->setVisible(true);
                zoneCanvases[2]->setVisible(false);
            }else{
                zoneCanvases[1]->setVisible(false);
                zoneCanvases[2]->setVisible(true);
            }
        }
        
        updateTZGuiElements();
        
    }else{
        for(int i = 0; i < 3; i++)zoneCanvases[i]->setVisible(false);
        sc2TextInput[1]->setTextString("none");
    }
    
    
}

void testApp::updateTZGuiElements(){
    
    sc2TextInput[1]->setTextString(currentZone->getName());
    sc2TextInput[2]->setTextString(currentZone->getSoundFileName());
    
    ofVec3f tp = currentZone->getPos();
    tPosX->setValue(tp.x);
    tPosY->setValue(tp.y);
    tPosZ->setValue(tp.z);
    
    radSlid->setValue(currentZone->getRadius());
    eblTog->setValue(currentZone->getIsEnabled());
    loopTog->setValue(currentZone->getIsLoop());
    playToEndTog->setValue(currentZone->getIsPlayToEnd());
    invertedTog->setValue(currentZone->getIsInverted());
    
    xDimSlid->setValue(currentZone->getBoxDims().x);
    yDimSlid->setValue(currentZone->getBoxDims().y);
    zDimSlid->setValue(currentZone->getBoxDims().z);
    (currentZone->getShape() == 0) ? shapeRad->activateToggle("sphere") : shapeRad->activateToggle("box");
    
    //sensSlider->setValue(currentZone->getSensitivity());
    repSlider->setValue(currentZone->getMinReplaySecs());
    synthTypeDisp->setLabel("SYNTH_TYPE: " + synthDictionary::getSynthString(currentZone->getSynthType()));
    currentZone->setIsSelected(true);
    
    
}

void testApp::populateSynthCanvas(){
    
    if(synthCanvas != NULL)delete synthCanvas;
    
    synthCanvas = new ofxUIScrollableCanvas(ofGetWidth()/2 - tabBarWidth * 0.9 , 0, tabBarWidth * 1.8, 200);
    synthCanvas->setColorFill(ofxUIColor(200));
    synthCanvas->setColorFillHighlight(ofxUIColor(255));
    synthCanvas->setColorBack(ofxUIColor(20, 20, 20, 150));
    
    synthCanvas->setVisible(false);
    synthCanvas->setSnapping(false);
    
    vector<synthParam> sp =  synthDictionary::getSynthParams(currentZone->getSynthType());
    
    mapTypeLabels.clear();
    
    for(int i = 0; i < sp.size(); i ++){
        
        synthCanvas->addLabel(sp[i].name, OFX_UI_FONT_MEDIUM);
        
        synthParam c_sp = currentZone->getSynthParam(i);
        
        ofxUISlider * absl = new ofxUISlider("abs_val" , sp[i].sl_min, sp[i].sl_max, c_sp.abs_val , 100,20);
        synthCanvas->addWidgetDown(absl);
        absl->setID(i);
        ofxUISlider * mvsl = new ofxUISlider("min_val" , sp[i].sl_min, sp[i].sl_max, c_sp.min_val , 100,20);
        synthCanvas->addWidgetRight(mvsl);
        mvsl->setID(i);
        ofxUISlider * mxsl = new ofxUISlider("max_val" , sp[i].sl_min, sp[i].sl_max, c_sp.max_val , 100,20);
        synthCanvas->addWidgetRight(mxsl);
        mxsl->setID(i);
        
        ofxUITextArea * l = new ofxUITextArea("MAP_TYPE", "MAP_TYPE: " + synthDictionary::getMapString(c_sp.map), 180,20,0,0, OFX_UI_FONT_SMALL);
        synthCanvas->addWidgetRight(l);
        l->setID(i);
        mapTypeLabels.push_back(l);
        
        
        ofxUILabelButton * zb = (ofxUILabelButton *)synthCanvas->addWidgetRight(new ofxUILabelButton("MINUS", true, 25));
        ofxUILabelButton * zc = (ofxUILabelButton *)synthCanvas->addWidgetRight(new ofxUILabelButton( "PLUS", true ,25));
        
        zb->setLabelText("-");
        zc->setLabelText("+");
        
        zb->setID(i);
        zc->setID(i);
        
        synthCanvas->addWidgetDown(new ofxUILabel("", OFX_UI_FONT_SMALL));
        synthCanvas->addSpacer();
        
    }
    
    ofAddListener(synthCanvas->newGUIEvent,this,&testApp::synthEvents);
}

void testApp::hideSynthCanvas(){
    
    if(isSynthView){
        isSynthView = false;
        synthCanvas->setVisible(false);
        dispSynthTog->setValue(false);
    }
    
}

void testApp::updateBankElements(){

    
    sc3TextInput->setTextString(currentBank->name);
    perfBankText->setTextString(currentBank->name);
    sceneText->setTextString(currentScene->getName());
    
    string b_str = "\n\n";
    
    for(int i = 0; i < currentBank->scenes.size(); i++){
        
        b_str += "   " + currentBank->scenes[i]->getName();
        b_str += (i == bSelScene) ? "   <--- \n\n" : "\n\n";
    
    }
    
    bankText->setTextString(b_str);
    perfSceneText->setTextString(b_str);
        

}

void testApp::perfChange(string name){

    if(name == "BANK_MINUS"){
        
        selBank = max(selBank - 1, 0);
        
        if(currentBank != allBanks[selBank]){
            currentBank = allBanks[selBank];
            bSelScene = 0;
            if(currentBank->scenes.size() > 0){
                currentScene = currentBank->scenes[bSelScene];
            }
            updateBankElements();
        }
        
    }
    
    if(name == "BANK_PLUS"){
        
        selBank = min((int)allBanks.size() -1, selBank + 1);
        
        if(currentBank != allBanks[selBank]){
            currentBank = allBanks[selBank];
            bSelScene = 0;
            if(currentBank->scenes.size() > 0){
                currentScene = currentBank->scenes[bSelScene];
            }
            updateBankElements();
        }
    }
    
    if(name == "B_ITEM_MINUS"){
        
        bSelScene = max(0, bSelScene - 1);
        if(currentBank->scenes.size() > 0){
            currentScene = currentBank->scenes[bSelScene];
        }
        updateBankElements();
    }
    
    if(name == "B_ITEM_PLUS"){
        
        bSelScene = min((int)currentBank->scenes.size()- 1, bSelScene + 1);
        if(currentBank->scenes.size() > 0){
            currentScene = currentBank->scenes[bSelScene];
        }
        updateBankElements();
        
    }

}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(0);
    ofBackground(120);
    
    ofDrawBitmapString("FPS: " +  ofToString(ofGetFrameRate(), 2), 20,20);
    
    
    if(displayMode == DT_DM_3D){
        
        cm.begin();
        
        
        glEnable(GL_DEPTH_TEST);
        
        ofScale(-100,100,100);
        
        ofNoFill();
        
        // topLight.draw();
        // ambientLight.enable();
        // topLight.enable();
        
        ofPushMatrix();
        ofScale(0.05, 0.05, 0.05);
        ofDrawGrid();
        ofPopMatrix();
        
        
        ofPushMatrix();
        ofTranslate(0, floorY, 0);
        drawFloor();
        ofPopMatrix();
        
        
        ofNoFill();
        ofSetColor(255,255,0);
        
        ofPushMatrix();
        ofRotate(qangle, qaxis.x, qaxis.y, qaxis.z);
        ofScale(2,0.5,0.5);
        ofBox(0.15);
        ofPopMatrix();
        
        
        if(isViewSegPoints){
            if(isUser || isFakeUser)drawUserPointCloud();
        }else{
            drawScenePointCloud();
        }
        
        if(isViewCom){
            ofNoFill();
            ofSetColor(0, 255, 255);
            ofSphere(com.x, com.y, com.z, userHeight/2);
        }
        
        
        if(isViewCScene){
            
            currentScene->draw(cm.getPosition());
        }
        
        cm.end();
        
        glDisable(GL_DEPTH_TEST);
        
    }
    
    
    if(displayMode == DT_DM_2D){
        
        ofSetColor(255);
        
        ofPushMatrix();
        ofTranslate(ofGetWidth() - 750, 50);
        liveImg.draw(0,0,320,240);
        ofTranslate(0, 260);
        ofDrawBitmapString("live depthMap", 0,0);
        ofTranslate(0, 40);
        segMask.draw(0,0,320,240);
        ofTranslate(0, 260);
        ofDrawBitmapString("segmented depthMap", 0,0);
        ofTranslate(0, 40);
        
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(ofGetWidth() - 360, 50);
        ofFill();
        ofSetColor(50);
        ofRect(0,0,320,240);
        cfFinder.draw(0,0,320,240);
        ofTranslate(0, 260);
        ofSetColor(255);
        ofDrawBitmapString("contour analysis", 0,0);
        ofPopMatrix();
        
    }
    
}

void testApp::drawScenePointCloud() {
    
	glBegin(GL_POINTS);
    glColor3ub(0, 0, 0);
    
    for(int i = 0; i < curDepths.size(); i ++){
        
        glVertex3f(curDepths[i].x, curDepths[i].y, curDepths[i].z);
        
    }
    
	glEnd();
}

void testApp::drawUserPointCloud() {
    
	glBegin(GL_POINTS);
    glColor3ub(0, 0, 0);
    
    
    for(int i = 0; i < userPixels.size(); i ++){
        
        glVertex3f(userPixels[i].x, userPixels[i].y, userPixels[i].z);
        
	}
    
	glEnd();
    
    
    
}

void testApp::drawFloor(){
    
    ofSetColor(0);
    
    float spacing = 0.5;
    float numLines = 20;
    float sx = -spacing * numLines * 0.5;
    float sz = 0;
    
    for(int i = 0; i < numLines; i ++){
        
        ofLine(sx + spacing * i, 0, 0, sx + spacing * i, 0, 10); //across the x -axis
        ofLine(-5, 0 , sz + spacing * i ,5, 0, sz + spacing * i ); //across the z -axis
        
    }
    
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(isTextFocus)return; //no key events whilst textFocus
    
    switch(key){
            
        case ' ':
            settingsTabBar->toggleVisible();
            isSettingsGui = !isSettingsGui;
            if(!isSettingsGui)
                for(int i = 0; i < 3; i++)zoneCanvases[i]->setVisible(false);
            else
                if(settingsTabBar->getActiveCanvas()->getName() == "Scene Setup")updateZoneControls();
            break;
            
        case OF_KEY_RETURN:
            displayTabBar->toggleVisible();
            isDisplayGui = !isDisplayGui;
            break;
            
        case 'F':
            isFakeUser = !isFakeUser;
            fakeCanvas->setVisible(isFakeUser);
            break;
            
            
    }
    
    
    if(isCamKey) {
        
        switch(key){
                
            case 'a':
            {
                
                cm.truck(-10);
                
                
            }
                break;
                
            case 's':
            {
                
                cm.truck(10);
                
            }
                break;
                
                
            case 'z':
            {
                
                cm.dolly(10);
                
            }
                break;
                
                
            case 'x':
            {
                
                cm.dolly(-10);
                
            }
                break;
                
                
                
        }
        
        
    }
    
    if(isPerfMode){
    
        if(key == OF_KEY_UP)perfChange("B_ITEM_MINUS");
        
        if(key == OF_KEY_DOWN)perfChange("B_ITEM_PLUS");
        
        if(key == OF_KEY_LEFT)perfChange("BANK_MINUS");
        
        if(key == OF_KEY_RIGHT)perfChange("BANK_PLUS");
            
    }
    
}

void testApp::keyReleased(int key){
    
}

void testApp::mouseMoved(int x, int y ){
    
}

void testApp::mouseDragged(int x, int y, int button){
    
}

void testApp::mousePressed(int x, int y, int button){
    
    isMouseDown = true;
    
}

void testApp::mouseReleased(int x, int y, int button){
    
    isMouseDown = false;
}

void testApp::windowResized(int w, int h){
    
    displayTabBar->setPosition(ofGetWidth() - 100 ,ofGetHeight() - tabBarHeight);
    settingsTabBar->setPosition(0,ofGetHeight() - tabBarHeight);
    
    for(int i = 0; i < 2; i ++){
        
        displayCanvases[i]->setPosition(ofGetWidth() - tabBarWidth, 0);
        
    }
    
}

void testApp::gotMessage(ofMessage msg){
    
}

void testApp::dragEvent(ofDragInfo dragInfo){
    
}



void testApp::exit(){
    
    kinect.close();
	delete settingsTabBar;
    delete displayTabBar;
    
    for(int i = 0; i < NUM_SETTINGS_CANVASES; i ++)delete settingsCanvases[i];
    for(int i = 0; i < 2; i++)delete displayCanvases[i];
    for(int i = 0; i < 3; i++)delete zoneCanvases[i];
    
    if(synthCanvas != NULL)delete synthCanvas;
    
    mOsc->sendExit();
    
}

