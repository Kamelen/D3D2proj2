#include "Terrain.h"
#include <iostream>
Terrain::Terrain(const string& filename, int rows, int cols, float heightScale, float heightOffset, ID3D11Device *device, ID3D11DeviceContext *deviceContext, std::string *textureName, std::string blendMap)
	:D3DObject()
{
	
	LoadHeightMap(filename,rows,cols,heightScale,heightOffset);
	createMesh();
	createIndices();
	D3DObject::initBuffer(device,deviceContext);

	textures = new ID3D11ShaderResourceView*[3];

	D3DX11CreateShaderResourceViewFromFile(device, textureName[0].c_str() , NULL , NULL, &this->textures[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(device, textureName[1].c_str() , NULL , NULL, &this->textures[1], NULL);
	D3DX11CreateShaderResourceViewFromFile(device, textureName[2].c_str() , NULL , NULL, &this->textures[2], NULL);
	D3DX11CreateShaderResourceViewFromFile(device, blendMap.c_str() , NULL , NULL, &this->texture, NULL);

}


Terrain::~Terrain(void)
{
	if(heightMap)
	{

		for(int i = 0; i < terrainWidth; i++)
		{
			delete[] heightMap[i];
		}
		delete [] heightMap;
	}

	if(textures)
	{
		for(int i = 0; i < 3; i++)
		{
			textures[i]->Release();
			textures[i] = NULL;

		}
		delete []  textures;
	}
}

void Terrain::LoadHeightMap(const string& filename, int rows, int cols, float heightScale, float heightOffset)
{
	terrainWidth = cols;
	terrainHeight = rows;

	vector<unsigned char> vertexHeights(rows*cols);

	ifstream fin;
	fin.open(filename.c_str(),ios_base::binary);


	if(!fin) return;
	
	fin.read((char *)&vertexHeights[0], (streamsize)vertexHeights.size());
	fin.close();

	heightMap = new float*[terrainWidth];
	for(int i = 0; i < terrainWidth; i++)
	{
		heightMap[i] = new float[terrainHeight];
	}
	
	for(int i = 0; i < terrainWidth; ++i)
	{
		for(int j = 0; j < terrainHeight; ++j)
		{
			int k = i*terrainWidth + j;

			heightMap[i][j] = (float) vertexHeights[k] * heightScale + heightOffset;
		}
	}
	for(int i = 0; i < 1; i++)
	{
		filter(rows, cols);
	}
	
}

void Terrain::filter(int rows, int cols)
{
	float ** filteredHeightMap = new float*[rows];
	for(int i = 0; i < rows; i++)
	{
		filteredHeightMap[i] = new float[cols];
	}

	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			filteredHeightMap[i][j] = sampleHeight(i,j);
		}
	}
	//tar bort den gamla heightmappen
	for(int i = 0; i < cols; i++)
	{
		delete[] heightMap[i];
	}
	delete [] heightMap;

	//slår om pekaren till den nya filtrerade heightmappen
	heightMap = filteredHeightMap;
}

