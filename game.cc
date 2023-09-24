#include "game.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <climits>
#include <vector>

const Block &Block::operator=(const Block &other)
{

	x = other.x;
	y = other.y;
	colour = other.colour;

	return *this;
}

Shape::Shape(int p, int q, ShapeType shapeType, Direction dir) : x(p), y(q), type(shapeType), rotationStatus(dir)
{
	blocks[0] = Block(p, q, Colour(shapeType));

	for (int i = 1; i < NumBlocks; ++i)
	{
		blocks[i] = Block(p + ShapesInfo[shapeType][dir][i][0], q + ShapesInfo[shapeType][dir][i][1], Colour(shapeType));
	}
}

const Shape &Shape::operator=(const Shape &other)
{
	x = other.x;
	y = other.y;
	type = other.type;
	rotationStatus = other.rotationStatus;
	blocks = other.blocks;
	return *this;
}

Game::Game() : numLinesDestroyed(0), score(0UL), started(false), paused(false), difficult(false), hellMode(false), gameOver(false)
{
	srand(time(nullptr));

	// Fill the placed-block list
	for (int i = 0; i < AreaWidth; ++i)
	{
		for (int j = 0; j < AreaHeight; ++j)
		{
			int type = rand() % (NumShapeTypes + 1); // a particular type of shape, or blank space

			if (type != NumShapeTypes)
			{
				placedBlocks.push_back({i, j, Colour(type)});
			}
		}
	}
}

bool Game::coordInvalid(int x, int y) const
{
	if (x < 0 || x >= AreaWidth || y >= AreaHeight)
	{
		return true;
	}
	else
	{
		Block toFind(x, y);
		return (std::find(placedBlocks.begin(), placedBlocks.end(), toFind) != placedBlocks.end());
	}
}

bool Game::coordInvalid(int x, int y, ShapeType type, Direction dir) const
{
	if (coordInvalid(x, y))
		return true;
	else if (coordInvalid(x + ShapesInfo[type][dir][1][0], y + ShapesInfo[type][dir][1][1]))
		return true;
	else if (coordInvalid(x + ShapesInfo[type][dir][2][0], y + ShapesInfo[type][dir][2][1]))
		return true;
	else if (coordInvalid(x + ShapesInfo[type][dir][3][0], y + ShapesInfo[type][dir][3][1]))
		return true;
	else
		return false;
}

const Block Game::blockAtBottomOf(int x, int y) const
{
	auto it = std::find(placedBlocks.begin(), placedBlocks.end(), Block(x, y));

	return Block(x, y + 1, it->colour);
}

const Shape Game::makeNewShape(int type, int rotation, int pos) const
{
	return Shape(pos, 0, ShapeType(type), Direction(rotation));
}

const Shape Game::makeNewShape(ShapeType type, Direction dir) const
{
	return Shape(AreaWidth / 2, 0, type, dir);
}

ShapeType Game::pickRandomShapeType() const
{
	return ShapeType(rand() % NumShapeTypes);
}

ShapeType Game::pickDifficultShapeType() const
{
	// Courtesy of Federico Poloni

	if (nearGameOver())
		return pickRandomShapeType();

	std::array<int, NumShapeTypes> metrics;

	for (int shapeType = 0; shapeType < NumShapeTypes; ++shapeType)
	{
		metrics[shapeType] = -32000;

		for (int rotation = 0; rotation < NumDirections; ++rotation)
		{
			for (int pos = 0; pos < AreaWidth; ++pos)
			{
				Game phoney(*this);

				if (coordInvalid(pos, 0, ShapeType(shapeType), Direction(rotation)))
				{
					continue;
				}
				phoney.currentShape = makeNewShape(shapeType, rotation, pos);

				auto origNumLinesDestroyed = phoney.numLinesDestroyed;
				phoney.drop();

				// High metric for each line destroyed
				int metric = 5000 * (phoney.numLinesDestroyed - origNumLinesDestroyed);

				// Low metric for large columns
				for (int x = 0; x < AreaWidth; ++x)
				{
					int y;
					for (y = 0; y < AreaHeight; ++y)
					{
						if (phoney.coordInvalid(x, y))
							break; // Stop upon finding a block
					}

					metric -= 5 * (AreaHeight - y);
				}

				if (metric > metrics[shapeType])
					metrics[shapeType] = metric;
			}
		}
	}

	// Perturb the metrics a little so as to avoid tie handling, awful!
	for (auto &it : metrics)
	{
		it += rand() % 11 - 5;
	}

	if (hellMode)
	{
		int mostDifficultType = 0;

		for (int i = 1; i < NumShapeTypes; ++i)
		{
			if (metrics[i] < metrics[mostDifficultType])
				mostDifficultType = i;
		}

		return ShapeType(mostDifficultType);
	}
	else
	{
		int easiestType = 0;

		for (int i = 1; i < NumShapeTypes; ++i)
		{
			if (metrics[i] > metrics[easiestType])
				easiestType = i;
		}

		int selectedType = rand() % NumShapeTypes;
		while (selectedType == easiestType)
			selectedType = rand() % NumShapeTypes;

		return ShapeType(selectedType);
	}
}

