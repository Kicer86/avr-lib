%{
  #define YYSTYPE std::string    //kontener na dane leksera
  
  #include <string>
  
  #include "codesize.tab.hh"         //include bison's definitions
%}

%option bison-bridge
%option noyywrap

%x LINE

ENDL [\n\r]
%%

^[[:xdigit:]]{8}    {    //na poczatku lini wartości hexadecymalne ? - adres funkcji
                      *yylval=yytext;
/*                       std::cout << "\"" << *yylval << "\"" << std::endl; */
                      return FADDRESS;
                    }
                    
\<.*\>:             {   //<...> ? nazwa funkcji
                      *yylval=yytext;
                      yylval->erase(yylval->length()-2,2); //usuń dwa ostatnie znaki (>:)
                      yylval->erase(yylval->begin());      //oraz pierwszy (<)                      
                      return FNAME;
                    }

^[[:blank:]]*[[:xdigit:]]+:\t { //adres instrukcji w funkcji
                                BEGIN(LINE);
                                return ADDRESS;
                              }
                     
^{ENDL}                 { return EOFUN; } //empty line

{ENDL}              {}  //eat new lines
.                   {}  //zjedz błądzące znaki

<LINE>{

\ +                 {}  //eat spaces
[[:xdigit:]]{2}     { return BYTE; }
[[:blank:]]{2}.*{ENDL}          { BEGIN(INITIAL); }  //end of line (rozkaz + pierdoły)

}

%%
