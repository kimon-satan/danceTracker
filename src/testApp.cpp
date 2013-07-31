#include "testApp.h"

#define OFX_KINECT_GRAVITY 9.80665




//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    
    //kinect.init();
	kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
	kinect.setVerbose(true);
	kinect.open();
    
    // zero the tilt on startup
	kinectAngle = 0;
	kinect.setCameraTiltAngle(kinectAngle);
    
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
    
    kinect.getCalibration().setClippingInCentimeters(50,1500);
    
    isViewCom = false;
    isViewCScene = false;
    isViewSegPoints = false;
    
    cm.reset();
    cm.setPosition(0, 0, 1000);
    cm.setTarget(ofVec3f(0, 0, 500));

    
    isCamMouse = false, isCamKey = true;

    cm.disableMouseInput();
    
    setupGui();

    

}

void testApp::setupGui(){

    tabBarWidth = 320;
    tabBarHeight = 100;
    float dim  = 24;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = tabBarWidth - xInit;
    
    
    isSettingsGui = true;
    isDisplayGui = true;
    
    // ---
    settingsTabBar = new ofxUITabBar();
    
    settingsTabBar->setPosition(0, ofGetHeight() - tabBarHeight);
    settingsTabBar->setColorFill(ofxUIColor(200));
    settingsTabBar->setColorFillHighlight(ofxUIColor(255));
    settingsTabBar->setColorBack(ofxUIColor(255, 20, 20, 150));
    
   
    for(int i = 0; i < NUM_CANVASES; i ++){
    
        settingsCanvases[i] = new ofxUICanvas(0, 0, tabBarWidth, 500);
        settingsCanvases[i]->setColorFill(ofxUIColor(200));
        settingsCanvases[i]->setColorFillHighlight(ofxUIColor(255));
        settingsCanvases[i]->setColorBack(ofxUIColor(20, 20, 20, 150));
        
    }
    
   
    //--------------------------------------------
    
    settingsCanvases[0]->setName("Save/Load");
    
    settingsCanvases[0]->addLabel("Save/Load");
    settingsCanvases[0]->addSpacer();
    settingsCanvases[0]->addButton("SAVE", false);
    settingsCanvases[0]->addButton("LOAD", false);
    
    
    ofAddListener(settingsCanvases[0]->newGUIEvent,this,&testApp::s0Events);
    settingsTabBar->addCanvas(settingsCanvases[0]);
    
    //---------------------------
    
	settingsCanvases[1]->setName("Initial Setup");
    
    settingsCanvases[1]->addSlider("KN_TILT", -30, 30, kinectAngle, length-xInit, dim);
    
    settingsCanvases[1]->addSpacer();
    
    settingsCanvases[1]->addButton("RECORD_BACKGROUND", false);
    
    settingsCanvases[1]->addSpacer();
    
    settingsCanvases[1]->addSlider("NEAR_THRESH", 0, 2, nearThresh, length-xInit, dim);
    settingsCanvases[1]->addSlider("FAR_THRESH", 5, 15, farThresh, length-xInit, dim);
    settingsCanvases[1]->addSlider("SEG_THRESH", 0, 1, segThresh, length-xInit, dim);
    settingsCanvases[1]->addSlider("MIN_BLOB", 0, 0.1, minBlob, length-xInit, dim);
    settingsCanvases[1]->addSlider("MAX_BLOB", 0.25, 1, maxBlob, length-xInit, dim);
     
    
    settingsCanvases[1]->addSpacer();
    settingsCanvases[1]->addSlider("FLOOR_Y", -10, -0.5, floorY, length-xInit, dim);
    settingsCanvases[1]->addSlider("USER_HEIGHT", 1, 2, userHeight, length-xInit, dim);
   
  
    ofAddListener(settingsCanvases[1]->newGUIEvent,this,&testApp::s1Events);
    settingsTabBar->addCanvas(settingsCanvases[1]);
    
    //------------------------
    
    
	settingsCanvases[2]->setName("Scene Setup");
    
    settingsCanvases[2]->addLabel("SELECT_ZONE");
    settingsCanvases[2]->addNumberDialer("C_ZONE", 0, 5, &cZone, 0);
    
    settingsCanvases[2]->addSpacer();
    
    eblTog = settingsCanvases[2]->addToggle("ENABLED", true);
    
    radSlid = settingsCanvases[2]->addSlider("RADIUS", 0.05, 0.5, 0.1);
    tPosX = settingsCanvases[2]->addSlider("T_POS_X", -5, 5, 0.0);
    tPosY = settingsCanvases[2]->addSlider("T_POS_Y", -2, 2, 0.0);
    tPosZ = settingsCanvases[2]->addSlider("T_POS_Z", 0, 10, 0.0);
    
    ofAddListener(settingsCanvases[2]->newGUIEvent,this,&testApp::s2Events);
    settingsTabBar->addCanvas(settingsCanvases[2]);




    

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
    
    
    
    ofAddListener(displayCanvases[1]->newGUIEvent,this,&testApp::dispEvents);
    displayTabBar->addCanvas(displayCanvases[1]);
  
    
    

   
}

