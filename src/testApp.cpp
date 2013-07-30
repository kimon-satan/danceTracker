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
    
    cDist = 500;
    floorY = -2;
    
    cm.disableMouseInput();
    
    
    liveImg.allocate(kinect.width,kinect.height);
    segMask.allocate(kinect.width, kinect.height);
    
    isBgRecorded = false;
    isUser = false;
    segThresh = 0.2;
    segRes = 2;
    nearThresh = 0.5;
    farThresh = 8;
    
    minBlob = 0.02;
    maxBlob = 0.5;
    
    userHeight = 1.88;
    
    kinect.getCalibration().setClippingInCentimeters(50,1000);
    
    setupGui();

    
}

void testApp::setupGui(){

    int tabBarWidth = 80;
    isGui = true;
    // ---
    guiTabBar = new ofxUITabBar();
    guiTabBar->setWidth(tabBarWidth);
    guiTabBar->setColorFill(ofxUIColor(200));
    guiTabBar->setColorFillHighlight(ofxUIColor(255));
    guiTabBar->setColorBack(ofxUIColor(255, 20, 20, 150));
    
    float dim  = 24;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320 - xInit;
    
    for(int i = 0; i < NUM_CANVASES; i ++){
    
        canvases[i] = new ofxUICanvas(ofGetWidth() - (length + xInit), 0, length + xInit, ofGetHeight());
        canvases[i]->setColorFill(ofxUIColor(200));
        canvases[i]->setColorFillHighlight(ofxUIColor(255));
        canvases[i]->setColorBack(ofxUIColor(20, 20, 20, 150));
        
    }
    
   
    //--------------------------------------------
    
    canvases[0]->setName("Settings");
    
    canvases[0]->addLabel("Settings");
    canvases[0]->addSpacer();
    canvases[0]->addButton("SAVE", false);
    canvases[0]->addButton("LOAD", false);
    
    
    ofAddListener(canvases[0]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[0]);
    
    //---------------------------
    
	canvases[1]->setName("Kinect Controls");
    canvases[1]->addSlider("CAM_TILT", -30, 30, kinectAngle, length-xInit, dim);

    canvases[1]->addSlider("FLOOR_Y", -10, -0.5, floorY, length-xInit, dim);
    
    canvases[1]->addButton("RECORD_BACKGROUND", false);
    canvases[1]->addSlider("SEG_THRESH", 0, 1, segThresh, length-xInit, dim);
  
    ofAddListener(canvases[1]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[1]);
    
    //------------------------
    

	canvases[2]->setName("Display Controls");
    
    
    canvases[2]->addWidgetDown(new ofxUILabel("Display", OFX_UI_FONT_LARGE));
    canvases[2]->addButton("POINT_CLOUD", false);
    canvases[2]->addButton("SEGMENTATION", false);
    canvases[2]->addButton("SCENE", false);

    canvases[2]->addSpacer();

    canvases[2]->addSlider("CAM_DISTANCE", 100, 1000, cDist, length-xInit,dim);


    
    ofAddListener(canvases[2]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[2]);
    
    //------------------------
    
	canvases[3]->setName("Scene Setup");
    
    canvases[3]->addLabel("SELECT_ZONE");
    canvases[3]->addNumberDialer("C_ZONE", 0, 5, &cZone, 0);
    
    canvases[3]->addSpacer();
    
    eblTog = canvases[3]->addToggle("ENABLED", true);
    
    radSlid = canvases[3]->addSlider("RADIUS", 0.05, 0.5, 0.1);
    tPosX = canvases[3]->addSlider("T_POS_X", -5, 5, 0.0);
    tPosY = canvases[3]->addSlider("T_POS_Y", -2, 2, 0.0);
    tPosZ = canvases[3]->addSlider("T_POS_Z", 0, 10, 0.0);
    
    ofAddListener(canvases[3]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[3]);

    

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
    
     cm.setDistance(cDist);
   
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
    
    if(displayMode == DT_DM_POINTCLOUD || displayMode == DT_DM_SCENE){
    
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
        
        //drawScenePointCloud();
        
        if(isUser)drawUserPointCloud();
        
        if(displayMode == DT_DM_POINTCLOUD){
            
            if(isUser){
                ofNoFill();
                ofSetColor(0, 255, 255);
                ofSphere(com.x, -com.y, com.z, userHeight/2);
            }
            
        }else{
        
            currentScene.draw();
        
        }
        
        cm.end();
        
        glDisable(GL_DEPTH_TEST);
        
    }
    
    
    if(displayMode == DT_DM_SEGMENTATION){
    
        
        ofSetColor(255);
        
        ofPushMatrix();
            ofTranslate(50, 100);
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
            ofTranslate(400, 100);
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
            guiTabBar->toggleVisible();
            isGui = !isGui;
            if(displayMode == DT_DM_POINTCLOUD || displayMode == DT_DM_SCENE){
                if(!isGui)
                    cm.enableMouseInput();
                else
                    cm.disableMouseInput();
            }
        break;
    

            
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
    
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320 - xInit;
    
    for(int i = 0; i < NUM_CANVASES; i ++){

        canvases[i]->setPosition(ofGetWidth() - (length + xInit), 0);
        
    }

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.widget->getName();
    
    if(name == "CAM_DISTANCE"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        cDist = slider->getScaledValue();
    }
    
    if(name == "CAM_TILT"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        kinectAngle = slider->getScaledValue();
        kinect.setCameraTiltAngle(kinectAngle);
    }
    
    if(name == "FLOOR_Y"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        floorY = slider->getScaledValue();
        
    }
    
    if(name == "SEG_THRESH"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        segThresh = slider->getScaledValue();
        
    }

    
    if(name == "RECORD_BACKGROUND"){
    
        recordBg();
    }
    
    
    if(name == "POINT_CLOUD"){
        displayMode = DT_DM_POINTCLOUD;
    }
    
    
    if(name == "SEGMENTATION"){
        displayMode = DT_DM_SEGMENTATION;
    }
    
    
    if(name == "SCENE"){
        displayMode = DT_DM_SCENE;
    }
    
    if(name == "C_ZONE"){
    
        ofxUINumberDialer * dialler = (ofxUINumberDialer *) e.widget;
        selZone = (int)dialler->getValue();
        
        ofVec3f tp = currentScene.getTZPos(selZone);
        tPosX->setValue(tp.x);
        tPosY->setValue(tp.y);
        tPosZ->setValue(tp.z);
        
        radSlid->setValue(currentScene.getTZRadius(selZone));
        
        eblTog->setValue(currentScene.getTZEnabled(selZone));
        
        
        
    }
    
    if(name == "RADIUS"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.setTZRadius(selZone, slider->getScaledValue());
    
    }
    
    if(name == "T_POS_X"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.setTZPosX(selZone, slider->getScaledValue());
    }
    
    if(name == "T_POS_Y"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
       currentScene.setTZPosY(selZone, slider->getScaledValue());
    
    }
    
    if(name == "T_POS_Z"){
        
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        currentScene.setTZPosZ(selZone, slider->getScaledValue());
    
    }
    
    if(name == "ENABLED"){
    
        ofxUIToggle *tog = (ofxUIToggle *) e.widget;
        currentScene.setTZEnabled(selZone, tog->getValue());
        
    }


    

    
	
}

void testApp::exit()
{
	delete guiTabBar;
    
    for(int i = 0; i < NUM_CANVASES; i ++)delete canvases[i];

}

