.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
.CODE
main PROC
	MOV AX,@DATA
	MOV DS,AX
	PUSH BP
	MOV BP,SP
L2:
	SUB SP,2
	SUB SP,2
	SUB SP,6
L3:
	MOV AX,1
	PUSH AX
	MOV AX,2
	PUSH AX
	MOV AX,3
	POP BX
	ADD BX,AX
	POP AX
	MUL BX
	PUSH AX
	MOV AX,3
	PUSH AX
	POP BX
	POP AX
	XOR DX,DX
	DIV BX
	PUSH DX
	POP AX
	MOV [BP-2],AX
L4:
	MOV AX,1
	PUSH AX
	MOV AX,5
	PUSH AX
	POP BX
	POP AX
	CMP AX,BX
	JL L5
	jmp L6
L5:
	MOV AX,1
	jmp L7
L6:
	MOV AX,0
L7:
	MOV [BP-4],AX
L8:
	MOV AX,2
	PUSH AX
	MOV AX,0
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV BX,10
	SUB BX,AX
	MOV SI,BX
	NEG SI
	POP AX
	MOV [BP+SI],AX
	MOV AX,[BP-2]
	CMP AX,0
	JNE L9
	JMP L11
L9:
	MOV AX,[BP-4]
	CMP AX,0
	JNE L10
	JMP L11
L10:
	MOV AX,1
	JMP L12
L11:
	MOV AX,0
L12:
	MOV BX,AX
	CMP BX,1
	JGE L13
	jmp L14
L13:
	MOV AX,0
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV BX,10
	SUB BX,AX
	MOV SI,BX
	NEG SI
	MOV AX,[BP+SI]
	ADD AX,1
	MOV [BP+SI],AX
	SUB AX,1
	jmp L15
L14:
L16:
	MOV AX,0
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV BX,10
	SUB BX,AX
	MOV SI,BX
	NEG SI
	PUSH [BP+SI]
	MOV AX,1
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV BX,10
	SUB BX,AX
	MOV SI,BX
	NEG SI
	POP AX
	MOV [BP+SI],AX
L15:
L17:
	MOV AX,[BP-2]
	CALL print_output
	CALL new_line
L18:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L1:
	ADD SP,10
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP


;------print library-------;

new_line proc
    push ax
    push dx
    mov ah,2
    mov dl,0Dh
    int 21h
    mov ah,2
    mov dl,0Ah
    int 21h
    pop dx
    pop ax
    ret
    new_line endp
print_output proc  ;print what is in ax
    push ax
    push bx
    push cx
    push dx
    push si
    lea si,number
    mov bx,10
    add si,4
    cmp ax,0
    jnge negate
    print:
    xor dx,dx
    div bx
    mov [si],dl
    add [si],'0'
    dec si
    cmp ax,0
    jne print
    inc si
    lea dx,si
    mov ah,9
    int 21h
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret
    negate:
    push ax
    mov ah,2
    mov dl,'-'
    int 21h
    pop ax
    neg ax
    jmp print
    print_output endp
END main
