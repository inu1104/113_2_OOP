class Solution
{
public:
    int minPathSum(vector<vector<int>> &grid)
    {
        vector<vector<int>> dp(grid.size());
        for (auto it = dp.begin(); it != dp.end(); it++)
            it->resize(grid[0].size());

        dp[0][0] = grid[0][0];
        for (int j = 1; j < grid[0].size(); j++)
            dp[0][j] = dp[0][j - 1] + grid[0][j];
        for (int i = 1; i < grid.size(); i++)
        {
            dp[i][0] = dp[i - 1][0] + grid[i][0];
            for (int j = 1; j < grid[0].size(); j++)
                dp[i][j] = min(dp[i - 1][j], dp[i][j - 1]) + grid[i][j];
        }
        return dp[grid.size() - 1][grid[0].size() - 1];
    }
};
