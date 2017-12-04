#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

bool try_use_gpu = false;
bool divide_images = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;
string result_name = "result.jpg";

void printUsage(char** argv);
int parseCmdArgs(int argc, char** argv);

int main(int argc, char* argv[])
{
	int retval = parseCmdArgs(argc, argv);
	if(retval)	return -1;

	Mat pano;
	Ptr<Stitcher> stitcher = Stitcher::create(mode, try_use_gpu);
	Stitcher::Status status = stitcher->stitch(imgs, pano);

	if(status != Stitcher::OK) {
		cout <<"Cannot stitch images, error code = " << int(status) <<endl;
		return -1;
	}

	imwrite(result_name, pano);
	cout <<"stitching completed successfully\n" << result_name << " saved!" << endl;

	return 0;
}

void printUsage(char** argv) {
	cout <<
			"Image stitcher.\n\n" << "Usage :\n" << argv[0] << " [Flags] img1 img2 [..imgN]\n\n"
			"Flags:\n"
			"  --d3\n"
			"       internally creates three chunks of each image to increase stitching success"
			"  --try_use_gpu (yes|no)\n"
			"       try to use GPU. The default value is 'no'. "
			"  --mode (panorama|scans)\n"
			"       determines configuration of stitcher. The default is 'panorama',\n"
			"  --output <result_img>\n"
			"       The default is 'result.jpg'.\n\n"
			"Example usage :\n"  << argv[0] << " --d3 --try_use_gpu yes --mode scans img1.jpg img2.jpg\n";

}

int parseCmdArgs(int argc, char** argv)
{
	if (argc < 2){
		printUsage(argv);
		return -1;
	}

	for(int i = 1; i < argc; ++i)
	{
		string arg(argv[i]);
		if(arg == "--help" || arg == "/?") {
			printUsage(argv);
			return -1;
		} else if( arg == "--try_use_gpu") {
			if(string(argv[i + 1]) == "no") {
				try_use_gpu = false;
			} else if(string(argv[i+1]) == "yes"){
				try_use_gpu = true;
			} else {
				cout << "Bad --try_use_gpu flag value!\n";
				return -1;
			}
			i++;
		} else if (arg == "--d3"){
			divide_images = true;
		} else if (arg == "--output"){
			result_name = argv[i+1];
			i++;
		} else if (arg == "--mode"){
			string arg_val(argv[i+1]);
			if(arg_val == "panorama") {
				mode = Stitcher::PANORAMA;
			} else if (arg_val == "scans"){
				mode = Stitcher::SCANS;
			} else {
				cout <<"Bad --mode flag value\n";
				return -1;
			}
			i++;
		} else {
			Mat img = imread(argv[i]);
			if(img.empty()){
				cout <<"Cannot read image '" << argv[i] << "'\n";
				return -1;
			}
			if(divide_images) {
				Rect rect(0, 0, img.cols/2, img.rows);
				imgs.push_back(img(rect).clone());
				rect.x = img.cols / 3;
				imgs.push_back(img(rect).clone());
				rect.x = img.cols / 2;
				imgs.push_back(img(rect).clone());
			} else {
				imgs.push_back(img);
			}
		}
	}
	return 0;
}



















