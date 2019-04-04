#include "ChangeSceneOnClick.hpp"

#include "kitten/K_Instance.h"

#include <iostream>


ChangeSceneOnClick::ChangeSceneOnClick(const std::string& p_sceneTarget) : m_targetScene(p_sceneTarget) 
{
}

ChangeSceneOnClick::~ChangeSceneOnClick() 
{

}

void ChangeSceneOnClick::onClick()
{
	std::cout << "change scene" << std::endl;
	kitten::K_Instance::changeScene(m_targetScene);
}