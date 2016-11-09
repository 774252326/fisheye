#pragma once

#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#if 0
static void BU(std::string &str)
{
	if(str.back()=='/')
		str.pop_back();

	if(str.back()!='\\')
		str.push_back('\\');
};
#endif

static std::string TimeString(unsigned int bSeg=0)
{
	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo=localtime(&rawtime);
	const size_t l=20;
	char buffer[l];
	switch(bSeg)
	{
	case 1:
		strftime(buffer,l,"%Y-%m-%d %H:%M:%S",timeinfo);
		break;
	case 2:
		strftime(buffer,l,"%Y%m%d_%H%M%S",timeinfo);
		break;
	default:
		strftime(buffer,l,"%Y%m%d%H%M%S",timeinfo);
		break;
	}

	std::string re(buffer);
	return re;
};


static std::string ms2str(clock_t nms)
{
	if(nms<1000)
	{
		char buf[6];
		sprintf(buf,"%dms",nms);
		return std::string(buf);
	}

	size_t ns=nms/1000;
	nms=nms%1000;	

	if(ns<60)
	{
		char buf[9];
		sprintf(buf,"%ds%dms",ns,nms);
		return std::string(buf);
	}

	size_t nm=ns/60;
	ns=ns%60;

	if(nm<60)
	{
		char buf[12];
		sprintf(buf,"%dm%ds%dms",nm,ns,nms);
		return std::string(buf);
	}

	size_t nh=nm/60;
	nm=nm%60;

	char buf[50];
	sprintf(buf,"%dh%dm%ds%dms",nh,nm,ns,nms);
	return std::string(buf);
};

#if 1
static int cpy(std::string dst, std::string src)
{
        std::ifstream ifs(src.c_str(),std::ifstream::binary);

	if(ifs.fail())
	{
		return 1;
	}
        std::ofstream ofs(dst.c_str(),std::ofstream::binary);
	if(ofs.fail())
	{
		ifs.close();
		return 2;
	}

	ofs<<ifs.rdbuf();
	ifs.close();

	ofs.close();

	return 0;
};
#endif

static int dlt(std::string fp)
{
	return remove(fp.c_str());
};

static int ren(std::string newName, std::string oldName)
{
	return rename(oldName.c_str(),newName.c_str());
};


static bool LoadList(std::vector<std::string> &list, std::string fp)
{
        std::ifstream ifs(fp.c_str());
	if(ifs)
	{
		std::string ifn;
		while(!ifs.eof())
		{
			std::getline(ifs,ifn);
			if(!ifn.empty())
				list.push_back(ifn);
		}
		ifs.close();
		return true;
	}
	return false;
};

static bool SaveList(const std::vector<std::string> &list, std::string fp)
{
        std::ofstream ofs(fp.c_str());
	if (ofs)
	{
		for (size_t i = 0; i < list.size(); i++)
			ofs << list[i] << '\n';
		ofs.close();
		return true;
	}
	return false;
};
