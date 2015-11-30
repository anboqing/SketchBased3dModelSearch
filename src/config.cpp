#include "config.h"
#include <fstream>
#include <glog/logging.h>


// static member need to declared
std::shared_ptr<SketchGenConfig> SketchGenConfig::_sketch_instance;
std::mutex SketchGenConfig::_sketch_mutex;


//@brief the singleton machinisim
std::shared_ptr<SketchGenConfig> SketchGenConfig::GetInstance(){
    if(_sketch_instance.get()==0){
        // for the sake of thread safe 
        std::lock_guard<std::mutex> guard(_sketch_mutex);  
        if(_sketch_instance.get() == 0){
            _sketch_instance.reset(new SketchGenConfig()); 
        }
    }    
    return _sketch_instance;
}

//@brief load configuration
void SketchGenConfig::load(const std::string& filename){
    using boost::property_tree::ptree;
    ptree pt;
    //load xml file into the property tree
    read_xml(filename,pt);
    // read from xml file
    _model_dir = pt.get<std::string>("sketchGenerator.model_dir");
    _view_num = pt.get("sketchGenerator.view_num",102);
    _view_dir = pt.get<std::string>("sketchGenerator.view_dir");
    _output_dir = pt.get<std::string>("sketchGenerator.output_dir",".");
    _isDrawBoundaries = pt.get<bool>("sketchGenerator.isDrawBoundaries",false);
    _isDrawOccludingContours = pt.get<bool>("sketchGenerator.isDrawOccludingContours",true);
    _isDrawSuggestiveContours = pt.get<bool>("sketchGenerator.isDrawSuggestiveContours",true);
    _isDrawApparentRidges = pt.get<bool>("sketchGenerator.isDrawApprentRidges",true);
    _isDrawRidges = pt.get<bool>("sketchGenerator.isDrawRidges",false);
    _isDrawValleys = pt.get<bool>("sketchGenerator.isDrawValleys",false);
}


void SketchGenConfig::save(const std::string& filename){
    using boost::property_tree::ptree;
    ptree pt;
    // put property
    pt.put("sketchGenerator.model_dir",_model_dir);
    pt.put("sketchGenerator.view_dir",_view_dir);
    pt.put("sketchGenerator.output_dir",_output_dir);
    pt.put("sketchGenerator.view_num",_view_num);
    pt.put("sketchGenerator._isDrawSuggestiveContours","true");
    pt.put("sketchGenerator._isDrawApparentRidges","true");

    // write to disk file
    write_xml(filename,pt);
}

/****************************************************************************************
  Feature extract config
 **************************************************************************************/

// static member need to declared
std::shared_ptr<FeatureConfig> FeatureConfig::_instance;
std::mutex FeatureConfig::_mutex;


//@brief the singleton machinisim
std::shared_ptr<FeatureConfig> FeatureConfig::GetInstance(){
    if(_instance.get()==0){
        // for the sake of thread safe 
        std::lock_guard<std::mutex> guard(_mutex);  
        if(_instance.get() == 0){
            _instance.reset(new FeatureConfig()); 
        }
    }    
    return _instance;
}

//@brief load configuration
void FeatureConfig::load(const std::string& filename){
    using boost::property_tree::ptree;
    ptree pt;
    //load xml file into the property tree
    read_xml(filename,pt);
    // read from xml file
    _width = pt.get<uint>("feature.sketchwidth",256);
    _orientation_n = pt.get<uint>("feature.orientation_num",4);
    _tile_n = pt.get<uint>("feature.tile_num",4);
    _peakFrequency = pt.get<double>("feature.peak_frequency",0.13);
    _line_width = pt.get<double>("feature.line_width",0.02);
    _lambda = pt.get<double>("feature.lambda",0.3);
    _feature_size = pt.get<float>("feature.feature_size",0.1);
    _is_smooth_hist = pt.get<bool>("feature.is_normalize_hist",true);
    _normalize_type = pt.get<std::string>("feature.normalize_type","l2");
    _detector_name = pt.get<std::string>("feature.detector.name","grid");
    _num_of_samples = pt.get<uint>("feature.num_of_samples",625);
}


void FeatureConfig::save(const std::string& filename){
    using boost::property_tree::ptree;
    ptree pt;
    // put property

    // replace get<*> with put by :s,e s/get<[^>]*>/put/
    pt.put("feature.sketchwidth",256);
    pt.put("feature.orientation_num",4);
    pt.put("feature.tile_num",4);
    pt.put("feature.peak_frequency",0.13);
    pt.put("feature.line_width",0.02);
    pt.put("feature.lambda",0.3);
    pt.put("feature.feature_size",0.1);
    pt.put("feature.is_normalize_hist",true);
    pt.put("feature.normalize_type","l2");
    pt.put("feature.detector.name","grid");
    pt.put("feature.num_of_samples",625);
    
    // write to disk file
    write_xml(filename,pt);
}

std::shared_ptr<PathConfig> PathConfig::_instance;
std::mutex PathConfig::_mutex;

//@brief the singleton machinisim
std::shared_ptr<PathConfig> PathConfig::GetInstance(){
    if(_instance.get()==0){
        // for the sake of thread safe 
        std::lock_guard<std::mutex> guard(_mutex);  
        if(_instance.get() == 0){
            _instance.reset(new PathConfig()); 
        }
    }    
    return _instance;
}

void PathConfig::save(const std::string& name){
    using std::ofstream;
    ofstream outfile(name,std::ios_base::out);
    if(!outfile.is_open()){
        LOG(FATAL) << " outfile is not open correctly " ;
    }
    // 遍历　map
    for(auto pair : _conf_map){
        outfile << pair.first << '\t' << pair.second << std::endl;
    }
    outfile.close();
}

void PathConfig::load(const std::string& name){
    using std::ifstream;

    ifstream infile(name.c_str());

    if(!infile.is_open()){
       LOG(FATAL) << " read PathConfig file failure ";
    }
    std::string line;
    while(getline(infile,line)){
        std::istringstream iss(line);
        std::string key,val;
        iss >> key >> val;
        _conf_map[key] = val;
    }
    infile.close();
}
