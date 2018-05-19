#pragma once


namespace UTILS
{
	namespace INPUT
	{
		LRESULT CALLBACK WndProcCallback(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

		struct MouseInfo
		{
			Vector2D position;

			int scroll;
			int left;
			int right;
		};
		class InputHandler
		{
		public:
			void Init();
			void Update();

			MouseInfo GetMouseInfo();
			void GetKeyboardState(int*);
			int GetKeyState(int);

			int GetKeyStateEx(int key);

			void Callback(UINT, WPARAM, LPARAM);

		private:
			MouseInfo cached_mouse_info;
			int cached_pressed_keys[255];
			int cached_pressed_keys_ex[255]; // these are only reset when you call GetKeyStateEx()

			Vector2D GetMousePosition();
		}; extern InputHandler input_handler;
	}
}