Direction Game::pickRandomDirection() const
{
	return Direction(rand() % NumDirections);
}

void Game::makeNextShape()
{
	if (difficult)
		nextShape = makeNewShape(pickDifficultShapeType(), pickRandomDirection());
	else
		nextShape = makeNewShape(pickRandomShapeType(), pickRandomDirection());
}

void Game::pushNextShapeToCurrent()
{
	currentShape = nextShape;

	makeNextShape();

	// if the new shape cannot fall down, game over
	if (coordInvalid(currentShape.x, currentShape.y + 1, currentShape.type, currentShape.rotationStatus))
		gameOver = true;
}

bool Game::turnShape(Direction dir)
{

	switch (dir)
	{
	case Up:
	{
		// Rotate current shape clockwise

		Direction nextDirection = Direction((currentShape.rotationStatus + 1) % NumDirections);

		// Check new coordinates
		if (coordInvalid(currentShape.x, currentShape.y, currentShape.type, nextDirection))
			return false;
		else
		{
			// Do the final rotation
			currentShape.rotationStatus = nextDirection;
			for (int i = 1; i < NumBlocks; ++i)
			{
				currentShape.blocks[i].x = ShapesInfo[currentShape.type][nextDirection][i][0];
				currentShape.blocks[i].y = ShapesInfo[currentShape.type][nextDirection][i][1];
			}
			return true;
		}
	}
	break;
	case Left:
	{
		int newX = currentShape.x - 1;
		if (coordInvalid(newX, currentShape.y, currentShape.type, currentShape.rotationStatus))
			return false;
		else
		{
			currentShape.x = newX;
			return true;
		}
	}
	break;
	case Right:
	{
		int newX = currentShape.x + 1;
		if (coordInvalid(newX, currentShape.y, currentShape.type, currentShape.rotationStatus))
			return false;
		else
		{
			currentShape.x = newX;
			return true;
		}
	}
	break;
	case Down:
	{
		int newY = currentShape.y + 1;
		if (coordInvalid(currentShape.x, newY, currentShape.type, currentShape.rotationStatus))
			return false;
		else
		{
			currentShape.y = newY;
			return true;
		}
	}
	break;
	default:
		break;
	}
	return true;
}

void Game::archiveCurrentShape()
{
	for (int i = 0; i < NumBlocks; ++i)
	{
		placedBlocks.push_back(Block(currentShape.x + ShapesInfo[currentShape.type][currentShape.rotationStatus][i][0], currentShape.y + ShapesInfo[currentShape.type][currentShape.rotationStatus][i][1], currentShape.type));
	}
}

bool Game::fall()
{
	if (turnShape(Down))
		return true;
	else if (gameOver)
	{
		archiveCurrentShape();
		return false;
	}
	else
	{
		archiveCurrentShape();
		checkForCompleteLines();
		pushNextShapeToCurrent();
		return false;
	}
}

void Game::drop()
{
	while (fall())
		;
}

bool Game::lineComplete(int y) const
{
	for (int x = 0; x < AreaWidth; ++x)
	{
		if (!coordInvalid(x, y)) // if the coordinate is unoccupied, in this context
			return false;
	}

	return true;
}

bool Game::nearGameOver() const
{
	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < AreaWidth; ++x)
		{
			if (coordInvalid(x, y)) // if the coordinate is occupied, in this context
				return true;
		}
	}
	return false;
}

void Game::destroyLine(int y)
{
	++numLinesDestroyed;
	if (numLinesDestroyed > DifficultThreshold)
	{
		if (!difficult)
		{
			difficult = true;
		}

		if (!hellMode && numLinesDestroyed > HellThreshold)
			hellMode = true;
	}

	if (y == AreaHeight - 1)
		score += 50UL * numLinesDestroyed; // Bonus if bottom reached
	else
		score += 10UL * numLinesDestroyed;

	// First, remove the line
	for (int x = 0; x < AreaWidth; ++x)
	{
		placedBlocks.remove({x, y});
	}

	// Next, make each block above fall
	for (int j = y - 1; j >= 0; --j)
	{
		for (int i = 0; i < AreaWidth; ++i)
		{
			if (coordInvalid(i, j)) // if there is a block at the given coordinate
			{

				placedBlocks.push_back(blockAtBottomOf(i, j));
				placedBlocks.remove({i, j});
			}
		}
	}
}

