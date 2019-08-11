#include "../../pljit/lexer/Lexer.hpp"
#include <pljit/source/SourceReference.hpp>
#include <gtest/gtest.h>
#include <vector>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
using TT = Token::Type;

//---------------------------------------------------------------------------
std::vector<Token> getLexerTokens(const std::string& codeText)
// Tokenize the provided source code text
{
    // store code text
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);

    // tokenize input
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (!token || token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    return lexerTokens;
}
//---------------------------------------------------------------------------
TEST(Lexer, TestParametersAll) {
    std::string codeText = "PARAM width, height, depth;\n"
                           "VAR volume;\n"
                           "CONST foo = 12;";

    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Colon, TT::Identifier, TT::Colon, TT::Identifier,
                                     TT::Semicolon, TT::Var, TT::Identifier, TT::Semicolon,
                                     TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedTypes.size() > i);
        EXPECT_EQ(lexerTokens[i].getType(), expectedTypes[i]);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestParametersNoVar) {
    std::string codeText = "PARAM width, height, depth;\n"
                           "CONST volume = 1, foo = 20;";
    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Colon, TT::Identifier, TT::Colon, TT::Identifier,
                                     TT::Semicolon, TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Colon,
                                     TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedTypes.size() > i);
        EXPECT_EQ(lexerTokens[i].getType(), expectedTypes[i]);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestParametersNoVarNoParam) {
    std::string codeText = "CONST volume = 1, foo = 20;";
    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::vector<TT> expectedTypes = {TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Colon,
                                     TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedTypes.size() > i);
        EXPECT_EQ(lexerTokens[i].getType(), expectedTypes[i]);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestSimpleStatement) {
    std::string codeText = "foo := 200 * (10 + 12);";
    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::vector<TT> expectedTypes = {TT::Identifier, TT::Assignment, TT::Literal, TT::Multiply,
                                     TT::Left_Bracket, TT::Literal, TT::Plus, TT::Literal, TT::Right_Bracket,
                                     TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedTypes.size() > i);
        EXPECT_EQ(lexerTokens[i].getType(), expectedTypes[i]);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestComplexStatement) {
    std::string codeText = "foo := a*1+1234a-12 *3b;";
    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::vector<TT> expectedTypes = {TT::Identifier, TT::Assignment, TT::Identifier, TT::Multiply,
                                     TT::Literal, TT::Plus, TT::Literal, TT::Identifier, TT::Minus,
                                     TT::Literal, TT::Multiply, TT::Literal, TT::Identifier, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedTypes.size() > i);
        EXPECT_EQ(lexerTokens[i].getType(), expectedTypes[i]);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestFullProgram) {
    std::string codeText = "PARAM width;\n"
                           "CONST foo = 12;\n"
                           "   BEGIN\n"
                           " foo:=width;\n"
                           "RETURN foo\n"
                           "END.";
    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Semicolon,
                                     TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon,
                                     TT::Begin, TT::Identifier, TT::Assignment, TT::Identifier, TT::Semicolon,
                                     TT::Return, TT::Identifier, TT::End, TT::Final};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedTypes.size() > i);
        EXPECT_EQ(lexerTokens[i].getType(), expectedTypes[i]);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestExpectError) {
    std::string codeText = "PARAM? width;";
    testing::internal::CaptureStderr();
    std::vector<Token> lexerTokens = getLexerTokens(codeText);
    std::string output = testing::internal::GetCapturedStderr();
    std::string expectedOutput = "0:5:Unexpected character\n"
                                 "PARAM? width;\n"
                                 "     ^\n";
    EXPECT_EQ(output, expectedOutput);

}
//---------------------------------------------------------------------------
TEST(Lexer, TestNoWhitespaces) {
    std::string codeText = "PARAM width,  height,  temp;";
    // store code text
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);

    // tokenize input
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (!token || token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    for (const Token& token: lexerTokens) {
        std::string_view text = token.source.getText();
        EXPECT_EQ(std::count_if(text.begin(), text.end(), isspace), 0);
    }
}
//---------------------------------------------------------------------------
TEST(Lexer, TestCorrectSources) {
    std::string codeText = "PARAM width;\n"
                           "VAR temp;";
    // store code text
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);

    // tokenize input
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (!token || token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    // tuples in the format line, pos, length
    std::vector<pljit_source::SourceReference> expectedSources{pljit_source::SourceReference(0, 0, 5, code),
                                                               pljit_source::SourceReference(0, 6, 5, code),
                                                               pljit_source::SourceReference(0, 11, 1, code),
                                                               pljit_source::SourceReference(1, 0, 3, code),
                                                               pljit_source::SourceReference(1, 4, 4, code),
                                                               pljit_source::SourceReference(1, 8, 1, code)};
    // validate the source-reference locations
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        ASSERT_TRUE(expectedSources.size() > i);
        EXPECT_EQ(lexerTokens[i].source.getCharPos(), expectedSources[i].getCharPos());
        EXPECT_EQ(lexerTokens[i].source.getLineNum(), expectedSources[i].getLineNum());
        EXPECT_EQ(lexerTokens[i].source.getLength(), expectedSources[i].getLength());
    }
}
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------
