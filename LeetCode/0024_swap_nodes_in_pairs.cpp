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
public:
    ListNode *swapPairs(ListNode *head)
    {
        ListNode *current, *next, *prev;
        current = head;
        while (current && current->next)
        {
            next = current->next;

            // swap current and next
            current->next = next->next;
            next->next = current;

            if (current == head)
                head = next;
            else
                prev->next = next;
            prev = current;
            current = current->next;
        }
        return head;
    }
};
