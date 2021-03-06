cmake_minimum_required(VERSION 3.1)
project (AMPE C CXX Fortran)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

find_package(MPI)
if(MPI_FOUND)
  message(STATUS
    "MPI was found:\n"
    "  MPI_CXX_COMPILER: ${MPI_CXX_COMPILER}\n"
    "  MPIEXEC_NUMPROC_FLAG: ${MPIEXEC_NUMPROC_FLAG}\n"
    "  MPIEXEC_MAX_NUMPROCS: ${MPIEXEC_MAX_NUMPROCS}\n"
    "  MPIEXEC: ${MPIEXEC}\n"
 )
endif()
if(DEFINED MPI_EXEC)
  set(MPIEXEC ${MPI_EXEC})
endif()

message(STATUS "MPIEXEC: ${MPIEXEC}\n")

set(HDF5_USE_STATIC_LIBRARIES ON)
find_package(HDF5 COMPONENTS CXX HL REQUIRED)
message(STATUS ${HDF5_VERSION} )
include_directories(${HDF5_INCLUDE_DIR})
message(STATUS "hdf5 libraries: ${HDF5_LIBRARIES}")
set(PROJECT_LINK_LIBS_HDF5 ${HDF5_HL_LIBRARIES} ${HDF5_LIBRARIES})
message(STATUS "hdf5 libraries: ${PROJECT_LINK_LIBS_HDF5}")

find_package(Boost)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

if(${USE_CVODE})
  message(STATUS "Use CVODE")
else()
  message(STATUS "Use CPODES")
  add_definitions(-DUSE_CPODE)
endif()

