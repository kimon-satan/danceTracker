#include "testApp.h"

#define OFX_KINECT_GRAVITY 9.80665


//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    ofEnableSmoothing();
    ofSetCircleResolution(60);

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
    
    bgDm.allocate(kinect.width, kinect.height);
    segImg.allocate(kinect.width, kinect.height);
    
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

    canvases[1]->addSlider("FLOOR_Y", -10, -1, floorY, length-xInit, dim);
    
    canvases[1]->addButton("RECORD_BACKGROUND", false);
  
    ofAddListener(canvases[1]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[1]);
    
    //------------------------
    

	canvases[2]->setName("Display Controls");
    
    
    canvases[2]->addWidgetDown(new ofxUILabel("Display", OFX_UI_FONT_LARGE));
    canvases[2]->addButton("POINT_CLOUD", false);
    canvases[2]->addButton("SEGMENTATION", false);

    canvases[2]->addSpacer();

    canvases[2]->addSlider("CAM_DISTANCE", 100, 1000, cDist, length-xInit,dim);


    
    ofAddListener(canvases[2]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[2]);
    
    //------------------------
    
	canvases[3]->setName("Triggers");

    
    ofAddListener(canvases[3]->newGUIEvent,this,&testApp::guiEvent);
    guiTabBar->addCanvas(canvases[3]);

    

}

//--------------------------------------------------------------
void testApp::update(){
    
    kinect.update();
    
    if(kinect.isFrameNew()){
        liveImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        segment();
        calcQ();
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
    
    bgDm.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

}

void testApp::segment(){

    int numPixels = kinect.width * kinect.height;
    
    unsigned char * bg_pix = bgDm.getPixels();
    unsigned char * l_pix = liveImg.getPixels();
    unsigned char * s_pix = segImg.getPixels();
    
    for(int i = 0; i < numPixels; i++) {
        
        if(l_pix[i] > bg_pix[i] + 10){
            s_pix[i] = 255;
        }else{
            s_pix[i] = 0;
        }
    }
    
    segImg.flagImageChanged();
    segImg.erode_3x3();
        
    
}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(0);
    
    if(displayMode == DT_DM_POINTCLOUD){
    
        cm.begin();
        
        ofScale(100,100,100);
        
        ofPushMatrix();
            ofScale(0.05, 0.05, 0.05);
            ofDrawGrid();
        ofPopMatrix();

        
        ofPushMatrix();
            ofTranslate(0, floorY, 0);
            drawFloor();
        ofPopMatrix();
        
        ofPushMatrix();
        
            //corrected coordinates area
            ofRotate(qangle, qaxis.x, qaxis.y, qaxis.z);
            
            ofNoFill();
            ofSetColor(255,255,0);
            
            ofPushMatrix();
                ofScale(2,0.5,0.5);
                ofBox(0.15);
            ofPopMatrix();
            
            drawPointCloud();
        
        ofPopMatrix();
                                        
        cm.end();                           
        
    }
    
    if(displayMode == DT_DM_SEGMENTATION){
    
        
        ofSetColor(255);
        
        ofPushMatrix();
            ofTranslate(50, 100);
            liveImg.draw(0,0,320,240);
            ofTranslate(0, 260);
            ofDrawBitmapString("live depthMap", 0,0);
            ofTranslate(0, 40);
            bgDm.draw(0,0,320,240);
            ofTranslate(0, 260);
            ofDrawBitmapString("background depthMap", 0,0);
            ofTranslate(0, 40);
            segImg.draw(0,0,320,240);
            ofTranslate(0, 260);
            ofDrawBitmapString("segmented depthMap", 0,0);
            ofTranslate(0, 40);
        
        ofPopMatrix();
        
    }
    


}

void testApp::drawPointCloud() {
    
	int w = 640;
	int h = 480;
    
	glBegin(GL_POINTS);
	
    int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofVec3f cur = kinect.getWorldCoordinateFor(x, y);
			ofColor color = kinect.getCalibratedColorAt(x,y);
			glColor3ub((unsigned char)color.r,(unsigned char)color.g,(unsigned char)color.b);
			glVertex3f(cur.x, -cur.y, cur.z);
		}
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
            if(displayMode == DT_DM_POINTCLOUD){
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
    
    if(name == "RECORD_BACKGROUND"){
    
        recordBg();
    }
    
    
    if(name == "POINT_CLOUD"){
        displayMode = DT_DM_POINTCLOUD;
    }
    
    
    if(name == "SEGMENTATION"){
        displayMode = DT_DM_SEGMENTATION;
    }
    

    
	
}

void testApp::exit()
{
	delete guiTabBar;
    
    for(int i = 0; i < NUM_CANVASES; i ++)delete canvases[i];

}

