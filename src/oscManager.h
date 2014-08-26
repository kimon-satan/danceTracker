//
//  oscManager.h
//  danceTracker
//
//  Created by Simon Katan on 05/08/2013.
//
//

#ifndef __danceTracker__oscManager__
#define __danceTracker__oscManager__

#include "ofMain.h"
#include "ofxOsc.h"


#define SCPORT 57120
#define RCPORT 71717


class oscManager{

    public:
    
    oscManager();
    
    void update();
    
    void addZone(string index, string name);
    void loadZoneSound(string index, string name);
    void removeZone(string index);
    void playZone(string index);
    void stopZone(string index);
    
    void updateZoneSettings(string index, string item, string value);
    void updateZoneSettings(string index, string item, float value);
    void updateZoneSettings(string index, string item, int value);
    
    void resetZone(string index);
    
    void updateSynth(string index);
    void newScene(float fIn, float fOut);
    
    
    void printMessage(ofxOscMessage & m);
    
    
    
    void sendExit();
    
    private:
    
    bool isGreeted;
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    
};




#endif /* defined(__danceTracker__oscManager__) */
