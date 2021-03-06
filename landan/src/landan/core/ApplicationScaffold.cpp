/*
Simplified BSD License
======================

Copyright(c) 2012, Karman Interactive Ltd. 
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.

THIS SOFTWARE IS PROVIDED BY KARMAN INTERACTIVE LTD "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL KARMAN INTERACTIVE LTD OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Karman Interactive Ltd.
*/

//////////////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#include "ApplicationScaffold.h"

#ifdef _WIN32
	#include <Windows.h>
#endif

#include <landan/application/IApplication.h>
#include <landan/application/BasicApplication.h>
#include <landan/application/WindowedApplication.h>
#include <landan/application/config/ApplicationConfig.h>
#include <landan/timer/Timer.h>

//////////////////////////////////////////////////////////////////////
// NAMESPACE /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

namespace landan {

	//////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	ApplicationScaffold::ApplicationScaffold(IApplication *app)
	:p_app(app)
	{
		
	}

	//////////////////////////////////////////////////////////////////////
	// DESTRUCTOR ////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	ApplicationScaffold::~ApplicationScaffold() 
	{
		if (p_appConfig != 0)
		{
			delete p_appConfig;
			p_appConfig = 0;
		}

		if (p_quitFlag != 0)
		{
			delete p_quitFlag;
			p_quitFlag = 0;
		}
		p_app = 0;
	}

	void ApplicationScaffold::Init()
	{
		//Assign a Quit Flag into the Application
		p_quitFlag = new u8();
		*p_quitFlag = 1;
		p_app->ApplyQuitFlag(p_quitFlag);

		//Create a new instance of an Application Config
		p_appConfig = new ApplicationConfig();

		//Initialize the Timer statically so we know how fast the system is.
		Timer::Init();
	}

	//////////////////////////////////////////////////////////////////////
	// BASIC APPLICATION /////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	void ApplicationScaffold::PrepBasic()
	{

		//Ensure minimum defaults are set in case the app doesn't override any settings
		p_appConfig->SetApplicationType(application::BASIC);
		p_appConfig->SetRenderType(application::NONE);
		p_appConfig->SetUpdateType(application::RUN_ONCE);

		//Allow the App to override any setting
		p_app->ApplyConfig(p_appConfig);

		//Assertions that they didn't break anything
		if (p_appConfig->GetApplicationType() != application::BASIC)
		{
			LOG_ERROR("Basic Applications must have their application type set to BASIC.");
		}
		if (p_appConfig->GetRenderType() != application::NONE)
		{
			LOG_ERROR("Basic Applications must have their render type set to NONE.");
		}


		//Initialize the App
		p_app->Init();

	}

	void ApplicationScaffold::RunBasic()
	{
		//Get the Update Loop Type
		application::UPDATE_TYPE updateType = p_appConfig->GetUpdateType();

		//Case 01: The program runs once and exits normally
		if (updateType == application::RUN_ONCE)
		{
			//If we're only running once, no need to calculate anything.
			p_app->Update(0.0f);
		}
		//Case 02: The program will run continuously until the application decides to quit and will run at a specified framerate.
		else if (updateType == application::FRAMERATE_LIMITED)
		{
			//Get the target framerate and convert to milliseconds per frame
			f32 targetFrameRate = p_appConfig->GetFrameRate();
			f32 targetMSPerFrame = (1.0f/targetFrameRate)*1000.0f;
			//Store the first timestamp, subtract the target MS per frame so we Update immediately and don't wait for one frame
			m_lastTime = Timer::GetMilliSeconds() - targetMSPerFrame;

			while(*p_quitFlag == 1)
			{
				m_currentTime = Timer::GetMilliSeconds();
				m_deltaTime = static_cast<f32>(m_currentTime - m_lastTime);

				//Clamp to >0
				m_deltaTime = (m_deltaTime > 0.0f) ? m_deltaTime : 0.0f;

				if (m_deltaTime < targetMSPerFrame)
				{
					Sleep(0);
				}
				else {
					p_app->Update(m_deltaTime);

					m_lastTime = m_currentTime;
				}
			}
		}
		//Case 03: The program will run continuously until the application decides to quit and will run as fast as possible.
		else if (updateType == application::FRAMERATE_UNLIMITED)
		{
			m_lastTime = Timer::GetMilliSeconds();

			while(*p_quitFlag == 1)
			{
				m_currentTime = Timer::GetMilliSeconds();
				m_deltaTime = static_cast<f32>(m_currentTime - m_lastTime);

				//Clamp to >0
				m_deltaTime = (m_deltaTime > 0.0f) ? m_deltaTime : 0.0f;

				p_app->Update(m_deltaTime);
				m_lastTime = m_currentTime;
			}
		}
		//Unknown Case - Should never happen
		else {
			LOG_ERROR("Update Type is not a known type. Currently set to " << updateType);
		}
	}

