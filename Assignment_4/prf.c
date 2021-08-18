#include <stdio.h>
#include <unistd.h>
#include "elf64.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <stdarg.h>
#include <signal.h>

#define GLOBAL_FOUND 0
#define LOCAL_FOUND 1
#define NOT_FOUND 2


int NameLength(Elf64_Word index, Elf64_Shdr tableNameHeader, FILE* file){
	long stringOffset = tableNameHeader.sh_offset + index;
	char next;
	int counter = 0;

	fseek(file, stringOffset, SEEK_SET);
	next = fgetc(file);
	while (next != '\0' && next != EOF){
		counter++;
		next = fgetc(file);
	}
	return counter;
}

void FindStringIndex ( Elf64_Word index, Elf64_Shdr tableNameHeader, FILE* file, int length, char* stringName){
	long stringOffset = tableNameHeader.sh_offset + index;
	char next;

	fseek(file, stringOffset, SEEK_SET);
	next = fgetc(file);
	for (int i =0 ; i<length; ++i){
		stringName[i] = next;
		next = fgetc(file);
	}
	
}

void FindSectionHeaders(FILE* file, Elf64_Half numOfSectionHeaders, Elf64_Half stringsNameSectionIndex, Elf64_Off sectionHeaderOffset, Elf64_Shdr* symtabHeader, Elf64_Shdr* strtabHeader){
	Elf64_Shdr sectionHeader;
	Elf64_Off offset;

	for (int i = 0; i< numOfSectionHeaders; ++i){
		offset = sectionHeaderOffset + sizeof(Elf64_Shdr)*i;
		fseek(file, offset, SEEK_SET);
		fread(&sectionHeader, sizeof(Elf64_Shdr), 1, file);
		if(sectionHeader.sh_type == 2){
			*symtabHeader = sectionHeader;
		} else if (sectionHeader.sh_type == 3 && i!= stringsNameSectionIndex)
			*strtabHeader = sectionHeader;
	}
}

int GetFuncAddrFromSymtab (FILE* file, Elf64_Shdr symtabHeader, Elf64_Shdr strtabHeader, char* funcName, unsigned long* addr){
	Elf64_Sym entry;
	char* name;
	int len;
	Elf64_Off offset;
	if (symtabHeader.sh_entsize == 0)
		return NOT_FOUND;
	
	int entryCount = symtabHeader.sh_size / symtabHeader.sh_entsize;

	for (int i =0; i<entryCount; ++i){
		offset = symtabHeader.sh_offset + sizeof(Elf64_Sym) * i;
		fseek(file, offset, SEEK_SET);
		fread(&entry, sizeof(Elf64_Sym), 1, file);

		len = NameLength(entry.st_name, strtabHeader, file);
		name = (char*)malloc(sizeof(char) * len);
		FindStringIndex(entry.st_name, strtabHeader, file, len, name);
		//printf("name: %s != %s and size: %d\n",name, funcName,strlen(funcName));
		if (strcmp(name, funcName) == 0){
			if (ELF64_ST_BIND(entry.st_info) == 1){
				//printf("global\n");
				*addr = entry.st_value;
				free(name);
				return GLOBAL_FOUND;
			}else if (ELF64_ST_BIND(entry.st_info) == 0){
				//printf("local\n");
				free(name);
				return LOCAL_FOUND;
			}
		}
		free(name);
	}

	return NOT_FOUND;		
}

