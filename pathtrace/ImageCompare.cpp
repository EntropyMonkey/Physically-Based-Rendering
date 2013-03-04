#include <cmath>
#include <valarray>
#include "GL/glew.h"
#include "GL/glut.h"
#include "CGLA/Vec3f.h"
#include "ImageCompare.h"

using namespace std;
using namespace CGLA;

namespace
{
  Vec3f signed_gray(const Vec3f& v)
  {
    float mean = (v[0] + v[1] + v[2])/3.0f;
    return Vec3f(mean < 0.0f ? -mean : 0.0f, mean > 0.0f ? mean : 0.0f, 0.0f);
  }
}

ImageCompare::ImageCompare(const Image& img)
  : ref_img(img), offset(0), compare_w(img.width), compare_h(img.height), subsize(11)
{ 
  gauss_filter.width = subsize;
  gauss_filter.height = subsize;
  gauss_filter.data.resize(gauss_filter.width*gauss_filter.height);
  for(unsigned int j = 0; j < gauss_filter.height; ++j)
    for(unsigned int i = 0; i < gauss_filter.width; ++i)
    {
      double x = i - 5.0;
      double y = j - 5.0;
      gauss_filter.data[i + gauss_filter.width*j] = Vec3f(exp(-(x*x + y*y)/4.5));
    }
  gauss_filter.data /= gauss_filter.data.sum();
}

void ImageCompare::set_window(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
  offset = x + ref_img.width*y;
  if(offset + width + ref_img.width*height > ref_img.data.size())
  {
    offset = 0;
    return;
  }
  compare_w = width;
  compare_h = height;
}

void ImageCompare::draw_difference(const Image& img)
{
  if(ref_img.data.size() != img.data.size() || ref_img.width != img.width || ref_img.height != img.height)
    return;

  size_t len[] = { compare_h, compare_w };
  size_t str[] = { img.width, 1 };
  gslice s(offset, valarray<size_t>(len, 2), valarray<size_t>(str, 2));
  valarray<Vec3f> ref_slice = ref_img.data[s];
  valarray<Vec3f> img_slice = img.data[s];
  valarray<Vec3f> diff_img = img_slice - ref_slice;
  diff_img = diff_img.apply(signed_gray);
  glDrawPixels(compare_w, compare_h, GL_RGB, GL_FLOAT, diff_img[0].get());
}

Vec3f ImageCompare::get_color_mse(const Image& img)
{
  if(ref_img.data.size() != img.data.size() || ref_img.width != img.width || ref_img.height != img.height)
    return Vec3f(0.0f);

  size_t len[] = { compare_h, compare_w };
  size_t str[] = { img.width, 1 };
  gslice s(offset, valarray<size_t>(len, 2), valarray<size_t>(str, 2));
  valarray<Vec3f> ref_slice = ref_img.data[s];
  valarray<Vec3f> img_slice = img.data[s];
  valarray<Vec3f> error_img = img_slice - ref_slice;
  return (error_img*error_img).sum()/static_cast<float>(img_slice.size());
}

Vec3f ImageCompare::get_color_ssim(const Image& img, bool apply_filter)
{
  if(ref_img.data.size() != img.data.size() || ref_img.width != img.width || ref_img.height != img.height)
    return Vec3f(0.0f);
  if(apply_filter && compare_h*compare_w != subsize*subsize)
    return Vec3f(0.0f);

  size_t len[] = { compare_h, compare_w };
  size_t str[] = { img.width, 1 };
  gslice s(offset, valarray<size_t>(len, 2), valarray<size_t>(str, 2));
  valarray<Vec3f> slice_x = ref_img.data[s];
  valarray<Vec3f> slice_y = img.data[s];
  float size = slice_x.size();
  Vec3f mean_x, mean_y;
  if(apply_filter)
  {
    mean_x = (slice_x*gauss_filter.data).sum();
    mean_y = (slice_y*gauss_filter.data).sum();
  }
  else
  {
    mean_x = slice_x.sum()/size;
    mean_y = slice_y.sum()/size;
  }
  valarray<Vec3f> dev_x = slice_x - valarray<Vec3f>(mean_x, slice_x.size());
  valarray<Vec3f> dev_y = slice_y - valarray<Vec3f>(mean_y, slice_y.size());
  Vec3f variance_x, variance_y, covariance;
  if(apply_filter)
  {
    variance_x = (dev_x*dev_x*gauss_filter.data).sum();
    variance_y = (dev_y*dev_y*gauss_filter.data).sum();
    covariance = (dev_x*dev_y*gauss_filter.data).sum();
  }
  else
  {
    variance_x = (dev_x*dev_x).sum()/(size - 1.0f);
    variance_y = (dev_y*dev_y).sum()/(size - 1.0f);
    covariance = (dev_x*dev_y).sum()/(size - 1.0f);
  }
  Vec3f c1(1.0e-4f); // K1 = 0.01, L = 1, c1 = (K1*L)^2
  Vec3f c2(9.0e-4f); // K2 = 0.03, L = 1, c2 = (K2*L)^2
  return (2.0f*mean_x*mean_y + c1)*(2.0f*covariance + c2)
    /((mean_x*mean_x + mean_y*mean_y + c1)*(variance_x + variance_y + c2));
}

float ImageCompare::get_mse(const Image& img)
{
  Vec3f c = get_color_mse(img);
  return (c[0] + c[1] + c[2])/3.0f;
}

float ImageCompare::get_ssim(const Image& img)
{
  Vec3f c(0.0f);
  float counter = 0.0f;
  unsigned int cw = compare_w;
  unsigned int ch = compare_h;
  unsigned int x = offset%ref_img.width;
  unsigned int y = offset/ref_img.width;
  for(unsigned int j = 0; j < ch - subsize; ++j)
    for(unsigned int i = 0; i < cw - subsize; ++i)
    {
      set_window(x + i, y + j, subsize, subsize);
      c += get_color_ssim(img, true);
      ++counter;
    }
  set_window(x, y, cw, ch);
  return (c[0] + c[1] + c[2])/(3.0f*counter);
}
