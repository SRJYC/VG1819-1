#include "K_Instance.h"

#include <thread>
#include <iostream>


#include <kibble/kibble.hpp>
#include <kitten/mouse picking/ActiveClickables.h>
#include <puppy/Renderer.h>

namespace kitten
{
	K_Instance* K_Instance::sm_instance = nullptr;

	K_Instance::K_Instance() : m_inputManager(input::InputManager::getInstance()), m_cameraList(K_CameraList::getInstance()),
		m_componentManager(K_ComponentManager::getInstance()), m_gameObjectManager(K_GameObjectManager::getInstance()),
		m_time(K_Time::getInstance()), m_clickables(ActiveClickables::getInstance()), m_eventManager(EventManager::getInstance())
	{
		
	}

	K_Instance::~K_Instance()
	{
		
	}

	void K_Instance::createInstance()
	{
		assert(sm_instance == nullptr);

		//K_JobManager::createInstance();
		input::InputManager::createInstance();
		K_CameraList::createInstance();
		K_ComponentManager::createInstance();
		K_GameObjectManager::createInstance();
		K_Time::createInstance();
		ActiveClickables::createInstance();
		EventManager::createInstance();
		AudioEngineWrapper::createInstance();	
		//AsyncFileOperations::startService();

		sm_instance = new K_Instance();
	}

	void K_Instance::destroyInstance()
	{
		assert(sm_instance != nullptr);

		input::InputManager::destroyInstance();
		K_CameraList::destroyInstance();
		K_ComponentManager::destroyInstance();
		K_GameObjectManager::destroyInstance();
		K_Time::destroyInstance();
		ActiveClickables::destroyInstance();
		EventManager::destroyInstance();
		AudioEngineWrapper::destroyInstance();
		//AsyncFileOperations::stopService();


		//K_JobManager::destroyInstance();
		

		delete sm_instance;
	}

	void K_Instance::update()
	{
		sm_instance->privateUpdate();
	}

	void K_Instance::privateUpdate()
	{
		if (m_sceneSwitchFlag) 
		{
			m_gameObjectManager->destroySceneGameObjects();
			kibble::setSceneFrom(m_nextScene);

			m_sceneSwitchFlag = false;
		}

		m_time->updateTime();

		//m_jobManager->update();
		AudioEngineWrapper::update();
		m_inputManager->update();
		m_eventManager->update();

		m_componentManager->updateComponents();
		m_gameObjectManager->deleteQueuedObjects();
	}

	void K_Instance::changeScene(const std::string& p_nextScene)
	{
		assert(sm_instance != nullptr);
		sm_instance->m_sceneSwitchFlag = true;
		sm_instance->m_nextScene = p_nextScene;

		kitten::Event* eventData = new kitten::Event(kitten::Event::Scene_Change);
		eventData->putString(NEXT_SCENE_PATH_KEY, p_nextScene);
		kitten::EventManager::getInstance()->triggerEvent(kitten::Event::Scene_Change, eventData);

		std::cout << "change scene event sent" << std::endl;
	}
}