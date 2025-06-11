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
typedef ListNode Node;

class Solution
{
public:
    Node *addTwoNumbers(Node *l1, Node *l2)
    {
        int val, carry = 0;
        Node *result = new Node;
        Node *current = result;
        while (l1 || l2 || carry != 0)
        {
            val = carry;
            if (l1)
            {
                val += l1->val;
                l1 = l1->next;
            }
            if (l2)
            {
                val += l2->val;
                l2 = l2->next;
            }
            carry = val / 10;
            val %= 10;

            Node *newNode = new Node;
            newNode->val = val;
            newNode->next = nullptr;
            current->next = newNode;
            current = newNode;
        }
        current = result;
        result = result->next;
        return result;
    }
};
