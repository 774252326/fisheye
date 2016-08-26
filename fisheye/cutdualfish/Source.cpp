#include <opencv2\opencv.hpp>

int main()
{
	std::string folder = "L:\\";
	std::string filename = "R0016813.MP4_20160316_173859.578";
	std::string ext = ".png";

	cv::Mat m = cv::imread(folder + filename + ext);

	int w = m.cols / 2;
	cv::Mat fish = m(cv::Rect(0, 0, w, w));
	cv::imwrite(folder + filename + "-0" + ext, fish);
	cv::flip(m(cv::Rect(w, 0, w, w)), fish, 1);
	cv::imwrite(folder + filename + "-1" + ext, fish);

	return 0;
}