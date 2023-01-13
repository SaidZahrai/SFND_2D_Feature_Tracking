#!/usr/bin/bash
#

# MP.7 Count the number of keypoints on the preceding vehicle for all 10 images and take note 
# of the distribution of their neighborhood size. Do this for all the detectors you have implemented.
# MP.7 AKAZE found 179 points in the window in image #0009 with averaged size 7.88576

echo '#### MP.7.a - Number of keypoints on the preceding vehicle' 
grep MP.7 $1 | awk '
{
    methods[$2]
    images[$11]
    found[$2][$11] = $4
    size[$2][$11] = $15
}
END {
    printf " | Method | "
    for (im in images){
        printf im
        printf " | "
    }
    printf "\n"
    printf " | :----: | "
    for (im in images){
        printf " :----: "
        printf " | "
    }
    printf "\n"
    for (m in methods) {
        printf " | "
        printf m
        printf " | "
        for (im in images){
            printf found[m][im]
            printf " | "
        }
        printf "\n"
    }
}'

echo
echo
echo '#### MP.7.b - Averaged neighborhood size for the keypoints on the preceding vehicle' 

grep MP.7 $1 | awk '
{
    methods[$2]
    images[$11]
    found[$2][$11] = $4
    size[$2][$11] = $15
}
END {
    printf " | Method | "
    for (im in images){
        printf im
        printf " | "
    }
    printf "\n"
    printf " | :----: | "
    for (im in images){
        printf " :----: "
        printf " | "
    }
    printf "\n"
    for (m in methods) {
        printf " | "
        printf m
        printf " | "
        for (im in images){
            printf "%.2f", size[m][im]
            printf " | "
        }
        printf "\n"
    }
}'

echo
echo

# MP.8 Count the number of matched keypoints for all 10 images using all possible 
# combinations of detectors and descriptors. 
# In the matching step, the BF approach is used with the descriptor distance ratio set to 0.8.
# MP.8 AKAZE SIFT MAT_FLANN SEL_KNN found 151 matches in the window in image #0009
# MP.8 AKAZE AKAZE MAT_FLANN SEL_KNN found 143 matches in the window in image #0009

echo
echo
echo '#### MP.8 - Number of matched keypoints' 

grep MP.8 $1 | awk '
{
    detector[$2]
    descriptor[$3]
    matcher[$4]
    selector[$5]
    images[$14]
    matches[$2][$3][$4][$5][$14] = $7
}
END {
    for (mat in matcher){
        for (sel in selector){
            printf "\n"
            printf "Total number of matches for "
            print mat, " and ", sel, "\n"
            printf " | Detect \\ Describe | "
            for (des in descriptor){
                printf des
                printf " | "
            }
            printf "\n"
            printf " | :----: | "
            for (des in descriptor){
                printf " :----: "
                printf " | "
            }
            printf "\n"
            for (det in detector) {
                printf " | "
                printf det
                printf " | "
                for (des in descriptor){
                    totalMatches = 0
                    for (im in images) 
                        totalMatches += matches[det][des][mat][sel][im]
                    if (totalMatches == 0)
                        printf "N/A"
                    else
                        printf totalMatches
                    printf " | "
                }
                printf "\n"
            }
        }
    }
}'

echo
echo



# MP.9 Log the time it takes for keypoint detection and descriptor extraction. 
# The results must be entered into a spreadsheet 
# and based on this data, the TOP3 detector / descriptor combinations must be recommended as the best choice for our 
# purpose of detecting keypoints on vehicles.
echo '#### MP.9.1 - Detector perfromance ' 

# MP.9.1 AKAZE found 1331 in image #0009 in 37.9068 milliseconds
echo 'Number of points' 

grep MP.9.1 $1 | awk '
{
    detector[$2]
    images[$7]
    keypoints[$2][$7] = $4
    time[$2][$7] = $9
}
END {

    printf " | Detect \\ Image | "
    for (im in images){
        printf im
        printf " | "
    }
    printf "\n"
    printf " | :----: | "
    for (im in images){
        printf " :----: "
        printf " | "
    }
    printf "\n"
    for (det in detector) {
        printf " | "
        printf det
        printf " | "
        for (im in images){
            printf keypoints[det][im]
            printf " | "
        }
        printf "\n"
    }
}'

echo
echo
echo 'Time ' 

grep MP.9.1 $1 | awk '
{
    detector[$2]
    images[$7]
    keypoints[$2][$7] = $4
    time[$2][$7] = $9
}
END {

    printf " | Detect \\ Image | "
    for (im in images){
        printf im
        printf " | "
    }
    printf "\n"
    printf " | :----: | "
    for (im in images){
        printf " :----: "
        printf " | "
    }
    printf "\n"
    for (det in detector) {
        printf " | "
        printf det
        printf " | "

        for (im in images){
            printf "%.2f", time[det][im]
            printf " ms | "
        }
        printf "\n"
    }
}'

echo
echo

echo 'Time per keypoint ' 

grep MP.9.1 $1 | awk '
{
    detector[$2]
    images[$7]
    keypoints[$2][$7] = $4
    time[$2][$7] = $9
}
END {

    printf " | Detect \\ Image | "
    for (im in images){
        printf im
        printf " | "
    }
    printf "\n"
    printf " | :----: | "
    for (im in images){
        printf " :----: "
        printf " | "
    }
    printf "\n"
    for (det in detector) {
        printf " | "
        printf det
        printf " | "

        for (im in images){
            printf "%.2f", time[det][im]/keypoints[det][im]*1000
            printf " µs | "
        }
        printf "\n"
    }
}'

echo
echo

echo '#### MP.9.2 - Descriptor perfromance ' 
# MP.9.2 AKAZE found 179 in image #0009 in 32.5634 milliseconds 

echo 'Time per points' 

grep MP.9.2 $1 | awk '
{
    descriptor[$2]
    images[$7]
    keypoints[$2][$7] = $4
    time[$2][$7] = $9
}
END {

    printf " | Descriptor \\ Image | "
    for (im in images){
        printf im
        printf " | "
    }
    printf "\n"
    printf " | :----: | "
    for (im in images){
        printf " :----: "
        printf " | "
    }
    printf "\n"
    for (des in descriptor) {
        printf " | "
        printf des
        printf " | "

        for (im in images){
            printf "%.2f", time[des][im]/keypoints[des][im]*1000
            printf " µs | "
        }
        printf "\n"
    }
}'

echo
echo


