#ifndef DEPTH_RENDER_H_
#define DEPTH_RENDER_H_

#include <opencv2/core/core.hpp>
#include <vector>

class DepthRender {
 public:
  DepthRender() {
    min_distance = 200;
    Init();
  }
  enum OutputColorType {
    COLORTYPE_RAINBOW = 0,
    COLORTYPE_BLUERED = 1,
    COLORTYPE_GRAY = 2
  };

  enum ColorRangeMode {
    COLOR_RANGE_ABS = 0,
    COLOR_RANGE_DYNAMIC = 1
  };

  //input 16UC1 output 8UC3
  void Compute(cv::Mat &src, cv::Mat &dst);
  void Init();
  void Uninit(){
    clr_disp.release();
    filtered_mask.release();
  }

  OutputColorType color_type;
  ColorRangeMode range_mode;
  //for abs mode
  int min_distance, max_distance;
  unsigned short invalid_label;

 private:

  void CalcColorMap(const cv::Mat &src, cv::Mat &dst) ;
  void BuildColorTable() ;
  //keep value in range
  void TruncValue(cv::Mat &img, cv::Mat &mask, short min_val, short max_val);
  void ClearInvalidArea(cv::Mat &clr_disp, cv::Mat &filtered_mask) ;

  std::vector<cv::Scalar> _color_lookup_table;

  cv::Mat clr_disp ;
  cv::Mat filtered_mask;
};


#endif
