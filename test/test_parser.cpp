#include <cstdint>
#include <vector>
#include <any>
#include <string>

#include "gtest/gtest.h"
#include "voidjs/lexer/token.h"
#include "voidjs/ir/ast.h"
#include "voidjs/ir/expression.h"
#include "voidjs/ir/statement.h"
#include "voidjs/ir/literal.h"
#include "voidjs/lexer/token_type.h"
#include "voidjs/parser/parser.h"

using namespace voidjs;

TEST(parser, ParsePrimaryExpression) {

  // Null
  {
    std::u16string source = u"null";

    Parser parser(source);

    auto expr = parser.ParsePrimaryExpression();
    EXPECT_EQ(ast::AstNodeType::NULL_LITERAL, expr->GetType());
  }

  // Boolean
  {
    std::u16string source = u"true false";

    Parser parser(source);

    std::vector<std::int32_t> expects = {
      true,
      false,
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::BOOLEAN_LITERAL, expr->GetType());
      EXPECT_EQ(expect, expr->AsBooleanLiteral()->GetBoolean());
    }
  }

  // Identifier
  {
    std::u16string source = u"True False DDOSvoid Void";

    Parser parser(source);

    std::vector<std::u16string> expects = {
      u"True",
      u"False",
      u"DDOSvoid",
      u"Void",
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::IDENTIFIER, expr->GetType());
      EXPECT_EQ(expect, expr->AsIdentifier()->GetName());
    }
  }

  // Number
  {
    std::u16string source = uR"(
0 101 0.01 12.05 .8 0xAbC09
)";

    Parser parser(source);

    std::vector<double> expects = {
      0,
      101,
      0.01,
      12.05,
      0.8,
      703497,
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::NUMERIC_LITERAL, expr->GetType());
      EXPECT_DOUBLE_EQ(expect, expr->AsNumericLiteral()->GetNumber<double>());
      EXPECT_EQ(static_cast<std::int32_t>(expect), expr->AsNumericLiteral()->GetNumber<std::int32_t>());
    }
  }

  // String
  {
     std::u16string source = uR"(
"hello, world"
'😊'
'\u1234'
)";

    Parser parser(source);

    std::vector<std::u16string> expects = {
      uR"(hello, world)",
      uR"(😊)",
      std::u16string(1, 4660),
    };

    for (auto& expect : expects) {
      auto expr = parser.ParsePrimaryExpression();
      EXPECT_EQ(ast::AstNodeType::STRING_LITERAL, expr->GetType());
      EXPECT_EQ(expect, expr->AsStringLiteral()->GetString());
    }
  }

  // ArrayLiteral
  {
    Parser parser(u"[]");

    auto expr = parser.ParsePrimaryExpression();
    ASSERT_TRUE(expr->IsArrayLiteral());

    auto arr = expr->AsArrayLiteral();
    const auto& elems = arr->GetElements();
    EXPECT_EQ(0, elems.size());
  }

  // ArrayLiteral
  {
    Parser parser(u"[1, 2]");

    auto expr = parser.ParsePrimaryExpression();
    ASSERT_TRUE(expr->IsArrayLiteral());

    auto arr = expr->AsArrayLiteral();
    const auto& elems = arr->GetElements();
    ASSERT_TRUE(elems[0]->IsNumericLiteral());
    ASSERT_TRUE(elems[1]->IsNumericLiteral());
    EXPECT_EQ(1, elems[0]->AsNumericLiteral()->GetNumber<std::int32_t>());
    EXPECT_EQ(2, elems[1]->AsNumericLiteral()->GetNumber<std::int32_t>());
  }

  {
    Parser parser(u"['Hello', , 'World',]");

    auto expr = parser.ParsePrimaryExpression();
    ASSERT_TRUE(expr->IsArrayLiteral());

    auto arr = expr->AsArrayLiteral();
    const auto& elems = arr->GetElements();
    ASSERT_TRUE(elems[0]->IsStringLiteral());
    ASSERT_TRUE(elems[1] == nullptr);
    ASSERT_TRUE(elems[2]->IsStringLiteral());
    EXPECT_EQ(u"Hello", elems[0]->AsStringLiteral()->GetString());
    EXPECT_EQ(u"World", elems[2]->AsStringLiteral()->GetString());
  }

}

