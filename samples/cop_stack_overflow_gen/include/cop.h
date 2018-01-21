void set_rsp(){
	asm volatile("\
        pop rbx;\
	add rsp,0x218;\
	pop rcx;\
	call rcx;\
	");
}

void set_rax(){
	asm volatile("\
	pop rbx;\
	pop rax;\
	pop rcx;\
	call rcx;\
	");
}

void set_rdi(){
	asm volatile("\
	pop rbx;\
	pop rdi;\
	pop rcx;\
	call rcx;\
	");
}

void set_rsi(){
	asm volatile("\
	pop rbx;\
	pop rsi;\
	pop rcx;\
	call rcx;\
	");
}

void set_rdx(){
	asm volatile("\
	pop rbx;\
	pop rdx;\
	pop rcx;\
	call rcx;\
	");
}

void set_rcx(){
	asm volatile("\
	pop rbx;\
	pop rax;\
	pop r8;\
	call r8;\
	");
}

void mov_rsi_rdi(){
	asm volatile("\
	pop rbx;\
	mov [rsi], rdi; \
	pop rcx;\
	call rcx;\
	");
}

void set_syscall(){
	asm volatile("\
	pop rbx;\
	syscall;\
	pop rcx;\
	call rcx;\
	");
}

void execute_all(){
	set_rax();
	set_rcx();
	set_rdx();
	set_rsi();
	set_rdi();
	mov_rsi_rdi();
	set_rsp();
	set_syscall();
}
