#ifndef IMAGECOMPARE_H
#define IMAGECOMPARE_H

#include <valarray>
#include "CGLA/Vec3f.h"

struct Image
{
  Image() : width(0), height(0) { }

  Image(const CGLA::Vec3f* img_data, unsigned int img_width, unsigned int img_height)
  {
    width = img_width;
    height = img_height;
    data.resize(width*height);
    for(unsigned int j = 0; j < height; ++j)
      for(unsigned int i = 0; i < width; ++i)
        data[i + width*j] = img_data[i + width*j];
  }

  std::valarray<CGLA::Vec3f> data;
  unsigned int width;
  unsigned int height;
};

class ImageCompare
{
public:
  ImageCompare(const Image& img);

  void set_window(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
  void draw_difference(const Image& img);
  CGLA::Vec3f get_color_mse(const Image& img);
  CGLA::Vec3f get_color_ssim(const Image& img, bool apply_filter = false);
  float get_mse(const Image& img);
  float get_ssim(const Image& img);

private:
  Image ref_img;
  Image gauss_filter;
  unsigned int subsize;
  unsigned int offset;
  unsigned int compare_w, compare_h;
};

#endif // IMAGECOMPARE_H
