QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config/config.cpp \
    crypto_tools/key_tools.cpp \
    crypto_tools/secp256k1/bench_ecdh.c \
    crypto_tools/secp256k1/secp256k1.c \
    main.cpp \
    ui/chat/chat.cpp \
    ui/main_widget/main_widget.cpp \
    ui/login/login.cpp

HEADERS += \
    config/config.h \
    crypto_tools/key_tools.h \
    crypto_tools/secp256k1/basic-config.h \
    crypto_tools/secp256k1/bench.h \
    crypto_tools/secp256k1/ecdsa.h \
    crypto_tools/secp256k1/ecdsa_impl.h \
    crypto_tools/secp256k1/eckey.h \
    crypto_tools/secp256k1/eckey_impl.h \
    crypto_tools/secp256k1/ecmult.h \
    crypto_tools/secp256k1/ecmult_const.h \
    crypto_tools/secp256k1/ecmult_const_impl.h \
    crypto_tools/secp256k1/ecmult_gen.h \
    crypto_tools/secp256k1/ecmult_gen_impl.h \
    crypto_tools/secp256k1/ecmult_impl.h \
    crypto_tools/secp256k1/field.h \
    crypto_tools/secp256k1/field_10x26.h \
    crypto_tools/secp256k1/field_10x26_impl.h \
    crypto_tools/secp256k1/field_5x52.h \
    crypto_tools/secp256k1/field_5x52_asm_impl.h \
    crypto_tools/secp256k1/field_5x52_impl.h \
    crypto_tools/secp256k1/field_5x52_int128_impl.h \
    crypto_tools/secp256k1/field_impl.h \
    crypto_tools/secp256k1/group.h \
    crypto_tools/secp256k1/group_impl.h \
    crypto_tools/secp256k1/hash.h \
    crypto_tools/secp256k1/hash_impl.h \
    crypto_tools/secp256k1/main_impl.h \
    crypto_tools/secp256k1/num.h \
    crypto_tools/secp256k1/num_gmp.h \
    crypto_tools/secp256k1/num_gmp_impl.h \
    crypto_tools/secp256k1/num_impl.h \
    crypto_tools/secp256k1/scalar.h \
    crypto_tools/secp256k1/scalar_4x64.h \
    crypto_tools/secp256k1/scalar_4x64_impl.h \
    crypto_tools/secp256k1/scalar_8x32.h \
    crypto_tools/secp256k1/scalar_8x32_impl.h \
    crypto_tools/secp256k1/scalar_impl.h \
    crypto_tools/secp256k1/scalar_low.h \
    crypto_tools/secp256k1/scalar_low_impl.h \
    crypto_tools/secp256k1/scratch.h \
    crypto_tools/secp256k1/scratch_impl.h \
    crypto_tools/secp256k1/secp256k1.h \
    crypto_tools/secp256k1/secp256k1_ecdh.h \
    crypto_tools/secp256k1/secp256k1_recovery.h \
    crypto_tools/secp256k1/testrand.h \
    crypto_tools/secp256k1/testrand_impl.h \
    crypto_tools/secp256k1/util.h \
    crypto_tools/string_tools.h \
    ui/chat/chat.h \
    ui/main_widget/main_widget.h \
    ui/login/login.h

FORMS += \
    ui/chat/chat.ui \
    ui/main_widget/main_widget.ui \
    ui/login/login.ui


INCLUDEPATH += D:/code/boost/boost_1_80_0
LIBS += -LD:/code/boost/boost_1_80_0/stage/lib -lboost_json-mgw11-mt-d-x64-1_80
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin


!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    crypto_tools/ed25519/README.md
