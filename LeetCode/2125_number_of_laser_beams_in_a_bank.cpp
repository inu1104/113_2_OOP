class Solution
{
public:
    int numberOfBeams(vector<string> &bank)
    {
        int last = 0, current, result = 0;
        for (auto it_v = bank.begin(); it_v < bank.end(); it_v++)
        {
            current = 0;
            for (auto it_s = it_v->begin(); it_s < it_v->end(); it_s++)
                current += (*it_s - '0');
            if (current > 0)
            {
                result += last * current;
                last = current;
            }
        }
        return result;
    }
};
