#include "testApp.h"

#define OFX_KINECT_GRAVITY 9.80665
#define KINECT_SCR_WIDTH 640
#define KINECT_SCR_HEIGHT 480


//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    kinect.init();
	//kinect.init(true);  // shows infrared instead of RGB video image
	//kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
	kinect.setVerbose(true);
	kinect.open();
    
    // zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    pointCloudRotation = 0;
    
    calcQ();
    
    cDist = 500;
    floorY = -2;

}

//--------------------------------------------------------------
void testApp::update(){
    
    kinect.update();
    
    if(kinect.isFrameNew()){
                
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
    
    int w = 640;
	int h = 480;

}

void testApp::segment(){

    int step = 2;
    int w = 640;
	int h = 480;
    
    for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofVec3f cur = kinect.getWorldCoordinateFor(x, y);
            
        }
        
    }

}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(0);
    
    
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
            
        case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
            
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
            
        case 'w':
            cDist += 10;
            break;
            
        case 's':
            cDist -= 10;
            break;
            
        case 'y':
            floorY += 0.1;
            break;
            
        case 'h':
            floorY -= 0.1;
            break;
            
        case ' ':
            calcQ();
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}