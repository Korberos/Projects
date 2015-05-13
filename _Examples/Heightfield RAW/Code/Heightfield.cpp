#include <stdio.h>
#include "./Utilities/OpenGL/OGL_Includes.h"

#include "heightfield.h"

bool SwiftHeightField::Init(void)
{
	glGenBuffersARB(1, &m_nVBOVertices);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOVertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_nVertexCount * 3 * sizeof(float), m_vVertices, GL_STATIC_DRAW_ARB);

	delete [] m_vVertices;
	m_vVertices = NULL;

	return true;
}

bool SwiftHeightField::Create(char *hFileName, const int hWidth, const int hHeight)
{	
	hmHeight = hHeight;
	hmWidth = hWidth;
	
	FILE *fp;

	fp = fopen(hFileName, "rb");

	fread(hHeightField, 1, hWidth * hHeight, fp);

	fclose(fp);

	m_nVertexCount = (int)(hmHeight * hmWidth * 6) / (m_nLOD * m_nLOD);
	m_vVertices = new Vert[m_nVertexCount];

	int nIndex = 0;
	float flX;
	float flZ;

	for (int hMapX = 0; hMapX < hmWidth; hMapX+=m_nLOD)
	{
		for (int hMapZ = 0; hMapZ < hmHeight; hMapZ+=m_nLOD)
		{
			for (int nTri = 0; nTri < 6; nTri++)
			{
				flX = (float)hMapX + ((nTri == 1 || nTri == 2 || nTri == 5) ? m_nLOD : 0);
				flZ = (float)hMapZ + ((nTri == 2 || nTri == 4 || nTri == 5) ? m_nLOD : 0);

				m_vVertices[nIndex].x = flX / 100.0f;
				m_vVertices[nIndex].y = hHeightField[(int)flX][(int)flZ] / 100.0f;
				m_vVertices[nIndex].z = flZ / 100.0f;
				nIndex++;
			}
		}
	}

	Init();

	return true;
}

void SwiftHeightField::Render(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_nVBOVertices);
	glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);

	glDrawArrays(GL_TRIANGLES, 0, m_nVertexCount);

	glDisableClientState(GL_VERTEX_ARRAY);
}