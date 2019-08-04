//
//  lexer_test.cpp
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2018, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#include <gtest/gtest.h>
#include <test/acatl/test_helper.h>

#include <acatl/lexer.h>
#include <acatl/string_helper.h>


class TestSQLLexer
{
public:
  enum eToken
  {
    IDENTIFIER,
    FLOAT,
    INTEGER,
    EQUAL,
    COMMA,
    LEFT_PAREN,
    RIGHT_PAREN,
    STAR,
    STRING,
    COMMENT,
    SELECT,
    FROM,
    WHERE,
    BY,
    GROUP,
    LIMIT,
    SUM,
    COUNT,
    LIKE,
    AND
  };

  TestSQLLexer(const std::string& text)
  : _lexer(std::bind(&TestSQLLexer::inspectToken, this, std::placeholders::_1))
  {
    _lexer.addDefinition("identifier", R"([_a-zA-Z][_a-zA-Z0-9]*)", IDENTIFIER);
    _lexer.addDefinition("float", R"([+-]?(?:0|[1-9]\d*)\.\d+)", FLOAT);
    _lexer.addDefinition("integer", R"(0|[+-]?[1-9]\d*)", INTEGER);
    _lexer.addDefinition("equal", R"(=)", EQUAL);
    _lexer.addDefinition("comma", R"(,)", COMMA);
    _lexer.addDefinition("left_paren", R"(\()", LEFT_PAREN);
    _lexer.addDefinition("right_paren", R"(\))", RIGHT_PAREN);
    _lexer.addDefinition("star", R"(\*)", STAR);
    _lexer.addDefinition("string", R"('([^']*)')", STRING);
    _lexer.addDefinition("comment", R"(//([^\r\n]*))", COMMENT);
    _lexer.addDefinition("comment", R"(/\*((.|[\r\n])*?)\*/)", COMMENT);

    _keywords["SELECT"] = eToken(SELECT);
    _keywords["FROM"] = eToken(FROM);
    _keywords["WHERE"] = eToken(WHERE);
    _keywords["SUM"] = eToken(SUM);
    _keywords["COUNT"] = eToken(COUNT);
    _keywords["LIKE"] = eToken(LIKE);
    _keywords["AND"] = eToken(AND);
    _keywords["GROUP"] = eToken(GROUP);
    _keywords["BY"] = eToken(BY);

    _lexer.setInput(text);
  }

  acatl::lexer::Token next()
  {
    return _lexer.next();
  }

  uint32_t lineCount() const
  {
    return _lexer.lineCount();
  }

private:
  typedef std::map<std::string, eToken> Keywords;

  void inspectToken(acatl::lexer::Token& token)
  {
    if(token._token == IDENTIFIER) {
      Keywords::const_iterator iter = _keywords.find(acatl::toupper_copy(token._value));
      if(iter != _keywords.end()) {
        token._token = iter->second;
      }
    }
  }

  acatl::lexer::Lexer _lexer;
  Keywords _keywords;
};


TEST(LexerTest, sqlLexerTest)
{
  std::string text = R"(/** and now a c style comment
with nested new lines
and more new lines **/
// this is a test
SELECT a,sum(b),count(*),c_d from test where a like 'test' and b= -70.6
group by a,c_d)";

  TestSQLLexer lexer(text);
  acatl::lexer::Token token = lexer.next();
  std::vector<acatl::lexer::Token> tokens;
  int n = 0;
  while(token._token != acatl::lexer::EOI) {
    tokens.push_back(token);
    ++n;
    token = lexer.next();
  }
  EXPECT_EQ(6u, lexer.lineCount());
  EXPECT_EQ(TestSQLLexer::COMMENT, tokens[0]._token);
  EXPECT_EQ(TestSQLLexer::SELECT, tokens[2]._token);
  EXPECT_EQ(tokens[2]._value, "SELECT");
  EXPECT_EQ(TestSQLLexer::SUM, tokens[5]._token);
  EXPECT_EQ(tokens[5]._value, "sum");
  EXPECT_EQ(TestSQLLexer::LEFT_PAREN, tokens[6]._token);
  EXPECT_EQ(TestSQLLexer::STAR, tokens[12]._token);
  EXPECT_EQ(TestSQLLexer::FROM, tokens[16]._token);
  EXPECT_EQ(tokens[16]._value, "from");
  EXPECT_EQ(TestSQLLexer::STRING, tokens[21]._token);
  EXPECT_EQ(tokens[21]._value, "test");
  EXPECT_EQ(TestSQLLexer::FLOAT, tokens[25]._token);
  EXPECT_EQ(tokens[25]._value, "-70.6");
}

TEST(LexerTest, exceptionTest)
{
  std::string text("select 1.1.1 from test");
  TestSQLLexer lexer(text);
  acatl::lexer::Token token = lexer.next();
  token = lexer.next();
  EXPECT_THROW(lexer.next(), acatl::lexer::LexicalAnalysisException);
}
