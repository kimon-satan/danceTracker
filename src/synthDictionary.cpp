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