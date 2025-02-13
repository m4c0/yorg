all: poc.rom
	uxncli $<

poc.rom: poc.tal
	uxnasm $< $@
