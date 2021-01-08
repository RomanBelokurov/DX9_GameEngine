#pragma once

#ifndef __TEXT_CLASS_H__
#define __TEXT_CLASS_H__

#include "d3dx9core.h"
#include <string>

class TextClass
{
	ID3DXFont* d3d9Font;

public:

	TextClass();
	~TextClass();

	bool Initialize(IDirect3DDevice9* m_device, const char* font, int fsize, int style);
	void Render(std::string textstring, int x, int y, D3DXCOLOR color);
};

TextClass::TextClass()
{
	d3d9Font = 0;
}

TextClass::~TextClass()
{
	d3d9Font->Release();
	d3d9Font = 0;
}

bool TextClass::Initialize(IDirect3DDevice9* m_device, const char* font, int fsize, int style)
{
	if (FAILED(D3DXCreateFont(m_device, fsize, 0, style, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, font, &d3d9Font)))
		return false;
	return true;
}

void TextClass::Render(std::string textstring, int x, int y, D3DXCOLOR color)
{
	RECT destRect1;
	SetRect(&destRect1, x, y, 0, 0);

	d3d9Font->DrawTextA(NULL, textstring.c_str(), -1, &destRect1, DT_NOCLIP, color);		
}




#endif