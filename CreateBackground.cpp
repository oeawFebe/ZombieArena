#include "ZombieArena.h"
int createBackground(VertexArray& rVA, IntRect arena)
{
	//how big is each tile/texture
	const int TILE_SIZE = 50;
	const int TILE_TYPES = 3;
	const int VERTS_IN_QUAD = 4;//vertices in quad
	int worldWidth = arena.width / TILE_SIZE;
	int worldHeight = arena.height / TILE_SIZE;
	rVA.setPrimitiveType(Quads);//explained in book set from Line, Quads, etc.
	rVA.resize(worldWidth * worldHeight * VERTS_IN_QUAD);//explained that it can resize during loop
	int currentVertex = 0;
	for (int w = 0;w < worldWidth;w++)
	{
		for (int h = 0;h < worldHeight;h++)
		{
			//position each vertex in current quad
			rVA[currentVertex + 0].position = Vector2f(w * TILE_SIZE, h * TILE_SIZE);
			rVA[currentVertex + 1].position = Vector2f((w+1) * TILE_SIZE, h * TILE_SIZE);
			rVA[currentVertex + 2].position = Vector2f((w + 1) * TILE_SIZE, (h + 1) * TILE_SIZE);
			rVA[currentVertex + 3].position = Vector2f(w * TILE_SIZE, (h + 1) * TILE_SIZE);
			// define position in the texture for current quad
			if (h == 0 || h == worldHeight - 1 || w == 0 || w == worldWidth - 1) 
			{
				rVA[currentVertex + 0].texCoords = Vector2f(0, 0 + TILE_TYPES * TILE_SIZE); 
				rVA[currentVertex + 1].texCoords = Vector2f(TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE); 
				rVA[currentVertex + 2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + TILE_TYPES * TILE_SIZE); 
				rVA[currentVertex + 3].texCoords = Vector2f(0, TILE_SIZE + TILE_TYPES * TILE_SIZE); 
				
			}
			else
			{
				srand((unsigned int)time(0) + h * w - h);
				int mOrG = rand() % TILE_TYPES;
				int verticalOffset = mOrG * TILE_SIZE;//Mud or Grass
				rVA[currentVertex + 0].texCoords = Vector2f(0, 0 + verticalOffset);
				rVA[currentVertex + 1].texCoords = Vector2f(TILE_SIZE, 0 + verticalOffset);
				rVA[currentVertex + 2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);
				rVA[currentVertex + 3].texCoords = Vector2f(0, TILE_SIZE + verticalOffset);
			}
			//position is ready for next 4 vertices
			currentVertex = currentVertex + VERTS_IN_QUAD;

		}
	}
	return TILE_SIZE;
}