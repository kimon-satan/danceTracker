#include "testApp.h"

#define OFX_KINECT_GRAVITY 9.80665


/*
 
 TODO:
 
 The testApp file is in desparate need of refactoring
 - AllScenes should be absorbed into a specialised bank
 - A separate manager class leaving only the gui functions here
 
 
 
 */

//--------------------------------------------------------------
void testApp::setup(){
    
    
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    
    numBlankFrames = 0;
    
    if(USE_KINECT){
        kinect.clear();
        kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
        //kinect.setVerbose(true);
        kinect.open();
        
        
        if(kinect.isConnected()){
            // zero the tilt on startup
            kinectAngle = 0;
            kinect.setCameraTiltAngle(kinectAngle);
            //kinect.getCalibration().setClippingInCentimeters(50,1500);
            kinect.setDepthClipping(500,10000);
        }
        
    }
    
    mOsc = ofPtr<oscManager>(new oscManager());
    m_bankManager = ofPtr<bankManager>(new bankManager(mOsc));
    
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
    isSaveDialog = false; isLoadDialog = false;
    
    cm.disableMouseInput();
    
    
    setupGui();
    updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
    
    isFakeUser = false;
    
}

void testApp::setupGui(){
    
    tabBarWidth = 320;
    tabBarHeight = 120;
    
    isSettingsGui = true;
    isDisplayGui = true;
    
    setupGeneralSettings();
    setupZonePanels();
    setupDisplaySettings();
    
    dialog = new ofxUICanvas(0, 150, tabBarWidth, 100);
    dialog->setColorFill(ofxUIColor(200));
    dialog->setColorFillHighlight(ofxUIColor(255));
    dialog->setColorBack(ofxUIColor(20, 20, 20, 150));
    confLab = dialog->addLabel("CONFIRM");
    dialog->addLabelButton("OK", true);
    dialog->addLabelButton("CANCEL", true);
    dialog->setVisible(false);
    
    ofAddListener(dialog->newGUIEvent, this, &testApp::s0Events);
    
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
    sc1Sliders[4] = settingsCanvases[1]->addSlider("MIN_BLOB", 0.0, 0.1, minBlob, slLength, slHeight);
    sc1Sliders[4]->setLabelPrecision(3);
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
    
    
    ofxUILabelButton * sb = (ofxUILabelButton *)settingsCanvases[2]->addWidgetDown(new ofxUILabelButton("SCENE_MINUS", true, 25));
    ofxUILabelButton * sc = (ofxUILabelButton *)settingsCanvases[2]->addWidgetRight(new ofxUILabelButton("SCENE_PLUS", true, 25));
    
    sb->setLabelText("-");
    sc->setLabelText("+");
    
    
    settingsCanvases[2]->addButton("CREATE_SCENE", false);
    settingsCanvases[2]->addButton("DELETE_SCENE", false);
    settingsCanvases[2]->addButton("COPY_SCENE", false);
    
    
    fInSlid = new ofxUISlider("FADE_IN", 0.01, 1.0, 0.01, tabBarWidth, 20);
    fOutSlid = new ofxUISlider("FADE_OUT", 0.01, 10.0, 0.01, tabBarWidth, 20);
    
    settingsCanvases[2]->addWidgetDown(fInSlid);
    settingsCanvases[2]->addWidgetDown(fOutSlid);
    
    settingsCanvases[2]->addSpacer();
    
    settingsCanvases[2]->addLabel("SELECTED_ZONE");
    sc2TextInput[1] = settingsCanvases[2]->addTextInput("Z_NAME", "none");
    
    ofxUILabelButton * zb = (ofxUILabelButton *)settingsCanvases[2]->addWidgetDown(new ofxUILabelButton("ZONE_MINUS", true, 25));
    ofxUILabelButton * zc = (ofxUILabelButton *)settingsCanvases[2]->addWidgetRight(new ofxUILabelButton("ZONE_PLUS", true, 25));
    
    zb->setLabelText("-");
    zc->setLabelText("+");
    
    settingsCanvases[2]->addButton("CREATE_ZONE", false);
    settingsCanvases[2]->addButton("DELETE_ZONE", false);
    settingsCanvases[2]->addButton("COPY_ZONE", false);
    
    
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
    
    sceneText = new ofxUITextArea("SCENE_TEXT", "none", 250, 20,0,0,OFX_UI_FONT_SMALL);
    
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
        
        
        m_bankManager->saveSettings(XML);
        
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
            
            
            //load settings call
            m_bankManager->loadSettings(XML);
            
            updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
            updateBankElements(m_bankManager->getCurrentBank(), m_bankManager->getCurrentScene());
            
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
    
    if((kinect.isFrameNew() || numBlankFrames == 10) && kinect.isConnected()&& !isFakeUser){
        
        numBlankFrames = 0;
        
        liveImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        calcQ();
        //rotate the pixels
        
        curDepths.clear();
        
        for(int y = 0; y < kinect.height; y += segRes) {
            
            for(int x = 0; x < kinect.width; x += segRes) {
                
                ofVec3f cur = kinect.getWorldCoordinateAt(x, y);
                
                ofVec3f curR = cur.getRotated(-qangle, qaxis);
                curR *= ofVec3f(0.001,-0.001,0.001); //new kinect lib in mm
                curDepths.push_back(curR);
                
            }
            
        }
        
        
        if(isBgRecorded){
            
            segment();
            if(isUser){
                
                analyseUser();
                m_bankManager->update(com, userHeight, userPixels);
            }else{
                m_bankManager->unTriggerAll();
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
        
        m_bankManager->update(fakePos, fakeRadius * 3, userPixels);
        
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
        
        if(bgDepths[i].z > nearThresh && bgDepths[i].z < farThresh ){
            
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
                ofVec3f cur = kinect.getWorldCoordinateAt(x, y);
                ofVec3f curR = cur.getRotated(-qangle, qaxis);
                curR *= ofVec3f(0.001,-0.001,0.001);
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
    
    isSaveDialog = false; isLoadDialog = false;
    dialog->setVisible(false);
    
    
    if(name == "Scene Setup"){
        
        m_bankManager->selectFirstZone();
        updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
        
    }else{
        
        for(int i = 0; i < 3; i++)zoneCanvases[i]->setVisible(false);
        hideSynthCanvas();
        
    }
    
    if(name == "Setup Banks"){
        
        updateBankElements(m_bankManager->getCurrentBank(), m_bankManager->getCurrentScene());
        m_bankManager->deselectCurrentZone();
        
    }
    
    if(name == "Performance Mode"){
        
        m_bankManager->resetForPerformance();
        updateBankElements(m_bankManager->getCurrentBank(), m_bankManager->getCurrentScene());;
        isPerfMode = true;
        
    }else if(isPerfMode){
        
        m_bankManager->resetScenes();
        
        updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
        
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
            if(settingsTabBar->getActiveCanvas()->getName() == "Scene Setup")updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
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
    
    if(e.widget->getKind() == 12){ //no key events whilst text is focussed
        
        ofxUITextInput * t = (ofxUITextInput *) e.widget;
        
        if(t->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_UNFOCUS);
            isTextFocus = true;
        }else{
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_FOCUS);
            isTextFocus = false;
        }
        
    }
    
    
    if(isMouseDown){
        
        if(!isSaveDialog && !isLoadDialog){
            if(name == "SAVE"){
                confLab->setLabel("CONFIRM SAVE FILE");
                isSaveDialog = true; isLoadDialog = false;
                dialog->setVisible(true);
            }
            
            if(name == "LOAD"){
                isLoadDialog = true; isSaveDialog = false;
                confLab->setLabel("CONFIRM LOAD FILE");
                dialog->setVisible(true);
            }
        }else{
            
            if(isSaveDialog){
                
                if(name == "OK"){
                    saveSettings(fileNameInput->getTextString());
                    dialog->setVisible(false);
                    isSaveDialog = false;
                }else{
                    isSaveDialog = false;
                    dialog->setVisible(false);
                }
            }
            
            if(isLoadDialog){
                
                if(name == "OK"){
                    loadSettings(fileNameInput->getTextString());
                    dialog->setVisible(false);
                    isLoadDialog = false;
                }else{
                    isLoadDialog = false;
                    dialog->setVisible(false);
                }
            }
            
        }
        
        
        
        
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

    bool isHideSC = false;
    string name = e.widget->getName();
    ofxUISlider *slider = (ofxUISlider *) e.widget;
    ofxUIToggle *tog = (ofxUIToggle *) e.widget;
    
    isTextFocus = false;
    
    if(e.widget->getKind() == 12){ //no key events whilst text is focussed
        
        ofxUITextInput * t = (ofxUITextInput *) e.widget;
        
        if(t->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_UNFOCUS);
            isTextFocus = true;
        }else{
            t->setTriggerType(OFX_UI_TEXTINPUT_ON_FOCUS);
            isTextFocus = false;
            
            if(name == "S_NAME")m_bankManager->setCSceneName(t->getTextString());
            if(name == "Z_NAME")m_bankManager->setCZoneName(t->getTextString());
            if(name == "SOUNDFILE")m_bankManager->setCZoneSoundFile(t->getTextString());
            
            updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
            
        }
        
    }
    
    if(name == "FADE_IN")m_bankManager->setCSceneFadeIn(slider->getScaledValue());
    if(name == "FADE_OUT")m_bankManager->setCSceneFadeOut(slider->getScaledValue());
    
    
    
    //all actions in which the synthCanvas is hidden

    if(isMouseDown){
        //problem of double trigger
        if(name == "SCENE_PLUS"){isHideSC = true; m_bankManager->incrementScene();}
        if(name == "SCENE_MINUS"){isHideSC = true; m_bankManager->decrementScene();}
        if(name == "CREATE_SCENE"){isHideSC = true; m_bankManager->createScene();}
        if(name == "DELETE_SCENE"){isHideSC = true; m_bankManager->deleteScene();}
        if(name == "COPY_SCENE"){isHideSC = true; m_bankManager->copyScene();}
        if(name == "CREATE_ZONE"){isHideSC = true; m_bankManager->createZone();}
        if(name == "COPY_ZONE"){isHideSC = true; m_bankManager->copyZone();}
        if(name == "ZONE_PLUS"){isHideSC = true; m_bankManager->incrementZone();}
        if(name == "ZONE_MINUS"){isHideSC = true; m_bankManager->decrementZone();}
        if(name == "DELETE_ZONE"){isHideSC = true; m_bankManager->deleteZone();}
        if(name == "ST_MINUS"){isHideSC = true; m_bankManager->decCZoneSynthType();}
        if(name == "ST_PLUS"){isHideSC = true; m_bankManager->incCZoneSynthType();}
    }
    if(name == "sphere")m_bankManager->setCZoneShape(0);
    if(name == "box")m_bankManager->setCZoneShape(1);
    if(name == "ENABLED")tog->setValue(m_bankManager->setCZoneEnabled(tog->getValue()));
    if(name == "LOOP")m_bankManager->setCZoneLoop(tog->getValue());
    if(name == "PLAY_TO_END")m_bankManager->setCZonePlayToEnd(tog->getValue());
    if(name == "INVERTED")m_bankManager->setCZoneInverted(tog->getValue());
    if(name == "MIN_REPLAY")m_bankManager->setCZoneMinReplay(slider->getScaledValue());
    if(name == "RADIUS")m_bankManager->setCZoneRadius(slider->getScaledValue());
    
    if(name == "EDIT_SYNTH_PARAMS"){
        
        //make synth panel visible and populate
        if(!isSynthView){
            populateSynthCanvas(m_bankManager->getCurrentZone());
            isSynthView = true;
        }else{
            isSynthView = false;
        }
        
        synthCanvas->setVisible(isSynthView);
        
    }
    
    if(name.substr(0,4) == "T_POS"){
        ofVec3f p = m_bankManager->getCurrentZone()->getPos();
        if(name.substr(6) == "X")p.x = slider->getScaledValue();
        if(name.substr(6) == "Y")p.y = slider->getScaledValue();
        if(name.substr(6) == "Z")p.z = slider->getScaledValue();
        m_bankManager->setCZonePosition(p);
    }
    
    if(name.substr(2,3) == "DIM"){
        ofVec3f bd = m_bankManager->getCurrentZone()->getBoxDims();
        if(name.substr(0,1) == "X")bd.x = slider->getScaledValue();
        if(name.substr(0,1) == "Y")bd.y = slider->getScaledValue();
        if(name.substr(0,1) == "Z")bd.z = slider->getScaledValue();
        m_bankManager->setCZoneBoxDims(bd);
    }
    
    updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
    if(isHideSC)hideSynthCanvas();
    
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
            if(name == "B_NAME")m_bankManager->getCurrentBank()->name = t->getTextString();
        }
    }
    
    if(isMouseDown){
        
        if(name == "BANK_MINUS")m_bankManager->decrementBank();
        if(name == "BANK_PLUS")m_bankManager->incrementBank();
        if(name == "CREATE_BANK")m_bankManager->createBank();
        if(name == "DELETE_BANK")m_bankManager->deleteBank();
        if(name == "B_SCENE_PLUS")m_bankManager->incrementScene();
        if(name == "B_SCENE_MINUS")m_bankManager->decrementScene();
        if(name == "ADD_SCENE_TO_BANK")m_bankManager->bankAddScene();
        if(name == "B_ITEM_MINUS")m_bankManager->bankDecrementScene();
        if(name == "B_ITEM_PLUS")m_bankManager->bankIncrementScene();
        if(name == "REMOVE_ITEM")m_bankManager->bankRemoveScene();
        
        updateBankElements(m_bankManager->getCurrentBank(), m_bankManager->getCurrentScene());;
        
    }
    
}

void testApp::s4Events(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    if(isMouseDown)perfChange(name);
    
    
}

void testApp::synthEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    int id = e.widget->getID();
    
    synthParam sp = m_bankManager->getCurrentZone()->getSynthParam(id);
    
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
    
    m_bankManager->setCZoneSynthParam(id, sp);
    
}

