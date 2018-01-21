void set_rsp(){
	asm volatile("\
	add rsp,0x220;\
	pop rcx;\
	jmp rcx;\
	");
}

void set_rax(){
	asm volatile("\
	pop rax;\
	pop rcx;\
	jmp rcx;\
	");
}

void set_rdi(){
	asm volatile("\
	pop rdi;\
	pop rcx;\
	jmp rcx;\
	");
}

void set_rsi(){
	asm volatile("\
	pop rsi;\
	pop rcx;\
	jmp rcx;\
	");
}

void set_rdx(){
	asm volatile("\
	pop rdx;\
	pop rcx;\
	jmp rcx;\
	");
}

void set_rcx(){
	asm volatile("\
	pop rcx;\
	pop rcx;\
	jmp rcx;\
	");
}

void mov_rsi_rdi(){
	asm volatile("\
	mov [rsi], rdi; \
	pop rcx;\
	jmp rcx;\
	");
}

void set_syscall(){
	asm volatile("\
	syscall;\
	pop rcx;\
	jmp rcx;\
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