int GetFuncAddrByName(char* funcName, char* progName, unsigned long* addr)
{
	Elf64_Half numOfSections;
	Elf64_Half indexOfShstr;
	Elf64_Half sizeOfSectionEntry;
	Elf64_Off sectionsOffset;
	Elf64_Ehdr elfHeader;

	FILE* file = fopen(progName,"r");

	if (file == NULL)
		return -1;
		
	fread(&elfHeader, sizeof(Elf64_Ehdr),1, file);
	
	//fill out the variables
	numOfSections = elfHeader.e_shnum;
	indexOfShstr = elfHeader.e_shstrndx;
	sizeOfSectionEntry = elfHeader.e_shentsize;
	sectionsOffset = elfHeader.e_shoff;		


	Elf64_Shdr section_header;
	Elf64_Shdr strtabHeader;
	Elf64_Shdr symtabHeader;

	FindSectionHeaders(file, numOfSections, indexOfShstr,sectionsOffset, &symtabHeader, &strtabHeader);

	int successPredicate = GetFuncAddrFromSymtab(file, symtabHeader, strtabHeader, funcName, addr);

	fclose(file);

	return successPredicate;
}

long putBreak(pid_t child_pid, unsigned long addr){
	int ptrace_ret;
	errno = 0;
	long data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)addr, NULL);
	if (errno == -1)
		exit(1);
	
	unsigned long insertedData = (data & 0xFFFFFFFFFFFFFF00) | 0xCC;
	ptrace_ret = ptrace(PTRACE_POKETEXT, child_pid, (void*)addr, (void*)insertedData);
	if (ptrace_ret == -1)
		exit(1);

	return data;
}

void deleteBreakpoint(pid_t child_pid, unsigned long addr, long data){
	struct user_regs_struct regs;
	int ptrace_ret;

	ptrace_ret = ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	if (ptrace_ret == -1)
		exit(1);

	ptrace_ret = ptrace(PTRACE_POKETEXT, child_pid, (void*)addr, (void*)data);
	if (ptrace_ret == -1)
		exit(1);

	regs.rip = regs.rip -1;
	ptrace_ret = ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
	if (ptrace_ret == -1)
		exit(1);

}

