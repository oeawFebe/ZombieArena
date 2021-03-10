#include "TextureHolder.h"
#include <assert.h>
TextureHolder* TextureHolder::m_s_Instance = nullptr;
TextureHolder::TextureHolder()
{
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;

}
Texture& TextureHolder::GetTexture(string const& filename)
{
	auto& m = m_s_Instance->m_Textures;// map<string,Texture>
	auto keyValuePair = m.find(filename);
	if(keyValuePair!=m.end())
	{//found it
		return keyValuePair->second;//Texture is the value (second)
	}
	else
	{
		auto& texture = m[filename];//not assignment for kvp ???
		texture.loadFromFile(filename);
		return texture;
	}

}