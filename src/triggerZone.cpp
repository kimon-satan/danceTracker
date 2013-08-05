//
//  triggerZone.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "triggerZone.h"

int triggerZone::index = 0;

triggerZone::triggerZone(ofPtr<oscManager> o) : mOsc(o){
    
    shape = TZ_SPHERE;
    center.set(0,0,2);
    boxDims.set(0.5,0.25,0.5);
    
    radius = 0.25;
    
    isOccupied = false;
    isEnabled = false;
    
    mName = "defaultZone_" + ofToString(index, 0);
    mSoundFileName = "none";
    font.loadFont("NewMedia.ttf", 15);
    
    mIndex = index;
    index += 1;
    
    isSelected = false;
    
    occupyCount = 0;
    
    isLoop = true;
    isPlayToEnd = false;
    
    sensitivity = 1.0;
    
    //addZone to SC
    mOsc->addZone(mIndex, mName);
    
    updateAllAudio();
    
    
}

void triggerZone::updateAllAudio(){
    
    mOsc->updateZoneSettings(mIndex, "loop", isLoop);
    mOsc->updateZoneSettings(mIndex, "playToEnd", isPlayToEnd);

}

void triggerZone::draw(){
    
    
        ofPushMatrix();
        
            ofTranslate(center.x, center.y, center.z);
        
            ofPushMatrix();
            (isSelected) ? ofSetColor(255,0,0) : ofSetColor(255);
                ofRectangle r = font.getStringBoundingBox(mName, 0, 0);
    
                if(shape == TZ_SPHERE)
                    ofTranslate(- r.width/200, radius + r.height/100, 0);
                else
                    ofTranslate(- r.width/200, boxDims.y/2 + r.height/100, 0);
    
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
                  
                    ofPushMatrix();
                    ofScale(boxDims.x, boxDims.y, boxDims.z);
                    ofBox(1);
                    ofPopMatrix();
                    
                }else{
                    ofNoFill();
                    ofSphere(0,0,0, radius);
                }

    
        ofPopMatrix();
    
    /*if(isOccupied){
        ofSetColor(0, 255, 255);
        ofSphere(intersect, 0.05);
    }*/
    
}


void triggerZone::checkPoints(vector<ofVec3f> & pc){
    
    
    int targetAmt = max(1, (int)(pc.size() * (1-sensitivity)));
    int inTotal = 0;
    
    vector<ofVec3f>::iterator it;
    
    isOccupied = false;
    
    if(shape == TZ_SPHERE){
    
        for(it = pc.begin(); it != pc.end(); it++){
            
            if(it->distance(center) < radius){
                
                inTotal += 1;
                
                if(inTotal >= targetAmt){
                    intersect.set(*it);
                    isOccupied = true;
                    break;
                }
                
            }
        
        }
        
    }else if(shape == TZ_BOX){
    
        for(it = pc.begin(); it != pc.end(); it++){
            
            if(it->x <= center.x + boxDims.x/2 && it->x >= center.x - boxDims.x/2){
                
                if(it->y <= center.y + boxDims.y/2 && it->y >= center.y - boxDims.y/2){
                 
                    if(it->z <= center.z + boxDims.z/2 && it->z >= center.z - boxDims.z/2){
                        
                        inTotal += 1;
                        
                        if(inTotal >= targetAmt){
                            intersect.set(*it);
                            isOccupied = true;
                             break;
                        }
                       
                        
                    }
                    
                }
            
            
            }
            
        }
        
    
    }
    
    if(isOccupied){
        
        if(occupyCount == 0){
           // mSound.stop();
            mOsc->stopZone(mIndex);
            //mSound.play();
            mOsc->playZone(mIndex);
        }
        
        occupyCount +=1;
        
    }else{
        
        if(occupyCount > 0){
            mOsc->stopZone(mIndex);
            occupyCount = 0;
        }
        
    }
    
    
    
}

bool triggerZone::checkInRange(ofVec3f com, float userHeight){
    
    float d = com.distance(center);
    
    bool inRange = (d <= userHeight/2 + radius);
    
    if(!inRange && isOccupied){
        
        mOsc->stopZone(mIndex);
        isOccupied = false;
        occupyCount = 0;
        
    }
    
    return inRange;

}

void triggerZone::update(){


    
    
}



void triggerZone::reloadSound(){ //when loading settings from file

    //no longer used
    string s = "sound/" + mSoundFileName;
    mSound.stop();
    mSound.unloadSound();
    mSound.loadSound(s);
    mSound.setLoop(isLoop);
    
}


//getters and setters

void triggerZone::setSoundFile(string s){
    
    mSoundFileName = s;
    
    ofFile f("sound/" + s);
    
    mOsc->loadZoneSound(mIndex, f.getAbsolutePath());
    
    
    //reloadSound();
    
}

string triggerZone::getSoundFileName(){return mSoundFileName;}


void triggerZone::setIsEnabled(bool b){

    isEnabled = b;
    if(!isEnabled)mOsc->stopZone(mIndex);

}
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


void triggerZone::setIsLoop(bool b){

    mOsc->updateZoneSettings(mIndex, "loop", b);
    isLoop = b;


}
bool triggerZone::getIsLoop(){return isLoop;}

void triggerZone::setIsPlayToEnd(bool b){
    
    mOsc->updateZoneSettings(mIndex, "playToEnd", b);
    isPlayToEnd = b;

}

bool triggerZone::getIsPlayToEnd(){return isPlayToEnd;}

bool triggerZone::getIsAudioLoaded(){
   
    bool b =   ofFile::doesFileExist("sound/" + mSoundFileName);
    
    return b;

}

void triggerZone::deselect(){
    
    if(isOccupied)mOsc->stopZone(mIndex);
    isOccupied = false;
    occupyCount = 0;
    
}

void triggerZone::setSensitivity(float s){sensitivity = s;}
float triggerZone::getSensitivity(){return sensitivity;}

int triggerZone::getIndex(){ return mIndex; }
