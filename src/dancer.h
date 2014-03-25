//
//  dancer.h
//  danceTracker
//
//  Created by Simon Katan on 25/03/2014.
//
//

#ifndef __danceTracker__dancer__
#define __danceTracker__dancer__

#include "ofMain.h"

class dancer {
    
public:
    
    dancer();
    
    ofVec3f com;
    bool isMoving;
    int movAmt;
    int stillCount, movCount;
    float height;
    vector<ofVec3f> pixels;
    
};


#endif /* defined(__danceTracker__dancer__) */