float Terrain::sampleHeight(int i, int j)
{
	float average = 0.0f;
	float sample = 0.0f;

	for(int m = i-1; m <= i+1; ++m)
	{
		for(int n = j-1; n <= j+1; ++n)
		{
			if(inBounds(m, n))
			{
				average += heightMap[m][n];
				sample += 1.0f;
			}
		}
	}
	return average/sample;
}
bool Terrain::inBounds(int i, int j)
{
	return (i >= 0 && i < terrainHeight && j >= 0 && j < terrainWidth);
}
void Terrain::createMesh()
{
	//Vertex* mesh = new Vertex[ terrainHeight * terrainWidth];
	mesh.resize(terrainHeight * terrainWidth);
	this->spacing = 1;
	int index = 0;

	float halfWidth = (terrainWidth-1) * spacing * 0.5;
	float halfDepth = (terrainHeight-1) * spacing * 0.5;

	float x;
	float y;
	float z;

	float repeat = 1.0;
	for(int i=0; i<(terrainWidth); i++)
	{
		x = -halfWidth + (i * spacing);
		for(int j=0; j<(terrainHeight); j++)
		{
			index = i * terrainWidth + j;
			z = -halfDepth + (j * spacing);
			y = this->heightMap[i][j];

			mesh.at(index).pos = D3DXVECTOR3(x,y,z);
			mesh.at(index).normal = D3DXVECTOR3(0,0,0);
			mesh.at(index).uv.x = i/(terrainWidth/repeat);
			mesh.at(index).uv.y = j/(terrainHeight/repeat);
		}
	}
	this->nrOfVertices =  terrainHeight * terrainWidth;
	//this->mesh = mesh;

	this->CalculateNormals();
}
void Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	D3DXVECTOR3* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new D3DXVECTOR3[(terrainHeight-1) * (terrainWidth-1)];

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(terrainHeight-1); j++)
	{
		for(i=0; i<(terrainWidth-1); i++)
		{
			index1 = (j * terrainHeight) + i;
			index2 = (j * terrainHeight) + (i+1);
			index3 = ((j+1) * terrainHeight) + i;

			// Get three vertices from the face.
			
			vertex1[0] = mesh.at(index1).pos.x;
			vertex1[1] = mesh.at(index1).pos.y;
			vertex1[2] = mesh.at(index1).pos.z;
			
			vertex2[0] = mesh.at(index2).pos.x;
			vertex2[1] = mesh.at(index2).pos.y;
			vertex2[2] = mesh.at(index2).pos.z;
		
			vertex3[0] = mesh.at(index3).pos.x;
			vertex3[1] = mesh.at(index3).pos.y;
			vertex3[2] = mesh.at(index3).pos.z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (terrainHeight-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<terrainHeight; j++)
	{
		for(i=0; i<terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (terrainWidth-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (terrainHeight-1)))
			{
				index = (j * (terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (terrainWidth-1)) && (j < (terrainHeight-1)))
			{
				index = (j * (terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}
			
			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
			
			// Get an index to the vertex location in the height map array.
			index = (j * terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			mesh.at(index).normal.x = (sum[0] / length);
			mesh.at(index).normal.y = -1 * (sum[1] / length);
			mesh.at(index).normal.z = (sum[2] / length);
		}
	}
	delete [] normals;
}

void Terrain::createIndices()
{
	int i = 0;

	this->indices.resize(terrainWidth * terrainHeight * 6);

	for(int x = 0; x < terrainWidth-1; x++)
	{
		for(int z = 0; z < terrainHeight-1; z++)
		{

			indices[i] = x * terrainWidth + z;
			indices[i+1] = x * terrainWidth + z + 1; 
			indices[i+2] = (x+1) * terrainWidth + z + 1;

			indices[i+3] = x * terrainWidth + z;
			indices[i+4] = (x+1) * terrainWidth + z + 1;
			indices[i+5] = (x+1) * terrainWidth + z;

			i += 6;
		}
	}
}

D3DXVECTOR3 Terrain::getTerrainPos(int col,int row)
{
	return this->mesh.at(col * this->terrainHeight + row).pos;
}

ID3D11ShaderResourceView* Terrain::getTerTexture(int i) const
{
	return this->textures[i];
}
float Terrain::getY(int x, int z)
{


	float halfWidth = (terrainWidth-1) * spacing * 0.5;
	float halfDepth = (terrainHeight-1) * spacing * 0.5;

	float i = (halfWidth + x) / this->spacing;
	float j = (halfDepth + z) / this->spacing;

	int col = (int)floorf(i);
	int row = (int)floorf(j);

	float a = heightMap[col][row];
	float b = heightMap[col+1][row];
	float c = heightMap[col][row+1];
	float d = heightMap[col+1][row+1];
 
	float s = i - float(col);
	float t = j - float(row);

	float uY;
	float vY;

	if(s + t <= 1.0f)
	{
		uY = b - a;
		vY = c - a;

		return (a + s*uY + t*vY) + 40;
	}

	else
	{
		uY = c - d;
		vY = b - d;

		return (d + (1.0f - s) * uY + (1.0f - t) * vY) + 40;
	}


}
