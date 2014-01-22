#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

#include "Token.h"


int main(int argc, char *argv[])
{
	string LineBuffer;
	int    LineCounter = 1;
	int    CharIndex = 0;
	InitKeywordMap();

	token_t Token;

    if(argc != 2) 
	{
        cout << "Usage: testlex <filename>\n";
        return 1;
    }

	string SourceFileName = argv[1];
    string ResultFileName = SourceFileName.substr(0, SourceFileName.rfind("."));
	ResultFileName += ".txt";

    ifstream SourceFile (SourceFileName);
    ofstream ResultFile (ResultFileName);

    ResultFile << "Fichier source : " << SourceFileName << "\n";
    ResultFile << "---------------------------------------------------------\n";

	while(getline(SourceFile, LineBuffer)) 
	{
        ResultFile << setiosflags( ios::right )
                   << setw( 4 )  
                   << setfill('0')  
                   << LineCounter << ' '
                   << setfill(' ')  
                   << resetiosflags( ios::right )
                   << setw( 0 ) 
                   << LineBuffer 
                   << endl;
		while(CharIndex < LineBuffer.length())
		{
			GetToken(LineBuffer, CharIndex, ResultFile, Token, true);
		};
		CharIndex = 0;

        ResultFile << "---------------------------------------------------------\n";
		LineCounter++;
    }

    SourceFile.close();
    ResultFile.close();

    return 0;
}

