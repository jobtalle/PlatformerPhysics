#pragma once

class Grid;

class GridNode final
{
	friend class Grid;

public:
	GridNode(
		const unsigned int width,
		const unsigned int height,
		const float x = 0,
		const float y = 0,
		const float vx = 0,
		const float vy = 0,
		const float mass = 80,
		const float friction = 0.92f);
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	float getX() const;
	float getY() const;
	float getvx() const;
	float getvy() const;
	bool isAdded() const;
	void setvx(const float vx);
	void setvy(const float vy);
	void addvx(const float delta);
	void addvy(const float delta);

private:
	enum Flags
	{
		ON_GROUND = 0x01
	};

	void add(Grid *grid);
	void remove();

	Grid *grid;
	unsigned char flags;
	unsigned int width, height;
	float mass;
	float friction;
	float x, y;
	float vx, vy;
};