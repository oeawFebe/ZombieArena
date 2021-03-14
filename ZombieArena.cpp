#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
	float fireRate = 3.0;//I think 1.0 too small
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
	//about the game
	int score = 0;
	int hiScore = 0;
	//for home and gameover screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);
	View hudView(FloatRect(0, 0, resolution.x, resolution.y));
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");
	//Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");
	//leveling up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream << "1-Increased rate of fire" << "\n2-Increased clip size(next reload)" << "\n3-Increased Max health" << "\n4-Increased run speed" << "\n5-More and better health pickups" << "\n6-More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());
	//Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(200, 980);
	//score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 0);
	//load the high score from a text file
	std::ifstream inputFile("gamedata/scores.txt");
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}
	//hiScore
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	hiScoreText.setString(s.str());
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies:100");
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave:0");
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);

	Text debugText;
	debugText.setFont(font);
	debugText.setCharacterSize(25);
	debugText.setFillColor(Color::White);
	debugText.setPosition(20,220);
	std::stringstream ss;
	int framesSinceLastHUDUpdate = 0;
	int fpsMeasurementFrameInterval = 200;

	//prepare the hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);
	//prepare the splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/splat.wav");
	Sound splat;
	splat.setBuffer(splatBuffer);
	//prepare the shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/shoot.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);
	//prepare the reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);
	//prepare the reloadFailed sound
	SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);
	//prepare the powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	Sound powerup;
	powerup.setBuffer(powerupBuffer);
	//prepare the pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);
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
				//start a new game while in game_over state
				else if (event.key.code == Keyboard::Return && state == State::GAME_OVER)
				{
					state = State::LEVELING_UP; 
					wave = 0;
					score = 0;
					currentBullet = 0;
					bulletsSpare = 24;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;
					//reset player's stats
					player.resetPlayerStats();
				}
				if (state == State::PLAYING)
				{
					//Reloading
					if (event.key.code == Keyboard::R)
					{
						if (bulletsSpare >= clipSize)
						{
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
							reload.play();
						}
						else if (bulletsSpare > 0)
						{
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
							reload.play();
						}
						else
						{
							reloadFailed.play();
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
					shoot.play();
					bulletsInClip--;
				}
			}
		}// End WASD while playing
		//Handle LEVELING_UP state
		if (state == State::LEVELING_UP)
		{
			//Handle the player LEVELING UP
			if (event.key.code == Keyboard::Num1) { fireRate++; state = State::PLAYING; }
			if (event.key.code == Keyboard::Num2) { clipSize += clipSize;state = State::PLAYING; }
			if (event.key.code == Keyboard::Num3) { player.upgradeHealth();state = State::PLAYING; }
			if (event.key.code == Keyboard::Num4) { player.upgradeSpeed(); state = State::PLAYING; }
			if (event.key.code == Keyboard::Num5) { healthPickup.upgrade();state = State::PLAYING; }
			if (event.key.code == Keyboard::Num6) { ammoPickup.upgrade();state = State::PLAYING; }
			if (state == State::PLAYING)
			{
				//increment the wave number
				wave++;
				//prepare the level
				//modify later
				arena.width = 500*wave;
				arena.height = 500 * wave;
				arena.left = 0;
				arena.top = 0;
				//pass the vertex array by reference
				int tileSize = createBackground(background, arena);
				//spawn player in the middle of arena
				player.spawn(arena, resolution, tileSize);
				player.resetPlayerStats();//I think it is necessary
				//configure pickups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);
				healthPickup.spawn();//?
				ammoPickup.spawn();//?
				//create horde of zombies
				numZombies = 5*wave;
				//delete priviously allocated memory (if it exists)
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;
				powerup.play();
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

			//Collision detection
			//have zombie been shot?
			for (int i = 0;i < 100;i++)
			{
				for (int j = 0;j < numZombies;j++)
				{
					if(bullets[i].isInFlight()&&zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{ 
							bullets[i].stop(); 
							if (zombies[j].hit())
							{
								score += 10;
								if (score >= hiScore) { hiScore = score; } 
								numZombies--; 
								if(numZombiesAlive==0){state=State::LEVELING_UP;}
							}
						}
						splat.play();
					}
				}
			}
			//have zombies touched player
			for (int i = 0;i < numZombies;i++)
			{
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive())
				{
					if(player.hit(gameTimeTotal))
					{hit.play();}
					if (player.getHealth() <= 0)
					{
						state=State::GAME_OVER;
						std::ofstream outputFile("gamedata/scores.txt");
						outputFile << hiScore;
						outputFile.close();
					}
					
				}
			}
			//has player touched picukp(Hleath)
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt());
				pickup.play();
			}
			//has player touched picukp(ammo)
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare+=ammoPickup.gotIt();
				reload.play();
			}
			//size up healthbar
			healthBar.setSize(Vector2f(player.getHealth() * 3/5, 50));//instead of 3, 3/5
			//increment num of frames
			framesSinceLastHUDUpdate++;
			//recalculate every fps measurement
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{
				std::stringstream ssAmmo, ssScore, ssHiScore, ssWave, ssZombiesAlive;
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());
				//update high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());
				//update wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());
				framesSinceLastHUDUpdate = 0;
				
			}//end hud update

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
			//switch to HUD view
			window.setView(hudView);
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}
		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}
		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}
		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}
		window.display();



	}//End  loop

	delete[] zombies;
	return 0;
}
