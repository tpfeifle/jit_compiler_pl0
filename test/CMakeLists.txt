set(TEST_SOURCES
    # add your *.cpp files here
    parser/TestParser.cpp
    lexer/TestLexer.cpp
    Tester.cpp
    )

add_executable(tester ${TEST_SOURCES})
target_link_libraries(tester PUBLIC
    pljit_core
    GTest::GTest)
