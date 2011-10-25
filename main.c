#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// default array length
#define LENGTH 10

enum {
	DUMP_LIST = 0,
	LIST_METHOD,
	VALIDATE_LIST,
	ALL_OPTIONS
} OPTIONS;

enum {
	BUBBLE_SORT = 0,
	INSERTION_SORT,
	QUICK_SORT,
	BINARY_RADIX_SORT,
	ALL_SORTS
} SORTS;

enum {
	LIST_SEQUENTIAL = 0,
	LIST_RANDOM,
	LIST_SHUFFLE,
	ALL_LISTS
} LIST_TYPES;

typedef struct timer {
	clock_t start;
	double elapsed;
	int running;
} timer;

void init_timer (timer* t) {
	t->start = 0;
	t->elapsed = 0;
	int running = 0;
}

void begin_timer (timer* t) {
	t->start = clock();
	t->running = 1;
}

double stop_timer (timer* t) {
	double time = clock();
	t->running = 0;
	return t->elapsed += time - t->start;
}

// for speed it assumes a != b
// if this is false then the passed address will contain 0 after running
void inline xor_swap(int* a, int* b) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

void inline stupid_swap (int* a, int* b) {
	int buffer = *a;
	*a = *b;
	*b = buffer;
}

void print_list(int * list, int length) {
	int i;
	for (i = 0; i < length; i++)
		printf("%d: %8x\n", i, list[i]);
	printf("\n");
}

void check_list(int *list, int length) {
	int i;
	for (i = 0; i < length - 1; i++) {
		if (list[i] > list[i + 1]) {
			printf("List is out of order at element %d:\n%d - %d\n", i, list[i], list[i + 1]);
			print_list(list, length);
			return;
		}
	}
}

// returns the time taken
double bubble_sort (int* list, int length) {
	timer t;
	init_timer(&t);
	begin_timer(&t);

	int swapped = 1;
	int i;
	int buffer;
	int trigger = 0;
	while (swapped > 0) {
		swapped = 0;
		trigger = -1;
		for (i = (trigger > 0) ? trigger : 0; i < length - 1; i++) {
			if (list[i] > list[i + 1]) {
				xor_swap(&(list[i]), &(list[i+1]));
				swapped = 1;
				if (trigger < 0) {
					trigger = i;
				}
			}
		}
	}
	return stop_timer(&t);
}

double insertion_sort(int* list, int length) {
	timer t;
	init_timer(&t);
	begin_timer(&t);

	int i, j;
	int key;
	for (j = 1; j < length; j++) {
		key = list[j];
		i = j - 1;
		while (i >= 0 && list[i] > key) {
			list[i+1] = list[i];
			i = i - 1;
		}
		list[i+1] = key;
	}

	return stop_timer(&t);
}

/* In place MSD radix */
void binary_radix_sort_real(int *list, int length, int digit) {

	if (length <= 1 || digit + 1 >= sizeof(int) * 8)
		return;
	int *zerobound = list, *onebound = &list[length - 1];

	while(zerobound - onebound != 1) {
		if ((*zerobound >> (sizeof(int) * 8 - digit - 2)) & 0x01) {
			stupid_swap(zerobound, onebound);
			onebound--;
		} else {
			zerobound++;
		}
	}

	binary_radix_sort_real(list, zerobound - list, digit + 1);
	binary_radix_sort_real(onebound + 1, &list[length - 1] - onebound, digit + 1);
}

double binary_radix_sort(int *list, int length) {
	timer t;
	init_timer(&t);
	begin_timer(&t);

	binary_radix_sort_real(list, length, 0);

	return stop_timer(&t);
}

/*
   BROKEN RADIX SORT CODE

   int get_digit(int number, int base, int digit) {
   return (int) floor(number / pow(base, digit)) % base;
   }

   void radix(int byte, int N, int *source, int *dest)
   {
   int* count = malloc(N * sizeof(int));
   int* index = malloc(N * sizeof(int));
   int i;
   memset(count, 0, sizeof (count));

   for(i=0; i<N; ++i)
   count[(((unsigned)source[i])>>(byte*8))&0xff]++;

   index[0]=0;
   for(i=1; i<N; ++i)
   index[i] = index[i-1] + count[i-1];

   for(i=0; i<N; ++i)
   dest[index[(((unsigned) source[i])>>(byte*8))&0xff]++] = source[i];

   free(count);
   free(index);
   }

// requires base * length bytes
double lsdradix_sort(int* list, int length) {
timer t;
init_timer(&t);
begin_timer(&t);

int passes = logf(INT_MAX) / logf(2);
int* bucket, *buffer;
int* permabucket = bucket = malloc(sizeof(int) * length);
int i;

for (i = 0; i < passes; i++) {
radix(i, length, list, bucket);
buffer = list;
list = bucket;
bucket = buffer;
}

free(permabucket);
return stop_timer (&t);
}
 */

