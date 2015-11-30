#include <Eigen/Dense>
#include <iostream>

using namespace Eigen;
using std::cout ;
using std::endl;


int main(){
    Matrix3f a;
    MatrixXf b;
    MatrixXf c(2,2);
    VectorXf d(2);
    cout << "a\n"<<a << endl;
    cout << "b\n" << b << endl;
    cout << "c\n" << c << endl;
    cout << "d\n" << d << endl;

    // fix-size matrix
    Vector2i e(1,2);
    Vector3d f(1.0,2.0,3.0);

    cout << "e\n" << e << endl;
    cout << "f\n" << f << endl;
    
    // Comma-initialization
    
    Matrix3f m;
    m << 1,2,3,
      4,5,6,
      7,8,9;
    cout << "m----"<<m<<endl;
    

    return 0;
}
