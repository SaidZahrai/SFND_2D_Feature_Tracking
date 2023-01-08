# SFND 2D Feature Tracking

<img src="images/keypoints.png" width="820" height="248" />

The idea of the camera course is to build a collision detection system - that's the overall goal for the Final Project. As a preparation for this, you will now build the feature tracking part and test various detector / descriptor combinations to see which ones perform best. This mid-term project consists of four parts:

* First, you will focus on loading images, setting up data structures and putting everything into a ring buffer to optimize memory load. 
* Then, you will integrate several keypoint detectors such as HARRIS, FAST, BRISK and SIFT and compare them with regard to number of keypoints and speed. 
* In the next part, you will then focus on descriptor extraction and matching using brute force and also the FLANN approach we discussed in the previous lesson. 
* In the last part, once the code framework is complete, you will test the various algorithms in different combinations and compare them with regard to some performance measures. 

See the classroom instruction and code comments for more details on each of these parts. Once you are finished with this project, the keypoint matching part will be set up and you can proceed to the next lesson, where the focus is on integrating Lidar points and on object detection using deep-learning. 

## Dependencies for Running Locally
1. cmake >= 2.8
 * All OSes: [click here for installation instructions](https://cmake.org/install/)

2. make >= 4.1 (Linux, Mac), 3.81 (Windows)
 * Linux: make is installed by default on most Linux distros
 * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
 * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)

3. OpenCV >= 4.1
 * All OSes: refer to the [official instructions](https://docs.opencv.org/master/df/d65/tutorial_table_of_content_introduction.html)
 * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors. If using [homebrew](https://brew.sh/): `$> brew install --build-from-source opencv` will install required dependencies and compile opencv with the `opencv_contrib` module by default (no need to set `-DOPENCV_ENABLE_NONFREE=ON` manually). 
 * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)

4. gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using either [MinGW-w64](http://mingw-w64.org/doku.php/start) or [Microsoft's VCPKG, a C++ package manager](https://docs.microsoft.com/en-us/cpp/build/install-vcpkg?view=msvc-160&tabs=windows). VCPKG maintains its own binary distributions of OpenCV and many other packages. To see what packages are available, type `vcpkg search` at the command prompt. For example, once you've _VCPKG_ installed, you can install _OpenCV 4.1_ with the command:
```bash
c:\vcpkg> vcpkg install opencv4[nonfree,contrib]:x64-windows
```
Then, add *C:\vcpkg\installed\x64-windows\bin* and *C:\vcpkg\installed\x64-windows\debug\bin* to your user's _PATH_ variable. Also, set the _CMake Toolchain File_ to *c:\vcpkg\scripts\buildsystems\vcpkg.cmake*.


## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`.

### Detectors
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

### Descriptors

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

### Matcher
* [Brute Force](https://docs.opencv.org/3.4/d3/da1/classcv_1_1BFMatcher.html)
* [FLANN (Fast Library for Approximate Nearest Neighbors)](https://docs.opencv.org/3.4/dc/de2/classcv_1_1FlannBasedMatcher.html)

### Selector
* Best match simply returns the best match between the two sets of key points.
* k nearest neighbors with (k=2) first returns the two closest points and then selects the keypoint, where the distance ratio is 0.8 or less, i.e. at least 20% closer match than the second candidate.

