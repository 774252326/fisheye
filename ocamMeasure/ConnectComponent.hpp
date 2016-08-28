#ifndef CONNECTCOMPONENT_HPP
#define CONNECTCOMPONENT_HPP

#include <opencv2/opencv.hpp>
#include <time.h>
#include <algorithm>
#include <functional>
#include <numeric>

class ConnectComponent
{
protected:

    static inline void RowCheckCC(std::vector< std::vector<cv::Point> > &cc1, const uchar *rptr, int nd, uchar one, int ridx)
	{
		if (rptr[0] == one)
		{
			cc1.push_back(std::vector<cv::Point>(1, cv::Point(0, ridx)));
		}

		for (int i = 1; i < nd; i++)
		{
			if (rptr[i] == one)
			{
				if (rptr[i - 1] != one)
					cc1.push_back(std::vector<cv::Point>(1, cv::Point(i, ridx)));
				else
					cc1.back().push_back(cv::Point(i, ridx));
			}
		}
	};

	static inline void GetRowCC(const cv::Mat &bw, std::vector< std::vector< std::vector<cv::Point> > > &ccr, uchar one)
	{
		ccr.resize(bw.rows);
		for (int ri = 0; ri < bw.rows; ri++)
		{
			RowCheckCC(ccr[ri], bw.ptr<uchar>(ri), bw.cols, one, ri);
		}
	};

	static size_t CountCCR(const std::vector< std::vector< std::vector<cv::Point> > > &cc, size_t rend)
	{
		size_t ti=0;
		for(size_t i=0;i<rend;i++)
			for(size_t j=0;j<cc[i].size();j++)
				ti+=cc[i][j].size();
		std::cout<<"total till "<<rend<<"="<<ti<<'\n';
		return ti;
	};

	static size_t CountCC(const std::vector< std::vector<cv::Point> > &cc)
	{
		size_t ti=0;
		for(size_t i=0;i<cc.size();i++)			
			ti+=cc[i].size();
		std::cout<<"total="<<ti<<'\n';
		return ti;
	};

	static inline int CheckConnection4TwoRow(const std::vector<cv::Point> &ccr1, const std::vector<cv::Point> &ccr2)
	{
		//ccr1 at right side of ccr2
		if (ccr1.front().x > ccr2.back().x)
			return -1;
		//ccr1 at left side of ccr2
		if (ccr1.back().x < ccr2.front().x)
			return 1;
		//ccr1 intersect ccr2
		return 0;
	};

	static inline int CheckConnection8TwoRow(const std::vector<cv::Point> &ccr1, const std::vector<cv::Point> &ccr2)
	{
		if (ccr1.front().x > ccr2.back().x + 1)
			return -1;
		if (ccr1.back().x + 1 < ccr2.front().x)
			return 1;
		return 0;
	};

    static size_t Connect4ToAboveRow(const std::vector< std::vector<cv::Point> > &ccRow, const std::vector<int> &ccRowIdx, size_t startIdx,
		const std::vector<cv::Point> &cc, std::vector<int> &ccIdx)
	{
		for (size_t i = startIdx; i < ccRow.size(); i++)
		{
			int flg = CheckConnection4TwoRow(ccRow[i], cc);
			if (flg == 0)
			{
				if (ccIdx.empty())
					ccIdx.assign(1, ccRowIdx[i]);
				else
				{
					std::vector<int> ccIdxCopy(ccIdx.size() + 1);
					std::vector<int>::iterator it = std::set_union(ccIdx.begin(), ccIdx.end(), ccRowIdx.begin() + i, ccRowIdx.begin() + i + 1, ccIdxCopy.begin());
					ccIdx.assign(ccIdxCopy.begin(), it);
				}
			}
			if (flg == -1)
				return i;
		}
		return ccRow.size();
	};
	
    static void Connect4ToAboveRow(const std::vector< std::vector<cv::Point> > &ccRow, const std::vector<int> &ccRowIdx,
        const std::vector< std::vector<cv::Point> > &ccRow2, std::vector< std::vector<int> > &ccRowIdxs2, int &newccIdx)
	{
		size_t starti = 0;
		ccRowIdxs2.assign(ccRow2.size(), std::vector<int>());
		for (size_t i = 0; i < ccRow2.size(); i++)
		{
			size_t endi = Connect4ToAboveRow(ccRow, ccRowIdx, starti, ccRow2[i], ccRowIdxs2[i]);
			if (ccRowIdxs2[i].empty())
			{
				ccRowIdxs2[i].assign(1, newccIdx);
				newccIdx++;
			}
		}
	};

    static size_t Connect8ToAboveRow(const std::vector< std::vector<cv::Point> > &ccRow, const std::vector<int> &ccRowIdx, size_t startIdx,
		const std::vector<cv::Point> &cc, std::vector<int> &ccIdx)
	{
		for (size_t i = startIdx; i < ccRow.size(); i++)
		{
			int flg = CheckConnection8TwoRow(ccRow[i], cc);
			if (flg == 0)
			{
				if (ccIdx.empty())
					ccIdx.assign(1, ccRowIdx[i]);
				else
				{
					std::vector<int> ccIdxCopy(ccIdx.size() + 1);
					std::vector<int>::iterator it = std::set_union(ccIdx.begin(), ccIdx.end(), ccRowIdx.begin() + i, ccRowIdx.begin() + i + 1, ccIdxCopy.begin());
					ccIdx.assign(ccIdxCopy.begin(), it);
				}
			}
			if (flg == -1)
				return i;
		}
		return ccRow.size();
	};
	
