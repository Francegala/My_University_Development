#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>
#include "Wall.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"

using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, FruitWizardGame* fwg) {
	this->fruitGame = fwg;
	tileTexture = texManager.GetTexture("FruitWizard//arcade_platformerV2.png");

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData = new char[mapWidth * mapHeight];

	//	for Path Finding read values from the file
	mapCosts = new int[mapWidth * mapHeight];
	nodeData = new MapNode[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';
		}
	}

	// get ready nodes to use but in order not to have nodes with data, they are empty
	// values like g and h set as maximum float so then compared they are considered like empty cause for sure the value is higher than any othe value
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			// same way used to clear the nodes when generating new path
			nodeData[tileIndex].bestParent = nullptr;
			nodeData[tileIndex].g = FLT_MAX;
			nodeData[tileIndex].h = FLT_MAX;
			nodeData[tileIndex].posX = x;
			nodeData[tileIndex].posY = (mapHeight - 1 - y); // here not simple y just because otherwise it is read mirrored-reflected
		}
	}

	BuildMapMesh();
}

GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;

	delete mapMesh;
}

void GameMap::DrawMap(GameSimsRenderer & r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
}
/*
		This function adds a collider to the relevant tiles representing the ground, a wall or an edge
		it iterates throught the map data and where the type is a specific one,
		it creates a wall SimObject and it adds a mass of 0 so every collision is completely inelastic
		it uses the tile position to give a specific game object position and collider

		To avoid creating an object for every tile, the function saves the position of the first tile,
		then is saves the position of a new tile of the same tipe updating a variable and when detected a different one it creates the object
*/
void NCL::CSC3222::GameMap::AddColliders(FruitWizardGame* game)
{
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
			if (tileType == 'G' && !alreadyG) {
				startX = x - 0.5f;
				startY = y;
				alreadyG = !alreadyG;
			}
			if (tileType == 'G' && alreadyG) {
				updateX = x + 0.5f;
			}
			if (tileType != 'G' && alreadyG) {
				alreadyG = !alreadyG;
				Wall* wall = new Wall();
				wall->SetInverseMass(0);
				wall->SetPosition(Vector2(8 +((startX + updateX)/2) * 16, 8 + startY * 16));
				wall->SetCollider(new AABBVolume(((float)updateX-startX)*8.0f, 8.0f));
				wall->SetCharacter(SimObject::Character::Wall);
				game->AddNewObject(wall);
			}
			if (tileType == 'W' || tileType == 'w' || tileType == 'H' || tileType == 'F') {
				Wall* wall = new Wall();
				wall->SetInverseMass(0);
				wall->SetPosition(Vector2(8 + x * 16, 8 + y * 16));
				wall->SetCollider(new AABBVolume(8.0f, 8.0f));
				wall->SetCharacter(SimObject::Character::Wall);
				game->AddNewObject(wall);
			}
		}
	}
}

/*
	Convert position from the map coordinates to the data asset values and vice versa
*/
Vector2 NCL::CSC3222::GameMap::WorldPosToTilePos(const Vector2& worldPos) const
{
	return Vector2(round((worldPos.x / 16)), round((worldPos.y / 16)));
}
Vector2 NCL::CSC3222::GameMap::TilePosToWorldPos(const Vector2& tilePos) const
{
	return Vector2(tilePos.x * 16 + 8, tilePos.y * 16 + 8);
}