TEST(parser, ParseLeftHandSideExpression) {
  // MemberExpression . IdentifierName
  {
    Parser parser(u"Array.length");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsMemberExpression());
    
    auto mem_expr = expr->AsMemberExpression();
    ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"Array", mem_expr->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"length", mem_expr->GetProperty()->AsIdentifier()->GetName());
  }

  // MemberExpression . IdentifierName . IdentifierName
  {
    Parser parser(u"DDOSvoid.ZigZagZing.vid");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsMemberExpression());
    
    auto mem_expr1 = expr->AsMemberExpression();
    ASSERT_TRUE(mem_expr1->GetObject()->IsMemberExpression());
    ASSERT_TRUE(mem_expr1->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"vid", mem_expr1->GetProperty()->AsIdentifier()->GetName());
    
    auto mem_expr2 = mem_expr1->GetObject()->AsMemberExpression();
    ASSERT_TRUE(mem_expr2->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr2->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"DDOSvoid", mem_expr2->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"ZigZagZing", mem_expr2->GetProperty()->AsIdentifier()->GetName());
  }

  // New MemberExpression
  {
    Parser parser(u"new A");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsNewExpression());

    auto new_expr = expr->AsNewExpression();
    ASSERT_TRUE(new_expr->GetConstructor()->IsIdentifier());
    EXPECT_EQ(u"A", new_expr->GetConstructor()->AsIdentifier()->GetName());
  }

  // New MemberExpression Arguments
  {
    Parser parser(u"new A()");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsNewExpression());

    auto new_expr = expr->AsNewExpression();
    ASSERT_TRUE(new_expr->GetConstructor()->IsIdentifier());
    EXPECT_EQ(u"A", new_expr->GetConstructor()->AsIdentifier()->GetName());
    EXPECT_EQ(0, new_expr->GetArguments().size());
  }

  // New New MemberExpression . IdentifierName Arguments Arguments
  {
    Parser parser(u"new new DDOSvoid.ddos()()");

    auto expr = parser.ParseLeftHandSideExpression();
    ASSERT_TRUE(expr->IsNewExpression());

    auto new_expr1 = expr->AsNewExpression();
    ASSERT_TRUE(new_expr1->GetConstructor()->IsNewExpression());
    EXPECT_EQ(0, new_expr1->GetArguments().size());

    auto new_expr2 = new_expr1->GetConstructor()->AsNewExpression();
    ASSERT_TRUE(new_expr2->GetConstructor()->IsMemberExpression());
    EXPECT_EQ(0, new_expr1->GetArguments().size());

    auto mem_expr = new_expr2->GetConstructor()->AsMemberExpression();
    ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"DDOSvoid", mem_expr->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"ddos", mem_expr->GetProperty()->AsIdentifier()->GetName());
  }
}

TEST(parser, ParsePostfixExpression) {
  // ++
  {
    Parser parser(u"Array.length++");

    auto expr = parser.ParsePostfixExpression();
    ASSERT_TRUE(expr->IsPostfixExpression());

    auto post_expr = expr->AsPostfixExpression();
    EXPECT_EQ(TokenType::INC, post_expr->GetOperator());
    ASSERT_TRUE(post_expr->GetExpression()->IsMemberExpression());
    
    auto mem_expr = post_expr->GetExpression()->AsMemberExpression();
    ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
    ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
    EXPECT_EQ(u"Array", mem_expr->GetObject()->AsIdentifier()->GetName());
    EXPECT_EQ(u"length", mem_expr->GetProperty()->AsIdentifier()->GetName());
  }

  // --
  {
    Parser parser(u"i--");

    auto expr = parser.ParsePostfixExpression();
    ASSERT_TRUE(expr->IsPostfixExpression());

    auto post_expr = expr->AsPostfixExpression();
    ASSERT_TRUE(post_expr->GetExpression()->IsIdentifier());
    EXPECT_EQ(TokenType::DEC, post_expr->GetOperator());
    EXPECT_EQ(u"i", post_expr->GetExpression()->AsIdentifier()->GetName());
  }
}

