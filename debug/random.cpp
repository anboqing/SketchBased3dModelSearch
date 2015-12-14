#include <functional>
#include <iostream>
#include <random>

int main(){
    std::random_device rd;
    std::mt19937 mt(rd());
    typedef std::uniform_real_distribution<double> Distribution;
    auto r = std::bind(Distribution(0.0,1.0),mt);

    double rnd = r();
    std::cout << rnd << std::endl;
}
