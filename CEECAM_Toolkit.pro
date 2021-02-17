QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lws2_32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#set exe icon
RC_ICONS = favicon.ico

SOURCES += \
    dialog_downloadtool.cpp \
    dialog_uploadtool.cpp \
    include/DatabaseConnection.cpp \
    include/LogFile.cpp \
    include/PGSQL_Helpers.cpp \
    main.cpp \
    mainmenugreeting.cpp

HEADERS += \
    argumentmap.h \
    curluploadthread.h \
    dialog_downloadtool.h \
    dialog_uploadtool.h \
    gcsdownloadthread.h \
    include/DatabaseConnection.h \
    include/LogFile.h \
    include/PGSQL_Helpers.h \
    include/aetl.h \
    mainmenugreeting.h

FORMS += \
    dialog_downloadtool.ui \
    dialog_uploadtool.ui \
    mainmenugreeting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#Includes
INCLUDEPATH += $$PWD\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\abseil_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\c-ares_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\crc32c_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\curl_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\google-cloud-cpp_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\grpc_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\libpq_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\libpqxx_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\nlohmann-json_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\openssl-windows_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\protobuf_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\re2_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\upb_x64-windows\include
INCLUDEPATH += $$PWD\submodules\vcpkg\packages\zlib_x64-windows\include

# -- GOOGLE-CLOUD-CPP LIBRARY INCLUDES START

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/lib/ -lgoogleapis_cpp_storage_protos
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/debug/lib/ -lgoogleapis_cpp_storage_protos

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/lib/ -lstorage_client
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/debug/lib/ -lstorage_client

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/lib/ -lgoogle_cloud_cpp_common
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/debug/lib/ -lgoogle_cloud_cpp_common

INCLUDEPATH += $$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/lib/
DEPENDPATH += $$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/lib/

INCLUDEPATH += $$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/debug/lib/
DEPENDPATH += $$PWD/submodules/vcpkg/packages/google-cloud-cpp_x64-windows/debug/lib/

# -- GOOGLE-CLOUD-CPP LIBRARY INCLUDES END

# -- ABSEIL LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labseil_dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labseil_dll

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_config
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_config

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_internal
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_internal

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_marshalling
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_marshalling

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_parse
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_parse

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_program_name
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_program_name

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_registry
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_registry

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_usage
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_usage

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_flags_usage_internal
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_flags_usage_internal

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/lib -labsl_random_internal_distribution_test_util
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/abseil_x64-windows/debug/lib -labsl_random_internal_distribution_test_util
# -- ABSEIL LIBRARY INCLUDES END

# -- C-ARES LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/c-ares_x64-windows/lib -lcares
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/c-ares_x64-windows/debug/lib -lcares
# -- C-ARES LIBRARY INCLUDES END

# -- CRC32C LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/crc32c_x64-windows/lib -lcrc32c
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/crc32c_x64-windows/debug/lib -lcrc32c
# -- CRC32C LIBRARY INCLUDES END

# -- CURL LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/curl_x64-windows/lib -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/curl_x64-windows/debug/lib -llibcurl-d
# -- CURL LIBRARY INCLUDES END

# -- GRPC LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -laddress_sorting
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -laddress_sorting

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgpr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgpr

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc_csharp_ext
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc_csharp_ext

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc_plugin_support
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc_plugin_support

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc_unsecure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc_unsecure

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc++
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc++

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc++_alts
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc++_alts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc++_error_details
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc++_error_details

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc++_reflection
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc++_reflection

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpc++_unsecure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpc++_unsecure

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/lib -lgrpcpp_channelz
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/grpc_x64-windows/debug/lib -lgrpcpp_channelz
# -- GRPC LIBRARY INCLUDES END

# -- LIBPQ LIBRARY INCLUDES START

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/lib -llibecpg
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/debug/lib -llibecpg

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/lib -llibecpg_compat
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/debug/lib -llibecpg_compat

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/lib -llibpgcommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/debug/lib -llibpgcommon

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/lib -llibpgport
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/debug/lib -llibpgport

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/lib -llibpgtypes
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/debug/lib -llibpgtypes

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/lib -llibpq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpq_x64-windows/debug/lib -llibpq

# -- LIBPQ LIBRARY INCLUDES END

# -- LIBPQXX LIBRARY INCLUDES START

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpqxx_x64-windows/lib -llibpqxx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/libpqxx_x64-windows/debug/lib -llibpqxxd

# -- LIBPQXX LIBRARY INCLUDES END

# -- OPENSSL LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/openssl-windows_x64-windows/lib -llibcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/openssl-windows_x64-windows/debug/lib -llibcrypto

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/openssl-windows_x64-windows/lib -llibssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/openssl-windows_x64-windows/debug/lib -llibssl
# -- OPENSSL LIBRARY INCLUDES END

# -- PROTOBUF LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/protobuf_x64-windows/lib -llibprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/protobuf_x64-windows/debug/lib -llibprotobufd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/protobuf_x64-windows/lib -llibprotobuf-lite
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/protobuf_x64-windows/debug/lib -llibprotobuf-lited

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/protobuf_x64-windows/lib -llibprotoc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/protobuf_x64-windows/debug/lib -llibprotocd
# -- PROTOBUF LIBRARY INCLUDES END

# -- RE2 LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/re2_x64-windows/lib -lre2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/re2_x64-windows/debug/lib -lre2
# -- RE2 LIBRARY INCLUDES END

# -- UPB LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/lib -lhandlers
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/debug/lib -lhandlers

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/lib -lport
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/debug/lib -lport

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/lib -lreflection
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/debug/lib -lreflection

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/lib -lupb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/debug/lib -lupb

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/lib -lupb_json
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/debug/lib -lupb_json

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/lib -lupb_pb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/upb_x64-windows/debug/lib -lupb_pb
# -- UPB LIBRARY INCLUDES END

# -- ZLIB LIBRARY INCLUDES START
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/zlib_x64-windows/lib -lzlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/vcpkg/packages/zlib_x64-windows/debug/lib -lzlibd
# -- ZLIB LIBRARY INCLUDES END



