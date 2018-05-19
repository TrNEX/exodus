#pragma once

namespace SDK
{
	class CRenderView
	{
	public:
		void SetBlend(float blend)
		{
			typedef void(__thiscall* Fn)(void*, float);
			VMT::VMTHookManager::GetFunction<Fn>(this, 4)(this, blend);
		}
		float GetBlend()
		{
			typedef float(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 5)(this);
		}
		void SetColorModulation(CColor color)
		{
			float blend[3] = { color.RGBA[0], color.RGBA[1], color.RGBA[2] };

			typedef void(__thiscall* Fn)(void*, float*);
			VMT::VMTHookManager::GetFunction<Fn>(this, 6)(this, blend);
		}
		void GetColorModulation(float* blend)
		{
			typedef void(__thiscall* Fn)(void*, float*);
			VMT::VMTHookManager::GetFunction<Fn>(this, 7)(this, blend);
		}
	};
}