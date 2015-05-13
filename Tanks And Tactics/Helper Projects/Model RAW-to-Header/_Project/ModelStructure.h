#include <vector>
#include <math.h>

struct Model
{
	struct Model_Part
	{
	public:
		struct Model_Vertex
		{
			float Position[3];
		};

		struct Model_Triangle
		{
			unsigned int VertexID[3];
		};

		char						Name[32];
		std::vector<Model_Vertex>	VertexList;
		std::vector<Model_Triangle>	TriangleList;

		void			AddTriangle(float* T);
		unsigned int	AddVertex(float* V);
	};

	Model_Part* AddPart(void);
	std::vector<Model_Part> PartList;
};

Model::Model_Part* Model::AddPart(void)
{
	Model_Part	Part;
	PartList.push_back(Part);
	return &PartList[PartList.size() - 1];
}

void Model::Model_Part::AddTriangle(float* T)
{
	Model_Part::Model_Triangle Triangle;
	Triangle.VertexID[0] = AddVertex(&T[0]);
	Triangle.VertexID[1] = AddVertex(&T[3]);
	Triangle.VertexID[2] = AddVertex(&T[6]);
	TriangleList.push_back(Triangle);
}

unsigned int Model::Model_Part::AddVertex(float* V)
{
	for (unsigned int i = 0; i < VertexList.size(); i++)
	{
		if (fabs(VertexList[i].Position[0] - V[0]) < 0.001f)
			if (fabs(VertexList[i].Position[1] - V[1]) < 0.001f)
				if (fabs(VertexList[i].Position[2] - V[2]) < 0.001f)
					return i;
	}

	Model_Part::Model_Vertex Vertex;
	Vertex.Position[0] = V[0];
	Vertex.Position[1] = V[1];
	Vertex.Position[2] = V[2];
	VertexList.push_back(Vertex);

	return (unsigned int)(VertexList.size() - 1);
}