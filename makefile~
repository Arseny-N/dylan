CD=cd
GULP=/bin/gulp
MAKE=make
ATMEL_PATH=atmel-studio/FREERTOS_SAM_LWIP_DHCP_EXAMPLE44/FREERTOS_SAM_LWIP_DHCP_EXAMPLE44/src/ASF/thirdparty/freertos/demo/lwip_avr32_uc3_example/network/basicweb/

html2hdr: web-interface
	@$(CD) tools; \
	$(MAKE) header	
	
http-server: web-interface html2hdr
	@$(CD) http-server; \
	$(MAKE) noos -B
	
main: http-server

sync-atmel:
	$(CP) `ls | egrep -v '(json_decode.c|json|noOS|*~$|\.o$|makefile|*\.log$|pseudo-lwip|macros.h)' -t $(ATMEL_PATH)`

	
	

web-interface:
	@$(CD) web-interface; \
	$(GULP)	 
	
.PHONY:	web-interface sync-atmel
