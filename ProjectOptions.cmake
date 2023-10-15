include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(patterns_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(patterns_setup_options)
  option(patterns_ENABLE_HARDENING "Enable hardening" ON)
  option(patterns_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    patterns_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    patterns_ENABLE_HARDENING
    OFF)

  patterns_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR patterns_PACKAGING_MAINTAINER_MODE)
    option(patterns_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(patterns_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(patterns_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(patterns_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(patterns_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(patterns_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(patterns_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(patterns_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(patterns_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(patterns_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(patterns_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(patterns_ENABLE_PCH "Enable precompiled headers" OFF)
    option(patterns_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(patterns_ENABLE_IPO "Enable IPO/LTO" ON)
    option(patterns_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(patterns_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(patterns_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(patterns_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(patterns_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(patterns_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(patterns_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(patterns_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(patterns_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(patterns_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(patterns_ENABLE_PCH "Enable precompiled headers" OFF)
    option(patterns_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      patterns_ENABLE_IPO
      patterns_WARNINGS_AS_ERRORS
      patterns_ENABLE_USER_LINKER
      patterns_ENABLE_SANITIZER_ADDRESS
      patterns_ENABLE_SANITIZER_LEAK
      patterns_ENABLE_SANITIZER_UNDEFINED
      patterns_ENABLE_SANITIZER_THREAD
      patterns_ENABLE_SANITIZER_MEMORY
      patterns_ENABLE_UNITY_BUILD
      patterns_ENABLE_CLANG_TIDY
      patterns_ENABLE_CPPCHECK
      patterns_ENABLE_COVERAGE
      patterns_ENABLE_PCH
      patterns_ENABLE_CACHE)
  endif()

  patterns_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (patterns_ENABLE_SANITIZER_ADDRESS OR patterns_ENABLE_SANITIZER_THREAD OR patterns_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(patterns_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(patterns_global_options)
  if(patterns_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    patterns_enable_ipo()
  endif()

  patterns_supports_sanitizers()

  if(patterns_ENABLE_HARDENING AND patterns_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR patterns_ENABLE_SANITIZER_UNDEFINED
       OR patterns_ENABLE_SANITIZER_ADDRESS
       OR patterns_ENABLE_SANITIZER_THREAD
       OR patterns_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${patterns_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${patterns_ENABLE_SANITIZER_UNDEFINED}")
    patterns_enable_hardening(patterns_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(patterns_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(patterns_warnings INTERFACE)
  add_library(patterns_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  patterns_set_project_warnings(
    patterns_warnings
    ${patterns_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(patterns_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(patterns_options)
  endif()

  include(cmake/Sanitizers.cmake)
  patterns_enable_sanitizers(
    patterns_options
    ${patterns_ENABLE_SANITIZER_ADDRESS}
    ${patterns_ENABLE_SANITIZER_LEAK}
    ${patterns_ENABLE_SANITIZER_UNDEFINED}
    ${patterns_ENABLE_SANITIZER_THREAD}
    ${patterns_ENABLE_SANITIZER_MEMORY})

  set_target_properties(patterns_options PROPERTIES UNITY_BUILD ${patterns_ENABLE_UNITY_BUILD})

  if(patterns_ENABLE_PCH)
    target_precompile_headers(
      patterns_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(patterns_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    patterns_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(patterns_ENABLE_CLANG_TIDY)
    patterns_enable_clang_tidy(patterns_options ${patterns_WARNINGS_AS_ERRORS})
  endif()

  if(patterns_ENABLE_CPPCHECK)
    patterns_enable_cppcheck(${patterns_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(patterns_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    patterns_enable_coverage(patterns_options)
  endif()

  if(patterns_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(patterns_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(patterns_ENABLE_HARDENING AND NOT patterns_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR patterns_ENABLE_SANITIZER_UNDEFINED
       OR patterns_ENABLE_SANITIZER_ADDRESS
       OR patterns_ENABLE_SANITIZER_THREAD
       OR patterns_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    patterns_enable_hardening(patterns_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
