#include <random>
#include <algorithm>
#include <iostream>
#include <functional>
#include <stdlib.h>
#include <chrono>

using namespace std;

int main(int argc,char**argv){

    if(argc<3){
        cout << "usage : ./this nrolls nstars " << endl;
        return -1;
    }
    
    std::mt19937 gen;
    std::uniform_int_distribution<int> distribution(0,9);
    auto one_ten = std::bind(distribution,gen);

    int nrolls = atoi(argv[1]);
    int nstars = atoi(argv[2]);

    int p[10] = {};

    for(int i = 0; i<nrolls;++i){
        int number = one_ten(); 
        ++p[number];
    }

    for(int i = 0;i<10;++i){
        std::cout << i<<":"<<std::string(p[i]*nstars/nrolls,'*')<<std::endl; 
    }

    // test std::shuffle
    std::vector<int> ivec = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::random_device rd;
    std::mt19937 gen_rd(rd());
    for(int i = 0; i< 15;i++){
        //shuffle(ivec.begin(),ivec.end(),std::mt19937(seed));
        shuffle(ivec.begin(),ivec.end(),gen_rd);
        //shuffle(ivec.begin(),ivec.end(),std::default_random_engine(seed));
        for(int &x : ivec)cout<< " " << x;
        cout << endl;
    }
    

    return 0;
}
