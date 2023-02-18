#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l) {
        while (!list_empty(l))
            q_release_element(q_remove_head(l, NULL, 0));
        free(l);
    }
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    int len = strlen(s);
    element->value = malloc((len + 1) * sizeof(char));
    if (!element->value) {
        free(element);
        return false;
    }
    strncpy(element->value, s, len);
    *(element->value + len) = '\0';
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    int len = strlen(s);
    element->value = malloc((len + 1) * sizeof(char));
    if (!element->value) {
        free(element);
        return false;
    }
    strncpy(element->value, s, len);
    *(element->value + len) = '\0';
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_first_entry(head, element_t, list);
    if (bufsize && sp) {
        strncpy(sp, element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    list_del_init(&element->list);
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_last_entry(head, element_t, list);
    if (bufsize && sp) {
        strncpy(sp, element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    list_del_init(&element->list);
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int count = 0;
    struct list_head *temp = malloc(sizeof(struct list_head));
    if (!temp)
        return 0;
    list_for_each (temp, head)
        count++;
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *forward = head->next;
    struct list_head *backward = head->prev;
    if (!forward)
        return false;
    if (!backward)
        return false;
    while (forward != backward && forward != backward->prev) {
        forward = forward->next;
        backward = backward->prev;
    }
    if (forward == backward) {
        list_del(forward);
        return true;
    }
    if (forward == backward->prev) {
        list_del(forward);
        return true;
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    bool diff = false;
    struct list_head *curr, *safe;
    list_for_each_safe (curr, safe, head) {
        struct list_head *next = curr->next;
        element_t *curr_entry = list_entry(curr, element_t, list);
        element_t *next_entry = list_entry(next, element_t, list);
        if ((next != head) && (!strcmp(curr_entry->value, next_entry->value))) {
            list_del(curr);
            q_release_element(curr_entry);
            diff = true;
        } else if (diff) {
            list_del(curr);
            q_release_element(curr_entry);
            diff = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *left = head->next;
    struct list_head *right = head->next->next;
    head->next = right;
    right->next->prev = left;
    left->next = right->next;
    right->next = left;
    right->prev = head;
    left->prev = right;
    while (left->next != head) {
        if (left->next->next != head) {
            right = left->next->next;
            right->prev->next = right->next;
            right->next->prev = right->prev;
            right->next = right->prev;
            right->prev->prev = right;
            right->prev = left;
            left->next = right;
            left = right->next;
        } else
            break;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