/*
	iterative process that continues until destination has been found or open list is empty

	1. best node is removed frm open list and added to close list
	2. if best node is destination path is found
	3. best node list expanded examining nodes connected to best node by edge
	4. if connected node is in close list it should be determined id shorter path has been found
	5. if connected node is not in closed list, add it to open list
*/
bool NCL::CSC3222::GameMap::SearchLoop(const Vector2& source, const Vector2& destination, std::vector<Vector2>& path) const
{
	//Initialization(Start Node, Goal Node, Open List, Closed List)
	std::vector<MapNode*> priorityQueue;
	std::vector<MapNode*> closedList;

	if (path.size() > 0) {
		// it has already been used, start from scratchs and clear all the values restoring the one given when creating the map (method above)
		path.clear();
		for (int y = 0; y < mapHeight; ++y) {
			for (int x = 0; x < mapWidth; ++x) {
				int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;

				nodeData[tileIndex].bestParent = nullptr;
				nodeData[tileIndex].g = FLT_MAX;
				nodeData[tileIndex].h = FLT_MAX;
				nodeData[tileIndex].posX = x;
				nodeData[tileIndex].posY = (mapHeight - 1 - y); // here not simple y just because otherwise it is read mirrored, reflected
			}
		}
	}

	// Getting MapNodes from positions in the game world
	int tileSourceIndex = (((int)source.y * mapWidth) + (int)source.x);
	MapNode* sourcePoint = &nodeData[tileSourceIndex]; //A = Start Node

	int tileDestinationIndex = (int)destination.y * mapWidth + (int)destination.x;
	MapNode* destinationPoint = &nodeData[tileDestinationIndex]; //B = Goal Node

	// the first position is certain so add it to the priority queue
	sourcePoint->g = 0;	//Ag = 0
	sourcePoint->h = ManhattanDistance(sourcePoint, destinationPoint); //Ah = heuristic(Start Node, Goal Node)
	priorityQueue.emplace_back(sourcePoint); //Insert A into Open List

	while (!priorityQueue.empty()) { //while Open List not empty
		// use the priority queue to find the best node
		MapNode* bestNode = FindBestNode(priorityQueue); //Node of lowest f-value = Best Node in Open List

		// at this point we have the one with lowest sum of g and j values
		// if we reached the end return
		if (bestNode == destinationPoint) {//If P = B, we have a solution!
			// generate the path (we also need to turn the list
			GeneratePath(closedList, path, sourcePoint, destinationPoint); //procedure Generate Path(Closed List, Path, A, B)
			return true;
		}
		else {
			//Data for Q is taken from Node Graph
			std::vector<MapNode*> activeNeighbours;
			// knowing the best node, let's expand the list of eligible candidates searching for neighbouring nodes
			FindNeighbours(bestNode, activeNeighbours);

			//iterating through the list of tiles close to the best node
			//for Each node Q connected to P
			for (auto neighbour : activeNeighbours) {

				// this node might not be in teh priority queue so it must be added but it needs to hold the correct values
				float tempG = bestNode->g + mapCosts[((neighbour->posY) * mapWidth) + neighbour->posX]; //g = QParentg + cost from P to Q
				float tempH = ManhattanDistance(neighbour, destinationPoint); //h = heuristic(Q, Goal Node)
				float tempF = tempG + tempH; //f = g + h

				// search if it is in the closest list to skip it
				if (std::count(closedList.begin(), closedList.end(), neighbour)) continue;

				// if it is already in the priority queue AND IT'S NOT A BETTER ROUTE THAN THE PAST EXPANSIONS skip it
				//Q in Open List AND f > Qf then
				std::vector<MapNode*>::iterator neighbourPosition = std::find(priorityQueue.begin(), priorityQueue.end(), neighbour);
				if (std::count(priorityQueue.begin(), priorityQueue.end(), neighbour) && tempF > (neighbour->g + neighbour->h)) {
		/* -> */			continue; // not sure if I should skip when >= or just >
				}
				else {
					//add the values and PARENT = BEST NODE and calculate values as it might be a better route than teh previous
					neighbour->g = tempG; // Qg = g
					neighbour->h = tempH; //Qh = h
					neighbour->bestParent = bestNode; //Qparent = P
					// and then add it to the list if not already in otherwise we just updated its values and that's it
					if (neighbourPosition == priorityQueue.end()) {//if Q not in Open List then
						priorityQueue.emplace_back(neighbour);//Insert Q in Open List
					}

					// remove the best node from priority open list and put it into the close list
					std::vector<MapNode*>::iterator bestNodePosition = std::find(priorityQueue.begin(), priorityQueue.end(), bestNode);
					if (!std::count(priorityQueue.begin(), priorityQueue.end(), bestNode)) priorityQueue.erase(bestNodePosition); //Remove P from Open List
					closedList.emplace_back(bestNode); //Insert P in Closed List
				}

			}
			
			// after iterating through the list of all the neighbour if best node is still in the priority, it must be removed
			std::vector<MapNode*>::iterator bestNodePosition = std::find(priorityQueue.begin(), priorityQueue.end(), bestNode);
			if (bestNodePosition != priorityQueue.end()) {
				priorityQueue.erase(bestNodePosition);
			}
		}
	}
	return false;
}

