set(PLJIT_SOURCES
    # add your *.cpp files here
        ast/AST.cpp
        ast/ASTNode.cpp
        ast/DotASTVisitor.cpp
        ast/SymbolTable.cpp
        ir/Evaluate.cpp
        lexer/Lexer.cpp
        parser/Parser.cpp
        parser/DotPTVisitor.cpp
        parser/PTNode.cpp
        lexer/Token.cpp
        source/SourceReference.cpp
        source/SourceCode.cpp)

add_library(pljit_core STATIC ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

#target_link_libraries(pljit_core PUBLIC Threads::Threads)

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core)