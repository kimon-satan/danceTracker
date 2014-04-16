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
    isIntersect= false;
    isEnabled = false;
    
   
    mSoundFileName = "none";
    
    isSelected = false;
    isSound = false;
    
    silentCount = 600;
    
    isLoop = true;
    isPlayToEnd = false;
    
    isOccInvert = false;
    isMovInvert = false;
    isMovEnabled = false;
    
    sensitivity = 1.0;
    minReplaySecs = 0.0;
    
    synth = ST_SIMPLE;
    selector = SELT_RANDOM;
    
    synthParams = synthDictionary::getSynthParams(synth);
    u_id = dt_utils::getRandom(10);
    
     mName = "zone_" + u_id;
    
    changeBuff = 0;
    changeCount = 0;
    
    
}

void triggerZone::updateAllAudio(){
    
    mOsc->updateZoneSettings(u_id, "loop", isLoop);
    mOsc->updateZoneSettings(u_id, "playToEnd", isPlayToEnd);
    mOsc->updateZoneSettings(u_id, "synthType", synthDictionary::getSynthString((int)synth));
    mOsc->updateZoneSettings(u_id, "selectorType", synthDictionary::getSelectorString((int)selector));
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
                if(isOccupied && isIntersect){
                    ofSetColor(255, 0, 0);
                }else if(isIntersect || isOccupied){
                    ofSetColor(205, 85, 85);
                }else{
                    ofSetColor(0, 180, 0);
                }
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
                    
                }else if(shape == TZ_SPHERE){
                    
                    ofNoFill();
                    ofSphere(0,0,0, radius);
                    
                }else if(shape == TZ_CYLINDER){
                    ofNoFill();
                    ofPushMatrix();
                   
                    ofTranslate(0, -boxDims.y/2, 0);
                    for(int i = 0; i < 20; i++){
                        ofTranslate(0, boxDims.y/20, 0);
                        ofPushMatrix();
                         ofRotate(90, 1, 0, 0);
                        ofCircle(0, 0, radius);
                        ofPopMatrix();
                    }
                    ofPopMatrix();
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
    
    isIntersect = false;
    
    iCom.set(0,0,0);
    
    if(shape == TZ_SPHERE){
    
        for(it = d->pixels.begin(); it != d->pixels.end(); it++){
            
            if(it->distance(center) < radius){
                
                inTotal += 1;
                
                iCom += (*it);
                
                if(inTotal >= targetAmt){
                    isIntersect = true;
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
                            isIntersect = true;
                            //break;
                        }
                       
                        
                    }
                    
                }
            
            
            }
            
        }
        
    
    }else if(shape == TZ_CYLINDER){
    
        for(it = d->pixels.begin(); it != d->pixels.end(); it++){
        
            if(it->y <= center.y + boxDims.y/2 && it->y >= center.y - boxDims.y/2){
            
                ofVec2f p(it->x, it->z);
                ofVec2f c(center.x, center.z);
                if(p.distance(c) <= radius){
                    
                    inTotal += 1;
                    
                    iCom += (*it);
                    
                    if(inTotal >= targetAmt){
                        isIntersect = true;
                        //break;
                    }
                    
                    
                }
                
            }
            
        }

    
    
    }
    
    if(isOccupied != isIntersect){
        changeCount += 1;
        if(changeCount >= changeBuff){
            isOccupied = isIntersect;
            changeCount = 0;
        }
    }else{
        changeCount = 0;
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
        dims += d->height;
        
        if(d->com.x <= center.x + dims.x/2 && d->com.x >= center.x - dims.x/2){
            
            if(d->com.y <= center.y + dims.y/2 && d->com.y >= center.y - dims.y/2){
                
                if(d->com.z <= center.z + dims.z/2 && d->com.z >= center.z - dims.z/2){
                    
                    inRange = true;
                    
                }
                
            }
            
            
        }

    }
    
    
    if(!inRange && isOccupied){
        isOccupied = false;
        changeCount = 0;
    }
        
    return inRange;

}

