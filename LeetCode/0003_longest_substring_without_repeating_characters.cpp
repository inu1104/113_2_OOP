class Solution
{
public:
    int lengthOfLongestSubstring(string s)
    {
        int result = 0;
        set<char> alphabets;
        auto it_l = s.begin(), it_r = s.begin();

        int len = 0;
        while (it_r < s.end())
        {
            if (alphabets.find(*it_r) == alphabets.end())
            {
                alphabets.insert(*it_r++);
                len++;
            }
            else
            {
                while (alphabets.find(*it_r) != alphabets.end())
                {
                    alphabets.erase(*it_l++);
                    len--;
                }
            }
            result = max(result, len);
        }
        return result;
    }
};
