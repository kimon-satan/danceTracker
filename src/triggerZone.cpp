//
//  triggerZone.cpp
//  danceTracker
//
//  Created by Simon Katan on 30/07/2013.
//
//

#include "triggerZone.h"

triggerZone::triggerZone(ofPtr<oscManager> o) : mOsc(o){
    
    shape = TZ_SPHERE;
    center.set(0,0,2);
    boxDims.set(0.5,0.25,0.5);
    
    radius = 0.25;
    
    isOccupied = false;
    isEnabled = false;
    
   
    mSoundFileName = "none";
    
    isSelected = false;
    isSound = false;
    
    playCount = 0;
    silentCount = 0;
    
    isLoop = true;
    isPlayToEnd = false;
    
    isOccInvert = false;
    isMovInvert = false;
    isMovEnabled = false;
    
    sensitivity = 1.0;
    minReplaySecs = 0.0;
    
    synth = ST_SIMPLE;
    
    synthParams = synthDictionary::getSynthParams(synth);
    u_id = dt_utils::getRandom(10);
    
     mName = "zone_" + u_id;
    
    
}

void triggerZone::updateAllAudio(){
    
    mOsc->updateZoneSettings(u_id, "loop", isLoop);
    mOsc->updateZoneSettings(u_id, "playToEnd", isPlayToEnd);
    mOsc->updateZoneSettings(u_id, "synthType", (int)synth);
    updateSynthParams();

}

