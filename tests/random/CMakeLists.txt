ENABLE_TESTING()
ADD_TEST(test_seed1 ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/random" "-f" "wrap_test('test_seed1')")
