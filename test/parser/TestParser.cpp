#include "../../pljit/parser/Parser.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <pljit/parser/DotPTVisitor.hpp>
#include <pljit/ast/ASTNode.hpp>
//---------------------------------------------------------------------------
using namespace pljit_lexer;
using namespace pljit_parser;
using namespace pljit;
using PTT = PTNode::Type;
using namespace std;
//---------------------------------------------------------------------------

unique_ptr<NonTerminalPTNode> getParseTree(const vector<string>& codeText) {
    pljit_source::SourceCode code = pljit_source::SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    unique_ptr<NonTerminalPTNode> pt = parser.parseFunctionDefinition();
    return pt;
}

std::string getDotOutput(const std::unique_ptr<pljit_parser::NonTerminalPTNode>& pt) {
    testing::internal::CaptureStdout();
    pljit_parser::DotPTVisitor visitor = pljit_parser::DotPTVisitor();
    visitor.visit(*pt);
    return testing::internal::GetCapturedStdout();
}

/*TEST(Parser, TestInvalidDeclaration) {
    //TODO TODO
    vector<string> codeText = {"PARAM width, CONST, height, depth;\n",
                               "BEGIN\n",
                               "    RETURN 1\n",
                               "END.\n"};
    testing::internal::CaptureStderr();
    auto pt = getParseTree(codeText);
    string error = testing::internal::GetCapturedStderr();
    // TODO
    string expectedError = "0:13:Expected identifier\n"
                           "PARAM width, CONST, height, depth;\n *"
                           "             ^~~~~";
    assert(error == expectedError);
}*/


TEST(Parser, TestReturn) {
    vector<string> codeText = {"BEGIN\n",
                               "RETURN 1\n",
                               "END.\n"};
    auto pt = getParseTree(codeText);
    std::string output = getDotOutput(pt);
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
    assert(output == expectedDotGraph);
}

TEST(Parser, TestAllSimple) {
    vector<string> codeText = {"PARAM width, height, depth;\n",
                               "VAR volume;\n",
                               "CONST density = 2400;\n",
                               "BEGIN\n",
                               "volume := width * height;\n",
                               "RETURN density * volume\n",
                               "END.\n"};
    auto pt = getParseTree(codeText);
    std::string output = getDotOutput(pt);
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
    assert(output == expectedDotGraph);
}

/*TEST(Parser, TestAllSimple) {
    vector<string> codeText = {"PARAM width;\n",
                                         "VAR volume;\n",
                                         "CONST density = 2400;\n",
                                         "BEGIN\n",
                                         "    RETURN width*volume*density\n",
                                         "END.\n"};

    vector<pair<pair<unsigned, PTT>, vector<pair<unsigned, PTT>>> expectedTree{ // Format: parent, type
            {{0, PTT::FunctionDefinition}, {{1, PTT::VarDeclaration}, {2, PTT::ConstDeclaration}, {3, PTT::CompoundStatement}, {4, PTT::GenericToken}}},
            {1, {6, 7, 8}},
            {7, {9}},
            {2, {10, 11, 12}},
    };
    vector<PTT> expectedTypes = {
            PTT::FunctionDefinition, PTT::VarDeclaration, PTT::ParamDeclaration, PTT::ConstDeclaration, PTT::CompoundStatement, PTT::GenericToken,
            PTT::GenericToken, PTT::Identifier, PTT::GenericToken, // param
            PTT::GenericToken, PTT::DeclaratorList, PTT::GenericToken, // var
            PTT::Identifier,


    };
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    shared_ptr<PTNode> pt = parser.parseFunctionDefinition();
    validateNode(pt, 0, expectedTree, expectedTypes);
}*/


/*TEST(ParserDeathTest, TestExpectError) {
    vector<string> codeText = {"PARAM width, CONST, height, depth;\n",
                                         "VAR volume;\n",
                                         "CONST density = 2400;\n",
                                         "BEGIN\n",
                                         "    volume := width * height * depth;\n",
                                         "    RETURN density * volume\n",
                                         "END.\n"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    EXPECT_DEATH(parser.parseFunctionDefinition(), "0:13:Expected identifier\n"
                                                   "PARAM width, CONST, height, depth;\n *"); // TODO
    //"             ^~~~~");
}

TEST(Parser1, Test1) {
    vector<string> codeText = {"PARAM width, height, depth;\n",
                                         "VAR volume;\n",
                                         "CONST density = 2400;\n",
                                         "CONST density = 2400;\n"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    //testing::internal::CaptureStdout();
    Parser parser(lexer);
    shared_ptr<PTNode> pt = parser.parseFunctionDefinition();
    if (!pt) {
        cout << "Parser failed" << endl;
    }
    //string output = testing::internal::GetCapturedStdout();
    string expectedOutput = "No END";
    //assert(output == expectedOutput);
}*/




