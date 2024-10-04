#include "Renderer.h"

#include <stdexcept>
#include <SDL_image.h>
#include <iostream>
#include <filesystem>

#include "GDebug/logtools.h"

vbl::Renderer::Renderer(uint32_t width, uint32_t height, uint32_t atlasWidth, uint32_t atlasHeight, float renderScale)
	:renderer(NULL), window(NULL), target(NULL), renderScale(renderScale), atlas(atlasWidth, atlasHeight)
{
	SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &window, &renderer);
	if (!window)
	{
		throw new std::exception(SDL_GetError());
	}
	if (!renderer)
	{
		throw new std::exception(SDL_GetError());
	}
	this->target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, int(width * renderScale), int(height * renderScale));
	if (!this->target)
	{
		throw new std::exception(SDL_GetError());
	}
	SDL_SetTextureBlendMode(this->target, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
	SDL_RenderClear(this->renderer);
	SDL_SetRenderTarget(this->renderer, this->target);
}

void vbl::Renderer::scaleRect(SDL_Rect* rect)
{
	if (!rect)
	{
		return;
	}
	rect->x = int(rect->x * this->renderScale);
	rect->y = int(rect->y * this->renderScale);
	rect->w = int(rect->w * this->renderScale);
	rect->h = int(rect->h * this->renderScale);
}

void vbl::Renderer::mapRect(SDL_Rect* rect)
{
	if (!rect)
	{
		return;
	}
	rect->x = int((rect->x + this->pos.x) * this->renderScale);
	rect->y = int((rect->y + this->pos.y) * this->renderScale);
	rect->w = int(rect->w * this->renderScale);
	rect->h = int(rect->h * this->renderScale);
}

void vbl::Renderer::mapRect(maf::frect* rect)
{
	if (!rect)
	{
		return;
	}
	rect->x = (rect->x + this->pos.x) * this->renderScale;
	rect->y = (rect->y + this->pos.y) * this->renderScale;
	rect->w = rect->w * this->renderScale;
	rect->h = rect->h * this->renderScale;
}

void vbl::Renderer::mapRect(SDL_FRect* rect)
{
	if (!rect)
	{
		return;
	}
	rect->x = (rect->x + this->pos.x) * this->renderScale;
	rect->y = (rect->y + this->pos.y) * this->renderScale;
	rect->w = rect->w * this->renderScale;
	rect->h = rect->h * this->renderScale;
}

void vbl::Renderer::clearFrame()
{
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
	SDL_RenderClear(this->renderer);
}

void vbl::Renderer::clearFrame(SDL_Color clr)
{
	SDL_SetRenderDrawColor(this->renderer, clr.r, clr.g, clr.b, clr.a);
	SDL_RenderClear(this->renderer);
}

void vbl::Renderer::presentFrame()
{
	if (SDL_SetRenderTarget(this->renderer, NULL)) { LOGF_WARN("Setting render target failed, {}", SDL_GetError()); }
	if (SDL_RenderCopy(this->renderer, this->target, NULL, NULL)) { LOGF_WARN("Copying from low-res target failed, {}", SDL_GetError()); }
	SDL_RenderPresent(this->renderer);
	if (SDL_SetRenderTarget(this->renderer, this->target)) { LOGF_WARN("Setting render target to low-res failed, {}", SDL_GetError()); }
}

void vbl::Renderer::renderSpriteConst(const SpriteTexture& sprite, uint8_t alpha)
{
	SDL_Rect clipping{};
	if (sprite.cachedID == SpriteTexture::INVALID_CACHED)
	{
		clipping = this->atlas.get(sprite.getPicture());
	}
	else
	{
		clipping = this->atlas.get(sprite.cachedID);
	}
	if (clipping.w == 0 || clipping.h == 0)
	{
		DEBUG_LOG_F("Rendering sprite texture with picture {} failed. Atlas likely does not have {} registered.", sprite.getPicture(), sprite.getPicture());
	}
	//DEBUG_LOG_F("{}, {} {}x{}", clipping.x, clipping.y, clipping.w, clipping.h);
	SDL_Rect scaled = *sprite.getRect();
	mapRect(&scaled);
	if (sprite.isAnimated())
	{
		SDL_Rect animClippingRect = sprite.getClippingRect();
		clipping.x += animClippingRect.x;
		clipping.y += animClippingRect.y;
		clipping.w = animClippingRect.w;
		clipping.h = animClippingRect.h;
	}
	SDL_Texture* atlasTexture = this->atlas.getTexture(this->renderer);
	if (alpha != 255)
	{
		SDL_SetTextureAlphaMod(atlasTexture, alpha);
	}
	if (sprite.usesSpecialBlendmode)
	{
		SDL_SetTextureBlendMode(atlasTexture, sprite.specialBlendingMode);
	}
	SDL_RenderCopyEx(this->renderer, atlasTexture, &clipping, &scaled, sprite.getRotation(), NULL, SDL_FLIP_NONE);
	if (alpha != 255)
	{
		SDL_SetTextureAlphaMod(atlasTexture, 255);
	}
	if (sprite.usesSpecialBlendmode)
	{
		SDL_SetTextureBlendMode(atlasTexture, SDL_BLENDMODE_BLEND);
	}
}

