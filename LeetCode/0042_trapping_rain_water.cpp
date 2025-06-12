class Solution
{
public:
    int trap(vector<int> &height)
    {
        int result = 0;
        auto it_l = height.begin(), it_r = height.end() - 1;
        int height_l = *it_l, height_r = *it_r;
        while (it_l < it_r)
        {
            if (*it_l < *it_r)
            {
                it_l++;
                if (height_l < *it_l)
                    height_l = *it_l;
                else
                    result += height_l - *it_l;
            }
            else
            {
                it_r--;
                if (height_r < *it_r)
                    height_r = *it_r;
                else
                    result += height_r - *it_r;
            }
        }
        return result;
    }
};
