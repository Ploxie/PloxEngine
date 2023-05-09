//
// Created by Ploxie on 2023-03-22.
//

#pragma once
#include "eastl/bitset.h"
#include "glm/vec2.hpp"
#include "Keyboard.h"
#include "Mouse.h"

enum class InputAction
{
	RELEASE = 0,
	PRESS = 1,
	REPEAT = 2
};

class Input
{
public:
	static bool IsMouseButtonDown(MouseButton button);
	static glm::vec2 GetMousePosition();
	static glm::vec2 GetMouseDeltaPosition();
	static glm::vec2 GetScrollOffset();

	static bool IsKeyDown(Key key, bool ignoreRepeated = false);
};
