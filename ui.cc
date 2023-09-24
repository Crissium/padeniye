#include "ui.h"
#include <fstream>

Square::Square(const Block &block)
{
	setSize({PixelsPerUnit, PixelsPerUnit});
	setPosition(block.x * PixelsPerUnit, block.y * PixelsPerUnit);

	setOutlineThickness(1.f);
	setOutlineColor(sf::Color(192, 192, 192));

	switch (block.colour)
	{
	case Blue:
		setFillColor(sf::Color::Blue);
		break;
	case Cyan:
		setFillColor(sf::Color::Cyan);
		break;
	case Green:
		setFillColor(sf::Color::Green);
		break;
	case Magenta:
		setFillColor(sf::Color::Magenta);
		break;
	case Red:
		setFillColor(sf::Color::Red);
		break;
	case White:
		setFillColor(sf::Color::White);
		break;
	case Yellow:
		setFillColor(sf::Color::Yellow);
		break;
	default:
		break;
	}
}

void Square::makeShadowy(int x, int y, Colour colour)
{
	setSize({PixelsPerUnit, PixelsPerUnit});
	setPosition(x * PixelsPerUnit, y * PixelsPerUnit);

	switch (colour)
	{
	case Blue:
		setFillColor(sf::Color(0, 0, 255, 64));
		break;
	case Cyan:
		setFillColor(sf::Color(0, 255, 255, 64));
		break;
	case Green:
		setFillColor(sf::Color(0, 255, 0, 64));
		break;
	case Magenta:
		setFillColor(sf::Color(255, 0, 255, 64));
		break;
	case Red:
		setFillColor(sf::Color(255, 0, 0, 64));
		break;
	case White:
		setFillColor(sf::Color(192, 192, 192, 64));
		break;
	case Yellow:
		setFillColor(sf::Color(255, 255, 0, 64));
		break;
	default:
		break;
	}
}

NextShape::NextShape(ShapeType type)
{
	switch (type)
	{
	case Tee:
		components[0] = Square({BaseX + 2, BaseY + 2, Blue});
		components[1] = Square({BaseX + 1, BaseY + 3, Blue});
		components[2] = Square({BaseX + 2, BaseY + 3, Blue});
		components[3] = Square({BaseX + 3, BaseY + 3, Blue});
		break;
	case CrawlerRight:
		components[0] = Square({BaseX + 2, BaseY + 2, Cyan});
		components[1] = Square({BaseX + 2, BaseY + 3, Cyan});
		components[2] = Square({BaseX + 1, BaseY + 3, Cyan});
		components[3] = Square({BaseX + 3, BaseY + 2, Cyan});
		break;
	case CrawlerLeft:
		components[0] = Square({BaseX + 1, BaseY + 2, Green});
		components[1] = Square({BaseX + 2, BaseY + 2, Green});
		components[2] = Square({BaseX + 2, BaseY + 3, Green});
		components[3] = Square({BaseX + 3, BaseY + 3, Green});
		break;
	case Bar:
		components[0] = Square({BaseX + 1, BaseY + 2, Magenta});
		components[1] = Square({BaseX + 2, BaseY + 2, Magenta});
		components[2] = Square({BaseX + 3, BaseY + 2, Magenta});
		components[3] = Square({BaseX + 4, BaseY + 2, Magenta});
		break;
	case Box:
		components[0] = Square({BaseX + 2, BaseY + 2, Red});
		components[1] = Square({BaseX + 2, BaseY + 3, Red});
		components[2] = Square({BaseX + 3, BaseY + 2, Red});
		components[3] = Square({BaseX + 3, BaseY + 3, Red});
		break;
	case Pistol:
		components[0] = Square({BaseX + 1, BaseY + 2, White});
		components[1] = Square({BaseX + 2, BaseY + 2, White});
		components[2] = Square({BaseX + 3, BaseY + 2, White});
		components[3] = Square({BaseX + 1, BaseY + 3, White});
		break;
	case Crank:
		components[0] = Square({BaseX + 1, BaseY + 2, Yellow});
		components[1] = Square({BaseX + 2, BaseY + 2, Yellow});
		components[2] = Square({BaseX + 3, BaseY + 2, Yellow});
		components[3] = Square({BaseX + 3, BaseY + 3, Yellow});
		break;
	default:
		break;
	}
}

Text::Text()
{
	font.loadFromFile("NotoSerif-Regular.ttf");

	setFont(font);
	setCharacterSize(30);
	setFillColor(sf::Color::White);
}

