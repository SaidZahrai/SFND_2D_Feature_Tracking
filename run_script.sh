#! /usr/bin/bash

echo "Looping over all detectors, descriptor extractors, matchers and selectors"

for matcher in MAT_BF MAT_FLANN  
do
    for selector in SEL_NN SEL_KNN  
    do
        for extractor in BRIEF BRISK FREAK SIFT  
        do
            for detector in HARRIS SHITOMASI FAST ORB AKAZE SIFT  
            do
                ./2D_feature_tracking $detector $extractor $matcher $selector OFF
            done
        done
        for detector in HARRIS SHITOMASI FAST ORB AKAZE  
        do
            ./2D_feature_tracking $detector ORB $matcher $selector OFF
        done
        ./2D_feature_tracking AKAZE AKAZE $matcher $selector OFF
    done
done