
#ifndef __KMEANS_H__
#define __KMEANS_H__

#include <vector>
#include <string> // getline
#include <fstream> // ifstream
#include <sstream> // istringstream
#include <cmath> // sqrt 
#include <ctime> // time(0)
#include <cassert> // assert
#include <algorithm> // min_element , max_element
#include <climits> // std::numeric_limits
#include <numeric> // std::accumulate
#include <utility> // std::pair

#include <iostream>
using std::cout ;
using std::endl;

using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;

bool LoadDataSet(const string& filename, vector<vector<float> >& dataSet){
	//°ÑÊý¾Ý¶ÁÈ¡µ½datasetÀï
	ifstream infile(filename);
	if (!infile.is_open()){
		return false;
	}
	string line;
	while (getline(infile, line)){
		istringstream iss(line);
		float data;
		vector<float> lineData;
		while (iss >> data){
			lineData.push_back(data);
		}
		dataSet.push_back(lineData);
	}
	infile.close();
	return true;
}

//¿¿¿¿¿¿
float distEclud(const vector<float>& a,const vector<float>& b){
    assert(a.size() == b.size());
    float sum = 0.0;
    for(vector<float>::size_type ix = 0 ;ix < a.size(); ++ix){
        sum+=pow(a[ix]-b[ix],2); 
    }
    return sqrt(sum);
}


//_________________________________________________

// @param j : column
float minVec(const vector<vector<float> >&dataSet,int j){
   vector<float> temp ;
   for(vector<vector<float> >::size_type ix = 0;ix < dataSet.size();++ix){
        temp.push_back(dataSet[ix][j]);
   } 
   float res = *(std::min_element(temp.begin(),temp.end()));
   return res;
}

// @param j : column
float maxVec(const vector<vector<float> >&dataSet,int j){
   vector<float> temp ;
   for(vector<vector<float> >::size_type ix = 0;ix < dataSet.size();++ix){
        temp.push_back(dataSet[ix][j]);
   } 
   float res = *(std::max_element(temp.begin(),temp.end()));
   return res;
}

float randf(){
    return (float)(abs(rand())%10001*0.0001);
}
/*
 * @brief ¿¿¿¿¿¿¿¿¿¿¿
 * @param dataSet | in |
 * @param k | in | the number of clusters
 * @param centroids | out | the cetroid of all clusters
 */
bool randCent(const vector<vector<float> >& dataSet,unsigned int k,vector<vector<float> >& centroids){
   if(dataSet.empty()|| k<=0){
       return false;
   }
   unsigned int n = dataSet[0].size(); 
   // init centroids
   for(unsigned int i = 0 ;i<k;i++){
         vector<float> fvec(n,0);
         centroids.push_back(fvec);
   }
   
   srand(time(0));
   // ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
   for(unsigned int j = 0 ;j<n ;j++){
        float minJ = minVec(dataSet,j);         
        float rangeJ = maxVec(dataSet,j)-minJ;
        for(vector<vector<float> >::size_type row= 0;row<k;++row){
            centroids[row][j] = minJ+rangeJ*randf(); 
        }
   }
   return true;
}

bool kMeans(const vector<vector<float> >& dataSet,
        unsigned int k,
        vector<std::pair<int,float> >& clusterAssment,
        vector<vector<float> >& centroids,
        float (*distMeans)(const vector<float>&,const vector<float>&) =distEclud,
        bool (*createCent)(const vector<vector<float> >&,unsigned int k,vector<vector<float> >&)=randCent
        ){
    // ¿¿¿¿¿¿¿¿¿
    for(unsigned int ix = 0 ;ix<dataSet.size() ;++ix){
        std::pair<int,float> p(-1,0.0);
        clusterAssment.push_back(p);
    }
    // ¿¿¿¿¿¿¿¿¿
    createCent(dataSet,k,centroids);
    // ¿¿¿¿¿¿¿
    bool clusterChanged = true;
    while(clusterChanged){
        clusterChanged = false;
        // ¿¿¿¿¿¿¿
        for(vector<vector<float> >::size_type ix = 0;ix < dataSet.size(); ++ix){
            float minDist = std::numeric_limits<float>::max();
            int minIndex = -1;
            //¿¿¿¿¿¿¿¿¿¿¿¿
            for(unsigned int jx = 0;jx < k;++jx){
                float distJI = distMeans(dataSet[ix],centroids[jx]); 
                if(distJI < minDist){
                    minDist = distJI;
                    minIndex = jx;
                }
            }
            //(¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
            if(clusterAssment[ix].first!=minIndex){
                clusterChanged = true; 
            }
            //¿¿¿¿¿¿¿¿¿¿
            clusterAssment[ix] = std::make_pair(minIndex,minDist*minDist);
        }
        if(clusterChanged){
#ifdef DEBUG
            // print centroids
            for(auto cent : centroids){
                for(auto item : cent ){
                    cout << item << " " ;
                } 
                cout << endl;
            }
            cout<<"-------------"<<endl;
#endif
            //  ¿¿¿¿¿¿¿¿¿¿¿¿¿
            for(vector<vector<float> >::size_type cent_row = 0;cent_row < centroids.size() ; ++cent_row){
                // ¿¿¿¿¿¿¿¿¿¿ 
                vector<vector<float> > currentCluster;
                for(vector<vector<float> >::size_type idata= 0;idata< dataSet.size() ; ++idata){
                    if(clusterAssment[idata].first == static_cast<int>(cent_row)) 
                        currentCluster.push_back(dataSet[idata]);
                }
                // ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
                for(vector<float>::size_type col= 0 ;col<centroids[0].size();++col){
                    float mean = 0.0;
                    for(vector<vector<float> >::size_type item = 0;item < currentCluster.size() ; ++item){
                        mean += currentCluster[item][col]; 
                    }
                    mean/=(float)currentCluster.size();
                    centroids[cent_row][col] = mean;
                }
            }
        }
    }
    return true;
}

#endif
