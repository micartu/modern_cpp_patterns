include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(patterns_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#9.1.0")
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.11.0
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()

  if(NOT TARGET CLI11::CLI11)
    cpmaddpackage("gh:CLIUtils/CLI11@2.3.2")
  endif()

  #if(NOT TARGET Boost::Boost)
  #  cpmaddpackage("gh:boostorg/boost#boost-1.81.0")
  #endif()

  # instead try to load it
  set(BOOST_SUBVER "84")
  set(BOOST_CMAKE_SCRIPT_PATH contrib/subset-boost/stage/lib/cmake/Boost-1.${BOOST_SUBVER}.0/BoostConfig.cmake)
  set(BOOST_CMAKE_SCRIPT_ABS_PATH "${CMAKE_SOURCE_DIR}/${BOOST_CMAKE_SCRIPT_PATH}")
  if (NOT "${BOOST_CMAKE_SCRIPT_ABS_PATH}")
    execute_process(
      COMMAND ${CMAKE_SOURCE_DIR}/scripts/download_subboost.sh "${BOOST_CMAKE_SCRIPT_ABS_PATH}" ${BOOST_SUBVER}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
  endif()

  include_directories(contrib/subset-boost)
  include(${BOOST_CMAKE_SCRIPT_PATH})
endfunction()
