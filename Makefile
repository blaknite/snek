include config.mk

all: showinfo $(APP_NAME).hex
	z80dasm -g $(mem_org) -a $(APP_NAME).bin > $(APP_NAME).asm
	ruby ../util/bin2bas-ruby/bin2bas.rb --start=$(mem_org) $(APP_NAME).bin > $(APP_NAME).bas

showinfo:
	@echo -------------------------------------------------------------------
	@echo Compiling $(APP_NAME) starting at address $(mem_org)
	@echo -------------------------------------------------------------------

rc2014_ansi:
	$(MAKE) -C ../lib/rc2014-ansi/

$(APP_NAME).bin: $(APP_NAME).c rc2014_ansi
	zcc +embedded -vn -O3 -m -startup=0 -clib=new -pragma-define:CRT_ORG_CODE=$(mem_org) -pragma-define:CRT_ORG_BSS="-1" -pragma-define:CRT_INITIALIZE_BSS=1 -L../lib/rc2014-uart/ -I../lib/rc2014-uart/ -lrc2014_uart.lib -L../lib/rc2014-ansi/ -I../lib/rc2014-ansi/ -lrc2014_ansi.lib -create-app -o $(APP_NAME) $(APP_NAME).c

%.hex : %.bin
	cp $< aux_INIT.bin
	appmake +hex --org $(mem_org) -b aux_INIT.bin -o $@
	rm aux_INIT.bin

.PHONY clean:
	rm -f *.bin *.lst *.ihx *.hex *.obj *.rom zcc_opt.def $(APP_NAME) *.reloc *.sym *.map disasm.txt
