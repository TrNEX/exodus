#pragma once

class CChams
{
public:
	SDK::IMaterial *CreateMaterial(bool ignorez, bool lit, bool wireframe);
private:
	void InitKeyValues(SDK::KeyValues* keyValues, std::string name);
	void LoadFromBuffer(SDK::KeyValues* keyValues, std::string name_, std::string buffer_);
};

extern CChams* chams;