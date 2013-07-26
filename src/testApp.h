#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"


#define NUM_CANVASES 4

enum dtDisplayMode{

    DT_DM_POINTCLOUD,
    DT_DM_SEGMENTATION,
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
    
        void guiEvent(ofxUIEventArgs &e);
        void exit();
    
        void setupGui();
    
        void calcQ();
        void recordBg();
        void segment();
    
        void drawPointCloud();
        void drawFloor();

        ofxUITabBar *guiTabBar;
        ofxUICanvas * canvases[NUM_CANVASES];
    
        ofxKinect 			kinect;
        int					kinectAngle;
        dtDisplayMode       displayMode;
    
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
    
        ofxCvGrayscaleImage bgDm;
        ofxCvGrayscaleImage liveImg;
        ofxCvGrayscaleImage segImg;
        bool isGui;
    

    
    
    
		
};
