#include <iostream>

#include "TClass.h"
#include "TString.h"
#include "TApplication.h"
#include "TMVAGui.C"

int main(int argc, char * argv[]) {
  if(argc != 2)
  {
    std::cout << "ERROR | Parameter needed: InputFile" << std::endl;
    return 1;
  }
  else {
    TString name = argv[1];
    
    TApplication* theApp;
    theApp = new TApplication("App", &argc, argv);

    TMVAGui(name);

    theApp->Run();

    return 0; 
  }
}