# clang-format (optional)
set(AMPE_WITH_CLANG_FORMAT FALSE CACHE BOOL "Indent code with clang-format")
if(${AMPE_WITH_CLANG_FORMAT})
  find_package(CLANG_FORMAT)
  if(${CLANG_FORMAT_FOUND})
    message(STATUS "Indent with clang-format")
    file(GLOB_RECURSE FORMAT_SOURCES source/*.C source/*.h tests/*.C tests/*.h)
    add_custom_target(format
      COMMAND ${CLANG_FORMAT_EXECUTABLE} -i -style=file ${FORMAT_SOURCES}
      DEPENDS ${FORMAT_SOURCES})
  endif(${CLANG_FORMAT_FOUND})
endif()

include(FortranCInterface)
FortranCInterface_HEADER(
    fc_samrai_mangle.h
    MACRO_NAMESPACE "FC_SAMRAI_"
    SYMBOLS
       ADJUSTRHS2D
       ADJUSTRHS3D
       ADJBDRYCONSTOFFDIAGS2D
       COMPDIAGVARIABLEC2D
       COMPDIAGVARIABLEC3D
       COMPDIAGSCALARC2D
       COMPDIAGSCALARC3D
       COMPDIAGZEROC2D
       COMPDIAGZEROC3D
       MULTIPLYOFFDIAGBYM
       EFO_COMPFLUXVARDC2D
       EFO_RBGSWITHFLUXMAXVARDCVARSF2D
       EFO_RBGSWITHFLUXMAXCONDCVARSF2D
       EFO_RBGSWITHFLUXMAXVARDCCONSF2D
       EFO_RBGSWITHFLUXMAXCONDCCONSF2D
       EFO_COMPRESVARSCA2D
       EFO_COMPRESCONSCA2D
       ACCUMOPVARSCA2D
       ACCUMOPCONSCA2D
       EFO_EWINGFIXFLUXVARDC2D
       EFO_EWINGFIXFLUXCONDC2D
       EFO_COMPFLUXVARDC3D
       EFO_RBGSWITHFLUXMAXVARDCVARSF3D
       EFO_RBGSWITHFLUXMAXCONDCVARSF3D
       EFO_RBGSWITHFLUXMAXVARDCCONSF3D
       EFO_RBGSWITHFLUXMAXCONDCCONSF3D
       EFO_COMPRESVARSCA3D
       EFO_COMPRESCONSCA3D
       ACCUMOPVARSCA3D
       ACCUMOPCONSCA3D
       EFO_EWINGFIXFLUXVARDC3D
       EFO_EWINGFIXFLUXCONDC3D
)      
FortranCInterface_HEADER(
    fc_mangle.h
    MACRO_NAMESPACE "FC_INTERNAL_"
    SYMBOLS
        WELL_FUNC
        DERIV_WELL_FUNC
        SECOND_DERIV_WELL_FUNC
        INTERP_FUNC
        DERIV_INTERP_FUNC
        SECOND_DERIV_INTERP_FUNC
        AVERAGE_FUNC
        DERIV_AVERAGE_FUNC
        INTERP_RATIO_FUNC
        COMPL_INTERP_RATIO_FUNC
        CONCENTRATIONFLUX
        ADD_CONCENTRATIONFLUX_EBS
        CONCENTRATIONFLUX_SPINODAL
        CONCENTRATION_PFMDIFFUSION
        CONCENTRATION_PFMDIFFUSION_OF_TEMPERATURE
        CONCENTRATION_DIFFCOEFF_OF_TEMPERATURE
        CONCENTRATIONDIFFUSION_BECKERMANN
        COMPUTERHSCONCENTRATION
        PHASERHS_FENERGY
        ETARHS_FENERGY
        CALPHAD_CONC_SOLV_THREE
        CALPHAD_CONC_SOLV_TWO
        CALPHAD_CONC_CEQ_TWO
        ADDCONCENTRATIONFLUXFROMGRADT
        ADDCONCENTRATIONFLUXFROMANTITRAPPING
        INITGAUSSIAN
        INITGRADIENT
        INITGAUSSIANSOURCE
        LINEARMELTINGLINE
        COMPUTE_CONCENTRATION_FROM_PHASE_CONCENTRATIONS
        SINGLE_INDEX_STIFFNESS
        STORAGE_INDEX_STIFFNESS
        SINGLE_INDEX_STIFFNESS
        STORAGE_INDEX_STIFFNESS
        GRADIENT_FLUX
        COMPUTE_FLUX_ISOTROPIC
        ANISOTROPIC_GRADIENT_FLUX
        COMPUTERHSPBG
        ADDVDPHIDX
        COMPUTERHSTEMP
        COMPUTERHSBIASWELL
        COMPUTERHSBIASWELLBECKERMANN
        COMPUTERHSDELTATEMPERATURE
        COMPUTERHSETA
        DIFFS
        GRAD_CELL
        GRAD_SIDE
        GRAD_SIDE_ISOTROPIC
        QUAT_SYMM_ROTATION
        QUAT_FUNDAMENTAL
        QUATDIFFS
        QUATDIFFS_SYMM
        QUATGRAD_CELL
        QUATGRAD_CELL_SYMM
        QUATGRAD_SIDE
        QUATGRAD_SIDE_SYMM
        QUATGRAD_SIDE_SYMM_ISOTROPIC
        QUATGRAD_SIDE_ISOTROPIC
        QUATGRAD_MODULUS
        QUATGRAD_MODULUS_FROM_SIDES
        QUATGRAD_MODULUS_FROM_SIDES_COMPACT
        QUATDIFFUSION
        QUATDIFFUSIONDERIV
        CORRECTRHSQUATFORSYMMETRY
        TAGFROMGRADS
        TAGFROMQUATGRADS
        QUATMOBILITY
        QUATMOBILITYDERIV
        QUATENERGY
        TEMPERATURE_ENERGY
        QUATAVG
        SMOOTHQUAT
        SETTOZERO
        SOURCE_TEMPERATURE
        HEAT_CAPACITY_NKR
        VELOCITY
        COMPUTE_FACE_COEF2D
        COMPUTE_FACE_COEF3D
        COMPUTE_DQUATDPHI_FACE_COEF2D
        COMPUTE_DQUATDPHI_FACE_COEF3D
        COMPUTE_FLUX2D
        COMPUTE_FLUX3D
        COMPUTE_FLUX2D_FROM_GRADQ
        COMPUTE_FLUX3D_FROM_GRADQ
        COMPUTE_SYM_FLUX2D
        COMPUTE_SYM_FLUX3D
        COMPUTE_Q_RESIDUAL2D
        COMPUTE_Q_RESIDUAL3D
        COMPUTE_Q_RESIDUAL2D_SYMM
        COMPUTE_Q_RESIDUAL3D_SYMM
        ADD_QUAT_OP2D
        ADD_QUAT_OP3D
        ADD_QUAT_PROJ_OP2D
        ADD_QUAT_PROJ_OP3D
        ADD_QUAT_PROJ_OP2D_SYMM
        ADD_QUAT_PROJ_OP3D_SYMM
        COMPUTE_LAMBDA_FLUX2D
        COMPUTE_LAMBDA_FLUX3D
        COMPUTE_LAMBDA_FLUX2D_SYMM
        COMPUTE_LAMBDA_FLUX3D_SYMM
        FIXFLUX2D
        FIXFLUX3D
        PROJECT2D
        PROJECT3D
        TAKE_SQUARE_ROOT2D
        TAKE_SQUARE_ROOT3D
        MULTICOMPONENT_MULTIPLY2D
        MULTICOMPONENT_MULTIPLY3D
        MULTICOMPONENT_DIVIDE2D
        MULTICOMPONENT_DIVIDE3D
        QUATLINREFINE2D
        QUATLINREFINE3D
        SET_J_IJ2D
        SET_J_IJ3D
        SET_STENCIL2D
        SET_STENCIL3D
        SET_SYMMETRIC_STENCIL2D
        SET_SYMMETRIC_STENCIL3D
        COPY2D
        COPY3D
        ADJUST_BDRY2D
        ADJUST_BDRY3D
        ADJUST_QRHS2D
        ADJUST_QRHS3D
        QUATCOARSEN
        COMPUTEDPHIDTEMPERATUREDELTATEMPERATURE
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

link_directories( ${SAMRAI_DIR}/lib )
link_directories( ${HYPRE_DIR}/lib )
if(${USE_CVODE})
  link_directories( ${SUNDIALS_DIR}/lib64 )
else()
  link_directories( ${SUNDIALS_DIR}/lib )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(NetCDF REQUIRED COMPONENTS CXX)

set( PROJECT_LINK_LIBS1 libSAMRAI_tbox.a )
set( PROJECT_LINK_LIBS2 libSAMRAI_math.a )
set( PROJECT_LINK_LIBS3 libSAMRAI_hier.a )
set( PROJECT_LINK_LIBS4 libSAMRAI_pdat.a )
set( PROJECT_LINK_LIBS5 libSAMRAI_xfer.a )
set( PROJECT_LINK_LIBS6 libSAMRAI_geom.a )
set( PROJECT_LINK_LIBS7 libSAMRAI_mesh.a )
set( PROJECT_LINK_LIBS8 libSAMRAI_solv.a )
set( PROJECT_LINK_LIBS9 libSAMRAI_appu.a )
set( PROJECT_LINK_LIBS_SAMRAI ${PROJECT_LINK_LIBS9}
                              ${PROJECT_LINK_LIBS8}
                              ${PROJECT_LINK_LIBS7}
                              ${PROJECT_LINK_LIBS6}
                              ${PROJECT_LINK_LIBS5}
                              ${PROJECT_LINK_LIBS4}
                              ${PROJECT_LINK_LIBS3}
                              ${PROJECT_LINK_LIBS2}
                              ${PROJECT_LINK_LIBS1} )

set( PROJECT_LINK_LIBS_HYPRE libHYPRE.so )

message(STATUS "Link with Sundials...")
set( PROJECT_LINK_LIBS_CVODE libsundials_cvode.a )
if(${USE_CVODE})
  message(STATUS "Do not link with CPODES...")
else()
  message(STATUS "Link with CPODES...")
  set( PROJECT_LINK_LIBS_CPODES libsundials_cpodes.a )
endif()

set( PROJECT_LINK_LIBS_NETCDF NetCDF::NetCDF_CXX )

include_directories( ${SAMRAI_DIR}/include )
include_directories( ${HYPRE_DIR}/include )
include_directories( ${SUNDIALS_DIR}/include )

enable_testing()

add_definitions(-DNDIM=${NDIM})

enable_language(Fortran C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -std=c++11")

add_subdirectory(source)

add_subdirectory(tests)
