#pragma once

#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "fiocpp.hpp"

static std::string FileName(int x, int y)
{
	char buf[40];
	sprintf(buf,"%.4d_%.4d",x,y);
	return std::string(buf);
};


static std::string FileNameZS(double z, int idx)
{
	char buf[40];
	sprintf(buf,"zs[%02d][%g]",idx,z);
	return std::string(buf);
};



template <typename T, typename TT>
static bool LoadFocusCurve(std::vector<T> &x, std::vector<TT> &y, T &xmax, TT &ymax, std::string fp)
{
	std::ifstream ifs(fp);
	if(ifs)
	{
		ifs>>xmax>>ymax;
		//ifs>>xmax;
		T xt;
		TT yt;
		while(!ifs.eof() && ifs>>xt>>yt)
		{
			x.push_back(xt);
			y.push_back(yt);
		}
		ifs.close();
		return true;
	}
	return false;
};


template <typename T, typename TT>
static bool SaveFocusCurve(const std::vector<T> &x, const std::vector<TT> &y, const T &xmax, std::string fp)
{
	std::ofstream ofs(fp);
	if(ofs)
	{
		ofs<<xmax<<'\t'<<0<<'\n';
		for(size_t i=0;i<x.size();i++)
		{
			ofs<<x[i]<<'\t'<<y[i]<<'\n';
		}			
		ofs.close();
		return true;
	}
	return false;
};


static void LoadFileNameZS(double &z, int &idx, std::string fn)
{
	std::string seg="[";
	std::string::size_type found1=fn.rfind(seg);

	std::string oneline1=fn.substr(found1+1);
	std::istringstream iss1(oneline1);
	iss1>>z;

	fn.erase(fn.begin()+found1,fn.end());
	std::string::size_type found2=fn.rfind(seg);

	std::string oneline2=fn.substr(found2+1);
	std::istringstream iss2(oneline2);
	iss2>>idx;

	std::cout<<"\n"<<z<<','<<idx;
};


static std::string AFstr(int fmidx)
{
	switch(fmidx)
	{
	case 0:
		return "z stack 2 pass";
	case 1:
		return "one shot";
	case 2:
		return "focus map";
	case 3:
		return "z stack";
	case 4:
		return "dense map";
	case 5:
		return "focus peak";
	case 6:
		return "Harley";
	case 7:
		return "hybrid";
	case 8:
		return "two shot";
	default:
		return "undefined";
	}
};


static std::string ImageExtention(int idx)
{
	switch(idx)
	{
	case 0:
		return "bmp";
	case 1:
		return "jpg";
	case 2:
		return "png";
	case 3:
		return "tif";
	default:
		return "bmp";
	}
};

static void GetFolderFileList(std::vector<std::string> &fileList, std::string folder, std::string filter="", std::string restmp=TimeString(2)+".txt")
{
	std::string cmmd="dir \""+folder+"\\"+filter+"\" /b "+" > "+restmp;
	system(cmmd.c_str());
	LoadList(fileList,restmp);
	dlt(restmp);
};

#define WINWIN

#ifdef WINWIN

#include "fiowin.hpp"

#endif


//#define QTT

#ifdef QTT
#include "fioqt.hpp"
#endif




