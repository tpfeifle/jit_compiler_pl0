#include "../../pljit/lexer/Lexer.hpp"
#include <pljit/source/SourceReference.hpp>
#include <gtest/gtest.h>
#include <vector>
//---------------------------------------------------------------------------
namespace pljit_lexer {
//---------------------------------------------------------------------------
using TT = Token::Type;

//---------------------------------------------------------------------------
TEST(Lexer, TestParametersAll) {
    std::string codeText = "PARAM width, height, depth;\n"
                           "VAR volume;\n"
                           "CONST foo = 12;";
    // tokenize input
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);

    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }

    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Colon, TT::Identifier, TT::Colon, TT::Identifier,
                                     TT::Semicolon, TT::Var, TT::Identifier, TT::Semicolon,
                                     TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        assert(lexerTokens[i].getType() == expectedTypes[i]);
    }

    // TODO create a test to validate the source of the tokens
    /*assert(lexerTokens[8].source.lineNum == 1);
    assert(lexerTokens[8].source.charPos == 4);
    assert(lexerTokens[8].source.length == 6);*/
}

TEST(Lexer, TestParametersNoVar) {
    std::string codeText = "PARAM width, height, depth;\n"
                           "CONST volume = 1, foo = 20;";
    // tokenize input
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Colon, TT::Identifier, TT::Colon, TT::Identifier,
                                     TT::Semicolon, TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Colon,
                                     TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        assert(lexerTokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestParametersNoVarNoParam) {
    std::string codeText = "CONST volume = 1, foo = 20;";
    // tokenize input
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    std::vector<TT> expectedTypes = {TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Colon,
                                     TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        assert(lexerTokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestSimpleStatement) {
    std::string codeText = "foo := 200 * (10 + 12);";
    // tokenize input
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    std::vector<TT> expectedTypes = {TT::Identifier, TT::Assignment, TT::Literal, TT::Multiply,
                                     TT::Left_Bracket, TT::Literal, TT::Plus, TT::Literal, TT::Right_Bracket,
                                     TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        assert(lexerTokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestComplexStatement) {
    std::string codeText = "foo := a*1+1234a-12 *3b;";
    // tokenize input
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    std::vector<TT> expectedTypes = {TT::Identifier, TT::Assignment, TT::Identifier, TT::Multiply,
                                     TT::Literal, TT::Plus, TT::Literal, TT::Identifier, TT::Minus,
                                     TT::Literal, TT::Multiply, TT::Literal, TT::Identifier, TT::Semicolon};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        assert(expectedTypes.size() >= i);
        assert(lexerTokens[i].getType() == expectedTypes[i]);
    }
}

TEST(Lexer, TestFullProgram) {
    std::string codeText = "PARAM width;\n"
                           "CONST foo = 12;\n"
                           "   BEGIN\n"
                           " foo:=width;\n"
                           "RETURN foo\n"
                           "END.";
    // tokenize input
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    std::vector<TT> expectedTypes = {TT::Param, TT::Identifier, TT::Semicolon,
                                     TT::Const, TT::Identifier, TT::Equal, TT::Literal, TT::Semicolon,
                                     TT::Begin, TT::Identifier, TT::Assignment, TT::Identifier, TT::Semicolon,
                                     TT::Return, TT::Identifier, TT::End, TT::Final};
    for (unsigned long i = 0; i < lexerTokens.size(); i++) {
        assert(expectedTypes.size() > i);
        assert(lexerTokens[i].getType() == expectedTypes[i]);
    }
}


TEST(Lexer, TestExpectError) {
    std::string codeText = "PARAM? width;";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    testing::internal::CaptureStderr();
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (!token || token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    std::string output = testing::internal::GetCapturedStderr();
    std::string expectedOutput = "0:5:Unexpected character\n"
                                 "PARAM? width;\n"
                                 "     ^\n";
    assert(output == expectedOutput);

}

TEST(Lexer, TestNoWhitespaces) {
    std::string codeText = "PARAM width,  height,  temp;";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    std::vector<Token> lexerTokens;
    while (true) {
        std::unique_ptr<Token> token = lexer.next();
        if (token->getType() == Token::Type::Invalid) {
            break;
        }
        lexerTokens.emplace_back(*token);
    }
    for (Token token: lexerTokens) {
        std::string text = token.source.getText();
        assert(std::count_if(text.begin(), text.end(), isspace) == 0);
    }
}
//---------------------------------------------------------------------------
} // namespace pljit_lexer
//---------------------------------------------------------------------------
