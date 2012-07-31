CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_DEFINITIONS("-std=c++0x")

ADD_DEFINITIONS(
	${QT_DEFINITIONS} 
	-fexceptions
)

PROJECT(ksflphone)

SET ( KDE4_KABC_LIBS  -lkabc )


add_subdirectory(dataengine)

SET(LOCAL_CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
SET(CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}")

FIND_PACKAGE ( KDE4 REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )

INCLUDE ( KDE4Defaults )

set(GENERIC_LIB_VERSION "1.1.0")

INCLUDE_DIRECTORIES ( ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

#File to compile
set( ksflphone_LIB_SRCS
   helperfunctions.cpp
   akonadibackend.cpp
   sortabledockcommon.cpp
   configurationskeleton.cpp
)

KDE4_ADD_KCFG_FILES(ksflphone_LIB_SRCS kcfg_settings.kcfgc)
 
kde4_add_library( ksflphone  SHARED ${ksflphone_LIB_SRCS} )
 
target_link_libraries( ksflphone
  qtsflphone
  ${QT_QTCORE_LIBRARY}
  ${KDEPIMLIBS_AKONADI_KMIME_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
  ${KDEPIMLIBS_AKONADI_CONTACT_LIBS}
  ${KDE4_KDEUI_LIBS}
)

set_target_properties( ksflphone
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)

set( ksflphone_LIB_HDRS
  akonadibackend.h
  helperfunctions.h
  sortabledockcommon.h
)

INSTALL(FILES sflphone-client-kde.kcfg DESTINATION ${KCFG_INSTALL_DIR})

install( FILES ${ksflphone_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/ksflphone
  COMPONENT Devel
)
 
install( TARGETS ksflphone  ${INSTALL_TARGETS_DEFAULT_ARGS} )