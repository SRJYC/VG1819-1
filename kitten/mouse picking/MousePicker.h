#pragma once

#include <tuple>
#include "Ray.h"
#include "ClickableBox.h"
#include "ClickableFrame.h"

namespace MousePicker
{
	bool rayHits(const kitten::Ray& p_ray, const kitten::ClickableBox* p_clickable, double* p_hitOutput);	
	bool uiHit(const kitten::ClickableFrame* p_clickable, float p_mouseX, float p_mouseY);
	kitten::ClickableBox* getClosestHit(const kitten::Ray& p_ray);
	kitten::ClickableFrame* getClosestHitFrame(int mouseX, int mouseY);
}