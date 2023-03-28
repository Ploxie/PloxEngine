//
// Created by Ploxie on 2023-03-22.
//

#pragma once
#include "eastl/bitset.h"
#include "glm/vec2.hpp"

enum class InputAction;

class Mouse
{
public:
	enum class Button
	{
		BUTTON_LEFT = 0,
		BUTTON_RIGHT = 1,
		BUTTON_MIDDLE = 2,
		BUTTON_4 = 3,
		BUTTON_5 = 5
	};

	explicit Mouse() = default;

	void Update();

	bool IsButtonDown(Button button) const;
	glm::vec2 GetPosition() const;
	glm::vec2 GetDeltaPosition() const;
	glm::vec2 GetScrollOffset() const;

	void SetMousePosition(float x, float y);

public: // Internal
	void OnMove(double x, double y);
	void OnScroll(double xOffset, double yOffset);
	void OnButton(Button button, InputAction action);

private:
	glm::vec2 m_scrollOffset;
	glm::vec2 m_position;
	glm::vec2 m_previousPosition;
	glm::vec2 m_deltaPosition;
	eastl::bitset<8> m_buttons;
};