void vbl::Renderer::renderSprite(SpriteTexture& sprite, uint8_t alpha)
{
	if (sprite.cachedID == SpriteTexture::INVALID_CACHED)
	{
		this->atlas.get(sprite.getPicture(), sprite.cachedID);
	}
	renderSpriteConst(sprite, alpha);
}

void vbl::Renderer::renderText(std::shared_ptr<RawTexture> text)
{
	SDL_Rect pos = text->getPos();
	this->mapRect(&pos);
	SDL_RenderCopy(this->renderer, text->getTexture(), NULL, &pos);
	printf(SDL_GetError());
}

void vbl::Renderer::renderRect(maf::frect rect, SDL_Color color)
{
	this->mapRect(&rect);
	SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
	SDL_FRect sdl_rect = rect.SDL();
	SDL_RenderDrawRectF(this->renderer, &sdl_rect);
}

void vbl::Renderer::renderBoundingBox(const MAABB& box, SDL_Color clr)
{
	SDL_SetRenderDrawColor(this->renderer, clr.r, clr.g, clr.b, clr.a);
	for (const auto& rect : box.getBoxes())
	{
		SDL_FRect frect = rect.SDL();
		mapRect(&frect);
		SDL_RenderDrawRectF(this->renderer, &frect);
	}
}

void vbl::Renderer::renderGeometry(const Geometry& geometry)
{
	for (const auto& rect : geometry.getRects())
	{
		SDL_SetRenderDrawColor(this->renderer, rect.clr.r, rect.clr.g, rect.clr.b, rect.clr.a);
		SDL_Rect temp = rect.box.SDLI();
		mapRect(&temp);
		SDL_RenderFillRect(this->renderer, &temp);
		SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(this->renderer, &temp);
	}
}

void vbl::Renderer::renderGeometry(const Geometry& geometry, bool debug)
{
	renderGeometry(geometry);
	SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255);
	for (const auto& box : geometry.getBoxes())
	{
		SDL_FRect frect = { box.box.x * renderScale, box.box.y * renderScale, box.box.w * renderScale, box.box.h * renderScale };
		SDL_Color specialColor = { 0, 255, 0, 255 };
		switch (box.team)
		{
		case 1:
			specialColor = { 255, 255, 0, 255 };
			break;
		case 2:
			specialColor = { 255, 0, 0, 255 };
			break;
		case 3:
			specialColor = { 0, 0, 255, 255 };
			break;
		case 4:
			specialColor = { 255, 0, 255, 255 };
			break;
		default:
			break;
		}
		SDL_SetRenderDrawColor(this->renderer, specialColor.r, specialColor.g, specialColor.b, specialColor.a);
		SDL_RenderDrawRectF(this->renderer, &frect);
		if (!box.trigger)
		{
			continue;
		}
		SDL_SetRenderDrawColor(this->renderer, specialColor.r, specialColor.g, specialColor.b, 64);
		SDL_RenderFillRectF(this->renderer, &frect);
	}
}

void vbl::Renderer::renderTrace(const std::vector<maf::ivec2>& points, SpriteTexture& endTex)
{
	if (points.size() < 2)
	{
		return;
	}
	SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
	maf::ivec2 lastPoint = points[0];
	for (int i = 1; i < points.size(); i++)
	{
		SDL_RenderDrawLine(this->renderer, int(lastPoint.x * renderScale), int(lastPoint.y * renderScale), int(points[i].x * renderScale), int(points[i].y * renderScale));
		lastPoint = points[i];
	}
	endTex.setMiddle({ lastPoint.x, lastPoint.y });
	renderSprite(endTex);
}

