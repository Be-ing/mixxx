# This file is part of Mixxx, Digital DJ'ing software.
# Copyright (C) 2001-2020 Mixxx Development Team
# Distributed under the GNU General Public Licence (GPL) version 2 or any later
# later version. See the LICENSE file for details.

#[=======================================================================[.rst:
FindFdkAac
-----------

Finds the FdkAac library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``FdkAac::FdkAac``
  The FdkAac library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``FdkAac_FOUND``
  True if the system has the FdkAac library.
``FdkAac_INCLUDE_DIRS``
  Include directories needed to use FdkAac.
``FdkAac_LIBRARIES``
  Libraries needed to link to FdkAac.
``FdkAac_DEFINITIONS``
  Compile definitions needed to use FdkAac.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``FdkAac_INCLUDE_DIR``
  The directory containing ``ebur128.h``.
``FdkAac_LIBRARY``
  The path to the FdkAac library.

#]=======================================================================]

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(PC_FdkAac QUIET fdk-aac)
endif()

find_path(FdkAac_INCLUDE_DIR
  NAMES FDK_audio.h
  PATHS ${PC_FdkAac_INCLUDE_DIRS}
  PATH_SUFFIXES fdk-aac
  DOC "FdkAac include directory")
mark_as_advanced(FdkAac_INCLUDE_DIR)

find_library(FdkAac_LIBRARY
  NAMES fdk-aac
  PATHS ${PC_FdkAac_LIBRARY_DIRS}
  DOC "FdkAac library"
)
mark_as_advanced(FdkAac_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  FdkAac
  DEFAULT_MSG
  FdkAac_LIBRARY
  FdkAac_INCLUDE_DIR
)

if(FdkAac_FOUND)
  set(FdkAac_LIBRARIES "${FdkAac_LIBRARY}")
  set(FdkAac_INCLUDE_DIRS "${FdkAac_INCLUDE_DIR}")
  set(FdkAac_DEFINITIONS ${PC_FdkAac_CFLAGS_OTHER})

  if(NOT TARGET FdkAac::FdkAac)
    add_library(FdkAac::FdkAac UNKNOWN IMPORTED)
    set_target_properties(FdkAac::FdkAac
      PROPERTIES
        IMPORTED_LOCATION "${FdkAac_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_FdkAac_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${FdkAac_INCLUDE_DIR}"
    )
  endif()
endif()