	void ApplicationScaffold::StopBasic()
	{
		p_app->Destroy();
	}



	//////////////////////////////////////////////////////////////////////
	// WINDOWED APPLICATION //////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	void ApplicationScaffold::PrepWindowed()
	{
		//Ensure minimum defaults are set in case the app doesn't override any settings
		p_appConfig->SetApplicationType(application::WINDOWED);
		p_appConfig->SetRenderType(application::HARDWARE_DIRECT_X_11);
		p_appConfig->SetUpdateType(application::FRAMERATE_LIMITED);

		//Allow the App to override any setting
		p_app->ApplyConfig(p_appConfig);

		//Assertions that they didn't break anything
		if (p_appConfig->GetApplicationType() != application::WINDOWED)
		{
			LOG_ERROR("Windowed Applications must have their application type set to WINDOWED.");
		}

		//Initialize the App
		p_app->Init();
	}

	void ApplicationScaffold::RunWindowed()
	{

		WindowedApplication *p_windowedApp = static_cast<WindowedApplication*>(p_app);

		//Get the Update Loop Type
		application::UPDATE_TYPE updateType = p_appConfig->GetUpdateType();

		//Case 01: The program runs once and exits normally
		if (updateType == application::RUN_ONCE)
		{
			//If we're only running once, no need to calculate anything.
			p_windowedApp->Update(0.0f);
			p_windowedApp->Render();
		}
		//Case 02: The program will run continuously until the application decides to quit and will run at a specified framerate.
		else if (updateType == application::FRAMERATE_LIMITED)
		{
			//Get the target framerate and convert to milliseconds per frame
			f32 targetFrameRate = p_appConfig->GetFrameRate();
			f32 targetMSPerFrame = (1.0f/targetFrameRate)*1000.0f;
			//Store the first timestamp, subtract the target MS per frame so we Update immediately and don't wait for one frame
			m_lastTime = Timer::GetMilliSeconds() - targetMSPerFrame;

			while(*p_quitFlag == 1)
			{
				m_currentTime = Timer::GetMilliSeconds();
				m_deltaTime = static_cast<f32>(m_currentTime - m_lastTime);

				//Clamp to >0
				m_deltaTime = (m_deltaTime > 0.0f) ? m_deltaTime : 0.0f;

				if (m_deltaTime < targetMSPerFrame)
				{
					Sleep(0);
				}
				else {
					p_app->Update(m_deltaTime);
					p_windowedApp->Render();

					m_lastTime = m_currentTime;
				}
			}
		}
		//Case 03: The program will run continuously until the application decides to quit and will run as fast as possible.
		else if (updateType == application::FRAMERATE_UNLIMITED)
		{
			m_lastTime = Timer::GetMilliSeconds();

			while(*p_quitFlag == 1)
			{
				m_currentTime = Timer::GetMilliSeconds();
				m_deltaTime = static_cast<f32>(m_currentTime - m_lastTime);

				//Clamp to >0
				m_deltaTime = (m_deltaTime > 0.0f) ? m_deltaTime : 0.0f;

				p_app->Update(m_deltaTime);
				p_windowedApp->Render();

				m_lastTime = m_currentTime;
			}
		}
		//Unknown Case - Should never happen
		else {
			LOG_ERROR("Update Type is not a known type. Currently set to " << updateType);
		}
	}

	void ApplicationScaffold::StopWindowed()
	{
		p_app->Destroy();
	}

}