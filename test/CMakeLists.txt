set(TEST_SOURCES
        Tester.cpp
        function/TestFunctionHandle.cpp
        parser/TestParser.cpp
        lexer/TestLexer.cpp
        ast/TestAst.cpp
        ir/TestOptimizationDeadCode.cpp
        ir/TestOptimizationConstPropagation.cpp
        )

add_executable(tester ${TEST_SOURCES})
target_link_libraries(tester PUBLIC
        pljit_core
        GTest::GTest)