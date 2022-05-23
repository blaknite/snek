SECTION code_user

EXTERN asm_cpm_bdos

; Immediately get a character from the console
public _getchar_noblock
_getchar_noblock:
            ld hl, 0
            ld c, 6
            ld e, 0FFh
            call asm_cpm_bdos
            ld l, a
            ret

; Quit and return to command prompt
public _quit
_quit:
            rst 0
