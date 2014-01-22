#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>

using namespace std;

#include "Token.h"

enum State_t { STATE_BEGIN, END_STATE, COMMENT_FOUND1, COMMENT_FOUND2, 
	           IDENTIFIER_FOUND, CHAR_FOUND1, CHAR_FOUND2, 
	           STRING_FOUND1, STRING_FOUND2, STRING_ESCAPE,  
			   INTEGER_FOUND, OPERATOR_FOUND };

const char *TokenTypeName[] = {
                    "LEX_UNDEFINED",  
					"LEX_LINE_COMMENT", "LEX_BEGIN_COMMENT", "LEX_END_COMMENT",
					"LEX_IDENTIFIER", "LEX_KEYWORD",  
					"LEX_CHAR",   "LEX_INTEGER",  "LEX_STRING", 
					"LEX_OPERATOR",
					"LEX_ASSIGNMENT", 
					"LEX_COMMA", "LEX_SEMICOLON",
					"LEX_LEFT_PARENTHESIS", "LEX_RIGHT_PARENTHESIS",
					"LEX_LEFT_BRACE", "LEX_RIGHT_BRACE",
					"LEX_END" };

const char *OperatorName[] = {
	                "LEX_NEG",  "LEX_INC",  "LEX_DEC",  "LEX_NOT",
    				"LEX_NO_OPR", "LEX_ADD",  "LEX_MUL",  "LEX_SUB",  "LEX_DIV",
					"LEX_EQ",   "LEX_NE",   "LEX_LT",   "LEX_GT",  "LEX_LE",  "LEX_GE",
					"LEX_COUT_OPR", "LEX_CIN_OPR" };

const char *KeywordName[] = { 
	             "LEX_IF",     "LEX_ELSE",  "LEX_FOR",   "LEX_DO",   "LEX_WHILE", 
                 "LEX_SWITCH", "LEX_CASE",  "LEX_BREAK", "LEX_COUT", "LEX_CIN" };

static void PrintTokenInfo(ofstream &ListingFile, token_t token, int BeginIndex, int Index)
{
	ListingFile << setw(BeginIndex + 6)
				<< "^"; 

	if(token.TokenString.length() > 1)
		ListingFile << setw(token.TokenString.length() - 1) << "^";
				     
	ListingFile	<< endl
				<< "     " 
				<< setw(15)
                << setiosflags( ios::left )
                << token.TokenString 
				<< TokenTypeName[token.TokenType];

	if(token.TokenType == LEX_OPERATOR || token.TokenType == LEX_ASSIGNMENT)
		ListingFile << "  " << OperatorName[token.OprID];

	if(token.TokenType == LEX_KEYWORD)
		ListingFile <<  "  " << KeywordName[token.KeywordID];

	ListingFile << resetiosflags( ios::left )
                << endl;
}

static map<string, Keyword_t> KeywordMap;
static map<string,Keyword_t>::iterator it;

void InitKeywordMap()
{
	KeywordMap["if"]     = LEX_IF;
	KeywordMap["else"]   = LEX_ELSE;
	KeywordMap["for"]    = LEX_FOR;
	KeywordMap["do"]     = LEX_DO;
	KeywordMap["while"]  = LEX_WHILE;
	KeywordMap["switch"] = LEX_SWITCH;
	KeywordMap["case"]   = LEX_CASE;
	KeywordMap["break"]  = LEX_BREAK;
	KeywordMap["cout"]   = LEX_COUT;
	KeywordMap["cin"]    = LEX_CIN;
};
		
