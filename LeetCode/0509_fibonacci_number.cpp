class Solution
{
    vector<int> f = {1, 1};

public:
    int fib(int n)
    {
        if (n < f.size())
            return f[n];
        f.push_back(fib(n - 2) + fib(n - 1));
        return f[n];
    }
};