Score::Score() : scoreString("Score: 0")
{
	setString(scoreString);
	setPosition(GameAreaWidth + PixelsPerUnit, ForecastAreaWidth + TextHeight);
}

void Score::update(unsigned long newScore)
{
	scoreString = "Score: " + std::to_string(newScore);

	setString(scoreString);
}

Title::Title()
{
	setString(L"падение");
	setPosition(GameAreaWidth + 2.5f * PixelsPerUnit, BorderWidth);
}

void Title::updateIfDifficult()
{
	setCharacterSize(20);
	setString(L"Стойте на своем, товарищ!");
	setPosition(GameAreaWidth + 0.6f * PixelsPerUnit, BorderWidth);
}

void Title::updateInHell()
{
	setCharacterSize(25);
	setPosition(GameAreaWidth + PixelsPerUnit * 0.5f, BorderWidth);
}

void Title::updateAfterGameover()
{
	setPosition(GameAreaWidth + PixelsPerUnit * 0.5f, BorderWidth);
	setFillColor(sf::Color::Red);
	setCharacterSize(18);
}

void Title::updateAfterRestart()
{
	setString(L"падение");
	setFillColor(sf::Color::White);
	setCharacterSize(30);
	setPosition(GameAreaWidth + 2.5f * PixelsPerUnit, BorderWidth);
}

// Music::Music()
// {
// 	openFromFile("/run/media/ellis/Data/Audio/Music/Classical/Bach/Bach 333 (2018)/154 - The Well-Tempered Clavier, Book I (1 Pollini)/Bach 333 - J.S. Bach The New Complete Edition - The Well-Tempered Clavier, Book I (1 Pollini).flac");
// }

void Padeniye::syncRenderedObjects()
{
	squareList.clear();

	for (auto const &it : game.placedBlocks)
	{
		squareList.emplace_back(it);
	}

	squareList.push_back(Square(Block(game.currentShape.x, game.currentShape.y, game.currentShape.type)));

	if (initialising)
	{
		nextShape = NextShape(game.pickRandomShapeType());
	}
	else
	{
		nextShape = NextShape(game.getNextShapeType());
	}

	if (!initialising)
	{
		score.update(game.getScore());

		for (int i = 1; i < NumBlocks; ++i)
		{
			squareList.push_back(Square(Block(game.currentShape.x + ShapesInfo[game.currentShape.type][game.currentShape.rotationStatus][i][0], game.currentShape.y + ShapesInfo[game.currentShape.type][game.currentShape.rotationStatus][i][1], game.currentShape.type)));
		}

		auto loc = game.getShadowLocation();
		for (int i = 0; i < NumBlocks; ++i)
		{
			shadow[i].makeShadowy(loc[i], loc[i + NumBlocks], game.getCurrentShapeColour());
		}
		delete loc;

		switch (game.getDifficultyLevel())
		{
		case 'd':
			title.updateIfDifficult();
			break;
		case 'h':
			title.updateInHell();
			break;
		default:
			break;
		}
	}
}

void Padeniye::start()
{
	initialising = false;
	shotOver = false;
	title.updateAfterRestart();

	timesShapeHasFallen = 0;
	timeStarted = time(nullptr);

	game.start();
}

void Padeniye::fall()
{
	game.fall();
	syncRenderedObjects();
}

Padeniye::Padeniye() : drawShadowOn(true), window(sf::VideoMode(WindowWidth, WindowHeight), "Padeniye", sf::Style::Close | sf::Style::Titlebar), initialising(true), musicPlaying(false)
{
	syncRenderedObjects();
}

