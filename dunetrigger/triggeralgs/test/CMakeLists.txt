add_executable(test_factory test_factory.cxx)
target_link_libraries(test_factory PRIVATE triggeralgs trgdataformats::trgdataformats)
target_include_directories(test_factory PRIVATE ${BOOST_INCLUDE_DIRS})
add_test(NAME factory COMMAND test_factory)
