
all: jailme

jailme: jailme.c
	${CC} -lutil -ljail -o jailme jailme.c

install: jailme
	install -o root -g wheel -m 4751 jailme /usr/local/sbin
