#include "mthpch.h"

#include "CLI/Application.h"

int32 main(int32 argc, char** argv)
{
	Application* app = Application::Get();
	app->LoadArguments(argc, argv);
	app->Run();
	int32 result = app->Abort();

	delete app;

	return result;
}