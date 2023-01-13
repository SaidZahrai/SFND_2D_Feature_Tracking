# Udacity Sensor Fusion Nanodegree - Camera Mid-term project: 2D Feature Tracking

<img src="images/keypoints.png" width="820" height="248" />

This repository is a fork from [Udacity's repository for the project](https://github.com/udacity/SFND_2D_Feature_Tracking). 

## Summary and conclusion
The purpose of this project is to implement different keypoint detectors, descriptors, matcher and selectors to be used for tracking of the preceeding car in a driving situation. Based on the results, the most efficient three detector/descriptor combinations should be proposed.

The results obtained here suggest that in terms of efficiency, the best detectors are
1. FAST
2. ShiTomassi
3. ORB

and the best three descriptors are
1. BRIEF
2. BRISK
3. ORB

For the specific use in this project, FAST will be the most suitable choice for detector, as it is much faster than the other choices. Consequently, the three best combinations will be

| | Detector | Selector |
| :----: | :----: | :----: |
| 1 | FAST | BRIEF |
| 2 | FAST | BRISK |
| 3 | FAST | ORB |

## Setting up the system
The implementation can be completed on the workspace provided by Udacity, or locally on your machine. The code was developed on Windows Subsystem for Linux on Windows 11, with Ubuntu 20.04, but also tested on the project workspace.
### Dependencies and required build tools on Windows Subsystem for Linux with Ubuntu 20.04 

#### [cmake](https://cmake.org/install/) 3.16.3

#### [build-essential](https://packages.ubuntu.com/focal/build-essential)
  * make 4.2.1 (Installed by default and upgraded.)
  * gcc/g++ 9.4.0 (Installed by default and upgraded.)

#### [OpenCV](https://github.com/opencv/opencv) 4.1.1 or (4.7.0) and [Contrib Modules](https://github.com/opencv/opencv_contrib)
I decided to build OpenCV, as the final project will need it. The requirement for the project is version 4.1.1 or higher, however, you need an extra fix, if you have a version more recent than 4.4. In earlier versions, SIFT is a nonfree module, while in later versions it is included in the core. As will be shown later, this has been taken care of by a conditional macro.

Locally, I used the latest version of OpenCV together with Contrib modules, which at the time of writing, January 12th, 2023, was 4.7.0.

To build the desired version, say 4.1.1, execute the following commands:

```sh
git clone https://github.com/opencv/opencv
git clone https://github.com/opencv/opencv_contrib
cd opencv_contrib
git checkout 4.1.1
cd opencv
git checkout 4.1.1
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DOPENCV_ENABLE_NONFREE=ON -DENABLE_PRECOMPILED_HEADERS=OFF -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..
make -j`nproc`
sudo make install
```

### This repository
```sh
git clone git@github.com:SaidZahrai/SFND_2D_Feature_Tracking.git
cd SFND_2D_Feature_Tracking
mkdir build && cd build
cmake ..
make
./2D_feature_tracking
```
This will start the program with a set of predefined selections, together with a help text:
```bash
$ ./2D_feature_tracking
You have started the program without any arguments. This is similar to:
        2D_feature_tracking FAST BRISK MAT_BF SEL_NN ON
meaning you intend to run 2D_feature_tracking with
        Detector: FAST [HARRIS/SHITOMASI/FAST/ORB/AKAZE/SIFT]
        Descriptor: BRISK [BRIEF/BRISK/FREAK/ORB/AKAZE/SIFT]
        Matcher: [MAT_BF/MAT_FLANN]
        Selector: [SEL_NN/SEL_KNN]
        Visualization: ON [ON/OFF]
*** Start of execution of the command  ./2D_feature_tracking  with OpenCV 4.7.0 
```
followed by result text. As given in the above help text, you can choose any combination, but there is no test whether the combination is a valid one or not. Invalid selection leads to exceptions. To test all possible combinations, you can use the script `../run_script.sh`, which goes through all combinations.
```bash
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
```
Note that you must make sure that the path to `bash` is sorrect on the first line of the script, i.e.
```bash
#! /usr/bin/bash
```
You can save the output and then use another script, `make_tables.sh` to generate tables in Markdown format.
```bash
../run_script.sh > ../results/results.470.txt
../make_tables.sh ../results/results.470.txt > ../results/tables.md
```
`tables.md` is added to this `README.md` file, below. 

## Performed tasks and results
The tasks to be carried out, in addition to this write-up, are described in the project rubric and are divided into 9 topics. Below, the topics are presented and discussed.
### MP.1 Data Buffer Optimization
In the original code, the size of the buffer was growing every time a new image was read. This has been modified so that the size stays constant and two, by removing the head of the vecctor before adding a new image.

```C++
        //// TASK MP.1 -> Keep the buffer size constant equal to dataBufferSize
        if (dataBuffer.size() == dataBufferSize)
            dataBuffer.erase(dataBuffer.begin());

        // push image into data frame buffer
        DataFrame frame;
        frame.cameraImg = imgGray;
        dataBuffer.push_back(frame);
```

### MP.2 Keypoint Detection
The final code should include 7 different detectors, i.e. HARRIS, FAST, BRISK, ORB, AKAZE, and SIFT, in addition to Shi-Thomasi, that was in the initial code. Below, a short description of each is given and the implementation API as given in the links. 

* [Harris](https://docs.opencv.org/3.4/d8/dd8/tutorial_good_features_to_track.html)
uses the Hessian matrix together with a scoring formula to exclude edges and to detect corner region. 
* [Shi-Thomasi](https://docs.opencv.org/3.4/d8/dd8/tutorial_good_features_to_track.html)
has a similar basis as Harris, but the scoring is different.
* [FAST(Features from Accelerated Segment Test)](https://docs.opencv.org/3.4/df/d74/classcv_1_1FastFeatureDetector.html)
uses a circle of 16 pixels to classify whether a candidate point is actually a corner. 
If a set of N contiguous pixels in the circle are all brighter than the intensity of the candidate pixel plus a threshold t or are all darker than the intensity of the candidate pixel p minus the threshold t, p is classified as a vertex. Machine learning is used to accelerate the comparisonn and selection.

* [BRISK (Binary Robust Invariant Scalable Keypoints)](https://docs.opencv.org/3.4/de/dbf/classcv_1_1BRISK.html)
uses scale-space pyramid layers consisting of typically 4 octaves and 4 intra-octaves. The octaves are formed by progressively half-sampling the original image. The first intra-octave is obtained by downsampling the original image by a factor of 1.5, while the rest of the intra-octave layers are derived by successive halfsasmpling. The keypoint is detected and localized by processing the FAST 9-16 detector scoring on each octave and intra-octave separately. The characteristic direction of each keypoint is user to allow for orientation-normalized descriptors and achieving rotation invariance.

* [ORB (Oriented Fast and Rotated Brief)](https://docs.opencv.org/3.4/d1/d89/tutorial_py_orb.html)
detector is a modified verion of FAST, which incorporates scale invariance by constructing a scale pyramid.
* [A-KAZE](https://docs.opencv.org/3.4/db/d70/tutorial_akaze_matching.html)
uses a scale dimension by applying a nonlinear diffusion filter. It starts at finest level and goes to courser levels by subsampling. The features are detected by calculating the Hessian matrix at each scale level and a subpixel position for the feature is estimated by fitting of a quadratic function to the determinant. 
* [SIFT (Scale Invariant Feature Transform)](https://docs.opencv.org/3.4/d7/d60/classcv_1_1SIFT.html)
uses the local extreme points of differences of images convoluted with Gaussian filters at different scales as indicator for keypoint candidates. Keypoints are then taken as maxima/minima of the Difference of Gaussians (DoG) that occur at multiple scales. in this process, edge responses are eliminated using the Hessian matrix.

Two points need extra comments:
1. Harris can be implemented following the course material, but also using OpenCV's `goodFeaturesToTrack`:

```C++
    bool useHarris = true, useShiTomasi = false;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, useHarris, k);
```
The implementation from the course can be found in
```C++
// Detect keypoints in image using the traditional Harris detector
void detKeypointsHarris_course(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
```
but it is not used.

2. As mentioned above SIFT is included in the core in the later versions of OpenCV. This has been taken care of byt the following conditional inclusion:
```C++
#if (CV_MINOR_VERSION > 4)
        detector = cv::SiftFeatureDetector::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);
#else
        detector = cv::xfeatures2d::SiftFeatureDetector::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);
#endif
```

#### MP.3 Keypoint Removal 
We remove all keypoints outside of a pre-defined rectangle and only use the keypoints within the rectangle. This can be done simply by checking if the rectangle contains the points by:
```C++
        //// TASK MP.3 -> only keep keypoints on the preceeding vehicle
        bool bFocusOnVehicle = true;
        cv::Rect vehicleRect(535, 180, 180, 150);
        if (bFocusOnVehicle)
        {
            vector<cv::KeyPoint> focusedKeypoints;
            for (auto itr = keypoints.begin(); itr != keypoints.end(); itr++)
            {
                if (vehicleRect.contains(itr->pt))
                    focusedKeypoints.push_back(*itr);
            }
            keypoints = focusedKeypoints;
        }
```

### MP.4 Keypoint Descriptors
BRISK was implemented in the original code and BRIEF, ORB, FREAK, AKAZE and SIFT are added. The descriptors can be divided into two groups, binary ones that require Hamming norm and histogram based that need L2-norm, as described below with the implementation according to the link OpenCV API. 

#### Binary descriptors requiring Hamming-norm
* [BRIEF (Binary Robust Independent Elementary Features)](https://docs.opencv.org/3.4/d1/d93/classcv_1_1xfeatures2d_1_1BriefDescriptorExtractor.html)
creates a binary descriptor by assembling results of comparisons between densities in smoothed version of specifically defined pairs of patches.
* [BRISK (Binary Robust Invariant Scalable Keypoints)](https://docs.opencv.org/3.4/de/dbf/classcv_1_1BRISK.html)
descriptor has a pattern for sampling the neighborhood of each keypoint and achieves the formation of the rotation and scale-normalized descriptor by applying rotated sampling pattern. The bit-vector descriptor is assembled by performing all the shortdistance intensity comparisons of point pairs.
* [ORB (Oriented Fast and Rotated Brief)](https://docs.opencv.org/3.4/d1/d89/tutorial_py_orb.html)
uses BRIEF together with an orientation compensation mechanism, making it rotation invariant. The rotation compensation mechanism is based on the comparison between the orientation of the corner point, which has been placed in the centroid of the patch.

* [FREAK (Fast Retina Keypoint)](https://docs.opencv.org/3.4/df/db4/classcv_1_1xfeatures2d_1_1FREAK.html)
uses a circular pattern where the density of points drops when moving away from the center. To provide rotation invariance property, an orientation for the selected patch is computed by summing the local gradients over
chosen symmetrically located pairs with center as base. Also, in descriptor creation stage, a similar
approach that was used in ORB is performed, simply the less
correlated pattern is selected. Generally, the 512 binary tests are
used in order to obtain maximum performance.

* [A-KAZE](https://docs.opencv.org/3.4/db/d70/tutorial_akaze_matching.html)
is a binary descriptor which uses a Modified-Local Difference Binary that exploits gradient and intensity information from the nonlinear scale space as defined in the detection step. Therefore, the descriptor can only work with the A-KAZE detector.

#### Descriptors requiring L2-norm
* [SIFT (Scale Invariant Feature Transform)](https://docs.opencv.org/3.4/d7/d60/classcv_1_1SIFT.html)
uses histograms to assemble the descriptor vector based on 16 histograms with 8 bins each, resulting in 128 elements.
The histograms are computed from magnitude and orientation values of samples in a 16×16 region around the keypoint such that each histogram contains samples from a 4×4 subregion of the original neighborhood region. 

Note that SIFT should be called differently for older version of OpenCV than 4.4
```C++
#if (CV_MINOR_VERSION > 4)
        extractor = cv::SiftDescriptorExtractor::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);
#else
        extractor = cv::xfeatures2d::SiftDescriptorExtractor::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);
#endif
```

### MP.5 Descriptor Matching
Implement FLANN matching as well as k-nearest neighbor selection. Both methods must be selectable using the respective strings in the main function.

* [Brute Force](https://docs.opencv.org/3.4/d3/da1/classcv_1_1BFMatcher.html)
* [FLANN (Fast Library for Approximate Nearest Neighbors)](https://docs.opencv.org/3.4/dc/de2/classcv_1_1FlannBasedMatcher.html)


### MP.6 Descriptor Distance Ratio
Use the K-Nearest-Neighbor matching to implement the descriptor distance ratio test, which looks at the ratio of best vs. second-best match to decide whether to keep an associated pair of keypoints.

* Best match simply returns the best match between the two sets of key points.
* k nearest neighbors with (k=2) first returns the two closest points and then selects the keypoint, where the distance ratio is 0.8 or less, i.e. at least 20% closer match than the second candidate.


### MP.7 Performance Evaluation 1 - Number of keypoints on the preceding vehicle for all 10 images and take note of the distribution of their neighborhood size for all the detectors you have implemented

#### MP.7.a - Number of keypoints on the preceding vehicle for all 10 images
 | Method | #0001 | #0002 | #0003 | #0004 | #0005 | #0006 | #0007 | #0008 | #0009 | #0000 | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 132 | 124 | 137 | 134 | 140 | 137 | 148 | 159 | 137 | 138 | 
 | AKAZE | 157 | 161 | 155 | 163 | 164 | 173 | 175 | 177 | 179 | 166 | 
 | FAST | 152 | 150 | 155 | 149 | 149 | 156 | 150 | 138 | 143 | 149 | 
 | ORB | 102 | 106 | 113 | 109 | 125 | 130 | 129 | 127 | 128 | 92 | 
 | HARRIS | 54 | 53 | 55 | 56 | 58 | 57 | 61 | 59 | 57 | 50 | 
 | SHITOMASI | 118 | 123 | 120 | 120 | 113 | 114 | 123 | 111 | 112 | 125 | 


#### MP.7.b - Averaged neighborhood size for the keypoints on the preceding vehicle
 | Method | #0001 | #0002 | #0003 | #0004 | #0005 | #0006 | #0007 | #0008 | #0009 | #0000 | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 5.09 | 4.94 | 4.73 | 4.72 | 4.68 | 5.41 | 4.62 | 5.52 | 5.63 | 4.98 | 
 | AKAZE | 7.49 | 7.45 | 7.58 | 7.73 | 7.69 | 7.74 | 7.83 | 7.82 | 7.89 | 7.73 | 
 | FAST | 7.00 | 7.00 | 7.00 | 7.00 | 7.00 | 7.00 | 7.00 | 7.00 | 7.00 | 7.00 | 
 | ORB | 57.23 | 56.49 | 55.14 | 56.74 | 56.64 | 56.77 | 55.43 | 54.67 | 54.39 | 57.07 | 
 | HARRIS | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 
 | SHITOMASI | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 4.00 | 


### MP.8 Performance Evaluation 2 - Number of matched keypoints for all 10 images using all possible combinations of detectors and descriptors. 

Total number of matches for MAT_FLANN  and  SEL_NN 

 | Detect \ Describe | BRISK | AKAZE | SIFT | FREAK | BRIEF | ORB | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 1248 | N/A | 1249 | 1239 | 1249 | N/A | 
 | AKAZE | 1491 | 1491 | 1491 | 1491 | 1491 | 1491 | 
 | FAST | 1348 | N/A | 1348 | 1348 | 1348 | 1348 | 
 | ORB | 950 | N/A | 1033 | 549 | 1033 | 1033 | 
 | HARRIS | 503 | N/A | 503 | 503 | 503 | 503 | 
 | SHITOMASI | 1067 | N/A | 1067 | 1067 | 1067 | 1067 | 

Total number of matches for MAT_FLANN  and  SEL_KNN 

 | Detect \ Describe | BRISK | AKAZE | SIFT | FREAK | BRIEF | ORB | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 539 | N/A | 803 | 507 | 607 | N/A | 
 | AKAZE | 1115 | 1169 | 1274 | 971 | 1096 | 930 | 
 | FAST | 782 | N/A | 1047 | 670 | 888 | 865 | 
 | ORB | 653 | N/A | 765 | 350 | 453 | 512 | 
 | HARRIS | 356 | N/A | 459 | 303 | 404 | 392 | 
 | SHITOMASI | 694 | N/A | 926 | 578 | 814 | 765 | 

Total number of matches for MAT_BF  and  SEL_NN 

 | Detect \ Describe | BRISK | AKAZE | SIFT | FREAK | BRIEF | ORB | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 1248 | N/A | 1249 | 1239 | 1249 | N/A | 
 | AKAZE | 1491 | 1491 | 1491 | 1491 | 1491 | 1491 | 
 | FAST | 1348 | N/A | 1348 | 1348 | 1348 | 1348 | 
 | ORB | 950 | N/A | 1033 | 549 | 1033 | 1033 | 
 | HARRIS | 503 | N/A | 503 | 503 | 503 | 503 | 
 | SHITOMASI | 1067 | N/A | 1067 | 1067 | 1067 | 1067 | 

Total number of matches for MAT_BF  and  SEL_KNN 

 | Detect \ Describe | BRISK | AKAZE | SIFT | FREAK | BRIEF | ORB | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 592 | N/A | 800 | 596 | 702 | N/A | 
 | AKAZE | 1215 | 1259 | 1270 | 1188 | 1266 | 1186 | 
 | FAST | 899 | N/A | 1046 | 881 | 1099 | 1081 | 
 | ORB | 751 | N/A | 763 | 421 | 545 | 761 | 
 | HARRIS | 393 | N/A | 459 | 403 | 460 | 449 | 
 | SHITOMASI | 767 | N/A | 927 | 766 | 944 | 907 | 


### MP.9 Performance Evaluation 3 - time it takes for keypoint detection and descriptor extraction.

#### MP.9.1 - Detector perfromance 
Number of points
 | Detect \ Image | #0001 | #0002 | #0003 | #0004 | #0005 | #0006 | #0007 | #0008 | #0009 | #0000 | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 1371 | 1380 | 1335 | 1305 | 1369 | 1396 | 1382 | 1463 | 1422 | 1438 | 
 | AKAZE | 1327 | 1311 | 1351 | 1360 | 1347 | 1363 | 1331 | 1358 | 1331 | 1351 | 
 | FAST | 1832 | 1810 | 1817 | 1793 | 1796 | 1788 | 1695 | 1749 | 1770 | 1824 | 
 | ORB | 500 | 500 | 500 | 500 | 500 | 500 | 500 | 500 | 500 | 500 | 
 | HARRIS | 502 | 516 | 524 | 523 | 511 | 505 | 510 | 529 | 520 | 492 | 
 | SHITOMASI | 1301 | 1361 | 1358 | 1333 | 1284 | 1322 | 1366 | 1389 | 1339 | 1370 | 


Time (ms)
 | Detect \ Image | #0001 | #0002 | #0003 | #0004 | #0005 | #0006 | #0007 | #0008 | #0009 | #0000 | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 49.55 ms | 62.66 ms | 72.27 ms | 67.83 ms | 58.18 ms | 96.47 ms | 54.38 ms | 55.63 ms | 54.13 ms | 67.30 ms | 
 | AKAZE | 43.02 ms | 42.50 ms | 41.50 ms | 35.92 ms | 36.61 ms | 41.88 ms | 40.51 ms | 38.14 ms | 37.91 ms | 48.95 ms | 
 | FAST | 0.61 ms | 0.63 ms | 0.55 ms | 0.79 ms | 0.59 ms | 0.55 ms | 0.71 ms | 0.68 ms | 0.68 ms | 0.66 ms | 
 | ORB | 6.57 ms | 6.68 ms | 5.19 ms | 4.58 ms | 5.11 ms | 5.62 ms | 4.76 ms | 4.67 ms | 6.99 ms | 6.08 ms | 
 | HARRIS | 10.22 ms | 7.48 ms | 7.38 ms | 7.97 ms | 6.85 ms | 6.48 ms | 7.28 ms | 7.96 ms | 7.56 ms | 7.82 ms | 
 | SHITOMASI | 7.88 ms | 6.68 ms | 7.02 ms | 7.57 ms | 8.59 ms | 8.92 ms | 8.29 ms | 9.04 ms | 9.12 ms | 8.69 ms | 


Time per keypoint (µs)
 | Detect \ Image | #0001 | #0002 | #0003 | #0004 | #0005 | #0006 | #0007 | #0008 | #0009 | #0000 | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | SIFT | 36.14 µs | 45.41 µs | 54.13 µs | 51.97 µs | 42.49 µs | 69.11 µs | 39.35 µs | 38.02 µs | 38.07 µs | 46.80 µs | 
 | AKAZE | 32.42 µs | 32.42 µs | 30.72 µs | 26.41 µs | 27.18 µs | 30.72 µs | 30.43 µs | 28.08 µs | 28.48 µs | 36.23 µs | 
 | FAST | 0.33 µs | 0.35 µs | 0.30 µs | 0.44 µs | 0.33 µs | 0.31 µs | 0.42 µs | 0.39 µs | 0.38 µs | 0.36 µs | 
 | ORB | 13.15 µs | 13.37 µs | 10.38 µs | 9.16 µs | 10.21 µs | 11.25 µs | 9.52 µs | 9.34 µs | 13.98 µs | 12.17 µs | 
 | HARRIS | 20.36 µs | 14.49 µs | 14.08 µs | 15.24 µs | 13.41 µs | 12.84 µs | 14.27 µs | 15.04 µs | 14.54 µs | 15.89 µs | 
 | SHITOMASI | 6.05 µs | 4.91 µs | 5.17 µs | 5.68 µs | 6.69 µs | 6.74 µs | 6.07 µs | 6.51 µs | 6.81 µs | 6.34 µs | 


#### MP.9.2 - Descriptor perfromance 
Time per points
 | Descriptor \ Image | #0001 | #0002 | #0003 | #0004 | #0005 | #0006 | #0007 | #0008 | #0009 | #0000 | 
 | :----: |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  |  :----:  | 
 | BRISK | 8.44 µs | 8.47 µs | 7.73 µs | 12.07 µs | 13.00 µs | 7.72 µs | 10.03 µs | 11.45 µs | 12.25 µs | 7.91 µs | 
 | AKAZE | 255.02 µs | 230.88 µs | 211.53 µs | 193.20 µs | 206.35 µs | 185.18 µs | 203.84 µs | 170.78 µs | 181.92 µs | 263.23 µs | 
 | SIFT | 362.42 µs | 455.68 µs | 713.61 µs | 424.58 µs | 420.74 µs | 339.87 µs | 313.85 µs | 288.42 µs | 393.61 µs | 335.78 µs | 
 | FREAK | 128.72 µs | 148.67 µs | 134.78 µs | 218.89 µs | 144.59 µs | 127.86 µs | 126.11 µs | 119.24 µs | 252.77 µs | 136.47 µs | 
 | BRIEF | 5.19 µs | 5.35 µs | 6.49 µs | 2.96 µs | 3.34 µs | 3.40 µs | 3.49 µs | 4.79 µs | 7.17 µs | 5.13 µs | 
 | ORB | 21.72 µs | 13.36 µs | 14.57 µs | 14.49 µs | 12.71 µs | 12.86 µs | 12.54 µs | 12.54 µs | 11.53 µs | 17.52 µs | 

## References

1. Instructions for setting up the system [Roman Smirnov](https://gist.github.com/roman-smirnov/efff8bb1db8a4063600a40c29a3a0874)


