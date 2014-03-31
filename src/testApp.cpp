#include "testApp.h"



/*
 
 TODO:
 
 The testApp file is in desparate need of refactoring
 - AllScenes should be absorbed into a specialised bank
 
 */

//--------------------------------------------------------------
void testApp::setup(){
    
    
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    
    mOsc = ofPtr<oscManager>(new oscManager());
    m_bankManager = ofPtr<bankManager>(new bankManager(mOsc));
    
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
    
    fakeCanvas->addSlider("F_POS_X", -5, 5, m_kinectManager.getFakePos().x);
    fakeCanvas->addSlider("F_POS_Y", -2, 2, m_kinectManager.getFakePos().y);
    fakeCanvas->addSlider("F_POS_Z", 0, 10, m_kinectManager.getFakePos().z);
    
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
    
    sc1Sliders[0] = settingsCanvases[1]->addSlider("KN_TILT", -30, 30, m_kinectManager.getKinectAngle(), slLength, slHeight);
    
    settingsCanvases[1]->addSpacer();
    
    settingsCanvases[1]->addButton("RECORD_BACKGROUND", false);
    
    settingsCanvases[1]->addSpacer();
    
    sc1Sliders[1] = settingsCanvases[1]->addSlider("NEAR_THRESH", 0, 2, m_kinectManager.getNearThresh(), slLength, slHeight);
    sc1Sliders[2] = settingsCanvases[1]->addSlider("FAR_THRESH", 5, 15, m_kinectManager.getFarThresh(), slLength, slHeight);
    sc1Sliders[3] =  settingsCanvases[1]->addSlider("SEG_THRESH", 0, 1, m_kinectManager.getSegThresh(), slLength, slHeight);
    sc1Sliders[4] = settingsCanvases[1]->addSlider("MIN_BLOB", 0.0, 0.1, m_kinectManager.getMinBlob(), slLength, slHeight);
    sc1Sliders[4]->setLabelPrecision(3);
    sc1Sliders[5] = settingsCanvases[1]->addSlider("MAX_BLOB", 0.25, 1, m_kinectManager.getMaxBlob(), slLength, slHeight);
    
    
    settingsCanvases[1]->addSpacer();
    sc1Sliders[6] = settingsCanvases[1]->addSlider("FLOOR_Y", -10, -0.5, m_kinectManager.getFloorY(), slLength, slHeight);
    sc1Sliders[7] = settingsCanvases[1]->addSlider("USER_HEIGHT", 1, 2, m_kinectManager.getDancerHeight(), slLength, slHeight);
    sc1Sliders[8] = settingsCanvases[1]->addSlider("MOV_THRESH", 0, 100, m_kinectManager.getMovThresh(), slLength, slHeight);
    sc1Sliders[9] = settingsCanvases[1]->addSlider("MOV_BUFF", 0, 60, m_kinectManager.getMovBuff(), slLength, slHeight);
    
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
            zoneCanvases[i] = new ofxUICanvas(ofGetWidth()/2 - 400, ofGetHeight() - 230, 600, 230);
        else
            zoneCanvases[i] = new ofxUICanvas(ofGetWidth()/2 + 210, ofGetHeight() - 230, 300, 230);
        
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
    
    occInvTog = new ofxUIToggle("OCC_INVERT", false, 20,20,0,0, OFX_UI_FONT_SMALL);
    movEnaTog = new ofxUIToggle("MOV_ENABLED", false, 20,20,0,0, OFX_UI_FONT_SMALL);
    movInvTog = new ofxUIToggle("MOV_INVERT", false, 20,20,0,0, OFX_UI_FONT_SMALL);
    
    zoneCanvases[0]->addWidgetRight(occInvTog);
    zoneCanvases[0]->addWidgetRight(movEnaTog);
    zoneCanvases[0]->addWidgetRight(movInvTog);
    
    zoneCanvases[0]->addSpacer();
    
    eblTog = new ofxUIToggle("ENABLED", true, 20, 20, 0, 0, OFX_UI_FONT_SMALL);
    eblTog->setColorFill(ofxUIColor(255,0,0));
    zoneCanvases[0]->addWidgetDown(eblTog);
    
    zoneCanvases[0]->addWidgetRight(new ofxUISpacer(1,20));
    
    zoneCanvases[0]->addWidgetRight(new ofxUILabel("SOUNDFILE", OFX_UI_FONT_SMALL));
    sc2TextInput[2] = new ofxUITextInput("SOUNDFILE", "none", 200);
    zoneCanvases[0]->addWidgetRight(sc2TextInput[2]);
    

    
    loopTog = new ofxUIToggle("LOOP", true, 20,20,0,0, OFX_UI_FONT_SMALL);
    playToEndTog = new ofxUIToggle("PLAY_TO_END", false, 20,20,0,0, OFX_UI_FONT_SMALL);

    zoneCanvases[0]->addWidgetRight(loopTog);
    zoneCanvases[0]->addWidgetRight(playToEndTog);

    repSlider = new ofxUISlider("MIN_REPLAY", 0, 5.0, 0.05, slw, 10);
        
    zoneCanvases[0]->addWidgetDown(repSlider);
    
    //---------------------------------------------------------
    zoneCanvases[0]->addWidgetRight(new ofxUISpacer(1,20));
    
    selTypeDisp = new ofxUILabel(0,0,200, "SELECTOR_TYPE: ", OFX_UI_FONT_SMALL);
    zoneCanvases[0]->addWidgetRight(selTypeDisp);
    
    ofxUILabelButton * sel_p = (ofxUILabelButton *)zoneCanvases[0]->addWidgetRight(new ofxUILabelButton("SEL_MINUS", true, 25));
    ofxUILabelButton * sel_m = (ofxUILabelButton *)zoneCanvases[0]->addWidgetRight(new ofxUILabelButton("SEL_PLUS", true, 25));
    
    sel_p->setLabelText("-");
    sel_m->setLabelText("+");
    
    //-------------------------------------------------------------------
    
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
        
        m_kinectManager.saveSettings(XML);
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
            
            m_kinectManager.loadSettings(XML);
            updateMainSettingsGui();
            
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

void testApp::updateMainSettingsGui(){

    sc1Sliders[0]->setValue(m_kinectManager.getKinectAngle());
    sc1Sliders[1]->setValue(m_kinectManager.getNearThresh());
    sc1Sliders[2]->setValue(m_kinectManager.getFarThresh());
    sc1Sliders[3]->setValue(m_kinectManager.getSegThresh());
    sc1Sliders[4]->setValue(m_kinectManager.getMinBlob());
    sc1Sliders[5]->setValue(m_kinectManager.getMaxBlob());
    sc1Sliders[6]->setValue(m_kinectManager.getFloorY());
    sc1Sliders[7]->setValue(m_kinectManager.getDancerHeight());
    sc1Sliders[8]->setValue(m_kinectManager.getMovThresh());
    sc1Sliders[9]->setValue(m_kinectManager.getMovBuff());
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    mOsc->update();
    m_kinectManager.update();
    
    if(m_kinectManager.getDancer()){
        m_bankManager->update(m_kinectManager.getDancer());
    }else{
        m_bankManager->unTriggerAll();
    }
    
    
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
    ofxUISlider *slider = (ofxUISlider *) e.widget;
    
    if(name == "KN_TILT")m_kinectManager.setKinectAngle(slider->getScaledValue());
    if(name == "SEG_THRESH")m_kinectManager.setSegThresh(slider->getScaledValue());
    if(name == "NEAR_THRESH")m_kinectManager.setNearThresh(slider->getScaledValue());
    if(name == "FAR_THRESH")m_kinectManager.setFarThresh(slider->getScaledValue());
    if(name == "MIN_BLOB")m_kinectManager.setMinBlob(slider->getScaledValue());
    if(name == "MAX_BLOB")m_kinectManager.setMaxBlob(slider->getScaledValue());
    if(name == "MAX_BLOB")m_kinectManager.setMaxBlob(slider->getScaledValue());
    if(name == "FLOOR_Y")m_kinectManager.setFloorY(slider->getScaledValue());
    if(name == "USER_HEIGHT")m_kinectManager.setDancerHeight(slider->getScaledValue());
    if(name == "MOV_THRESH")m_kinectManager.setMovThresh(slider->getScaledValue());
    if(name == "MOV_BUFF")m_kinectManager.setMovBuff(slider->getScaledValue());
    if(name == "RECORD_BACKGROUND")m_kinectManager.recordBg();
    
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
    

    if(isMouseDown){
        //preventing double trigger
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
        if(name == "SEL_MINUS"){m_bankManager->decCZoneSelectorType();}
        if(name == "SEL_PLUS"){m_bankManager->incCZoneSelectorType();}
    }
    
    if(name == "sphere")m_bankManager->setCZoneShape(0);
    if(name == "box")m_bankManager->setCZoneShape(1);
    if(name == "ENABLED")tog->setValue(m_bankManager->setCZoneEnabled(tog->getValue()));
    if(name == "LOOP")m_bankManager->setCZoneLoop(tog->getValue());
    if(name == "PLAY_TO_END")m_bankManager->setCZonePlayToEnd(tog->getValue());
    if(name == "OCC_INVERT")m_bankManager->setCZoneOccInvert(tog->getValue());
    if(name == "MOV_ENABLED")m_bankManager->setCZoneMovEnabled(tog->getValue());
    if(name == "MOV_INVERT")m_bankManager->setCZoneMovInvert(tog->getValue());
    if(name == "OCC_INVERT")m_bankManager->setCZoneOccInvert(tog->getValue());
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

    string t_str = name;
    t_str = t_str.substr(0,5);
    
    
    if(t_str == "T_POS"){
        t_str = name;
        t_str = t_str.substr(6);
        ofVec3f p = m_bankManager->getCurrentZone()->getPos();
        if(t_str == "X")p.x = slider->getScaledValue();
        if(t_str == "Y")p.y = slider->getScaledValue();
        if(t_str == "Z")p.z = slider->getScaledValue();
        m_bankManager->setCZonePosition(p);
    }
    

    t_str = name;
    t_str = t_str.substr(2,3);
    
    
    if(t_str == "DIM"){
        t_str = name;
        t_str = t_str.substr(0,1);
        ofVec3f bd = m_bankManager->getCurrentZone()->getBoxDims();
        if(t_str == "X")bd.x = slider->getScaledValue();
        if(t_str == "Y")bd.y = slider->getScaledValue();
        if(t_str == "Z")bd.z = slider->getScaledValue();
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
    
    ofVec3f p = m_kinectManager.getFakePos();
    
    if(name == "F_POS_X")p.x = slider->getScaledValue();
    if(name == "F_POS_Y")p.y = slider->getScaledValue();
    if(name == "F_POS_Z")p.z = slider->getScaledValue();
    
    m_kinectManager.setFakePos(p);
    
    
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
    occInvTog->setValue(zn->getIsOccInvert());
    
    xDimSlid->setValue(zn->getBoxDims().x);
    yDimSlid->setValue(zn->getBoxDims().y);
    zDimSlid->setValue(zn->getBoxDims().z);
    (zn->getShape() == 0) ? shapeRad->activateToggle("sphere") : shapeRad->activateToggle("box");
    
    //sensSlider->setValue(zn->getSensitivity());
    repSlider->setValue(zn->getMinReplaySecs());
    synthTypeDisp->setLabel("SYNTH_TYPE: " + synthDictionary::getSynthString(zn->getSynthType()));
    selTypeDisp->setLabel("SELECTOR_TYPE: " + synthDictionary::getSelectorString(zn->getSelectorType()));
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
        
        ofPushMatrix();
        ofScale(0.05, 0.05, 0.05);
        ofDrawGrid();
        ofPopMatrix();
        
        
        ofPushMatrix();
        ofTranslate(0, m_kinectManager.getFloorY(), 0);
        drawFloor();
        ofPopMatrix();
        
        
        ofNoFill();
        ofSetColor(255,255,0);
        
        ofPushMatrix();
        ofVec3f a = m_kinectManager.getQAxis();
        ofRotate(m_kinectManager.getQAngle(), a.x, a.y, a.z);
        ofScale(2,0.5,0.5);
        ofBox(0.15);
        ofPopMatrix();
        
        
        if(isViewSegPoints){
            if(m_kinectManager.getDancer())m_kinectManager.drawUserPointCloud();
        }else{
            m_kinectManager.drawScenePointCloud();
        }
        
        if(isViewCom){
            ofNoFill();
            ofSetColor(0, 255, 255);
            ofVec3f com = m_kinectManager.getDancer()->com;
            ofSphere(com.x, com.y, com.z, m_kinectManager.getDancerHeight()/2);
        }
        
        
        if(isViewCScene)m_bankManager->getCurrentScene()->draw(cm.getPosition());
        
        
        cm.end();
        
        glDisable(GL_DEPTH_TEST);
        
    }
    
    
    if(displayMode == DT_DM_2D){
        
        ofSetColor(255);
        
        ofPushMatrix();
        ofTranslate(ofGetWidth() - 750, 50);
        m_kinectManager.getLiveImg()->draw(0,0,320,240);
        ofTranslate(0, 260);
        ofDrawBitmapString("live depthMap", 0,0);
        ofTranslate(0, 40);
        m_kinectManager.getSegMask()->draw(0,0,320,240);
        ofTranslate(0, 260);
        ofDrawBitmapString("segmented depthMap", 0,0);
        ofTranslate(0, 40);
        
        
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(ofGetWidth() - 360, 50);
        ofFill();
        ofSetColor(50);
        ofRect(0,0,320,240);
        m_kinectManager.getCfFinder()->draw(0,0,320,240);
        ofTranslate(0, 260);
        ofSetColor(255);
        ofDrawBitmapString("contour analysis", 0,0);
        ofPopMatrix();
        
    }
    
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
            m_kinectManager.toggleFake();
            fakeCanvas->setVisible(m_kinectManager.getIsFake());
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
    
    m_kinectManager.exit();
    
    delete settingsTabBar;
    delete displayTabBar;
    
    for(int i = 0; i < NUM_SETTINGS_CANVASES; i ++)delete settingsCanvases[i];
    for(int i = 0; i < 2; i++)delete displayCanvases[i];
    for(int i = 0; i < 3; i++)delete zoneCanvases[i];
    
    if(synthCanvas != NULL)delete synthCanvas;
    
    mOsc->sendExit();
    
}

