#ifndef GAME_H_
#define GAME_H_

#include "common.h"
#include <array>
#include <list>

enum
{
	AreaWidth = 14,
	AreaHeight = 20
};

class Block
{
public:
	int x;
	int y;

	Colour colour;

	Block() {}
	Block(int p, int q, Colour color = Red) : x(p), y(q), colour(color) {}
	Block(int p, int q, ShapeType type) : x(p), y(q), colour(Colour(type)) {}
	const Block &operator=(const Block &other);
	bool operator==(const Block &other) const { return (x == other.x && y == other.y); }
	~Block() {}
};

class Shape
{
public:
	/* the central block's location */
	int x;
	int y;

	ShapeType type;
	Direction rotationStatus;

	std::array<Block, NumBlocks> blocks;

	// Shape() {}
	Shape(int p = AreaWidth / 2, int q = 0, ShapeType shapeType = Tee, Direction dir = Up);
	const Shape &operator=(const Shape &other);
	~Shape() {}
};

class Game
{
private:
	enum
	{
		DifficultThreshold = 30,
		HellThreshold = 50
	};

public:
	Shape nextShape;

public: // I hope this does not violate the spirit of OOP
	Shape currentShape;
	std::list<Block> placedBlocks;

private:
	int numLinesDestroyed;
	unsigned long score;

	bool started;
	bool paused;
	bool gameOver;

	bool difficult;
	bool hellMode;

	/** Check for a single coordinate pair. */
	bool coordInvalid(int x, int y) const;
	/** Check for a whole new shape. */
	bool coordInvalid(int x, int y, ShapeType, Direction) const;

	inline const Block blockAtBottomOf(int x, int y) const;

	inline Direction pickRandomDirection() const;

public:
	ShapeType pickRandomShapeType() const;

private:
	ShapeType pickDifficultShapeType() const;

	inline const Shape makeNewShape(int type, int rotation, int pos) const;
	inline const Shape makeNewShape(ShapeType, Direction) const;
	inline void makeNextShape();

	void pushNextShapeToCurrent();

	/**
	 * Add blocks of dead current shape to the list
	 * And update the lines to check
	 */
	void archiveCurrentShape();

	/**
	 * Check if the given line is complete
	 */
	bool lineComplete(int y) const;

	bool nearGameOver() const;

	void destroyLine(int y);

	/**
	 * Check given line numbers;
	 * Destroy each found.
	 */
	void checkForCompleteLines();

public:
	/**
	 * Default constructor,
	 * Display 'splash' on game start.
	 * That is, fill the game drawing area with random blocks
	 * and blank spaces.
	 */
	Game();

	Game(const Game &other);

	~Game() {}

	void start();

	void pause() { paused = true; }
	void proceed() { paused = false; }
	bool isPaused() const { return paused; }
	bool isOver() const { return gameOver; }
	const unsigned long &getScore() const { return score; }
	const ShapeType &getNextShapeType() const { return nextShape.type; }
	const int &getNumLinesDestroyed() const { return numLinesDestroyed; }
	char getDifficultyLevel() const;
	void updateScore(unsigned long newScore) { score = newScore; }
	void updateNumLinesDestroyed(int n);
	void updateCurrentShape(const Shape &newShape) { currentShape = newShape; }
	void updateNextShape(const Shape &newShape) { nextShape = newShape; }

	/**
	 * Change current shape's rotation status or location
	 * True on success.
	 */
	bool turnShape(Direction);

	/**
	 * Make current shape fall by one unit.
	 * False on reaching dead end,
	 * And archive current shape
	 *
	 * Potential main loop
	 */
	bool fall();

	void drop();
	void autoDrop();

	int *getShadowLocation() const;
	Colour getCurrentShapeColour() const { return Colour(currentShape.type); };
};

#endif