SDL_Texture* vbl::Renderer::load(const std::string& path, bool ignoreMissing)
{
	SDL_Surface* surf = IMG_Load(path.c_str());
	if (!surf)
	{
		LOG_WARN(std::format("Loading {} as surface failed, {}", path, SDL_GetError()));
		if (!ignoreMissing)
		{
			surf = IMG_Load(this->missingTex.c_str());
		}
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surf);
	if (!texture)
	{
		LOG_WARN(std::format("Loading {} into a texture failed, {}", path, SDL_GetError()));
	}
	SDL_FreeSurface(surf);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	return texture;
}

void vbl::Renderer::debugRender(SDL_Surface* surface)
{
	SDL_Surface* windowSurf = SDL_GetWindowSurface(this->window);
	SDL_FillRect(windowSurf, NULL, SDL_MapRGBA(windowSurf->format, 0, 0, 0, 255));
	SDL_BlitScaled(surface, NULL, windowSurf, NULL);
	SDL_UpdateWindowSurface(this->window);
	SDL_Delay(1000);
}

void vbl::Renderer::debugRender(SDL_Texture* texture)
{
	this->clearFrame();
	SDL_RenderCopy(this->renderer, texture, NULL, NULL);
	this->presentFrame();
	SDL_Delay(1000);
}

void vbl::Renderer::loadChars(const std::string& fontPath, SDL_Color clr)
{
	char charset[] = {
		'_', '0', '1', '2', '3', '4',
		'5', '6', '7', '8', '9', 'a',
		'b', 'c', 'd', 'e', 'f', 'g',
		'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's',
		't', 'u', 'v', 'w', 'x', 'y',
		'z', '.', '*', '-', ',', ' '
	};
	int i = 0;
	int characterW = 5;
	int characterH = 7;
	SDL_Rect clip = { 0,0,characterW,characterH };
	LOG_F("Loading font {}.", fontPath);
	this->fontSurface = IMG_Load(fontPath.c_str());
	if (!this->fontSurface)
	{
		LOG_F("Could not load font {}, {}.", fontPath, SDL_GetError());
		for (int i = 0; i < sizeof(charset) / sizeof(char); i++)
		{
			this->charMap[charset[i]] = -1;
		}
		return;
	}
	int limit = sizeof(charset) / sizeof(char);
	for (int i = 0; i < limit; i++)
	{
		this->charMap[charset[i]] = i * characterW;
		if (i * characterW >= this->fontSurface->w)
		{
			this->charMap[charset[i]] = -characterW;
		}
	}
	SDL_SetSurfaceBlendMode(this->fontSurface, SDL_BLENDMODE_BLEND);
}

std::shared_ptr<vbl::RawTexture> vbl::Renderer::makeText(const std::string& str)
{
	// size_t narrow
	SDL_Surface* textSurf = SDL_CreateRGBSurfaceWithFormat(0, int(str.size()) * 5 + std::max((int)str.size() - 1, 0) + 1, 7, 32, SDL_PIXELFORMAT_RGBA8888);
	SDL_FillRect(textSurf, NULL, SDL_MapRGBA(textSurf->format, 0, 0, 0, 0));
	SDL_Rect at = { 0, 0, 5, 7 };
	SDL_SetSurfaceBlendMode(textSurf, SDL_BLENDMODE_BLEND);
	for (int i = 0; i < str.size(); i++)
	{
		int charX = this->charMap[str[i]];
		if (charX < 0)
		{
			at.x += at.w;
			continue;
		}
		SDL_Rect src = { charX, 0, 5, 7 };
		SDL_BlitScaled(this->fontSurface, &src, textSurf, &at);
		at.x += at.w;
	}
	SDL_Texture* text = SDL_CreateTextureFromSurface(this->renderer, textSurf);
	SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
	std::shared_ptr<RawTexture> raw = std::make_shared_for_overwrite<RawTexture>();
	raw->changeTexture(text);
	raw->setRect({ 0,0,int(float(textSurf->w) / this->renderScale), int(float(textSurf->h) / this->renderScale) });
	SDL_FreeSurface(textSurf);
	return raw;
}
