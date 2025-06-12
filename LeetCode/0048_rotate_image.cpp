class Solution
{
public:
    void rotate(vector<vector<int>> &matrix)
    {
        int x = (matrix.size() + 1) / 2;
        int y = matrix.size() / 2;
        for (int i = 0; i < x; i++)
        {
            for (int j = 0; j < y; j++)
            {
                int temp = matrix[i][j];
                matrix[i][j] = matrix[matrix.size() - j - 1][i];
                matrix[matrix.size() - j - 1][i] = matrix[matrix.size() - i - 1][matrix.size() - j - 1];
                matrix[matrix.size() - i - 1][matrix.size() - j - 1] = matrix[j][matrix.size() - i - 1];
                matrix[j][matrix.size() - i - 1] = temp;
            }
        }
    }
};
