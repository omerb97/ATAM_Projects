This is split into two parts:

Part 1:
We needed to hack the readelf of a file. 
We add to the name of each section header the text "<3KIREL". 
We count the number of symbols in the symbol table that are global and print the number. 

Part 2:
Inserting a malicious file using the dynamic linker by using the fact that the hook is the first part of the program to run. 