void run_debugger(pid_t child_pid, unsigned long func_addr) {
	int wait_status;
	//printf("helllloooooo)");
	
	struct user_regs_struct regs;
	long data;
	int ptrace_ret;
	unsigned long frameStartAddr;
	unsigned char possibleOpcode;
	unsigned long funcEndAddr;

	wait(&wait_status);

	while (WIFSTOPPED(wait_status)){
		data = putBreak(child_pid, func_addr);
		ptrace_ret = ptrace(PTRACE_CONT, child_pid, NULL, NULL);
		
		if(ptrace_ret == -1){
			//printf("enter1");			
			exit(1);		
		}
		
		wait(&wait_status);

		if (WIFEXITED(wait_status)){
			deleteBreakpoint(child_pid, func_addr, data);
			break;
		}
		deleteBreakpoint(child_pid, func_addr, data);
		ptrace_ret = ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
		if(ptrace_ret == -1){
			//printf("enter2");			
			exit(1);		
		}
		errno = 0; 
		funcEndAddr = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)regs.rsp,NULL);
		if (errno == -1){
			//printf("errno99");
			exit(1);
		}
		data = putBreak(child_pid, funcEndAddr);
		frameStartAddr = regs.rsp + 8;

		while (WIFSTOPPED(wait_status)){
			ptrace_ret = ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
			if(ptrace_ret == -1){
				//printf("enter3");			
				exit(1);		
			}
			wait(&wait_status);
			ptrace_ret = ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
			if(ptrace_ret == -1){
				//printf("enter4");			
				exit(1);		
			}
			
			if (regs.rip -1 == funcEndAddr){
				if (frameStartAddr == regs.rsp){
					deleteBreakpoint(child_pid, funcEndAddr, data);
					break;
				}else{
					deleteBreakpoint(child_pid, funcEndAddr, data);
					//CHECK IS SYS
					int flag;
					pid_t child_pid_sys = child_pid;
					struct user_regs_struct regs_sys;
					int ptrace_ret_sys = ptrace(PTRACE_GETREGS, child_pid_sys, 0, &regs_sys);
					unsigned char possibleOpcode;
					if(ptrace_ret_sys == -1){
						//printf("enter5");			
						exit(1);		
					}
					errno = 0;
					possibleOpcode = ptrace(PTRACE_PEEKTEXT, child_pid_sys, (void*)regs_sys.rip, NULL);
					if (errno == -1){
						//printf("errno2");
						exit(1);
					}
					if (possibleOpcode != 0x0F)
						flag = 0;
					errno = 0;
					possibleOpcode = ptrace(PTRACE_PEEKTEXT, child_pid_sys, (void*)(regs_sys.rip + 1), NULL);
					if (errno == -1){
						//printf("errno1");
						exit(1);
					}
					if (possibleOpcode == 0x05)
						flag = 1;
					if (possibleOpcode != 0x05)
						flag = 0;
					//

					if (flag == 1){
						pid_t child_pid_new = child_pid;
						struct user_regs_struct new_regs;
						int wait_status_new;
						int ptrace_ret_new = ptrace(PTRACE_SYSCALL, child_pid_new, NULL, NULL);
						if(ptrace_ret_new == -1){
							//printf("enter11");			
							exit(1);		
						}

						wait(&wait_status_new);
						ptrace_ret_new = ptrace(PTRACE_SYSCALL, child_pid_new, NULL, NULL);
						if(ptrace_ret_new == -1){
							//printf("enter10");			
							exit(1);		
						}
						
						wait(&wait_status_new);
						ptrace_ret_new = ptrace(PTRACE_GETREGS, child_pid_new, 0, &new_regs);
						if(ptrace_ret_new == -1){
							//printf("enter6");			
							exit(1);		
						}
						long raxRetVal = new_regs.rax;
						unsigned long syscallAddr = new_regs.rip -2;
						if (raxRetVal < 0){
							printf("PRF:: syscall in %lx returned with %ld\n",syscallAddr, raxRetVal);
						}

					}else{	
						ptrace_ret = ptrace(PTRACE_SINGLESTEP, child_pid, 0,0);
						if(ptrace_ret == -1){
							//printf("enter7");			
							exit(1);		
						}
					}
					data = putBreak(child_pid, funcEndAddr);
					continue;
				}
			}
			ptrace_ret = ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
			if(ptrace_ret == -1){
				//printf("enter8");			
				exit(1);		
			}
			wait(&wait_status);

			ptrace_ret = ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
			if(ptrace_ret == -1){
				//printf("enter9");			
				exit(1);		
			}
			//printf("testing");
			long raxRetVal = regs.rax;
			unsigned long syscallAddr = regs.rip -2;
			if (raxRetVal < 0)
				printf("PRF:: syscall in %lx returned with %ld\n",syscallAddr, raxRetVal);
			
		}			
	}			
}

pid_t run_target(const char* programname, char** arguments) {
    pid_t pid;
    int exelcValue;
    pid = fork();
    if (pid > 0) {
        return pid;
    } else if (pid == 0) {
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("ptrace");
            exit(1);
        }
        exelcValue = execl(programname,programname,*arguments);
        if (exelcValue < 0) {
            perror("execl failed");
            exit(1);
        }
    } else {
        perror("fork");
        exit(1);
    }
}

int main (int argc, char *argv[])
{
	char* funcName = argv[1];
	char** arguments = (char **)(argv + 3);
	unsigned long functionAddr;
	int successPredicate = GetFuncAddrByName(argv[1], argv[2], &functionAddr);
	if (successPredicate == -1) {
		//printf("hell");
		exit(1);
	}
	if (successPredicate == NOT_FOUND) {
		printf("PRF:: not found!\n");
		return 1;
	}
	if (successPredicate == LOCAL_FOUND) {
		printf("PRF:: local found!\n");
		return 1;
	}

	pid_t child_pid;
	//printf("gothel");
	char* programName = argv[2];
	child_pid = run_target(programName, arguments);
	run_debugger(child_pid, functionAddr);
	return 0;
}

