#include <iostream>

#include "codesize.tab.hh"
#include "codesize.yy.hh"
#include "codesize.hpp"

std::vector<FEntry> functions;
int yyparse();
extern int yydebug;


static void show_help()
{
  std::cout << "Usage: codesize [options] listing_file\n" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "-f\tshow functions size\n" << std::endl;
  std::cout << "Listing_file should be a listing of binary file generated by avr-objdump -d file.bin > file.lst" << std::endl;
}


int main (int argc, char **argv)
{
  char opt_f=0;
  char *filename=0;

  if (argc<2)
  {
    show_help();
    return 1;
  }

  for (int i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "-f")==0)
      opt_f=1;
    else if (i+1==argc)
      filename=argv[i];
  }

  if (filename==0)
  {
    show_help();
    return 1;
  }

  FILE *input;
  if ( (input=fopen(filename, "r")) )
  {
    char temp[64];
    for (int i=0; i<6; i++)
      fgets(temp, 64, input);       //pierwsze 6 lini jest niepotrzebne

    yyin=input;
    yyparse();

    //posortuj funkcje metodą naiwną ;)
    const int size=functions.size();
    bool repeat;
    do
    {
      repeat=false;
      for (int i=0; i<size-1; i++)
        if (functions[i].size<functions[i+1].size)  //porównywanie sąsiadów
        {
          FEntry tmp=functions[i];
          functions[i]=functions[i+1];
          functions[i+1]=tmp;             //zamiana elementów
          repeat=true;
        }
    }
    while (repeat);

    unsigned int maxlen=0;

    for (int i=0; i<size; i++)
      if (functions[i].name.length()>maxlen)
        maxlen=functions[i].name.length();

    //wyświetl wynik
    for (int i=0; i<size; i++)
    {
      int diff=maxlen-functions[i].name.length();
      std::string space;
      for (int j=0; j<diff; j++)
        space += " ";

      std::cout << functions[i].name << ": " << space << functions[i].size << std::endl;
    }
  }
  else
  {
    std::cout << "Could not open " << filename << " file" << std::endl;
    return 2;
  }

  return 0;
}
