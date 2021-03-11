#include <SFML/Graphics.hpp>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
using namespace sf;
int main()
{
	//here is the instance of textureholder
	TextureHolder holder;
	enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };
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
	//create background
	VertexArray background;
	//load the texture for our background vertex arrey
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");
	//prepare for hoarde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;
	// 100 bullets should do
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1.0;
	Time lastPressed;
	//mouse pointer to crosshair
	window.setMouseCursorVisible(false);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);
	//create a few pickups
	Pickup healthPickup(1);
	Pickup ammoPickup(2);
	//The main game loop
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
				if (state == State::PLAYING)
				{
					//Reloading
					if (event.key.code == Keyboard::R)
					{
						if (bulletsSpare >= clipSize)
						{
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
						}
						else if (bulletsSpare > 0)
						{
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
						}
						else
						{
							;//soundcoming
						}
					}
				}

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
			if (Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0)
				{
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
					currentBullet++;
					if (currentBullet > 99)
					{
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;
					bulletsInClip--;
				}
			}
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
				//pass the vertex array by reference
				int tileSize = createBackground(background, arena);
				//spawn player in the middle of arena
				player.spawn(arena, resolution, tileSize);
				//configure pickups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);
				healthPickup.spawn();//?
				ammoPickup.spawn();//?
				//create horde of zombies
				numZombies = 10;
				//delete priviously allocated memory (if it exists)
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;
				clock.restart();
			}
		}
		/*
	************
	Updating the frame
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
			//set crosshair to mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);
			player.update(dtAsSeconds, Mouse::getPosition());
			Vector2f playerPosition(player.getCenter());
			mainView.setCenter(player.getCenter());
			//loop thru each zombie and update them
			for (int i = 0; i < numZombies;i++)
			{
				if (zombies[i].isAlive()) { zombies[i].update(dt.asSeconds(), playerPosition); }
			}
			//update any bullets that are in-flight
			for (int i = 0;i < 100;i++)
			{
				if (bullets[i].isInFlight()) { bullets[i].update(dtAsSeconds); }
			}
			//update pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);
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
			//Draw  background
			window.draw(background, &textureBackground);
			//Draw zombies
			for (int i = 0;i < numZombies;i++)
			{
				window.draw(zombies[i].getSprite());
			}
			//Draw bullets
			for (int i = 0;i < 100;i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}
			//Draw player
			window.draw(player.getSprite());
			//draw pick ups if spawned
			if (ammoPickup.isSpawned()) { window.draw(ammoPickup.getSprite()); }
			if (healthPickup.isSpawned()) { window.draw(healthPickup.getSprite()); }
			//draw crosshair
			window.draw(spriteCrosshair);

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

	delete[] zombies;
	return 0;
}
