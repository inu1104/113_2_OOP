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
    ListNode *mergeTwoLists(ListNode *l1, ListNode *l2)
    {
        ListNode *result = new ListNode;
        ListNode *current = result;
        while (l1 && l2)
        {
            if (l1->val < l2->val)
            {
                current->next = l1;
                l1 = l1->next;
            }
            else
            {
                current->next = l2;
                l2 = l2->next;
            }
            current = current->next;
        }
        current->next = l1 ? l1 : l2;
        current = result;
        result = current->next;
        delete current;
        return result;
    }

    ListNode *merge(vector<ListNode *> &lists, int head, int rear)
    {
        if (head == rear)
            return lists[head];

        int middle = (head + rear) / 2;
        lists[head] = merge(lists, head, middle);
        lists[middle + 1] = merge(lists, middle + 1, rear);
        lists[head] = mergeTwoLists(lists[head], lists[middle + 1]);

        return lists[head];
    }

public:
    ListNode *mergeKLists(vector<ListNode *> &lists)
    {
        if (lists.empty())
            return nullptr;
        ListNode *result = merge(lists, 0, lists.size() - 1);
        return result;
    }
};