LexErrorCode_t GetToken (string Buffer, int &Index, ofstream &ListingFile, token_t &token, bool testing)
{
    State_t        LexState = STATE_BEGIN;
	LexErrorCode_t LexError = LEX_NO_ERROR;

	if(token.TokenType == LEX_BEGIN_COMMENT) LexState = COMMENT_FOUND2;

	token.TokenType   = LEX_UNDEFINED;
	token.TokenString = "";
	token.IntVal = 0;

	while( Buffer[Index] == ' ' ) Index++;

	int BeginIndex = Index;

    while( (LexState != END_STATE) && (LexError == LEX_NO_ERROR) )
	{
		char ch = Buffer[Index];
		token.TokenString += ch;

		switch(LexState)
		{
//---------------------------------------------------------------------------
		  case STATE_BEGIN:
			if(isalpha(ch))
			{
				LexState = IDENTIFIER_FOUND;
				break;
			}

			if(isdigit(ch))
			{
				LexState = INTEGER_FOUND;
				break;
			}

		    switch(ch)
			{
			  case '\\':
				LexState = COMMENT_FOUND1;
				break;

			  case '\'':
				token.TokenString.pop_back();
				LexState = CHAR_FOUND1;
				break;

			  case '\"':
				token.TokenString.pop_back();
				LexState = STRING_FOUND1;
				break;

			  case ',':
				LexState = END_STATE;
				token.TokenType = LEX_COMMA;
				break;

			  case ';':
				LexState = END_STATE;
				token.TokenType = LEX_SEMICOLON;
				break;

			  case '(':
				LexState = END_STATE;
				token.TokenType = LEX_LEFT_PARENTHESIS;
				break;

			  case ')':
				LexState = END_STATE;
				token.TokenType = LEX_RIGHT_PARENTHESIS;
				break;

			  case '{':
				LexState = END_STATE;
				token.TokenType = LEX_LEFT_BRACE;
				break;

			  case '}':
				LexState = END_STATE;
				token.TokenType = LEX_RIGHT_BRACE;
				break;

			  case '=':
			  case '!':
		      case '+':
			  case '-':
			  case '*':
			  case '/':
			  case '<':
			  case '>':
				LexState = OPERATOR_FOUND;
				break;
			}
			break;

//---------------------------------------------------------------------------

		case IDENTIFIER_FOUND:
			if(isalnum(ch))
				LexState = IDENTIFIER_FOUND;
			else
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				it = KeywordMap.find(token.TokenString);
				if(it == KeywordMap.end())
					token.TokenType = LEX_IDENTIFIER;
				else
				{
					token.TokenType = LEX_KEYWORD;
					token.KeywordID = (Keyword_t)it->second;
				}
				Index--;
			}
			break;

//---------------------------------------------------------------------------

		case INTEGER_FOUND:
			if(isdigit(ch))
				LexState = INTEGER_FOUND;
			else
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				token.TokenType = LEX_INTEGER;
			}
			break;

//---------------------------------------------------------------------------

		case COMMENT_FOUND1:
			if(ch == '\\')
			{
				token.TokenType = LEX_LINE_COMMENT;
				LexState = END_STATE;
				break;
			}

			else 
			{
				if(ch == '*')
				{
					token.TokenType = LEX_BEGIN_COMMENT;
					LexState = END_STATE;
					break;
				}
				else
					return LEX_UNKNOWN_CHAR;
			}
			break;
//---------------------------------------------------------------------------
		case COMMENT_FOUND2:
			return GetEndOfComment(Buffer, Index, ListingFile, token, testing);
//---------------------------------------------------------------------------

		case CHAR_FOUND1:
			if(ch == '\'')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				token.CharVal = '\0';
				token.TokenType = LEX_CHAR;
			}
			else
			{
                if(isprint(ch))
				    LexState = CHAR_FOUND2;
			    else
					return LEX_UNKNOWN_CHAR;
			};
			break;

		case CHAR_FOUND2:
			if(ch == '\'')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				token.TokenType = LEX_CHAR;
			}
			break;

