.MODEL SMALL
.STACK 1000H
.Data
	number DB "00000$"
	a DW 1 DUP (0000H)
	arr DW 5 DUP (0000H)
.CODE
f PROC
	PUSH BP
	MOV BP, SP
	MOV AX, [BP+8]       ; Line 5
	CALL print_output
	CALL new_line
L1:
	MOV AX, [BP+6]       ; Line 6
	CALL print_output
	CALL new_line
L2:
	MOV AX, [BP+4]       ; Line 7
	CALL print_output
	CALL new_line
L3:
	MOV AX, 1       ; Line 8
	PUSH AX
	MOV AX, 441       ; Line 8
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	POP AX
	MOV arr[BX], AX
	PUSH AX
	POP AX
L4:
	MOV AX, 0       ; Line 9
	PUSH AX
	MOV AX, 555       ; Line 9
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	POP AX
	MOV arr[BX], AX
	PUSH AX
	POP AX
L5:
	MOV AX, 1       ; Line 10
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 10
	MUL BX
	MOV BX, AX
	MOV AX, arr[BX]
	MOV [BP+6], AX
	PUSH AX
	POP AX
L6:
	MOV AX, [BP+6]       ; Line 11
	CALL print_output
	CALL new_line
L7:
	MOV AX, 0       ; Line 12
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 12
	MUL BX
	MOV BX, AX
	MOV AX, arr[BX]
	JMP L9
L8:
L9:
	POP BP
	RET 6
f ENDP
recursive PROC
	PUSH BP
	MOV BP, SP
	MOV AX, 1       ; Line 16
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 16
	CMP AX, DX
	JE L10
	JMP L11
L10:
	MOV AX, 1       ; Line 17
	JMP L15
L11:
	MOV AX, 0       ; Line 18
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 18
	CMP AX, DX
	JE L12
	JMP L13
L12:
	MOV AX, 0       ; Line 19
	JMP L15
L13:
	MOV AX, 1       ; Line 20
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 20
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 20
	PUSH AX
	CALL recursive
	PUSH AX
	MOV AX, 2       ; Line 20
	MOV DX, AX
	MOV AX, [BP+4]       ; Line 20
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 20
	PUSH AX
	CALL recursive
	PUSH AX
	POP AX       ; Line 20
	MOV DX, AX
	POP AX       ; Line 20
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 20
	JMP L15
L14:
L15:
	POP BP
	RET 2
recursive ENDP
v PROC
	PUSH BP
	MOV BP, SP
	MOV AX, 3       ; Line 25
	MOV a, AX
	PUSH AX
	POP AX
L16:
L17:
	SUB SP, 2
L18:
	MOV AX, 1       ; Line 29
	MOV [BP-2], AX
	PUSH AX
	POP AX
L19:
	MOV AX, [BP-2]       ; Line 30
	CALL print_output
	CALL new_line
L20:
L21:
	MOV AX, a       ; Line 32
	CALL print_output
	CALL new_line
L22:
L23:
	ADD SP, 2
	POP BP
	RET 
v ENDP
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 10
L24:
	MOV AX, 5       ; Line 38
	MOV [BP-8], AX
	PUSH AX
	POP AX
L25:
	CALL v       ; Line 39
	PUSH AX
	POP AX
L26:
	MOV AX, [BP-8]       ; Line 40
	CALL print_output
	CALL new_line
L27:
	MOV AX, 0       ; Line 41
	MOV [BP-2], AX
	PUSH AX
	POP AX
L28:
	MOV AX, 5       ; Line 41
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 41
	CMP AX, DX
	JL L30
	JMP L32
L29:
	MOV AX, [BP-2]       ; Line 41
	PUSH AX
	INC AX
	MOV [BP-2], AX
	POP AX
	JMP L28
L30:
	MOV AX, [BP-2]       ; Line 43
	PUSH AX
	MOV AX, 1       ; Line 43
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 43
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 43
	POP BX
	PUSH AX
	MOV AX, 2
	MUL BX
	MOV BX, AX
	MOV AX, 18
	SUB AX, BX
	MOV BX, AX
	POP AX
	MOV SI, BX
	NEG SI
	MOV [BP+SI], AX
	PUSH AX
	POP AX
L31:
	JMP L29
L32:
	MOV AX, 4       ; Line 45
	MOV [BP-2], AX
	PUSH AX
	POP AX
L33:
L34:
	MOV AX, [BP-2]       ; Line 46
	PUSH AX
	DEC AX
	MOV [BP-2], AX
	POP AX       ; Line 46
	CMP AX, 0
	JNE L35
	JMP L38
L35:
	MOV AX, [BP-2]       ; Line 48
	PUSH AX
	POP BX
	MOV AX, 2       ; Line 48
	MUL BX
	MOV BX, AX
	MOV AX, 18
	SUB AX, BX
	MOV BX, AX
	MOV SI, BX
	NEG SI
	MOV AX, [BP+SI]
	MOV [BP-4], AX
	PUSH AX
	POP AX
