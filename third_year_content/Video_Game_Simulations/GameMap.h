#pragma once
#include <string>
#include <vector>

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;
		class FruitWizardGame;

		class GameMap	{
		public:
			// node structure to generate path 
			struct MapNode {
				MapNode*	bestParent;
				int			posX;
				int			posY;
				float		g;
				float		h;
			};
			MapNode* nodeData;

			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, FruitWizardGame* fwg);
			~GameMap();

			void DrawMap(GameSimsRenderer & r);

			int GetMapWidth() const {
				return mapWidth;
			}

			int GetMapHeight() const {
				return mapHeight;
			}

				// When generating the map I am calling this method to create an object for every ground or wall tile
			void AddColliders(FruitWizardGame* game);
				// Getter to return the tile type, the "letter" in a specific position
			char GetMapData(int data) {
				return mapData[data];
			}
				// method to return cost value to calculate best path and to know where it's blocked
			int GetMapCost(int cost) {
				return mapCosts[cost];
			}

			/*
				Pathfinder:
				the game world is defined by a set of 30 by 20 tiles, each 16 units across in each axis. 
				Therefore, you should also consider methods that can transform a world position into a tile position, and vice versa, as this will help in working in the right 'space'.

				The two Vector2s would then be the 'world space' source and destination of the path to calculate, with the return type indicating whether a path can be found or not. 
				The path container will be used to store the world positions of the nodes that must be passed through to get from the source to destination.

				A* requires a heuristic to determine how 'good' a particular tile is likely to be. 
			*/
			Vector2 WorldPosToTilePos(const Vector2& worldPos) const;
			Vector2 TilePosToWorldPos(const Vector2& tilePos) const;
			bool SearchLoop(const Vector2& source, const Vector2& destination, std::vector <Vector2>& path) const;
			MapNode* FindBestNode(std::vector<MapNode*> openList) const;
			void FindNeighbours(MapNode* bestNode, std::vector<MapNode*>& activeNeighbours) const;
			float ManhattanDistance(MapNode* firstNode, MapNode* secondNode) const;
			void GeneratePath(std::vector<MapNode*>& closedList, std::vector < Vector2 >& path, MapNode* sourcePoint, MapNode* destinationPoint)const ;

		protected:
			void BuildMapMesh();
			void AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize = Vector2(16, 16));
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize = Vector2(16,16));

			int mapWidth;
			int mapHeight;
			int structureCount;

			/*
				in order not to have a collider for each tile, I am grouping adjacent tiles of the same tipe together
				and to do so I need to keep trach of them in the same raw and of tile changes to have a big collider 
				starting from the first tile to the last tile including the ones in the middle
			*/
			bool alreadyG = false;
			float startX;
			float startY;
			float updateX;
			Rendering::TextureBase*	tileTexture;
			char*			mapData;
			int*			mapCosts;
			Vector2*		mapTexCoords;

			MeshGeometry* mapMesh;
			FruitWizardGame* fruitGame;
		};
	}
}

