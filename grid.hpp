#pragma once

#include <vector>

#include "gridNode.hpp"

class Grid final
{
public:
	Grid(
		const unsigned int width,
		const unsigned int height,
		const float gravity = 48);
	~Grid();
	void add(GridNode *node);
	void remove(GridNode *node);
	void update(const float timestep);
	void paintWall(const unsigned int x, const unsigned int y);
	void paintFloor(const unsigned int x, const unsigned int y);

private:
	class Cell;
	Cell *getCell(const unsigned int x, const unsigned int y);

	enum BarrierType
	{
		NONE,
		SOLID
	};

	class Cell final
	{
	public:
		Cell();
		BarrierType getWallType() const;
		BarrierType getFloorType() const;
		void setWallType(const BarrierType type);
		void setFloorType(const BarrierType type);

	private:
		struct {
			BarrierType type;
		} wall;

		struct {
			BarrierType type;
		} floor;
	};

	std::vector<GridNode*> nodes;
	float gravity;
	unsigned int resolution;
	unsigned int width, height;
	Cell *cells;
};