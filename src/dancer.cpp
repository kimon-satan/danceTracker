//
//  dancer.cpp
//  danceTracker
//
//  Created by Simon Katan on 25/03/2014.
//
//

#include "dancer.h"


dancer::dancer(){
    
    isMoving = false;
    movAmt = 0;
    stillCount = 0;
    movCount = 0;
    com.set(0,0,0);
    height = 1.8;
    
};
