# Re-check the branch when the sonar_scan target runs. A configure-time check
# is insufficient because the checkout can change branches without CMake being
# reconfigured first.

if(NOT DEFINED SCOPETIMER_SOURCE_DIR OR SCOPETIMER_SOURCE_DIR STREQUAL "")
  message(FATAL_ERROR
    "[sonar_scan] ERROR: SCOPETIMER_SOURCE_DIR is required for the main-branch guard."
  )
endif()

set(_SCOPETIMER_ENV_REF "")
if(DEFINED ENV{GITHUB_REF_NAME} AND NOT "$ENV{GITHUB_REF_NAME}" STREQUAL "")
  set(_SCOPETIMER_ENV_REF "$ENV{GITHUB_REF_NAME}")
endif()

if(_SCOPETIMER_ENV_REF AND NOT _SCOPETIMER_ENV_REF STREQUAL "main")
  message(FATAL_ERROR
    "[sonar_scan] ERROR: GITHUB_REF_NAME is '${_SCOPETIMER_ENV_REF}'; "
    "free-tier analysis is main-only."
  )
endif()

find_program(_SCOPETIMER_GIT_EXECUTABLE NAMES git)
if(NOT _SCOPETIMER_GIT_EXECUTABLE)
  message(FATAL_ERROR
    "[sonar_scan] ERROR: cannot verify the checkout because git was not found."
  )
endif()

execute_process(
  COMMAND "${_SCOPETIMER_GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY "${SCOPETIMER_SOURCE_DIR}"
  RESULT_VARIABLE _SCOPETIMER_GIT_RESULT
  OUTPUT_VARIABLE _SCOPETIMER_GIT_BRANCH
  ERROR_VARIABLE _SCOPETIMER_GIT_ERROR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_STRIP_TRAILING_WHITESPACE
)
if(NOT _SCOPETIMER_GIT_RESULT EQUAL 0 OR _SCOPETIMER_GIT_BRANCH STREQUAL "")
  message(FATAL_ERROR
    "[sonar_scan] ERROR: cannot identify the git checkout; "
    "free-tier analysis is main-only. ${_SCOPETIMER_GIT_ERROR}"
  )
endif()

if(_SCOPETIMER_ENV_REF)
  if(NOT _SCOPETIMER_GIT_BRANCH STREQUAL "HEAD"
      AND NOT _SCOPETIMER_GIT_BRANCH STREQUAL "main")
    message(FATAL_ERROR
      "[sonar_scan] ERROR: GITHUB_REF_NAME is 'main' but the attached git "
      "branch is '${_SCOPETIMER_GIT_BRANCH}'; refusing a mismatched checkout."
    )
  endif()

  if(_SCOPETIMER_GIT_BRANCH STREQUAL "HEAD")
    message(STATUS
      "[sonar_scan] Main-branch guard passed (GITHUB_REF_NAME; detached HEAD)."
    )
  else()
    message(STATUS
      "[sonar_scan] Main-branch guard passed (GITHUB_REF_NAME and git)."
    )
  endif()
elseif(_SCOPETIMER_GIT_BRANCH STREQUAL "HEAD")
  message(FATAL_ERROR
    "[sonar_scan] ERROR: detached HEAD without GITHUB_REF_NAME; "
    "free-tier analysis is main-only."
  )
elseif(NOT _SCOPETIMER_GIT_BRANCH STREQUAL "main")
  message(FATAL_ERROR
    "[sonar_scan] ERROR: current git branch is '${_SCOPETIMER_GIT_BRANCH}'; "
    "free-tier analysis is main-only."
  )
else()
  message(STATUS "[sonar_scan] Main-branch guard passed (git).")
endif()
