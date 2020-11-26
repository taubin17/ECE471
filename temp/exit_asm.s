
@ Syscall defines
.equ SYSCALL_EXIT,     1


        .globl _start
_start:

        @================================
        @ Exit
        @================================
exit:
	@ After reading manpage, r0 holds return value and r7 involkes syscall. Putting 42 in r0 will return 42.
	mov	r0, #42			@ move value 42 to return into r0
	mov	r7, #SYSCALL_EXIT	@ put exit() syscall number in r7
	swi	0			@ execute syscall
