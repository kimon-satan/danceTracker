//
//  oscManager.cpp
//  danceTracker
//
//  Created by Simon Katan on 05/08/2013.
//
//

#include "oscManager.h"


oscManager::oscManager(){

    sender.setup("localhost", SCPORT);
    isGreeted = false;
    
    ofxOscMessage m;
    m.setAddress("/init");
    sender.sendMessage(m);
    
    receiver.setup(RCPORT);

}

void oscManager::update(){

    
    if(ofGetFrameNum() == 300 && !isGreeted){
        
        
        //should be a screen dialogue;
        cout << "handshake with Supercollider failed. Reboot and try again \n";
        
    }
    
    while(receiver.hasWaitingMessages()){
        
        
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        printMessage(m);
        
        if(m.getAddress() == "/sc_hello"){
            
            cout << "hello received ... sending handshake back \n";
            ofxOscMessage n;
            n.setAddress("/of_hello");
            isGreeted = true;
            sender.sendMessage(n);
        }
            
    }
    

}

void oscManager::printMessage(ofxOscMessage & m){

    for(int i = 0; i < m.getNumArgs(); i++){
        
        int t = m.getArgType(i);
        cout << m.getAddress() << ",";
        switch(t){
                
            case OFXOSC_TYPE_FLOAT:
                cout << m.getArgAsFloat(i)  << ",";
                break;
                
            case OFXOSC_TYPE_INT32:
                cout << m.getArgAsInt32(i)  << ",";
                break;
                
            case OFXOSC_TYPE_STRING:
                cout << m.getArgAsString(i)  << ",";
                break;
                
        }
        
        
    }
    
    cout << endl;

}

void oscManager::loadZoneSound(int index, string name){

    ofxOscMessage m;
    m.setAddress("/loadZoneSound");
    m.addIntArg(index);
    m.addStringArg(name);
    
    sender.sendMessage(m);
    
}

void oscManager::addZone(int index, string name){

    ofxOscMessage m;
    m.setAddress("/addZone");
    m.addIntArg(index);
    m.addStringArg(name);
    
    sender.sendMessage(m);
}


void oscManager::removeZone(int index){
    
    ofxOscMessage m;
    m.setAddress("/removeZone");
    m.addIntArg(index);
    
    sender.sendMessage(m);

}


void oscManager::playZone(int index){
    
    ofxOscMessage m;
    m.setAddress("/playZone");
    m.addIntArg(index);
    
    sender.sendMessage(m);

}


void oscManager::stopZone(int index){

    ofxOscMessage m;
    m.setAddress("/stopZone");
    m.addIntArg(index);
    
    sender.sendMessage(m);

}

void oscManager::updateZoneSettings(int index, string item, string value){

    ofxOscMessage m;
    m.setAddress("/updateZoneSettings");
    m.addIntArg(index);
    m.addStringArg(item);
    m.addStringArg(value);
    
    sender.sendMessage(m);

}

void oscManager::updateZoneSettings(int index, string item, int value){
    
    ofxOscMessage m;
    m.setAddress("/updateZoneSettings");
    m.addIntArg(index);
    m.addStringArg(item);
    m.addIntArg(value);
    
    sender.sendMessage(m);
    
}

void oscManager::updateZoneSettings(int index, string item, float value){
    
    ofxOscMessage m;
    m.setAddress("/updateZoneSettings");
    m.addIntArg(index);
    m.addStringArg(item);
    m.addFloatArg(value);
    
    sender.sendMessage(m);
    
}

void oscManager::updateSynth(int index){

    ofxOscMessage m;
    m.setAddress("/updateSynth");
    m.addIntArg(index);
  
    sender.sendMessage(m);
    
}

void oscManager::newScene(float fIn, float fOut){
    
    ofxOscMessage m;
    m.setAddress("/newScene");
    m.addFloatArg(fIn);
    m.addFloatArg(fOut);
    sender.sendMessage(m);
    
}

void oscManager::sendExit(){
    
    ofxOscMessage m;
    m.setAddress("/exit");
    
    sender.sendMessage(m);
    
}
