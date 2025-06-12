class Solution
{
public:
    int numJewelsInStones(string jewels, string stones)
    {
        int result = 0;
        sort(jewels.begin(), jewels.end());
        sort(stones.begin(), stones.end());

        auto it_j = jewels.begin(), it_s = stones.begin();
        while (it_j != jewels.end() && it_s != stones.end())
        {
            if (*it_s == *it_j)
            {
                result++;
                it_s++;
            }
            else
            {
                if (*it_s > *it_j)
                    it_j++;
                else
                    it_s++;
            }
        }
        return result;
    }
};
