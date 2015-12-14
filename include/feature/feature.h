#ifndef FEATURE_H
#define FEATURE_H

#include "typedefs.h"

namespace sbe {

/**
 * @brief The Feature class
 * The base class of all features
 */
class Feature
{
public:
    virtual void compute(const cv::Mat &image,
                         sbe::KeyPoints_t &keypoints,
                        sbe::Features_t &features) const = 0;
    /**
     * @brief scale: scale image as a suitable size
     *
     * @param image
     * @param scaled
     */
    virtual double scale(const cv::Mat &image, cv::Mat &scaled) const = 0;
    /**
     * @brief detect keypoints
     *
     * @param image
     * @param keypoints
     */
    virtual void detect(const cv::Mat &image, sbe::KeyPoints_t &keypoints) const = 0;
    /**
     * @brief extract features from image
     *
     * @param image
     * @param keypoints
     * @param features
     * @param emptyFeatures
     */
    virtual void extract(const cv::Mat &image, const sbe::KeyPoints_t &keypoints,
                         sbe::Features_t &features, sbe::Vec_Index_t &emptyFeatures) const = 0;
};

} //namspace sse

#endif // FEATURE_H
