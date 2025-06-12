class Solution
{
public:
    int addDigits(int num)
    {
        int temp, result = num;

        while (result >= 10)
        {
            temp = 0;
            while (result > 0)
            {
                temp += result % 10;
                result /= 10;
            }
            result = temp;
        }
        return result;
    }
};
