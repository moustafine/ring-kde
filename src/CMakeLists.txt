IF(POLICY CMP0017)
   CMAKE_POLICY(SET CMP0017 NEW)
ENDIF(POLICY CMP0017)

IF(POLICY CMP0022)
   CMAKE_POLICY(SET CMP0022 NEW)
ENDIF(POLICY CMP0022)

IF(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

# set(CMAKE_AUTOMOC ON)

SET(KF5_DEP_VERSION "5.6.0")
SET(QT_MIN_VERSION  "5.2.0")

FIND_PACKAGE ( KF5 REQUIRED )

FIND_PACKAGE (ECM 1.1.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

FIND_PACKAGE ( LibRingClient REQUIRED ENABLE_STATIC=true )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  FIND_LIBRARY(OSX_FRAMEWORK_OPENGL NAMES OpenGL)
ENDIF()

ADD_SUBDIRECTORY( conf/account/ )

ADD_DEFINITIONS(
   ${KF5_DEFINITIONS}
   ${QT_DEFINITIONS}
   -fexceptions
   -Wno-error
   -Wno-reorder
   -Wno-error=deprecated-declarations
)

# Enable some useful warnings
ADD_DEFINITIONS(
   -Wall
   -Wextra
   -Wmissing-declarations
   -Wmissing-noreturn
   -Wpointer-arith
   -Wcast-align
   -Wwrite-strings
   -Wformat-nonliteral
   -Wformat-security
   -Wswitch-enum
   -Winit-self
   -Wmissing-include-dirs
   -Wundef
   -Wmissing-format-attribute
   -Wno-reorder
   -Wunused
   -Wuninitialized
   -Woverloaded-virtual
   -Wunused-value
   -pedantic
   -Wnonnull
   -Wsequence-point
   #-Wsystem-headers
   -Wsizeof-pointer-memaccess
   #-Wuseless-cast
   -Wvarargs
)

#Add more warnings for compilers that support it. I used this command:
#curl https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Warning-Options.html | \
#grep -E "^[\t ]+<br><dt><code>-W[a-zA-Z=-]*" -o | grep -E "\-W[a-zA-Z=-]*" -o >
#cat /tmp/48 /tmp/49 | sort | uniq -u
# IF (CMAKE_COMPILER_IS_GNUCC)

IF (CMAKE_COMPILER_IS_GNUCC)
   IF (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
      ADD_DEFINITIONS(
         -Wunused-but-set-parameter
         -Wconditionally-supported
         -Wsuggest-attribute=const
         -Wno-cpp
#          -Wdouble-promotion
         -Wdate-time
         -Wdelete-incomplete
#          -Wfloat-conversion
      )
   ENDIF()

   IF (GCC_VERSION VERSION_GREATER 5.0 OR GCC_VERSION VERSION_EQUAL 5.0)
      ADD_DEFINITIONS(
         -Wsuggest-override
         -Wsuggest-final-types
         -Wsuggest-final-methods
         -Wbool-compare
         -Wformat-signedness
         -Wlogical-not-parentheses
         -Wnormalized
         -Wshift-count-negative
         -Wshift-count-overflow
         -Wsized-deallocation
         -Wsizeof-array-argument
      )
   ENDIF()

   IF (GCC_VERSION VERSION_GREATER 6.0 OR GCC_VERSION VERSION_EQUAL 6.0)
      ADD_DEFINITIONS(
         -Wnull-dereference
         -Wtautological-compare
         -Wduplicated-cond
         -Wmisleading-indentation
      )
   ENDIF()

ENDIF()

IF (CMAKE_COMPILER_IS_GNUCC)
   EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   IF (GCC_VERSION VERSION_LESS 4.6.9 OR GCC_VERSION VERSION_EQUAL 4.6)
      #GCC 4.6 version of those warnings does detect valid C++0x/C++11 as invalid.
      ADD_DEFINITIONS(
         -Wno-error=pragmas
         -Wno-pragmas
         -Wno-error
      )
   ENDIF()
   IF (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
      ADD_DEFINITIONS(
         -Wmaybe-uninitialized
         -Wunused-local-typedefs
         -pedantic
      )
   ENDIF()
   IF (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
#       ADD_DEFINITIONS("-Wzero-as-null-pointer-constant")
#       ADD_DEFINITIONS( -DENABLE_IGNORE_NULL=true )
   ENDIF()
ELSE()
   ADD_DEFINITIONS(-Wno-unknown-pragmas)
ENDIF()

ADD_DEFINITIONS("-std=c++0x")

#Make sure it can access DBUS autogenerated files
INCLUDE_DIRECTORIES(SYSTEM
   ${KF5_INCLUDES}
   ${Qt5Widgets_INCLUDES}
   ${Qt5PrintSupport_INCLUDES}
   ${Qt5Core_INCLUDES}
   ${CMAKE_BINARY_DIR}
   ${LIB_RING_CLIENT_INCLUDE_DIR}
   ${LIB_TIP_INCLUDE}
   ${LIB_CONFIG_INCLUDE}
   ${LIB_CONFIG_WIDGETS}
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/
)

#Build KDE specific files
ADD_SUBDIRECTORY( klib )

IF(${CMAKE_BUILD_TYPE} MATCHES Release)
   MESSAGE("NO DEBUG OUTPUT")
   ADD_DEFINITIONS( -DQT_NO_DEBUG_OUTPUT)
ELSE()
#     ADD_DEFINITIONS(-Werror)
ENDIF(${CMAKE_BUILD_TYPE} MATCHES Release)

# SET ( KF5_KABC_LIBS  -lkabc )

# Delegates
SET(
   ring_kde_DELEGATES
   delegates/conferencedelegate.cpp
   delegates/contactdelegate.cpp
   delegates/historydelegate.cpp
   delegates/phonenumberdelegate.cpp
   delegates/delegatedropoverlay.cpp
   delegates/dialpaddelegate.cpp
   delegates/imdelegate.cpp
   delegates/autocompletiondelegate.cpp
   delegates/autocompletiondelegate2.cpp
   delegates/toolbardelegate.cpp
   delegates/accountinfodelegate.cpp
   delegates/kdepixmapmanipulation.cpp
   proxies/simplerotateproxy.cpp
   proxies/deduplicateproxy.cpp
   proxies/filtertoplevelproxy.cpp
   proxies/roletransformationproxy.cpp
   proxies/flagsfilterproxy.cpp
   proxies/pollingproxy.cpp
   #proxies/valuefilterproxy.cpp
)

# Configuration pages
SET(
   ring_kde_CONF
   conf/configurationdialog.cpp
   conf/dlggeneral.cpp
   conf/dlgdisplay.cpp
   conf/dlgaudio.cpp
   conf/dlgaddressbook.cpp
   conf/dlghooks.cpp
   conf/dlgaccessibility.cpp
   conf/dlgpresence.cpp
   configurator/localhistoryconfigurator.cpp
   configurator/audiorecordingconfigurator.cpp
   configurator/fallbackpersonconfigurator.cpp
)

# Widgets
SET(
   ring_kde_WIDGETS
   widgets/personselector.cpp
   widgets/contactmethodselector.cpp
   widgets/systray.cpp
   widgets/dialpad.cpp
   widgets/dockbase.cpp
   widgets/categorizedtreeview.cpp
   widgets/immanager.cpp
   widgets/callviewoverlay.cpp
   widgets/overlaytoolbar.cpp
   widgets/callviewtoolbar.cpp
   widgets/tips/conftip.cpp
   widgets/tips/dialpadtip.cpp
   widgets/tips/ringingtip.cpp
   widgets/tips/connectionlosttip.cpp
   widgets/tips/searchingtip.cpp
   widgets/tips/tipcollection.cpp
   widgets/tips/resizabletip.cpp
   widgets/tips/removeconferencetip.cpp
   widgets/player.cpp
   widgets/playeroverlay.cpp
   widgets/filterlineedit.cpp
   widgets/kphonenumberselector.cpp
   widgets/contactview.cpp
   widgets/presence.cpp
   widgets/autocompletion.cpp
   widgets/menumodelview.cpp
   widgets/toolbuttonmodelview.cpp
   widgets/wizard.cpp
   widgets/personselector.cpp
   widgets/contactmethodselector.cpp
)

# Video Widgets
IF(NOT (${ENABLE_VIDEO} MATCHES false))
   SET(
      ring_kde_VIDEO
      widgets/videodock.cpp
      widgets/videosettings.cpp
      widgets/videowidget.cpp
      widgets/videoglframe.cpp
      widgets/videoscene.cpp
      widgets/videotoolbar.cpp
      conf/dlgvideo.cpp
   )
ENDIF(NOT (${ENABLE_VIDEO} MATCHES false))

SET(
   ring_kde_SRCS
   main.cpp
   view.cpp
   mainwindow.cpp
   cmd.cpp
   dock.cpp
   notification.cpp
   notification.cpp
   ringapplication.cpp
   errormessage.cpp
   canvasobjectmanager.cpp
   eventmanager.cpp
   actioncollection.cpp
   implementation.cpp
   ${ring_kde_DELEGATES}
   ${ring_kde_CONF}
   ${ring_kde_WIDGETS}
   ${ring_kde_VIDEO}
)


IF (Qt5TextToSpeech_FOUND)

SET(
   ring_kde_SRCS
   ${ring_kde_SRCS}
   accessibility.cpp
)
ENDIF()

# generate rules for building source files from the resources
SET(QtApp_RCCS
   qrc/resources.qrc
   ../data/appicon.qrc
)
QT5_ADD_RESOURCES(QtApp_RCC_SRCS ${QtApp_RCCS})


# kde4_automoc(${ring_kde_SRCS})
SET(
   config_ui_files
   conf/dlggeneralbase.ui
   conf/dlgdisplaybase.ui
   conf/dlgaudiobase.ui
   conf/dlgaddressbookbase.ui
   conf/dlghooksbase.ui
   conf/dlgaccessibility.ui
   conf/dlgaudiorecording.ui
   conf/dlgpresence.ui
   conf/dlgfallbackperson.ui
   widgets/ui/personselector.ui
   widgets/ui/contactmethodselector.ui
   widgets/ui/player.ui
   widgets/ui/playeroverlay.ui
   widgets/ui/contactdock.ui
   widgets/ui/transfer.ui
   widgets/ui/presence.ui
   widgets/ui/videodevicesetting.ui
   widgets/ui/screensharingwidget.ui
   widgets/ui/mediafilepicker.ui
   widgets/ui/welcome.ui
   widgets/ui/share.ui
   widgets/ui/accountcreator.ui
)

# add_subdirectory( test   ) #Enable again some day, it cause compile problems for some users

IF(NOT (${ENABLE_VIDEO} MATCHES false))
   SET(ENABLE_VIDEO 1 CACHE BOOLEAN "Enable video")
   ADD_DEFINITIONS( -DENABLE_VIDEO=true )
   SET (
      config_ui_files
      ${config_ui_files}
      conf/dlgvideobase.ui
   )
ENDIF(NOT (${ENABLE_VIDEO} MATCHES false))

IF(${DISABLE_UNIQUE_APPLICATION} MATCHES true)
   MESSAGE("KUniqueApplication disabled")
   SET(DISABLE_UNIQUE_APPLICATION 1 CACHE BOOLEAN "Disable KUniqueApplication")
   ADD_DEFINITIONS( -DDISABLE_UNIQUE_APPLICATION=true )
ENDIF(${DISABLE_UNIQUE_APPLICATION} MATCHES true)

ki18n_wrap_ui(ring_kde_SRCS ui/View_base.ui  ${config_ui_files}  )
ki18n_wrap_ui(ring_kde_SRCS ${config_ui_files} )

ADD_EXECUTABLE(ring-kde ${ring_kde_SRCS} ${QtApp_RCC_SRCS})

IF(NOT (${ENABLE_VIDEO} MATCHES false))
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   SET( OPENGL_link   ${QT_QTOPENGL_LIBRARY} )# -lGLU
ELSE()
   SET( OPENGL_link   ${QT_QTOPENGL_LIBRARY} -lGL)# -lGLU
ENDIF()
ENDIF(NOT (${ENABLE_VIDEO} MATCHES false))

IF(NOT ${ENABLE_STATIC} MATCHES false)
  FIND_PACKAGE(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED DBus)
  INCLUDE_DIRECTORIES(SYSTEM ${Qt5DBus_INCLUDE_DIRS} )
  LINK_DIRECTORIES(${Qt5DBus_LIBRARY_DIRS} )
  ADD_DEFINITIONS(${Qt5DBus_CFLAGS})
  TARGET_LINK_LIBRARIES(ring-kde
    ${Qt5DBus_LIBRARIES}
    ${LIB_RING_CLIENT_LIBRARY_STATIC}
    -lpthread
    -lrt
  )
ELSE()
   TARGET_LINK_LIBRARIES(ring-kde ${LIB_RING_CLIENT_LIBRARY_DYNAMIC})
ENDIF()

QT5_ADD_DBUS_ADAPTOR(ring_kde_SRCS ../data/org.kde.kuiserver.xml cmd.h Cmd )

TARGET_LINK_LIBRARIES(ring-kde
  libkring
  tip
  accountdialog
  ${LIB_RING_CLIENT_LIBRARY}
  ${OPENGL_link}

  # Qt5
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  Qt5::Svg
  Qt5::OpenGL
  Qt5::PrintSupport

  # KF5
  KF5::I18n
  KF5::WidgetsAddons
  KF5::ConfigCore
  KF5::ConfigGui
  KF5::DBusAddons
  KF5::XmlGui
  KF5::Notifications
  KF5::IconThemes
  KF5::KIOWidgets
  KF5::Completion
  KF5::Crash
  KF5::NotifyConfig
  KF5::GlobalAccel
)

IF (Qt5TextToSpeech_FOUND)
   TARGET_LINK_LIBRARIES(ring-kde
      Qt5TextToSpeech
   )
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  TARGET_LINK_LIBRARIES(ring-kde ${OSX_FRAMEWORK_OPENGL})
ENDIF()

########### install files ###############

INSTALL( TARGETS ring-kde                     DESTINATION ${BIN_INSTALL_DIR}           )
INSTALL( FILES icons/overlay_right_corner.svg DESTINATION ${DATA_INSTALL_DIR}/ring-kde )
INSTALL( FILES icons/overlay_left_corner.svg  DESTINATION ${DATA_INSTALL_DIR}/ring-kde )