    static void Connect8ToAboveRow(const std::vector< std::vector<cv::Point> > &ccRow, const std::vector<int> &ccRowIdx,
        const std::vector< std::vector<cv::Point> > &ccRow2, std::vector< std::vector<int> > &ccRowIdxs2, int &newccIdx)
	{
		size_t starti = 0;
		ccRowIdxs2.assign(ccRow2.size(), std::vector<int>());
		for (size_t i = 0; i < ccRow2.size(); i++)
		{
			size_t endi = Connect8ToAboveRow(ccRow, ccRowIdx, starti, ccRow2[i], ccRowIdxs2[i]);
			if (ccRowIdxs2[i].empty())
			{
				ccRowIdxs2[i].assign(1, newccIdx);
				newccIdx++;
			}
		}
	};
	
	static void EraseIndex(std::vector<int> &ccIdxs, int oldi, int newi)
	{
		std::vector<int>::iterator it = ccIdxs.begin();

		while (it != ccIdxs.end())
		{
			if (*it > oldi)
			{
				break;
			}
			if (*it == oldi)
			{
				if (it == ccIdxs.begin())
				{
					*it = newi;
					it++;
				}
				else
				{
					it = ccIdxs.erase(it);
				}
				break;
			}
			it++;
		}

		while (it != ccIdxs.end())
		{
			(*it) -= 1;
			it++;
		}
	}

    static void EraseIndex(std::vector< std::vector<int> > &ccRowIdxs, int oldi, int newi)
	{
        std::vector< std::vector<int> >::iterator it = ccRowIdxs.begin();
		while (it != ccRowIdxs.end())
		{
			EraseIndex(*it, oldi, newi);
			it++;
		}
	}

    static void MergeCC(std::vector< std::vector<cv::Point> > &cc, std::vector< std::vector<int> > &ccRowIdxs)
	{
		for (size_t i = 0; i < ccRowIdxs.size(); i++)
		{
			if (ccRowIdxs[i].size() > 1)
			{
				for (size_t j = 1; j < ccRowIdxs[i].size();)
				{
					cc[ccRowIdxs[i][0]].insert(cc[ccRowIdxs[i][0]].end(), cc[ccRowIdxs[i][j]].begin(), cc[ccRowIdxs[i][j]].end());
					cc.erase(cc.begin() + ccRowIdxs[i][j]);

					EraseIndex(ccRowIdxs, ccRowIdxs[i][j], ccRowIdxs[i][0]);
				}
			}
		}
	};

    static void MergeRowToCC(std::vector< std::vector<cv::Point> > &cc, const std::vector<int> &ccRowIdx, const std::vector< std::vector<cv::Point> > &ccRow)
	{
		size_t CCNum = *std::max_element(ccRowIdx.begin(), ccRowIdx.end())+1;
		
		if(CCNum>cc.size())
			cc.resize(CCNum);

		for (size_t i = 0; i < ccRow.size(); i++)
		{
			cc[ccRowIdx[i]].insert(cc[ccRowIdx[i]].end(), ccRow[i].begin(), ccRow[i].end());
		}
	};
	
	static void Connect4RowCC(const std::vector< std::vector< std::vector<cv::Point> > > &ccr, std::vector< std::vector<cv::Point> > &cc)
	{
		size_t i = 0;
		while (i < ccr.size())
		{
			if (!ccr[i].empty())
			{
				std::vector<int> ccridx(ccr[i].size(), 1);
				ccridx[0] = cc.size();
				std::partial_sum(ccridx.begin(), ccridx.end(), ccridx.begin());
				cc.insert(cc.end(), ccr[i].begin(), ccr[i].end());
				i++;
				while (i < ccr.size())
				{
					std::vector< std::vector<int> > ccridx2;
					int ns = cc.size();
					Connect4ToAboveRow(ccr[i - 1], ccridx, ccr[i], ccridx2, ns);					
					if (ccridx2.empty())
						break;
					MergeCC(cc, ccridx2);
					ccridx.resize(ccridx2.size());
					for (size_t j = 0; j < ccridx.size(); j++)
					{
						ccridx[j] = ccridx2[j][0];
					}
					MergeRowToCC(cc, ccridx, ccr[i]);
					i++;
				}
			}

			i++;
		}


	}
	
	static void Connect8RowCC(const std::vector< std::vector< std::vector<cv::Point> > > &ccr, std::vector< std::vector<cv::Point> > &cc)
	{
		size_t i = 0;
		while (i < ccr.size())
		{
			if (!ccr[i].empty())
			{
				std::vector<int> ccridx(ccr[i].size(), 1);
				ccridx[0] = cc.size();
				std::partial_sum(ccridx.begin(), ccridx.end(), ccridx.begin());
				cc.insert(cc.end(), ccr[i].begin(), ccr[i].end());
				i++;
				while (i < ccr.size())
				{
					std::vector< std::vector<int> > ccridx2;
					int ns = cc.size();
					Connect8ToAboveRow(ccr[i - 1], ccridx, ccr[i], ccridx2, ns);
					if (ccridx2.empty())
						break;
					MergeCC(cc, ccridx2);
					ccridx.resize(ccridx2.size());
					for (size_t j = 0; j < ccridx.size(); j++)
					{
						ccridx[j] = ccridx2[j][0];
					}
					MergeRowToCC(cc, ccridx, ccr[i]);
					i++;

					//if(CountCCR(ccr,i)!=CountCC(cc))
					//	std::cout<<"error\n";
				}
			}

			i++;
		}


	}

public:
    static void GetCC(const cv::Mat &bw, std::vector< std::vector<cv::Point> > &cc, uchar one = 255, bool b4=true)
	{
		std::vector< std::vector< std::vector<cv::Point> > > ccrs;
		GetRowCC(bw,ccrs,one);
        if(b4)
            Connect4RowCC(ccrs, cc);
        else
            Connect8RowCC(ccrs, cc);
    }



};

#endif

