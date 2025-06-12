  class Solution
{
    vector<int> f = {1, 1};

public:
    int climbStairs(int n)
    {
        if (n < f.size())
            return f[n];
        f.push_back(climbStairs(n - 2) + climbStairs(n - 1));
        return f[n];
    }
};
