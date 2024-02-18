.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	a DW 1 DUP (0000H)
	arr DW 5 DUP (0000H)
.CODE
f PROC
	PUSH BP
	MOV BP,SP
L2:
	MOV AX,[BP+8]
	CALL print_output
	CALL new_line
L3:
	MOV AX,[BP+6]
	CALL print_output
	CALL new_line
L4:
	MOV AX,[BP+4]
	CALL print_output
	CALL new_line
L5:
	MOV AX,441
	PUSH AX
	MOV AX,1
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV SI,AX
	POP AX
	MOV arr[SI],AX
L6:
	MOV AX,555
	PUSH AX
	MOV AX,0
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV SI,AX
	POP AX
	MOV arr[SI],AX
L7:
	MOV AX,1
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV SI,AX
	PUSH arr[SI]
	POP AX
	MOV [BP+6],AX
L8:
	MOV AX,[BP+6]
	CALL print_output
	CALL new_line
	MOV AX,0
	PUSH AX
	MOV BX,2
	POP AX
	MUL BX
	MOV SI,AX
	MOV CX,arr[SI]
	jmp L1
L1:
	POP BP
	RET 6
recursive PROC
	PUSH BP
	MOV BP,SP
	PUSH [BP+4]
	MOV AX,1
	PUSH AX
	POP BX
	POP AX
	CMP AX,BX
	JE L10
	jmp L11
L10:
	MOV AX,1
	jmp L12
L11:
	MOV AX,0
L12:
	MOV BX,AX
	CMP BX,1
	JGE L13
	jmp L14
L13:
	MOV AX,1
	MOV CX,AX
	jmp L9
L14:
	PUSH [BP+4]
	MOV AX,0
	PUSH AX
	POP BX
	POP AX
	CMP AX,BX
	JE L15
	jmp L16
L15:
	MOV AX,1
	jmp L17
L16:
	MOV AX,0
L17:
	MOV BX,AX
	CMP BX,1
	JGE L18
	jmp L19
L18:
	MOV AX,0
	MOV CX,AX
	jmp L9
L19:
	PUSH [BP+4]
	MOV AX,1
	POP BX
	SUB BX,AX
	MOV AX,BX
	PUSH AX
	CALL recursive
	PUSH CX
	PUSH [BP+4]
	MOV AX,2
	POP BX
	SUB BX,AX
	MOV AX,BX
	PUSH AX
	CALL recursive
	POP AX
	ADD AX,CX
	MOV CX,AX
	jmp L9
L9:
	POP BP
	RET 2
v PROC
	PUSH BP
	MOV BP,SP
L21:
	MOV AX,3
	MOV a,AX
	MOV AX,a
	CMP AX,1
	JGE L22
	jmp L23
L22:
L24:
	SUB SP,2
L25:
	MOV AX,1
	MOV [BP-2],AX
L26:
	MOV AX,[BP-2]
	CALL print_output
	CALL new_line
L23:
L27:
	MOV AX,a
	CALL print_output
	CALL new_line
L20:
	ADD SP,2
	POP BP
	RET
v ENDP
main PROC
	MOV AX,@DATA
	MOV DS,AX
	PUSH BP
	MOV BP,SP
L29:
	SUB SP,2
	SUB SP,2
	SUB SP,2
	SUB SP,2
	SUB SP,10
L30:
	MOV AX,5
	MOV [BP-8],AX
	CALL v
L31:
	MOV AX,[BP-8]
	CALL print_output
	CALL new_line
L32:
	MOV AX,0
	MOV [BP-2],AX
L33:
	PUSH [BP-2]
	MOV AX,5
	PUSH AX
	POP BX
	POP AX
	CMP AX,BX
	JL L34
	jmp L35
L34:
	MOV AX,1
	jmp L36
L35:
	MOV AX,0
L36:
	CMP AX,1
	JGE L37
	jmp L38
L37:
L39:
	PUSH [BP-2]
	MOV AX,1
	POP BX
	ADD BX,AX
	PUSH BX
	PUSH [BP-2]
	MOV BX,2
	POP AX
	MUL BX
	MOV BX,18
	SUB BX,AX
	MOV SI,BX
	NEG SI
	POP AX
	MOV [BP+SI],AX
	MOV AX,[BP-2]
	ADD AX,1
	MOV [BP-2],AX
	SUB AX,1
	jmp L33
L38:
L40:
	MOV AX,4
	MOV [BP-2],AX
L41:
	MOV AX,[BP-2]
	SUB AX,1
	MOV [BP-2],AX
	ADD AX,1
	CMP AX,1
	JGE L42
	jmp L43
L42:
L44:
	PUSH [BP-2]
	MOV BX,2
	POP AX
	MUL BX
	MOV BX,18
	SUB BX,AX
	MOV SI,BX
	NEG SI
	PUSH [BP+SI]
	POP AX
	MOV [BP-4],AX
L45:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
	jmp L41
L43:
L46:
	MOV AX,2
	MOV [BP-6],AX
	PUSH [BP-6]
	MOV AX,0
	PUSH AX
	POP BX
	POP AX
	CMP AX,BX
	JG L47
	jmp L48
L47:
	MOV AX,1
	jmp L49
L48:
	MOV AX,0
L49:
	MOV BX,AX
	CMP BX,1
	JGE L50
	jmp L51
L50:
	MOV AX,[BP-6]
	ADD AX,1
	MOV [BP-6],AX
	SUB AX,1
	jmp L52
L51:
	MOV AX,[BP-6]
	SUB AX,1
	MOV [BP-6],AX
	ADD AX,1
L52:
L53:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L54:
	MOV AX,2
	MOV BX,AX
	NEG BX
	PUSH BX
	POP AX
	MOV [BP-6],AX
	PUSH [BP-6]
	MOV AX,0
	PUSH AX
	POP BX
	POP AX
	CMP AX,BX
	JL L55
	jmp L56