void testApp::fEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    ofxUISlider *slider = (ofxUISlider *) e.widget;
    
    if(name == "F_POS_X")fakePos.x = slider->getScaledValue();
    if(name == "F_POS_Y")fakePos.y = slider->getScaledValue();
    if(name == "F_POS_Z")fakePos.z = slider->getScaledValue();
    
    
}

//-----------------------------------------------------

void testApp::updateSceneControls(ofPtr<scene> s, ofPtr<triggerZone> zn){
    
    sc2TextInput[0]->setTextString(s->getName());
    
    fInSlid->setValue(s->getFadeIn());
    fOutSlid->setValue(s->getFadeOut());
    
    if(s->getNumTriggerZones() > 0){
        
        if(displayMode == DT_DM_3D){
            
            zoneCanvases[0]->setVisible(true);
            
            if(zn->getShape() == 0){
                zoneCanvases[1]->setVisible(true);
                zoneCanvases[2]->setVisible(false);
            }else{
                zoneCanvases[1]->setVisible(false);
                zoneCanvases[2]->setVisible(true);
            }
        }
        
        updateTZGuiElements(zn);
        
    }else{
        for(int i = 0; i < 3; i++)zoneCanvases[i]->setVisible(false);
        sc2TextInput[1]->setTextString("none");
    }
    
    
}

