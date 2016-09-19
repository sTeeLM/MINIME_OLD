BINDIR      := $(shell [ -x ../../gfxboot-font ] && echo ../../ )

GFXBOOT_FONT = $(shell which gfxboot-font)
KEYMAPCHARS  = /usr/share/gfxboot/bin/keymapchars

all: .ready fonts

fonts: 16x16.fnt

.ready:
	@touch .ready

16x16.fnt: ../po/en.tr ../src/main.log
	cat ../po/*.tr ../help/*/*.html ../trans/* >tmp.txt
	$(GFXBOOT_FONT)  -v -l 14 \
	-a 0x2022-0x2023 \
	-c ISO-8859-15 -c ISO-8859-2 -c koi8-r -c gbk \
	`$(KEYMAPCHARS) ../keymaps/keymap.*.inc` \
	-t tmp.txt \
	-t ../src/main.log \
	-t ../data/languages \
	-p /usr/share/fonts/minime/ -f msyhcon:size=12 \
	16x16.fnt >16x16.fnt.log
	rm -f tmp.txt

clean:
	rm -f .ready *~

