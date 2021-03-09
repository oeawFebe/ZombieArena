#include <SFML/Graphics.hpp>
#include "Player.h"
using namespace sf;
int main()
{
	enum class State {PAUSED,LEVELING_UP,GAME_OVER,PLAYING};
	State state = State::GAME_OVER;
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;
	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));
	Clock clock;
	Time gameTimeTotal;
	Vector2f mouseWorldPosition;
	Vector2i mouseScreenPosition;
	Player player;
	IntRect arena;
	while (window.isOpen())
	{
		/*
		************
		Handle input
		************
		*/
		//handle events by polling
		Event event;
		/*Bool. This function is not blocking: if there's no pending event then it will return false and leave event unmodified. Note that more than one event may be present in the event queue, thus you should always call this function in a loop to make sure that you process every pending event. */
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Return && state == State::PLAYING) { state = State::PAUSED; }
				else if (event.key.code == Keyboard::Return && state == State::PAUSED) { state = State::PLAYING; clock.restart(); }
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) { state = State::LEVELING_UP; }
				if (state == State::PLAYING) { ; }

			}
		}// End even polling

		//Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { window.close(); }
		// Handle WASD while playing
		if (state == State::PLAYING)
		{
			if (Keyboard::isKeyPressed(Keyboard::W)) { player.moveUp(); }
			else { player.stopUp(); }
			if (Keyboard::isKeyPressed(Keyboard::S)) { player.moveDown(); }
			else { player.stopDown(); }
			if (Keyboard::isKeyPressed(Keyboard::D)) { player.moveRight(); }
			else { player.stopRight(); }
			if (Keyboard::isKeyPressed(Keyboard::A)) { player.moveLeft(); }
			else { player.stopLeft(); }
		}// End WASD while playing
		//Handle LEVELING_UP state
		if (state == State::LEVELING_UP)
		{
			//Handle the player LEVELING UP
			if (event.key.code == Keyboard::Num1) { state = State::PLAYING; }
			if (event.key.code == Keyboard::Num2) { state = State::PLAYING; }
			if (event.key.code == Keyboard::Num3) { state = State::PLAYING; }
			if (event.key.code == Keyboard::Num4) { state = State::PLAYING; }
			if (event.key.code == Keyboard::Num5) { state = State::PLAYING; }
			if (event.key.code == Keyboard::Num6) { state = State::PLAYING; }
			if (state == State::PLAYING)
			{
				//prepare the level
				//modify later
				arena.width = 500;
				arena.height = 500;
				arena.left = 0;
				arena.top = 0;
				//modify later
				int tileSize = 50;
				//spawn player in the middle of arena
				player.spawn(arena, resolution, tileSize);
				clock.restart();
			}
		}
			/*
		************
		Updating the fram
		************
		*/
		if (state == State::PLAYING)
		{
			//Updating delta time
			Time dt = clock.restart();
			gameTimeTotal += dt;
			float dtAsSeconds = dt.asSeconds();
			mouseScreenPosition = Mouse::getPosition();
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);
			player.update(dtAsSeconds, Mouse::getPosition());
			Vector2f playerPosition(player.getCenter());
			mainView.setCenter(player.getCenter());
		}// End Updating the scene

		/*
		****************
		Draw the scene
		****************
		*/
		if (state == State::PLAYING)
		{
			window.clear();
			//set mainView to be displayed
			window.setView(mainView);
			//Draw player
			window.draw(player.getSprite());

		}
		if (state == State::LEVELING_UP)
		{

		}
		if (state == State::PAUSED)
		{

		}
		if (state == State::GAME_OVER)
		{

		}
		window.display();



	}//End  loop
	return 0;
}