void Game::checkForCompleteLines()
{
	for (int y = AreaHeight - 1; y >= 0; --y)
	{
		if (lineComplete(y))
		{
			destroyLine(y);
			++y; // line number increased!
		}
	}
}

Game::Game(const Game &other) : currentShape(other.currentShape), placedBlocks(other.placedBlocks), numLinesDestroyed(other.numLinesDestroyed), started(true), paused(false), gameOver(false), difficult(false)
{
}

char Game::getDifficultyLevel() const
{
	if (!difficult)
		return 'e';
	else if (hellMode)
		return 'h';
	else
		return 'd';
}

void Game::updateNumLinesDestroyed(int n)
{
	numLinesDestroyed = n;

	if (n > HellThreshold)
	{
		hellMode = true;
	}
	else if (n > DifficultThreshold)
	{
		difficult = true;
	}
}

void Game::start()
{
	numLinesDestroyed = 0;
	score = 0UL;
	started = true;
	paused = false;
	difficult = false;
	hellMode = false;
	gameOver = false;

	placedBlocks.clear();

	pushNextShapeToCurrent();
}

int *Game::getShadowLocation() const
{
	int y = currentShape.y + 1;

	while (!coordInvalid(currentShape.x, y, currentShape.type, currentShape.rotationStatus))
	{
		++y;
	}

	--y; // last valid location

	auto loc = new int[2 * NumBlocks];

	for (int i = 0; i < NumBlocks; ++i)
	{
		// x
		loc[i] = currentShape.x + ShapesInfo[currentShape.type][currentShape.rotationStatus][i][0];

		// y
		loc[i + NumBlocks] = y + ShapesInfo[currentShape.type][currentShape.rotationStatus][i][1];
	}

	return loc;
}

void Game::autoDrop()
{
	// Inspired by Quadrapassel's pick_difficult_shapes function, which was in turn replicated from Federico Poloni's 'bastet' game,
	// With some enhancements.

	int metrics[AreaWidth][NumDirections]{0};

	for (int pos = 0; pos < AreaWidth; ++pos)
	{
		for (int rotation = 0; rotation < NumDirections; ++rotation)
		{
			if (coordInvalid(pos, currentShape.y, currentShape.type, Direction(rotation)))
			{
				metrics[pos][rotation] = INT_MIN;
				continue;
			}

			Game phoney(*this);

			phoney.currentShape.x = pos;
			phoney.currentShape.rotationStatus = Direction(rotation);

			auto origNumLinesDestroyed = phoney.numLinesDestroyed;
			phoney.drop();

			// High metric for each line destroyed
			int metric = 5000 * (phoney.numLinesDestroyed - origNumLinesDestroyed);

			std::vector<int> modifiedXs;
			modifiedXs.push_back(pos);
			for (int i = 1; i < NumBlocks; ++i)
			{
				if (std::find(modifiedXs.begin(), modifiedXs.end(), pos + ShapesInfo[currentShape.type][rotation][i][0]) == modifiedXs.end())
					modifiedXs.push_back(pos + ShapesInfo[currentShape.type][rotation][i][0]);
			}

			// High metric for spreading out
			metric += 200 * modifiedXs.size();

			int columnHeight[modifiedXs.size()];

			// Low metric for large columns
			for (size_t i = 0; i < modifiedXs.size(); ++i)
			{
				int y;
				for (y = 0; y < AreaHeight; ++y)
				{
					if (phoney.coordInvalid(modifiedXs[i], y))
						break; // Stop upon finding a block
				}
				columnHeight[i] = y;
				metric -= 5 * (AreaHeight - y);
			}

			// low metric for holes
			bool noHoleFound = true;
			for (size_t i = 0; i < modifiedXs.size(); ++i)
			{
				for (int y = AreaHeight - 1; y > columnHeight[i]; --y)
				{
					if (!phoney.coordInvalid(modifiedXs[i], y)) // if there is a hole
					{
						metric -= 100;
						noHoleFound = false;
					}
				}
			}

			// low metric for blocks piled too high
			for (auto height : columnHeight)
			{
				metric -= 30 * (AreaHeight - height);
			}

			if (noHoleFound)
				metric += 100;

			metrics[pos][rotation] = metric;
		}
	}

	int bestX = 0;
	int bestDirection = 0;

	for (int pos = 0; pos < AreaWidth; ++pos)
	{
		for (int rotation = 0; rotation < NumDirections; ++rotation)
		{
			if (metrics[pos][rotation] > metrics[bestX][bestDirection])
			{
				bestX = pos;
				bestDirection = rotation;
			}
		}
	}

	currentShape.x = bestX;
	currentShape.rotationStatus = Direction(bestDirection);

	drop();
}