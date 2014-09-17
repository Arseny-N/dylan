CC=gcc
CFALGS=-Wall
F2H=./f2h


f2h: file2hdr.c
	$(CC) $(CFALGS) file2hdr.c -o f2h

header:
	gzip -9 -c ../web-interface/build/index.min.html | $(F2H) > ../http-server/wp.h
	@tail -1 ../http-server/wp.h
	@echo
	@tail -1 ../http-server/wp.h >> webpage-size.log
	@echo >> webpage-size.log
	@ls -l ../web-interface/build/index.min.html |  cut -f 5,9 -d ' ' >> webpage-size.log
	@date >> webpage-size.log	
	@echo "-----------------------------------" >> webpage-size.log
