#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
int main(){
   vector<float> a(10,0.1) ,b(10,0.2);
   vector<vector<float> > aa,bb;
   aa.push_back(a);
   aa.push_back(b);
   bb.push_back(b);
   bb.push_back(a);

   string name;
   ostringstream oss(name);
   oss<<"feature_"<<aa.size()<<"_"<<aa[0].size()<<".dat";
   cout <<oss.str()<<endl;
}
