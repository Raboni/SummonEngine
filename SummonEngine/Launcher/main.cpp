#include "stdafx.h"
#include "Engine.h"
#include "Scene.h"
#include "Threadpool.h"
#include "SpriteUnits.h"
#include "PhysicsInterface.h"
#include "..\Game\Game.h"
#include "..\Editor\Editor.h"

#include <Camera.hpp>
#include <StateStack.hpp>
#include <Time.hpp>
#include <CommandLineManager.h>
#include <Input.hpp>
#include <FileWatcher.h>
#include <DebugTools.h>

#include <iostream>
#include <Windows.h>
#include <DbgHelp.h>

#define USE_THREADS
//#define USE_DUMP
//#define TIME_DURATIONS

#pragma comment(lib,"Dbghelp.lib")

#ifdef USE_DUMP
bool CreateMiniDump(_EXCEPTION_POINTERS* aException)
{
	HANDLE file = CreateFile("MiniDump.dmp", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	bool dumped = false;
	if (file != NULL && file != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION info;
		info.ThreadId = GetCurrentThreadId();
		info.ExceptionPointers = aException;
		info.ClientPointers = true;

		dumped = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MINIDUMP_TYPE::MiniDumpNormal, &info, nullptr, nullptr);
		if (!dumped)
		{
			CU::Log("MiniDump.dmp not created. Error: " + std::to_string(GetLastError()), CU::LogType::Error);
		}
		else
		{
			CU::Log("MiniDump.dmp created", CU::LogType::Success);
		}
		CloseHandle(file);
	}
	return dumped;
}

LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aException)
{
	CreateMiniDump(aException);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void Run()
{
	CU::CommandLineManager::CreateInstance();
	CU::FileWatcher::CreateInstance();
	Threadpool::CreateInstance(32);

	CU::Time timeKeeper;
	WindowHandler::WindowData winData;
	winData.myTitle = "Summon Engine";
	const bool runServer = CU::CommandLineManager::GetInstance()->HasParameter("server");
	const bool runEditor = CU::CommandLineManager::GetInstance()->HasParameter("editor");
	if (runServer)
	{
		winData.myTitle += " Server";
	}
	else if (runEditor)
	{
		winData.myTitle += " Editor";
	}
	if (CU::CommandLineManager::GetInstance()->HasParameter("position"))
	{
		winData.myX = std::stoi(CU::CommandLineManager::GetInstance()->GetArgument("position", 0));
		winData.myY = std::stoi(CU::CommandLineManager::GetInstance()->GetArgument("position", 1));
	}
	if (CU::CommandLineManager::GetInstance()->HasParameter("size"))
	{
		winData.myWidth = std::stoi(CU::CommandLineManager::GetInstance()->GetArgument("size", 0));
		winData.myHeight = std::stoi(CU::CommandLineManager::GetInstance()->GetArgument("size", 1));
	}
	CU::Vector2i consolePosition;
	if (CU::CommandLineManager::GetInstance()->HasParameter("consoleposition"))
	{
		consolePosition.x = std::stoi(CU::CommandLineManager::GetInstance()->GetArgument("consoleposition", 0));
		consolePosition.y = std::stoi(CU::CommandLineManager::GetInstance()->GetArgument("consoleposition", 1));
	}
	const bool forceVR = CU::CommandLineManager::GetInstance()->HasParameter("vr");

	Engine engine;
	engine.OpenConsole(consolePosition, CU::Vector2i(960, 540), winData.myTitle + " Debug Console");
	bool run = engine.Init(winData, forceVR);
	if (!run)
	{
		system("pause");
		engine.CloseConsole();
		Threadpool::DestroyInstance();
		CU::FileWatcher::DestroyInstance();
		CU::CommandLineManager::DestroyInstance();
		return;
	}

	run = PhysicsInterface::Init() && (PhysicsInterface::CreateScene(CU::Vector3f(0.0f, -9.81f, 0.0f), true) != -1);
	if (!run)
	{
		system("pause");
		engine.CloseConsole();
		PhysicsInterface::Destroy();
		Threadpool::DestroyInstance();
		CU::FileWatcher::DestroyInstance();
		CU::CommandLineManager::DestroyInstance();
		return;
	}

	CU::StateStack stateStack;
	Editor editor(&stateStack);
	Game game(&stateStack);
	game.Init(runServer);
	if (runEditor)
	{
		editor.Init(game.GetComponentManager());
		editor.Start();
	}
	else
	{
		if (CU::CommandLineManager::GetInstance()->HasParameter("state"))
		{
			game.Start(CU::CommandLineManager::GetInstance()->GetArgument("state", 0));
		}
		else
		{
			game.Start();
		}
	}
	
	#ifdef TIME_DURATIONS
	std::chrono::high_resolution_clock::time_point timerStart;
	std::chrono::duration<float, std::milli> timeSpan;
	#endif

	#ifdef _DEBUG
	TextInstance fpsCounter;
	int framerate = 0;
	int framecounter = 0;
	float tickTimer = 0.0f;
	fpsCounter.SetPosition(CU::Vector2f(SpriteUnits::GetBorderLeft(), SpriteUnits::GetBorderTop()));
	fpsCounter.SetPivot(CU::Vector2f::Zero);
	#endif

	MSG windowMessage = { 0 };
	while (run)
	{
		while (PeekMessage(&windowMessage, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);

			if (windowMessage.message == WM_QUIT)
			{
				run = false;
			}
		}

		timeKeeper.Update();
		engine.BeginFrame();

		#ifdef USE_THREADS
		Threadpool::GetInstance()->AddTask(std::bind(&Engine::Render, &engine));
		Threadpool::GetInstance()->AddTask(std::bind(&PhysicsInterface::Update, timeKeeper.GetDeltaTime()));
		#else
		engine.Render();
		PhysicsInterface::Update(timeKeeper.GetDeltaTime());
		#endif

		#ifdef _DEBUG
		engine.Draw(&fpsCounter);
		#endif

		#ifdef TIME_DURATIONS
		timerStart = std::chrono::high_resolution_clock::now();
		#endif

		if (!stateStack.Update(timeKeeper.GetDeltaTime()))
		{
			run = false;
		}
		stateStack.Draw();
		engine.Update();

		#ifdef TIME_DURATIONS
		timeSpan = (std::chrono::high_resolution_clock::now() - timerStart) / 1000;
		#endif

		#ifdef USE_THREADS
		Threadpool::GetInstance()->Sync();
		#endif

		engine.EndFrame();
		CU::FileWatcher::GetInstance()->Update();
		CU::Input::Get()->UpdateReset();

		#ifdef _DEBUG
		tickTimer += timeKeeper.GetDeltaTime();
		framecounter++;
		if (tickTimer >= 1.0f)
		{
			tickTimer -= 1.0f;
			framerate = framecounter;
			framecounter = 0;
			#ifdef TIME_DURATIONS
			//timerStart = std::chrono::high_resolution_clock::now();
			fpsCounter.Init("FPS:" + std::to_string(framerate) +
				"\nRender:" + std::to_string(engine.GetLastRenderDuration()) +
				"\nCulling:" + std::to_string(engine.GetLastCullingDuration()) +
				"\nUpdate:" + std::to_string(timeSpan.count()) +
				"\nPhysics:" + std::to_string(PhysicsInterface::GetLastSimulationDuration()), "font");
			//timeSpan = (std::chrono::high_resolution_clock::now() - timerStart) / 1000;
			#else
			fpsCounter.Init("FPS:" + std::to_string(framerate), "font");
			#endif
		}
		#endif
	}

	engine.CloseConsole();
	PhysicsInterface::Destroy();
	Threadpool::DestroyInstance();
	CU::FileWatcher::DestroyInstance();
	CU::CommandLineManager::DestroyInstance();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	hInstance; hPrevInstance; lpCmdLine; nShowCmd;

	#ifdef USE_DUMP
	__try
	{
		Run();
	}
	__except (ExceptionFilterFunction(GetExceptionInformation()))
	{
		//MessageBox(engine.GetWindow(), "Game crashed, MiniDump.dmp should be created.", "Crash", MB_OK);
	}
	#else
	Run();
	#endif

	return 0;
}