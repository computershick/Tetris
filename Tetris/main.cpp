#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <string.h>

using namespace sf;

int const M = 20;
int const N = 10;


int score, next;
int dx;
int x, y, tet;
float t, ty;
float speed_y = 2.0, extra_speed, hard_speed;
bool rotate, flag, flag1, new_tetr = true;
String num, gameOver = "Game Over!";

int field[N][M];

int figures[7][4][2] = {
	{{0, 0}, {1, 0}, {0 ,1}, {1, 1}}, // O
	{{0, 0}, {0, 1}, {0, 2}, {0, 3}}, // I
	{{0, 0}, {0, 1}, {1, 1}, {1, 2}}, // S
	{{1, 0}, {1, 1}, {0, 1}, {0, 2}}, // Z
	{{0, 0}, {0, 1}, {0, 2}, {1, 2}}, // L
	{{1, 0}, {1, 1}, {1, 2}, {0, 2}}, // J
	{{0, 0}, {0, 1}, {1, 1}, {0, 2}}  // T
};

struct Point {
	int x;
	int y;
} a[4], b[4];

RenderWindow window(VideoMode(360, 468), "Tetris");
Image icon;
Font font;
Text text;
Texture texture;
Texture canv;
Sprite sprite(texture);
Sprite spr(canv);
Point p;

void input();
void logic();
void draw();
bool collision();
bool game_over();

int main()
{
	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_HIDE);
	window.setFramerateLimit(30);
	texture.loadFromFile("res\\tiles.png");
	canv.loadFromFile("res\\field.png");
	icon.loadFromFile("res\\icon.png");
	font.loadFromFile("res\\duck4game.ttf");
	
	window.setIcon(32, 32, icon.getPixelsPtr());
	text.setFont(font);
	text.setFillColor(Color::White);
	
	Clock clock;
	srand(time(0));

	next = rand() % 7;

	// Главный цикл приложения: выполняется, пока открыто окно
	while (window.isOpen())
	{
		clock.restart();

		input();
		if (!game_over()) {
			logic();
		}
		
		
		// Установка цвета фона - белый
		window.clear(Color::White);

		draw();

		if (game_over()) {
			text.setPosition(4, M * 18 / 2 - 12);
			text.setCharacterSize(24);
			text.setString(gameOver);
			window.draw(text);
		}
		
		text.setPosition(N * 18 + 24, 24);
		text.setCharacterSize(20);
		text.setString("Score:");
		window.draw(text);
		num = std::to_string(score);
		text.setPosition(N * 18 + 24, 52);
		text.setString(num);
		window.draw(text);

		// Отрисовка окна
		window.display();

		
		t = clock.getElapsedTime().asMilliseconds() / 1000.0;
	}  

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void input() {
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
			window.close();
		}

		if (event.type == Event::KeyPressed) {
			if (event.key.code == Keyboard::Up) {
				rotate = true;
			}
			if (event.key.code == Keyboard::Down) {
				extra_speed = 4 * speed_y;
			}
			if (event.key.code == Keyboard::Left) {
				dx = -1;
			}
			if (event.key.code == Keyboard::Right) {
				dx = 1;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void logic() {
	// new tetramine
	if (new_tetr) {
		tet = next;
		next = rand() % 7;
		for (int i = 0; i < 4; i++) {
			a[i].x = figures[tet][i][0] + N / 2 - 1;
			a[i].y = figures[tet][i][1];
		}
		new_tetr = false;
	}

	// x moving
	for (int i = 0; i < 4; i++) {
		b[i] = a[i];
		a[i].x += dx;
	}
	dx = 0;
	if (collision()) {
		for (int i = 0; i < 4; i++) {
			a[i] = b[i];
		}
	}

	// y moving
	ty += t;
	if (ty >= 1 / (speed_y + extra_speed + hard_speed)) {
		for (int i = 0; i < 4; i++) {
			b[i] = a[i];
			a[i].y++;
		}
		ty = 0;
		extra_speed = 0;
		if (collision()) {
			for (int i = 0; i < 4; i++) {
				a[i] = b[i];
			}
			new_tetr = true;
			for (int i = 0; i < 4; i++) {
				field[a[i].x][a[i].y] = tet + 1;
			}
		}
	}
	
	// rotation
	if (rotate) {
		p = a[1];
		for (int i = 0; i < 4; i++) {
			b[i] = a[i];
			x = a[i].y - p.y;
			y = a[i].x - p.x;
			a[i].x = p.x - x;
			a[i].y = p.y + y;
		}
		if (collision()) {
			for (int i = 0; i < 4; i++) {
				a[i] = b[i];
			}
		}
		rotate = false;
	}

	// scoring
	for (int i = 0; i < M; i++) {
		flag = true;
		for (int j = 0; j < N; j++) {
			if (field[j][i] == 0) {
				flag = false;
			}
			
		}
		if (flag) {
			score += 100;
			if (score % 1000 == 0) {
				hard_speed++;
			}
			for (int k = i; k > 0; k--) {
				for (int l = 0; l < N; l++) {
					field[l][k] = field[l][k - 1];
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void draw() {
	spr.setTextureRect(IntRect(0, 0, 360, 468));
	window.draw(spr);
	sprite.setTextureRect(IntRect(tet * 18, 0, 18, 18));
	
	for (int i = 0; i < 4; i++) {
		sprite.setPosition(a[i].x * 18, a[i].y * 18);
		window.draw(sprite);
	}
	
	sprite.setTextureRect(IntRect(next * 18, 0, 18, 18));
	for (int i = 0; i < 4; i++) {
		sprite.setPosition(figures[next][i][0] * 18 + 254, figures[next][i][1] * 18 + 120);
		window.draw(sprite);
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (field[i][j]) {
				sprite.setTextureRect(IntRect((field[i][j] - 1) * 18, 0, 18, 18));
				sprite.setPosition(i * 18, j * 18);
				window.draw(sprite);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool collision() {
	for (int i = 0; i < 4; i++) {
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) {
			return 1;
		}
		if (field[a[i].x][a[i].y]) {
			return 1;
		}
	}	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

bool game_over() {
	for (int i = 0; i < N; i++) {
		if (field[i][0]) {
			return true;
		}
	}
	return false;
}