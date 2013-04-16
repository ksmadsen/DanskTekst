c:
	./waf configure && ./waf build
l: c
	tools/deploypebble.sh load build/DanskTekst.pbw
d: c
	tools/deploypebble.sh reinstall build/DanskTekst.pbw 
