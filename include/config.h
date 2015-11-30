#ifndef __CONFIG__H__
#define __CONFIG__H__

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include <memory> // shared_ptr
#include <mutex> // mutex , lock_guard
#include <map>

/*
 *@brief : base class for configuration classes
 */
class Configuration{
public:
    virtual void load(const std::string & filename)=0;
    virtual void save(const std::string & filename)=0;
};

/*
 * @brief : 路径配置
 */
class PathConfig : public Configuration{
    
public:
    // @overwrite
    void load(const std::string& filename);
    void save(const std::string& filename);
private:
    PathConfig(){}
    static std::shared_ptr<PathConfig> _instance;
    static std::mutex _mutex;
public:
    static std::shared_ptr<PathConfig> GetInstance();
public:
    std::map<std::string,std::string> _conf_map;
};


/****************************************************************************************
  sketch generate config
 **************************************************************************************/

/*
 *@brief : 存取草图生成模块配置文件的类
 */
class SketchGenConfig : public Configuration{
    public:
        //@overwrite
        void load(const std::string& filename);
        void save(const std::string& filename);
    public:
        std::string _model_dir;
        std::string _view_dir;
        std::string _output_dir;
        int _view_num;

        bool _isDrawBoundaries = false;
        bool _isDrawOccludingContours = false;
        bool _isDrawSuggestiveContours = true;
        bool _isDrawApparentRidges = true;
        bool _isDrawRidges = false;
        bool _isDrawValleys = false;

    private:
        
        // used for singleton pattern
        SketchGenConfig(){}
        static std::shared_ptr<SketchGenConfig> _sketch_instance;
        static std::mutex _sketch_mutex;
    public:
        static std::shared_ptr<SketchGenConfig> GetInstance();
};


/*
 *@brief : 存取特征提取模块配置文件的类
 */
class FeatureConfig: public Configuration{
    public:
        //@overwrite
        void load(const std::string& filename);
        void save(const std::string& filename);
    public:
        typedef unsigned int uint;
        uint _width;
        uint _orientation_n;
        uint _tile_n;
        double _peakFrequency;
        double _line_width;
        double _lambda;
        float _feature_size;
        bool _is_smooth_hist;
        std::string _normalize_type;
        std::string _detector_name;
        uint _num_of_samples;
    private:
        // used for singleton pattern
        FeatureConfig(){}
        static std::shared_ptr<FeatureConfig> _instance;
        static std::mutex _mutex;
    public:
        static std::shared_ptr<FeatureConfig> GetInstance();
};

#endif
