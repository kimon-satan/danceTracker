//
//  synthDictionary.h
//  danceTracker
//
//  Created by Simon Katan on 05/08/2013.
//
//

#ifndef __danceTracker__synthDictionary__
#define __danceTracker__synthDictionary__

#include "ofMain.h"

enum synthType{
    
    ST_SIMPLE,
    ST_GRAN_1,
    ST_GRAN_2,
    ST_MULTISAMPLE,
    ST_COUNT
    
};

enum mapType{
    
    MT_FIXED,
    MT_GLOBAL_X,
    MT_GLOBAL_Y,
    MT_GLOBAL_Z,
    MT_GLOBAL_CENTER,
    MT_LOCAL_X,
    MT_LOCAL_Y,
    MT_LOCAL_Z,
    MT_LOCAL_CENTER,
    MT_NUM_POINTS,
    MT_USER_Y,
    MT_USER_RADIAL,
    MT_USER_CENTER,
    MT_COUNT
    
};


struct synthParam{
    
    synthParam(){
        name = "blank";
        min_val = 0;
        max_val = 1;
        abs_val = 0.5;
        map = MT_FIXED;
        
        sl_min = min_val;
        sl_max = max_val;
    
    };
    
    synthParam(string name, float min_val, float max_val, float abs_val, mapType map)
    : name(name), min_val(min_val), max_val(max_val), abs_val(abs_val), map(map){
    
        sl_min = min_val;
        sl_max = max_val;
        
    }
    
    float sl_min, sl_max;
    float min_val, max_val, abs_val;
    mapType map;
    string name;
    
};

class synthDictionary{

    public:

    static vector<synthParam> getSynthParams(int sType);
    static string getSynthString(int sType);
    static string getMapString(int mType);


};


#endif /* defined(__danceTracker__synthDictionary__) */