/*
	to find a best node, iterate throught the list and evaluate if the sum of the g(travelling) value plus h(heuristic) values is lower that the current best node
*/
GameMap::MapNode* NCL::CSC3222::GameMap::FindBestNode(std::vector<MapNode*> priorityQueue) const
{
	// starting from the first one in the list search if there is any other better
	float bestF = priorityQueue[0]->g + priorityQueue[0]->h;
	MapNode* bestNode = priorityQueue[0];

	// iterate the whole list to pick the one with lowest combination of values q and h
	for (auto& i : priorityQueue) {
		float iF = i->g + i->h;
		if (iF < bestF) {
			bestF = iF;
			bestNode = i;
		}
	}
	// return the one with lowest sum
	return bestNode;
}

/*
	5 is the tile type of wall and also used not to move ""vertically"" creating invisible boundaries
	1 simple tile floor walking
	2 is the jump which is possible but harder than 1 
	tiles '2' grow in lenght where there are tiles above ground to discourage jump in high since there always is gravity so it is pointless to calculate a path
		that takes for granted it can jump a lot when if it tries it falls
*/
void NCL::CSC3222::GameMap::FindNeighbours(MapNode* bestNode, std::vector<MapNode*>& activeNeighbours) const// valid nodes to expand to
{
	// knowing the tile picked as best node let's add all the tiles around it adding/subtracting 1 from x and y coords
	// starting from the best node, let's see if there is a neighbour to the right
	if (bestNode->posX < mapWidth - 2) {
		int tileType = mapCosts[((bestNode->posY) * mapWidth) + bestNode->posX + 1];
		if (tileType != 5) {
			MapNode* neighbour = &nodeData[((bestNode->posY) * mapWidth) + bestNode->posX + 1];
			if (neighbour != bestNode->bestParent)activeNeighbours.emplace_back(neighbour);
		}
	}

	// to the left
	if (bestNode->posX > 0) {
		int tileType = mapCosts[((bestNode->posY) * mapWidth) + bestNode->posX - 1];
		if (tileType != 5) {
			MapNode* neighbour = &nodeData[((bestNode->posY) * mapWidth) + bestNode->posX - 1];
			if (neighbour != bestNode->bestParent)activeNeighbours.emplace_back(neighbour);
		}
	}

	// to the top
	if (bestNode->posY > 0) {
		int tileType = mapCosts[((bestNode->posY - 1) * mapWidth) + bestNode->posX];
		if (tileType != 5) {
			MapNode* neighbour = &nodeData[((bestNode->posY - 1) * mapWidth) + bestNode->posX];
			if (neighbour != bestNode->bestParent)activeNeighbours.emplace_back(neighbour);
		}
	}

	// to the bottom
	if (bestNode->posY < mapHeight-2) {
		int tileType = mapCosts[((bestNode->posY + 1) * mapWidth) + bestNode->posX];
		if (tileType != 5) {
			MapNode* neighbour = &nodeData[((bestNode->posY + 1) * mapWidth) + bestNode->posX];
			if (neighbour != bestNode->bestParent)activeNeighbours.emplace_back(neighbour);
		}
	}
}
/*
	Manhattan distance, cost of traveling each axes in turn
	while it might be higher than Euclidean, it gives a distance that doesn't overestimate how many actual steps are required
*/
float NCL::CSC3222::GameMap::ManhattanDistance(GameMap::MapNode* firstNode, GameMap::MapNode* secondNode) const
{
	return (abs(firstNode->posX - secondNode->posX) + abs(firstNode->posY - secondNode->posY));
}

