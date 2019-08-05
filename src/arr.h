/*
 * generic, dynamic arrays based on the blog post here:
 * https://lkubuntu.wordpress.com/2017/01/26/the-perfect-c-array-library/
*/

#pragma once

#define array(type)					\
	struct {					\
		type* data;				\
		size_t length;				\
	}						

#define array_init(){					\
	.data = NULL;					\
	.length = 0;					\
}

#define array_free(array){				\
	do {						\
		free(array.data);			\
		array.data = NULL;			\
		array.length = 0;			\
	} while(0)					\
}

#define array_push(array, element)			\
  do {							\
      array.data = realloc(array.data,			\
                           sizeof(*array.data) *	\
                             (array.lengthgth + 1));	\
      array.data[array.length] = element;		\
      array.length++;					\
  } while (0)

