ENABLE_TESTING()
ADD_TEST(test_bitand1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/binary" "-f" "wrap_test('test_bitand1')")
ADD_TEST(test_bitor1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/binary" "-f" "wrap_test('test_bitor1')")
ADD_TEST(test_bitxor1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/binary" "-f" "wrap_test('test_bitxor1')")