TEST(parser, ParseUnaryExpression) {
  // delete UnaryExpression
  {
    Parser parser(u"delete Array");

    auto expr = parser.ParseUnaryExpression();
    ASSERT_TRUE(expr->IsUnaryExpression());

    auto unary_expr = expr->AsUnaryExpression();
    ASSERT_TRUE(unary_expr->GetExpression()->IsIdentifier());
    EXPECT_EQ(TokenType::KEYWORD_DELETE, unary_expr->GetOperator());
    EXPECT_EQ(u"Array", unary_expr->GetExpression()->AsIdentifier()->GetName());
  }

  // ! ! UnaryExpression
  {
    Parser parser(u"!!true");

    auto expr = parser.ParseUnaryExpression();
    ASSERT_TRUE(expr->IsUnaryExpression());

    auto unary_expr1 = expr->AsUnaryExpression();
    ASSERT_TRUE(unary_expr1->GetExpression()->IsUnaryExpression());
    EXPECT_EQ(TokenType::LOGICAL_NOT, unary_expr1->GetOperator());

    auto unary_expr2 = unary_expr1->GetExpression()->AsUnaryExpression();
    ASSERT_TRUE(unary_expr2->GetExpression()->IsBooleanLiteral());
    EXPECT_EQ(TokenType::LOGICAL_NOT, unary_expr2->GetOperator());
    EXPECT_EQ(true, unary_expr2->GetExpression()->AsBooleanLiteral()->GetBoolean());
  }
}

