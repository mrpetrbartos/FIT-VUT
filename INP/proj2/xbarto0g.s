; Autor reseni: Petr Bartos xbarto0g

; Projekt 2 - INP 2022
; Vernamova sifra na architekture MIPS64

; DATA SEGMENT
                .data
login:          .asciiz "xbarto0g"  ; sem doplnte vas login
cipher:         .space  17  ; misto pro zapis sifrovaneho loginu

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text

                ; ZDE NAHRADTE KOD VASIM RESENIM
main:
                ; xbarto0g-r19-r22-r3-r29-r0-r4
                DADDI r3, r0, 2 ; r3 <- b
                DADDI r4, r0, 1 ; r4 <- a
                DADDI r29, r0, 0

                load:
                LB r19, login(r29)
                DADDI r22, r0, 97
                SLT r22, r19, r22 ; pokud aktualni znak neni pismeno, konec
                BNE r22, r0, done

                ANDI r22, r29, 1
                BEQ r22, r0, add_b

                sub_b:
                DSUB r19, r19, r4 ; -a
                B fixflows

                add_b:
                DADD r19, r19, r3 ; +b

                fixflows:
                DADDI r22, r0, 122
                SLT r22, r22, r19
                BNE r22, r0, overflow ; znak ma hodnotu > 122 -> pretekl
                DADDI r22, r0, 97
                SLT r22, r19, r22
                BNE r22, r0, underflow ; znak ma hodnotu < 97 -> podtekl
                B end

                underflow:
                DADDI r19, r19, 26
                B end

                overflow:
                DADDI r19, r19, -26

                end:
                SB r19, cipher(r29)
                DADDI r29, r29, 1
                B load

                done:
                SB r0, cipher(r29)
                daddi   r4, r0, cipher   ; vozrovy vypis: adresa cipher: do r4
                jal     print_string    ; vypis pomoci print_string - viz nize
                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
