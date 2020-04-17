Windows Third parts
===================

This project brings together all the dynamic and static librairies used in
Windows plateform development.

## x86 and x64 library support

To allow your project to support x86 and x64 libraries, you must add to your .pro file :


```
contains(QT_ARCH, x86_64) {
    LIBS += -L../<path_to_qt_project>/windows-3rdparts/x64/lib
} else {
    LIBS += -L../<path_to_qt_project>/windows-3rdparts/x86/lib
}
```

## ASIO C++ Headers-only library

To use with QT, you must update your ".pro" project file as follow :

```
QMAKE_CXXFLAGS += -DASIO_STANDALONE
QMAKE_CXXFLAGS += -isystem windows-3rdparts/include/asio
```

## SSH2 protocol library

To use with QT, you must update your ".pro" project file as follow :

```
QMAKE_CXXFLAGS += -isystem windows-3rdparts/include/ssh2
LIBS           += -lbcrypt
LIBS           += -lcrypt32
LIBS           += -lmswsock
LIBS           += -lssh2
```

---      