void Padeniye::processEvents()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::KeyPressed:
			if (game.isOver())
			{
				if (event.key.code == sf::Keyboard::R)
				{
					start();
					syncRenderedObjects();
				}
			}
			else if (game.isPaused())
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Q:
					saveGame();
					window.close();
					break;
				case sf::Keyboard::E:
					restoreGame();
					syncRenderedObjects();
					timeStarted = time(nullptr);
					timesShapeHasFallen = 1;
					break;
				case sf::Keyboard::S:
					toggleDrawShadow();
					break;
				// case sf::Keyboard::M:
				// 	togglePlayMusic();
				// 	break;
				case sf::Keyboard::R:
					start();
					syncRenderedObjects();
					break;
				case sf::Keyboard::Numpad0:
					timesShapeHasFallen = 1;
					timeStarted = time(nullptr);
					game.proceed();
					break;
				default:
					break;
				}
			}
			else
				switch (event.key.code)
				{
				case sf::Keyboard::Q:
					saveGame();
					window.close();
					break;
				case sf::Keyboard::E:
					restoreGame();
					syncRenderedObjects();
					timeStarted = time(nullptr);
					timesShapeHasFallen = 1;
					break;
				case sf::Keyboard::S:
					toggleDrawShadow();
					break;
				// case sf::Keyboard::M:
				// 	togglePlayMusic();
				// 	break;
				case sf::Keyboard::R:
					start();
					syncRenderedObjects();
					break;
				case sf::Keyboard::Numpad0:
					game.pause();
					syncRenderedObjects();
					break;
				case sf::Keyboard::A:
					game.autoDrop();
					syncRenderedObjects();
					break;
				case sf::Keyboard::Up:
					game.turnShape(Up);
					syncRenderedObjects();
					break;
				case sf::Keyboard::Left:
					game.turnShape(Left);
					syncRenderedObjects();
					break;
				case sf::Keyboard::Right:
					game.turnShape(Right);
					syncRenderedObjects();
					break;
				case sf::Keyboard::Down:
					game.turnShape(Down);
					syncRenderedObjects();
					break;
				case sf::Keyboard::Space:
					game.drop();
					syncRenderedObjects();
					break;
				default:
					break;
				}
			break;
		default:
			break;
		}
	}
}

void Padeniye::render()
{
	for (auto const &it : squareList)
	{
		window.draw(it);
	}

	if (!initialising && drawShadowOn)
	{
		for (auto const &it : shadow)
		{
			window.draw(it);
		}
	}

	for (auto const &it : nextShape.components)
	{
		window.draw(it);
	}

	window.draw(title);
	window.draw(score);

	sf::Vertex separator[] = {
		sf::Vertex(sf::Vector2f(GameAreaWidth, 0.f)),
		sf::Vertex(sf::Vector2f(GameAreaWidth, WindowHeight))};

	window.draw(separator, 2, sf::Lines);
}

void Padeniye::run()
{
	while (window.isOpen())
	{
		processEvents();

		if (!initialising && !game.isPaused() && !game.isOver() && time(nullptr) - timeStarted >= timesShapeHasFallen * (1.f - 0.01f * game.getNumLinesDestroyed()))
		{
			fall();
			++timesShapeHasFallen;
		}

		if (game.isOver() && !shotOver)
		{
			title.updateAfterGameover();
			shotOver = true;
		}

		window.clear(sf::Color::Black);

		render();

		window.display();
	}
}

void Padeniye::saveGame() const
{
	std::ofstream fout("SavedGame", std::ios::binary | std::ios::out);

	fout.write((char *)&(game.nextShape), sizeof(Shape));
	fout.write((char *)&(game.currentShape), sizeof(Shape));
	fout.write((char *)&(game.getNumLinesDestroyed()), sizeof(int));
	fout.write((char *)&(game.getScore()), sizeof(unsigned long));

	const size_t numBlocksWritten = game.placedBlocks.size();
	fout.write((char *)&(numBlocksWritten), sizeof(size_t));

	for (auto it = game.placedBlocks.begin(); it != game.placedBlocks.end(); ++it)
	{
		fout.write((char *)&(*it), sizeof(Block));
	}

	fout.close();
}

void Padeniye::restoreGame()
{
	std::ifstream fin("SavedGame", std::ios::binary | std::ios::in);

	Shape current, next; // omitted 'Shape' in order to avoid a name clash
	fin.read((char *)&next, sizeof(Shape));
	game.updateNextShape(next);
	fin.read((char *)&current, sizeof(Shape));
	game.updateCurrentShape(current);

	int numLinesDestroyed = 0;
	fin.read((char *)&numLinesDestroyed, sizeof(int));
	game.updateNumLinesDestroyed(numLinesDestroyed);

	unsigned long score = 0UL;
	fin.read((char *)&score, sizeof(unsigned long));
	game.updateScore(score);

	size_t numBlocksWritten = 0;
	fin.read((char *)&numBlocksWritten, sizeof(size_t));

	game.placedBlocks.clear();
	for (size_t i = 0; i < numBlocksWritten; ++i)
	{
		Block blockRead;
		fin.read((char *)&blockRead, sizeof(Block));
		game.placedBlocks.push_back(blockRead);
	}

	fin.close();
}