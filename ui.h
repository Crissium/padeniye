#ifndef UI_H_
#define UI_H_

#include "game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <ctime>

#define PixelsPerUnit 40.f

enum
{
	GameAreaWidth = int(AreaWidth * PixelsPerUnit),
	GameAreaHeight = int(AreaHeight * PixelsPerUnit),

	BorderWidth = int(2 * PixelsPerUnit),
	ForecastAreaWidth = int(BorderWidth + NumBlocks * PixelsPerUnit + BorderWidth),

	TextHeight = (GameAreaHeight - ForecastAreaWidth) / 2,

	WindowWidth = GameAreaWidth + ForecastAreaWidth,
	WindowHeight = GameAreaHeight
};

class Square : public sf::RectangleShape
{
public:
	Square() {}
	Square(const Block &);

	void makeShadowy(int x, int y, Colour colour);

	~Square() {}
};

class NextShape
{
private:
	enum
	{
		Border = 2,
		BaseX = AreaWidth + Border,
		BaseY = Border + int(TextHeight / PixelsPerUnit)
	};

public:
	std::array<Square, NumBlocks> components;

	NextShape() {}
	NextShape(ShapeType);
	~NextShape() {}
};

class Text : public sf::Text
{
private:
	sf::Font font;

public:
	Text();
	virtual ~Text() {}
};

class Score : public Text
{
private:
	std::string scoreString;

public:
	Score();
	void update(unsigned long);
	virtual ~Score() {}
};

class Title : public Text
{
public:
	Title();
	void updateIfDifficult();
	void updateInHell();
	void updateAfterGameover();
	void updateAfterRestart();
	virtual ~Title() {}
};

// class Music : public sf::Music
// {
// public:
// 	Music();
// 	~Music() {}
// };

class Padeniye
{
private:
	Game game;

	std::list<Square> squareList;
	NextShape nextShape;
	Square shadow[4];
	bool drawShadowOn;
	Score score;
	Title title;

	sf::RenderWindow window;

	bool initialising;
	bool shotOver;

	time_t timeStarted;

	int timesShapeHasFallen;

	// Music clavier;
	bool musicPlaying;
	// void togglePlayMusic() { (musicPlaying = !musicPlaying) ? clavier.play() : clavier.stop(); }

	void toggleDrawShadow() { drawShadowOn = !drawShadowOn; }

	void syncRenderedObjects();

	void start();
	void fall(); // the main loop

	void processEvents();
	void render();

	void saveGame() const;
	void restoreGame();

public:
	Padeniye();
	~Padeniye() { squareList.clear(); }

	void run();
};

#endif
