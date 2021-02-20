#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

int binSearchHelper(const vector<int>& nums, int target, int start, int end) {
    if(start > end) {
        return -1;
    }

    int mid = (start+end)/2;
    if(nums[mid] == target) {
        return mid;
    }
    else if (nums[mid] > target) {
        return binSearchHelper(nums, target, start, mid-1);
    }
    else {
        return binSearchHelper(nums, target, mid+1, end);
    }
}

int binarySearch(const vector<int>& nums, int target) {
    return binSearchHelper(nums, target, 0, nums.size()-1);
}

int main() {
    vector<int> nums1 = {1,3,4,5,6,7,8,10};

    // positive test cases
    assert(binarySearch(nums1,10) == 7);
    assert(binarySearch(nums1,4) == 2);
    assert(binarySearch(nums1,1) == 0);

    // negative test cases
    assert(binarySearch(nums1,2) == -1);
    assert(binarySearch(nums1,9) == -1);
    assert(binarySearch(nums1,0) == -1);
    assert(binarySearch(nums1,11) == -1);
}
