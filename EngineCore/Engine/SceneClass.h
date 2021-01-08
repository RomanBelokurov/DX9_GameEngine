#pragma once

#ifndef _SCENE_CLASS_H_
#define _SCENE_CLASS_H_

#include <iostream>
#include "ResourceManager.h"

class SceneClass 
{
	public:
		SceneClass();

		void LoadSceneFromFile(std::string);
		void Update();
		void Render();

	private:
		ResourceManager m_resourceManager;
};

void SceneClass::LoadSceneFromFile(std::string filename)
{

}


#endif