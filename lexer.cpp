#include "lexer.h"
#include <cctype>
#include <iostream>

using namespace std;
using ll = long long;

// 생성자
Lexer::Lexer(string input)
{
    code = input;

    pos = 0;

    line = 1;

    column = 1;
}


// 코드 전체를 토큰으로 변환
vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;


    while(pos < code.size())
    {
        char c = code[pos];


        // 줄바꿈
        if(c == '\n')
        {
            tokens.push_back(
                Token(TokenType::NewLine, "\n", line, column)
            );

            pos++;

            line++;
            column = 1;

            continue;
        }

        if(c=='_')
        {
            tokens.push_back(
                Token(TokenType::UnderBar, "_", line, column)
            );
            pos++;
            column++;
            continue;
        }

        if(c==',')
        {
            tokens.push_back(
                Token(TokenType::Comma, ",", line, column)
            );
            pos++;
            column++;
            continue;
        }


        // 공백
        if(isspace(c))
        {
            pos++;
            column++;

            continue;
        }

        if(c == '\\')
        {
            int startColumn = column;
            pos++;
            column++;

            string name = "";

            while(pos < code.size() && isalpha(code[pos]))
            {
                name += code[pos];

                pos++;
                column++;
            }
            if(name == "repeat")
            {
                tokens.push_back(
                    Token(TokenType::Repeat, "repeat", line, startColumn)
                );
            }

            if(name == "if")
            {
                tokens.push_back(
                    Token(TokenType::If, "if", line, startColumn)
                );
            }

            if(name == "func" || name == "Func")
            {
                tokens.push_back(
                    Token(TokenType::Function, "Function", line, startColumn)
                );
            }

            if(name == "print")
            {
                tokens.push_back(
                    Token(TokenType::Print, name, line, startColumn)
                );
            }
            if(name == "not")
            {
                tokens.push_back(
                    Token(TokenType::Not, name, line, startColumn)
                );
            }
            if(name == "return" || name == "Return")
            {
                tokens.push_back(
                    Token(TokenType::Return, name, line, startColumn)
                );
            }

            continue;

        }

        // 숫자
        if(isdigit(c))
        {
            ll startColumn = column;

            string number = "";


            while(pos < code.size() && isdigit(code[pos]))
            {
                number += code[pos];

                pos++;
                column++;
            }


            tokens.push_back(
                Token(TokenType::Number, number, line, startColumn)
            );


            continue;
        }



        // 변수 / 명령어
        if(isalpha(c))
        {
            ll startColumn = column;

            string name = "";


            while(pos < code.size() && isalnum(code[pos]))
            {
                name += code[pos];

                pos++;
                column++;
            }
            if(name == "not")
            {
                tokens.push_back(
                    Token(TokenType::Not, name, line, startColumn)
                );
            }
            else if(name == "and")
            {
                tokens.push_back(
                    Token(TokenType::And, name, line, startColumn)
                );
            }
            else if(name == "or")
            {
                tokens.push_back(
                    Token(TokenType::Or, name, line, startColumn)
                );
            }
            else if(name == "true" || name == "True")
            {
                tokens.push_back(
                    Token(TokenType::Boolean, "true", line, startColumn)
                );
            }
            else if(name == "false" || name == "False")
            {
                tokens.push_back(
                    Token(TokenType::Boolean, "false", line, startColumn)
                );
            }
            else
            {
                tokens.push_back(
                    Token(TokenType::Identifier, name, line, startColumn)
                );
            }


            continue;
        }



        // 문자열
        if(c == '"')
        {
            ll startColumn = column;

            pos++;
            column++;


            string str = "";


            while(pos < code.size() && code[pos] != '"')
            {
                if(code[pos]=='\\')
                {
                    if(code[pos+1]=='n')
                    {
                        str += '\n';
                        pos+=2;
                        column+=2;
                        continue;
                    }
                    if(code[pos+1]=='\\')
                    {
                        str += '\\';
                        pos+=2;
                        column+=2;
                        continue;
                    }
                }
                str += code[pos];

                pos++;
                column++;
            }


            if(pos >= code.size())
            {
                throw runtime_error("Unclosed string");
            }


            // 닫는 "
            pos++;
            column++;


            tokens.push_back(
                Token(TokenType::String, str, line, startColumn)
            );


            continue;
        }




		// 비교 연산자 (=, ==, <, <=, >, >=, 예외 : =>(equal이 여기 있어서 어쩔 수 x))

		if(c == '=')
		{
			if(pos + 1 < code.size() && code[pos + 1] == '=')
			{
				tokens.push_back(
					Token(TokenType::Equal, "==", line, column)
				);

				pos += 2;
				column += 2;
			}
			else if(pos + 1 < code.size()&&code[pos+1]=='>')
            {
                tokens.push_back(
                    Token(TokenType::Arrow, "=>", line, column)
                );
                pos+=2;
                column+=2;
            }
			else
			{
				tokens.push_back(
					Token(TokenType::Assign, "=", line, column)
				);

				pos++;
				column++;
			}

			continue;
		}


		if(c == '<')
		{
			if(pos + 1 < code.size() && code[pos + 1] == '=')
			{
				tokens.push_back(
					Token(TokenType::LessEqual, "<=", line, column)
				);

				pos += 2;
				column += 2;
			}
			else
			{
				tokens.push_back(
					Token(TokenType::Less, "<", line, column)
				);

				pos++;
				column++;
			}

			continue;
		}


		if(c == '>')
		{
			if(pos + 1 < code.size() && code[pos + 1] == '=')
			{
				tokens.push_back(
					Token(TokenType::GreaterEqual, ">=", line, column)
				);

				pos += 2;
				column += 2;
			}
			else
			{
				tokens.push_back(
					Token(TokenType::Greater, ">", line, column)
				);

				pos++;
				column++;
			}

			continue;
		}

		if(c=='~')
        {
            if(pos+1<code.size()&&code[pos+1]=='=')
            {
                tokens.push_back(
                    Token(TokenType::NotEqual, "~=", line, column)
                );
                pos+=2;
                column+=2;
            }
            continue;
        }


		// 일반 연산자

		switch(c)
		{
			case '+':
				tokens.push_back(
					Token(TokenType::Plus, "+", line, column)
				);
				break;


			case '-':
				tokens.push_back(
					Token(TokenType::Minus, "-", line, column)
				);
				break;


			case '*':
				tokens.push_back(
					Token(TokenType::Multiply, "*", line, column)
				);
				break;


			case '/':
				tokens.push_back(
					Token(TokenType::Divide, "/", line, column)
				);
				break;


			case '(':
				tokens.push_back(
					Token(TokenType::LeftParen, "(", line, column)
				);
				break;


			case ')':
				tokens.push_back(
					Token(TokenType::RightParen, ")", line, column)
				);
				break;

            case '{':
                tokens.push_back(
                    Token(TokenType::LeftBrace, "{", line, column)
                );
                break;

            case '}':
                tokens.push_back(
                    Token(TokenType::RightBrace, "}", line, column)
                );
                break;

            case '%':
                tokens.push_back(
                    Token(TokenType::Modulo, "%", line, column)
                );
                break;

			default:
				throw runtime_error(
					string("Unknown character : ") + c
				);
		}


		// 연산자 하나 소비
		pos++;
		column++;

    }

    // 끝
    tokens.push_back(
        Token(TokenType::End, "", line, column)
    );


    return tokens;
}
