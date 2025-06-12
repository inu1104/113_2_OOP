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
    ListNode *rotateRight(ListNode *head, int k)
    {
        if(!head)
            return head;

        int nNode = 0;
        ListNode *tail = head;
        while (tail->next)
        {
            nNode++;
            tail = tail->next;
        }

        k %= (++nNode);

        ListNode *current = head;
        for (int i = 0; i < nNode - k - 1; i++)
            current = current->next;
        tail->next = head;
        head = current->next;
        current->next = nullptr;
        return head;
    }
};