L36:
	MOV AX, [BP-4]       ; Line 49
	CALL print_output
	CALL new_line
L37:
	JMP L34
L38:
	MOV AX, 2       ; Line 51
	MOV [BP-6], AX
	PUSH AX
	POP AX
L39:
	MOV AX, 0       ; Line 52
	MOV DX, AX
	MOV AX, [BP-6]       ; Line 52
	CMP AX, DX
	JG L40
	JMP L41
L40:
	MOV AX, [BP-6]       ; Line 53
	PUSH AX
	INC AX
	MOV [BP-6], AX
	POP AX
	JMP L42
L41:
	MOV AX, [BP-6]       ; Line 55
	PUSH AX
	DEC AX
	MOV [BP-6], AX
	POP AX
L42:
	MOV AX, [BP-6]       ; Line 56
	CALL print_output
	CALL new_line
L43:
	MOV AX, 2       ; Line 57
	NEG AX
	PUSH AX
	POP AX       ; Line 57
	MOV [BP-6], AX
	PUSH AX
	POP AX
L44:
	MOV AX, 0       ; Line 58
	MOV DX, AX
	MOV AX, [BP-6]       ; Line 58
	CMP AX, DX
	JL L45
	JMP L46
L45:
	MOV AX, [BP-6]       ; Line 59
	PUSH AX
	INC AX
	MOV [BP-6], AX
	POP AX
	JMP L47
L46:
	MOV AX, [BP-6]       ; Line 61
	PUSH AX
	DEC AX
	MOV [BP-6], AX
	POP AX
L47:
	MOV AX, [BP-6]       ; Line 62
	CALL print_output
	CALL new_line
L48:
	MOV AX, 121       ; Line 63
	MOV [BP-6], AX
	PUSH AX
	POP AX
L49:
	MOV AX, [BP-6]       ; Line 64
	NEG AX
	PUSH AX
	POP AX       ; Line 64
	MOV [BP-6], AX
	PUSH AX
	POP AX
L50:
	MOV AX, 5       ; Line 65
	MOV [BP-2], AX
	PUSH AX
	POP AX
L51:
	MOV AX, [BP-6]       ; Line 66
	MOV DX, AX
	MOV AX, [BP-2]       ; Line 66
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 66
	MOV [BP-6], AX
	PUSH AX
	POP AX
L52:
	MOV AX, [BP-6]       ; Line 67
	CALL print_output
	CALL new_line
L53:
	MOV AX, 4       ; Line 68
	NEG AX
	PUSH AX
	POP AX       ; Line 68
	MOV [BP-6], AX
	PUSH AX
	POP AX
L54:
	MOV AX, 4       ; Line 69
	MOV CX, AX
	MOV AX, [BP-6]       ; Line 69
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 69
	MOV [BP-6], AX
	PUSH AX
	POP AX
L55:
	MOV AX, [BP-6]       ; Line 70
	CALL print_output
	CALL new_line
L56:
	MOV AX, 19       ; Line 71
	MOV [BP-4], AX
	PUSH AX
	POP AX
L57:
	MOV AX, 4       ; Line 72
	MOV [BP-2], AX
	PUSH AX
	POP AX
L58:
	MOV AX, [BP-2]       ; Line 73
	MOV CX, AX
	MOV AX, [BP-4]       ; Line 73
	CWD
	DIV CX
	PUSH AX
	POP AX       ; Line 73
	MOV [BP-6], AX
	PUSH AX
	POP AX
L59:
	MOV AX, [BP-6]       ; Line 74
	CALL print_output
	CALL new_line
L60:
	MOV AX, [BP-2]       ; Line 75
	MOV CX, AX
	MOV AX, [BP-4]       ; Line 75
	CWD
	DIV CX
	PUSH DX
	POP AX       ; Line 75
	MOV [BP-6], AX
	PUSH AX
	POP AX
L61:
	MOV AX, [BP-6]       ; Line 76
	CALL print_output
	CALL new_line
L62:
	MOV AX, 111       ; Line 77
	PUSH AX
	MOV AX, 222       ; Line 77
	PUSH AX
	MOV AX, 333       ; Line 77
	PUSH AX
	CALL f
	PUSH AX
	MOV AX, 444       ; Line 77
	MOV DX, AX
	POP AX       ; Line 77
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 77
	MOV [BP-6], AX
	PUSH AX
	POP AX
L63:
	MOV AX, [BP-6]       ; Line 78
	CALL print_output
	CALL new_line
L64:
	MOV AX, 5       ; Line 79
	PUSH AX
	CALL recursive
	PUSH AX
	POP AX       ; Line 79
	MOV [BP-6], AX
	PUSH AX
	POP AX
L65:
	MOV AX, [BP-6]       ; Line 80
	CALL print_output
	CALL new_line
L66:
	MOV AX, 2       ; Line 81
	MOV [BP-6], AX
	PUSH AX
	POP AX
L67:
	MOV AX, 1       ; Line 82
	MOV [BP-2], AX
	PUSH AX
	POP AX
L68:
	MOV AX, [BP-2]       ; Line 83
	CMP AX, 0
	JNE L70
	JMP L69
