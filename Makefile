all: poc.rom
	uxnemu $<

poc.rom: poc.tal
	uxnasm $< $@
