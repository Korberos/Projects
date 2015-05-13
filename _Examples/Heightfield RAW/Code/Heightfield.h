#include <windows.h>

class Vert
{
public:
	float x;
	float y;
	float z;
};


class SwiftHeightField {
private:
	int hmHeight;
	int hmWidth;

	int				m_nVertexCount;
	Vert*			m_vVertices;

	unsigned int	m_nVBOVertices;
public:

	bool Init(void);
	bool Create(char *hFileName, const int hWidth, const int hHeight);
	void Render(void);

	BYTE hHeightField[1024][1024];

	int		m_nLOD;
};