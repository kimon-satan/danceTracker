//
//  kinectManager.cpp
//  danceTracker
//
//  Created by Simon Katan on 23/03/2014.
//
//

#include "kinectManager.h"

kinectManager::kinectManager(){

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
    
    mDancer = ofPtr<dancer>(new dancer());
    dancerHeight = 1.80;
    mDancer->com.set(0, 0, 0);

    isFakeUser = false;
    numFakePoints = 2000;
    fakePos.set(0,0,0);
    fakeRadius = 0.5;

    
}

void kinectManager::saveSettings(ofxXmlSettings & XML){
    
    XML.addTag("MAIN_SETTINGS");
    
    if(XML.pushTag("MAIN_SETTINGS")){
        
        XML.addValue("KN_TILT", kinect.getTargetCameraTiltAngle());
        XML.addValue("FLOOR_Y", floorY);
        XML.addValue("SEG_FRESH", segThresh);
        XML.addValue("USER_HEIGHT", dancerHeight);
        XML.addValue("NEAR_THRESH", nearThresh);
        XML.addValue("FAR_THRESH", farThresh);
        XML.addValue("MIN_BLOB", minBlob);
        XML.addValue("MAX_BLOB", maxBlob);
        
        XML.popTag();
    }
    
    
}

void kinectManager::loadSettings(ofxXmlSettings & XML){
    
    if(XML.pushTag("MAIN_SETTINGS")){
        
        float kt = XML.getValue("KN_TILT", 0.0);
        if(kinect.isConnected())kinect.setCameraTiltAngle(kt);
        nearThresh = XML.getValue("NEAR_THRESH", 0.5);
        farThresh = XML.getValue("FAR_THRESH", 10.0);
        segThresh = XML.getValue("SEG_FRESH", 0.1);
        minBlob = XML.getValue("MIN_BLOB", 0.005);
        maxBlob = XML.getValue("MAX_BLOB", 10.0);
        floorY = XML.getValue("FLOOR_Y", 1.0);
        dancerHeight = XML.getValue("USER_HEIGHT", 1.8);

        
        XML.popTag();
    }

}

void kinectManager::update(){
    
    if(USE_KINECT){
        
        kinect.update();
        numBlankFrames += 1;
        
    }
    
    if((kinect.isFrameNew() || numBlankFrames == 10) && kinect.isConnected()){
        
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
            if(isUser)analyseUser();
           
        }
        
    }else if(isFakeUser){
        
        mDancer->pixels.clear();
        
        for(int i = 0; i < numFakePoints; i ++){
            ofVec3f p(0,ofRandom(0, fakeRadius),0);
            p.rotate(ofRandom(0,360), ofRandom(0,360), ofRandom(0,360));
            p += fakePos;
            mDancer->pixels.push_back(p);
        }
        
    }
    
    
}


void kinectManager::drawScenePointCloud() {
    
    glBegin(GL_POINTS);
    glColor3ub(0, 0, 0);
    
    for(int i = 0; i < curDepths.size(); i ++){
        
        glVertex3f(curDepths[i].x, curDepths[i].y, curDepths[i].z);
        
    }
    
    glEnd();
}

void kinectManager::drawUserPointCloud() {
    
    glBegin(GL_POINTS);
    glColor3ub(0, 0, 0);
    
    
    for(int i = 0; i < mDancer->pixels.size(); i ++){
        
        glVertex3f(mDancer->pixels[i].x, mDancer->pixels[i].y, mDancer->pixels[i].z);
        
    }
    
    glEnd();
    
    
    
}


void kinectManager::calcQ(){
    
    
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

void kinectManager::recordBg(){
    
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

void kinectManager::segment(){
    
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

void kinectManager::analyseUser(){
    
    mDancer->pixels.clear();
    
    //create a dense array of rotated mDancer->pixels
    
    unsigned char * s_pix = segMask.getPixels();
    
    ofVec3f total(0,0,0);
    
    for(int y = 0; y < kinect.height; y ++){
        
        for(int x = 0; x < kinect.width; x ++){
            
            if(s_pix[y * kinect.width + x] > 0){
                ofVec3f cur = kinect.getWorldCoordinateAt(x, y);
                ofVec3f curR = cur.getRotated(-qangle, qaxis);
                curR *= ofVec3f(0.001,-0.001,0.001);
                total += curR;
                mDancer->pixels.push_back(curR);
                
                
            }
            
        }
        
	}
    
    com = total/mDancer->pixels.size();
    
    vector<ofVec3f>::iterator it = remove_if(mDancer->pixels.begin(), mDancer->pixels.end(),findOutliers(com, dancerHeight));
    
    mDancer->pixels.erase(it, mDancer->pixels.end());
    
    
}

void kinectManager::exit(){
    kinect.close();
}

//getters and setters

void kinectManager::toggleFake(){
    isFakeUser = !isFakeUser;
}

bool kinectManager::getIsFake(){return isFakeUser;}

const ofPtr<dancer> kinectManager::getDancer(){return mDancer;}

ofVec3f kinectManager::getFakePos(){return fakePos;}
void kinectManager::setFakePos(ofVec3f p){fakePos.set(p);}

int kinectManager::getKinectAngle(){ 
    kinectAngle = kinect.getTargetCameraTiltAngle();
    return kinectAngle;
}
void kinectManager::setKinectAngle(int f){
    
    kinectAngle = f;
    kinect.setCameraTiltAngle(kinectAngle);
    
}

float kinectManager::getSegThresh(){return segThresh;}
void kinectManager::setSegThresh(float f){segThresh = f;}

float kinectManager::getNearThresh(){return nearThresh;}
void kinectManager::setNearThresh(float f){nearThresh = f;}

float kinectManager::getFarThresh(){return farThresh;}
void kinectManager::setFarThresh(float f){farThresh = f;}

float kinectManager::getMinBlob(){return minBlob;}
void kinectManager::setMinBlob(float f){minBlob = f;}

float kinectManager::getMaxBlob(){return maxBlob;}
void kinectManager::setMaxBlob(float f){maxBlob = f;}

float kinectManager::getFloorY(){return floorY;}
void kinectManager::setFloorY(float f){floorY = f;}

float kinectManager::getDancerHeight(){ return dancerHeight;}
void kinectManager::setDancerHeight(float f){dancerHeight = f;}

float kinectManager::getQAngle(){return qangle;}
ofVec3f kinectManager::getQAxis(){return qaxis;}

ofxCvGrayscaleImage * kinectManager::getLiveImg(){return &liveImg;}
ofxCvGrayscaleImage * kinectManager::getSegMask(){return &segMask;}
ofxCvContourFinder * kinectManager::getCfFinder(){return &cfFinder;}




