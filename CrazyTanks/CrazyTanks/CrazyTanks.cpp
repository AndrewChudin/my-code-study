#include "stdafx.h"
#include "tanks.hpp"
#include "Rendering.hpp"
#define ROTATE_CHANСE 30
#define SHOOT_CHANСE 50	
static HANDLE g_hStdin = nullptr;
Tank *character;
int score;
int lives = 3;
void UpScore(int add) {
	score += add;
	render::RefreshScore(score);
}
void GetDamage() {
	render::DeleteHeart(lives);
	lives--;
}
template <typename T>
T* FindByCoord(COORD coord, std::vector<T> &v)
{
	//render::Write(" ", coord, BACKGROUND_RED | BACKGROUND_GREEN);
	for (T &obj : v)
	{
		if (obj.getCoord().X == coord.X && obj.getCoord().Y == coord.Y)
			return &obj;
	}
	return nullptr;
}

template <typename T>
bool DeleteFromVector(T &value, std::vector<T> &v)
{
	auto it = v.begin();
	do {
		if (value == *it)
		{
			v.erase(it);
			return true;
		}
		it++;
	} while (it != v.end());
	return false;
}


bool Shooting(Tank &tank, std::vector<Object>& walls, std::vector<Bullet>& bullets) {
	if (rand() % 100 < SHOOT_CHANСE)
	{
		COORD t_coord = tank.getCoord();
		COORD c_coord = character->getCoord();
		if (t_coord.X == c_coord.X)//если на одной вертикали
		{
			if (t_coord.Y > c_coord.Y)//если ниже гг
			{
				if (FindByCoord({ t_coord.X, t_coord.Y - 1 }, walls) == nullptr)
				{
					tank.changeDirection(D_UP);
					bullets.push_back(Bullet(t_coord.X, t_coord.Y - 1, D_UP, true));
					return true;
				}
			}
			else //если выше гг
			{
				if (FindByCoord({ t_coord.X, t_coord.Y + 1 }, walls) == nullptr)
				{
					tank.changeDirection(D_DOWN);
					bullets.push_back(Bullet(t_coord.X, t_coord.Y + 1, D_DOWN, true));
					return true;
				}
			}
		}
		if (t_coord.Y == c_coord.Y)//если на одной горизонтали
		{
			if (t_coord.X > c_coord.X)//если правее гг
				if (FindByCoord({ t_coord.X - 1, t_coord.Y }, walls) == nullptr) {
					tank.changeDirection(D_LEFT);
					bullets.push_back(Bullet(t_coord.X - 1, t_coord.Y, D_LEFT, true));
					return true;
				}
				else//если левее гг
					if (FindByCoord({ t_coord.X + 1, t_coord.Y }, walls) == nullptr)
					{
						tank.changeDirection(D_RIGHT);
						bullets.push_back(Bullet(t_coord.X + 1, t_coord.Y, D_RIGHT, true));
						return true;
					}
		}
	}
	return false;
}
void BulletsStep(std::vector<Tank>& tanks, std::vector<Object>& walls, std::vector<Bullet>& bullets) {
	int bullets_amount = bullets.size();
	for (int i = 0; i < bullets_amount;i++) {
		COORD next_coord = bullets[i].getNeighborCoord();
		if (next_coord.X == 0 || next_coord.X == FIELD_WIDTH || next_coord.Y == 0 || next_coord.Y == FIELD_HEIGHT)
		{
			bullets[i].clear();
			DeleteFromVector(bullets[i], bullets);
			i--;
			bullets_amount = bullets.size();
			continue;
		}
		Tank *tank = FindByCoord(next_coord, tanks);
		if (tank != nullptr)
		{
			bullets[i].clear();
			if (!bullets[i].isEnemy()) {
				tank->clear();
				DeleteFromVector(*tank, tanks);
				UpScore(1);
			}
			DeleteFromVector(bullets[i], bullets);
			i--;
			bullets_amount = bullets.size();
			continue;
		}
		if (FindByCoord(next_coord, walls) != nullptr)
		{
			bullets[i].clear();
			DeleteFromVector(bullets[i], bullets);
			i--;
			bullets_amount = bullets.size();
			continue;
		}
		if (next_coord.X == character->getCoord().X &&
			next_coord.Y == character->getCoord().Y &&
			bullets[i].isEnemy())
		{
			bullets[i].clear();
			DeleteFromVector(bullets[i], bullets);
			i--;
			GetDamage();
			bullets_amount = bullets.size();
			continue;
		}
		bullets[i].move();

	}
}
bool CanMove(Tank tank, std::vector<Tank>& tanks, std::vector<Object>& walls) {
	COORD next_coord = tank.getNeighborCoord();
	if (FindByCoord(next_coord, walls) != nullptr)
		return false;
	if (FindByCoord(next_coord, tanks) != nullptr)
		return false;
	if (next_coord.X == character->getCoord().X && next_coord.Y == character->getCoord().Y)
		return false;
	return true;
}
void Shoot(std::vector<Tank>& tanks, std::vector<Object>& walls, std::vector<Bullet>& bullets) {
	COORD bullet_c = character->getNeighborCoord();
	Tank *tank = FindByCoord(bullet_c, tanks);
	if (tank != nullptr) {
		DeleteFromVector(*tank, tanks);
		UpScore(1);
		return;
	}
	if (FindByCoord(bullet_c, walls) == nullptr &&
		bullet_c.X != 0 && bullet_c.X != FIELD_WIDTH &&
		bullet_c.Y != 0 && bullet_c.Y != FIELD_HEIGHT)
		bullets.push_back(Bullet(bullet_c, character->getDirection(), false));
}
void TanksStep(std::vector<Tank>& tanks, std::vector<Object>& walls, std::vector<Bullet>& bullets) {
	for (Tank &tank : tanks) {
		if (!Shooting(tank, walls, bullets)) {
			if (rand() % 100 < 30)
				tank.changeDirection(rand() % 4 + 1);
			if (CanMove(tank, tanks, walls))
				tank.move();
		}
	}
}
void GenerateField(std::vector<Tank>& tanks, std::vector<Object>& walls) {
	COORD new_coord;
	bool suitable;
	srand(time(NULL));
	for (int n = 0; n < 10; n++) {
		do {
			new_coord = { rand() % (FIELD_WIDTH - 4) + 2,rand() % (FIELD_HEIGHT - 4) + 2 };
			suitable = true;
			for (Object existing_tank : tanks) {
				if (existing_tank.isInRange(new_coord, 2))
					suitable = false;
			}
		} while (!suitable);
		tanks.push_back(Tank(new_coord, rand() % 4 + 1, true));
	}
	for (int n = 0; n < 10; n++) {
		do {
			new_coord = { rand() % (FIELD_WIDTH - 4) + 2,rand() % (FIELD_HEIGHT - 4) + 2 };
			suitable = true;
			if (FindByCoord(new_coord, tanks) != nullptr)
				suitable = false;
		} while (!suitable);
		walls.push_back(Object(new_coord));
	}
}
WORD GetKeyDown()
{
	INPUT_RECORD input;
	DWORD nr;
	WORD key = 0;

	PeekConsoleInput(g_hStdin, &input, 1, &nr);
	if (nr > 0) {
		if (input.EventType == KEY_EVENT)
			if (input.Event.KeyEvent.bKeyDown)
				key = input.Event.KeyEvent.wVirtualKeyCode;

		FlushConsoleInputBuffer(g_hStdin);
	}
	return key;
}
void CharacterStep(WORD key, std::vector<Tank>& tanks, std::vector<Object>& walls, std::vector<Bullet>& bullets) {
	switch (GetKeyDown())
	{
	case(VK_LEFT): character->changeDirection(D_RIGHT); break;
	case(VK_RIGHT): character->changeDirection(D_RIGHT); break;
	case(VK_UP): character->changeDirection(D_UP); break;
	case(VK_DOWN): character->changeDirection(D_DOWN); break;
	case(VK_SPACE): Shoot(tanks, walls, bullets); break;
	default: character->move(); return;
	}
	if (CanMove(*character, tanks, walls))
		character->move();
}
void MainGameCycle()
{
	do {
		score = 0;
		lives = 3;
		std::vector<Object> walls;
		std::vector<Tank> tanks;
		std::vector<Bullet> bullets;
		render::ClearScreen();
		character = new Tank({ FIELD_WIDTH/2, FIELD_HEIGHT-1 }, D_UP, false);
		GenerateField(tanks, walls);
		render::RenderField();
		auto start = std::chrono::high_resolution_clock::now();
		while (tanks.size() > 0 && lives > 0) {
			Sleep(500);
			render::WriteEnemiesAmount(tanks.size());
			BulletsStep(tanks, walls, bullets);
			TanksStep(tanks, walls, bullets);
			render::RefreshTime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start));
			switch (GetKeyDown())
			{
			case(VK_LEFT): character->changeDirection(D_LEFT); break;
			case(VK_RIGHT): character->changeDirection(D_RIGHT); break;
			case(VK_UP): character->changeDirection(D_UP); break;
			case(VK_DOWN): character->changeDirection(D_DOWN); break;
			case(VK_SPACE): Shoot(tanks, walls, bullets);
				continue; break;
			default: continue; break;
			}
			if (CanMove(*character, tanks, walls))
				character->move();
			
		}
		delete character;
		char buf[16] = "Набрано очков:";
		_itoa(score, &buf[14], 10);
		MessageBox(0, buf, (lives > 0 ? "Победа":"Поражение"), MB_OK);
		if (MessageBox(0, "Хотите сыграть снова?", "Конец игры.", MB_YESNO) != IDYES)
			return;
	} while (true);
}

int main()
{

	g_hStdin = GetStdHandle(STD_INPUT_HANDLE);
	render::Init();
	MainGameCycle();
	return 0;
}
