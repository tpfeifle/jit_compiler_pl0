#include "../../pljit/parser/Parser.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/ASTNode.hpp>
//---------------------------------------------------------------------------
namespace pljit_parser {
//---------------------------------------------------------------------------
using PTT = PTNode::Type;
//---------------------------------------------------------------------------
std::string getParsingErrors(const pljit_source::SourceCode& code)
// Capture the error output of the parser and return it
{
    testing::internal::CaptureStderr();
    Parser parser(code);
    std::unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    return testing::internal::GetCapturedStderr();
}
//---------------------------------------------------------------------------
TEST(Parser, TestInvalidDeclaration) {
    std::string codeText = "PARAM width, CONST, height, depth;";
    std::string expectedError = "0:13:Expected identifier\n"
                                "PARAM width, CONST, height, depth;\n"
                                "             ^~~~~\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    EXPECT_EQ(getParsingErrors(code), expectedError);
}
//---------------------------------------------------------------------------
TEST(Parser, TestMissingFinalPoint) {
    std::string codeText = "BEGIN\n"
                           "RETURN 1\n"
                           "END";
    std::string expectedError = "2:3:Expected final token\n"
                                "END\n"
                                "   ^\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    EXPECT_EQ(getParsingErrors(code), expectedError);
}
//---------------------------------------------------------------------------
TEST(Parser, TestMissingSemicolonInDeclaration) {
    std::string codeText = "PARAM width";
    std::string expectedError = "0:11:Expected separator\n"
                                "PARAM width\n"
                                "           ^\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    EXPECT_EQ(getParsingErrors(code), expectedError);
}

TEST(Parser, TestMissingBracket) {
    std::string codeText = "BEGIN\n"
                           "RETURN 1 * (2 + 3;\n"
                           "END.";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    std::string expectedError = "1:17:Expected ')'\n"
                                "RETURN 1 * (2 + 3;\n"
                                "                 ^\n"
                                "1:11:to match this '('\n"
                                "RETURN 1 * (2 + 3;\n"
                                "           ^\n";
    EXPECT_EQ(getParsingErrors(code), expectedError);
}
TEST(Parser, TestWrongPrimaryExpression) {
    std::string codeText = "BEGIN\n"
                           "RETURN 1 * VAR\n"
                           "END.";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    std::string expectedError = "1:11:Expected an identifer, literal or '('\n"
                                "RETURN 1 * VAR\n"
                                "           ^~~\n";
    EXPECT_EQ(getParsingErrors(code), expectedError);
}
//---------------------------------------------------------------------------
TEST(Parser, TestMissingBegin) {
    std::string codeText = "RETURN 2 * (1 + 2 \n"
                           "END.";
    std::string expectedError = "0:0:Expected BEGIN keyword\n"
                                "RETURN 2 * (1 + 2 \n"
                                "^~~~~~\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    EXPECT_EQ(getParsingErrors(code), expectedError);
}
//---------------------------------------------------------------------------
TEST(Parser, TestMissingAssignment) {
    std::string codeText = "BEGIN\n"
                           "foo 12 * 3;\n"
                           "RETURN 1\n"
                           "END";
    std::string expectedError = "1:4:Expected assignment\n"
                                "foo 12 * 3;\n"
                                "    ^~\n";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    EXPECT_EQ(getParsingErrors(code), expectedError);
}
//---------------------------------------------------------------------------
TEST(ParserDefaultNodesTest, TestDetailed) {
    std::string codeText = "BEGIN\n"
                           "RETURN 12\n"
                           "END.";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    pljit_lexer::Lexer lexer = pljit_lexer::Lexer(code);
    Parser parser(code);
    std::unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    assert((*pt).getType() == PTNode::Type::FunctionDefinition);
    EXPECT_EQ(pt->children[0]->getType(), PTNode::Type::CompoundStatement);
    EXPECT_EQ(pt->children[1]->getType(), PTNode::Type::GenericToken);

    // compound statement
    auto* compoundStatement = static_cast<NonTerminalPTNode*>(pt->children[0].get());
    EXPECT_EQ(compoundStatement->children[0]->getType(), PTNode::Type::GenericToken); // begin
    EXPECT_EQ(compoundStatement->children[1]->getType(), PTNode::Type::StatementList); // statement list
    EXPECT_EQ(compoundStatement->children[2]->getType(), PTNode::Type::GenericToken); // end
    auto* statementList = static_cast<NonTerminalPTNode*>(compoundStatement->children[1].get());
    EXPECT_EQ(statementList->children[0]->getType(), PTNode::Type::Statement); // statement
    auto* returnStatement = static_cast<NonTerminalPTNode*>(statementList->children[0].get());
    EXPECT_EQ(returnStatement->children[0]->getType(), PTNode::Type::GenericToken); // return keyword
    EXPECT_EQ(returnStatement->children[1]->getType(), PTNode::Type::AdditiveExpr);
    auto* additiveExpr = static_cast<NonTerminalPTNode*>(returnStatement->children[1].get());
    EXPECT_EQ(additiveExpr->children[0]->getType(), PTNode::Type::MultiplicativeExpr);
    auto* multiplicativeExpr = static_cast<NonTerminalPTNode*>(additiveExpr->children[0].get());
    EXPECT_EQ(multiplicativeExpr->children[0]->getType(), PTNode::Type::UnaryExpr);
    auto* unaryExpr = static_cast<NonTerminalPTNode*>(multiplicativeExpr->children[0].get());
    EXPECT_EQ(unaryExpr->children[0]->getType(), PTNode::Type::PrimaryExpr);
    auto* primaryExpr = static_cast<NonTerminalPTNode*>(unaryExpr->children[0].get());
    EXPECT_EQ(primaryExpr->children[0]->getType(), PTNode::Type::Literal);
}
//---------------------------------------------------------------------------
TEST(Parser, TestReturnDotFormat) {
    std::string codeText = "BEGIN\n"
                           "RETURN 1\n"
                           "END.";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Parser parser(code);
    std::unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    testing::internal::CaptureStdout();
    pljit_parser::DotPTVisitor visitor = pljit_parser::DotPTVisitor();
    visitor.visit(*pt);
    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedDotGraph;
    expectedDotGraph = "FunctionDefinition0[label=\"FunctionDefinition\"];\n"
                       "FunctionDefinition0 -> CompoundStatement1\n"
                       "CompoundStatement1[label=\"CompoundStatement\"];\n"
                       "CompoundStatement1 -> GenericToken2\n"
                       "GenericToken2[label=\"BEGIN\"];\n"
                       "CompoundStatement1 -> StatementList4\n"
                       "StatementList4[label=\"StatementList\"];\n"
                       "StatementList4 -> Statement5\n"
                       "Statement5[label=\"Statement\"];\n"
                       "Statement5 -> GenericToken6\n"
                       "GenericToken6[label=\"RETURN\"];\n"
                       "Statement5 -> AdditiveExpr8\n"
                       "AdditiveExpr8[label=\"AdditiveExpr\"];\n"
                       "AdditiveExpr8 -> MultiplicativeExpr9\n"
                       "MultiplicativeExpr9[label=\"MultiplicativeExpr\"];\n"
                       "MultiplicativeExpr9 -> UnaryExpr10\n"
                       "UnaryExpr10[label=\"UnaryExpr\"];\n"
                       "UnaryExpr10 -> PrimaryExpr11\n"
                       "PrimaryExpr11[label=\"PrimaryExpr\"];\n"
                       "PrimaryExpr11 -> Literal12\n"
                       "Literal12[label=\"1\"];\n"
                       "CompoundStatement1 -> GenericToken14\n"
                       "GenericToken14[label=\"END\"];\n"
                       "FunctionDefinition0 -> GenericToken16\n"
                       "GenericToken16[label=\".\"];\n";
    EXPECT_EQ(output, expectedDotGraph);
}
//---------------------------------------------------------------------------
TEST(Parser, TestAllDotFormat) {
    std::string codeText = "PARAM width, height, depth;\n"
                           "VAR volume;\n"
                           "CONST density = 2400;\n"
                           "BEGIN\n"
                           "volume := width * height;\n"
                           "RETURN density * volume\n"
                           "END.";
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    pljit_lexer::Lexer lexer = pljit_lexer::Lexer(code);
    Parser parser(code);
    std::unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    testing::internal::CaptureStdout();
    pljit_parser::DotPTVisitor visitor = pljit_parser::DotPTVisitor();
    visitor.visit(*pt);
    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedDotGraph;
    expectedDotGraph = "FunctionDefinition0[label=\"FunctionDefinition\"];\n"
                       "FunctionDefinition0 -> ParamDeclaration1\n"
                       "ParamDeclaration1[label=\"ParamDeclaration\"];\n"
                       "ParamDeclaration1 -> GenericToken2\n"
                       "GenericToken2[label=\"PARAM\"];\n"
                       "ParamDeclaration1 -> DeclaratorList4\n"
                       "DeclaratorList4[label=\"DeclaratorList\"];\n"
                       "DeclaratorList4 -> Identifier5\n"
                       "Identifier5[label=\"width\"];\n"
                       "DeclaratorList4 -> GenericToken7\n"
                       "GenericToken7[label=\",\"];\n"
                       "DeclaratorList4 -> Identifier9\n"
                       "Identifier9[label=\"height\"];\n"
                       "DeclaratorList4 -> GenericToken11\n"
                       "GenericToken11[label=\",\"];\n"
                       "DeclaratorList4 -> Identifier13\n"
                       "Identifier13[label=\"depth\"];\n"
                       "ParamDeclaration1 -> GenericToken15\n"
                       "GenericToken15[label=\";\"];\n"
                       "FunctionDefinition0 -> VarDeclaration17\n"
                       "VarDeclaration17[label=\"VarDeclaration\"];\n"
                       "VarDeclaration17 -> GenericToken18\n"
                       "GenericToken18[label=\"VAR\"];\n"
                       "VarDeclaration17 -> DeclaratorList20\n"
                       "DeclaratorList20[label=\"DeclaratorList\"];\n"
                       "DeclaratorList20 -> Identifier21\n"
                       "Identifier21[label=\"volume\"];\n"
                       "VarDeclaration17 -> GenericToken23\n"
                       "GenericToken23[label=\";\"];\n"
                       "FunctionDefinition0 -> ConstDeclaration25\n"
                       "ConstDeclaration25[label=\"ConstDeclaration\"];\n"
                       "ConstDeclaration25 -> GenericToken26\n"
                       "GenericToken26[label=\"CONST\"];\n"
                       "ConstDeclaration25 -> InitDeclaratorList28\n"
                       "InitDeclaratorList28[label=\"InitDeclaratorList\"];\n"
                       "InitDeclaratorList28 -> InitDeclarator29\n"
                       "InitDeclarator29[label=\"InitDeclarator\"];\n"
                       "InitDeclarator29 -> Identifier30\n"
                       "Identifier30[label=\"density\"];\n"
                       "InitDeclarator29 -> GenericToken32\n"
                       "GenericToken32[label=\"=\"];\n"
                       "InitDeclarator29 -> Literal34\n"
                       "Literal34[label=\"2400\"];\n"
                       "ConstDeclaration25 -> GenericToken36\n"
                       "GenericToken36[label=\";\"];\n"
                       "FunctionDefinition0 -> CompoundStatement38\n"
                       "CompoundStatement38[label=\"CompoundStatement\"];\n"
                       "CompoundStatement38 -> GenericToken39\n"
                       "GenericToken39[label=\"BEGIN\"];\n"
                       "CompoundStatement38 -> StatementList41\n"
                       "StatementList41[label=\"StatementList\"];\n"
                       "StatementList41 -> Statement42\n"
                       "Statement42[label=\"Statement\"];\n"
                       "Statement42 -> AssignmentExpr43\n"
                       "AssignmentExpr43[label=\"AssignmentExpr\"];\n"
                       "AssignmentExpr43 -> Identifier44\n"
                       "Identifier44[label=\"volume\"];\n"
                       "AssignmentExpr43 -> GenericToken46\n"
                       "GenericToken46[label=\":=\"];\n"
                       "AssignmentExpr43 -> AdditiveExpr48\n"
                       "AdditiveExpr48[label=\"AdditiveExpr\"];\n"
                       "AdditiveExpr48 -> MultiplicativeExpr49\n"
                       "MultiplicativeExpr49[label=\"MultiplicativeExpr\"];\n"
                       "MultiplicativeExpr49 -> UnaryExpr50\n"
                       "UnaryExpr50[label=\"UnaryExpr\"];\n"
                       "UnaryExpr50 -> PrimaryExpr51\n"
                       "PrimaryExpr51[label=\"PrimaryExpr\"];\n"
                       "PrimaryExpr51 -> Identifier52\n"
                       "Identifier52[label=\"width\"];\n"
                       "MultiplicativeExpr49 -> GenericToken54\n"
                       "GenericToken54[label=\"*\"];\n"
                       "MultiplicativeExpr49 -> MultiplicativeExpr56\n"
                       "MultiplicativeExpr56[label=\"MultiplicativeExpr\"];\n"
                       "MultiplicativeExpr56 -> UnaryExpr57\n"
                       "UnaryExpr57[label=\"UnaryExpr\"];\n"
                       "UnaryExpr57 -> PrimaryExpr58\n"
                       "PrimaryExpr58[label=\"PrimaryExpr\"];\n"
                       "PrimaryExpr58 -> Identifier59\n"
                       "Identifier59[label=\"height\"];\n"
                       "StatementList41 -> GenericToken61\n"
                       "GenericToken61[label=\";\"];\n"
                       "StatementList41 -> Statement63\n"
                       "Statement63[label=\"Statement\"];\n"
                       "Statement63 -> GenericToken64\n"
                       "GenericToken64[label=\"RETURN\"];\n"
                       "Statement63 -> AdditiveExpr66\n"
                       "AdditiveExpr66[label=\"AdditiveExpr\"];\n"
                       "AdditiveExpr66 -> MultiplicativeExpr67\n"
                       "MultiplicativeExpr67[label=\"MultiplicativeExpr\"];\n"
                       "MultiplicativeExpr67 -> UnaryExpr68\n"
                       "UnaryExpr68[label=\"UnaryExpr\"];\n"
                       "UnaryExpr68 -> PrimaryExpr69\n"
                       "PrimaryExpr69[label=\"PrimaryExpr\"];\n"
                       "PrimaryExpr69 -> Identifier70\n"
                       "Identifier70[label=\"density\"];\n"
                       "MultiplicativeExpr67 -> GenericToken72\n"
                       "GenericToken72[label=\"*\"];\n"
                       "MultiplicativeExpr67 -> MultiplicativeExpr74\n"
                       "MultiplicativeExpr74[label=\"MultiplicativeExpr\"];\n"
                       "MultiplicativeExpr74 -> UnaryExpr75\n"
                       "UnaryExpr75[label=\"UnaryExpr\"];\n"
                       "UnaryExpr75 -> PrimaryExpr76\n"
                       "PrimaryExpr76[label=\"PrimaryExpr\"];\n"
                       "PrimaryExpr76 -> Identifier77\n"
                       "Identifier77[label=\"volume\"];\n"
                       "CompoundStatement38 -> GenericToken79\n"
                       "GenericToken79[label=\"END\"];\n"
                       "FunctionDefinition0 -> GenericToken81\n"
                       "GenericToken81[label=\".\"];\n";
    EXPECT_EQ(output, expectedDotGraph);
}
//---------------------------------------------------------------------------
} // namespace pljit_parser
//---------------------------------------------------------------------------