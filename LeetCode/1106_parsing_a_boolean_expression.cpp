class Solution
{
    stack<char> s;

public:
    bool parseBoolExpr(string expression)
    {
        auto it = expression.begin();
        while (it != expression.end())
        {
            if (*it == ')')
            {
                int nTrue = 0, nFalse = 0;
                do
                {
                    if (s.top() == 't')
                        nTrue++;
                    else if (s.top() == 'f')
                        nFalse++;
                    s.pop();
                } while (s.top() != '(');
                s.pop();

                switch (s.top())
                {
                case '!':
                    if (nFalse)
                        s.push('t');
                    else
                        s.push('f');
                    break;
                case '&':
                    if (nFalse)
                        s.push('f');
                    else
                        s.push('t');
                    break;
                case '|':
                    if (nTrue)
                        s.push('t');
                    else
                        s.push('f');
                    break;
                }
            }
            else
            {
                if (*it != ',')
                    s.push(*it);
            }
            it++;
        }

        return s.top() == 't';
    }
};
