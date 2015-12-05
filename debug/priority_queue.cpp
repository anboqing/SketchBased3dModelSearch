#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <map>

using namespace std;

typedef pair<int,float> Hist_t;

// 函数对象： 定义了operator()的 类的对象

class MyComp{
public:
    bool operator()(const Hist_t& l,const Hist_t& r){
         if(l.second < r.second ){
            return true;
         }else{
            return false; 
         }
    }
};


int main(){
   
    pair<int,float> p1 = make_pair(1,20);
    pair<int,float> p2 = make_pair(2,80);
    pair<int,float> p3 = make_pair(3,40);

    std::priority_queue<Hist_t,std::vector<Hist_t>,MyComp> pq;
    pq.push(p1);
    pq.push(p2);
    pq.push(p3);


    for(;!pq.empty(); ){
        cout << pq.top().first << " " << pq.top().second << endl;
        pq.pop();
    }
    
    return 0;
}
