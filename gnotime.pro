TARGET = gnotime
TEMPLATE = app

CONFIG += \
    debug \
    link_pkgconfig \

DEFINES += \
    DATADIR=\\\"/usr/share\\\" \
    GTTDATADIR=\\\"/usr/share/gnotime\\\" \
    GTTGLADEDIR=\\\"/usr/share/gnotime\\\" \
    GNOMELOCALEDIR=\\\"/usr/share/locale\\\" \
    LIBDIR=\\\"/usr/lib/gnotime\\\" \
    PREFIX=\\\"/usr/share\\\" \
    SYSCONFDIR=\\\"/usr/share\\\" \

HEADERS += \
    src/active-dialog.h \
    src/app.h \
    src/cur-proj.h \
    src/dbus-glue.h \
    src/dbus.h \
    src/dialog.h \
    src/err-throw.h \
    src/export.h \
    src/file-io.h \
    src/gconf-gnomeui.h \
    src/gconf-io.h \
    src/gconf-io-p.h \
    src/ghtml-deprecated.h \
    src/ghtml.h \
    src/gtt.h \
    src/idle-dialog.h \
    src/idle-timer.h \
    src/journal.h \
    src/log.h \
    src/menucmd.h \
    src/menus.h \
    src/myoaf.h \
    src/notes-area.h \
    src/plug-in.h \
    src/prefs.h \
    src/projects-tree.h \
    src/proj.h \
    src/proj_p.h \
    src/proj-query.h \
    src/props-invl.h \
    src/props-proj.h \
    src/props-task.h \
    src/query.h \
    src/status-icon.h \
    src/timer.h \
    src/toolbar.h \
    src/util.h \
    src/xml-gtt.h \

PKGCONFIG += \
    guile-2.0 \
    libglade-2.0 \
    libgnome-2.0 \
    libgnomeui-2.0 \
    libgtkhtml-3.14 \
    libxml-2.0 \
    qof \
    x11 \
    xscrnsaver \

SOURCES += \
    src/active-dialog.c \
    src/app.c \
    src/dbus.c \
    src/dialog.c \
    src/err.c \
    src/err-throw.c \
    src/export.c \
    src/file-io.c \
    src/gconf-gnomeui.c \
    src/gconf-io.c \
    src/ghtml.c \
    src/ghtml-deprecated.c \
    src/idle-dialog.c \
    src/idle-timer.c \
    src/journal.c \
    src/log.c \
    src/main.c \
    src/menucmd.c \
    src/menus.c \
    src/myoaf.c \
    src/notes-area.c \
    src/plug-edit.c \
    src/plug-in.c \
    src/prefs.c \
    src/proj.c \
    src/projects-tree.c \
    src/proj-query.c \
    src/props-invl.c \
    src/props-proj.c \
    src/props-task.c \
    src/query.c \
    src/status-icon.c \
    src/timer.c \
    src/toolbar.c \
    src/util.c \
    # src/vfs-io.c \
    src/xml-read.c \
    src/xml-write.c \