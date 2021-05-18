/* MP7testScript.c
 * Judson Cooper
 * MP7
 *
 * This file is a test script to quickly test the functionality of MP7. It
 * automatically runs several test cases using different sets of command line
 * options for the program to verify that the output is correct.
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {

	// 1) run test plan driver
	system("./lab7 -u"); 

	// 2) show what happens when double hashing is run with small even number
	system("./lab7 -r -h double -m 8");
	// now show quadratic hashing with table size not power of 2
	system("./lab7 -r -h quad -m 15");

	// 3) run deletion driver
	system("./lab7 -d");

	// 4) show no memory leaks
	system("valgrind --leak-check=full ./lab7 -e");

	return 0;
}
