

#include<random>
#include<iostream>
#include<chrono>
#include<vector>
#include"algorithm/async_quick_sort.h"
const int num_size = 1000000;
using namespace std;
int main()
{
    default_random_engine eng;
    uniform_int_distribution<int> dis(0, 100000);

    vector<int> vec0;
    vec0.reserve(num_size);
    for (int i = 0; i < num_size; ++i) {
        vec0.push_back(dis(eng));
    }

    vector<int> vec1 = vec0;
    vector<int> vec2 = vec0;
    vector<int> vec3 = vec0;
    star::thread_pool pool(10);
    std::chrono::system_clock::time_point tp_point0, tp_point1, tp_point3;
    tp_point0 = std::chrono::system_clock::now();

    star::quicksort(vec1.begin(), vec1.end());
    tp_point1 = std::chrono::system_clock::now();

    star::quicksort_threadpool(vec1.begin(), vec1.end(), pool);
    tp_point3 = std::chrono::system_clock::now();

    std::chrono::system_clock::duration dua = tp_point1 - tp_point0;
    int total_time = std::chrono::duration_cast<std::chrono::milliseconds>(dua).count();
    printf("quick sort Total time: %d miliseconds\n", total_time);
    dua = tp_point3 - tp_point1;
    total_time = std::chrono::duration_cast<std::chrono::milliseconds>(dua).count();
    printf("quick sort thread pool Total time: %d miliseconds\n", total_time);
}