void testApp::updateTZGuiElements(ofPtr<triggerZone> zn){
    
    sc2TextInput[1]->setTextString(zn->getName());
    sc2TextInput[2]->setTextString(zn->getSoundFileName());
    
    ofVec3f tp = zn->getPos();
    tPosX->setValue(tp.x);
    tPosY->setValue(tp.y);
    tPosZ->setValue(tp.z);
    
    radSlid->setValue(zn->getRadius());
    eblTog->setValue(zn->getIsEnabled());
    loopTog->setValue(zn->getIsLoop());
    playToEndTog->setValue(zn->getIsPlayToEnd());
    invertedTog->setValue(zn->getIsInverted());
    
    xDimSlid->setValue(zn->getBoxDims().x);
    yDimSlid->setValue(zn->getBoxDims().y);
    zDimSlid->setValue(zn->getBoxDims().z);
    (zn->getShape() == 0) ? shapeRad->activateToggle("sphere") : shapeRad->activateToggle("box");
    
    //sensSlider->setValue(zn->getSensitivity());
    repSlider->setValue(zn->getMinReplaySecs());
    synthTypeDisp->setLabel("SYNTH_TYPE: " + synthDictionary::getSynthString(zn->getSynthType()));
    zn->setIsSelected(true);
    
    
}