// at the end using closed list generate and fill the path list
void NCL::CSC3222::GameMap::GeneratePath(std::vector<MapNode*>& closedList, std::vector<Vector2>& path, MapNode* sourcePoint, MapNode* destinationPoint) const
{
	path.clear(); // create a new path
	// startin from the destination we "visit" all the parents to reach the starting point: 
	// parent of destination and then parent of parent and so on 
	// because we were registering what nodes we visited saving each one as parent of the following
	MapNode* node = destinationPoint; // R = B
	path.emplace_back(TilePosToWorldPos(Vector2(destinationPoint->posX, destinationPoint->posY)));
	while (node != sourcePoint) { //while R not equal A do
		path.emplace_back(TilePosToWorldPos(Vector2(node->posX, node->posY)));//Add R to Path
		node = node->bestParent; //R = Rparent
	}
	/*
		It must be reversed otherwise it is the other way around example
			destination
				destination's parent
				node we reached destination's parent through
				node before
				...etc.
				node reached after the starting point
			starting point
	*/
	std::reverse(path.begin(), path.end());

	// uncomment to know how many steps path finding calculates
	//std::cout << "\nsteps: " << path.size()<<"\n";
}

void GameMap::BuildMapMesh() {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 primaryTile;

	AddNewTilePos(0, 0, positions, Vector2(mapWidth *16.0f, mapHeight * 16.0f));
	AddNewTileTex(0, 176, texCoords, Vector2(160, 128));

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
	
			switch (tileType) {
			case '0': {
				continue;
			}break; //Top of ladder
				//the normal tile types first
				case 'T':primaryTile = Vector2(320, 0); break; //Top of ladder
				case 'L':primaryTile = Vector2(320, 16); break; //Ladder
				case 'B':primaryTile = Vector2(320, 32); break; //Bottom of ladder

				case 't':primaryTile = Vector2(336, 0); break; //Top of ladder
				case 'l':primaryTile = Vector2(336, 16); break; //Ladder
				case 'b':primaryTile = Vector2(336, 32); break; //Bottom of ladder

				case 'G': {
					primaryTile = Vector2(224, 48);
				}break;//Random grass tiles
				case 'F': { //Left End
					primaryTile = Vector2(208, 48);
				}break;//Random grass tiles
				case 'H': { //Right End
					primaryTile = Vector2(240, 48);
				}break;//Random grass tiles

				case 'W': {//wall
					primaryTile = Vector2(256, 112);
				}break;//Random grass tiles
				case 'w': {//wall top
					primaryTile = Vector2(256, 96);
				}break;//Random grass tiles
			}			
			AddNewTilePos(x, y, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize) {
	Vector3 topLeft		= Vector3((x + 0) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 topRight	= Vector3((x + 1) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 bottomLeft	= Vector3((x + 0) * tileSize.x, (y + 1) * tileSize.y, 0);
	Vector3 bottomRight = Vector3((x + 1) * tileSize.x, (y + 1) * tileSize.y, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize) {
	Vector2 topLeft		= Vector2((x + 0.0f) , (y + texSize.y));
	Vector2 topRight	= Vector2((x + texSize.x), (y + texSize.y));
	Vector2 bottomLeft	= Vector2((x + 0.0f) , (y + 0.0f));
	Vector2 bottomRight = Vector2((x + texSize.x), (y + 0.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft); 
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}