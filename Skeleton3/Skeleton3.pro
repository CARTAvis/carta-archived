TEMPLATE = subdirs
SUBDIRS += cpp \
    scriptedClient
#SUBDIRS += html5

OTHER_FILES += readme.txt

message( "root            PWD=$$PWD")
message( "root         IN_PWD=$$IN_PWD")
message( "root _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
message( "root        OUT_PWD=$$OUT_PWD")

