APPNAME			= simple_gpu_player
SRCDIR			= src
CD			= cd
MAKE			= make

all: $(APPNAME)

$(APPNAME): 
				$(CD) $(SRCDIR); \
				$(MAKE) -f Makefile distclean; \
				$(MAKE) -f Makefile install;


clean:

	$(CD) $(SRCDIR); \
	$(MAKE) -f Makefile clean; 
	
distclean: clean

	$(CD) $(SRCDIR); \
	$(MAKE) -f Makefile distclean; 
	