//--------------------------------------------------------------
void testApp::update(){
    
    kinect.update();
    
    if(kinect.isFrameNew()){
        
       
        liveImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        calcQ();
        //rotate the pixels
        
        curDepths.clear();
        
        for(int y = 0; y < kinect.height; y += segRes) {
            
            for(int x = 0; x < kinect.width; x += segRes) {
                
                ofVec3f cur = kinect.getWorldCoordinateFor(x, y);
                
                ofVec3f curR = cur.getRotated(-qangle, qaxis);
                
                curDepths.push_back(curR);
                
            }
            
        }
        
        
        if(isBgRecorded){
            
            segment();
            if(isUser){
                
                analyseUser();
                currentScene.update(com, userHeight, userPixels);
            }
        }
       
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

//--------------------------------------------------------------

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
                total += curR;
                userPixels.push_back(curR);
                
                
            }
            
        }
        
	}
    
    com = total/userPixels.size();
    
   
    
    vector<ofVec3f>::iterator it = remove_if(userPixels.begin(), userPixels.end(),findOutliers(com, userHeight));
    
    userPixels.erase(it, userPixels.end());


}



//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(0);
    
    ofDrawBitmapString("FPS: " +  ofToString(ofGetFrameRate(), 2), 200,20);
    
    if(displayMode == DT_DM_3D){
    
        cm.begin();
        
        glEnable(GL_DEPTH_TEST);
        
        ofScale(100,100,100);
        
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
            if(isUser)drawUserPointCloud();
        }else{
            drawScenePointCloud();
        }
        
        if(isViewCom){
            ofNoFill();
            ofSetColor(0, 255, 255);
            ofSphere(com.x, -com.y, com.z, userHeight/2);
        }
        
        
        if(isViewCScene){
            currentScene.draw();
        }
        
        cm.end();
        
        glDisable(GL_DEPTH_TEST);
        
    }
    
    
    if(displayMode == DT_DM_2D){
    
        ofSetColor(255);
        
        ofPushMatrix();
            ofTranslate(50, 50);
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
            ofTranslate(400, 50);
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
        
        glVertex3f(curDepths[i].x, -curDepths[i].y, curDepths[i].z);
 
    }
    
	glEnd();
}


void testApp::drawUserPointCloud() {
    
	glBegin(GL_POINTS);
    glColor3ub(0, 0, 0);
    
    
    for(int i = 0; i < userPixels.size(); i ++){
        
        glVertex3f(userPixels[i].x, -userPixels[i].y, userPixels[i].z);
        
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
    
    switch(key){
            
        case ' ':
            settingsTabBar->toggleVisible();
            isSettingsGui = !isSettingsGui;
        break;
            
        case OF_KEY_RETURN:
            displayTabBar->toggleVisible();
            isDisplayGui = !isDisplayGui;
            break;
        
                     
    }


    if(isCamKey){
        
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
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
    displayTabBar->setPosition(ofGetWidth() - 100 ,ofGetHeight() - tabBarHeight);
    settingsTabBar->setPosition(0,ofGetHeight() - tabBarHeight);
    
    for(int i = 0; i < 2; i ++){

        displayCanvases[i]->setPosition(ofGetWidth() - tabBarWidth, 0);
        
    }

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

void testApp::dispEvents(ofxUIEventArgs &e){
    
    string name = e.widget->getName();
    
    if(name == "2D"){
        
        displayMode = DT_DM_2D;
        displayCanvases[0]->toggleVisible();
        cm.disableMouseInput();
        isCamMouse = false;
        isCamMouse = false;
    }
    if(name == "3D"){
        
        displayMode = DT_DM_3D;
        mouseTog->setValue(false);
        isCamKey = true;
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
    
    if(name == "C_ZONE"){
        
        ofxUINumberDialer * dialler = (ofxUINumberDialer *) e.widget;
        selZone = (int)dialler->getValue();
        
        ofPtr<triggerZone> tz = currentScene.getTriggerZone(selZone);
        ofVec3f tp = tz->getPos();
        tPosX->setValue(tp.x);
        tPosY->setValue(tp.y);
        tPosZ->setValue(tp.z);
        
        radSlid->setValue(tz->getRadius());
        eblTog->setValue(tz->getIsEnabled());
        
        
    }
    
    if(name == "RADIUS"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.getTriggerZone(selZone)->setRadius(slider->getScaledValue());
    }
    
    if(name == "T_POS_X"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.getTriggerZone(selZone)->setPosX(slider->getScaledValue());
    }
    
    if(name == "T_POS_Y"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.getTriggerZone(selZone)->setPosY(slider->getScaledValue());
    }
    
    if(name == "T_POS_Z"){
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.getTriggerZone(selZone)->setPosZ(slider->getScaledValue());
    }
    
    if(name == "ENABLED"){
        ofxUIToggle *tog = (ofxUIToggle *) e.widget;
        currentScene.getTriggerZone(selZone)->setIsEnabled(tog->getValue());
    }


}

void testApp::exit()
{
	delete settingsTabBar;
    delete displayTabBar;
    
    for(int i = 0; i < NUM_CANVASES; i ++)delete settingsCanvases[i];
    for(int i = 0; i < 2; i ++)delete displayCanvases[i];

}

