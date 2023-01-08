/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /* INIT VARIABLES AND DATA STRUCTURES */

    // data location
    string dataPath = "../";

    // camera
    string imgBasePath = dataPath + "images/";
    string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
    string imgFileType = ".png";
    int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
    int imgEndIndex = 9;   // last file index to load
    int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

    // misc
    int dataBufferSize = 2;       // no. of images which are held in memory (ring buffer) at the same time
    vector<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time


    //// Default settings:
    bool bVis = true;            // visualize results
    string detectorType = "FAST"; // "FAST"; // "ORB"; // "HARRIS"; // "SIFT"; // "SHITOMASI"; // "AKAZE"; // 
    string descriptorType = "BRISK"; // "BRIEF"; // "BRISK"; // "FREAK"; // "ORB"; //  "AKAZE"; // "SIFT"; //
    string matcherType = "MAT_BF";        // "MAT_BF"; // "MAT_FLANN"; //
    string selectorType = "SEL_NN";       // "SEL_NN"; // "SEL_KNN"; //

    if (argc == 1)
    {
        cout << "You have started the program without any arguments. This is similar to:\n";
        cout << "\t2D_feature_tracking FAST BRISK MAT_BF SEL_NN TRUE\n";
        cout << "meaning you intend to run 2D_feature_tracking with\n";
        cout << "\tDetector: FAST [HARRIS/SHITOMASI/FAST/ORB/AKAZE/SIFT]\n";
        cout << "\tDescriptor: BRISK [BRIEF/BRISK/FREAK/ORB/AKAZE/SIFT]\n";
        cout << "\tMatcher: [MAT_BF/MAT_FLANN]\n";
        cout << "\tSelector: [SEL_NN/SEL_KNN]\n";
        cout << "\tVisualization: ON [ON/OFF]\n";
    } 
    else if (argc == 6)
    {
        detectorType = string(argv[1]);
        if (!((detectorType.compare("HARRIS") == 0) || (detectorType.compare("SHITOMASI")) || (detectorType.compare("FAST")) ||
            (detectorType.compare("ORB") == 0) || (detectorType.compare("AKAZE") == 0) || (detectorType.compare("SIFT") == 0)))
        {
            cout << detectorType << " not recognized. Please check your choice for detector.\n";
            cout << "\tDetector: [HARRIS/SHITOMASI/FAST/ORB/AKAZE/SIFT]\n";
            return 1;
        }
        descriptorType = string(argv[2]);
        if (!((descriptorType.compare("BRIEF") == 0) || (descriptorType.compare("BRISK")) || (descriptorType.compare("FREAK")) ||
            (descriptorType.compare("ORB") == 0) || (descriptorType.compare("AKAZE") == 0) || (descriptorType.compare("SIFT") == 0)))
        {
            cout << descriptorType << " not recognized. Please check your choice for descriptor.\n";
            cout << "\tDescriptor: [BRIEF/BRISK/FREAK/ORB/AKAZE/SIFT]\n";
            return 1;
        }
        matcherType = string(argv[3]);
        if (!((matcherType.compare("MAT_BF") == 0) || (matcherType.compare("MAT_FLANN") == 0)))
        {
            cout << matcherType << " not recognized. Please check your choice for matcher.\n";
            cout << "\tMAT_BF [MAT_BF/MAT_FLANN]\n";
            return 1;
        }
        selectorType = string(argv[4]);
        if (!((selectorType.compare("SEL_NN") == 0) || (selectorType.compare("SEL_KNN") == 0)))
        {
            cout << selectorType << " not recognized. Please check your choice for selector.\n";
            cout << "\tSelector: [SEL_NN/SEL_KNN]\n";
            return 1;
        }
        bVis = string(argv[5]).compare("ON") == 0;
        if (!((string(argv[5]).compare("ON") == 0) || (string(argv[5]).compare("OFF") == 0)))
        {
            cout << string(argv[5]) << " not recognized. Please check your choice for visualization.\n";
            cout << "\tVisualization: [ON/OFF]\n";
            return 1;
        }
    }
    else
    {
        cout << "You have entered " << argc << " arguments:" << "\n";
    
        for (int i = 0; i < argc; ++i)
            cout << argv[i] << "\n";

        cout << "Please execute\n";
        cout << "\t2D_feature_tracking Detector Descriptor Matcher Selector Visualization\n";
        cout << "with one of the following choices:\n";
        cout << "\tDetector: [HARRIS/SHITOMASI/FAST/ORB/AKAZE/SIFT]\n";
        cout << "\tDescriptor: [BRIEF/BRISK/FREAK/ORB/AKAZE/SIFT]\n";
        cout << "\tMatcher: [MAT_BF/MAT_FLANN]\n";
        cout << "\tSelector: [SEL_NN/SEL_KNN]\n";
        cout << "\tVisualization: [ON/OFF]\n";
        return 1;
    }


    //// Consistency check:
    if ((descriptorType.compare("AKAZE") == 0) && (detectorType.compare("AKAZE") != 0))
    {
        std::cout << "AKAZE descriptor requires AKAZE detector. Execution aborted.\n";
        return 1;
    }

    cout << "*** Start of execution of the command  ";
    for (int i = 0; i < argc; ++i)
        cout << argv[i] << " " ;
    cout <<  "\n";

    /* MAIN LOOP OVER ALL IMAGES */
    for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
    {
        /* LOAD IMAGE INTO BUFFER */

        // assemble filenames for current index
        ostringstream imgNumber;
        imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
        string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

        // load image from file and convert to grayscale
        cv::Mat img, imgGray;
        img = cv::imread(imgFullFilename);
        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

        //// STUDENT ASSIGNMENT
        //// TASK MP.1 -> Keep the buffer size constant equal to dataBufferSize
        if (dataBuffer.size() == dataBufferSize)
            dataBuffer.erase(dataBuffer.begin());

        // push image into data frame buffer
        DataFrame frame;
        frame.cameraImg = imgGray;
        dataBuffer.push_back(frame);

        //// EOF STUDENT ASSIGNMENT
        cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

        /* DETECT IMAGE KEYPOINTS */


        //// TASK MP.2 -> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT detectors added in file matching2D.cpp
        //// and can be selected by the following string before the loop
        ////    string detectorType = "AKAZE"; // "ORB"; // "HARRIS"; // "SIFT"; // "SHITOMASI"; //

        // extract 2D keypoints from current image
        vector<cv::KeyPoint> keypoints; // create empty feature list for current image
        double t;
        if (detectorType.compare("HARRIS") == 0)
        {
            std::cout << "Detector: " << detectorType << ".\n";
            detKeypointsHarris(keypoints, imgGray, t, false);
        }
        else if (detectorType.compare("SHITOMASI") == 0)
        {
            std::cout << "Detector: " << detectorType << ".\n";
            detKeypointsShiTomasi(keypoints, imgGray, t, false);
        }
        else 
        {
            detKeypointsModern(keypoints, imgGray, detectorType, t, false);
        }
        cout << "MP.9.1 " << detectorType << " found " << keypoints.size() << " totally in " << t*1000.0 << " milliseconds" << endl;

        //// EOF STUDENT ASSIGNMENT

        //// TASK MP.3 -> only keep keypoints on the preceding vehicle
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

        cout << "MP.7 " << detectorType << " found " << keypoints.size() << " points in the window in image #";
        cout << imgNumber.str() << " with averaged size ";
        float averageSize = 0.0;
        for (auto itr = keypoints.begin(); itr != keypoints.end(); itr++)
        {
            averageSize += itr->size;
        }
        cout << averageSize/keypoints.size() << endl;

        //// EOF STUDENT ASSIGNMENT

        // optional : limit number of keypoints (helpful for debugging and learning)
        bool bLimitKpts = false;
        if (bLimitKpts)
        {
            int maxKeypoints = 50;

            if (detectorType.compare("SHITOMASI") == 0)
            { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
            }
            cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
            cout << " NOTE: Keypoints have been limited!" << endl;
        }

        // push keypoints and descriptor for current frame to end of data buffer
        (dataBuffer.end() - 1)->keypoints = keypoints;
        cout << "#2 : DETECT KEYPOINTS done" << endl;

        /* EXTRACT KEYPOINT DESCRIPTORS */

        //// STUDENT ASSIGNMENT
        //// TASK MP.4 -> BRIEF, ORB, FREAK, AKAZE, SIFT descriptors added in file matching2D.cpp 
        //// The descriptor type can be selected by the belo string before the loop
        ////    string descriptorType = "SIFT" or "BRIEF" or "BRISK" or "FREAK" or "ORB" or  "AKAZE" or "SIFT"

        cv::Mat descriptors;
        descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType, t);
        cout << "MP.9.2 " << descriptorType << " found " << descriptors.size() << " totally in " << t*1000.0 << " milliseconds" << endl;

        //// EOF STUDENT ASSIGNMENT

        // push descriptors for current frame to end of data buffer
        (dataBuffer.end() - 1)->descriptors = descriptors;

        cout << "#3 : EXTRACT DESCRIPTORS done" << endl;

        if (dataBuffer.size() > 1) // wait until at least two images have been processed
        {

            /* MATCH KEYPOINT DESCRIPTORS */

            // All descriptors have binary output vectors except SIFT which has a histogram based description vecctor
            string descriptorOutputType;
            if (descriptorType.compare("SIFT") == 0)
            {
                descriptorOutputType =  "DES_HOG";
            }
            else 
            {
                descriptorOutputType =  "DES_BINARY";
            }

            //// STUDENT ASSIGNMENT
            //// TASK MP.5 -> FLANN matching added in file matching2D.cpp
            //// TASK MP.6 -> KNN match with k=2 and selection with distance ratio filtering t=0.8 added in file matching2D.cpp
            //// They are selected by two following strings before the loop:
            ////    string matcherType = "MAT_BF" or "MAT_FLANN"
            ////    string selectorType = "SEL_NN" or "SEL_KNN"


            vector<cv::DMatch> matches;
            matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                             (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                             matches, descriptorOutputType, matcherType, selectorType, t);

            //// EOF STUDENT ASSIGNMENT

            // store matches in current data frame
            (dataBuffer.end() - 1)->kptMatches = matches;


            cout << "MP.8 " << detectorType << " " <<  descriptorType << " " <<  matcherType << " " <<  selectorType;
            cout << " found " << matches.size() << " matches in the window in image #" << imgNumber.str() << endl;
            cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;
            cout << "MP.9.3 " << matcherType << " and " << selectorType << " found " << matches.size() << 
            " matches in " << t*1000.0 << " milliseconds" << endl;

            // visualize matches between current and previous image
            if (bVis)
            {
                cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                matches, matchImg,
                                cv::Scalar::all(-1), cv::Scalar::all(-1),
                                vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                string windowName = "Matching keypoints between two camera images";
                cv::namedWindow(windowName, 7);
                cv::imshow(windowName, matchImg);
                cout << "Press key to continue to next image" << endl;
                cv::waitKey(0); // wait for key to be pressed
            }
        }
    } // eof loop over all images

    cout << "***  End of execution of the command  ";
    for (int i = 0; i < argc; ++i)
        cout << argv[i] << " " ;
    cout <<  "\n";
    cout << "***" << endl;

    return 0;
}