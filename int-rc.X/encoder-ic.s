; Fast quadrature decoder algorithm for couch
; W.A. 8/17
    
.text
.equ __33EP512GM706, 1
.include "p33EP512GM706.inc"
    
.global __IC1Interrupt
.global __IC2Interrupt
.global __IC3Interrupt
.global __IC4Interrupt
.global __T6Interrupt
	
.macro dec_disp reg, exit
    dec \reg
    bra c, \exit
    dec \reg+2
    bra \exit
.endm
    
.macro inc_disp reg, exit
    inc \reg
    bra nc, \exit
    inc \reg+2
    bra \exit
.endm
    
.macro nop_disp exit
    bra \exit
    nop
    nop
    nop
.endm
    
__T6Interrupt:
    clr TMR7
    bclr IFS2, #T6IF
    retfie
    
; QEA1 change
__IC1Interrupt:
    push.s
    clr TMR6
    
    mov PORTB, w0
    lsr w0, #12, w0
    and w0, #3, w0
	
    btsc _old_enc1, #1
    bset w0, #2
    mov w0, _old_enc1
	
    mul.uu w0, #4, w0
    bra w0
	
    dec_disp _disp1, ic1exit	; Bp0 B0  A0
    nop_disp ic1exit		; Bp0 B0  A1
    nop_disp ic1exit		; Bp0 B1  A0
    inc_disp _disp1, ic1exit	; Bp0 B1  A1
    inc_disp _disp1, ic1exit	; Bp1 B0  A0
    nop_disp ic1exit		; Bp1 B0  A1
    nop_disp ic1exit		; Bp1 B1  A0
    dec_disp _disp1, ic1exit	; Bp1 B1  A1
	
ic1exit:
    bclr IFS0, #IC1IF
    pop.s
    retfie

    
; QEB1 change
__IC2Interrupt:
    push.s
    clr TMR6
    
    mov PORTB, w0
    lsr w0, #12, w0
    and w0, #3, w0
	
    btsc _old_enc1, #0
    bset w0, #2
    mov w0, _old_enc1
	
    mul.uu w0, #4, w0
    bra w0
	
    inc_disp _disp1, ic2exit	; Ap0 B0  A0
    nop_disp ic2exit		; Ap0 B0  A1
    nop_disp ic2exit		; Ap0 B1  A0
    dec_disp _disp1, ic2exit	; Ap0 B1  A1
    dec_disp _disp1, ic2exit	; Ap1 B0  A0
    nop_disp ic2exit		; Ap1 B0  A1
    nop_disp ic2exit		; Ap1 B1  A0
    inc_disp _disp1, ic2exit	; Ap1 B1  A1
	
ic2exit:
    bclr IFS0, #IC2IF
    pop.s
    retfie

        
; QEA3 change
__IC3Interrupt:
    push.s
    clr TMR6
    
    mov PORTC, w0
    lsr w0, #8, w0
    and w0, #1, w0
    btsc PORTD, #5
    bset w0, #1;
	
    btsc _old_enc3, #1
    bset w0, #2
    mov w0, _old_enc3
	
    mul.uu w0, #4, w0
    bra w0
	
    dec_disp _disp3, ic3exit	; Bp0 B0  A0
    nop_disp ic3exit		; Bp0 B0  A1
    nop_disp ic3exit		; Bp0 B1  A0
    inc_disp _disp3, ic3exit	; Bp0 B1  A1
    inc_disp _disp3, ic3exit	; Bp1 B0  A0
    nop_disp ic3exit		; Bp1 B0  A1
    nop_disp ic3exit		; Bp1 B1  A0
    dec_disp _disp3, ic3exit	; Bp1 B1  A1
	
ic3exit:
    bclr IFS2, #IC3IF
    pop.s
    retfie

    
; QEB3 change
__IC4Interrupt:
    push.s
    clr TMR6
    
    mov PORTC, w0
    lsr w0, #8, w0
    and w0, #1, w0
    btsc PORTD, #5
    bset w0, #1
	
    btsc _old_enc3, #0
    bset w0, #2
    mov w0, _old_enc3
	
    mul.uu w0, #4, w0
    bra w0
	
    inc_disp _disp3, ic4exit	; Ap0 B0  A0
    nop_disp ic4exit		; Ap0 B0  A1
    nop_disp ic4exit		; Ap0 B1  A0
    dec_disp _disp3, ic4exit	; Ap0 B1  A1
    dec_disp _disp3, ic4exit	; Ap1 B0  A0
    nop_disp ic4exit		; Ap1 B0  A1
    nop_disp ic4exit		; Ap1 B1  A0
    inc_disp _disp3, ic4exit	; Ap1 B1  A1
	
ic4exit:
    bclr IFS2, #IC4IF
    pop.s
    retfie

.end
    