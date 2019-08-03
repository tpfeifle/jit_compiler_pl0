#include "../../pljit/parser/Parser.hpp"
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
TEST(ParserDeathTest, TestExpectError) {
    std::vector<std::string> codeText = {"PARAM width, CONST, height, depth;\n",
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

TEST(ParserDefaultNodesTest, TestNodes) {
    std::vector<std::string> codeText = {"PARAM width, height, depth;\n",
                                         "VAR volume;\n",
                                         "CONST density = 2400;\n",
                                         "BEGIN\n",
                                         "    volume := width * height * depth;\n",
                                         "    RETURN density * volume\n",
                                         "END.\n"};
    SourceCode code = SourceCode(codeText);
    Lexer lexer = Lexer(code);
    Parser parser(lexer);
    std::shared_ptr<PTNode> pt = parser.parseFunctionDefinition();
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
//---------------------------------------------------------------------------