void triggerZone::updateSynthParams(bool isInit){

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
    
        
        if(synthParams[i].map == MT_INIT_RANDOM ){
            
            if(isInit){
                float val = ofMap(ofRandom(0, 1), 0,1,synthParams[i].min_val, synthParams[i].max_val);
                mOsc->updateZoneSettings(u_id, synthParams[i].name, val);
            }
            
        }else if(synthParams[i].map == MT_FIXED){
            
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
        updateSynthParams(true);
        mOsc->playZone(u_id);
        isSound = true;
        changeCount = 0;
        
    }else if(isSound && !b){
        
        //turn off the sound
        silentCount = 0;
        mOsc->stopZone(u_id);
        isSound = false;
        changeCount = 0;
        
    }
    
}


void triggerZone::reloadSound(){ //used for copying zones
    
    ofFile f("sound/" + mSoundFileName);
    
    mOsc->loadZoneSound(u_id, f.getAbsolutePath());
    
    updateAllAudio();
    
}


//getters and setters

void triggerZone::setSoundFile(string s){
    
    mSoundFileName = s;
    
    ofFile f("sound/" + s);
    
    mOsc->loadZoneSound(u_id, f.getAbsolutePath());
    
    
}

string triggerZone::getSoundFileName(){return mSoundFileName;}


void triggerZone::setIsEnabled(bool b, bool isLoading){

    isEnabled = b;
    if(!isLoading)evaluate();

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
string triggerZone::getShapeString(){
    switch(shape){
        case TZ_SPHERE:return "sphere";
        case TZ_BOX:return "box";
        case TZ_CYLINDER:return "cylinder";
    }
}

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
   
    bool b = false;
    
    string lc = mSoundFileName.substr(mSoundFileName.length() - 1);
    if(lc == "/"){
       b = ofDirectory::doesDirectoryExist("sound/" + mSoundFileName);
       if(b)b = !ofDirectory::isDirectoryEmpty("sound/" + mSoundFileName.substr(0, mSoundFileName.length() - 1));
        
    }else if(mSoundFileName.substr(mSoundFileName.length() - 4, 1) == "."){
       b = ofFile::doesFileExist("sound/" + mSoundFileName);
    }
    
   
    return b;

}

void triggerZone::forceStop(){
    
    if(isSound){
        
        //turn off the sound
        silentCount = 600;
        mOsc->stopZone(u_id);
        isSound = false;
        
    }
}

void triggerZone::deselect(){
    
    changeCount = 0;
    isIntersect = false;
    isOccupied = false;
    isMoving = false;
    forceStop();
    
}

void triggerZone::setSensitivity(float s){sensitivity = s;}
float triggerZone::getSensitivity(){return sensitivity;}

void triggerZone::setMinReplaySecs(float s){minReplaySecs = s;}
float triggerZone::getMinReplaySecs(){ return minReplaySecs;}

int triggerZone::getSynthType(){return (int)synth;}

void triggerZone::setSynthType(int i, bool isLoading){
    
    synth = synthType(i);
    synthParams.clear();
    synthParams = synthDictionary::getSynthParams(synth);
    if(!isLoading)updateAllAudio();

}

int triggerZone::getSelectorType(){
    return (int)selector;
}

void triggerZone::setSelectorType(int i){
    
    selector = selectorType(i);
    mOsc->updateZoneSettings(u_id, "selectorType", synthDictionary::getSelectorString((int)selector));
    mOsc->resetZone(u_id);
    
    
}

synthParam triggerZone::getSynthParam(int i){return synthParams[i];}
void triggerZone::setSynthParam(int i, synthParam p){synthParams[i] = p;}

bool triggerZone::getIsOccInvert(){return isOccInvert;}
void triggerZone::setIsOccInvert(bool b){
    
    isOccInvert = b;
    evaluate();
}



bool triggerZone::getIsMovInvert(){return isMovInvert;}
void triggerZone::setIsMovInvert(bool b){
    
    isMovInvert = b;
    evaluate();
}


bool triggerZone::getIsMovEnabled(){return isMovEnabled;}
void triggerZone::setIsMovEnabled(bool b){
    
    isMovEnabled = b;
    evaluate();
}



void triggerZone::newIndex(){
    
    u_id = dt_utils::getRandom(10);
    
}

string triggerZone::getUid(){
    
    return u_id;
}


void triggerZone::setChangeBuff(int i){changeBuff = i;}
int triggerZone::getChangeBuff(){ return changeBuff; }