L69:
	MOV AX, [BP-6]       ; Line 83
	CMP AX, 0
	JNE L70
	JMP L72
L70:
	MOV AX, 1       ; Line 83
	JMP L71
L72:
	MOV AX, 0
L71:
	MOV [BP-4], AX
	PUSH AX
	POP AX
L73:
	MOV AX, [BP-4]       ; Line 84
	CALL print_output
	CALL new_line
L74:
	MOV AX, [BP-2]       ; Line 85
	CMP AX, 0
	JNE L75
	JMP L78
L75:
	MOV AX, [BP-6]       ; Line 85
	CMP AX, 0
	JNE L76
	JMP L78
L76:
	MOV AX, 1       ; Line 85
	JMP L77
L78:
	MOV AX, 0
L77:
	MOV [BP-4], AX
	PUSH AX
	POP AX
L79:
	MOV AX, [BP-4]       ; Line 86
	CALL print_output
	CALL new_line
L80:
	MOV AX, 2       ; Line 87
	MOV [BP-6], AX
	PUSH AX
	POP AX
L81:
	MOV AX, 0       ; Line 88
	MOV [BP-2], AX
	PUSH AX
	POP AX
L82:
	MOV AX, [BP-2]       ; Line 89
	CMP AX, 0
	JNE L84
	JMP L83
L83:
	MOV AX, [BP-6]       ; Line 89
	CMP AX, 0
	JNE L84
	JMP L86
L84:
	MOV AX, 1       ; Line 89
	JMP L85
L86:
	MOV AX, 0
L85:
	MOV [BP-4], AX
	PUSH AX
	POP AX
L87:
	MOV AX, [BP-4]       ; Line 90
	CALL print_output
	CALL new_line
L88:
	MOV AX, [BP-2]       ; Line 91
	CMP AX, 0
	JNE L89
	JMP L92
L89:
	MOV AX, [BP-6]       ; Line 91
	CMP AX, 0
	JNE L90
	JMP L92
L90:
	MOV AX, 1       ; Line 91
	JMP L91
L92:
	MOV AX, 0
L91:
	MOV [BP-4], AX
	PUSH AX
	POP AX
L93:
	MOV AX, [BP-4]       ; Line 92
	CALL print_output
	CALL new_line
L94:
	MOV AX, [BP-6]       ; Line 93
	NOT AX
	PUSH AX
	POP AX       ; Line 93
	MOV [BP-4], AX
	PUSH AX
	POP AX
L95:
	MOV AX, [BP-4]       ; Line 94
	CALL print_output
	CALL new_line
L96:
	MOV AX, [BP-4]       ; Line 95
	NOT AX
	PUSH AX
	POP AX       ; Line 95
	MOV [BP-4], AX
	PUSH AX
	POP AX
L97:
	MOV AX, [BP-4]       ; Line 96
	CALL print_output
	CALL new_line
L98:
	MOV AX, 89       ; Line 97
	MOV CX, AX
	MOV AX, 2       ; Line 97
	CWD
	DIV CX
	PUSH AX
	MOV AX, 33       ; Line 97
	MOV DX, AX
	MOV AX, 3       ; Line 97
	SUB AX, DX
	PUSH AX
	MOV AX, 2       ; Line 97
	MOV CX, AX
	MOV AX, 64       ; Line 97
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 97
	MOV DX, AX
	POP AX       ; Line 97
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 97
	MOV CX, AX
	POP AX       ; Line 97
	CWD
	DIV CX
	PUSH DX
	POP AX       ; Line 97
	MOV DX, AX
	MOV AX, 12       ; Line 97
	ADD AX, DX
	PUSH AX
	MOV AX, 3       ; Line 97
	MOV DX, AX
	POP AX       ; Line 97
	SUB AX, DX
	PUSH AX
	MOV AX, 9       ; Line 97
	MOV CX, AX
	MOV AX, 59       ; Line 97
	CWD
	DIV CX
	PUSH AX
	MOV AX, 2       ; Line 97
	MOV CX, AX
	POP AX       ; Line 97
	CWD
	MUL CX
	PUSH AX
	POP AX       ; Line 97
	MOV DX, AX
	MOV AX, 3       ; Line 97
	ADD AX, DX
	PUSH AX
	MOV AX, 4       ; Line 97
	MOV DX, AX
	POP AX       ; Line 97
	SUB AX, DX
	PUSH AX
	POP AX       ; Line 97
	MOV DX, AX
	POP AX       ; Line 97
	ADD AX, DX
	PUSH AX
	POP AX       ; Line 97
	MOV [BP-4], AX
	PUSH AX
	POP AX
L99:
	MOV AX, [BP-4]       ; Line 98
	CALL print_output
	CALL new_line
L100:
	MOV AX, 0       ; Line 100
	JMP L102
L101:
L102:
	ADD SP, 18
	POP BP
	MOV AX,4CH
	INT 21H
main ENDP
;-------------------------------
;         print library         
;-------------------------------
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
;-------------------------------
END main