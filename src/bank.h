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
        
        name = "default";
    
    };
    
    vector<ofPtr<scene> > scenes;
    string name;

};


#endif
