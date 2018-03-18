#pragma once
#include "stdafx.h"
#include "Rendering.hpp"
#define D_LEFT  1
#define D_RIGHT 2
#define D_UP    3
#define D_DOWN  4

class Object {
public:	
	Object(short x, short y) {
		coord_.X = x;
		coord_.Y = y;
	}
	Object(COORD coord) {
		coord_ = coord;
		this->draw();
	}
	virtual void draw() {
		render::Write(" ", coord_, BACKGROUND_RED | BACKGROUND_GREEN);
	}
	inline bool isInRange(COORD obj, short range) {
		if(abs(coord_.X - obj.X)>range|| abs(coord_.Y - obj.Y)>range)
			return false;
		return true;
	}

	COORD getCoord() {
		return coord_;
	}
	void clear() {
		render::Write(" ", coord_, 0);
	}
	bool operator==(Object &v1) {
		if (coord_.X == v1.coord_.X && coord_.Y == v1.coord_.Y)
			return true;
		return false;
	}
	
protected:
	COORD coord_;
};

class Tank: public Object {
public:	
/*	Tank(COORD coords, char direction, bool is_enemy) :Object(coords) {
		direction_ = direction;
		color_ = (is_enemy ? FOREGROUND_RED : FOREGROUND_GREEN);
		this->Draw();
	}
	Tank(COORD coords, char direction, bool is_enemy) :Object(coords) {
		direction_ = direction;
		color_ = (is_enemy ? FOREGROUND_RED : FOREGROUND_GREEN);
		this->Draw();
	}*/
	Tank(COORD coords, char direction, bool is_enemy):Object(coords) {
		direction_ = direction;
		is_enemy_ = is_enemy;
		this->draw();
	}

	COORD getNeighborCoord(char direction = 0) {
		if (direction == 0)
			direction = direction_;
		switch (direction)
		{
		case(D_UP):if (1) {
			return { coord_.X, coord_.Y - 1 };
		}break;
		case(D_DOWN):if (1) {
			return { coord_.X, coord_.Y + 1 };
		}break;
		case(D_LEFT):if (1) {
			return { coord_.X - 1, coord_.Y };
		}break;
		case(D_RIGHT):if (1) {
			return { coord_.X + 1, coord_.Y };;
		}break;
		default: return { 0, 0 };
				 break;
		}
	}

	void draw() { 
		WORD color = (is_enemy_ ? FOREGROUND_RED : FOREGROUND_GREEN) | BACKGROUND_INTENSITY;
		switch (direction_)
		{
		case(D_UP):if (1) {
			render::Write("^", coord_, color);
		}break;
		case(D_DOWN):if (1) {
			render::Write("v", coord_, color);
		}break;
		case(D_LEFT):if (1) {
			render::Write("<", coord_, color);
		}break;
		case(D_RIGHT):if (1) {
			render::Write(">", coord_, color);
		}break;
		default:render::Write(" ", coord_, BACKGROUND_RED);
			break;
		}
		
	}
	char getDirection() {
		return direction_;
	}
	void changeDirection(const char direction) {
		direction_ = direction;
		this->draw();
	}
	bool isEnemy() {
		return is_enemy_;
	}
	bool move(char direction = 0) {
		if(direction)
			direction_ = direction;
		this->clear();
		switch (direction_)
		{
		case(D_UP):if (coord_.Y!=1) {
			coord_.Y--;
		}break;
		case(D_DOWN):if (coord_.Y != FIELD_HEIGHT-1) {
			coord_.Y++;
		}break;
		case(D_LEFT):if (coord_.X != 1) {
			coord_.X--;
		}break;
		case(D_RIGHT):if (coord_.X != FIELD_WIDTH - 1) {
			coord_.X++;
		}break;
		default:
			break;
		}
		this->draw();
		return false;
	}

protected:
	bool is_enemy_;
	char direction_ = 0;
};
class Bullet : public Tank {
public:
	Bullet(COORD coord, char direction, bool is_enemy) : Tank (coord, direction, is_enemy) 
	{
		direction_ = direction;
		this->draw();
	}
	Bullet(short x, short y, char direction, bool is_enemy) : Tank({ x, y }, direction, is_enemy)
	{
		direction_ = direction;
		this->draw();
	}

	void draw() {
		switch (direction_)
		{
		case(D_DOWN):
		case(D_UP):
			render::Write("|", coord_, FOREGROUND_RED| FOREGROUND_GREEN);
		break;
		case(D_LEFT):
		case(D_RIGHT):
			render::Write("-", coord_, FOREGROUND_RED | FOREGROUND_GREEN);
		break;
		default:render::Write(" ", coord_, BACKGROUND_RED);
			break;
		}

	}
	bool move() {
		this->clear();
		switch (direction_)
		{
		case(D_UP): coord_.Y--; break;
		case(D_DOWN): coord_.Y++; break;
		case(D_LEFT): coord_.X--; break;
		case(D_RIGHT): coord_.X++;break;
		default:
			break;
		}
		this->draw();
		return false;
	}
	bool operator==(Bullet &v1) {
		if (coord_.X == v1.coord_.X && coord_.Y == v1.coord_.Y)
			return true;
		else
			return false;
	}
protected:
	char direction_;
};