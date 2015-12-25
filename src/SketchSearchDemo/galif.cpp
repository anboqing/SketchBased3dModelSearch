
#include "galif.h"
#include <glog/logging.h>


namespace sbe {


// Normalizes keypoint coordinates into range [0, 1] x [0, 1] to have them stored independently of image size
void normalizeKeypoints(const KeyPoints_t &keypoints, const cv::Size &imageSize, KeyPoints_t &keypointsNormalized)
{
    Vec_f32_t p(2);
    for (size_t i = 0; i < keypoints.size(); i++) {
        p[0] = keypoints[i][0] / imageSize.width;
        p[1] = keypoints[i][1] / imageSize.height;
        keypointsNormalized.push_back(p);
    }
}

// Removes all empty features, i.e. those that only contains zeros
void filterEmptyFeatures(const Features_t &features,
        const KeyPoints_t &keypoints,
        const vector<Index_t> &emptyFeatures,
        Features_t &featuresFiltered,
        KeyPoints_t &keypointsFiltered)
{
    CHECK(features.size() == keypoints.size())<< " features.size() != keypoints.size";
    CHECK(features.size() == emptyFeatures.size()) << " features.size() != keypoints.size";

    for (size_t i = 0; i < emptyFeatures.size(); i++) {

        if (!emptyFeatures[i]) {
            featuresFiltered.push_back(features[i]);
            keypointsFiltered.push_back(keypoints[i]);
        }
    }
}

template <class T>
void generate_gabor_filter(cv::Mat_<T> &image,
                           double peakFrequency,
                           double theta,
                           double sigmaX,
                           double sigmaY)
{
    const uint w = image.size().width;
    const uint h = image.size().height;
    const double step_u = 1.0 / static_cast<double>(w);
    const double step_v = 1.0 / static_cast<double>(h);
    const double cos_theta = std::cos(theta);
    const double sin_theta = std::sin(theta);

    const double sigmaXSquared = sigmaX*sigmaX;
    const double sigmaYSquared = sigmaY*sigmaY;

    image.setTo(cv::Scalar(0));

    /*
     * g(u,v) = exp(-2PI^2((u_rotate-omega_0)^2*sigma_x^2+v_rotate^2*sigma_y^2
     *
     * @param omega_0 : peak response frequency
     * @param theata : filter orientation
     * @param sigma_x : frequency bandwidth
     * @param sigma_y : angular bandwidth
     * @ <u_rotate,v_rotate> = R_theta(u,v)^T : 
     * 标准坐标系统旋转角度 theta,
     * 把笛卡尔坐标的列向量关于原点顺时针旋转theta的矩阵是:
     * | cos(theta) sin(theta) |
     * | -sin(theta) cos(theta)|
     * 
     */
    for(int ny = -1; ny <= 1; ny++) {
        for(int nx = -1; nx <=1; nx++) {
            double v = ny;
            for(uint y = 0; y < h; y++) {
                double u = nx;
                for(uint x = 0; x < w; x++) {
                    double ur = u*cos_theta - v*sin_theta;
                    double vr = u*sin_theta + v*cos_theta;

                    double temp = ur - peakFrequency;
                    double value = std::exp(-2*M_PI*M_PI*(temp*temp*sigmaXSquared + vr*vr*sigmaYSquared));
                    image(y, x) += value;

                    u += step_u;
                }
                v += step_v;
            }
        }
    }
}


// 带参数构造函数
Galif::Galif(
        uint width, //sketch的边长 
        uint numOrients,//gabor过滤器的方向数 k
        uint tiles,//每个local image分成多少个小 tile
        double peakFrequency, //最大峰值响应频率 omega_0 : 0.13
        double lineWidth,//线宽 : 0.02
        double lambda,//　lambda = sigmaX/sigmaY :论文中最佳的是　lambda = 0.3
        double featureSize, // ???
        bool isSmoothHist,// ???
        const std::string &normalizeHist, // ???
        const std::string &detectorName, // 关键点检测器名称 
        uint numOfSamples //每张sketch要采样多少个
        )
    : _width(width),
    _numOrients(numOrients),
    _tiles(tiles)
    , _peakFrequency(peakFrequency),
    _lineWidth(lineWidth), _lambda(lambda)
    , _featureSize(featureSize), //这个应该没有用
    _isSmoothHist(isSmoothHist)
    , _normalizeHist(normalizeHist),
    _detectorName(detectorName)
{
    //创建一个网格关键点检测器:TODO改用c++11　make_shared
    _detector = boost::make_shared<GridDetector>(numOfSamples);

    //sigmaX是　frequency bandwidth 而　lineWidth = sigmaX/(边长 of a sketch)
    double sigmaX = _lineWidth * _width;
    double sigmaY = _lambda * sigmaX;

    // pad the image by 3*sigma_max, this avoids any boundary effects
    // afterwards increase size to something that fft is working efficiently on
    //  获取最优离散傅里叶变换尺寸,在边框外补空白
    int paddedSize = cv::getOptimalDFTSize(_width + 3*std::max(sigmaX, sigmaY));
    //std::cout << "galo padded size: " << paddedSize << std::endl;
    // 获取过滤器尺寸
    _filterSize = cv::Size(paddedSize, paddedSize);

    // 用多个方向生成　filterbank,生成的这个可以保存起来
    for(uint i = 0; i < _numOrients; i++) {
        cv::Mat_<std::complex<double> > filter(_filterSize);
        //按照各个角度变化（其他参数不变）生成各个角度的过滤器
        double theta = i * M_PI / _numOrients;
        generate_gabor_filter(filter, _peakFrequency, theta, sigmaX, sigmaY);
        filter(0, 0) = 0;
        _gaborFilter.push_back(filter);
    }
}

Galif::Galif(const PropertyTree_t &parameters)
    : _width(parameters.get<uint>("feature.sketchwidth", 256))
    , _numOrients(parameters.get<uint>("feature.orientation_num", 4))
    , _tiles(parameters.get<uint>("feature.tile_num", 4))
    , _peakFrequency(parameters.get<double>("feature.peak_frequency", 0.1)) // 论文中是0.13
    , _lineWidth(parameters.get<double>("feature.line_width", 0.02))
    , _lambda(parameters.get<double>("feature.lambda", 0.3))
    , _featureSize(parameters.get<double>("feature.feature_size", 0.1))
    , _isSmoothHist(parameters.get<bool>("feature.is_smooth_hist", true))
    , _normalizeHist(parameters.get<std::string>("feature.is_normalize_hist", "l2"))
    , _detectorName(parameters.get<std::string>("feature.detector.name", "grid"))
{
    uint numOfSamples = parameters.get<uint>("feature.detector.num_of_samples", 625);
    _detector = boost::make_shared<GridDetector>(numOfSamples);

    double sigmaX = _lineWidth * _width;
    double sigmaY = _lambda * sigmaX;

    // pad the image by 3*sigma_max, this avoids any boundary effects
    // afterwards increase size to something that fft is working efficiently on
    int paddedSize = cv::getOptimalDFTSize(_width + 3*std::max(sigmaX, sigmaY));
    //std::cout << "galo padded size: " << paddedSize << std::endl;

    _filterSize = cv::Size(paddedSize, paddedSize);

    for(uint i = 0; i < _numOrients; i++) {
        cv::Mat_<std::complex<double> > filter(_filterSize);
        double theta = i * M_PI / _numOrients;

        generate_gabor_filter(filter, _peakFrequency, theta, sigmaX, sigmaY);

        filter(0, 0) = 0;
        _gaborFilter.push_back(filter);
    }
}

Galif::Galif(const FeatureConfig& config):
_width(config._width),
    _numOrients(config._orientation_n),
    _tiles(config._tile_n),
   _peakFrequency ( config._peakFrequency),
    _lineWidth ( config._line_width),
    _lambda ( config._lambda),
    _featureSize ( config._feature_size),
    _isSmoothHist ( config._is_smooth_hist),
    _normalizeHist ( config._normalize_type),
    _detectorName (config._detector_name)
{
    
    uint numOfSamples = config._num_of_samples;
    
    _detector = boost::make_shared<GridDetector>(numOfSamples);

    double sigmaX = _lineWidth * _width;
    double sigmaY = _lambda * sigmaX;

    // pad the image by 3*sigma_max, this avoids any boundary effects
    // afterwards increase size to something that fft is working efficiently on
    int paddedSize = cv::getOptimalDFTSize(_width + 3*std::max(sigmaX, sigmaY));
    //std::cout << "galo padded size: " << paddedSize << std::endl;

    _filterSize = cv::Size(paddedSize, paddedSize);

    for(uint i = 0; i < _numOrients; i++) {
        cv::Mat_<std::complex<double> > filter(_filterSize);
        double theta = i * M_PI / _numOrients;

        generate_gabor_filter(filter, _peakFrequency, theta, sigmaX, sigmaY);

        filter(0, 0) = 0;
        _gaborFilter.push_back(filter);
    }
}

void Galif::OutputFilters(){
    //output the filters
    for(uint i = 0; i < _numOrients; i++) {
        char filename[64];
        sprintf(filename, "filter_%d.png", i);
        const cv::Mat_<std::complex<double> >& filter = _gaborFilter[i];

        //compute magnitude of response
        cv::Mat mag(filter.size(), CV_32FC1);
        cv::Mat mag_c3(filter.size(), CV_32FC3);
        for(int r = 0; r < mag.rows; r++) {
            for(int c = 0; c < mag.cols; c++) {
                const std::complex<double>& v = filter(r, c);
                //计算幅值
                float m = std::sqrt(v.real() * v.real() + v.imag() * v.imag());
                mag.at<float>(r, c) = m * 255;

                cv::Vec3f &bgr = mag_c3.at<cv::Vec3f>(r,c);
                bgr.val[0] = 255-m*255;
                bgr.val[1] = 255;
                bgr.val[2] = 255-m*255;
//                bgr.val[0] = m*255;
//                bgr.val[1] = 255;
//                bgr.val[2] = m*255;
            }
        }
        cv::imwrite(filename, mag);

        char filename_color[64];
        sprintf(filename_color, "filter_%d_color.png", i);
        cv::imwrite(filename_color, mag_c3);
    }
}

/**
 * @brief Galif::compute
 * @param image : input image (3-channel image, make sure  image.type() == CV_8UC3)
 * @param keypoints : output, has been normalized in range [0,1]x[0,1], so tgat they are independent of image size
 * @param features : output, Galif features
 */
void Galif::compute(const cv::Mat &image, KeyPoints_t &keypoints, Features_t &features) const
{
    // --------------------------------------------------------------
    // prerequisites:
    //
    // this generator expects a 3-channel image, with
    // each channel containing exactly the same pixel values
    //
    // the image must have a white background with black sketch lines
    // --------------------------------------------------------------
    CHECK(image.type() == CV_8UC3)<< "image.type != CV_8UC3";

    //把输入图片转换成灰度图像
    cv::Mat gray;
    cv::cvtColor(image, gray, CV_RGB2GRAY);

    CHECK(gray.type() == CV_8UC1) << " gray.type !=CV_8UC1";

    // scale image to desired size
    cv::Mat scaled;
    //缩放到合适尺寸
    scale(gray, scaled);

    // detect keypoints on the scaled image
    // the keypoint cooredinates lie in the domain defined by
    // the scaled image size, i.e. if the image has been scaled
    // to 256x256, keypoint coordinates lie in [0,255]x[0,255]
    KeyPoints_t _keypoints;
    //检测特征点
    detect(scaled, _keypoints);

    //extract local features at the given keypoints
    Features_t _features;
    std::vector<Index_t> emptyFeatures;
    //抽取特征
    extract(scaled, _keypoints, _features, emptyFeatures);

    CHECK(_features.size() == _keypoints.size()) << "_features.size() != keypoints.size() " ;
    CHECK(emptyFeatures.size() == _keypoints.size()) << " emptyFeatures.size() != _keypoints.size() " ;

    // normalize keypoints to range [0,1]x[0,1] so they are
    // independent of image size
    KeyPoints_t keypointsNormalized;
    //归一化特征点
    normalizeKeypoints(_keypoints, scaled.size(), keypointsNormalized);

    //过滤掉空特征
    filterEmptyFeatures(_features, keypointsNormalized, emptyFeatures, features, keypoints);
    CHECK(features.size() == keypoints.size()) << " features.size() != keypoints .size() " ;

    // features.size()==0说明这整张草图都是空的，就加一个空特征进去
    if(features.size() == 0) {
        Vec_f32_t histogram(_tiles * _tiles * _numOrients, 0.0f);
        Vec_f32_t zero(2, 0.0f);
        features.push_back(histogram);
        keypoints.push_back(zero);
    }
}

double Galif::scale(const cv::Mat &image, cv::Mat &scaled) const
{
    //uniformly scale the image such that it has no side that is larger than the filter's size
    //Note: _width actually means the maximum desired image side length
    // _width 是图像的边长，是矩形最长的边
    //计算出缩放比例
    double scaling_factor = (image.size().width > image.size().height)
            ? static_cast<double>(_width) / image.size().width
            : static_cast<double>(_width) / image.size().height;
    /*
     * void resize(InputArray src,OutputArray dst,Size dsize,double fx=0,double fy = 0,int interpolation=INTER_LINEAR)
     * @param dsize : 输出图像的大小，如果等于０，由　fx,fy计算出来
     * @param fx,fy:　x,y轴的缩放系数
     * @param INTER_AREA　区域差值
     */
    cv::resize(image, scaled, cv::Size(0, 0), scaling_factor, scaling_factor, cv::INTER_AREA);
    return scaling_factor;
}

void Galif::detect(const cv::Mat &image, KeyPoints_t &keypoints) const
{
    CHECK(image.type() == CV_8UC1) << " image.type != CV_8UC1 ";
    assertImageSize(image);
    //调用　grid　detector的detect方法,其实就是均匀的从图像里计算了一个网格，把每个网格顶点坐标保存起来了
    _detector->detect(image, keypoints);
}

/*
 * @param image :<In> | target image
 * @param emptyFeatures：<Out>| empty features
 * @param keyPoints: <In> ,the keypoints that detected by detect()
 * @param features: <Out>,the galif features of image
 */
void Galif::extract(const cv::Mat &image, const KeyPoints_t &keypoints, Features_t &features, Vec_Index_t &emptyFeatures) const
{
    CHECK(image.type() == CV_8UC1) << "image type not equal cv_8Uc1";
    assertImageSize(image);

    // copy input image centered onto a white background image with
    // exactly the size of our gabor filters
    //　把输入图像拷贝到一个和过滤器大小相等的白背景图片src上
    // WARNING: white background assumed!!!
    cv::Mat_<std::complex<double> > src(_filterSize, 1.0);
    cv::Mat_<unsigned char> inverted = cv::Mat_<unsigned char>::zeros(_filterSize);
    for (int r = 0; r < image.rows; r++) {
        for (int c = 0; c < image.cols; c++)
        {
            // 把实部归一化到[0,1],虚部置０
            // (complex<T>& operator=( const T& x )-> Assigns x to the real part of the complex number. Imaginary part is set to zero.
            src(r, c) = static_cast<double>(image.at<unsigned char>(r, c)) * (1.0/255.0);
            //把像素的灰度值翻转
            inverted(r, c) = 255 - image.at<unsigned char>(r, c);
        }
    }

    cv::Mat_<int> integral;
    //integral是求和的，求出来的和用于计算
    /*
     *Using these integral images, you can calculate sum, mean,
     *and standard deviation over a specific up-right or rotated
     *rectangular region of the image in a constant time
     */
    cv::integral(inverted, integral, CV_32S);

    // just a sanity check that the complex part
    // is correctly default initialized to 0
    //检查虚部为０ std::complex::imag()是访问虚部的 real()是访问实部的
    CHECK(src(0,0).imag() == 0) << " imaginary part of src is not zero " ;

    // ################################################################　把图像和Gabor滤波器卷积 ######################################
    // filter scaled input image by directional filter bank
    // transform source to frequency domain
    // 用傅里叶变换变到频域,因为滤波器是定义在频域的
    cv::Mat_<std::complex<double> > src_ft(_filterSize);
    cv::dft(src, src_ft);

    // apply each filter
    std::vector<cv::Mat> responses; // －－－－－－－－－－－－－－－responses里面存档的是当前图像对每个方向滤波器的响应幅值
    for (uint i = 0; i < _numOrients; i++) {
        // convolve in frequency domain (i.e. multiply spectrums)
        cv::Mat_<std::complex<double> > dst_ft(_filterSize);

        // it remains unclear what the 4th parameter stands for
        // 将变换到频域的src_ft和滤波器i　在element-wise相乘(卷积)
        cv::mulSpectrums(src_ft, _gaborFilter[i], dst_ft, 0);

        // transform back to spatial domain
        // 再用傅里叶逆变换变回空间域
        cv::Mat_<std::complex<double> > dst;
        cv::dft(dst_ft, dst, cv::DFT_INVERSE | cv::DFT_SCALE);

        // 计算傅里叶逆变换结果dst的幅值
        // 或许直接可以调用　cv magnitude(InputArray x,InputArray y,OutputArray magnitude)
        cv::Mat mag(image.size(), CV_32FC1);
        for (int r = 0; r < mag.rows; r++) {
            for (int c = 0; c < mag.cols; c++) {
                const std::complex<double>& v = dst(r, c);
                float m = std::sqrt(v.real() * v.real() + v.imag() * v.imag());
                mag.at<float>(r, c) = m;
            }
        }
#ifdef __DEBUG__
        char filename[64];
        sprintf(filename, "reponse_%d.png", i);
        cv::imwrite(filename, mag*255);
#endif //__DEBUG__
        responses.push_back(mag);
    } 
    //################################################################## 每个方向都卷积完毕 #########################################
    


    //##################################################################
    //
    //　　开始提取局部特征:见论文5.3
    //
    //##################################################################

    // _featureSize设定为图像尺寸的x%,area()是image的面积
    int featureSize = std::sqrt(image.size().area() * _featureSize);

    // 如果算出来的特征的大小不能整分为tiles,就把featureSize补全
    if (featureSize % _tiles)
    {
        featureSize += _tiles - (featureSize % _tiles);
    }

    //“瓷片"大小
    int tileSize = featureSize / _tiles;
    float halfTileSize = (float) tileSize / 2;
    // ##################################################################　给 response　加上边框，并且用　高斯核或者方框核 对响应幅值　进行滤波
    for (uint i = 0; i < _numOrients; i++) {
        //给图像加一圈（大小为tileSize）的边框,为了下面计算滤波
        cv::Mat framed = cv::Mat::zeros(image.rows + 2*tileSize, image.cols + 2*tileSize, CV_32FC1); // 2*tileSize是左边一个，右边一个，竖着也是上下各一个,等于加了一圈
        cv::Mat image_rect_in_frame = framed(cv::Rect(tileSize, tileSize, image.cols, image.rows));
        // 把respons　image复制到一个有一圈空白边框（大小一个tile）的image里面,这可以为后面计算出界的值提供方便
        responses[i].copyTo(image_rect_in_frame);
        //这里这两个平滑其实是求像素邻域的加权和，为了之后求tile值的时候提供便利
        if (_isSmoothHist){
            int kernelSize = 2 * tileSize + 1;
            float gaussBlurSigma = tileSize / 3.0;
            //高斯模糊平滑
            cv::GaussianBlur(framed, framed, cv::Size(kernelSize, kernelSize), gaussBlurSigma, gaussBlurSigma);
        }else{
            int kernelSize = tileSize;
            //　方框滤波:每个像素的值是周围一个方框核内的所有像素的加权（当参数boolnormalize为false时,所有权值为１）和
            //@param (-1,-1)表示被计算的点（锚点)在核的中心
            cv::boxFilter(framed, framed, CV_32F, cv::Size(kernelSize, kernelSize), cv::Point(-1, -1), false);
        }
        // response have now size of image + 2*tileSize in each dimension
        responses[i] = framed;
    }

    // emptyFeatures 是一个网格，每个节点是一个标志位，如果该tile是空的(没有笔画)，那么值为１，否则值为０   using std::cout;
    using std::endl;
    // 有多少个keypoints，就有多少个feature,就可能有多少各空feature 
    emptyFeatures.resize(keypoints.size(), 0);

    /**
     * -------------------------------------------------------------------------------------------------------------------------------
     */
    //对当前图像中的每个tile收集响应幅值
    for (uint i = 0; i < keypoints.size(); i++) { // 遍历当前image所有feature的循环
        const Vec_f32_t& keypoint = keypoints[i];

        // histogram是对一个feature的各个方向上的tile内的像素值进行统计
        Vec_f32_t histogram(_tiles * _tiles * _numOrients, 0.0f);

        //选取　feature区域　的方法： 以keypoint为中心，keypoint_size/2为半径的圆的外切正方形
        cv::Rect rect(keypoint[0] - featureSize/2, keypoint[1] - featureSize/2, featureSize, featureSize);

        // TODO:&是求两个Rect的交集
        cv::Rect isec = rect & cv::Rect(0, 0, src.cols, src.rows);

        // adjust rect position by frame width
        // 因为边框加了一圈(tileSize)，所以坐标也要相应的偏移
        rect.x += tileSize;
        rect.y += tileSize;

        //############检查当前tile有没有笔画,是不是空tile
        // tl() : 左上角 point
        // br : 右下角 point
        // patchsum就是求当前patch区域的所有像素的和
        int patchsum = integral(isec.tl())
                + integral(isec.br())
                - integral(isec.y, isec.x + isec.width)
                - integral(isec.y + isec.height, isec.x);

        if (patchsum == 0)//就是空的feature
        {
            // skip this patch. It contains no strokes.
            // add empty histogram, filled with zeros,
            // will be (optionally) filtered in a later descriptor computation step
            features.push_back(histogram); //　TODO:空的特征为什么要加入呢,不加也可以
            emptyFeatures[i] = 1; //把当前特征是不是空的标志位置为１
            continue;
        }

        const int ndims[3] = { _tiles, _tiles, _numOrients };
        cv::Mat_<float> hist(3, ndims, 0.0f);
        //遍历所有图像Gabor滤波后的响应图像
        // responses.size() == orientations_nums
        for (uint k = 0; k < _numOrients; k++) {
            for (int y = rect.y + halfTileSize; // y设为当前feature的中点像素的坐标
                    y < rect.br().y; y += tileSize) {
                for (int x = rect.x + halfTileSize; // x也是当前feature最中间像素的坐标
                        x < rect.br().x; x += tileSize) {
                    //　x,y的步长都是 tileSize　: 说明这两层循环是遍历了一遍当前feature内所有tile的中点像素
                    // 为什么遍历中点像素呢，因为上面用方框滤波器或者高斯滤波器把中点的值算为了周围像素的和了，公式F(s,t,i) = \sum R_i(x,y)
                    // NOTE: we have added a frame with the size of a tile
                    if (y < 0 || x < 0 || y >= responses[k].rows || x >= responses[k].cols)
                        continue;
                    // 获得在本feature内的相对坐标
                    int ry = y - rect.y;   
                    int rx = x - rect.x;
                    // 计算tile的坐标
                    int tx = rx / tileSize;
                    int ty = ry / tileSize;

                    CHECK(tx >= 0 && ty >= 0) << "tile coordination is wrong !";
                    CHECK(static_cast<uint>(tx) < _tiles && static_cast<uint>(ty)  < _tiles)<<"tile coordination is wrong!";
                    //　见论文　５．３ 的公式 公式里是把当前tile里的Ri(x,y)求和，
                    //  这里没求和(是因为上面那个方框滤波器的缘故,看上面,
                    //  因为高斯滤波或者方框滤波已经把中点的值设置为当前tile的和了，
                    //  所以这里只需要遍历所有tile中点像素就可以了)
                    hist(ty, tx, k) = responses[k].at<float>(y, x); // x,y存的是tile中点的值
                }
            }
        }
        // 把hist里面的值复制到histogram里 
        std::copy(hist.begin(), hist.end(), histogram.begin());

        // 归一化
        if (_normalizeHist == "l2")
        {
            float sum = 0;
            for (size_t i = 0; i < histogram.size(); i++) sum += histogram[i]*histogram[i];
            sum = std::sqrt(sum)  + std::numeric_limits<float>::epsilon(); // + eps avoids div by zero
            for (size_t i = 0; i < histogram.size(); i++) histogram[i] /= sum;
        }
        else if (_normalizeHist == "lowe")
        {
            cv::Mat histwrap(histogram);
            cv::Mat tmp;
            cv::normalize(histwrap, tmp, 1, 0, cv::NORM_L1);
            tmp = cv::min(tmp, 0.2);
            cv::normalize(histwrap, histwrap, 1, 0, cv::NORM_L1);
            histogram = histwrap;
        }
        // do not normalize if user has explicitly asked for that
        else if (_normalizeHist == "none") {}

        // let the user know about the wrong parameter
        else throw std::runtime_error("unsupported histogram normalization method passed (" + _normalizeHist + ")." + "Allowed methods are : lowe, l2, none." );

        // add histogram to the set of local features for that image
        // 把当前feature_region的histogram存入features
        features.push_back(histogram);
    }//遍历当前image所有feature的循环
    /**
     * -------------------------------------------------------------------------------------------------------------------------------
     */
}

void Galif::assertImageSize(const cv::Mat &image) const
{
    //image size must be larger than filter size
    CHECK((image.size().width <= _filterSize.height) &&
            (image.size().height <= _filterSize.width))<<
        "image is small than filter size";
}

} //namespace sse


