if ENABLE_CLIENT

lib_LTLIBRARIES = libnetmaumauclient.la
noinst_LTLIBRARIES = libnetmaumauclient_private.la

noinst_HEADERS = abstractclientv05impl.h base64bridge.h clientcardfactory.h clientconnectionimpl.h \
	interceptederrorexception.h

libnetmaumauclient_private_la_CXXFLAGS = -I$(top_srcdir)/src/common -I$(top_srcdir)/src/include \
	 $(NO_RTTI_FLAGS)
libnetmaumauclient_private_la_SOURCES = base64bridge.cpp clientcardfactory.cpp

libnetmaumauclient_la_CXXFLAGS = -I$(top_srcdir)/src/common -I$(top_srcdir)/src/include
libnetmaumauclient_la_SOURCES = abstractclient.cpp abstractclientv05impl.cpp \
	capabilitiesexception.cpp clientconnection.cpp clientconnectionimpl.cpp \
	connectionrejectedexception.cpp gamerunningexception.cpp interceptederrorexception.cpp \
	nonetmaumauserverexception.cpp playerlistexception.cpp protocolerrorexception.cpp \
	scoresexception.cpp shutdownexception.cpp timeoutexception.cpp versionmismatchexception.cpp 
libnetmaumauclient_la_LDFLAGS = -nodefaultlibs -nostartfiles -no-undefined \
	-version-info 3:$(SERVER_VERSION_MINOR):$(SERVER_VERSION_MAJOR)
libnetmaumauclient_la_LIBADD = libnetmaumauclient_private.la ../common/libnetmaumaucommon.la

endif
