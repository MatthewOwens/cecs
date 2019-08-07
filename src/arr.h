/*
 * generic, dynamic arrays based on the blog post here:
 * https://lkubuntu.wordpress.com/2017/01/26/the-perfect-c-array-library/
*/

#pragma once

#define array(type)						\
	struct {						\
		type* data;					\
		size_t length;					\
		size_t capacity;				\
	}							

#define array_init()						\
{								\
	.data = NULL,						\
	.length = 0,						\
	.capacity = 0						\
}

#define array_free(array)					\
	do {							\
		free(array.data);				\
		array.data = NULL;				\
		array.length = 0;				\
		array.capacity = 0;				\
	} while(0)					

#define array_push(array, element)				\
do {								\
	if (array.capacity == 0) {				\
		array.data = malloc(sizeof(*array.data));	\
		array.data[0] = element;			\
		array.capacity = 1;				\
		array.length = 1;				\
	} if (array.length == array.capacity) {			\
		array.capacity *= 2;				\
		type* tmp = reallocarray(array.data,		\
			array.capacity, sizeof(*array.data))	\
		if (tmp == NULL) return				\
		array.data = tmp;				\
		array.data[array.length] = element;		\
		array.length++;					\
} while(0)
