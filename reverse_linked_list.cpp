//A challenge from leetcode.
//Node: I've always used something like this:
//void reverse_list ( ListNode **node );
//That allows me to modify the root node without returning the modified list head.
//They must be calling this like so:
//list_head = reverseList ( list_head );
//Either way, it works.

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
class Solution {
public:
    ListNode* reverseList ( ListNode* head ) {
        if ( ! head )
            return (ListNode *) 0;

        ListNode *prev = head, *cur = head ->next, *next;
        int i_index = 0;

        //The old list's head node is the new ending node and needs to point to nothing.
        head ->next = 0;

        while ( cur ) {

          //Store the next pointer before we wipe it out.
          next = cur ->next;

          //Set the next pointer to be the previous node.
          cur ->next = prev;

          //Set the previous node to be the current, since we've already altered it.
          prev = cur;

          //Point the current node to the next node.
          cur = next;
        }

        //The new beginning of the list is the last node of the old list.
        head = prev;

        return head;
    }
    
};
