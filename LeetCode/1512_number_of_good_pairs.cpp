class Solution
{
public:
    int numIdenticalPairs(vector<int> &nums)
    {
        int result = 0;
        sort(nums.begin(), nums.end());

        int temp = 1;
        int current = nums[0];
        for (int i = 1; i < nums.size(); i++)
        {
            if (nums[i] == current)
                temp++;
            else
            {
                result += temp * (temp - 1) / 2;
                current = nums[i];
                temp = 1;
            }
        }
        result += temp * (temp - 1) / 2;
        return result;
    }
};
