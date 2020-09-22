// Simple RadixSort implentation based on the code from https://www.geeksforgeeks.org/radix-sort/ and slightly reworked using stl containers

#include <vector>

void radixSort(std::vector<int>& arr)
{
    int n = arr.size();
    int m = 0;
    for(const auto& a : arr) { if(a > m) { m = a; } }
    
    std::vector<int> sorted_arr;
    sorted_arr.resize(arr.size());

    for (int exp = 1; m / exp > 0; exp *= 10) {
        int count[10] { 0 };

        for (int i = 0; i < n; i++) {
            count[(arr[i] / exp) % 10]++;
        }

        for (int i = 1; i < 10; i++){
            count[i] += count[i - 1];
        }

        for (int i = n - 1; i >= 0; i--) {
            sorted_arr[count[(arr[i] / exp) % 10] - 1] = arr[i];
            count[(arr[i] / exp) % 10]--;
        }

        for(int i = 0; i < n; i++) {
            arr[i] = sorted_arr[i];
        }
    }
}