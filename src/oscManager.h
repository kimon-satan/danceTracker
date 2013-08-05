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
    
    void addZone(int index, string name);
    void loadZoneSound(int index, string name);
    void removeZone(int index);
    void playZone(int index);
    void stopZone(int index);
    
    void updateZoneSettings(int index, string item, string value);
    void updateZoneSettings(int index, string item, float value);
    void updateZoneSettings(int index, string item, int value);
    
    void updateSynth(int index);
    
    void printMessage(ofxOscMessage & m);
    
    private:
    
    bool isGreeted;
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    
};




#endif /* defined(__danceTracker__oscManager__) */
