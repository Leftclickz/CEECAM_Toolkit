QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lws2_32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    include/DatabaseConnection.cpp \
    include/LogFile.cpp \
    include/PGSQL_Helpers.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    include/DatabaseConnection.h \
    include/LogFile.h \
    include/PGSQL_Helpers.h \
    include/aetl.h \
    mainwindow.h \

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    dlls/libeay32.dll \
    dlls/libiconv-2.dll \
    dlls/libintl-9.dll \
    dlls/libpq.dll \
    dlls/libwinpthread-1.dll \
    dlls/ssleay32.dll \
    libs/libpq.dll \
    libs/libpq.lib \
    libs/pqxx.lib \
    libs/pqxx_static.pdb \
    libs/release/pqxx.lib \
    submodules/libpq-bin/ALL_BUILD.vcxproj \
    submodules/libpq-bin/ALL_BUILD.vcxproj.filters \
    submodules/libpq-bin/CPackConfig.cmake \
    submodules/libpq-bin/CPackSourceConfig.cmake \
    submodules/libpq-bin/INSTALL.vcxproj \
    submodules/libpq-bin/INSTALL.vcxproj.filters \
    submodules/libpq-bin/PACKAGE.vcxproj \
    submodules/libpq-bin/PACKAGE.vcxproj.filters \
    submodules/libpq-bin/ZERO_CHECK.vcxproj \
    submodules/libpq-bin/ZERO_CHECK.vcxproj.filters \
    submodules/libpq-bin/cmake_install.cmake \
    submodules/libpq-bin/include/CMakeFiles/generate.stamp \
    submodules/libpq-bin/include/CMakeFiles/generate.stamp.depend \
    submodules/libpq-bin/include/INSTALL.vcxproj \
    submodules/libpq-bin/include/INSTALL.vcxproj.filters \
    submodules/libpq-bin/include/PACKAGE.vcxproj \
    submodules/libpq-bin/include/PACKAGE.vcxproj.filters \
    submodules/libpq-bin/include/cmake_install.cmake \
    submodules/libpq-bin/include/pqxx/config_cmake.h.in \
    submodules/libpq-bin/libpqxx-config-version.cmake \
    submodules/libpq-bin/libpqxx-config.cmake \
    submodules/libpq-bin/libpqxx-targets.cmake \
    submodules/libpq-bin/libpqxx.sln \
    submodules/libpq-bin/src/CMakeFiles/generate.stamp \
    submodules/libpq-bin/src/CMakeFiles/generate.stamp.depend \
    submodules/libpq-bin/src/Debug/pqxx.lib \
    submodules/libpq-bin/src/INSTALL.vcxproj \
    submodules/libpq-bin/src/INSTALL.vcxproj.filters \
    submodules/libpq-bin/src/PACKAGE.vcxproj \
    submodules/libpq-bin/src/PACKAGE.vcxproj.filters \
    submodules/libpq-bin/src/Release/pqxx.lib \
    submodules/libpq-bin/src/cmake_install.cmake \
    submodules/libpq-bin/src/pqxx_shared.dir/Debug/pqxx.Build.CppClean.log \
    submodules/libpq-bin/src/pqxx_shared.dir/Debug/pqxx_shared.log \
    submodules/libpq-bin/src/pqxx_shared.dir/Debug/pqxx_shared.vcxproj.FileListAbsolute.txt \
    submodules/libpq-bin/src/pqxx_shared.vcxproj \
    submodules/libpq-bin/src/pqxx_shared.vcxproj.filters \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx.Build.CppClean.log \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.log \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.pdb \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/CL.command.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/CL.read.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/CL.write.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/Lib-link.read.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/Lib-link.write.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/Lib.command.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/custombuild.command.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/custombuild.read.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/custombuild.write.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.tlog/pqxx_static.lastbuildstate \
    submodules/libpq-bin/src/pqxx_static.dir/Debug/pqxx_static.vcxproj.FileListAbsolute.txt \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx.Build.CppClean.log \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.log \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/CL.command.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/CL.read.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/CL.write.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/Lib-link.read.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/Lib-link.write.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/Lib.command.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/custombuild.command.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/custombuild.read.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/custombuild.write.1.tlog \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.tlog/pqxx_static.lastbuildstate \
    submodules/libpq-bin/src/pqxx_static.dir/Release/pqxx_static.vcxproj.FileListAbsolute.txt \
    submodules/libpq-bin/src/pqxx_static.vcxproj \
    submodules/libpq-bin/src/pqxx_static.vcxproj.filters \
    submodules/libpq-bin/test/CMakeFiles/generate.stamp \
    submodules/libpq-bin/test/CMakeFiles/generate.stamp.depend \
    submodules/libpq-bin/test/CTestTestfile.cmake \
    submodules/libpq-bin/test/INSTALL.vcxproj \
    submodules/libpq-bin/test/INSTALL.vcxproj.filters \
    submodules/libpq-bin/test/PACKAGE.vcxproj \
    submodules/libpq-bin/test/PACKAGE.vcxproj.filters \
    submodules/libpq-bin/test/cmake_install.cmake \
    submodules/libpq-bin/test/runner.dir/Debug/runner.Build.CppClean.log \
    submodules/libpq-bin/test/runner.dir/Debug/runner.log \
    submodules/libpq-bin/test/runner.dir/Debug/runner.tlog/unsuccessfulbuild \
    submodules/libpq-bin/test/runner.dir/Debug/runner.vcxproj.FileListAbsolute.txt \
    submodules/libpq-bin/test/runner.vcxproj \
    submodules/libpq-bin/test/runner.vcxproj.filters \
    submodules/libpq-bin/test/unit/CMakeFiles/generate.stamp \
    submodules/libpq-bin/test/unit/CMakeFiles/generate.stamp.depend \
    submodules/libpq-bin/test/unit/CTestTestfile.cmake \
    submodules/libpq-bin/test/unit/INSTALL.vcxproj \
    submodules/libpq-bin/test/unit/INSTALL.vcxproj.filters \
    submodules/libpq-bin/test/unit/PACKAGE.vcxproj \
    submodules/libpq-bin/test/unit/PACKAGE.vcxproj.filters \
    submodules/libpq-bin/test/unit/cmake_install.cmake \
    submodules/libpq-bin/test/unit/unit_runner.dir/Debug/unit_runner.Build.CppClean.log \
    submodules/libpq-bin/test/unit/unit_runner.dir/Debug/unit_runner.log \
    submodules/libpq-bin/test/unit/unit_runner.dir/Debug/unit_runner.tlog/unsuccessfulbuild \
    submodules/libpq-bin/test/unit/unit_runner.dir/Debug/unit_runner.vcxproj.FileListAbsolute.txt \
    submodules/libpq-bin/test/unit/unit_runner.vcxproj \
    submodules/libpq-bin/test/unit/unit_runner.vcxproj.filters \
    submodules/libpqxx/.circleci/config.yml \
    submodules/libpqxx/.gitignore \
    submodules/libpqxx/AUTHORS \
    submodules/libpqxx/CMakeLists.txt \
    submodules/libpqxx/COPYING \
    submodules/libpqxx/INSTALL \
    submodules/libpqxx/NEWS \
    submodules/libpqxx/README-UPGRADE \
    submodules/libpqxx/README.md \
    submodules/libpqxx/VERSION \
    submodules/libpqxx/aclocal.m4 \
    submodules/libpqxx/autogen.sh \
    submodules/libpqxx/cmake/config.cmake \
    submodules/libpqxx/cmake/libpqxx-config.cmake \
    submodules/libpqxx/config/config.guess \
    submodules/libpqxx/config/config.sub \
    submodules/libpqxx/config/depcomp \
    submodules/libpqxx/config/install-sh \
    submodules/libpqxx/config/ltmain.sh \
    submodules/libpqxx/config/m4/libtool.m4 \
    submodules/libpqxx/config/m4/ltoptions.m4 \
    submodules/libpqxx/config/m4/ltsugar.m4 \
    submodules/libpqxx/config/m4/ltversion.m4 \
    submodules/libpqxx/config/m4/lt~obsolete.m4 \
    submodules/libpqxx/config/missing \
    submodules/libpqxx/config/mkinstalldirs \
    submodules/libpqxx/config/test-driver \
    submodules/libpqxx/configitems \
    submodules/libpqxx/configure \
    submodules/libpqxx/configure.ac \
    submodules/libpqxx/doc/Doxyfile.in \
    submodules/libpqxx/doc/conf.py \
    submodules/libpqxx/doc/html/README.md \
    submodules/libpqxx/doc/index.rst \
    submodules/libpqxx/doc/libpqxx.xml \
    submodules/libpqxx/include/CMakeLists.txt \
    submodules/libpqxx/include/CMakeLists.txt.template \
    submodules/libpqxx/include/pqxx/config.h.in \
    submodules/libpqxx/include/pqxx/doc/README.md \
    submodules/libpqxx/include/pqxx/doc/accessing-results.md \
    submodules/libpqxx/include/pqxx/doc/escaping.md \
    submodules/libpqxx/include/pqxx/doc/getting-started.md \
    submodules/libpqxx/include/pqxx/doc/mainpage.md \
    submodules/libpqxx/include/pqxx/doc/performance.md \
    submodules/libpqxx/include/pqxx/doc/prepared-statement.md \
    submodules/libpqxx/include/pqxx/doc/streams.md \
    submodules/libpqxx/include/pqxx/doc/thread-safety.md \
    submodules/libpqxx/libpqxx.pc.in \
    submodules/libpqxx/src/CMakeLists.txt \
    submodules/libpqxx/src/CMakeLists.txt.template \
    submodules/libpqxx/test/CMakeLists.txt \
    submodules/libpqxx/test/CMakeLists.txt.template \
    submodules/libpqxx/test/unit/CMakeLists.txt \
    submodules/libpqxx/test/unit/CMakeLists.txt.template \
    submodules/libpqxx/tools/extract_version \
    submodules/libpqxx/tools/lint \
    submodules/libpqxx/tools/pqxx-fulltest \
    submodules/libpqxx/tools/pre-release-check \
    submodules/libpqxx/tools/sign-tarball \
    submodules/libpqxx/tools/splitconfig \
    submodules/libpqxx/tools/template2mak.py \
    submodules/libpqxx/win32/INSTALL.txt \
    submodules/libpqxx/win32/MinGW.mak \
    submodules/libpqxx/win32/MinGW.mak.template \
    submodules/libpqxx/win32/common-sample \
    submodules/libpqxx/win32/vc-libpqxx.mak \
    submodules/libpqxx/win32/vc-libpqxx.mak.template \
    submodules/libpqxx/win32/vc-test-unit.mak \
    submodules/libpqxx/win32/vc-test-unit.mak.template \
    submodules/libpqxx/win32/vc-test.mak \
    submodules/libpqxx/win32/vc-test.mak.template

INCLUDEPATH += $$PWD\submodules\libpqxx\include
INCLUDEPATH += $$PWD\submodules\libpq-bin\include
INCLUDEPATH += $$PWD\include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/release/ -lpqxx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ -lpqxx
else:unix: LIBS += -L$$PWD/libs/ -lpqxx

INCLUDEPATH += $$PWD/libs
DEPENDPATH += $$PWD/libs

INCLUDEPATH += $$PWD/libs/release
DEPENDPATH += $$PWD/libs/release

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/ -llibpq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ -llibpq
else:unix: LIBS += -L$$PWD/libs/ -llibpq

INCLUDEPATH += $$PWD/libs
DEPENDPATH += $$PWD/libs
