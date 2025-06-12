class Solution
{
public:
    int uniquePaths(int m, int n)
    {
        vector<vector<int>> dp;
        dp.resize(m);
        for (auto it = dp.begin(); it != dp.end(); it++)
            it->resize(n);

        for (int i = 0; i < dp.size(); i++)
        {
            for (int j = 0; j < dp[i].size(); j++)
            {
                if (i == 0 || j == 0)
                    dp[i][j] = 1;
                else
                    dp[i][j] = dp[i - 1][j] + dp[i][j - 1];
            }
        }

        return dp[m - 1][n - 1];
    }
};
