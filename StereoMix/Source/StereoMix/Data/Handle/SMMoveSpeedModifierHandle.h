#pragma once

struct FSMMoveSpeedModifierHandle
{
	FSMMoveSpeedModifierHandle()
		: ID(0) {}

	FSMMoveSpeedModifierHandle(int32 NewID)
		: ID(NewID) {}

	bool IsValid() const { return ID != 0; }

	void Invalidate() { ID = 0; }

	int32 ID;
};
