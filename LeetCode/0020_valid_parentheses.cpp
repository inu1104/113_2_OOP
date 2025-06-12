class Solution
{
    stack<char> st;

public:
    bool isValid(string s)
    {
        unordered_map<char, int> left = {{'{', 0}, {'[', 1}, {'(', 2}};
        unordered_map<char, int> right = {{'}', 0}, {']', 1}, {')', 2}};

        for (auto it = s.begin(); it != s.end(); it++)
        {
            if (left.find(*it) != left.end())
                st.push(*it);
            else
            {
                if (st.empty())
                    return false;
                if(right.at(*it) != left.at(st.top()))
                    return false;
                st.pop();
            }
        }
        return st.empty();
    }
};
