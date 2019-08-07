#include "../../pljit/lexer/Lexer.hpp"
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace pljit;
// using namespace lexer;
//---------------------------------------------------------------------------
/*namespace {
//---------------------------------------------------------------------------
    struct PairHash {
        size_t operator()(const pair<uint32_t, uint32_t>& pair) const {
            return hash<uint32_t>()(pair.first) ^ hash<uint32_t>()(pair.second);
        }
    };
//---------------------------------------------------------------------------
} // namespace */
//---------------------------------------------------------------------------
TEST(Lexer, TestSimple) {
    std::vector<std::string> codeText = {"PARAM width, height, depth;",
                                         "VAR volume;"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while(lexer.next()) {}
    assert(lexer.tokens[0].type == Token::Type::PARAM);
    assert(lexer.tokens[1].category == Token::Category::IDENTIFIER);
    assert(lexer.tokens[2].type == Token::Type::COLON);
    assert(lexer.tokens[3].category == Token::Category::IDENTIFIER);
    assert(lexer.tokens[4].type == Token::Type::COLON);
    assert(lexer.tokens[5].category == Token::Category::IDENTIFIER);
    assert(lexer.tokens[6].type == Token::Type::SEMICOLON);
    assert(lexer.tokens[7].type == Token::Type::VAR);
    assert(lexer.tokens[8].category == Token::Category::IDENTIFIER);
    assert(lexer.tokens[9].type == Token::Type::SEMICOLON);

    assert(lexer.tokens[8].source.lineNum == 1);
    assert(lexer.tokens[8].source.charPos == 4);
    assert(lexer.tokens[8].source.length == 6);
}

TEST(Lexer, TestExpectError) {
    std::vector<std::string> codeText = {"PARAM? width;"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    testing::internal::CaptureStdout();
    while(lexer.next()) {}
    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = "0:5:Unexpected character\n"
                                 "PARAM? width;\n"
                                 "     ^\n";
    assert(output == expectedOutput);

}

/*TEST(Lexer, TestNoWhitespaces) {
    std::vector<std::string> codeText = {"PARAM? width;"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    testing::internal::CaptureStdout();
    while(lexer.next()) {}
    for (Token token: lexer.tokens) {
        // assert(token.source.find_first_of("abcdefghijklmnopqrstuvwxyz "))
    }
    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = "0:5:Unexpected character\n"
                                 "PARAM? width;\n"
                                 "     ^\n";
    assert(output == expectedOutput);

}*/
//---------------------------------------------------------------------------
