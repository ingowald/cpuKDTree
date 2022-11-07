# ======================================================================== #
# Copyright 2021-2022 Ingo Wald                                            #
#                                                                          #
# Licensed under the Apache License, Version 2.0 (the "License");          #
# you may not use this file except in compliance with the License.         #
# You may obtain a copy of the License at                                  #
#                                                                          #
#     http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                          #
# Unless required by applicable law or agreed to in writing, software      #
# distributed under the License is distributed on an "AS IS" BASIS,        #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
# See the License for the specific language governing permissions and      #
# limitations under the License.                                           #
# ======================================================================== #

cmake_policy(SET CMP0048 NEW)
project(cpuKDTree VERSION 0.0.1 LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.18)
  cmake_policy(SET CMP0104 NEW)
endif()

if (NOT (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}))
  set(CPUKD_IS_SUBMODULE ON)
else()
  set(CPUKD_IS_SUBPROJECT OFF)
endif()

# ------------------------------------------------------------------
# general cmake project configs
# ------------------------------------------------------------------
if (NOT CPUKD_IS_SUBPROJECT)
  if(NOT SET_UP_CONFIGURATIONS_DONE)
    set(SET_UP_CONFIGURATIONS_DONE 1)

    # No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator
    # Also no reason mess with CMAKE_BUILD_TYPE if it's a multiconfig generator.
    if(CMAKE_CONFIGURATION_TYPES) # multiconfig generator?
      set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 
    else()
      if(NOT CMAKE_BUILD_TYPE)
	#            message("Defaulting to release build.")
        set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
      endif()
      set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
      # set the valid options for cmake-gui drop-down list
      set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
    endif()
  endif()

  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(cmake/configure_tbb.cmake)

add_library(cpuKDTree INTERFACE)
target_sources(cpuKDTree INTERFACE
  cpukd/common.h
  cpukd/builder.h
  cpukd/fcp.h
  cpukd/knn.h
  )
target_include_directories(cpuKDTree INTERFACE
  ${PROJECT_SOURCE_DIR}/
  )
if (CPUKD_HAVE_TBB)
  target_include_directories(cpuKDTree INTERFACE ${TBB_INCLUDE_DIR})
  target_compile_definitions(cpuKDTree INTERFACE CPUKD_HAVE_TBB=1)
  target_link_libraries(cpuKDTree INTERFACE tbb)
endif()

add_executable(cpukd_test_float4-fcp testing/float4-fcp.cpp)
target_link_libraries(cpukd_test_float4-fcp cpuKDTree)

#add_executable(cpukd_test_float4-knn testing/float4-knn.cpp)
#target_link_libraries(cpukd_test_float4-knn cpuKDTree)

#add_executable(cpukd_test_float3-knn testing/float3-knn.cpp)
#target_link_libraries(cpukd_test_float3-knn cpuKDTree)

  




