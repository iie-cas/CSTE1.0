void set_rsp(){
	asm volatile("\
        call rcx;\
	add rsp,0x220;\
	ret;\
	");
}

void set_rax(){
	asm volatile("\
	call rcx;\
	pop rax;\
	ret;\
	");
}

void set_rdi(){
	asm volatile("\
	call rcx;\
	pop rdi;\
	ret;\
	");
}

void set_rsi(){
	asm volatile("\
	call rcx;\
	pop rsi;\
	ret;\
	");
}

void set_rdx(){
	asm volatile("\
	call rcx;\
	pop rdx;\
	ret;\
	");
}

void set_rcx(){
	asm volatile("\
	call rcx;\
	pop rcx;\
	ret;\
	");
}

void mov_rsi_rdi(){
	asm volatile("\
	call rcx;\
	mov [rsi], rdi; \
	ret; \
	");
}

void set_syscall(){
	asm volatile("\
	call rcx;\
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
