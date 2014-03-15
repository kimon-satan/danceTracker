//
//  dt_utils.cpp
//  danceTracker
//
//  Created by Simon Katan on 15/03/2014.
//
//

#include "dt_utils.h"

string dt_utils::getRandom(int len){

    static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string s = "";
    
    for (int i = 0; i < len; ++i) {
        s = s + alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return s;
}