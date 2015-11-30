#include "detector.h"
#include <glog/logging.h>

namespace sbe {

GridDetector::GridDetector(uint numSamples) :
    _numSamples(numSamples){}

/**
 * @brief GridDetector::detect
 * Keypoints are cross points, when we divide image to square grid.
 * _numSamples, default value is 625
 */
void GridDetector::detect(const cv::Mat &image, KeyPoints_t &keypoints) const
{
    //采样区域
    cv::Rect samplingArea(0, 0, image.size().width, image.size().height);
    //把采样数开平方,下取整,求出每一行/列多少个
    uint numSample1D = std::ceil(std::sqrt(static_cast<float>(_numSamples)));
    //DLOG(INFO) << "numSample1D : " << numSample1D << " \n";
    //　求出每个cell的尺寸　stepX,stepY
    float stepX = samplingArea.width / static_cast<float>(numSample1D+1);
    float stepY = samplingArea.height / static_cast<float>(numSample1D+1);
    //DLOG(INFO) << "stepX , stepY " << stepX  << " " << stepY << " \n";

    for(uint x = 1; x <= numSample1D; x++) {
        //找到x的坐标
        uint posX = x*stepX;
        for(uint y = 1; y <= numSample1D; y++) {
            //找到y的坐标
            uint posY = y*stepY;
            //这里用pair或者point结构比vector好，因为vector浪费空间
            Vec_f32_t p(2);
            p[0] = posX;
            p[1] = posY;
            //把当前的关键点存起来
            keypoints.push_back(p);
        }
    }
}
// 上面整个算法就是对当前图像均匀求采样点

} //namespace sse
