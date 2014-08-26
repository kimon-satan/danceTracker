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
    
    bank();
    
    void newIndex();
    void firstScene();
    void nextScene();
    void prevScene();
    void insertScene(ofPtr<scene> sn);
    void removeScene();
    
    
    vector<ofPtr<scene> > scenes;
    ofPtr<scene> selScene;
    int selSceneIndex;
    string name;
    string u_id;

};


#endif
