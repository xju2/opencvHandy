#include "OpenCVHelper.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "opencv2/calib3d/calib3d.hpp" // for findHomography
#include "opencv2/xfeatures2d/nonfree.hpp"

#include <vector>
#include <iostream>

using namespace cv;
using namespace cv::xfeatures2d;

using namespace std;

void OpenCVHelper::SIFT_obj_identify(
		const Mat& img1, 
		const Mat& img2, 
		vector<Point2f>& matchedCorners)
{
	bool debug = false;
	Ptr<SIFT> detector = SIFT::create();
//	const int minHessian = 400;
//	Ptr<SURF> detector = SURF::create(minHessian);
	// Ptr<ORB> detector = ORB::create();
	
	vector<KeyPoint> kp1, kp2;
	Mat des1, des2;
	detector->detectAndCompute(img1, noArray(), kp1, des1);
	detector->detectAndCompute(img2, noArray(), kp2, des2);
	if(debug) cout <<"computed features" << endl;

	const int checks = 50;
	Ptr<flann::SearchParams> searchParams = new flann::SearchParams(checks);
	Ptr<flann::KDTreeIndexParams> indexParams = new flann::KDTreeIndexParams(5);
	
	auto flann = FlannBasedMatcher(indexParams, searchParams);
	if(debug) cout <<"created FLANN Matcher" << endl;

	vector<vector<DMatch> > matches;	
	flann.knnMatch(des1, des2, matches, 2); 

	vector<DMatch> good_matches;
	// perform a ratio test.
	const float minRatio = 1./1.5;

	for(int i = 0; i < (int)matches.size(); i++) {
		const DMatch& bestMatch = matches[i][0];
		const DMatch& betterMatch = matches[i][1];
		float distanceRatio = bestMatch.distance / betterMatch.distance;
		if(distanceRatio < minRatio) {
			good_matches.push_back(bestMatch);
		}
	}

	if(debug) cout <<"found good matches" << endl;

	const int min_match_count = 10;
	if(good_matches.size() > min_match_count) {
		vector<Point2f> src_pts;
		vector<Point2f> dst_pts;

		for(int i = 0; i < (int) good_matches.size(); i++) {
			src_pts.push_back( kp1[ good_matches[i].queryIdx ].pt );
			dst_pts.push_back( kp2[ good_matches[i].trainIdx ].pt );
		}
		if(debug) cout <<"created source and destination points" << endl;

		Mat H = findHomography(src_pts, dst_pts, cv::RANSAC, 5.0);

		//-- Get the corners from the img1 (the object to be "detected")
		vector<Point2f> obj_corners(4);
		obj_corners[0] = cvPoint(0, 0);
		obj_corners[1] = cvPoint(img1.cols, 0);
		obj_corners[2] = cvPoint(img1.cols, img1.rows);
		obj_corners[3] = cvPoint(0, img1.rows);
		// vector<Point2f> scene_corners(4);
		if(debug) cout << "before the transformation" << endl;
		/**
		if(matchedCorners.size() != 4) {
			matchedCorners.reserve(4);
		}
		**/
		perspectiveTransform(obj_corners, matchedCorners, H);
	} else {
		cout <<"Found less than " << min_match_count << " matches" << endl;
	}
	if(debug) cout << "Finished" <<endl;
	return;
}
