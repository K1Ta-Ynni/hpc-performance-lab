#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;
using namespace chrono;

// ==================== 任务1：矩阵列内积 ====================
namespace MatrixVector {
    vector<double> naive(const vector<vector<double>>& A, const vector<double>& b) {
        int n = A.size();
        vector<double> result(n, 0.0);
        for (int j = 0; j < n; ++j) {
            for (int i = 0; i < n; ++i) {
                result[j] += A[i][j] * b[i];
            }
        }
        return result;
    }
    
    vector<double> cacheOptimized(const vector<vector<double>>& A, const vector<double>& b) {
        int n = A.size();
        vector<double> result(n, 0.0);
        for (int i = 0; i < n; ++i) {
            double bi = b[i];
            for (int j = 0; j < n; ++j) {
                result[j] += A[i][j] * bi;
            }
        }
        return result;
    }
}

// ==================== 任务2：数组求和 ====================
namespace Sum {
    double naive(const vector<double>& arr) {
        double sum = 0.0;
        for (double x : arr) {
            sum += x;
        }
        return sum;
    }
    
    double twoWay(const vector<double>& arr) {
        double sum0 = 0.0, sum1 = 0.0;
        size_t i = 0;
        for (; i + 1 < arr.size(); i += 2) {
            sum0 += arr[i];
            sum1 += arr[i + 1];
        }
        for (; i < arr.size(); ++i) {
            sum0 += arr[i];
        }
        return sum0 + sum1;
    }
}

// ==================== 高精度计时 ====================
template<typename Func>
double measureTime(Func func, int warmup = 2, int targetMs = 100) {
    // 预热
    for (int i = 0; i < warmup; ++i) {
        func();
    }
    
    // 估算单次耗时
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    double singleTime = duration<double>(end - start).count();
    
    // 如果单次时间太短（<1us），用固定重复次数
    int repeatTimes;
    if (singleTime < 1e-6) {
        repeatTimes = 100000;  // 太短时固定10万次
    } else {
        repeatTimes = max(1, (int)(targetMs / 1000.0 / singleTime));
        repeatTimes = min(repeatTimes, 100000);
    }
    
    start = high_resolution_clock::now();
    for (int i = 0; i < repeatTimes; ++i) {
        func();
    }
    end = high_resolution_clock::now();
    
    double totalTime = duration<double>(end - start).count();
    return totalTime / repeatTimes;
}

// ==================== 主函数 ====================
int main() {
    // 矩阵测试
    vector<int> matrixSizes = {256, 512, 1024, 2048, 4096};
    
    cout << "========== Matrix-Vector Dot Product ==========" << endl;
    cout << "n\tnaive(ms)\tcache-opt(ms)\tspeedup" << endl;
    for (int n : matrixSizes) {
        vector<vector<double>> A(n, vector<double>(n, 1.0));
        vector<double> b(n, 1.0);
        
        auto naiveTime = measureTime([&]() {
            MatrixVector::naive(A, b);
        }, 2, 500);
        
        auto cacheTime = measureTime([&]() {
            MatrixVector::cacheOptimized(A, b);
        }, 2, 500);
        
        cout << n << "\t" 
             << naiveTime * 1000 << "\t"
             << cacheTime * 1000 << "\t"
             << (cacheTime > 0 ? naiveTime / cacheTime : 0) << endl;
    }
    
    // 求和测试
    vector<int> sumSizes = {100000, 500000, 1000000, 5000000, 10000000};
    
    cout << "\n========== Array Sum ==========" << endl;
    cout << "n\tnaive(us)\ttwo-way(us)\tspeedup" << endl;
    
    for (int n : sumSizes) {
        vector<double> arr(n, 1.0);
        
        // 用 volatile 防止编译器优化掉计算结果
        volatile double result_naive = 0;
        volatile double result_twoway = 0;
        
        auto naiveTime = measureTime([&]() {
            result_naive = Sum::naive(arr);
        }, 2, 200);
        
        auto twoWayTime = measureTime([&]() {
            result_twoway = Sum::twoWay(arr);
        }, 2, 200);
        
        // 可选：打印结果验证正确性（仅第一个）
        static bool first = true;
        if (first && n == 100000) {
            cout << "[Verify] naive sum = " << result_naive << ", two-way sum = " << result_twoway << endl;
            first = false;
        }
        
        cout << n << "\t" 
             << naiveTime * 1000000 << "\t"
             << twoWayTime * 1000000 << "\t"
             << (twoWayTime > 0 ? naiveTime / twoWayTime : 0) << endl;
    }
    
    return 0;
}