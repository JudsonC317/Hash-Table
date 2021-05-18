/* MP7perfScript.c
 * Judson Cooper
 * MP7
 *
 * This file is a test script to automatically run the commands for the
 * performance evaluation of MP7.
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {

	// 1) 
	system("./lab7 -r -h linear -m 65537 -a 0.5"); 
	system("./lab7 -r -h linear -m 65537 -a 0.75"); 
	system("./lab7 -r -h linear -m 65537 -a 0.9"); 
	system("./lab7 -r -h linear -m 65537 -a 0.95"); 
	system("./lab7 -r -h linear -m 65537 -a 0.99"); 
	
	system("./lab7 -r -h double -m 65537 -a 0.5"); 
	system("./lab7 -r -h double -m 65537 -a 0.75"); 
	system("./lab7 -r -h double -m 65537 -a 0.9"); 
	system("./lab7 -r -h double -m 65537 -a 0.95"); 
	system("./lab7 -r -h double -m 65537 -a 0.99"); 

	system("./lab7 -r -h quad -m 65536 -a 0.5"); 
	system("./lab7 -r -h quad -m 65536 -a 0.75"); 
	system("./lab7 -r -h quad -m 65536 -a 0.9"); 
	system("./lab7 -r -h quad -m 65536 -a 0.95"); 
	system("./lab7 -r -h quad -m 65536 -a 0.99"); 

	// 2)
	system("./lab7 -r -h linear -m 65537 -a 0.85 -i rand"); 
	system("./lab7 -r -h linear -m 65537 -a 0.85 -i seq"); 
	system("./lab7 -r -h linear -m 65537 -a 0.85 -i fold"); 
	system("./lab7 -r -h linear -m 65537 -a 0.85 -i worst"); 
	
	system("./lab7 -r -h double -m 65537 -a 0.85 -i rand"); 
	system("./lab7 -r -h double -m 65537 -a 0.85 -i seq"); 
	system("./lab7 -r -h double -m 65537 -a 0.85 -i fold"); 
	system("./lab7 -r -h double -m 65537 -a 0.85 -i worst"); 

	system("./lab7 -r -h quad -m 65536 -a 0.85 -i rand"); 
	system("./lab7 -r -h quad -m 65536 -a 0.85 -i seq"); 
	system("./lab7 -r -h quad -m 65536 -a 0.85 -i fold"); 
	system("./lab7 -r -h quad -m 65536 -a 0.85 -i worst"); 

	// 3)
	system("./lab7 -e -m 65537 -t 50000");
	system("./lab7 -e -m 65537 -t 100000");
	
	return 0;
}
