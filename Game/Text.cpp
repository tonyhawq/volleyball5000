#include "Text.h"


vbl::RawTexture::RawTexture(SDL_Rect pos, SDL_Texture* texture)
	:pos(pos), texture(texture)
{

}

vbl::RawTexture::RawTexture()
	:pos({ 0 }), texture(NULL)
{

}

vbl::RawTexture::~RawTexture()
{
	if (responsible)
	{
		SDL_DestroyTexture(this->texture);
	}
}