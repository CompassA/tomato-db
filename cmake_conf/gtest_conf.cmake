function(tomato_db_test test_file)
    get_filename_component(test_target_name "${test_file}" NAME_WE)

    add_executable("${test_target_name}")
    target_sources("${test_target_name}"
        PRIVATE
            "${test_file}"
    )
    target_link_libraries("${test_target_name}" ${PROJECT_NAME} gmock gtest gtest_main)

    add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
endfunction(tomato_db_test)