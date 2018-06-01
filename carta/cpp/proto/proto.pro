#! include(../common.pri) {
#  error( "Could not find the common.pri file!" )
#}

! include(./proto_compile.pri) {
  error( "Could not find the proto_compile.pri file!" )
}

TEMPLATE = lib

###CONFIG += staticlib
#QT += network
#QT += xml

PROTOS = lm.helloworld.proto

INCLUDEPATH += ../../../ThirdParty/protobuf/include
LIBS += -L../../../ThirdParty/protobuf/lib -lprotobuf

