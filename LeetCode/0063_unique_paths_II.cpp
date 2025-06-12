class Solution
{
public:
    int uniquePathsWithObstacles(vector<vector<int>> &obstacleGrid)
    {
        vector<vector<int>> dp(obstacleGrid.size());
        for (auto it = dp.begin(); it != dp.end(); it++)
            it->resize(obstacleGrid[0].size());

        dp[0][0] = 1 - obstacleGrid[0][0];
        for (int j = 1; j < dp[0].size(); j++)
            dp[0][j] = obstacleGrid[0][j] == 1 ? 0 : dp[0][j - 1];
        for (int i = 1; i < dp.size(); i++)
        {
            dp[i][0] = obstacleGrid[i][0] == 1 ? 0 : dp[i - 1][0];
            for (int j = 1; j < dp[0].size(); j++)
                dp[i][j] = obstacleGrid[i][j] == 1 ? 0 : dp[i - 1][j] + dp[i][j - 1];
        }
        return dp[obstacleGrid.size() - 1][obstacleGrid[0].size() - 1];
    }
};
