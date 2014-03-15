//
//  synthDictionary.cpp
//  danceTracker
//
//  Created by Simon Katan on 05/08/2013.
//
//

#include "synthDictionary.h"



vector<synthParam>synthDictionary::getSynthParams(int sType){

    
    vector<synthParam> paramList;

    if(sType == ST_SIMPLE){
    
        paramList.push_back(synthParam("pan", -1, 1, 0, MT_GLOBAL_Z));
        paramList.push_back(synthParam("amp", 0,1, 0.5, MT_FIXED));
        paramList.push_back(synthParam("rate", -2,2,1, MT_FIXED));
        paramList.push_back(synthParam("fadeIn", 0.01,2.0, 0.01,MT_FIXED));
        paramList.push_back(synthParam("fadeOut", 0.01,2.0, 0.01,MT_FIXED));
        
    }
    
    if(sType == ST_GRAN_1){
        
        paramList.push_back(synthParam("pan", -1, 1, 0, MT_GLOBAL_Z));
        paramList.push_back(synthParam("amp", 0,1, 0.5, MT_FIXED));
        paramList.push_back(synthParam("trate", 0.1,20,10, MT_FIXED));
        paramList.push_back(synthParam("rate", -2.0,2.0,1, MT_LOCAL_X));
        paramList.push_back(synthParam("dur", 0.01,1,0.1, MT_FIXED));
        paramList.push_back(synthParam("pos", 0.01,1,0.5, MT_LOCAL_Z));
        paramList.push_back(synthParam("fadeIn", 0.01,2.0, 0.01,MT_FIXED));
        paramList.push_back(synthParam("fadeOut", 0.01,2.0, 0.01,MT_FIXED));
        
    }
    
    if(sType == ST_GRAN_2){
        
        paramList.push_back(synthParam("amp", 0,1, 0.5, MT_FIXED));
        paramList.push_back(synthParam("trate", 0.1,20,10, MT_FIXED));
        paramList.push_back(synthParam("rate", -2.0,2.0,1, MT_LOCAL_X));
        paramList.push_back(synthParam("prange", 0.1,1.0,1, MT_LOCAL_X));
        paramList.push_back(synthParam("dur", 0.01,1,0.1, MT_FIXED));
        paramList.push_back(synthParam("width", 0.1, 1.0, 0.75, MT_GLOBAL_Z));
        paramList.push_back(synthParam("fadeIn", 0.01,2.0, 0.01,MT_FIXED));
        paramList.push_back(synthParam("fadeOut", 0.01,2.0, 0.01,MT_FIXED));
        
    }
    
    return paramList;

}

string synthDictionary::getSynthString(int sType){

    switch(sType){
    
        case ST_SIMPLE:return "simple";
        case ST_GRAN_1:return "gran1";
        case ST_GRAN_2:return "gran2";
        default: return "";
    }

}


string synthDictionary::getMapString(int mType){
    
    switch(mType){
            
            case MT_FIXED: return "fixed";
            case MT_GLOBAL_X: return "global_x";
            case MT_GLOBAL_Y: return "global_y";
            case MT_GLOBAL_Z: return "global_z";
            case MT_GLOBAL_CENTER: return "global_center";
            case MT_LOCAL_X: return "local_x";
            case MT_LOCAL_Y: return "local_y";
            case MT_LOCAL_Z: return "local_z";
            case MT_LOCAL_CENTER: return "local_center";
            case MT_NUM_POINTS: return "num_points";
            case MT_USER_Y: return "user_y";
            case MT_USER_RADIAL: return "user_radial";
            case MT_USER_CENTER: return "user_center";
            default: return "";
    }
    
}