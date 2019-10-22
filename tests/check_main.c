#include <check.h>
#include <stdlib.h>
#include <stdint.h>
#include "runner.h"
#include "check_check.h"
#include "arr_check.h"
#include "cecs_check.h"

int main(void)
{
	int failed_count = 0;

	failed_count += run_suite(test_suite());
	failed_count += run_suite(arr_suite());
	failed_count += run_suite(cecs_suite());

	return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}