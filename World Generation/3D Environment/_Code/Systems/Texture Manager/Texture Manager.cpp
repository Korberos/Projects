#include "Texture Manager.h"

#pragma warning(disable : 4996)

TextureManager* TextureManager::GetInstance(void)
{
	static TextureManager INSTANCE;
	return &INSTANCE;
}

TextureManager::TextureManager(void)
{
	m_nCurrentTexture = 0;
	m_vTextures.clear();
}

TextureManager::~TextureManager(void)
{
	Shutdown();
}

int TextureManager::LoadTexture(char* Filename)
{
	if (Filename == 0) return -1;

	// Check to see if the file has already been loaded
	for (unsigned int i = 0; i < m_vTextures.size(); i += 1)
	{
		if (strcmp(Filename, m_vTextures[i].Filename) == 0)
		{
			m_vTextures[i].References += 1;
			return i;
		}
	}

	char* Extension = &Filename[strlen(Filename) - 3];

	if (strcmp(Extension, "png") == 0)
	{
		Texture NewTexture;
		NewTexture.Identifier = m_nCurrentTexture;
		sprintf(NewTexture.Filename, "%s", Filename);

		TextureInfo TexInfo;
		if (LoadAndBindPNG(Filename, &NewTexture.Identifier, &TexInfo) == false) return -1;

		m_nCurrentTexture	+= 1;
		NewTexture.Width	= TexInfo.Width;
		NewTexture.Height	= TexInfo.Height;
		NewTexture.References	= 1;
		m_vTextures.push_back(NewTexture);
		return NewTexture.Identifier;
	}

	return -1;
}

void TextureManager::ReleaseTexture(unsigned int nTextureID)
{
	if (nTextureID < 0) return;
	if (nTextureID >= m_vTextures.size()) return;

	m_vTextures[nTextureID].References -= 1;
	if (m_vTextures[nTextureID].References <= 0)
	{
		m_vTextures[nTextureID].Filename[0]	= 0;
		m_vTextures[nTextureID].References	= 0;

		if (strcmp(&m_vTextures[nTextureID].Filename[strlen(m_vTextures[nTextureID].Filename) - 3], "bmp") == 0)
			glDeleteTextures(1, &nTextureID);
		
		vector<Texture>::iterator I = m_vTextures.begin();
		for (unsigned int i = 0; i < nTextureID; i++) I++;
		Texture* DeleteTexture = &(*I);
		m_vTextures.erase(I);
		delete DeleteTexture;
	}
}

bool TextureManager::DrawTexture(int ID, float X, float Y, float X_Scale, float Y_Scale)
{
	if (ID < 0) return false;
	if (ID >= (int)m_vTextures.size()) return false;

	Texture T = m_vTextures[ID];

	// NOTE: These three lines are in due to the REVERSE RATIO bug. Ask Richard Wright about this.
	float W	= T.Width * X_Scale;
	float H	= T.Height * Y_Scale * REVERSE_RATIO;
	Y *= REVERSE_RATIO;

	glPushMatrix();
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)ID);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);		glVertex2f(X,	Y);
			glTexCoord2f(1.0f, 0.0f);		glVertex2f(X+W,	Y);
			glTexCoord2f(1.0f, 1.0f);		glVertex2f(X+W,	Y+H);
			glTexCoord2f(0.0f, 1.0f);		glVertex2f(X,	Y+H);
		glEnd();
	}
	glPopMatrix();

	return true;
}

void TextureManager::Shutdown(void)
{
	char* Extension;
	for (unsigned int i = 0; i < m_vTextures.size(); i += 1)
	{
		Extension = &m_vTextures[i].Filename[strlen(m_vTextures[i].Filename) - 3];
		if (strcmp(Extension, "bmp") == 0)
			glDeleteTextures(1, &i);
	}
	m_vTextures.clear();
}