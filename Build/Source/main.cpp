#include "mthpch.h"

#include "App.h"

int main(int argc, char** argv) 
{
   App* app = App::Get();
   app->LoadCL(argc, argv);
   app->Run();
   int result = app->Abort();

   delete app;

   return result;
}