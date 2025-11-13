#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>
#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include "functions.h"


const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;
struct car
{
	SDL_Texture* playerTexture;
	SDL_FRect position;
	double positionX;
	double positionY;
	double rotation;
	double velocityX;
	double velocityY;

};
//const float ROTATION_SPEED = 0.1f;
const float ACCELERATION = 1200.0f;


void handleInputandLogic(car& playercar, float deltaTime)
{
	SDL_PumpEvents();
	const bool* keystate = SDL_GetKeyboardState(NULL);
	
	float radians = playercar.rotation * M_PI / 180.0f;
	SDL_FPoint direction = { cos(radians), sin(radians) };
	
	float ROTATION_SPEED = 0.0005 * (sqrt(pow(playercar.velocityX, 2) + pow(playercar.velocityY, 2)));
	
	if (keystate[SDL_SCANCODE_LEFT])
	{
		playercar.rotation -= ROTATION_SPEED;
	}
	else if (keystate[SDL_SCANCODE_RIGHT])
	{
		playercar.rotation += ROTATION_SPEED;
	}
	if (keystate[SDL_SCANCODE_UP])
	{
		playercar.velocityX += direction.x * ACCELERATION * deltaTime;
		playercar.velocityY += direction.y * ACCELERATION * deltaTime;
	}
	else if (keystate[SDL_SCANCODE_DOWN])
	{
		playercar.velocityX -= direction.x * ACCELERATION * deltaTime;
		playercar.velocityY -= direction.y * ACCELERATION * deltaTime;
	}
	

	float DRAG_FACTOR = 0.999f;
	playercar.velocityX *= DRAG_FACTOR;
	playercar.velocityY *= DRAG_FACTOR;
	

	playercar.positionX += playercar.velocityX * deltaTime;
	playercar.positionY += playercar.velocityY * deltaTime;
}

void logSDLError(const std::string& msg) {
	SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s error: %s\n", msg.c_str(), SDL_GetError());
}

SDL_Texture* loadTexture(std::string path) {
	SDL_Texture* newTexture = nullptr;

	// 1. Load the BMP into an SDL_Surface using core SDL function
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());

	if (loadedSurface == nullptr) {
		std::cerr << "Unable to load BMP " << path.c_str() << "! SDL Error: " << SDL_GetError() << std::endl;
	}
	else {
		// 2. Create hardware-accelerated texture from the surface
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == nullptr) {
			std::cerr << "Unable to create texture from " << path.c_str() << "! SDL Error: " << SDL_GetError() << std::endl;
		}

		// 3. Free the surface (we only need the texture now)
		SDL_DestroySurface(loadedSurface);
	}

	return newTexture;
}

bool loadMedia() {
	bool success = true;

	// Load background texture
	// *** CRITICAL: Update this path to your actual BMP file location! ***
	texture = loadTexture("track.bmp");

	if (texture == nullptr) {
		success = false;
	}

	return success;
}

int main()
{


	car player;

	
	
	window = SDL_CreateWindow("Driving", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window) {
		logSDLError("SDL_CreateWindow");
		SDL_Quit();
		return 1;
	}
	renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer) {
		logSDLError("SDL_CreateRenderer");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	loadMedia();
	if (loadMedia()) {
		
		player.playerTexture = loadTexture("car.bmp");

		// Initialize position/size ONCE
		player.position = { 500.0f, 500.0f, 25.0f, 10.0f };
		player.positionX = player.position.x;
		player.positionY = player.position.y;
		player.rotation = 0.0;
		player.velocityX = 0.0;
		player.velocityY = 0.0;
	}
	else {
		// Handle media loading failure
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	SDL_Surface* backgroundSurface = SDL_LoadBMP("track.bmp");

	SDL_Color color = { 34, 177, 76 };
	Uint32 collisionColor = SDL_MapRGB( backgroundSurface->format, color.r, color.g, color.b );

	auto last_time = std::chrono::high_resolution_clock::now();
	bool running = true;
	SDL_Event event;
	while (running)
	{
		auto current_time = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(current_time - last_time).count();
		last_time = current_time;

		double nextX = player.positionX + player.velocityX;
		double nextY = player.positionY + player.velocityY;

		if (!checkCollision (nextX, nextY, backgroundSurface, collisionColor))
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
		}
		handleInputandLogic(player, deltaTime);
		player.position.x = (float)player.positionX;
		player.position.y = (float)player.positionY;

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, nullptr, nullptr);

		SDL_RenderTextureRotated(renderer, player.playerTexture, nullptr, &player.position,
			player.rotation, nullptr, SDL_FLIP_NONE);

		SDL_RenderPresent(renderer);
	}
}