void testApp::populateSynthCanvas(ofPtr<triggerZone> zn){
    
    if(synthCanvas != NULL)delete synthCanvas;
    
    synthCanvas = new ofxUIScrollableCanvas(ofGetWidth()/2 - tabBarWidth * 0.9 , 0, tabBarWidth * 1.8, 200);
    synthCanvas->setColorFill(ofxUIColor(200));
    synthCanvas->setColorFillHighlight(ofxUIColor(255));
    synthCanvas->setColorBack(ofxUIColor(20, 20, 20, 150));
    
    synthCanvas->setVisible(false);
    synthCanvas->setSnapping(false);
    
    vector<synthParam> sp =  synthDictionary::getSynthParams(zn->getSynthType());
    
    mapTypeLabels.clear();
    
    for(int i = 0; i < sp.size(); i ++){
        
        synthCanvas->addLabel(sp[i].name, OFX_UI_FONT_MEDIUM);
        
        synthParam c_sp = zn->getSynthParam(i);
        
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

void testApp::updateBankElements(ofPtr<bank> b, ofPtr<scene> s){
    
    
    sc3TextInput->setTextString(b->name);
    perfBankText->setTextString(b->name);
    string nm = s->getName();
    if(nm.length() > 30)nm.resize(30);
    sceneText->setTextString(nm);
    
    string b_str = "\n\n";
    
    for(int i = 0; i < b->scenes.size(); i++){
        
        b_str += "   " + b->scenes[i]->getName();
        b_str += (i == b->selSceneIndex) ? "   <--- \n\n" : "\n\n";
        
    }
    
    bankText->setTextString(b_str);
    perfSceneText->setTextString(b_str);
    
    
}

void testApp::perfChange(string name){
    
    if(name == "BANK_MINUS")m_bankManager->perfBankDecrement();
    if(name == "BANK_PLUS")m_bankManager->perfBankIncrement();
    if(name == "B_ITEM_MINUS")m_bankManager->perfBankDecrement();
    if(name == "B_ITEM_PLUS")m_bankManager->perfBankIncrement();
    
    updateBankElements(m_bankManager->getCurrentBank(), m_bankManager->getCurrentScene());
    
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
        
        
        if(isViewCScene)m_bankManager->getCurrentScene()->draw(cm.getPosition());
        
        
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
                if(settingsTabBar->getActiveCanvas()->getName() == "Scene Setup")updateSceneControls(m_bankManager->getCurrentScene(), m_bankManager->getCurrentZone());
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

