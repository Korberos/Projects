#pragma once

#include "SDL2/SDL.h"
#undef main
#pragma comment(lib, "SDL2/SDL2.lib")

#include "SDL2/SDL_opengl.h"
#pragma comment(lib, "opengl32.lib")

#include <unordered_map>
#include <sstream>

class WindowManager
{
private:
	struct WindowLink
	{
		SDL_Window*		m_Window;
		SDL_GLContext	m_Context;
		SDL_Renderer*	m_Renderer;
		int				m_WindowID;
		bool			m_Shown;
		int				m_Width;
		int				m_Height;
		bool			m_MouseFocus;
		bool			m_KeyboardFocus;
		bool			m_Minimized;

		WindowLink(int windowID) : m_WindowID(windowID) {}
		void HandleEvent(SDL_Event& e);
	};

public:
	static WindowManager& GetInstance() { static WindowManager INSTANCE; return INSTANCE; }

	int CreateNewWindow(const char* title = "", int x = SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED, int w = 100, int h = 100, bool shown = true, bool current = false);
	bool DestroyWindow(const int index);
	void HandleEvent(SDL_Event& e);
	void Render();
	void Shutdown();

	SDL_Window* GetWindow(const int index = -1);
	SDL_GLContext GetContext(const int index = -1);
	SDL_Renderer* GetRenderer(const int index = -1);
	int GetWindowID(const int index = -1);
	bool GetWindowShown(const int index = -1);
	int GetWindowWidth(const int index = -1);
	int GetWindowHeight(const int index = -1);

private:

	WindowManager();
	~WindowManager();

	int FirstFreeIndex();
	int FirstUsedIndex();
	void DestroyWindow(std::unordered_map<int, WindowLink*>::iterator& iter, bool erase = true);

	std::unordered_map<int, WindowLink*> m_WindowList;
	int m_CurrentWindow;
};

void WindowManager::WindowLink::HandleEvent(SDL_Event& e)
{
	//If an event was detected for this window
	if (e.type == SDL_WINDOWEVENT && e.window.windowID == m_WindowID)
	{
		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event)
		{
			//Window appeared
		case SDL_WINDOWEVENT_SHOWN:
			m_Shown = true;
			break;

			//Window disappeared
		case SDL_WINDOWEVENT_HIDDEN:
			m_Shown = false;
			break;

			//Get new dimensions and repaint
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			m_Width = e.window.data1;
			m_Height = e.window.data2;
			SDL_RenderPresent(m_Renderer);
			break;

			//Repaint on expose
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(m_Renderer);
			break;

			//Mouse enter
		case SDL_WINDOWEVENT_ENTER:
			m_MouseFocus = true;
			updateCaption = true;
			break;

			//Mouse exit
		case SDL_WINDOWEVENT_LEAVE:
			m_MouseFocus = false;
			updateCaption = true;
			break;

			//Keyboard focus gained
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			m_KeyboardFocus = true;
			updateCaption = true;
			break;

			//Keyboard focus lost
		case SDL_WINDOWEVENT_FOCUS_LOST:
			m_KeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			m_Minimized = true;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			m_Minimized = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			m_Minimized = false;
			break;

			//Hide on close
		case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow(m_Window);
			break;
		}

		//Update window caption with new data
		if (updateCaption)
		{
			std::stringstream caption;
			caption << "SDL Tutorial - ID: " << m_WindowID << " MouseFocus:" << ((m_MouseFocus) ? "On" : "Off") << " KeyboardFocus:" << ((m_KeyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(m_Window, caption.str().c_str());
		}
	}
}

int WindowManager::CreateNewWindow(const char* title, int x, int y, int w, int h, bool shown, bool current)
{
	auto index = FirstFreeIndex();

	//  Create the Window
	SDL_Window* newWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_OPENGL | (shown ? SDL_WINDOW_SHOWN : 0));
	if (newWindow == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return -1;
	}

	//  Create the OpenGL context
	SDL_GLContext newContext = SDL_GL_CreateContext(newWindow);
	if (newContext == nullptr)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(newWindow);
		return -1;
	}

	//  Create the Renderer
	SDL_Renderer* newRenderer = SDL_CreateRenderer(newWindow, -1, SDL_RENDERER_ACCELERATED);
	if (newRenderer == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		SDL_GL_DeleteContext(newContext);
		SDL_DestroyWindow(newWindow);
		return -1;
	}

	//  Create the WindowLink
	m_WindowList[index] = new WindowLink(SDL_GetWindowID(newWindow));
	if ((m_WindowList.find(index) == m_WindowList.end()) || m_WindowList[index] == nullptr)
	{
		printf("A new WindowLink could not be allocated!\n");
		SDL_GL_DeleteContext(newContext);
		SDL_DestroyWindow(newWindow);
		newRenderer = nullptr;
		return -1;
	}

	//  We've successfully formed our new Window and created a WindowLink with it, so set the data
	m_WindowList[index]->m_Window = newWindow;
	m_WindowList[index]->m_Renderer = newRenderer;
	m_WindowList[index]->m_Context = newContext;

	//  Set the current window based on whether we're setting this or not
	if (current | (m_CurrentWindow == -1)) m_CurrentWindow = index;
	SDL_GL_MakeCurrent(m_WindowList[m_CurrentWindow]->m_Window, m_WindowList[m_CurrentWindow]->m_Context);

	//  Return the new window's index
	return index;
}

