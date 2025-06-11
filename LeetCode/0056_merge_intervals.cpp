class Solution
{
public:
    vector<vector<int>> merge(vector<vector<int>> &intervals)
    {
        vector<vector<int>> sol;
        sort(intervals.begin(), intervals.end()); // make sure that intervals are in increasing order
        int i = 0;
        while (i < intervals.size())
        {
            vector<int> interval = intervals[i];
            while (i + 1 < intervals.size() && interval[1] >= intervals[i + 1][0]){
                interval[0] = interval[0] > intervals[i + 1][0] ? intervals[i + 1][0] : interval[0];
                interval[1] = interval[1] < intervals[i + 1][1] ? intervals[i + 1][1] : interval[1];
                i++;
            }
            sol.push_back(interval);
            i++;
        }
        return sol;
    }
};