/*
   Very naive version, worst case scenario is actually a sorted list
 */
void quick_sort_r(int* list, int length) {
	int i;
	int pivot = list[0];
	int *lowlist = malloc(length * sizeof(int));
	int lowcount = 0;

	int *highlist = malloc(length * sizeof(int));
	int highcount = 0;

	if (highlist == NULL || lowlist == NULL) {
		printf("Ran out of memory :C\n");
		exit(1);
	}

	// first element is our pivot so start at index 1
	for (i = 1; i < length; i++) {
		if (list[i] > pivot)
			highlist[highcount++] = list[i];
		else
			lowlist[lowcount++] = list[i];
	}

	if (highcount > 1)
		quick_sort_r(highlist, highcount);
	if (lowcount > 1)
		quick_sort_r(lowlist, lowcount);

	memcpy(list, lowlist, lowcount * sizeof(int));
	list[lowcount] = pivot;
	memcpy(&list[lowcount + 1], highlist, highcount * sizeof(int));
	free(lowlist);
	free(highlist);
}

double quick_sort(int* list, int length) {
	timer t;
	init_timer(&t);
	begin_timer(&t);

	quick_sort_r(list, length);

	return stop_timer(&t);
}

void generate_random_list(int* list, int length) {
	unsigned int iseed = (unsigned int) time(NULL);
	srand(iseed);
	int i = 0;
	for (i = 0; i < length; i++) {
		list[i] = rand();
	}
}

void generate_sequential_list(int *list, int length) {
	int i;
	for (i = 0; i < length; i++)
		list[i] = i;
}

void generate_shuffled_list(int *list, int length) {
	int i, j;
	unsigned int iseed = (unsigned int) time(NULL);
	srand(iseed);
	generate_sequential_list(list, length);

	for (i = length - 1; i >= 0; i--) {
		j = rand() % length;
		stupid_swap(&list[i], &list[j]);
	}

}

void print_help(char** argv) {
	printf("Usage:\n\
			%s -l<length>\n", argv[0]);
}

int main (int argc, char** argv) {
	int length = LENGTH;

	// initializes all elements to 0 (even when the initializer has only one element)
	int options[ALL_OPTIONS] = {0};

	// function map
	double (*sort_functions[ALL_SORTS])(int*, int);
	char* function_names[ALL_SORTS];

	// silly macro, I feel bad about using this
#define addsort(bigname, littlename) sort_functions[bigname] = littlename;\
	function_names[bigname] = #littlename

	addsort(BUBBLE_SORT, bubble_sort);
	addsort(INSERTION_SORT, insertion_sort);
	addsort(QUICK_SORT, quick_sort);
	addsort(BINARY_RADIX_SORT, binary_radix_sort);

	// parse the command line options
	if (argc >= 2) {
		char option;
		while ((option = (char) getopt(argc, argv, "l:adg:c")) > 0) {
			switch (option) {
				case 'l' :
					length = strtol(optarg, NULL, 10);
					break;

				case 'd' :
					options[DUMP_LIST] = 1;
					break;

				case 'g' :
					switch (*optarg) {
						case 's':
							options[LIST_METHOD] = LIST_SEQUENTIAL;
							break;
						case 'r':
							options[LIST_METHOD] = LIST_RANDOM;
							break;
						case 'h':
							options[LIST_METHOD] = LIST_SHUFFLE;
							break;
					}
					break;

				case 'c' :
					options[VALIDATE_LIST] = 1;
					break;

			}
		}
	} else {
		print_help(argv);
		exit(0);
	}

	int* numbers = malloc(length * sizeof(int));
	int* templist = malloc(length * sizeof(int));
	int i;

	switch (options[LIST_METHOD]) {
		case LIST_SEQUENTIAL:
			generate_sequential_list(numbers, length);
			break;
		case LIST_RANDOM:
			generate_random_list(numbers, length);
			break;
		case LIST_SHUFFLE:
			generate_shuffled_list(numbers, length);
			break;
	}

	if (options[DUMP_LIST]) {
		print_list(numbers, length);
		exit(0);
	}

	for (i = 0; i < ALL_SORTS; i++) {
		memcpy(templist, numbers, length * sizeof(int));
		printf("%s: %.2fs\n",function_names[i] , (*sort_functions[i])(templist, length) / CLOCKS_PER_SEC);
		if (options[VALIDATE_LIST])
			check_list(templist, length);
	}
	return 0;

}