//---------------------------------------------------------------------------

		case STRING_FOUND1:
			if(ch == '\"')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				token.TokenType = LEX_STRING;
			}
			break;

			if(ch == '\\')
			{
				LexState = STRING_ESCAPE;
				token.TokenString.pop_back();
			}
			break;

            if(isprint(ch))
				LexState = STRING_FOUND2;
			else
				return LEX_UNKNOWN_CHAR;
			break;

		case STRING_FOUND2:
			if(ch == '\"')
			{
				LexState = END_STATE;
				token.TokenString.pop_back();
				token.TokenType = LEX_STRING;
			}
			break;

			if(ch == '\\')
			{
				LexState = STRING_ESCAPE;
				token.TokenString.pop_back();
			}
			break;

            if(isprint(ch))
				LexState = CHAR_FOUND2;
			else
				return LEX_UNKNOWN_CHAR;
			break;

		case STRING_ESCAPE:
			if(ch == 'n')
			{
				LexState = STRING_FOUND2;
				token.TokenString += '\n';
				token.TokenType = LEX_STRING;
			}
			else
				return LEX_ILLEGAL_ESCAPE_CHAR;
			break;

//---------------------------------------------------------------------------
		case OPERATOR_FOUND:
		    switch(token.TokenString[0])
			{
			  case '!':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_NE;
				}
				else
				{
					LexState = END_STATE;
					token.TokenString.pop_back();
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_NOT;
				}
				break;

		      case '+':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_ADD;
				}
				else if(ch == '+')
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_INC;
				}
				else
				{
					LexState = END_STATE;
					token.TokenString.pop_back();
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_ADD;
				}
				break;

			  case '-':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_SUB;
				}
				else if(ch == '-')
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_DEC;
				}
				else
				{
					LexState = END_STATE;
					token.TokenString.pop_back();
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_SUB;
				}
				break;

			  case '*':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_MUL;
				}
				else
				{
					LexState = END_STATE;
					token.TokenString.pop_back();
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_MUL;
				}
				break;

			  case '/':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_DIV;
				}
				else
				{
					LexState = END_STATE;
					token.TokenString.pop_back();
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_DIV;
				}
				break;

			  case '=':
				if(ch == '=')
				{
					LexState = END_STATE;
					token.TokenType = LEX_OPERATOR;
					token.OprID = LEX_EQ;
				}
				else
				{
					LexState = END_STATE;
					token.TokenType = LEX_ASSIGNMENT;
					token.OprID = LEX_NO_OPR;
					token.TokenString.pop_back();
					Index--;
				}
				break;

			  case '<':
				switch(ch)
				{
					case '<': LexState = END_STATE;
						token.TokenType = LEX_OPERATOR;
						token.OprID = LEX_COUT_OPR;
						break;
					case '=': LexState = END_STATE;
						token.TokenType = LEX_OPERATOR;
						token.OprID = LEX_LE;
						break;
					default: LexState = END_STATE;
						token.TokenType = LEX_OPERATOR;
						token.OprID = LEX_LT;
						Index--;
				}
				break;

			  case '>':
				switch(ch)
				{
					case '>': LexState = END_STATE;
						token.TokenType = LEX_OPERATOR;
						token.OprID = LEX_CIN_OPR;
						break;
					case '=': LexState = END_STATE;
						token.TokenType = LEX_OPERATOR;
						token.OprID = LEX_GE;
						break;
					default: LexState = END_STATE;
						token.TokenType = LEX_OPERATOR;
						token.OprID = LEX_GT;
						Index--;
				}
				break;
			}
			break;
		}  // End while
//---------------------------------------------------------------------------
		Index++;
    }
	if(testing)
	    PrintTokenInfo(ListingFile, token, BeginIndex, Index);
	
	return LEX_NO_ERROR;
};

//---------------------------------------------------------------------------
LexErrorCode_t GetEndOfComment (string Buffer, int &Index, ofstream &ListingFile, token_t &token, bool testing)
{ 
	Index = Buffer.find("*\\",Index);

	if(Index != std::string::npos)
	{
		token.TokenType = LEX_END_COMMENT;
		token.TokenString = "*\\";
		Index+=2;
	}
	else
	{
		Index = Buffer.length();
		return LEX_END_OF_COMMENT_NOT_FOUND;
	}
	return LEX_NO_ERROR;
};

