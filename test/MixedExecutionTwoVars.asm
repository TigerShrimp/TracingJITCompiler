; Run with 23 5 23
ENTER 0,0
; Init code
MOV RAX, [RDI+8]
MOV RAX, [RAX]
MOV RSI, [RDI+16]
MOV RSI, [RSI]
; Start of trace (pc = 5)
MOV R9, RSI 
IMUL R9, 3
ADD RAX, R9 
IMUL RSI, 6
SUB RSI, 43
IMUL RAX, 2
; Bailout code
MOV R9, [RDI+8]
MOV [R9], RAX
MOV R9, [RDI+16]
MOV [R9], RSI
MOV RAX, 0 ; Default exit point (pc = 23)
LEAVE
RET