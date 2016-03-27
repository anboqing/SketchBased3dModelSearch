#include "../include/evaluate_utils.h"
#include <iostream>
using std::cout;
using std::endl;
int main(int argc,char** argv){
    if(argc<2){
        return -1; 
    }

    std::map<size_t,std::string> model_class;
    std::map<std::string,size_t> cla_info;

    ReadClassificationFile(argv[1],model_class,cla_info);

    std::map<std::string,size_t> name_dic;
    //GetModelNameIndexDiction(cla_info,name_dic);

    std::vector<std::vector<size_t> > model_cla_vec;

    GenModelClassList(model_class,cla_info,model_cla_vec);

    for(auto& x: model_cla_vec){
        cout << x[0]<< " " << x[1] << endl; 
    }

    return 0;
}
