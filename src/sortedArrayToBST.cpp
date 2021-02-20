#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* left, TreeNode* right) : val(x), left(left), right(right) {}
};

TreeNode* arrayToBST(vector<int>& nums, int start, int end) {
    if (end - start == 0) {
        return nullptr;
    }
    
    int a = (end - start) / 2;
    TreeNode* tree = new TreeNode(nums[start + a]);
    tree->left = arrayToBST(nums, start, start + a);
    tree->right = arrayToBST(nums, start + a + 1, end);
    return tree;
}

TreeNode* sortedArrayToBST(vector<int>& nums) {

    return arrayToBST(nums, 0, nums.size());
}

int main()
{
    vector<int> nums1 = { -10,-3,0,5,9,10};
    TreeNode* tree1 = sortedArrayToBST(nums1);
    cout << ":)" << endl;
}