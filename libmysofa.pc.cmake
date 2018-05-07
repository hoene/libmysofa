Name: @PROJECT_NAME@
Description: @CPACK_PACKAGE_DESCRIPTION@
Version: @PROJECT_VERSION@
Requires: @PKG_CONFIG_REQUIRES@
prefix=@CMAKE_INSTALL_PREFIX@
includedir=${prefix}/include
libdir=${prefix}/lib
Libs: -L${libdir} -lmysofa
Cflags: -I${includedir}

Libs.private: @PKG_CONFIG_PRIVATELIBS@
#Requires.private:
