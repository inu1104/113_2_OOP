/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution
{
    int gcd(int a, int b) { return (b == 0) ? a : gcd(b, a % b); }

public:
    ListNode *insertGreatestCommonDivisors(ListNode *head)
    {
        ListNode *result = new ListNode;
        ListNode *current = result;
        while (head->next)
        {
            ListNode *gcd_node = new ListNode;
            gcd_node->val = gcd(head->val, head->next->val);
            current->next = head;
            head = head->next;
            current = current->next;
            current->next = gcd_node;
            current = gcd_node;
        }
        current->next = head;
        current = result;
        result = result->next;
        delete current;
        return result;
    }
};
