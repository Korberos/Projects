#include <fstream>
using namespace std;

#include <iostream>
using namespace std;

void WriteVertex(fstream& F, float X, float Y, float Z);
void WriteGeometry(fstream& F, int Vertex1, int Vertex2, int Vertex3);

void main(void)
{
	// Create an fstream binary file for writing to
	fstream F("MapFile.bin", ios_base::binary|ios_base::out);

	// Write out the amount of points and each each vertex point in order
	int VertexCount = 89;
	F.write((char*)&VertexCount, sizeof(int));

	WriteVertex(F, 0.0f,	0.0f,	0.0f);		// 0
	WriteVertex(F, 0.32f,	0.1f,	2.22f);		// 1
	WriteVertex(F, 0.32f,	0.1f,	1.53f);		// 2
	WriteVertex(F, 0.42f,	0.1f,	1.1f);		// 3
	WriteVertex(F, 0.66f,	0.1f,	0.76f);		// 4
	WriteVertex(F, 1.04f,	0.1f,	0.58f);		// 5
	WriteVertex(F, 2.26f,	0.1f,	0.33f);		// 6
	WriteVertex(F, 3.77f,	0.1f,	0.18f);		// 7
	WriteVertex(F, 5.19f,	0.1f,	0.37f);		// 8
	WriteVertex(F, 7.08f,	0.1f,	0.5f);		// 9
	WriteVertex(F, 8.25f,	0.1f,	0.98f);		// 10
	WriteVertex(F, 9.34f,	0.1f,	2.16f);		// 11
	WriteVertex(F, 9.81f,	0.1f,	2.79f);		// 12
	WriteVertex(F, 9.72f,	0.1f,	4.59f);		// 13
	WriteVertex(F, 9.31f,	0.1f,	5.69f);		// 14
	WriteVertex(F, 8.77f,	0.1f,	6.65f);		// 15
	WriteVertex(F, 8.14f,	0.1f,	7.39f);		// 16
	WriteVertex(F, 7.7f,	0.1f,	7.96f);		// 17
	WriteVertex(F, 6.8f,	0.1f,	8.65f);		// 18
	WriteVertex(F, 5.96f,	0.1f,	9.04f);		// 19
	WriteVertex(F, 4.38f,	0.1f,	9.25f);		// 20
	WriteVertex(F, 3.04f,	0.1f,	9.3f);		// 21
	WriteVertex(F, 2.1f,	0.1f,	8.92f);		// 22
	WriteVertex(F, 1.77f,	0.1f,	8.6f);		// 23
	WriteVertex(F, 1.08f,	0.1f,	7.44f);		// 24
	WriteVertex(F, 0.68f,	0.1f,	5.77f);		// 25
	WriteVertex(F, 0.23f,	0.1f,	3.99f);		// 26
	WriteVertex(F, 3.34f,	0.3f,	1.42f);		// 27
	WriteVertex(F, 3.69f,	0.3f,	1.49f);		// 28
	WriteVertex(F, 4.3f,	0.3f,	1.66f);		// 29
	WriteVertex(F, 4.77f,	0.3f,	2.42f);		// 30
	WriteVertex(F, 4.9f,	0.3f,	3.01f);		// 31
	WriteVertex(F, 4.72f,	0.3f,	3.39f);		// 32
	WriteVertex(F, 4.23f,	0.3f,	3.6f);		// 33
	WriteVertex(F, 3.59f,	0.3f,	3.8f);		// 34
	WriteVertex(F, 2.81f,	0.3f,	3.77f);		// 35
	WriteVertex(F, 2.41f,	0.3f,	3.47f);		// 36
	WriteVertex(F, 2.15f,	0.3f,	3.08f);		// 37
	WriteVertex(F, 2.19f,	0.3f,	2.44f);		// 38
	WriteVertex(F, 2.35f,	0.3f,	1.97f);		// 39
	WriteVertex(F, 2.73f,	0.3f,	1.7f);		// 40
	WriteVertex(F, 3.53f,	0.8f,	1.93f);		// 41
	WriteVertex(F, 3.85f,	0.8f,	1.94f);		// 42
	WriteVertex(F, 4.07f,	0.8f,	2.1f);		// 43
	WriteVertex(F, 4.2f,	0.8f,	2.42f);		// 44
	WriteVertex(F, 4.21f,	0.8f,	2.76f);		// 45
	WriteVertex(F, 3.81f,	0.8f,	3.0f);		// 46
	WriteVertex(F, 3.36f,	0.8f,	3.06f);		// 47
	WriteVertex(F, 3.11f,	0.8f,	2.91f);		// 48
	WriteVertex(F, 2.97f,	0.8f,	2.65f);		// 49
	WriteVertex(F, 2.99f,	0.8f,	2.34f);		// 50
	WriteVertex(F, 3.18f,	0.8f,	2.1f);		// 51
	WriteVertex(F, 3.44f,	1.1f,	2.33f);		// 52
	WriteVertex(F, 3.67f,	1.1f,	2.29f);		// 53
	WriteVertex(F, 3.8f,	1.1f,	2.49f);		// 54
	WriteVertex(F, 3.71f,	1.1f,	2.67f);		// 55
	WriteVertex(F, 3.45f,	1.1f,	2.69f);		// 56
	WriteVertex(F, 3.36f,	1.1f,	2.5f);		// 57
	WriteVertex(F, 8.1f,	0.4f,	4.19f);		// 58
	WriteVertex(F, 8.33f,	0.4f,	4.18f);		// 59
	WriteVertex(F, 8.73f,	0.4f,	4.33f);		// 60
	WriteVertex(F, 8.51f,	0.4f,	5.24f);		// 61
	WriteVertex(F, 7.39f,	0.4f,	6.13f);		// 62
	WriteVertex(F, 5.77f,	0.4f,	6.85f);		// 63
	WriteVertex(F, 5.05f,	0.4f,	7.49f);		// 64
	WriteVertex(F, 4.69f,	0.4f,	7.74f);		// 65
	WriteVertex(F, 3.68f,	0.4f,	7.65f);		// 66
	WriteVertex(F, 3.0f,	0.4f,	7.42f);		// 67
	WriteVertex(F, 2.84f,	0.4f,	7.11f);		// 68
	WriteVertex(F, 2.98f,	0.4f,	6.77f);		// 69
	WriteVertex(F, 4.05f,	0.4f,	5.95f);		// 70
	WriteVertex(F, 4.31f,	0.4f,	5.64f);		// 71
	WriteVertex(F, 4.97f,	0.4f,	4.91f);		// 72
	WriteVertex(F, 5.39f,	0.4f,	4.34f);		// 73
	WriteVertex(F, 5.88f,	0.4f,	3.88f);		// 74
	WriteVertex(F, 6.34f,	0.4f,	3.66f);		// 75
	WriteVertex(F, 6.88f,	0.4f,	3.65f);		// 76
	WriteVertex(F, 7.02f,	0.0f,	4.85f);		// 77
	WriteVertex(F, 7.36f,	0.0f,	4.99f);		// 78
	WriteVertex(F, 7.35f,	0.0f,	5.39f);		// 79
	WriteVertex(F, 7.13f,	0.0f,	5.6f);		// 80
	WriteVertex(F, 6.46f,	0.0f,	5.92f);		// 81
	WriteVertex(F, 5.23f,	0.0f,	6.79f);		// 82
	WriteVertex(F, 4.97f,	0.0f,	6.88f);		// 83
	WriteVertex(F, 4.77f,	0.0f,	6.74f);		// 84
	WriteVertex(F, 4.87f,	0.0f,	6.43f);		// 85
	WriteVertex(F, 10.24f,	0.0f,	0.0f);		// 86
	WriteVertex(F, 10.24f,	0.0f,	10.24f);	// 87
	WriteVertex(F, 0.0f,	0.0f,	10.24f);	// 88

	// Write out each triangle with the indices of the vertices they will use
	int GeometryCount = 172;
	F.write((char*)&GeometryCount, sizeof(int));

	WriteGeometry(F, 0, 5, 4);		// 1
	WriteGeometry(F, 0, 6, 5);		// 2
	WriteGeometry(F, 0, 7, 6);		// 3
	WriteGeometry(F, 0, 86, 7);		// 4
	WriteGeometry(F, 7, 86, 8);		// 5
	WriteGeometry(F, 8, 86, 9);		// 6
	WriteGeometry(F, 9, 86, 10);	// 7
	WriteGeometry(F, 10, 86, 11);	// 8
	WriteGeometry(F, 11, 86, 12);	// 9
	WriteGeometry(F, 12, 86, 87);	// 10
	WriteGeometry(F, 13, 12, 87);	// 11
	WriteGeometry(F, 14, 13, 87);	// 12
	WriteGeometry(F, 15, 14, 87);	// 13
	WriteGeometry(F, 16, 15, 87);	// 14
	WriteGeometry(F, 17, 16, 87);	// 15
	WriteGeometry(F, 18, 17, 87);	// 16
	WriteGeometry(F, 19, 18, 87);	// 17
	WriteGeometry(F, 20, 19, 87);	// 18
	WriteGeometry(F, 88, 20, 87);	// 19
	WriteGeometry(F, 21, 20, 88);	// 20
	WriteGeometry(F, 22, 21, 88);	// 21
	WriteGeometry(F, 23, 22, 88);	// 22
	WriteGeometry(F, 24, 23, 88);	// 23
	WriteGeometry(F, 25, 24, 88);	// 24
	WriteGeometry(F, 26, 25, 88);	// 25
	WriteGeometry(F, 0, 26, 88);	// 26
	WriteGeometry(F, 0, 1, 26);		// 27
	WriteGeometry(F, 0, 2, 1);		// 28
	WriteGeometry(F, 0, 3, 2);		// 29
	WriteGeometry(F, 0, 4, 3);		// 30
	WriteGeometry(F, 4, 5, 39);		// 31
	WriteGeometry(F, 5, 40, 39);	// 32
	WriteGeometry(F, 5, 6, 40);		// 33
	WriteGeometry(F, 6, 27, 40);	// 34
	WriteGeometry(F, 6, 7, 27);		// 35
	WriteGeometry(F, 27, 7, 28);	// 36
	WriteGeometry(F, 7, 8, 28);		// 37
	WriteGeometry(F, 28, 8, 29);	// 38
	WriteGeometry(F, 29, 8, 9);		// 39
	WriteGeometry(F, 29, 9, 30);	// 40
	WriteGeometry(F, 30, 9, 10);	// 41
	WriteGeometry(F, 30, 10, 31);	// 42
	WriteGeometry(F, 31, 10, 11);	// 43
	WriteGeometry(F, 31, 11, 32);	// 44
	WriteGeometry(F, 32, 11, 12);	// 45
	WriteGeometry(F, 32, 12, 75);	// 46
	WriteGeometry(F, 75, 12, 76);	// 47
	WriteGeometry(F, 76, 12, 58);	// 48
	WriteGeometry(F, 58, 12, 59);	// 49
	WriteGeometry(F, 59, 12, 60);	// 50
	WriteGeometry(F, 60, 12, 13);	// 51
	WriteGeometry(F, 60, 13, 61);	// 52
	WriteGeometry(F, 61, 13, 14);	// 53
	WriteGeometry(F, 61, 14, 15);	// 54
	WriteGeometry(F, 62, 61, 15);	// 55
	WriteGeometry(F, 62, 15, 16);	// 56
	WriteGeometry(F, 63, 62, 16);	// 57
	WriteGeometry(F, 63, 16, 17);	// 58
	WriteGeometry(F, 64, 63, 17);	// 59
	WriteGeometry(F, 64, 17, 18);	// 60
	WriteGeometry(F, 65, 64, 18);	// 61
	WriteGeometry(F, 65, 18, 19);	// 62
	WriteGeometry(F, 65, 19, 20);	// 63
	WriteGeometry(F, 66, 65, 20);	// 64
	WriteGeometry(F, 66, 20, 21);	// 65
	WriteGeometry(F, 67, 66, 21);	// 66
	WriteGeometry(F, 22, 67, 21);	// 67
	WriteGeometry(F, 23, 67, 22);	// 68
	WriteGeometry(F, 23, 68, 67);	// 69
	WriteGeometry(F, 24, 68, 23);	// 70
	WriteGeometry(F, 25, 68, 24);	// 71
	WriteGeometry(F, 25, 69, 68);	// 72
	WriteGeometry(F, 32, 75, 74);	// 73
	WriteGeometry(F, 32, 74, 73);	// 74
	WriteGeometry(F, 33, 32, 73);	// 75
	WriteGeometry(F, 33, 73, 72);	// 76
	WriteGeometry(F, 34, 33, 72);	// 77
	WriteGeometry(F, 34, 72, 71);	// 78
	WriteGeometry(F, 34, 71, 70);	// 79
	WriteGeometry(F, 35, 34, 70);	// 80
	WriteGeometry(F, 35, 70, 69);	// 81
	WriteGeometry(F, 36, 35, 69);	// 82
	WriteGeometry(F, 25, 36, 69);	// 83
	WriteGeometry(F, 26, 36, 25);	// 84
	WriteGeometry(F, 26, 37, 36);	// 85
	WriteGeometry(F, 26, 1, 37);	// 86
	WriteGeometry(F, 1, 38, 37);	// 87
	WriteGeometry(F, 2, 38, 1);		// 88
	WriteGeometry(F, 2, 39, 38);	// 89
	WriteGeometry(F, 3, 39, 2);		// 90
	WriteGeometry(F, 3, 4, 39);		// 91
	WriteGeometry(F, 39, 40, 50);	// 92
	WriteGeometry(F, 40, 51, 50);	// 93
	WriteGeometry(F, 40, 27, 51);	// 94
	WriteGeometry(F, 51, 27, 41);	// 95
	WriteGeometry(F, 27, 28, 41);	// 96
	WriteGeometry(F, 41, 28, 42);	// 97
	WriteGeometry(F, 28, 29, 42);	// 98
	WriteGeometry(F, 42, 29, 43);	// 99
	WriteGeometry(F, 43, 29, 30);	// 100
	WriteGeometry(F, 43, 30, 44);	// 101
	WriteGeometry(F, 44, 30, 31);	// 102
	WriteGeometry(F, 44, 31, 45);	// 103
	WriteGeometry(F, 45, 31, 32);	// 104
	WriteGeometry(F, 46, 45, 32);	// 105
	WriteGeometry(F, 46, 32, 33);	// 106
	WriteGeometry(F, 47, 46, 33);	// 107
	WriteGeometry(F, 47, 33, 34);	// 108
	WriteGeometry(F, 35, 47, 34);	// 109
	WriteGeometry(F, 48, 47, 35);	// 110
	WriteGeometry(F, 36, 48, 35);	// 111
	WriteGeometry(F, 49, 48, 36);	// 112
	WriteGeometry(F, 37, 49, 36);	// 113
	WriteGeometry(F, 38, 49, 37);	// 114
	WriteGeometry(F, 38, 50, 49);	// 115
	WriteGeometry(F, 39, 50, 38);	// 116
	WriteGeometry(F, 50, 51, 57);	// 117
	WriteGeometry(F, 51, 52, 57);	// 118
	WriteGeometry(F, 51, 41, 52);	// 119
	WriteGeometry(F, 41, 53, 52);	// 120
	WriteGeometry(F, 41, 42, 53);	// 121
	WriteGeometry(F, 53, 42, 54);	// 122
	WriteGeometry(F, 42, 43, 54);	// 123
	WriteGeometry(F, 54, 43, 44);	// 124
	WriteGeometry(F, 54, 44, 55);	// 125
	WriteGeometry(F, 55, 44, 45);	// 126
	WriteGeometry(F, 55, 45, 46);	// 127
	WriteGeometry(F, 47, 55, 46);	// 128
	WriteGeometry(F, 56, 55, 47);	// 129
	WriteGeometry(F, 48, 56, 47);	// 130
	WriteGeometry(F, 48, 57, 56);	// 131
	WriteGeometry(F, 49, 57, 48);	// 132
	WriteGeometry(F, 50, 57, 49);	// 133
	WriteGeometry(F, 57, 52, 56);	// 134
	WriteGeometry(F, 52, 53, 54);	// 135
	WriteGeometry(F, 56, 54, 55);	// 136
	WriteGeometry(F, 52, 54, 56);	// 137
	WriteGeometry(F, 73, 74, 77);	// 138
	WriteGeometry(F, 74, 75, 77);	// 139
	WriteGeometry(F, 75, 76, 77);	// 140
	WriteGeometry(F, 76, 58, 77);	// 141
	WriteGeometry(F, 77, 58, 78);	// 142
	WriteGeometry(F, 58, 59, 78);	// 143
	WriteGeometry(F, 78, 59, 60);	// 144
	WriteGeometry(F, 78, 60, 79);	// 145
	WriteGeometry(F, 79, 60, 61);	// 146
	WriteGeometry(F, 79, 61, 62);	// 147
	WriteGeometry(F, 80, 79, 62);	// 148
	WriteGeometry(F, 81, 80, 62);	// 149
	WriteGeometry(F, 81, 62, 63);	// 150
	WriteGeometry(F, 82, 81, 63);	// 151
	WriteGeometry(F, 82, 63, 64);	// 152
	WriteGeometry(F, 83, 82, 64);	// 153
	WriteGeometry(F, 65, 83, 64);	// 154
	WriteGeometry(F, 84, 83, 65);	// 155
	WriteGeometry(F, 66, 84, 65);	// 156
	WriteGeometry(F, 67, 84, 66);	// 157
	WriteGeometry(F, 68, 84, 67);	// 158
	WriteGeometry(F, 69, 84, 68);	// 159
	WriteGeometry(F, 69, 70, 84);	// 160
	WriteGeometry(F, 70, 85, 84);	// 161
	WriteGeometry(F, 70, 71, 85);	// 162
	WriteGeometry(F, 71, 72, 85);	// 163
	WriteGeometry(F, 72, 77, 85);	// 164
	WriteGeometry(F, 72, 73, 77);	// 165
	WriteGeometry(F, 77, 78, 79);	// 166
	WriteGeometry(F, 77, 79, 80);	// 167
	WriteGeometry(F, 81, 77, 80);	// 168
	WriteGeometry(F, 85, 77, 81);	// 169
	WriteGeometry(F, 85, 81, 82);	// 170
	WriteGeometry(F, 84, 85, 82);	// 171
	WriteGeometry(F, 84, 82, 83);	// 172

	// Write out each particle system effect with it's information
	int EffectCount = 1;
	F.write((char*)&EffectCount, sizeof(int));

	// Effect 0
	char EffectFile_00[32];
	strcpy_s(EffectFile_00, sizeof("EffectTest.bin"), "EffectTest.bin");
	F.write((char*)&EffectFile_00, sizeof(EffectFile_00));

	F.close();
}

void WriteVertex(fstream& F, float X, float Y, float Z)
{
	F.write((char*)&X, 		sizeof(float));
	F.write((char*)&Y, 		sizeof(float));
	F.write((char*)&Z, 		sizeof(float));
}

void WriteGeometry(fstream& F, int Vertex1, int Vertex2, int Vertex3)
{
	F.write((char*)&Vertex1, sizeof(int));
	F.write((char*)&Vertex2, sizeof(int));
	F.write((char*)&Vertex3, sizeof(int));
}