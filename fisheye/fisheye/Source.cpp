#include <opencv2\opencv.hpp>

#include "fio.hpp"



static bool fcbc(const cv::Mat &image, cv::Size patternSize, std::vector<cv::Point2f> &corners, std::string winname = "", int delay = 1000)
{
	bool flg = cv::findChessboardCorners(image, patternSize, corners);

	cv::drawChessboardCorners(image, patternSize, corners, flg);

	if (!winname.empty())
	{
		cv::imshow(winname, image);
		cv::waitKey(delay);
		cv::destroyWindow(winname);
	}

	return flg;

}


static void SelectFiles(std::vector<std::string> &filelist, std::string folder, std::string ext = ".png")
{
	std::vector<std::string> fol;

	GetFolderContentList(filelist, fol, folder);

	for (size_t i = 0; i < filelist.size();)
	{
		if (filelist[i].size() >= ext.size() && filelist[i].compare(filelist[i].size() - ext.size(), ext.size(), ext) == 0)
		{
			i++;
		}
		else
		{
			filelist.erase(filelist.begin() + i);
		}
	}
}


static void CalcBoardCornerPositionsSymmetric(cv::Mat &corners, int cornerCols, int cornerRows, float SquareSize) {

	corners.create(cornerRows, cornerCols, CV_32FC3);

	for (int i = 0; i < cornerRows; i++)
	{
		for (int j = 0; j < cornerCols; j++)
		{
			corners.at<cv::Vec3f>(i, j)[0] = j * SquareSize;
			corners.at<cv::Vec3f>(i, j)[1] = i * SquareSize;
			corners.at<cv::Vec3f>(i, j)[2] = 0;
		}
	}

	corners = corners.reshape(corners.channels(), corners.total());
};

static void SaveMat(std::string fp, const cv::Mat &m)
{
	cv::FileStorage fs(fp, cv::FileStorage::WRITE);
	fs << "m" << m;
	fs.release();
}

static void LoadMat(std::string fp, cv::Mat &m)
{
	cv::FileStorage fs(fp, cv::FileStorage::READ);
	fs["m"] >> m;
	fs.release();
}


static void EquriRectangularToFisheye(double ratioer[2], double ptf[2], const cv::Mat &K, const double d[4], double pi = 3.1415926535)
{
	double beta = ratioer[0] * 2 * pi;
	double theta = ratioer[1] * pi;
	double theta2 = theta*theta;

	double thetad = theta*((((d[3] * theta2 + d[2])*theta2 + d[1])*theta2 + d[0])*theta2 + 1);

	cv::Mat v =( cv::Mat_<double>(3, 1) << thetad*cos(beta), thetad*sin(beta), 1);

	cv::Mat ov = K*v;

	ptf[0] = ov.at<double>(0);
	ptf[1] = ov.at<double>(1);
}

static void FisheyeToEquriRectangular(const cv::Mat &mf, const cv::Mat &K, const cv::Mat &D, cv::Mat &mer, double pi=3.1415926535)
{
	double *d = (double*)D.data;

	double ratioer[2];
	double ptf[2];
	for (int i = 0; i < mer.rows; i++)
	{
		ratioer[1] = (double)i/(mer.rows-1);

		if (ratioer[1]>95./180)
			return;

		for (int j = 0; j < mer.cols; j++)
		{
			ratioer[0] = (double)j / mer.cols;
			EquriRectangularToFisheye(ratioer, ptf, K, d, pi);

			int ptfi[2] = { ptf[0], ptf[1] };

			if (ptfi[0] < 0 || ptfi[0] >= mf.cols || ptfi[1] < 0 || ptfi[1] >= mf.rows)
				mer.at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,0);
			else
				mer.at<cv::Vec3b>(i, j) = mf.at<cv::Vec3b>(ptfi[1], ptfi[0]);
		}
	}
}


