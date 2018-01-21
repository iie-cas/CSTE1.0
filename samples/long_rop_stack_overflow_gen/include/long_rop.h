void set_rsp(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	add rsp,0x220;\
	ret;\
	");
}

void set_rax(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	pop rax;\
	ret;\
	");
}

void set_rdi(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	pop rdi;\
	ret;\
	");
}

void set_rsi(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	pop rsi;\
	ret;\
	");
}

void set_rdx(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	pop rdx;\
	ret;\
	");
}

void set_rcx(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	pop rcx;\
	ret;\
	");
}

void mov_rsi_rdi(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
	mov [rsi], rdi; \
	ret;\
	");
}

void set_syscall(){
	asm volatile("\
	xor rbx,rbx;\
	inc rbx;\
	mov rcx,rbx;\
	add rcx,rbx;\
	push rcx;\
	pop rbx;\
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
