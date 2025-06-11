class Solution
{
    bool checkBit(vector<string> &strs, int idx)
    {
        char c = strs[0][idx];
        if(c == '\0')
            return false;
        for (int i = 1; i < strs.size(); i++)
            if (strs[i][idx] != c)
                return false;
        return true;
    }

public:
    string longestCommonPrefix(vector<string> &strs)
    {
        int idx = 0;
        string sol = "";
        while (checkBit(strs, idx))
            idx++;
        sol += strs[0].substr(0, idx);
        return sol;
    }
};
