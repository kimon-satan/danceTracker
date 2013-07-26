#pragma once

#include "ofMain.h"
#include "ofxKinect.h"

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
    
        void calcQ();
        void recordBg();
        void segment();
    
        void drawPointCloud();
        void drawFloor();

    
        ofxKinect 			kinect;
        int					angle;
    
        ofxBase3DVideo* 	kinectSource;
    
        int pointCloudRotation;
        float pitch, roll;
    
        ofVec3f qaxis;
        float qangle;
    
        vector<float> mPitches;
        vector<float> mRolls;
    
        float rRange;
        float pRange;
    
        ofEasyCam cm;
        float cDist;
    
        float floorY;
    

    
    
    
		
};
