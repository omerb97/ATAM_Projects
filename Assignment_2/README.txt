The project is divided into two parts.
Part 1:
A matrix multiplication calculator. Implemented in two functions. First function get_element_from_matrix lets us get a specific element from matrix.
Second function lets uses the first function to implement the matrix multiplication.
The functions adhere to the calling conventions on the Assembly x86-64 calling conventions.

Part 2:
Built a kernel module that handles and illegal instruction interrupt and sents the illegal opcode to an outside function called what_to_do.
The program continues or sends the interrupt to the default handler according to the return value of what_to_do.
