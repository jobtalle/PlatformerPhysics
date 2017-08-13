#include "grid.hpp"

Grid::Grid(
	const unsigned int width,
	const unsigned int height,
	const float gravity)
:width(width + 1), height(height + 1), resolution(32), gravity(gravity)
{
	cells = new Cell[this->width * this->height];

	for(unsigned int x = 0; x < width; ++x)
		paintFloor(x, height - 2);

	for(unsigned int y = 0; y < height; ++y)
	{
		paintWall(0, y);
		paintWall(width, y);
	}

	paintWall(14, height - 3);
	paintWall(15, height - 3);
	paintFloor(14, height - 3);

	paintWall(4, height - 3);
	paintWall(5, height - 3);
	paintFloor(4, height - 3);

	paintWall(16, height - 3);
	paintWall(17, height - 3);
	paintFloor(16, height - 3);

	paintWall(18, height - 4);
	paintWall(19, height - 4);
	paintFloor(18, height - 4);
	paintFloor(18, height - 3);
}

Grid::~Grid()
{
	delete cells;
}

void Grid::add(GridNode *node)
{
	nodes.push_back(node);

	node->add(this);
}

void Grid::remove(GridNode *node)
{
	nodes.erase(std::find(nodes.begin(), nodes.end(), node));

	node->remove();
}

void Grid::update(const float timestep)
{
	const float EPSILON = 0.001f;

	for(auto node : nodes)
	{
		// Add gravity
		if(!(node->flags & GridNode::Flags::ON_GROUND))
			node->vy += node->mass * gravity * timestep;

		// Move over X
		if(node->vx != 0)
		{
			float vx = node->vx * timestep;

			if(vx < -float(resolution) + EPSILON) vx = -float(resolution) + EPSILON;
			if(vx > float(resolution) - EPSILON) vx = float(resolution) - EPSILON;

			const float xp = node->x;
			node->x += vx;

			// Moved over X, check walls
			if(node->vx > 0)
			{
				const unsigned int gridxp = (unsigned int)((xp + node->width - EPSILON) / resolution);
				const unsigned int gridx = (unsigned int)((node->x + node->width) / resolution);

				if(gridxp != gridx)
				{
					const unsigned int yStart = (unsigned int)((node->y - node->height) / resolution);
					const unsigned int yEnd = (unsigned int)((node->y - EPSILON) / resolution);

					bool solid = false;

					for(unsigned int y = yStart; y <= yEnd; ++y)
					{
						if(getCell(gridx, y)->getWallType() == SOLID)
						{
							solid = true;

							break;
						}
					}

					if(solid)
					{
						node->vx = 0;
						node->x = float(gridx * resolution) - node->width;
					}
				}
			}
			else
			{
				const unsigned int gridxp = (unsigned int)((xp + EPSILON) / resolution) + 1;
				const unsigned int gridx = (unsigned int)(node->x / resolution) + 1;

				if(gridxp != gridx)
				{
					const unsigned int yStart = (unsigned int)((node->y - node->height) / resolution);
					const unsigned int yEnd = (unsigned int)((node->y - EPSILON) / resolution);

					bool solid = false;

					for(unsigned int y = yStart; y <= yEnd; ++y)
					{
						if(getCell(gridx, y)->getWallType() == SOLID)
						{
							solid = true;

							break;
						}
					}

					if(solid)
					{
						node->vx = 0;
						node->x = float(gridx * resolution);
					}
				}
			}

			// Check if no longer on ground
			if(node->flags & GridNode::Flags::ON_GROUND)
			{
				const unsigned int xStart = (unsigned int)(node->x / resolution);
				const unsigned int xEnd = (unsigned int)((node->x + node->width - EPSILON) / resolution);
				const unsigned int gridy = (unsigned int)(node->y / resolution);

				bool solid = false;

				for(unsigned int x = xStart; x <= xEnd; ++x)
				{
					if(getCell(x, gridy)->getFloorType() == SOLID)
					{
						solid = true;

						break;
					}
				}

				if(!solid)
					node->flags &= ~GridNode::Flags::ON_GROUND;
			}
		}

		// Move over Y, check floors
		if(node->vy != 0)
		{
			float vy = node->vy * timestep;

			if(vy < -float(resolution) + EPSILON) vy = -float(resolution) + EPSILON;
			if(vy > float(resolution) - EPSILON) vy = float(resolution) - EPSILON;

			const float yp = node->y;
			node->y += vy;

			if(node->vy > 0)
			{
				const unsigned int gridyp = (unsigned int)((yp - EPSILON) / resolution);
				const unsigned int gridy = (unsigned int)(node->y / resolution);

				if(gridyp != gridy)
				{
					const unsigned int xStart = (unsigned int)((node->x + EPSILON) / resolution);
					const unsigned int xEnd = (unsigned int)((node->x + node->width - EPSILON) / resolution);

					bool solid = false;

					for(unsigned int x = xStart; x <= xEnd; ++x)
					{
						if(getCell(x, gridy)->getFloorType() == SOLID)
						{
							solid = true;

							break;
						}
					}

					if(solid)
					{
						node->flags |= GridNode::Flags::ON_GROUND;
						node->vy = 0;
						node->y = float(gridy * resolution);
					}
				}
			}
			else
			{
				const unsigned int gridyp = (unsigned int)((yp - node->height + EPSILON) / resolution) + 1;
				const unsigned int gridy = (unsigned int)((node->y - node->height) / resolution) + 1;

				if(gridyp != gridy)
				{
					const unsigned int xStart = (unsigned int)((node->x + EPSILON) / resolution);
					const unsigned int xEnd = (unsigned int)((node->x + node->width - EPSILON) / resolution);

					bool solid = false;

					for(unsigned int x = xStart; x <= xEnd; ++x)
					{
						if(getCell(x, gridy)->getFloorType() == SOLID)
						{
							solid = true;

							break;
						}
					}

					if(solid)
					{
						node->vy = 0;
						node->y = float(gridy * resolution + node->height);
					}
				}
			}
		}
	}
}

void Grid::paintWall(const unsigned int x, const unsigned int y)
{
	getCell(x, y)->setWallType(SOLID);
}

void Grid::paintFloor(const unsigned int x, const unsigned int y)
{
	getCell(x, y)->setFloorType(SOLID);
}

Grid::Cell *Grid::getCell(const unsigned int x, const unsigned int y)
{
	return cells + x + y * width;
}

Grid::Cell::Cell()
{
	wall.type = NONE;
	floor.type = NONE;
}

Grid::BarrierType Grid::Cell::getWallType() const
{
	return wall.type;
}

Grid::BarrierType Grid::Cell::getFloorType() const
{
	return floor.type;
}

void Grid::Cell::setWallType(const Grid::BarrierType type)
{
	wall.type = type;
}

void Grid::Cell::setFloorType(const Grid::BarrierType type)
{
	floor.type = type;
}