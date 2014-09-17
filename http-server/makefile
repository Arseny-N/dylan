CC=gcc
CFALGS=-Wall -g3 -O0

OBJS=netbuf.o pbuf.o generic-json-zero-copy.o post_methods.o low-level-api.o http_server.o api_lib.o http-utils.o



pbuf.o: pseudo-lwip/pbuf.c pseudo-lwip/pbuf.h
	$(CC) $(CFALGS) -c pseudo-lwip/pbuf.c	
api_lib.o: pseudo-lwip/api_lib.c pseudo-lwip/api.h
	$(CC) $(CFALGS) -c pseudo-lwip/api_lib.c	
netbuf.o: pseudo-lwip/netbuf.c pseudo-lwip/netbuf.h
	$(CC) $(CFALGS) -c pseudo-lwip/netbuf.c	
	

http-utls.o: http-utils.c http-utils.h
	$(CC) $(CFALGS) -c http-utils.c			
	
generic-json-zero-copy.o: generic-json-zero-copy.c generic-json-zero-copy.h
	$(CC) $(CFALGS) -c generic-json-zero-copy.c
	
post_methods.o: generic-json-zero-copy.o
	$(CC) $(CFALGS) -c post_methods.c	
	
low-level-api.o: low-level-api.c low-level-api.h
	$(CC) $(CFALGS) -c low-level-api.c
	
http_server.o: http_server.c
	$(CC) $(CFALGS) -c http_server.c


	
noos: $(OBJS)
	$(CC) $(CFALGS) $(OBJS) noOS.c -o noOS
	
json: netbuf.o pbuf.o generic-json-zero-copy.o
	$(CC) $(CFALGS) netbuf.o pbuf.o generic-json-zero-copy.o json_decode.c -o json
	
clean:
	rm $(OBJS) noOS json valgrid.log
	
valgrind:	
	valgrind --log-file=valgrid.log --leak-check=full --show-reachable=yes ./noOS $(PORT)

.PHONY: clean json noos valgrind