L55:
	MOV AX,1
	jmp L57
L56:
	MOV AX,0
L57:
	MOV BX,AX
	CMP BX,1
	JGE L58
	jmp L59
L58:
	MOV AX,[BP-6]
	ADD AX,1
	MOV [BP-6],AX
	SUB AX,1
	jmp L60
L59:
	MOV AX,[BP-6]
	SUB AX,1
	MOV [BP-6],AX
	ADD AX,1
L60:
L61:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L62:
	MOV AX,121
	MOV [BP-6],AX
L63:
	MOV AX,[BP-6]
	NEG AX
	MOV [BP-6],AX
L64:
	MOV AX,5
	MOV [BP-2],AX
L65:
	PUSH [BP-2]
	POP AX
	ADD AX,[BP-6]
	MOV [BP-6],AX
L66:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L67:
	MOV AX,4
	MOV BX,AX
	NEG BX
	PUSH BX
	POP AX
	MOV [BP-6],AX
L68:
	PUSH [BP-6]
	MOV AX,4
	PUSH AX
	POP BX
	POP AX
	MUL BX
	MOV [BP-6],AX
L69:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L70:
	MOV AX,19
	MOV [BP-4],AX
L71:
	MOV AX,4
	MOV [BP-2],AX
L72:
	PUSH [BP-4]
	PUSH [BP-2]
	POP BX
	POP AX
	XOR DX,DX
	DIV BX
	MOV [BP-6],AX
L73:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L74:
	PUSH [BP-4]
	PUSH [BP-2]
	POP BX
	POP AX
	XOR DX,DX
	DIV BX
	PUSH DX
	POP AX
	MOV [BP-6],AX
L75:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L76:
	MOV AX,111
	MOV BX,AX
	PUSH BX
	MOV BX,222
	MOV AX,BX
	PUSH AX
	MOV BX,333
	MOV AX,BX
	PUSH AX
	CALL f
	PUSH CX
	MOV AX,444
	POP BX
	SUB BX,AX
	PUSH BX
	POP AX
	MOV [BP-6],AX
L77:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L78:
	MOV AX,5
	MOV BX,AX
	PUSH BX
	CALL recursive
	PUSH CX
	POP AX
	MOV [BP-6],AX
L79:
	MOV AX,[BP-6]
	CALL print_output
	CALL new_line
L80:
	MOV AX,2
	MOV [BP-6],AX
L81:
	MOV AX,1
	MOV [BP-2],AX
L82:
	MOV AX,[BP-2]
	CMP AX,0
	JNE L84
	JMP L83
L83:
	MOV AX,[BP-6]
	CMP AX,0
	JNE L84
	JMP L85
L84:
	MOV AX,1
	JMP L86
L85:
	MOV AX,0
L86:
	MOV [BP-4],AX
L87:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L88:
	MOV AX,[BP-2]
	CMP AX,0
	JNE L89
	JMP L91
L89:
	MOV AX,[BP-6]
	CMP AX,0
	JNE L90
	JMP L91
L90:
	MOV AX,1
	JMP L92
L91:
	MOV AX,0
L92:
	MOV [BP-4],AX
L93:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L94:
	MOV AX,2
	MOV [BP-6],AX
L95:
	MOV AX,0
	MOV [BP-2],AX
L96:
	MOV AX,[BP-2]
	CMP AX,0
	JNE L98
	JMP L97
L97:
	MOV AX,[BP-6]
	CMP AX,0
	JNE L98
	JMP L99
L98:
	MOV AX,1
	JMP L100
L99:
	MOV AX,0
L100:
	MOV [BP-4],AX
L101:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L102:
	MOV AX,[BP-2]
	CMP AX,0
	JNE L103
	JMP L105
L103:
	MOV AX,[BP-6]
	CMP AX,0
	JNE L104
	JMP L105
L104:
	MOV AX,1
	JMP L106
L105:
	MOV AX,0
L106:
	MOV [BP-4],AX
L107:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L108:
	PUSH [BP-6]
	POP AX
	NOT AX
	MOV [BP-4],AX
L109:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L110:
	PUSH [BP-4]
	POP AX
	NOT AX
	MOV [BP-4],AX
L111:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
L112:
	MOV AX,12
	PUSH AX
	MOV AX,2
	PUSH AX
	MOV AX,89
	PUSH AX
	POP BX
	POP AX
	XOR DX,DX
	DIV BX
	PUSH AX
	MOV AX,3
	PUSH AX
	MOV AX,33
	POP BX
	SUB BX,AX
	PUSH BX
	MOV AX,64
	PUSH AX
	MOV AX,2
	PUSH AX
	POP BX
	POP AX
	MUL BX
	POP BX
	ADD BX,AX
	POP AX
	XOR DX,DX
	DIV BX
	POP AX
	ADD AX,DX
	PUSH AX
	MOV AX,3
	POP BX
	SUB BX,AX
	PUSH BX
	MOV AX,3
	PUSH AX
	MOV AX,59
	PUSH AX
	MOV AX,9
	PUSH AX
	POP BX
	POP AX
	XOR DX,DX
	DIV BX
	PUSH AX
	MOV AX,2
	PUSH AX
	POP BX
	POP AX
	MUL BX
	POP BX
	ADD BX,AX
	PUSH BX
	MOV AX,4
	POP BX
	SUB BX,AX
	POP AX
	ADD AX,BX
	MOV [BP-4],AX
L113:
	MOV AX,[BP-4]
	CALL print_output
	CALL new_line
	MOV AX,0
	MOV CX,AX
	jmp L28
L28:
	ADD SP,18
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
