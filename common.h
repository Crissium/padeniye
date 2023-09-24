#ifndef COMMON_H_
#define COMMON_H_

#define NumBlocks 4

enum ShapeType
{
	//   *
	// * * *
	Tee,

	//   * *
	// * *
	CrawlerRight,

	// * *
	//   * *
	CrawlerLeft,

	// * * * *
	Bar,

	// * *
	// * *
	Box,

	// * * *
	// *
	Pistol,

	// * * *
	//     *
	Crank,

	NumShapeTypes
};

/**
 * Clockwise rotation scheme
 */
enum Direction
{
	Up,
	Right,
	Down,
	Left,

	NumDirections
};

enum Colour
{
	Blue,
	Cyan,
	Green,
	Magenta,
	Red,
	White,
	Yellow,

	NumColourTypes
};

/* The subscripts are, shape type, rotation direction, block order, dimension */
extern const int ShapesInfo[NumShapeTypes][NumDirections][NumBlocks][2];

#endif
