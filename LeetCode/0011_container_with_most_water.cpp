class Solution
{
public:
    int maxArea(vector<int> &height)
    {
        int result = 0, current, width;
        int left = 0, right = height.size() - 1;
        while (left < right)
        {
            width = right - left;
            if (height[left] < height[right])
                current = height[left++] * width;
            else
                current = height[right--] * width;
            result = max(result, current);
        }
        return result;
    }
};
