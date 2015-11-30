#include "kmeans.h"
#include <iostream>
using std::cout; using std::endl;

void TestLoadData(){
	vector<vector<float> > fvec;
	LoadDataSet("testSet.txt", fvec);
	for (auto fv : fvec){
		for (auto f : fv){
			cout << f << " ";
		}
		cout << endl;
	}

}

void TestDist(){
    vector<float> a(2,0);
    vector<float> b(2,1);
    float res = distEclud(a,b);
    cout << res*res << endl; 
}


void TestRandCent(){
    vector<vector<float> > a,cent;
    vector<float> aa(2,1);
    vector<float> ab(2,2);
    vector<float> ac(2,10);
    a.push_back(aa);
    a.push_back(ab);
    a.push_back(ac);
    cout << minVec(a,0) << " " << maxVec(a,1) << endl;
    randCent(a,3,cent);
    for(auto c:cent){
        for(auto b:c){
            cout << b <<" "; 
        } 
        cout << endl;
    }
}


void TestKmeans(){
	vector<vector<float> > dataSet;
	LoadDataSet("testSet.txt", dataSet);
    vector<std::pair<int,float> > clusterAssment;
    vector<vector<float> > centroids;
    kMeans(dataSet,4,clusterAssment,centroids);
    // print centroids
    for(auto cent : centroids){
        for(auto x:cent){
            cout << x << " "; 
        } 
        cout << endl;
    }
}

int main(){
    // TestLoadData();
    // TestDist();
    // TestRandCent();
    TestKmeans();
    
    return 0;
}
