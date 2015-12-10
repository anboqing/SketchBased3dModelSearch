#include <iostream>
#include <omp.h>
using namespace std;

int main(){
    cout << omp_get_num_procs() << endl;
#pragma omp parallel for
    for(int i = 0 ;i<10;i++){
        cout<< i << endl; 
    }
    return 0;
}
