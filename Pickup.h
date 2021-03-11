#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
class Pickup
{
private:
	const int HEALTH_START_VALUE= 50;
	const int AMMO_START_VALUE=12;
	const int START_WAIT_TIME= 10;
	const int START_SECONDS_TO_LIVE= 5;
	Sprite m_Sprite;
	IntRect m_Arena;
	int m_Value;//how much this pickup worth

	int m_Type;//1=health,2=ammo
	bool m_Spawned;
	float m_SecondsSinceSpawn;
	float m_SecondsSinceDeSpawn;//despawn=disappear
	float m_SecondsToLive;
	float m_SecondsToWait;
public:
	Pickup(int type);//construct with type (1=health,2=ammo)//mistake?
	void setArena(IntRect arena);
	void spawn();
	FloatRect getPosition();
	Sprite getSprite();
	void update(float elapsedTime);
	bool isSpawned();//is it currently spawned?
	int gotIt();//get the goodness from the pickup (called when collides)
	void upgrade();//upgrade the value of pickup (called when p chooses to upgrade)

};