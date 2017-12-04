#ifndef __OpenCVHelper_H__
#define __OpenCVHelper_H__

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"


namespace OpenCVHelper {
	void SIFT_obj_identify(const cv::Mat& img1, const cv::Mat& img2, std::vector<cv::Point2f>& matchedCorners);
};

#endif
