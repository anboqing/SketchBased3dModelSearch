#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
// static void help()
// {
//     cout << "\nThis program demonstrates kmeans clustering.\n"
//             "It generates an image with random points, then assigns a random number of cluster\n"
//             "centers and uses kmeans to move those cluster centers to their representitive location\n"
//             "Call\n"
//             "./kmeans\n" << endl;
// }
int main( int /*argc*/, char** /*argv*/ )
{
    const int MAX_CLUSTERS = 5;
    Scalar colorTab[] =
    {
        Scalar(0, 0, 255),
        Scalar(0,255,0),
        Scalar(255,100,100),
        Scalar(255,0,255),
        Scalar(0,255,255)
    };
    Mat img(800, 800, CV_8UC3);
    RNG rng(12345);
    for(;;)
    {
        // 生成聚类中心数
        int k, clusterCount = rng.uniform(2, MAX_CLUSTERS+1);
        //生成样本数
        int i, sampleCount = rng.uniform(1, 1001);
        // 声明样本点和标签
        Mat points(sampleCount, 1, CV_32FC2), labels;
        // 聚类数量不能比样本数大
        clusterCount = MIN(clusterCount, sampleCount);
        // 声明聚类中心
        Mat centers;
        /* generate random sample from multigaussian distribution */
        for( k = 0; k < clusterCount; k++ )
        {
            Point center;
            center.x = rng.uniform(0, img.cols);
            center.y = rng.uniform(0, img.rows);
            Mat pointChunk = points.rowRange(k*sampleCount/clusterCount,
                                             k == clusterCount - 1 ? sampleCount :
                                             (k+1)*sampleCount/clusterCount);
            rng.fill(pointChunk, RNG::NORMAL, Scalar(center.x, center.y), Scalar(img.cols*0.05, img.rows*0.05));
        }
        randShuffle(points, 1, &rng);
        // 调用kmeans
        kmeans(points, clusterCount, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),// 算法终止条件，最大跌代数(10)或目标精度(1.0)
               3, KMEANS_PP_CENTERS, centers);
        // 声明一个空图像
        img = Scalar::all(0);
        for( i = 0; i < sampleCount; i++ )
        {
            int clusterIdx = labels.at<int>(i);
            Point ipt = points.at<Point2f>(i);
            // 画出每个点
            circle( img, ipt, 2, colorTab[clusterIdx], 1, CV_AA );
        }
        imshow("clusters", img);
        char key = (char)waitKey();
        if( key == 27 || key == 'q' || key == 'Q' ) // 'ESC'
            break;
    }
    return 0;
}
