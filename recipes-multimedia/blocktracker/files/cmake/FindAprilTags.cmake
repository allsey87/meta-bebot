#=============================================================================
# - Find the AprilTags library
#=============================================================================
# This module defines
# 	AprilTags_INCLUDE_DIR, where to find TagDetector.h, etc.
# 	AprilTags_LIB, libraries to link against to use AprilTags.
# 	AprilTags_FOUND, libraries to link against to use AprilTags.
#
#=============================================================================

SET (AprilTags_FOUND 0)

FIND_PATH (AprilTags_INCLUDE_DIR
  NAMES TagDetector.h
  PATH_SUFFIXES apriltags
  DOC "The AprilTags include directory"
)

FIND_LIBRARY (AprilTags_LIB
  NAMES apriltags
  DOC "The AprilTags shared library"
)

#=============================================================================

INCLUDE (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS (AprilTags
				   FOUND_VAR AprilTags_FOUND
			 	   REQUIRED_VARS AprilTags_LIB AprilTags_INCLUDE_DIR)

IF (NOT AprilTags_FOUND)
  MESSAGE (WARNING "AprilTags has not been found!")
ENDIF (NOT AprilTags_FOUND)
