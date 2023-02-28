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

static struct list_head *merge(struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(container_of(a, element_t, list)->value,
                   container_of(b, element_t, list)->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

/*
 * Combine final list merge with restoration of standard doubly-linked
 * list structure.  This approach duplicates code from merge(), but
 * runs faster than the tidier alternatives of either a separate final
 * prev-link restoration pass, or maintaining the prev links
 * throughout.
 */

static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(container_of(a, element_t, list)->value,
                   container_of(b, element_t, list)->value) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */

        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

/**
 * list_sort - sort a list
 * @priv: private data, opaque to list_sort(), passed to @cmp
 * @head: the list to sort
 * @cmp: the elements comparison function
 *
 * The comparison function @cmp must return > 0 if @a should sort after
 * @b ("@a > @b" if you want an ascending sort), and <= 0 if @a should
 * sort before @b *or* their original order should be preserved.  It is
 * always called with the element that came first in the input in @a,
 * and list_sort is a stable sort, so it is not necessary to distinguish
 * the @a < @b and @a == @b cases.
 *
 * This is compatible with two styles of @cmp function:
 * - The traditional style which returns <0 / =0 / >0, or
 * - Returning a boolean 0/1.
 * The latter offers a chance to save a few cycles in the comparison
 * (which is used by e.g. plug_ctx_cmp() in block/blk-mq.c).
 *
 * A good way to write a multi-word comparison is::
 *
 *	if (a->high != b->high)
 *		return a->high > b->high;
 *	if (a->middle != b->middle)
 *		return a->middle > b->middle;
 *	return a->low > b->low;
 *
 *
 * This mergesort is as eager as possible while always performing at least
 * 2:1 balanced merges.  Given two pending sublists of size 2^k, they are
 * merged to a size-2^(k+1) list as soon as we have 2^k following elements.
 *
 * Thus, it will avoid cache thrashing as long as 3*2^k elements can
 * fit into the cache.  Not quite as good as a fully-eager bottom-up
 * mergesort, but it does use 0.2*n fewer comparisons, so is faster in
 * the common case that everything fits into L1.
 *
 *
 * The merging is controlled by "count", the number of elements in the
 * pending lists.  This is beautifully simple code, but rather subtle.
 *
 * Each time we increment "count", we set one bit (bit k) and clear
 * bits k-1 .. 0.  Each time this happens (except the very first time
 * for each bit, when count increments to 2^k), we merge two lists of
 * size 2^k into one list of size 2^(k+1).
 *
 * This merge happens exactly when the count reaches an odd multiple of
 * 2^k, which is when we have 2^k elements pending in smaller lists,
 * so it's safe to merge away two lists of size 2^k.
 *
 * After this happens twice, we have created two lists of size 2^(k+1),
 * which will be merged into a list of size 2^(k+2) before we create
 * a third list of size 2^(k+1), so there are never more than two pending.
 *
 * The number of pending lists of size 2^k is determined by the
 * state of bit k of "count" plus two extra pieces of information:
 *
 * - The state of bit k-1 (when k == 0, consider bit -1 always set), and
 * - Whether the higher-order bits are zero or non-zero (i.e.
 *   is count >= 2^(k+1)).
 *
 * There are six states we distinguish.  "x" represents some arbitrary
 * bits, and "y" represents some arbitrary non-zero bits:
 * 0:  00x: 0 pending of size 2^k;           x pending of sizes < 2^k
 * 1:  01x: 0 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * 2: x10x: 0 pending of size 2^k; 2^k     + x pending of sizes < 2^k
 * 3: x11x: 1 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * 4: y00x: 1 pending of size 2^k; 2^k     + x pending of sizes < 2^k
 * 5: y01x: 2 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * (merge and loop back to state 2)
 *
 * We gain lists of size 2^k in the 2->3 and 4->5 transitions (because
 * bit k-1 is set while the more significant bits are non-zero) and
 * merge them away in the 5->2 transition.  Note in particular that just
 * before the 5->2 transition, all lower-order bits are 11 (state 3),
 * so there is one list of each smaller size.
 *
 * When we reach the end of the input, we merge all the pending
 * lists, from smallest to largest.  If you work through cases 2 to
 * 5 above, you can see that the number of elements we merge with a list
 * of size 2^k varies from 2^(k-1) (cases 3 and 5 when x == 0) to
 * 2^(k+1) - 1 (second merge of case 5 when x == 2^(k-1) - 1).
 */

void list_sort(struct list_head *head)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    /*
     * Data structure invariants:
     * - All lists are singly linked and null-terminated; prev
     *   pointers are not maintained.
     * - pending is a prev-linked "list of lists" of sorted
     *   sublists awaiting further merging.
     * - Each of the sorted sublists is power-of-two in size.
     * - Sublists are sorted by size and age, smallest & newest at front.
     * - There are zero to two sublists of each size.
     * - A pair of pending sublists are merged as soon as the number
     *   of following pending elements equals their size (i.e.
     *   each time count reaches an odd multiple of that size).
     *   That ensures each later final merge will be at worst 2:1.
     * - Each round consists of:
     *   - Merging the two sublists selected by the highest bit
     *     which flips when count is incremented, and
     *   - Adding an element from the input as a size-1 sublist.
     */
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list);
}
