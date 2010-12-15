include @top_srcdir@/m4/ltrc.inc

omegalibexecdir = @libexecdir@/omegaonline

omegalibexec_LTLIBRARIES = OOIConv.la

OOIConv_la_SOURCES = \
	Converter.h \
	Converter.cpp \
	ooiconv.cpp \
	ooiconv_precomp.h
	
OOIConv_la_CXXFLAGS = $(PTHREAD_CFLAGS) $(INCICONV)
OOIConv_la_LDFLAGS = -module -no-undefined $(PTHREAD_LIBS)

if WIN32

OOIConv_la_CXXFLAGS += -I@top_srcdir@/../oocore/include
OOIConv_la_SOURCES += OOIConv_res.rc
OOIConv_la_LIBADD = @top_srcdir@/../oocore/build/src/OOCore/OOCore.la

# All this -Wl,-l shenanigans is to get the static lib of iconv linked into the dll
OOIConv_la_LDFLAGS += -Wl,-l:libcharset.a -Wl,-l:libiconv.a -Wl,-lgcc -Wl,-lmingwex -Wl,-lmsvcrt -Wl,-lkernel32 -avoid-version

else

OOIConv_la_LIBADD = oocore $(LIBICONV)
OOIConv_la_LDFLAGS += -release $(VERSION) -rpath /nowhere

endif
