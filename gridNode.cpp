#include "grid.hpp"
#include "gridNode.hpp"

GridNode::GridNode(
	const unsigned int width,
	const unsigned int height,
	const float x,
	const float y,
	const float vx,
	const float vy,
	const float mass,
	const float friction)
:flags(0), width(width), height(height), x(x), y(y), vx(vx), vy(vy), mass(mass), friction(friction), grid(nullptr)
{

}

unsigned int GridNode::getWidth() const
{
	return width;
}

unsigned int GridNode::getHeight() const
{
	return height;
}

float GridNode::getX() const
{
	return x;
}

float GridNode::getY() const
{
	return y;
}

float GridNode::getvx() const
{
	return vx;
}

float GridNode::getvy() const
{
	return vy;
}

bool GridNode::isAdded() const
{
	return grid != nullptr;
}

void GridNode::setvx(const float vx)
{
	this->vx = vx;
}

void GridNode::setvy(const float vy)
{
	this->vy = vy;

	if(vy != 0)
		flags &= ~ON_GROUND;
}

void GridNode::addvx(const float delta)
{
	setvx(getvx() + delta);
}

void GridNode::addvy(const float delta)
{
	setvy(getvy() + delta);
}

void GridNode::add(Grid *grid)
{
	this->grid = grid;
}

void GridNode::remove()
{
	this->grid = nullptr;
}