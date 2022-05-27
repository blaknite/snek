SECTION code_user

defc cpm_bdos = 5

; Immediately get a character from the console
public _getchar_noblock
_getchar_noblock:
  ld c, 6 ; CP/M direct console I/O
  ld e, $FF
  call cpm_bdos
  ld h, 0
  ld l, a
  ret

; Quit and return to command prompt
public _quit
_quit:
  rst 0
