global asm_process

section .text
; void asm_process(const float* input, float* output, uint32_t frames)
asm_process:
    ; rdi = input array
    ; rsi = output array
    ; edx = number of audio samples (frames)
    test edx, edx
    jz .done

    ; Load 0.5 into xmm1
    mov eax, 0x3F000000
    movd xmm1, eax

.loop:
    movss xmm0, [rdi]
    mulss xmm0, xmm1
    movss [rsi], xmm0
    add rdi, 4
    add rsi, 4
    dec edx
    jnz .loop
.done:
    ret

; FIX: Explicitly mark the stack as non-executable for modern linkers/DAWs
section .note.GNU-stack noalloc noexec nowrite progbits
