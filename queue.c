#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}


/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *curr = head->next->next;

    while (curr != head) {
        if (curr->prev == head) {
            break;
        }
        element_t *e = container_of(curr->prev, element_t, list);
        list_del(curr->prev);
        q_release_element(e);
        curr = curr->next;
    }
    element_t *e = container_of(curr->prev, element_t, list);
    list_del(curr->prev);
    q_release_element(e);
    list_del_init(head);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *element = malloc(sizeof(element_t));

    if (!element) {
        return false;
    }
    element->value = malloc(strlen(s) + 1);
    if (!element->value) {
        free(element);  // 缺少這行！
        return false;
    }
    strncpy(element->value, s, strlen(s) + 1);
    list_add(&(element->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *element = malloc(sizeof(element_t));

    if (!element) {
        return false;
    }

    element->value = malloc(strlen(s) + 1);
    if (!element->value) {
        return false;
    }
    strncpy(element->value, s, strlen(s) + 1);
    list_add_tail(&(element->list), head);
    return true;
}

/* Remove an element from head of queue */
// 不用free
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head) {
        return NULL;
    }

    struct list_head *temp;

    if (head->next != head) {
        // 取第一個元素
        element_t *element = list_first_entry(head, element_t, list);

        temp = head->next;
        head->next = temp->next;
        temp->next->prev = head;

        temp->prev = NULL;
        temp->next = NULL;
        return element;
    }
    return NULL;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head) {
        return NULL;
    }

    struct list_head *temp;

    if (head->prev != head) {
        // 取最後一個元素
        element_t *element = list_last_entry(head, element_t, list);

        temp = head->prev;
        head->prev = temp->prev;
        temp->prev->next = head;

        temp->prev = NULL;
        temp->next = NULL;

        return element;
    }

    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }

    struct list_head *node, *safe;
    int count = 0;
    list_for_each_safe(node, safe, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head) {
        return false;
    }
    int size = q_size(head);
    int mid = (size - 1) / 2;


    struct list_head *node, *safe;

    int count = 0;
    list_for_each_safe(node, safe, head) {
        count++;
        if (count == mid) {
            node->prev->next = safe;
            safe->prev = node->prev;
            node->next = NULL;
            node->prev = NULL;
            element_t *element = container_of(node, element_t, list);
            free(element->value);
            free(element);
            break;
        }
    }
    return true;
}

/* Delete all nodes that have duplicate string */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *curr = head->next;
    while (curr != head) {
        const element_t *e = container_of(curr, element_t, list);
        struct list_head *next = curr->next;

        // 這個點跟下一個點不一樣，就繼續下一輪
        if (next == head ||
            strcmp(e->value, container_of(next, element_t, list)->value) != 0) {
            curr = next;
            continue;
        }

        // 這個點跟下一個點一樣
        struct list_head *run = curr;
        while (run != head) {
            element_t *re = container_of(run, element_t, list);
            struct list_head *run_next = run->next;

            list_del(run);
            q_release_element(re);

            // 確認再下一個點有一樣嗎
            bool same =
                (run_next != head) &&
                strcmp(re->value,
                       container_of(run_next, element_t, list)->value) == 0;
            run = run_next;

            // 下一個點不一樣，不刪
            if (!same)
                break;
        }
        curr = run;
    }
    return true;
}



/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *prev = head;
    struct list_head *curr = head->next;
    struct list_head *next = curr->next;

    while (curr != head) {
        curr->next = curr->prev;
        curr->prev = next;

        prev = curr;
        curr = next;
        next = next->next;
    }

    head->next = prev;
    head->prev = next;
}

