#define JIAN

int cvFindChessboardCorners3( const void* arr, CvSize pattern_size,
#ifdef JIAN
	cv::Mat &outCorners,
#endif
                             CvPoint2D32f* out_corners, int* out_corner_count,
                             int min_number_of_corners );

#ifdef JIAN

static bool FindChessboardCorners3(const cv::Mat &image, cv::Size patternSize, cv::Mat &corners, std::string winname = "", int delay=1000)
{
	CvSize board_size = { patternSize.width, patternSize.height };

	int min_number_of_corners = patternSize.area();

	int count = 0;

	IplImage view = image;

	int found = cvFindChessboardCorners3(&view, board_size, corners,
		0, &count, min_number_of_corners);

	if (!winname.empty())
	{
		std::vector<cv::Point2f> ptl(corners.begin<cv::Vec2f>(), corners.end<cv::Vec2f>());
		cv::drawChessboardCorners(image, patternSize, ptl, found == 1);
		cv::imshow(winname, image);
		cv::waitKey(delay);
		cv::destroyWindow(winname);
	}

	return found == 1;
}

#endif