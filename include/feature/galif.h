#ifndef GALIF_H
#define GALIF_H

#include "feature.h"
#include "detector.h"
#include "config.h"

namespace sbe {

class Galif : public Feature
{
public:
    Galif(const FeatureConfig& config);
    Galif(const PropertyTree_t &parameters);
    Galif(uint width, uint numOrients, uint tiles,
          double peakFrequency, double lineWidth, double lambda,
          double featureSize, bool isSmoothHist,
          const std::string& normalizeHist,
          const std::string& detectorName,
          uint numOfSamples);
    void compute(const cv::Mat &image, KeyPoints_t &keypoints, Features_t &features) const;
private:
    double scale(const cv::Mat &image, cv::Mat &scaled) const;
    void detect(const cv::Mat &image, KeyPoints_t &keypoints) const;
    void extract(const cv::Mat &image, const KeyPoints_t &keypoints, Features_t &features, Vec_Index_t &emptyFeatures) const;
    void assertImageSize(const cv::Mat &image) const;

    const uint _width;
    const uint _numOrients;
    const uint _tiles;
    const double _peakFrequency;
    const double _lineWidth;
    const double _lambda;
    const double _featureSize;
    const bool _isSmoothHist;
    const std::string _normalizeHist;
    const std::string _detectorName;

    cv::Size _filterSize;
    std::vector<cv::Mat_<std::complex<double> > > _gaborFilter;
    boost::shared_ptr<Detector> _detector;
public: 
    void OutputFilters();
};

} //namespace sbe

#endif // GALIF_H