/* Reverse the nodes of the list k at a time */
// https://leetcode.com/problems/reverse-nodes-in-k-group/
void q_reverseK(struct list_head *head, int k)
{
    if (!head)
        return;
    int size = q_size(head);

    int group = size / k;

    struct list_head *tempHead = head;
    struct list_head *prev;
    struct list_head *curr = tempHead->next;
    struct list_head *next = curr->next;

    for (int i = 0; i < group; i++) {
        prev = tempHead->next;
        for (int j = 0; j < k; j++) {
            curr->next = curr->prev;
            curr->prev = next;

            curr = next;
            next = next->next;
        }

        tempHead->next = curr->prev;

        curr->prev = prev;


        tempHead->next->prev = tempHead;

        prev->next = curr;


        // 移動tempHead
        tempHead = prev;
    }
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    bool swapped = true;

    while (swapped) {
        struct list_head *curr = head->next;
        swapped = false;

        while (curr != head && curr->next != head) {
            const element_t *element = container_of(curr, element_t, list);
            const element_t *next_element =
                container_of(curr->next, element_t, list);

            int cmp = strcmp(element->value, next_element->value);
            bool should_swap = descend ? (cmp < 0) : (cmp > 0);

            if (should_swap) {
                struct list_head *next = curr->next;
                struct list_head *next_next = next->next;

                curr->next = next_next;
                next_next->prev = curr;
                next->next = curr;
                next->prev = curr->prev;
                curr->prev->next = next;
                curr->prev = next;

                swapped = true;
            }

            curr = curr->next;
        }
    }
}


int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *min = head->prev;
    struct list_head *curr = min->prev;

    while (curr != head) {
        const element_t *min_element = container_of(min, element_t, list);
        element_t *curr_element = container_of(curr, element_t, list);

        const char *min_value = min_element->value;
        char *curr_value = curr_element->value;

        int cmp = strcmp(min_value, curr_value);

        // min比較大
        if (cmp > 0) {
            min = curr;
            curr = curr->prev;
        } else {
            struct list_head *temp = curr->prev;
            free(curr_value);
            free(curr_element);
            curr = temp;
            curr->next = min;
            min->prev = curr;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *max = head->prev;
    struct list_head *curr = max->prev;

    while (curr != head) {
        const element_t *max_element = container_of(max, element_t, list);
        element_t *curr_element = container_of(curr, element_t, list);

        const char *max_value = max_element->value;
        char *curr_value = curr_element->value;

        int cmp = strcmp(max_value, curr_value);

        // max比較大
        if (cmp > 0) {
            struct list_head *temp = curr->prev;
            free(curr_value);
            free(curr_element);
            curr = temp;
            curr->next = max;
            max->prev = curr;
        } else {
            max = curr;
            curr = curr->prev;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
// https://leetcode.com/problems/merge-k-sorted-lists/
static void merge_two_queues(struct list_head *l1,
                             struct list_head *l2,
                             bool descend)
{
    LIST_HEAD(tmp);

    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *e1 = list_first_entry(l1, element_t, list);
        element_t *e2 = list_first_entry(l2, element_t, list);


        int cmp = strcmp(e1->value, e2->value);
        bool take_l2 = descend ? (cmp < 0) : (cmp > 0);

        // 把l1或l2的第一個點，一到tmp暫存串列
        list_move_tail(take_l2 ? l2->next : l1->next, &tmp);
    }

    // 因為其中一條串列已經是空的了，所以不用比較，直接接到尾巴即可
    list_splice_tail_init(l1, &tmp);
    list_splice_tail_init(l2, &tmp);
    list_splice_init(&tmp, l1);
}

int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    // 『第一條串列』
    queue_contex_t *first = container_of(head->next, queue_contex_t, chain);

    // 第一條要跟『第一條串列』比較的串列
    struct list_head *curr = head->next->next;

    // 一直用first跟別條串列合併
    while (curr != head) {
        queue_contex_t *other = container_of(curr, queue_contex_t, chain);
        merge_two_queues(first->q, other->q, descend);

        // 處理size
        first->size += other->size;
        other->size = 0;

        // curr換下一條串列
        curr = curr->next;
    }

    return first->size;
}
