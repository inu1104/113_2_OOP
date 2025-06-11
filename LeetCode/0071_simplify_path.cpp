class Solution
{
    stack<string> argv;

public:
    string simplifyPath(string path)
    {
        size_t pos;
        string token;

        path += "/";
        pos = path.find("/");
        while (pos != string::npos)
        {
            token = path.substr(0, pos);
            if (!token.empty())
            {
                if (token == "..")
                {
                    if (!argv.empty())
                        argv.pop();
                }
                else if (token != ".")
                    argv.push(token);
            }
            path.erase(0, pos + 1);
            pos = path.find("/");
        }
        
        while (!argv.empty())
        {
            path = "/" + argv.top() + path;
            argv.pop();
        }

        return path.empty() ? "/" : path;
    }
};
