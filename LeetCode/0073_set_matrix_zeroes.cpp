class Solution
{
public:
    void setZeroes(vector<vector<int>> &matrix)
    {
        unordered_set<int> reset_row;
        unordered_set<int> reset_col;
        for (int i = 0; i < matrix.size(); i++)
        {
            for (int j = 0; j < matrix[0].size(); j++)
            {
                if (matrix[i][j] == 0)
                {
                    reset_row.insert(i);
                    reset_col.insert(j);
                }
            }
        }

        for (int i = 0; i < matrix.size(); i++)
            for (int j = 0; j < matrix[0].size(); j++)
                if (reset_row.find(i) != reset_row.end() || reset_col.find(j) != reset_col.end())
                    matrix[i][j] = 0;
    }
};
