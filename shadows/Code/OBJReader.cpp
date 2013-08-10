#include "OBJReader.h"


OBJReader::OBJReader(void)
{
}


OBJReader::~OBJReader(void)
{
}

bool OBJReader::ReadFileCounts(string fileName, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;


	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(fileName);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while(!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ') { vertexCount++; }
			if(input == 't') { textureCount++; }
			if(input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ') { faceCount++; }
		}
		
		// Otherwise read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}

Vertex* OBJReader::LoadDataStructures(string fileName, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	Vertex* mesh;
	D3DXVECTOR3 *vertices, *normals;
	D3DXVECTOR2 *texcoords;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;


	// Initialize the four data structures.
	vertices = new D3DXVECTOR3[vertexCount];
	if(!vertices)
	{
		return false;
	}

	texcoords = new D3DXVECTOR2[textureCount];
	if(!texcoords)
	{
		return false;
	}

	normals = new D3DXVECTOR3[normalCount];
	if(!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if(!faces)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(fileName);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if(input == ' ') 
			{ 
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++; 
			}

			// Read in the texture uv coordinates.
			if(input == 't') 
			{ 
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++; 
			}

			// Read in the normals.
			if(input == 'n') 
			{ 
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++; 
			}
		}

		// Read in the faces.
		if(input == 'f') 
		{
			fin.get(input);
			if(input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
				    >> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
				    >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();



	//read this into a vertexMesh and return it
	mesh = new Vertex[faceCount*3];
	int face = 0;

	for(int i = 0; i < faceCount*3; i+=3)
	{
		mesh[i].pos = vertices[faces[face].vIndex1-1];
		mesh[i].normal = normals[faces[face].nIndex1-1];
		mesh[i].uv = texcoords[faces[face].tIndex1-1];

		mesh[i+1].pos = vertices[faces[face].vIndex2-1];
		mesh[i+1].normal = normals[faces[face].nIndex2-1];
		mesh[i+1].uv = texcoords[faces[face].tIndex2-1];

		mesh[i+2].pos = vertices[faces[face].vIndex3-1];
		mesh[i+2].normal = normals[faces[face].nIndex3-1];
		mesh[i+2].uv = texcoords[faces[face].tIndex3-1];
		
		face++;
	}

	// Release the four data structures.
	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}
	if(texcoords)
	{
		delete [] texcoords;
		texcoords = 0;
	}
	if(normals)
	{
		delete [] normals;
		normals = 0;
	}
	if(faces)
	{
		delete [] faces;
		faces = 0;
	}


	return mesh;
}



Vertex* OBJReader::getOBJfromFile(string fileName, int &nrOfVerts)
{
	Vertex* mesh;

	int vertexCount;
	int textureCount;
	int normalCount;
	int faceCount;

	//get all the counts to ease reading all the data
	ReadFileCounts(fileName,vertexCount,textureCount,normalCount,faceCount);
	mesh = LoadDataStructures(fileName, vertexCount, textureCount, normalCount, faceCount);

	nrOfVerts = faceCount*3;
	return mesh;
}
