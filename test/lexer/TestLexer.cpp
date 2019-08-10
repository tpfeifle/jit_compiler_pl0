#include "../../pljit/lexer/Lexer.hpp"
#include <pljit/source/SourceReference.hpp>
#include <gtest/gtest.h>
#include <vector>
//---------------------------------------------------------------------------
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
namespace pljit::lexer {
using TT = Token::Type;

//---------------------------------------------------------------------------
TEST(Lexer, TestParametersAll) {
    std::vector<std::string> codeText = {"PARAM width, height, depth;",
                                         "VAR volume;",
                                         "CONST foo = 12;"};
    // tokenize input
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while (lexer.next()) {}

    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Colon, TT::Identifier, TT::Colon, TT::Identifier,
                                     TT::Semicolon, TT::Var, TT::Identifier, TT::Semicolon,
                                     TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexer.tokens.size(); i++) {
        assert(lexer.tokens[i].getType() == expectedTypes[i]);
    }

    // TODO create a test to validate the source of the tokens
    /*assert(lexer.tokens[8].source.lineNum == 1);
    assert(lexer.tokens[8].source.charPos == 4);
    assert(lexer.tokens[8].source.length == 6);*/
}

TEST(Lexer, TestParametersNoVar) {
    std::vector<std::string> codeText = {"PARAM width, height, depth;",
                                         "CONST volume = 1, foo = 20;"};
    // tokenize input
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while (lexer.next()) {}
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Colon, TT::Identifier, TT::Colon, TT::Identifier,
                                     TT::Semicolon, TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Colon,
                                     TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexer.tokens.size(); i++) {
        assert(lexer.tokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestParametersNoVarNoParam) {
    std::vector<std::string> codeText = {"CONST volume = 1, foo = 20;"};
    // tokenize input
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while (lexer.next()) {}
    std::vector<TT> expectedTypes = {TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Colon,
                                     TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexer.tokens.size(); i++) {
        assert(lexer.tokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestSimpleStatement) {
    std::vector<std::string> codeText = {"foo := 200 * (10 + 12);"};
    // tokenize input
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while (lexer.next()) {}
    std::vector<TT> expectedTypes = {TT::Identifier, TT::Assignment, TT::Literal, TT::Multiply,
                                     TT::Left_Bracket, TT::Literal, TT::Plus, TT::Literal, TT::Right_Bracket,
                                     TT::Semicolon};
    for (unsigned long i = 0; i < lexer.tokens.size(); i++) {
        assert(lexer.tokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestComplexStatement) {
    std::vector<std::string> codeText = {"foo := a*1+1234a-12 *3b;"};
    // tokenize input
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while (lexer.next()) {}
    std::vector<TT> expectedTypes = {TT::Identifier, TT::Assignment, TT::Identifier, TT::Multiply,
                                     TT::Literal, TT::Plus, TT::Literal, TT::Identifier, TT::Minus,
                                     TT::Literal, TT::Multiply, TT::Literal, TT::Identifier, TT::Semicolon};
    for (unsigned long i = 0; i < lexer.tokens.size(); i++) {
        assert(expectedTypes.size() >= i);
        assert(lexer.tokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestFullProgram) {
    std::vector<std::string> codeText = {"PARAM width;",
                                         "CONST foo = 12;",
                                         "   BEGIN",
                                         " foo:=width;",
                                         "RETURN foo",
                                         "END."};
    // tokenize input
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while (lexer.next()) {}
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Semicolon,
                                     TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon,
                                     TT::Begin, TT::Identifier, TT::Assignment, TT::Identifier, TT::Semicolon,
                                     TT::Return, TT::Identifier, TT::End, TT::Final};
    for (unsigned long i = 0; i < lexer.tokens.size(); i++) {
        assert(expectedTypes.size() > i);
        assert(lexer.tokens[i].getType() == expectedTypes[i]);
    }
}


TEST(Lexer, TestExpectError) {
    std::vector<std::string> codeText = {"PARAM? width;"};
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    testing::internal::CaptureStderr();
    while (lexer.next()) {}
    std::string output = testing::internal::GetCapturedStderr();
    std::string expectedOutput = "0:5:Unexpected character\n"
                                 "PARAM? width;"
                                 "     ^\n";
    assert(output == expectedOutput);

}

TEST(Lexer, TestNoWhitespaces) {
    std::vector<std::string> codeText = {"PARAM width,  height,  temp;"};
    source::SourceCode code = source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    while(lexer.next()) {}
    for (Token token: lexer.tokens) {
        std::string text = token.source.getText();
        assert(std::count_if(text.begin(), text.end(), isspace) == 0);
    }
}
//---------------------------------------------------------------------------
} // namespace pljit::lexer
//---------------------------------------------------------------------------