TEST(parser, ParseBinaryExpression) {
  {
    Parser parser(u"1 + 2 * 3");

    auto expr = parser.ParseBinaryExpression();
    ASSERT_TRUE(expr->IsBinaryExpression());

    auto binary_expr1 = expr->AsBinaryExpression();
    ASSERT_TRUE(binary_expr1->GetLeft()->IsNumericLiteral());
    ASSERT_TRUE(binary_expr1->GetRight()->IsBinaryExpression());
    EXPECT_EQ(TokenType::ADD, binary_expr1->GetOperator());
    EXPECT_EQ(1, binary_expr1->GetLeft()->AsNumericLiteral()->GetNumber<std::int32_t>());

    auto binary_expr2 = binary_expr1->GetRight()->AsBinaryExpression();
    ASSERT_TRUE(binary_expr2->GetLeft()->IsNumericLiteral());
    ASSERT_TRUE(binary_expr2->GetRight()->IsNumericLiteral());
    EXPECT_EQ(TokenType::MUL, binary_expr2->GetOperator());
    EXPECT_EQ(2, binary_expr2->GetLeft()->AsNumericLiteral()->GetNumber<std::int32_t>());
    EXPECT_EQ(3, binary_expr2->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
  }

  {
    Parser parser(u"1 + 2 % 3 <= ++4");

    auto expr = parser.ParseBinaryExpression();
    ASSERT_TRUE(expr->IsBinaryExpression());

    auto binary_expr1 = expr->AsBinaryExpression();
    ASSERT_TRUE(binary_expr1->GetLeft()->IsBinaryExpression());
    ASSERT_TRUE(binary_expr1->GetRight()->IsUnaryExpression());
    EXPECT_EQ(TokenType::LESS_EQUAL, binary_expr1->GetOperator());

    auto binary_expr2 = binary_expr1->GetLeft()->AsBinaryExpression();
    ASSERT_TRUE(binary_expr2->GetLeft()->IsNumericLiteral());
    ASSERT_TRUE(binary_expr2->GetRight()->IsBinaryExpression());
    EXPECT_EQ(TokenType::ADD, binary_expr2->GetOperator());
    EXPECT_EQ(1, binary_expr2->GetLeft()->AsNumericLiteral()->GetNumber<std::int32_t>());

    auto unary_expr = binary_expr1->GetRight()->AsUnaryExpression();
    ASSERT_TRUE(unary_expr->GetExpression()->IsNumericLiteral());
    EXPECT_EQ(TokenType::INC, unary_expr->GetOperator());
    EXPECT_EQ(4, unary_expr->GetExpression()->AsNumericLiteral()->GetNumber<std::int32_t>());

    auto binary_expr3 = binary_expr2->GetRight()->AsBinaryExpression();
    ASSERT_TRUE(binary_expr3->GetLeft()->IsNumericLiteral());
    ASSERT_TRUE(binary_expr3->GetRight()->IsNumericLiteral());
    EXPECT_EQ(TokenType::MOD, binary_expr3->GetOperator());
    EXPECT_EQ(2, binary_expr3->GetLeft()->AsNumericLiteral()->GetNumber<std::int32_t>());
    EXPECT_EQ(3, binary_expr3->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
  }
}

TEST(parser, ParseConditionalExpression) {
  Parser parser(u"true ? 1 : 2");

  auto expr = parser.ParseConditionalExpression();
  ASSERT_TRUE(expr->IsConditionalExpression());

  auto cond_expr = expr->AsConditionalExpression();
  ASSERT_TRUE(cond_expr->GetConditional()->IsBooleanLiteral());
  ASSERT_TRUE(cond_expr->GetConsequent()->IsNumericLiteral());
  ASSERT_TRUE(cond_expr->GetAlternate()->IsNumericLiteral());
  EXPECT_EQ(true, cond_expr->GetConditional()->AsBooleanLiteral()->GetBoolean());
  EXPECT_EQ(1, cond_expr->GetConsequent()->AsNumericLiteral()->GetNumber<std::int32_t>());
  EXPECT_EQ(2, cond_expr->GetAlternate()->AsNumericLiteral()->GetNumber<std::int32_t>());
}


TEST(parser, ParseAssignmentExpression) {
  Parser parser(u"DDOSvoid.ddos = 1");

  auto expr = parser.ParseAssignmentExpression();
  ASSERT_TRUE(expr->IsAssignmentExpression());

  auto assign_expr = expr->AsAssignmentExpression();
  ASSERT_TRUE(assign_expr->GetLeft()->IsMemberExpression());
  ASSERT_TRUE(assign_expr->GetRight()->IsNumericLiteral());
  EXPECT_EQ(TokenType::ASSIGN, assign_expr->GetOperator());
  EXPECT_EQ(1, assign_expr->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());

  auto mem_expr = assign_expr->GetLeft()->AsMemberExpression();
  ASSERT_TRUE(mem_expr->GetObject()->IsIdentifier());
  ASSERT_TRUE(mem_expr->GetProperty()->IsIdentifier());
  EXPECT_EQ(u"DDOSvoid", mem_expr->GetObject()->AsIdentifier()->GetName());
  EXPECT_EQ(u"ddos", mem_expr->GetProperty()->AsIdentifier()->GetName());
}

TEST(parser, ParseExpression) {
  Parser parser(u"x = 0, y = x + 1");

  auto expr = parser.ParseExpression();
  ASSERT_TRUE(expr->IsSequenceExpression());

  const auto& exprs = expr->AsSequenceExpression()->GetExpressions();

  auto expr1 = exprs[0];
  ASSERT_TRUE(expr1->IsAssignmentExpression());

  auto assign_expr1 = expr1->AsAssignmentExpression();
  ASSERT_TRUE(assign_expr1->GetLeft()->IsIdentifier());
  ASSERT_TRUE(assign_expr1->GetRight()->IsNumericLiteral());
  EXPECT_EQ(TokenType::ASSIGN, assign_expr1->GetOperator());
  EXPECT_EQ(u"x", assign_expr1->GetLeft()->AsIdentifier()->GetName());
  EXPECT_EQ(0, assign_expr1->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());

  auto expr2 = exprs[1];

  auto assign_expr2 = expr2->AsAssignmentExpression();
  ASSERT_TRUE(assign_expr2->GetLeft()->IsIdentifier());
  ASSERT_TRUE(assign_expr2->GetRight()->IsBinaryExpression());
  EXPECT_EQ(TokenType::ASSIGN, assign_expr2->GetOperator());
  EXPECT_EQ(u"y", assign_expr2->GetLeft()->AsIdentifier()->GetName());

  auto binary_expr = assign_expr2->GetRight()->AsBinaryExpression();
  ASSERT_TRUE(binary_expr->GetLeft()->IsIdentifier());
  ASSERT_TRUE(binary_expr->GetRight()->IsNumericLiteral());
  EXPECT_EQ(TokenType::ADD, binary_expr->GetOperator());
  EXPECT_EQ(u"x", binary_expr->GetLeft()->AsIdentifier()->GetName());
  EXPECT_EQ(1, binary_expr->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
}


TEST(parser, ParseBlockStatement) {
  // {}
  {
    Parser parser(u"{ }");

    auto stmt = parser.ParseBlockStatement();
    ASSERT_TRUE(stmt->IsBlockStatement());

    auto block_stmt = stmt->AsBlockStatement();
    EXPECT_EQ(0, block_stmt->GetStatements().size());
  }
}

TEST(parser, ParseVariableStatement) {
  Parser parser(u"var i = 'test', j = i + 1;");

  auto stmt = parser.ParseVariableStatement();
  ASSERT_TRUE(stmt->IsVariableStatement());

  auto var_stmt = stmt->AsVariableStatement();
  ASSERT_TRUE(var_stmt->GetVariableDeclarations().size() == 2);

  const auto& decls = var_stmt->GetVariableDeclarations();
    
  auto decl1 = decls[0];
  ASSERT_TRUE(decl1->GetIdentifier()->IsIdentifier());
  ASSERT_TRUE(decl1->GetInitializer()->IsStringLiteral());
  EXPECT_EQ(u"i", decl1->GetIdentifier()->AsIdentifier()->GetName());
  EXPECT_EQ(u"test", decl1->GetInitializer()->AsStringLiteral()->GetString());

  auto decl2 = decls[1];
  ASSERT_TRUE(decl2->GetIdentifier()->IsIdentifier());
  ASSERT_TRUE(decl2->GetInitializer()->IsBinaryExpression());
  EXPECT_EQ(u"j", decl2->GetIdentifier()->AsIdentifier()->GetName());

  auto binary_expr = decl2->GetInitializer()->AsBinaryExpression();
  ASSERT_TRUE(binary_expr->GetLeft()->IsIdentifier());
  ASSERT_TRUE(binary_expr->GetRight()->IsNumericLiteral());
  EXPECT_EQ(TokenType::ADD, binary_expr->GetOperator());
  EXPECT_EQ(u"i", binary_expr->GetLeft()->AsIdentifier()->GetName());
  EXPECT_EQ(1, binary_expr->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
}

TEST(parser, ParseEmptyStatement) {
  Parser parser(u"  ;  ");
  
  auto stmt = parser.ParseEmptyStatement();
  ASSERT_TRUE(stmt->IsEmptyStatement());
}

TEST(parser, ParseExpressionStatement) {
  Parser parser(u"str + 'Hello';");
  
  auto stmt = parser.ParseExpressionStatement();
  ASSERT_TRUE(stmt->IsExpressionStatement());

  auto expr_stmt = stmt->AsExpressionStatement();
  ASSERT_TRUE(expr_stmt->GetExpression()->IsBinaryExpression());

  auto binary_expr = expr_stmt->GetExpression()->AsBinaryExpression();
  ASSERT_TRUE(binary_expr->GetLeft()->IsIdentifier());
  ASSERT_TRUE(binary_expr->GetRight()->IsStringLiteral());
  EXPECT_EQ(TokenType::ADD, binary_expr->GetOperator());
  EXPECT_EQ(u"str", binary_expr->GetLeft()->AsIdentifier()->GetName());
  EXPECT_EQ(u"Hello", binary_expr->GetRight()->AsStringLiteral()->GetString());
}

TEST(parser, ParseIfStatement) {
  std::u16string source = uR"(
if (true) {
  i += 2;
} else {
  var j = i;
  j *= 2;
}
)";

  Parser parser(source);

  auto stmt = parser.ParseIfStatement();
  ASSERT_TRUE(stmt->IsIfStatement());

  auto if_stmt = stmt->AsIfStatement();
  ASSERT_TRUE(if_stmt->GetCondition()->IsBooleanLiteral());
  ASSERT_TRUE(if_stmt->GetConsequent()->IsBlockStatement());
  ASSERT_TRUE(if_stmt->GetConsequent()->IsBlockStatement());
  EXPECT_EQ(true, if_stmt->GetCondition()->AsBooleanLiteral()->GetBoolean());

  auto block_stmt1 = if_stmt->GetConsequent()->AsBlockStatement();
  ASSERT_TRUE(block_stmt1->GetStatements().size() == 1);

  {
    const auto& stmts = block_stmt1->GetStatements();

    auto stmt = stmts[0];
    ASSERT_TRUE(stmt->IsExpressionStatement());

    auto expr_stmt = stmt->AsExpressionStatement();
    ASSERT_TRUE(expr_stmt->GetExpression()->IsAssignmentExpression());

    auto assign_expr = expr_stmt->GetExpression()->AsAssignmentExpression();
    ASSERT_TRUE(assign_expr->GetLeft()->IsIdentifier());
    ASSERT_TRUE(assign_expr->GetRight()->IsNumericLiteral());
    EXPECT_EQ(TokenType::ADD_ASSIGN, assign_expr->GetOperator());
    EXPECT_EQ(u"i", assign_expr->GetLeft()->AsIdentifier()->GetName());
    EXPECT_EQ(2, assign_expr->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
  }

  auto block_stmt2 = if_stmt->GetAlternate()->AsBlockStatement();
  ASSERT_TRUE(block_stmt2->GetStatements().size() == 2);

  {
    const auto& stmts = block_stmt2->GetStatements();

    auto stmt1 = stmts[0];
    ASSERT_TRUE(stmt1->IsVariableStatement());

    auto var_stmt = stmt1->AsVariableStatement();
    ASSERT_TRUE(var_stmt->GetVariableDeclarations().size() == 1);

    const auto& decls = var_stmt->GetVariableDeclarations();

    auto decl = decls[0];
    ASSERT_TRUE(decl->GetIdentifier()->IsIdentifier());
    ASSERT_TRUE(decl->GetInitializer()->IsIdentifier());
    EXPECT_EQ(u"j", decl->GetIdentifier()->AsIdentifier()->GetName());
    EXPECT_EQ(u"i", decl->GetInitializer()->AsIdentifier()->GetName());

    auto stmt2 = stmts[1];
    
    auto expr_stmt = stmt2->AsExpressionStatement();
    ASSERT_TRUE(expr_stmt->GetExpression()->IsAssignmentExpression());

    auto assign_expr = expr_stmt->GetExpression()->AsAssignmentExpression();
    ASSERT_TRUE(assign_expr->GetLeft()->IsIdentifier());
    ASSERT_TRUE(assign_expr->GetRight()->IsNumericLiteral());
    EXPECT_EQ(TokenType::MUL_ASSIGN, assign_expr->GetOperator());
    EXPECT_EQ(u"j", assign_expr->GetLeft()->AsIdentifier()->GetName());
    EXPECT_EQ(2, assign_expr->GetRight()->AsNumericLiteral()->GetNumber<std::int32_t>());
  }
}
