
set(LIBUDEV_FIND_ARGS
  HINTS
    ENV LIBUDEV_DIR
  PATHS
    /usr
    /usr/local
)

if(NOT LIBUDEV_LIBRARY)
  find_library(LIBUDEV_LIBRARY
    NAMES
      udev
      libudev
      udev.so.1
      libudev.so.1
    ${LIBUDEV_FIND_ARGS}
    PATH_SUFFIXES
      lib
  )
endif()

unset(LIBUDEV_FIND_ARGS)

if(LIBUDEV_LIBRARY)
  if(NOT LibUdev_FIND_QUIETLY)
    message(STATUS " Found LibUdev: " ${LIBUDEV_LIBRARY})
  endif()
  set(LIBUDEV_LIBRARIES "${LIBUDEV_LIBRARY}")
else()
  unset(LIBUDEV_LIBRARIES)
  if(NOT LibUdev_FIND_QUIETLY)
    message(ERROR " LibUdev has not been found")
  endif()
endif()
