//////////////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#include "SystemWindow.h"
#include <landan/util/DebugUtil.h>

//////////////////////////////////////////////////////////////////////
// NAMESPACE /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

namespace landan {

	//////////////////////////////////////////////////////////////////////
	// STATICS ///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

#ifdef _WIN32

	const tstring SystemWindow::WINDOW_CLASS_NAME = LT("LANDAN_WINDOW_CLASS");


	//Every Windows Message will hit this function. 
	LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
	{

		//Hold our target window instance
		SystemWindow *targetWindow = NULL;

		//If it's the WM_NCCREATE message (which should be the first message we get...)
		if(msg == WM_NCCREATE) {
			//Pull the target window out of the lpCreateParams which is the this pointer we pass into CreateWindowEx
			targetWindow = reinterpret_cast<SystemWindow*>((LONG)((LPCREATESTRUCT)lparam)->lpCreateParams);
			//Set the pointer to this instance in the GWLP_USERDATA so we can pull it out reliably in the future
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)targetWindow);
		}
		else {
			//Pull the window instance out of the GWLP_USERDATA
			targetWindow = reinterpret_cast<SystemWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		//If we still don't have a window we can't respond to any events so kick it to the default.
		if(targetWindow == NULL) {
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		//Otherwise we're all good and we can pipe it to the instances version of the WndProc
		return targetWindow->LocalWndProc(hwnd, msg, wparam, lparam);
	}
#endif


	//////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	SystemWindow::SystemWindow(tstring title, u32 x, u32 y, u32 width, u32 height, window::WINDOW_TYPE type)
	:m_title(title), m_x(x), m_y(y), m_width(width), m_height(height), m_type(type), m_state(window::NORMAL)
	{

	}

	//////////////////////////////////////////////////////////////////////
	// DESTRUCTOR ////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	SystemWindow::~SystemWindow() 
	{

	}

	//////////////////////////////////////////////////////////////////////
	// INITIALIZE ////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	bool SystemWindow::Init()
	{
#ifdef _WIN32

		//Get the HINSTANCE - This is how Cinder does it
		m_hinstance = GetModuleHandle(NULL);

		//Create the Definition
		//TODO: Potentially pull some of these in via external config file
		WNDCLASSEX definition;
		ZeroMemory(&definition, sizeof(WNDCLASSEX));
		definition.cbSize = sizeof(WNDCLASSEX);
		definition.cbClsExtra = 0;
		definition.cbWndExtra = 0;
		definition.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //TODO: If we're using DirectX or OpenGL to render, we don't need a background
		definition.hCursor = LoadCursor(NULL, IDC_ARROW);
		definition.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		definition.hIconSm = definition.hIcon;
		definition.hInstance = m_hinstance;
		definition.lpfnWndProc = GlobalWndProc;
		definition.lpszClassName = WINDOW_CLASS_NAME.c_str(); //TODO: Cinder uses a different class name depending if Fullscreen or not. Do we care?
		definition.lpszMenuName = NULL;
		definition.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		//Register the class
		if (!RegisterClassEx(&definition))
		{
			LOG_ERROR("Register Class Failed!");
			return false;
		}


		//Set the initial window size and position
		RECT rect;
		rect.left = m_x;
		rect.top = m_y;
		rect.right = m_x + m_width;
		rect.bottom = m_y + m_height;

#endif

		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// DESTROY ///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////
	// BODY //////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////
	// EVENT HANDLERS ////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	void SystemWindow::OnResize(u32 width, u32 height, window::WINDOW_RESIZE_STATE)
	{
		m_width = width;
		m_height = height;
		//TODO: Dispatch Event for window Resize
	}

	void SystemWindow::OnMove(u32 x, u32 y)
	{
		m_x = x;
		m_y = y;
		//TODO: Dispatch Event for window Move
	}

	void SystemWindow::OnClose()
	{

	}

	void SystemWindow::OnDestroy()
	{

	}


	//////////////////////////////////////////////////////////////////////
	// GETTERS/SETTERS ///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////
	// WINDOWS SPECIFIC //////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

#ifdef _WIN32

	LRESULT CALLBACK SystemWindow::LocalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		
		switch(msg)
		{
		case WM_SIZE:
			OnResize(static_cast<u32>(LOWORD(lparam)), static_cast<u32>(HIWORD(lparam)), static_cast<window::WINDOW_RESIZE_STATE>(wparam));
			break;
		case WM_MOVE:
			OnMove(static_cast<u32>(LOWORD(lparam)), static_cast<u32>(HIWORD(lparam)));
			break;
		case WM_CLOSE:
			OnClose();
			break;
		case WM_DESTROY:
			OnDestroy();
			break;

		default:
			//We don't handle the message so let the Default Window Proc handle it
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}


#endif

}