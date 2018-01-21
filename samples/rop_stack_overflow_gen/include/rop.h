void set_rsp(){
	asm volatile("\
	add rsp,0x220;\
	ret;\
	");
}

void set_rax(){
	asm volatile("\
	pop rax;\
	ret;\
	");
}

void set_rdi(){
	asm volatile("\
	pop rdi;\
	ret;\
	");
}

void set_rsi(){
	asm volatile("\
	pop rsi;\
	ret;\
	");
}

void set_rdx(){
	asm volatile("\
	pop rdx;\
	ret;\
	");
}

void set_rcx(){
	asm volatile("\
	pop rcx;\
	ret;\
	");
}

void mov_rsi_rdi(){
	asm volatile("\
	mov [rsi], rdi; \
	ret; \
	");
}

void set_syscall(){
	asm volatile("\
	syscall;\
	ret;\
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
