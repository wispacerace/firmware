add_library(Xbus STATIC
	xbusmessage.c xbusparser.c xbusutility.c xsdeviceid.c)
target_include_directories(Xbus PUBLIC ${CMAKE_CURRENT_LIST_DIR})
