#include <SDL3/SDL.h>
#include <iostream>




bool checkCollision(double objX, double objY, SDL_Surface* backgroundSurface, Uint32 collisionColor)
{
	if (objX < 0 || objY < 0 || objX >= backgroundSurface->w || objY >= backgroundSurface->h)
	{
		return true;
	}
	int pixelIndex = (objY * backgroundSurface->w) + objX;
	Uint32* pixels = static_cast<Uint32*>(backgroundSurface->pixels);
	Uint32 pixelColor = pixels[pixelIndex];

	return (pixelColor == collisionColor);
}