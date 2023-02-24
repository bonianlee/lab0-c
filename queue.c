#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Function prototypes
 */
struct list_head *merge_sort(struct list_head *head);
struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2);

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
    struct list_head *temp = NULL;
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
        element_t *element = list_first_entry(forward->prev, element_t, list);
        list_del(forward);
        q_release_element(element);
        return true;
    }
    if (forward == backward->prev) {
        element_t *element = list_first_entry(forward, element_t, list);
        list_del(backward);
        q_release_element(element);
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
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *node = NULL;
    struct list_head *safe = NULL;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    if (k < 2)
        return;
    else if (k == 2)
        q_swap(head);
    else {
        int count = 0;
        struct list_head *node = NULL;
        list_for_each (node, head)
            count++;
        if (count < k)
            return;
        else {
            int repeat_times = count / k;
            LIST_HEAD(dummy);
            struct list_head *head_temp = &dummy;
            struct list_head *sub_head = head;
            struct list_head *cut_pos = head;
            for (int i = 0; i < repeat_times; i++) {
                for (int j = 0; j < k; j++)
                    cut_pos = cut_pos->next;
                list_cut_position(head_temp, sub_head, cut_pos);
                q_reverse(head_temp);
                list_splice_init(head_temp, sub_head);
                for (int j = 0; j < k; j++)
                    sub_head = sub_head->next;
                cut_pos = sub_head;
            }
        }
    }
    return;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    head->prev->next = NULL;
    head->next = merge_sort(head->next);
    struct list_head *curr = head, *next = curr->next;
    while (next) {
        next->prev = curr;
        curr = next;
        next = next->next;
    }
    curr->next = head;
    head->prev = curr;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    // reference to @KHLee529
    char *max = NULL;
    element_t *entry = NULL, *safe = NULL;
    int total = 0, n_del = 0;
    for (entry = list_entry(head->prev, element_t, list),
        safe = list_entry(entry->list.prev, element_t, list);
         &entry->list != head;
         entry = safe, safe = list_entry(safe->list.prev, element_t, list)) {
        total += 1;
        if (!max || strcmp(entry->value, max) > 0) {
            max = entry->value;
        } else {
            list_del(&entry->list);
            q_release_element(entry);
            n_del += 1;
        }
    }
    return total - n_del;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    // reference to @chiangkd
    if (!head)
        return 0;
    queue_contex_t *c_cont;
    queue_contex_t *n_cont;
    struct list_head *sorted = NULL;

    list_for_each_entry_safe (c_cont, n_cont, head, chain) {  // iterate context
        c_cont->q->prev->next = NULL;
        c_cont->q->prev = NULL;
        sorted = mergeTwoLists(sorted, c_cont->q->next);
        INIT_LIST_HEAD(c_cont->q);  // reconnect the lists which are moved and
                                    // merged to "sorted" list;
    }
    LIST_HEAD(tmp);
    struct list_head *t = &tmp;
    t->next = sorted;
    struct list_head *c = t;
    while (sorted) {
        sorted->prev = c;
        c = sorted;
        sorted = sorted->next;
    }
    c->next = t;
    t->prev = c;
    int size = q_size(t);  // store size before splice to main queue
    list_splice(t, list_first_entry(head, queue_contex_t, chain)->q);
    return size;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *slow = head;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }
    struct list_head *mid = slow->next;
    slow->next = NULL;
    struct list_head *left = merge_sort(head), *right = merge_sort(mid);
    return mergeTwoLists(left, right);
}

/*
 * Merge two sublists to one sorted list
 */
struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node = NULL;
    while (L1 && L2) {
        element_t *L1_entry = list_entry(L1, element_t, list);
        element_t *L2_entry = list_entry(L2, element_t, list);
        node = strcmp(L1_entry->value, L2_entry->value) < 0 ? &L1 : &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
        *node = (*node)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}