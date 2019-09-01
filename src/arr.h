/*
 * generic, dynamic arrays based on the blog post here:
 * https://lkubuntu.wordpress.com/2017/01/26/the-perfect-c-array-library/
*/

#pragma once
#define array(type)\
	struct {\
		type* data;\
		size_t length;\
		size_t capacity;\
	}

/*
 * Can only be used at array declaration, since the macro is just a wrapper for
 * a struct {} assignment
*/
#define array_init()\
{\
	.data = NULL,\
	.length = 0,\
	.capacity = 0\
}

#define array_free(array)\
do {\
	free(array.data);\
	array.data = NULL;\
	array.length = 0;\
	array.capacity = 0;\
} while(0)

#define array_push(array, element)\
({\
	 printf("cap is %ld len is %ld\n", array.capacity, array.length);\
	 if (array.capacity == 0) {\
		 array.data = malloc(sizeof(*array.data));\
		 array.data[0] = element;\
		 array.capacity = 1;\
		 array.length = 1;\
	 } else {\
		 if (array.length == array.capacity) {\
		 array.capacity *= 2;\
		 \
		 void* tmp = reallocarray(array.data,\
			 array.capacity, sizeof(*array.data));\
			 if (tmp != NULL) {\
				 array.data = tmp;\
			 } else {\
				 fprintf(stderr, "arr.h - realloc failed!\n");\
				 return 1;\
			 }\
		 }\
		 array.data[array.length] = element;\
		 array.length += 1;\
	 }\
})
