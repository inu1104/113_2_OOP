class Solution
{
public:
    string addBinary(string a, string b)
    {
        string result = "";
        int val = 0, carry = 0;

        auto it_a = a.rbegin();
        auto it_b = b.rbegin();
        while (it_a < a.rend() || it_b < b.rend() || carry != 0)
        {
            int val_a = 0, val_b = 0;
            if (it_a < a.rend())
                val_a = static_cast<int>(*it_a) - '0';
            if (it_b < b.rend())
                val_b = static_cast<int>(*it_b) - '0';

            int val = val_a + val_b + carry;
            carry = val / 2;
            val %= 2;
            if(val == 0)
                result = "0" + result;
            else if(val == 1)
                result = "1" + result;
            
            it_a++;
            it_b++;
        }
        return result;
    }
};