bool WindowManager::DestroyWindow(const int index)
{
	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return nullptr;

	DestroyWindow(iter);
	return true;
}

void WindowManager::HandleEvent(SDL_Event& e)
{
	for (auto iter = m_WindowList.begin(); iter != m_WindowList.end(); ++iter) (*iter).second->HandleEvent(e);
}

void WindowManager::Render()
{
	for (auto iter = m_WindowList.begin(); iter != m_WindowList.end(); ++iter)
	{
		if (!(*iter).second->m_Minimized)
		{
			//Clear screen
			SDL_SetRenderDrawColor((*iter).second->m_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear((*iter).second->m_Renderer);

			//Update screen
			SDL_RenderPresent((*iter).second->m_Renderer);
		}
	}
}

void WindowManager::Shutdown()
{
	for (auto iter = m_WindowList.begin(); iter != m_WindowList.end(); ++iter) DestroyWindow(iter, false);
	m_WindowList.clear();
}

SDL_Window* WindowManager::GetWindow(int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_Window;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return nullptr;

	return (*iter).second->m_Window;
}

SDL_GLContext WindowManager::GetContext(int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_Context;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return nullptr;

	return (*iter).second->m_Context;
}

SDL_Renderer* WindowManager::GetRenderer(int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_Renderer;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return nullptr;

	return (*iter).second->m_Renderer;
}

int WindowManager::GetWindowID(const int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_WindowID;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return -1;

	return (*iter).second->m_WindowID;
}

bool WindowManager::GetWindowShown(const int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_Shown;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return false;

	return (*iter).second->m_Shown;
}

int WindowManager::GetWindowWidth(const int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_Width;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return -1;

	return (*iter).second->m_Width;
}

int WindowManager::GetWindowHeight(const int index)
{
	if (index == -1) return m_WindowList[m_CurrentWindow]->m_Height;

	auto iter = m_WindowList.find(index);
	if (iter == m_WindowList.end()) return -1;

	return (*iter).second->m_Height;
}

WindowManager::WindowManager() :
	m_CurrentWindow(-1)
{

}

WindowManager::~WindowManager()
{
	Shutdown();
}

int WindowManager::FirstFreeIndex()
{
	//  Find the first free index
	auto index = 0;
	for (; ; ++index)
	{
		auto iter = m_WindowList.find(index);
		if (iter == m_WindowList.end()) return index;
	}
}

int WindowManager::FirstUsedIndex()
{
	//  Find the first free index
	auto index = 0;
	for (; ; ++index)
	{
		auto iter = m_WindowList.find(index);
		if (iter != m_WindowList.end()) return index;
	}
}

void WindowManager::DestroyWindow(std::unordered_map<int, WindowLink*>::iterator& iter, bool erase)
{
	SDL_GL_DeleteContext((*iter).second->m_Context);

	SDL_DestroyWindow((*iter).second->m_Window);

	(*iter).second->m_Renderer = nullptr;

	if (erase) m_WindowList.erase(iter);
}