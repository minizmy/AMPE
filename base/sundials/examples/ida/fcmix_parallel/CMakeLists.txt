# ---------------------------------------------------------------
# $Revision: 1.3 $
# $Date: 2007/10/25 20:03:34 $
# ---------------------------------------------------------------
# Programmer:  Radu Serban @ LLNL
# ---------------------------------------------------------------
# Copyright (c) 2007, The Regents of the University of California.
# Produced at the Lawrence Livermore National Laboratory.
# All rights reserved.
# For details, see the LICENSE file.
# ---------------------------------------------------------------
# CMakeLists.txt file for the FIDA parallel examples


# Add variable ida_examples with the names of the parallel IDA examples

SET(FIDA_examples
  fidaHeat2D_kry_bbd_p
  )

# Check whether we use MPI compiler scripts.
# If yes, then change the Fortran compiler to the MPIF77 script.
# If not, then add the MPI include directory for MPI headers.

IF(MPI_USE_MPISCRIPTS )
  # use MPI_MPIF77 as the compiler
  SET(CMAKE_Fortran_COMPILER ${MPI_MPIF77})
ELSE(MPI_USE_MPISCRIPTS)
  # add MPI_INCLUDE_PATH to include directories
  INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
ENDIF(MPI_USE_MPISCRIPTS)

# Specify libraries to link against (through the target that was used to 
# generate them) based on the value of the variable LINK_LIBRARY_TYPE

IF(LINK_LIBRARY_TYPE MATCHES "static")
  SET(IDA_LIB sundials_ida_static)
  SET(NVECP_LIB sundials_nvecparallel_static)
  SET(FNVECP_LIB sundials_fnvecparallel_static)
ELSE(LINK_LIBRARY_TYPE MATCHES "static")
  SET(IDA_LIB sundials_ida_shared)
  SET(NVECP_LIB sundials_nvecparallel_shared)
  SET(FNVECP_LIB sundials_fnvecparallel_shared)
ENDIF(LINK_LIBRARY_TYPE MATCHES "static")

# Only static FCMIX libraries are available

SET(FIDA_LIB sundials_fida_static)

# Add the build and install targets for each IDA example

FOREACH(example ${FIDA_examples})
  # add build target for example
  ADD_EXECUTABLE(${example} ${example}.f)
  # link to the sundials libraries
  TARGET_LINK_LIBRARIES(${example} 
    ${FIDA_LIB} ${IDA_LIB}
    ${FNVECP_LIB} ${NVECP_LIB}
    ${EXTRA_LINK_LIBS})
  # if needed, link to the MPI libraries
  IF(NOT MPI_USE_MPISCRIPTS)
    TARGET_LINK_LIBRARIES(${example} ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARIES})
  ENDIF(NOT MPI_USE_MPISCRIPTS)
  IF(EXAMPLES_INSTALL)
    # install example files
    INSTALL(FILES ${example}.f ${example}.out DESTINATION ${EXAMPLES_INSTALL_PATH}/ida/fcmix_parallel)
  ENDIF(EXAMPLES_INSTALL)
ENDFOREACH(example ${FIDA_examples})

IF(EXAMPLES_INSTALL)

  # Install the README file
  INSTALL(FILES README DESTINATION ${EXAMPLES_INSTALL_PATH}/ida/fcmix_parallel)

  # Prepare substitution variables for Makefile and/or CMakeLists templates
  SET(SOLVER "IDA")
  SET(SOLVER_LIB "sundials_ida")
  SET(SOLVER_FLIB "sundials_fida")
  LIST2STRING(FIDA_examples EXAMPLES)

  # Regardless of the platform we're on, we will generate and install 
  # CMakeLists.txt file for building the examples. This file  can then 
  # be used as a template for the user's own programs.

  # generate CMakelists.txt in the binary directory
  CONFIGURE_FILE(
      ${PROJECT_SOURCE_DIR}/examples/templates/cmakelists_parallel_F77_ex.in
      ${PROJECT_BINARY_DIR}/examples/ida/fcmix_parallel/CMakeLists.txt
      @ONLY
      )

  # install CMakelists.txt
  INSTALL(
    FILES ${PROJECT_BINARY_DIR}/examples/ida/fcmix_parallel/CMakeLists.txt
    DESTINATION ${EXAMPLES_INSTALL_PATH}/ida/fcmix_parallel 
    )

  # On UNIX-type platforms, we have the option of generating and instaling
  # a makefile for building the examples. This makefile can then be used
  # as a template for the user's own programs.

  IF(EXAMPLES_GENERATE_MAKEFILES)

    # generate Makefile and place it in the binary dir
    CONFIGURE_FILE(
      ${PROJECT_SOURCE_DIR}/examples/templates/makefile_parallel_F77_ex.in
      ${PROJECT_BINARY_DIR}/examples/ida/fcmix_parallel/Makefile_ex
      @ONLY
      )

    # install the configured Makefile_ex as Makefile
    INSTALL(
      FILES ${PROJECT_BINARY_DIR}/examples/ida/fcmix_parallel/Makefile_ex
      DESTINATION ${EXAMPLES_INSTALL_PATH}/ida/fcmix_parallel 
      RENAME Makefile
      )

  ENDIF(EXAMPLES_GENERATE_MAKEFILES)

ENDIF(EXAMPLES_INSTALL)
