// camport_test.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "percipio_camport.h"
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include "depth_render.h"

static cv::Mat left, right, depth, point_cloud;
static DepthRender render;
static int fps_counter = 0;
static clock_t fps_tm = 0;


void process_frames(percipio::DepthCameraDevice &port);
void save_frame_to_file();
int get_fps();
void CopyBuffer(percipio::ImageBuffer *pbuf, cv::Mat &img);

void frame_arrived_callback(void *user_data) {
  // call port.FramePackageGet to update internal buffer
  // call port.FrameGet to get frame data here
  // To avoid performance problem ,time consuming task in callback function is not recommended.
}


int main(int argc, char** argv) {
  percipio::DepthCameraDevice port(percipio::MODEL_DPB04GN);
  render.range_mode = DepthRender::COLOR_RANGE_DYNAMIC;
  render.color_type = DepthRender::COLORTYPE_BLUERED;
  render.invalid_label = 0;
  render.Init();
  percipio::SetLogLevel(percipio::LOG_LEVEL_INFO);
  port.SetCallbackUserData(NULL);
  port.SetFrameReadyCallback(frame_arrived_callback);

  int ver = percipio::LibVersion();
  printf("Sdk version is %d\n", ver);

  int ret = port.OpenDevice();
  if (percipio::CAMSTATUS_SUCCESS != ret) {
    printf("open device failed\n");
    return -1;
  }

  int wait_time;
  port.GetProperty(percipio::PROP_WAIT_NEXTFRAME_TIMEOUT, (char *)&wait_time, sizeof(int));
  printf("get property PROP_WAIT_NEXTFRAME_TIMEOUT %d\n", wait_time);

  int reti = port.SetProperty_Int(percipio::PROP_WORKMODE, percipio::WORKMODE_DEPTH);
  //int reti = port.SetProperty_Int(percipio::PROP_WORKMODE, percipio::WORKMODE_IR);
  //int reti = port.SetProperty_Int(percipio::PROP_WORKMODE, percipio::WORKMODE_IR_DEPTH);
  if (reti < 0) {
    printf("set mode failed,error code:%d\n", reti);
    return -1;
  }

  //display a empty window for receiving key input
  cv::imshow("left", cv::Mat::zeros(100, 100, CV_8UC1));
  fps_tm = clock();
  fps_counter = 0;
  while (true) {
    if (port.FramePackageGet() == percipio::CAMSTATUS_SUCCESS) {
      process_frames(port);
    }
    int k = cv::waitKey(1);
    if (k == 'q' || k == 1048689) {
      break;
    }
    if (k == 's' || k == 1048691) {
      save_frame_to_file();
    }
  }//while
  port.CloseDevice();
  left.release();
  right.release();
  depth.release();
  point_cloud.release();
  render.Uninit();
  return 0;
}

void process_frames(percipio::DepthCameraDevice &port) {
  percipio::ImageBuffer pimage;
  int ret = port.FrameGet(percipio::CAMDATA_LEFT, &pimage);
  if (percipio::CAMSTATUS_SUCCESS == ret) {
    CopyBuffer(&pimage, left);
    cv::imshow("left", left);
  }
  ret = port.FrameGet(percipio::CAMDATA_RIGHT, &pimage);
  if (percipio::CAMSTATUS_SUCCESS == ret) {
    CopyBuffer(&pimage, right);
    cv::imshow("right", right);
  }
  ret = port.FrameGet(percipio::CAMDATA_DEPTH, &pimage);
  if (percipio::CAMSTATUS_SUCCESS == ret) {
    CopyBuffer(&pimage, depth);
    cv::Mat t;
    int fps = get_fps();
    render.Compute(depth, t);
    cv::imshow("depth", t);
    if (fps > 0) {
      unsigned short v = depth.ptr<unsigned short>(depth.rows / 2)[depth.cols / 2];
      printf("fps:%d distance: %d\n", (int)fps, v);
    }
  }
  ret = port.FrameGet(percipio::CAMDATA_POINT3D, &pimage);
  if (percipio::CAMSTATUS_SUCCESS == ret) {
    CopyBuffer(&pimage, point_cloud);
  }
}
#ifdef _WIN32
int get_fps() {
  const int kMaxCounter = 20;
  fps_counter++;
  if (fps_counter < kMaxCounter) {
    return -1;
  }
  int elapse = (clock() - fps_tm);
  int v = (int)(((float)fps_counter) / elapse * CLOCKS_PER_SEC);
  fps_tm = clock();

  fps_counter = 0;
  return v;
}
#else
int get_fps() {
  const int kMaxCounter = 20;
  struct timeval start;
  fps_counter++;
  if (fps_counter < kMaxCounter) {
    return -1;
  }

  gettimeofday(&start, NULL);
  int elapse = start.tv_sec * 1000 + start.tv_usec / 1000 - fps_tm;
  int v = (int)(((float)fps_counter) / elapse * 1000);
  gettimeofday(&start, NULL);
  fps_tm = start.tv_sec * 1000 + start.tv_usec / 1000;

  fps_counter = 0;
  return v;
}
#endif

void save_frame_to_file() {
  static int idx = 0;
  char buff[100];
  if (!left.empty()) {
    sprintf(buff, "%d-left.png", idx);
    cv::imwrite(buff, left);
  }
  if (!right.empty()) {
    sprintf(buff, "%d-right.png", idx);
    cv::imwrite(buff, right);
  }
  if (!depth.empty()) {
    sprintf(buff, "%d-depth.txt", idx);
    std::ofstream ofs(buff);
    ofs << depth;
    ofs.close();
  }
  if (!point_cloud.empty()) {
    sprintf(buff, "%d-points.txt", idx);
    std::ofstream ofs(buff);
    percipio::Vect3f *ptr = point_cloud.ptr<percipio::Vect3f>();
    for (int i = 0; i < point_cloud.size().area(); i++) {
      ofs << ptr->x << "," << ptr->y << "," << ptr->z << std::endl;
      ptr++;
    }
    ofs.close();
  }
  idx++;
}

void CopyBuffer(percipio::ImageBuffer *pbuf, cv::Mat &img) {
  switch (pbuf->type) {
  case percipio::ImageBuffer::PIX_8C1:
    img.create(pbuf->height, pbuf->width, CV_8UC1);
    break;
  case percipio::ImageBuffer::PIX_16C1:
    img.create(pbuf->height, pbuf->width, CV_16UC1);
    break;
  case percipio::ImageBuffer::PIX_8C3:
    img.create(pbuf->height, pbuf->width, CV_8UC3);
    break;
  case percipio::ImageBuffer::PIX_32FC3:
    img.create(pbuf->height, pbuf->width, CV_32FC3);
    break;
  default:
    img.release();

    return;
  }
  memcpy(img.data, pbuf->data, pbuf->width * pbuf->height * pbuf->get_pixel_size());
}
