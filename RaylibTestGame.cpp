#include <raylib.h>
#include <stdio.h>
#include <vector>

typedef struct ball {
	Vector2 pos;
	Vector2 velocity;
	Color color;
	float radius;
	float speedIncreaser;
	float speed;
} ball;

namespace textures {
	Texture2D ballTex;
	Texture2D playerTex;
}

namespace enemyHolder {
	std::vector<ball*> enemies;
}

void render(const int& winWidth, const int& winHeight, float& boxPosY, bool& exitWish, bool& shouldExit, bool& isDead, int& points) {
	BeginDrawing();

	ClearBackground(DARKGRAY);

	Rectangle _src = { 0.0f, 0.0f, (float)textures::ballTex.width, (float)textures::ballTex.height };
	Rectangle _dst = { winWidth / 2 - 10, boxPosY, 30, 30 };
	Vector2 _origin = { 8.0f, 8.0f };

	DrawTexturePro(textures::playerTex,
		_src,
		_dst,
		_origin,
		0.0f,
		WHITE);

	for (ball* i : enemyHolder::enemies) {
		Rectangle src = { 0.0f, 0.0f, (float)textures::ballTex.width * -i->velocity.x, (float)textures::ballTex.height };
		Rectangle dst = { i->pos.x, i->pos.y, i->radius * 2, i->radius * 2 };
		Vector2 origin = { 33.333f, 33.333f };

		DrawTexturePro(textures::ballTex,
			src,
			dst,
			origin,
			0.0f,
			WHITE);
	}

	if (exitWish) {
		DrawRectangle(0, winHeight / 2, winWidth, winHeight / 4, BLACK);
		DrawText("Exit? (Y/N)", 40, winHeight / 2 + 30, 30, RAYWHITE);
	}
	else if (isDead) {
		DrawRectangle(0, winHeight / 2, winWidth, winHeight / 4, BLACK);
		DrawText("YOU DIED! Retry? (R)", 40, winHeight / 2 + 30, 30, RED);
	}

	DrawFPS(10, 40);
	DrawText(TextFormat("SCORE: %03i", points), 10, 10, 30, RAYWHITE);

	EndDrawing();
}

void enemyUpdate() {
	for (ball* e : enemyHolder::enemies) {
		if (((e->pos.x + e->radius) >= GetScreenWidth()) || (e->pos.x <= 0 + e->radius)) e->velocity.x *= -1;
		if (((e->pos.y + e->radius) >= GetScreenHeight()) || (e->pos.y <= 0 + e->radius)) e->velocity.y *= -1;

		e->pos = { e->pos.x += e->velocity.x * GetFrameTime() * e->speed, e->pos.y += e->velocity.y * GetFrameTime() * e->speed };
		
		e->speed += e->speedIncreaser * GetFrameTime();
	}
}

void gameInit(ball* enemy, float& playerPos, float& velocity, bool& isDead) {
	enemy->radius = 30;
	enemy->pos = { 90, 90 };
	enemy->color = RED;
	enemy->velocity = { 1, 1 };
	enemy->speedIncreaser = 2.5f;
	enemy->speed = 80;

	playerPos = 30.0f;

	velocity = 0;

	isDead = false;
}

void textureLoader(Texture2D& tex, const char* path) {
	tex = LoadTexture(path);
}

int main(void) {
	const int winWidth = 800;
	const int winHeight = 450;

	InitWindow(winWidth, winHeight, "Test Game");

	SetExitKey(KEY_NULL);

	float boxPosY{ 0 };
	int scrollSpeed = 2;

	float velocity{ 0 };
	float grav = 9.81;

	float timer{ 0.0f };
	int points{ 0 };

	bool exitWish{ false };
	bool shouldExit{ false };

	bool isDead{ false };

	ball* enemy = new ball();

	Rectangle playerRect = { winWidth / 2 - 10, boxPosY, 20, 20 };

	textureLoader(textures::ballTex, "textures/enemy.png");
	textureLoader(textures::playerTex, "textures/player.png");

	gameInit(enemy, boxPosY, velocity, isDead);

	enemyHolder::enemies.push_back(enemy);

	SetTargetFPS(60);

	SetWindowState(FLAG_WINDOW_UNDECORATED);

	while (!shouldExit && !WindowShouldClose()) {
		if (IsKeyPressed(KEY_ESCAPE)) exitWish = true;

		timer += GetFrameTime();

		if (boxPosY > GetScreenHeight() || boxPosY < -25.0f) isDead = true;

		if (isDead) {
			enemy->velocity = { 0, 0 };
			if (IsKeyPressed(KEY_R)) gameInit(enemy, boxPosY, velocity, isDead);
		}

		if (exitWish) {
			if (IsKeyPressed(KEY_Y)) shouldExit = true;
			else if (IsKeyPressed(KEY_N)) exitWish = false;
		}
		else if (!isDead) {
			velocity -= grav * GetFrameTime();

			velocity += (GetMouseWheelMove() * scrollSpeed);
			boxPosY -= velocity;

			playerRect.y = boxPosY;

			enemyUpdate();

			if (timer >= 1.0f) {
				points++;
				timer = 0;
			}
		}

		if (CheckCollisionCircleRec(enemy->pos, enemy->radius, playerRect)) isDead = true;

		render(winWidth, winHeight, boxPosY, exitWish, shouldExit, isDead, points);
	}

	CloseWindow();

	enemyHolder::enemies.clear();

	delete enemy;

	return 0;
}