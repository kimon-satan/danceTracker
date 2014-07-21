//
//  cHullFinder.h
//  danceTracker
//
//  Created by Simon Katan on 21/07/2014.
//
//

#ifndef __danceTracker__cHullFinder__
#define __danceTracker__cHullFinder__



#include "ofxCvContourFinder.h"

class cHullFinder : public ofxCvContourFinder{
    
public:

    int  findContours( ofxCvGrayscaleImage& input,
                              int minArea, int maxArea,
                              int nConsidered, bool bFindHoles,
                              bool bUseApproximation = true);

};

#endif /* defined(__danceTracker__cHullFinder__) */
