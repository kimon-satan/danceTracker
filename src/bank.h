//
//  bank.h
//  danceTracker
//
//  Created by Simon Katan on 07/08/2013.
//
//

#ifndef danceTracker_bank_h
#define danceTracker_bank_h

#include "scene.h"

class bank{

    public:
    
    bank(){
        
        mIndex = index;
        name = "bank_" + ofToString(index,0);
        index += 1;
    
    };
    
    vector<ofPtr<scene> > scenes;
    string name;
    static int index;
    int mIndex;

};


#endif
