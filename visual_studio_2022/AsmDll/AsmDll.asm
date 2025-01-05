;-------------------------------------------------------------------------
; file: AsmDll.asm
; author: krzsztfwtk
; version: 2.1
; date: 2024-12-23
; brief: Kod w 64-bitowym assemblerze do oblicznia wartości wielomianu
; Microsoft Macro Assembler (MASM) 64-bit environment
;
; @copyright: Copyright (c) 2025 krzsztfwtk
;-------------------------------------------------------------------------

.CODE

;-------------------------------------------------------------------------
HornerPolynomial proc
    ; Argumenty:
    ; RCX = wskaźnik do x (float)
    ; RDX = wskaźnik do tablicy współczynników a[] (float[])
    ; R8  = liczba współczynników n (int)

    ; Zarezerwuj miejsce na stosie
    sub rsp, 32 ; Zarezerwuj miejsce na zmienne lokalne

    ; Załaduj x do xmm1
    movss xmm1, dword ptr [rcx] ; xmm1 = x
    dec r8  ; r8 = n - 1 (indeks ostatniego elementu)

    ; wynik zapisany w xmm0, ponieważ w konwencji wywoływania 
    ; Windows x64, wartość zwracana funkcji musi być umieszczona 
    ; w rejestrze RAX dla wartości całkowitych
    ; lub w XMM0 dla wartości zmiennoprzecinkowych.
    movss xmm0, dword ptr [rdx + r8*4] ; w = a[r8]

    ; Iteracja od przedostatniego a[] do początku
    dec r8                              ; r8 = n - 2
StartLoop:
    cmp r8, 0           ; Czy r8 >= 0?
    jl EndLoop          ; Jeśli nie, wyjdź z pętli

    ; Pobierz a[r8] do xmm2
    movss xmm2, dword ptr [rdx + r8*4]  ; xmm2 = a[r8]
    mulss xmm0, xmm1                    ; w = w * x
    addss xmm0, xmm2                    ; w = w + a[r8]
    dec r8                              ; r8--

    jmp StartLoop                       ; Powrót do pętli

EndLoop:
    add rsp, 32
    ret
HornerPolynomial endp
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
HornerPolynomialAvx proc
    ; Argumenty:
    ; RCX = wskaźnik do tablicy x (float[8])
    ; RDX = wskaźnik do tablicy współczynników a[] (float[])
    ; R8  = liczba współczynników n (int)

    ; Zarezerwuj miejsce na stosie
    sub rsp, 32

    ; Załaduj tablicę x do ymm0 (8 elementów float)
    vmovups ymm1, ymmword ptr [rcx] ; ymm1 = x[0..7]
    dec r8                          ; r8 = n - 1 (indeks ostatniego)

    ; Zainicjalizuj wynik jako ostatni współczynnik (8 floatów)
    vbroadcastss ymm2, dword ptr [rdx + r8*4] ; ymm2 = a[r8]
    vmovups ymm0, ymm2       ; ymm1 = a[r8]

    ; Iteracja od przedostatniego a[] do początku
    dec r8                              ; r8 = n - 2

StartLoopAvx:
    cmp r8, 0                           ; Czy r8 >= 0?
    jl EndLoopAvx                       ; Jeśli nie, wyjdź z pętli

    ; Pobierz współczynnik a[r8] do ymm2
    vbroadcastss ymm2, dword ptr [rdx + r8*4] ; ymm2 = a[r8]

    ; Wykonaj obliczenia Hornera: w = w * x + a[r8]
    vmulps ymm0, ymm0, ymm1       ; w = w * x
    vaddps ymm0, ymm0, ymm2       ; w = w + a[r8])

    ; Zmniejsz indeks
    dec r8                        ; r8--
    jmp StartLoopAvx              ; Powrót do pętli

EndLoopAvx:
    ; Przywróć stos
    add rsp, 32
    ret
HornerPolynomialAvx endp
;-------------------------------------------------------------------------

END