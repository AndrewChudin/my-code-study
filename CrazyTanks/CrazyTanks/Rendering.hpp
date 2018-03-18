#pragma once
#include "stdafx.h"
#define FIELD_HEIGHT 25
#define FIELD_WIDTH 50
namespace render {
	static HANDLE g_hStdout = nullptr;
	
	void Init() {
		SetConsoleTitle("CrazyTanks");
		g_hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;
		cursorInfo.dwSize = 1;
		cursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo(g_hStdout, &cursorInfo);
	}
	void GotoXY(int x, int y)
	{
		COORD coord;
		coord.X = x;
		coord.Y = y;

		SetConsoleCursorPosition(g_hStdout, coord);
	}
	void Write(const char *text, COORD coord, WORD attributes)
	{
		DWORD length = lstrlen(text);
		DWORD nw;


		WriteConsoleOutputCharacter(g_hStdout, text, length, coord, &nw);
		FillConsoleOutputAttribute(g_hStdout, attributes, length, coord, &nw);
	}
	void ClearScreen()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(g_hStdout, &csbi);

		COORD coord = { 0, 0 };
		DWORD size = csbi.dwSize.X*csbi.dwSize.Y;
		DWORD nw;
		FillConsoleOutputCharacter(g_hStdout, ' ', size, coord, &nw);
		FillConsoleOutputAttribute(g_hStdout, csbi.wAttributes, size, coord, &nw);

		GotoXY(0, 0);
	}
	void RefreshScore(int score)
	{
		char buf[3] = { 0 };
		_itoa(score, buf, 10);
		Write(buf, { FIELD_WIDTH + 8, 1 }, FOREGROUND_GREEN);
	}
	void DeleteHeart(short lives) {
		Write(" ", { FIELD_WIDTH + 8 + lives, 2 }, 0);
	}
	void WriteEnemiesAmount(int amount)
	{
		char buf[6] = { "     " };
		Write(buf, { FIELD_WIDTH + 10, 3 }, FOREGROUND_BLUE);
		_itoa(amount, buf, 10);
		Write(buf, { FIELD_WIDTH + 10, 3 }, FOREGROUND_GREEN);
	}
	void RefreshTime(std::chrono::seconds sec)
	{
		
		char buf[6] = { 0 };
		_itoa(sec.count(), buf, 10);
		Write(buf, { FIELD_WIDTH + 7, 4 }, FOREGROUND_BLUE);
	}
	void RenderField()
	{
		for (short h = 0; h <= FIELD_HEIGHT; h++)
			for (short w = 0; w <= FIELD_WIDTH; w++)
				if (h == 0 || h == FIELD_HEIGHT || w == 0 || w == FIELD_WIDTH)
					Write(" ", { w,h }, BACKGROUND_RED | BACKGROUND_GREEN);
		Write("score: 0", { FIELD_WIDTH + 2, 1 }, FOREGROUND_INTENSITY);
		Write("lives:", { FIELD_WIDTH + 2, 2 }, FOREGROUND_INTENSITY);
		Write("\x03\x03\x03", { FIELD_WIDTH + 8, 2 }, FOREGROUND_RED);
		Write("Enemies:", { FIELD_WIDTH + 2, 3 }, FOREGROUND_INTENSITY);
		Write("Time:", { FIELD_WIDTH + 2, 4 }, FOREGROUND_INTENSITY);
	}
}