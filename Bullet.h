#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
class Bullet
{
private:
	Vector2f m_Position;
	RectangleShape m_BulletShape;// abit like timber's timebar
	bool m_InFlight = false;
	float m_BulletSpeed = 1000;//speed for x and y axis
	float m_BulletDistanceX;//what fraction of 1 pixel does bullet travel
	float m_BulletDistanceY;//what fraction of 1 pixel does bullet travel
	//boundaries so that bullet does not fly forever
	float m_MaxX;
	float m_MinX;
	float m_MaxY;
	float m_MinY;
public:
	Bullet();//constructor. Always in public
	void stop();//stop the bullet
	bool isInFlight();
	void shoot(float startX, float startY, float xTarget, float YTarget);//XYtarget is crosshair
	FloatRect getPosition();
	RectangleShape getShape();//for drawing
	void update(float elapsedTime);
};
