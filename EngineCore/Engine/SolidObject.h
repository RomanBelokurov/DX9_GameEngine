#pragma once

#ifndef _SOLIDOBJECT_H_
#define _SOLIDOBJECT_H_

#include	<d3dx9math.h>

class SolidObject {
	public:

		void SetPosition(float, float, float);
		void SetRotation(float, float, float);
		void SetScale(float, float, float);

	private:
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Scalling;
		D3DXVECTOR3 Rotation;
};

#endif