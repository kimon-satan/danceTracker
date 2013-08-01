//
//  triggerZone.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "triggerZone.h"

int triggerZone::index = 0;

triggerZone::triggerZone(){

    shape = TZ_SPHERE;
    center.set(0,0,2);
    boxDims.set(1,0.5,1);
    
    radius = 0.25;
    
    isOccupied = false;
    isEnabled = false;
    
    mName = "defaultZone_" + ofToString(index, 0);
    mSoundFileName = "none";
    font.loadFont("NewMedia.ttf", 15);
    
    index += 1;
    
    isSelected = false;
    
    
}

void triggerZone::draw(){
    
    
        ofPushMatrix();
        
            ofTranslate(center.x, center.y, center.z);
        
        ofPushMatrix();
        (isSelected) ? ofSetColor(255,0,0) : ofSetColor(255);
            ofRectangle r = font.getStringBoundingBox(mName, 0, 0);
            
            ofTranslate(- r.width/200, radius + r.height/100, 0);
            ofPushMatrix();
                ofScale(0.01,-0.01,0.01);
                font.drawString(mName, 0, 0);
            ofPopMatrix();
        ofPopMatrix();
        
        if(isEnabled){
            (isOccupied) ? ofSetColor(255, 0, 0): ofSetColor(255, 255, 0);
        }else{
            ofSetColor(60);
        }
    
            if(shape == TZ_BOX){
                ofFill();
                ofPushMatrix();
                ofScale(boxDims.x, boxDims.y, boxDims.z);
                ofBox(1);
                ofPopMatrix();
                
            }else{
                ofNoFill();
                ofSphere(0,0,0, radius);
            }
    
        ofPopMatrix();
    
}


void triggerZone::checkPoints(vector<ofVec3f> & pc){
    
    
    vector<ofVec3f>::iterator it;
    
    isOccupied = false;
    
    if(shape == TZ_SPHERE){
    
        for(it = pc.begin(); it != pc.end(); it++){
            
            if(it->distance(center) < radius){
                
                isOccupied = true;
                break;
                
            }
        
        }
        
    }else if(shape == TZ_BOX){
    
        for(it = pc.begin(); it != pc.end(); it++){
            
            if(it->x <= center.x + boxDims.x/2 && it->x >= center.x - boxDims.x/2){
                
                if(it->y <= center.y + boxDims.y/2 && it->y >= center.y - boxDims.y/2){
                 
                    if(it->z <= center.z + boxDims.z/2 && it->z >= center.z - boxDims.z/2){
                        
                        isOccupied = true;
                        break;
                        
                    }
                    
                }
            
            
            }
            
        }
        
    
    }
    
}

bool triggerZone::checkInRange(ofVec3f com, float userHeight){
    
    float d = com.distance(center);
    
    bool inRange = (d <= userHeight/2 + radius );
    
    return inRange;

}

void triggerZone::update(){

    if(isOccupied){
    
        if(!mSound.getIsPlaying())mSound.play();
        
    }else{
    
        if(mSound.getIsPlaying())mSound.stop();
    }
}



void triggerZone::reloadSound(){ //when loading settings from file

    string s = "sound/" + mSoundFileName;
    mSound.stop();
    mSound.unloadSound();
    mSound.loadSound(s);
}


//getters and setters

void triggerZone::setSoundFile(string s){
    
    mSoundFileName = s;
    
    s = "sound/" + s;
    
    mSound.stop();
    mSound.unloadSound();
    mSound.loadSound(s);
    
}

string triggerZone::getSoundFileName(){return mSoundFileName;}


void triggerZone::setIsEnabled(bool b){isEnabled = b;}
bool triggerZone::getIsEnabled(){return isEnabled;}

ofVec3f triggerZone::getPos(){return center;}
ofVec3f triggerZone::getBoxDims(){return boxDims;}
float triggerZone::getRadius(){return radius;}

void triggerZone::setRadius(float r){radius = r;}

void triggerZone::setPosX(float x){center.x = x;}
void triggerZone::setPosY(float y){center.y = y;}
void triggerZone::setPosZ(float z){center.z = z;}

void triggerZone::setShape(int t){shape = tzShape(t);}
int triggerZone::getShape(){return (int)shape;}


void triggerZone::setName(string s){
    
    mName = s;
}

string triggerZone::getName(){
    
    return mName;
}


void triggerZone::toggleSelected(){isSelected = !isSelected;}
void triggerZone::setIsSelected(bool b){isSelected = b;}

void triggerZone::setBoxDimsX(float x){boxDims.x = x; }
void triggerZone::setBoxDimsY(float y){boxDims.y = y; }
void triggerZone::setBoxDimsZ(float z){boxDims.z = z; }