void triggerZone::draw(ofVec3f camPos){
    
    
        ofPushMatrix();
        
            ofTranslate(center.x, center.y, center.z);
        
            ofPushMatrix();
            (isSelected) ? ofSetColor(0,255,255) : ofSetColor(170);
            //ofRectangle r = font.getStringBoundingBox(mName, 0, 0);
    
            if(shape == TZ_SPHERE)
                ofTranslate(0, radius * 1.5, 0);
            else
                ofTranslate(0, boxDims.y/2 * 1.5, 0);
    
                ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
                ofDrawBitmapString(mName, 0,0,0);

            ofPopMatrix();
            
            if(isEnabled){
                (isOccupied) ? ofSetColor(255, 0, 0): ofSetColor(0, 150, 0);
            }else{
                ofSetColor(60);
            }
        
                if(shape == TZ_BOX){
                  
                    ofPushStyle();
                    ofSetLineWidth(3);
                    ofPushMatrix();
                    ofScale(boxDims.x, boxDims.y, boxDims.z);
                    ofBox(1);
                    ofPopMatrix();
                    ofPopStyle();
                    
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


void triggerZone::checkPoints(ofPtr<dancer> d){
    
    
    numUp = d->pixels.size();
    int targetAmt = max(1, (int)(d->pixels.size() * (1-sensitivity)));
    inTotal = 0;
    
    vector<ofVec3f>::iterator it;
    
    isOccupied = false;
    
    iCom.set(0,0,0);
    
    if(shape == TZ_SPHERE){
    
        for(it = d->pixels.begin(); it != d->pixels.end(); it++){
            
            if(it->distance(center) < radius){
                
                inTotal += 1;
                
                iCom += (*it);
                
                if(inTotal >= targetAmt){
                    isOccupied = true;
                    break;
                }
                
            }
        
        }
        
    }else if(shape == TZ_BOX){
    
        for(it = d->pixels.begin(); it != d->pixels.end(); it++){
            
            if(it->x <= center.x + boxDims.x/2 && it->x >= center.x - boxDims.x/2){
                
                if(it->y <= center.y + boxDims.y/2 && it->y >= center.y - boxDims.y/2){
                 
                    if(it->z <= center.z + boxDims.z/2 && it->z >= center.z - boxDims.z/2){
                        
                        inTotal += 1;
                        
                        iCom += (*it);
                        
                        if(inTotal >= targetAmt){
                            isOccupied = true;
                            break;
                        }
                       
                        
                    }
                    
                }
            
            
            }
            
        }
        
    
    }
    
   
    if(isOccupied)iCom /= inTotal;
    
    
}

bool triggerZone::checkInRange(ofPtr<dancer> d){
   
    
    bool inRange;

    mCom.set(d->com);
    
    if(shape == TZ_SPHERE){
        
        float dist = d->com.distance(center);
        inRange = (dist <= d->height/2 + radius);
        
    }else{
    
        ofVec3f dims(boxDims);
        dims += d->height/2;
        
        if(d->com.x <= center.x + dims.x/2 && d->com.x >= center.x - dims.x/2){
            
            if(d->com.y <= center.y + dims.y/2 && d->com.y >= center.y - dims.y/2){
                
                if(d->com.z <= center.z + dims.z/2 && d->com.z >= center.z - dims.z/2){
                    
                    inRange = true;
                    
                }
                
            }
            
            
        }

    }
    
    
    if(!inRange && isOccupied)isOccupied = false;
        
    return inRange;

}

void triggerZone::updateSynthParams(){

    ofVec3f iLocal(iCom - center);
    
    if(shape == TZ_SPHERE){
        
        iLocal /= radius; // - 1 to + 1
        
    }else{
        
        iLocal.x /= (boxDims.x/2); //- 1 to + 1
        iLocal.y /= (boxDims.y/2);
        iLocal.z /= (boxDims.z/2);
    }
    
    
    for(int i = 0; i < synthParams.size(); i ++){
        
       
        
        float mul = 0.0;
        
        switch(synthParams[i].map){
                
            case MT_FIXED:
                mul = 1;
                break;
                
            case MT_GLOBAL_CENTER:
                mul = center.distance(ofVec3f(0,0,5))/8.8317;
                mul = 1 - mul;
                break;
            case MT_GLOBAL_X:
                mul =  (center.x + 3.0)/6.0;
                break;
                
            case MT_GLOBAL_Y:
                mul = (center.y + 1.0)/2.0;
                break;
                
            case MT_GLOBAL_Z:
                mul = center.z/9.0;
                break;
                
            case MT_LOCAL_X:
                mul = (iLocal.x + 1)/2;
                break;
                
            case MT_LOCAL_Y:
                mul = (iLocal.y + 1)/2;
                break;
                
            case MT_LOCAL_Z:
                mul = (iLocal.z + 1)/2;
                break;
                
            case MT_LOCAL_CENTER:
                mul = iLocal.distance(ofVec3f(0,0,0));
                if(shape == TZ_BOX)mul /= 1.78;
                mul = 1 - mul;
                break;
                
            case MT_NUM_POINTS:
                mul = (float)inTotal/(float)numUp;
                break;
                

            case MT_USER_Y:
                mul =  ofMap(mCom.y ,-0.5,0.5, 0,1);
                break;
                
            case MT_USER_RADIAL:
            {
                ofVec2f p1(center.z, center.x);
                ofVec2f p2(mCom.z, mCom.x);
                mul = abs((p2-p1).angle(ofVec2f(0,1)))/180;
                
            }
                break;
                
            case MT_USER_CENTER:
                mul = mCom.distance(center)/4.5;
                mul = 1 - mul;
                break;
                
                
        }
        
        mul = max((float)0.0, min((float)1.0, mul));
    
        
        if(synthParams[i].map == MT_FIXED){
            
            mOsc->updateZoneSettings(u_id, synthParams[i].name, synthParams[i].abs_val);
            
        }else{
  
            float val = ofMap(mul, 0, 1, synthParams[i].min_val, synthParams[i].max_val);
            mOsc->updateZoneSettings(u_id, synthParams[i].name, val);
            
        }
        
    }

    
    
}

void triggerZone::update(ofPtr<dancer> d){

    isMoving = d->isMoving;
    
    evaluate();
    
    if(isSound){
        updateSynthParams();
        mOsc->updateSynth(u_id);
    }
    
    
}


void triggerZone::evaluate(){

    bool b = isEnabled;
    b *= (isOccInvert)? !isOccupied : isOccupied;
    if(isMovEnabled)b *= (isMovInvert)? !isMoving : isMoving;
    
    //minReplay check for replay
    if(!isSound){
        silentCount += 1;
        b *= ((float)silentCount/ofGetFrameRate() >= minReplaySecs);
    }
    
    if(!isSound && b){
        
        //turn on the sound
        mOsc->playZone(u_id);
        isSound = true;

        
    }else if(isSound && !b){
        
        //turn off the sound
        silentCount = 0;
        mOsc->stopZone(u_id);
        isSound = false;
        
    }
    
}


void triggerZone::reloadSound(){ //when loading settings from file

    //no longer used
    /*string s = "sound/" + mSoundFileName;
    mSound.stop();
    mSound.unloadSound();
    mSound.loadSound(s);
    mSound.setLoop(isLoop);*/
    
    ofFile f("sound/" + mSoundFileName);
    
    if(ofFile::doesFileExist("sound/" + mSoundFileName)){
        mOsc->loadZoneSound(u_id, f.getAbsolutePath());
    }
    
    updateAllAudio();
    
}


//getters and setters

void triggerZone::setSoundFile(string s){
    
    mSoundFileName = s;
    
    ofFile f("sound/" + s);
    
    mOsc->loadZoneSound(u_id, f.getAbsolutePath());
    
    
    //reloadSound();
    
}

string triggerZone::getSoundFileName(){return mSoundFileName;}


void triggerZone::setIsEnabled(bool b){

    isEnabled = b;
    evaluate();

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

    mOsc->updateZoneSettings(u_id, "loop", b);
    isLoop = b;


}
bool triggerZone::getIsLoop(){return isLoop;}

void triggerZone::setIsPlayToEnd(bool b){
    
    mOsc->updateZoneSettings(u_id, "playToEnd", b);
    isPlayToEnd = b;

}

bool triggerZone::getIsPlayToEnd(){return isPlayToEnd;}

bool triggerZone::getIsAudioLoaded(){
   
    bool b = ofFile::doesFileExist("sound/" + mSoundFileName);
    return b;

}

void triggerZone::deselect(){
    
    isOccupied = false;
    isMoving = false;
    silentCount = 0;
    evaluate();
    
}

void triggerZone::setSensitivity(float s){sensitivity = s;}
float triggerZone::getSensitivity(){return sensitivity;}

void triggerZone::setMinReplaySecs(float s){minReplaySecs = s;}
float triggerZone::getMinReplaySecs(){ return minReplaySecs;}

int triggerZone::getSynthType(){return (int)synth;}
void triggerZone::setSynthType(int i){
    
    synth = synthType(i);
    synthParams.clear();
    synthParams = synthDictionary::getSynthParams(synth);
    updateAllAudio();

}

synthParam triggerZone::getSynthParam(int i){return synthParams[i];}
void triggerZone::setSynthParam(int i, synthParam p){synthParams[i] = p;}

bool triggerZone::getIsInverted(){return isOccInvert;}
void triggerZone::setIsInverted(bool b){
    
    isOccInvert = b;
    playCount = 0;
    silentCount = 0;
    
    evaluate();
}



void triggerZone::newIndex(){
    
    u_id = dt_utils::getRandom(10);
    
}

string triggerZone::getUid(){
    
    return u_id;
}
