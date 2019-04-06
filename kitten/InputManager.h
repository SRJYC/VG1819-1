#pragma once
#include "puppy\P_Common.h"
#include "mouse picking\Clickable.h"
#include "mouse picking\ClickableUI.h"
#include "K_GameObject.h"
#include <string>
#include <unordered_set>

namespace kitten
{
	class K_Instance;
}

namespace input
{
	class StringListener
	{
	private:
	public:
		virtual void onStringFinished(const std::string& p_string) =0;
		virtual void onStringChanged(const std::string& p_string) =0;
	};

	class InputManager
	{
		friend class kitten::K_Instance;
	private:
		static InputManager* sm_inputManagerInstance;

		bool m_keysDown[GLFW_KEY_LAST];
		bool m_keysDownLast[GLFW_KEY_LAST];

		bool m_mouseDown[GLFW_MOUSE_BUTTON_LAST];
		bool m_mouseDownLast[GLFW_MOUSE_BUTTON_LAST];

		int m_mouseXChange, m_mouseYChange;
		int m_lastMouseX, m_lastMouseY, m_mouseYOpenGL;

		int m_windowX, m_windowY;

		int m_mouseWheel;

		bool m_shouldResetMouse, m_inputPollMode;
		bool m_inputStringChanged, m_notifyStringFinished;

		std::string m_inputString;
		std::unordered_set<StringListener*> m_stringListeners;

		

		InputManager();
		~InputManager();

		static void createInstance();
		static void destroyInstance();

		static void GLFWCALL keyCallback(int key, int action);
		static void GLFWCALL charCallback(int key, int action);

		void privateKeyCallback(int key, int action);
		void privateCharCallback(int key, int action);

		kitten::K_GameObject*  m_lastMouseHitObject = nullptr, *m_lastMouseHitFrame = nullptr;

		void update();
	public:
		static InputManager* getInstance();

		void addStringListener(StringListener* m_toAdd);
		void removeStringListener(StringListener* m_toRemove);

		void resetMouse(bool p_shouldReset);
		void setPollMode(bool p_enabled);

		bool keyDown(int p_key);
		bool keyDownLast(int p_key);

		bool mouseDown(int p_button);
		bool mouseDownLast(int p_button);

		int getMouseXChange();
		int getMouseYChange();

		int getMouseXPos();
		int getMouseYWindowPos();
		int getMouseYOpenGLPos();

		int getWindowWidth();
		int getWindowHeight();

		int getMouseWheel();

		kitten::K_GameObject* getMouseLastHitObject();
		kitten::K_GameObject* getMouseLastHitFrame();
	};
}