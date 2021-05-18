# Hash-Table

This was a school project for one of my C programming courses.

This application provides an implementation of a hash table. It provides several test drivers
and settings to test the functionality of the hash table.

To run the application, run ./lab7 and follow with the flags for which driver and other 
settings you would like to use.

-r: run retrieve test driver.  
-b: run basic test driver.  
-u: run test plan driver for unit tests.  
-d: run deletion driver.  
-m x: table size 'x'.  
-a x: load factor 'x'.  
-h linear|double|quad: type of probing decrement.  
-e: run equilibrium test driver.   
-i rand|seq|fold|worse: type of keys for retrieve test driver.  
-t x: number of drials in drivers.  
-v: turn on verbose output.  
-s x: seed for random number generator.  

Below is a brief description of each file in the application. Further information can be found
in the header comment of each file. 

lab7.c: Contains the main loop, and drivers for testing the hash table application.  
table.c: Provides functions for creating and interacting with a hash table.  
MP7perfScript.c: test script to automatically run several drivers for performance evaluation.  
MP7testScript.c: test script to quickly test functionality of application with the main drivers.  
table.h: Function prototype declarations for table.c.  
makefile: Compiles the application.  
