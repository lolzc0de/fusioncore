#ifndef __LINKED_LIST_H_
#define __LINKED_LIST_H_

#include <stdint.h>
#include <stddef.h>

typedef struct slist {
	struct slist *next;
} slist_t;

// shamelessly stolen from Linux
#define container_of(ptr, type, member)                                        \
	({                                                                     \
		const typeof(((type *)0)->member) *__mptr = (ptr);             \
		type *__t = (type *)((char *)__mptr - offsetof(type, member)); \
		__mptr == NULL ? NULL : __t;                                   \
	})

// get the next element in a list
#define list_next(input, member)                                          \
	({                                                                \
		typeof(input) *_next_ = (input)->member.next;             \
		!_next_ ? NULL :                                          \
			  container_of(_next_, typeof(*(input)), member); \
	})

#define list_foreach(out, member, input)             \
	for (typeof(input) out = input; out != NULL; \
	     out = list_next(out, member))

#define list_append(node, member, input) \
	({ __list_append(&(input)->member, &(node)->member); })

static inline void __list_append(slist_t *head, slist_t *node)
{
	slist_t *prev = head;
	while (head)
		prev = head, head = head->next;

	prev->next = node;
}

#define list_set_next(node, member, input)                                 \
	({                                                                 \
		typeof(node) node_to_append = node;                        \
		slist_t *head = &(input)->member;                          \
		slist_t *next = !node_to_append ? NULL :                   \
						  &node_to_append->member; \
		head->next = next;                                         \
	})

#endif /* __LINKED_LIST_H_ */