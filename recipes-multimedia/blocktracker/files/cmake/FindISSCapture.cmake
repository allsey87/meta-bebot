#=============================================================================
# - Find the ISS Capture library
#=============================================================================
# This module defines
# 	ISSCapture_INCLUDE_DIR, where to find TagDetector.h, etc.
# 	ISSCapture_LIB, libraries to link against to use AprilTags.
# 	ISSCapture_FOUND, libraries to link against to use AprilTags.
#
#=============================================================================

SET (ISSCapture_FOUND 0)

FIND_PATH (ISSCapture_INCLUDE_DIR
  NAMES iss_capture.h
  PATH_SUFFIXES isscapture
  DOC "The ISS Capture include directory"
)

FIND_LIBRARY (ISSCapture_LIB
  NAMES isscapture
  DOC "The ISS Capture shared library"
)

#=============================================================================

INCLUDE (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS (ISSCapture
				   FOUND_VAR ISSCapture_FOUND
			 	   REQUIRED_VARS ISSCapture_LIB ISSCapture_INCLUDE_DIR)

IF (NOT ISSCapture_FOUND)
  MESSAGE (WARNING "The ISS Capture has not been found!")
ENDIF (NOT ISSCapture_FOUND)
