class Solution
{
public:
    int lengthOfLastWord(string s)
    {
        int current = 0, result = 0;
        for (auto it = s.begin(); it - 1 != s.end(); it++)
        {
            if (*it == ' ' || *it == '\0')
            {
                result = current == 0 ? result : current;
                current = 0;
            }
            else
                current++;
        }
        return result;
    }
};
