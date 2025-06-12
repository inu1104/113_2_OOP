class Solution
{
public:
    int maximum69Number(int num)
    {
        int temp = num;
        int digit = -1, count = 0;
        while (temp > 0)
        {
            if (temp % 10 == 6)
                digit = count;
            temp /= 10;
            count++;
        }

        if (digit < 0)
            return num;

        temp = 1;
        for (int i = 0; i < digit; i++)
            temp *= 10;
        return num + 3 * temp;
    }
};
