A debugger based on PTRACE the takes as an input the name of a function in a program and traces that program everytime there is a syscall. 
If there is a error in the syscall the debugger will print the error code.
If there is not function in the name that was given, the program will return not found.
If such a function exist but its local and not global the debugger will print that "local found".
In both those cases the program will stop running. 
