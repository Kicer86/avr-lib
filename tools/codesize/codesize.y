
%{         
  #define YYSTYPE std::string
  
  #include <string>
  #include <iostream>
  
  #include "codesize.yy.hh"  
  #include "codesize.hpp"
  
  int yyerror(char const* s)
  {
    std::cout << "error: " << s << std::endl;
    return 1;
  }       
  
  FEntry func;
%}

%error-verbose
%pure_parser

%token FADDRESS
%token FNAME
%token ADDRESS
%token BYTE
%token EOFUN

%%

//eat lines
input:    /* empty */;
input:    input line;

line:     FADDRESS FNAME    { func.name=$2; func.size=0; };
line:     ADDRESS  bytes;
line:     EOFUN             { functions.push_back(func); };

bytes:    BYTE              { func.size++; };
bytes:    bytes BYTE        { func.size++; };

%%
