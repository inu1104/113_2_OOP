class Solution
{
public:
    vector<int> twoSum(vector<int> &nums, int target)
    {
        vector<int> sol;
        unordered_map<int, int> complement; // key: target - nums[i], val:i
        bool found = false;
        for (int i = 0; i < nums.size(); i++)
            complement.insert({target - nums[i], i});
        
        for (int i = 0; i < nums.size(); i++){
            auto it = complement.find(nums[i]);
            if(it != complement.end() && i != it->second){
                sol.push_back(i);
                sol.push_back(it->second);
                break;
            }
        } 
        return sol;
    }
};