static void CalibrateTheta(std::string folder, float intvmm = 15)
{
	std::vector<std::string> fnl, fol;

	SelectFiles(fnl, folder, ".yml");


	cv::Rect roi(0, 0, 960, 970);


	std::vector<cv::Mat> objectPoints, corners;

	std::vector<std::vector<cv::Point3f>> ops(fnl.size(), std::vector<cv::Point3f>());
	std::vector<std::vector<cv::Point2f>> ips(fnl.size(), std::vector<cv::Point2f>());

	for (size_t i = 0; i<fnl.size(); i++)
	{
		corners.push_back(cv::Mat());
		LoadMat(folder + fnl[i], corners.back());
		ips[i].assign(corners.back().begin<cv::Vec2f>(), corners.back().end<cv::Vec2f>());

		objectPoints.push_back(cv::Mat());
		CalcBoardCornerPositionsSymmetric(objectPoints.back(), corners.back().cols, corners.back().rows, intvmm);

		ops[i].assign(objectPoints.back().begin<cv::Vec3f>(), objectPoints.back().end<cv::Vec3f>());
	}

	cv::Mat K, D;
	//cv::Matx33d K;
	//cv::Vec4d D;
	//std::vector<cv::Mat> rvecs, tvecs;
	std::vector<cv::Vec3d> rvecs, tvecs;


	int flag =
		cv::fisheye::CALIB_FIX_SKEW |
		//cv::fisheye::CALIB_CHECK_COND | 
		cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;



	//double rms = cv::fisheye::calibrate(ops, ips, roi.size(), K, D, cv::noArray(), cv::noArray(), flag);
	double rms = cv::fisheye::calibrate(ops, ips, roi.size(), K, D, rvecs, tvecs, flag,
		cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10000, DBL_EPSILON));



	//double rms=cv::calibrateCamera(ops, ips, roi.size(), K, D, rvecs, tvecs);

	std::cout << "K=" << K << '\n';
	std::cout << "D=" << D << '\n';
	std::cout << "rms=" << rms << '\n';

	SaveMat("K.yml", K);
	SaveMat("D.yml", D);
}

static void BlendTheta(const cv::Mat &K, const cv::Mat &D, const cv::Mat &mdualfish, cv::Mat &mequrirectangular)
{
	cv::Mat imageer[2];


	imageer[0].create(mdualfish.cols / 2, mdualfish.cols, mdualfish.type());
	FisheyeToEquriRectangular(mdualfish, K, D, imageer[0]);

	cv::Mat tmp;
	cv::flip(mdualfish, tmp, 1);
	imageer[1].create(mdualfish.cols / 2, mdualfish.cols, mdualfish.type());
	FisheyeToEquriRectangular(tmp, K, D, imageer[1]);

	cv::flip(imageer[1], imageer[1], 0);




	cv::imwrite("er0.jpg", imageer[0]);
	cv::imwrite("er1.jpg", imageer[1]);

	cv::vconcat(imageer[0].rowRange(0, imageer[0].rows / 2), imageer[1].rowRange(imageer[1].rows / 2, imageer[1].rows), mequrirectangular);
		
}

int main()
{




	std::string folder = "L:\\New folder\\";
	//CalibrateTheta(folder);




	std::vector<std::string> fnl;
	SelectFiles(fnl, folder, ".png");

	//cv::Mat image = cv::imread(folder + fnl[0], 1);
	cv::Mat image = cv::imread("L:\\Encode_1080P_5_all.mp4_20160315_170435.593.png", 1);
	cv::Mat K, D, mer;
	LoadMat("K.yml", K);
	LoadMat("D.yml", D);

	BlendTheta(K, D, image, mer);

	cv::imshow("mer", mer);
	cv::waitKey();

	/*
	cv::Mat Knew;
	K.copyTo(Knew);
	Knew(cv::Rect(0, 0, 2, 2)) = Knew(cv::Rect(0, 0, 2, 2)) * 0.3;

	cv::Mat Kd;
	K.convertTo(Kd, CV_64F);
	cv::Mat Dd;
	D.convertTo(Dd, CV_64F);

	for (size_t i = 0; i < fnl.size(); i++)
	{
	cv::Mat image = cv::imread(folder + fnl[i], 1)(roi);

	cv::Mat imageu;
	//cv::undistort(image, imageu, K, D);
	cv::fisheye::undistortImage(image, imageu, K, D, Knew);

	cv::Mat imageer(image.cols / 2, image.cols, image.type());

	FisheyeToEquriRectangular(image, Kd, Dd, imageer);

	cv::Mat canvas;
	cv::vconcat(image, imageu, canvas);
	cv::vconcat(canvas, imageer, canvas);

	cv::imwrite(folder + "er" + fnl[i]+".jpg", imageer);

	cv::imshow(fnl[i], canvas);

	cv::waitKey(1000);

	cv::destroyWindow(fnl[i]);
	}
	*/
	
	return 0;
}