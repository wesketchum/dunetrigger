
foreach(INC_ALGO ${ALGS_H})
  set_alg_type_name_lib(${INC_ALGO})

  if ("${ALGONAME}" STREQUAL "")
    continue()
  endif()

  message(STATUS "Generating auto-compiling file for header " ${INC_ALGO})

  set(EXECNAME "main_${ALGONAME}")
  configure_file(main.cxx.in ${EXECNAME}.cxx "@ONLY")

  add_executable(${EXECNAME} ${EXECNAME}.cxx)

  target_include_directories(${EXECNAME} PUBLIC ${PROJECT_SOURCE_DIR})
  target_include_directories(${EXECNAME} PUBLIC ${PROJECT_SOURCE_DIR}/includes)
  
  target_link_libraries(${EXECNAME} PUBLIC ${ALGOLIBNAME}) 
  
  target_compile_options(${EXECNAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
    )

  
  
endforeach()
