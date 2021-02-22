ENABLE_TESTING()
ADD_TEST(test_ctype1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/external" "-f" "wrap_test('test_ctype1')")