/*
TEST(ParserDefaultNodesTest, TestNodes) {
    vector<string> codeText = {"PARAM width, height, depth;\n",
                                         "VAR volume;\n",
                                         "CONST density = 2400;\n",
                                         "BEGIN\n",
                                         "    volume := width * height * depth;\n",
                                         "    RETURN density * volume\n",
                                         "END.\n"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    shared_ptr<PTNode> pt = parser.parseFunctionDefinition();
    assert((*pt).type == PTNode::Type::FunctionDefinition);
    assert((*pt).children[0].type == PTNode::Type::ParamDeclaration);
    assert((*pt).children[1].type == PTNode::Type::VarDeclaration);
    assert((*pt).children[2].type == PTNode::Type::ConstDeclaration);
    assert((*pt).children[3].type == PTNode::Type::CompoundStatement);
    assert((*pt).children[4].type == PTNode::Type::GenericToken);

    // first line
    assert((*pt).children[0].children[0].type == PTNode::Type::GenericToken);
    assert((*pt).children[0].children[1].type == PTNode::Type::DeclaratorList);
    assert((*pt).children[0].children[1].children[0].type == PTNode::Type::Identifier);
    assert((*pt).children[0].children[1].children[1].type == PTNode::Type::GenericToken);
    assert((*pt).children[0].children[1].children[2].type == PTNode::Type::Identifier);
    assert((*pt).children[0].children[1].children[3].type == PTNode::Type::GenericToken);
    assert((*pt).children[0].children[1].children[4].type == PTNode::Type::Identifier);
    assert((*pt).children[0].children[2].type == PTNode::Type::GenericToken);

    // second line
    assert((*pt).children[1].children[0].type == PTNode::Type::GenericToken);
    assert((*pt).children[1].children[1].type == PTNode::Type::DeclaratorList);
    assert((*pt).children[1].children[1].children[0].type == PTNode::Type::Identifier);
    assert((*pt).children[1].children[2].type == PTNode::Type::GenericToken);

    // third line
    assert((*pt).children[2].children[0].type == PTNode::Type::GenericToken);
    assert((*pt).children[2].children[1].type == PTNode::Type::InitDeclaratorList);
    assert((*pt).children[2].children[1].children[0].type == PTNode::Type::InitDeclarator);
    // TODO here should more than just an InitDeclarator
    assert((*pt).children[2].children[2].type == PTNode::Type::GenericToken);

    // compound statement
    assert((*pt).children[3].children[0].type == PTNode::Type::GenericToken); // begin
    assert((*pt).children[3].children[1].children[0].type == PTNode::Type::Statement); // statement 1
        PTNode statement1 = (*pt).children[3].children[1].children[0];
        assert(statement1.children[0].children[0].type == PTNode::Type::Identifier);
        assert(statement1.children[0].children[1].type == PTNode::Type::GenericToken);
        assert(statement1.children[0].children[2].type == PTNode::Type::AdditiveExpr);
        assert(statement1.children[0].children[2].children[0].type == PTNode::Type::MultiplicativeExpr);
        PTNode mulNode = statement1.children[0].children[2].children[0];
        assert(mulNode.children[0].type == PTNode::Type::UnaryExpr);
        assert(mulNode.children[1].type == PTNode::Type::GenericToken);
        assert(mulNode.children[2].type == PTNode::Type::MultiplicativeExpr);
        // TODO finish this, but seems to be working correctly


    assert((*pt).children[3].children[1].children[1].type == PTNode::Type::GenericToken);
    assert((*pt).children[3].children[1].children[2].type == PTNode::Type::Statement); // statement 2
    assert((*pt).children[3].children[2].type == PTNode::Type::GenericToken); // end

    // final point
    assert((*pt).children[4].type == PTNode::Type::GenericToken);
}
 */
//---------------------------------------------------------------------------
