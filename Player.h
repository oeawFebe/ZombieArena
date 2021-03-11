#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
class Player
{
private:
	const float START_SPEED = 200;
	const float START_HEALTH = 500;//I think 100 too small
	Vector2f m_Position;//center of sprite coordinate
	Sprite m_Sprite;
	//
	Texture m_Texture;
	Vector2f m_Resolution;
	IntRect m_Arena;
	int m_TileSize;
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;
	int m_Health;//how much health has player got?
	int m_MaxHealth;
	Time m_LastHit;
	float m_Speed;//speed in pixels per second
	//
public:
	Player();
	void spawn(IntRect arena, Vector2f resolution, int tileSize);
	void resetPlayerStats();//call this at the end of every game
	//handle player getting hit by zombie
	bool hit(Time timeHit);
	Time getLastHitTime();//how long ago was last hit
	FloatRect getPosition();
	Vector2f getCenter();
	float getRotation();
	Sprite getSprite();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();
	void update(float elapsedTime, Vector2i mousePosition);// we will call this once every frame
	void upgradeSpeed();
	void upgradeHealth();//player choose to have more health
	void increaseHealthLevel(int amount);//increase up to maximum amount of health
	int getHealth();
};