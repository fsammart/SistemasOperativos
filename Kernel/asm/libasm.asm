GLOBAL cpuVendor
GLOBAL outputb
GLOBAL inputb
GLOBAL timerTickHandler
EXTERN timerTickHandlerC
GLOBAL sti
GLOBAL keyboardHandler
EXTERN keyboardHandlerC
GLOBAL test
GLOBAL mouse_handler
EXTERN mouse_handlerC
GLOBAL sys_callHandler
EXTERN sys_call_writeC
EXTERN sys_call_readC
EXTERN sys_call_clearC
EXTERN sys_call_echoC
EXTERN sys_call_runC
EXTERN sys_call_changeModuleEnvironmetC
EXTERN sys_call_undoBackwardsC
GLOBAL cli
GLOBAL updateCR3
GLOBAL pageFaultHandler
EXTERN pageFaultHandlerC
GLOBAL generalProtectionHandler
EXTERN generalProtectionHandlerC
extern switchUserToKernel
extern switchKernelToUser
global finishStartup
extern getCurrentEntryPoint


GLOBAL master
GLOBAL slave

EXTERN spure

%macro	pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push fs
	push gs
%endmacro

%macro	popState 0
	pop gs
	pop fs
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

section .text


finishStartup:

call switchKernelToUser

mov rsp, rax

call getCurrentEntryPoint

sti

jmp rax


updateCR3:

	mov rax, cr3
	mov cr3, rax
	ret
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

inputb:
	push rbp
	mov rbp, rsp
	sub rsp, 0x04
	mov [rbp-0x04], edi
	mov dx, word[rbp-0x04]
	in al, dx
	mov rsp,rbp
	pop rbp
	ret

outputb:
	push rbp
	mov rbp, rsp
	sub rsp, 0x08
	mov[rbp-4], edi
	mov [rbp-8], esi
	mov dx, word[rbp-4]
	mov al, byte[rbp-8]
	out dx, al
	mov rsp, rbp
	pop rbp
	ret

sti:
	sti
	ret

keyboardHandler:
	cli
	push rbp
	mov rbp, rsp
	call keyboardHandlerC
	mov al, 20h
	out 20h, al

	mov rsp, rbp
	pop rbp
	sti
	iretq

timerTickHandler:
	push rbp
	mov rbp, rsp

	call timerTickHandlerC

	mov rsp, rbp
	pop rbp

	; scheduler part

	pushState
	
	; save current process's RSP
	mov rdi, rsp

	; enter kernel context by setting current process's kernel-RSP
	call switchUserToKernel
	;xchg bx, bx

	mov rsp, rax

	; schedule, get new process's RSP and load it
	call switchKernelToUser
	;xchg bx, bx

	mov rsp, rax

	mov al, 20h
	out 20h, al	

	popState
	iretq

pageFaultHandler:
	cli
	push rbp
	mov rbp, rsp
	call pageFaultHandlerC
	mov rsp, rbp
	pop rbp
	sti
	iretq

generalProtectionHandler:
	cli
	push rbp
	mov rbp, rsp
	call generalProtectionHandlerC
	mov rsp, rbp
	pop rbp
	sti
	iretq



sys_callHandler:
	cli
	push rbp
	mov rbp, rsp
	mov rdi,rbx
	mov rsi,rcx
	mov rdx,rdx
	cmp eax, 4
	jne read
	call sys_call_writeC
	jp finish
read:
	cmp eax,3
	jne clear
	call sys_call_readC
clear:
	cmp eax,5
	jne echo
	call sys_call_clearC
echo:
	cmp eax,6
	jne run
	mov rdi,rcx
	call sys_call_echoC
run:
	cmp eax, 7
	jne moduleEnvironment
	mov rdi,rcx
	call sys_call_runC
moduleEnvironment:
	cmp eax, 8
	jne undoBackwards
	mov rdi,rdx
	mov rsi,rcx
	call sys_call_changeModuleEnvironmetC
undoBackwards:
	cmp eax, 9
	jne finish
	mov rdi,rdx
	call sys_call_undoBackwardsC
finish:
	mov rdi,rax
	mov al, 20h
	out 20h, al
	mov rax,rdi	
	mov rsp, rbp
	pop rbp
	sti
	iretq


master:
	call spure
	mov al, 20h
	out 20h, al
	ret

slave:

	call spure
	mov al, 20h
	out 0A0h, al
	out 20h, al
	ret

test:
        mov al, 11101111b
	out 0A1h,al
	ret
    
mouse_handler:
	cli
	push rbp
	mov rbp, rsp
	call mouse_handlerC
	mov al, 0x20
		
	
	out 0xA0, al

	out 0x20, al

	mov rsp, rbp
	pop rbp
	sti
	iretq

cli:
	cli
	ret



section .data

msg db "hola mundo",0







