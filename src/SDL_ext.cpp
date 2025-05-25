#ifndef SDL_EXT_CPP
#define SDL_EXT_CPP

#include "../includes/SDL_ext.hpp"


// Логирование сообщений об ошибках в модулях SDL2
void logSDLError(std::ostream& out, const char* err_msg)
{
	out << err_msg << SDL_GetError() << std::endl;
}

/**
 * Загружает изображение в текстуру для рендерера
 * @param file путь к изображению
 * @param ren Рендерер, на который эту текстуру можно будет отрисовать
 * @return Возвращает текстуру, либо nullptr в случае ошибки.
 */
SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* ren)
{
	SDL_Texture* texture = IMG_LoadTexture(ren, file.c_str());
	if ( !texture )
	{
		std::cout << IMG_GetError() << std::endl;
	}

	return texture;
}


/**
 * Отобразить SDL_Texture на SDL_Renderer на координатах x, y с масштабированием
 * @param tex Текстура для отображения
 * @param ren Рендерер
 * @param x Координаты
 * @param y
 * @param w Фактический размер при отрисовке
 * @param h
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, int w, int h)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, nullptr, &dst);
}


/**
 * Отобразить SDL_Texture на SDL_Renderer в целевом прямоугольнике
 * По желанию передаём параметр выреза
 * @param tex  Текстура для отображения
 * @param ren  Рендерер
 * @param dst  Область в виде прямоугольника, куда нужно отрисовать текстуру tex или её часть clip
 * @param clip Часть текстуры для рисования (вырезаемый прямоугольник),
 *				По умолчанию nullptr отрисовывает всю текстуру
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, SDL_Rect dst, SDL_Rect* clip)
{
	SDL_RenderCopy(ren, tex, clip, &dst);
}


/**
 * Отобразить SDL_Texture на SDL_Renderer в точке x, y, сохраняя
 * ширину и высоту текстуры и передаём параметр выреза по желанию
 * Если вырез был передан, ширина и высота выреза будут использованы вместо
 *		ширины и высоты текстуры
 * @param tex Текстура для отображения
 * @param ren Рендерер
 * @param x Координаты
 * @param y
 * @param clip Часть текстуры для рисования (вырезаемый прямоугольник)
 *			По умолчанию nullptr отрисовывает всю текстуру
 */
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, SDL_Rect* clip)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;

	if ( clip != nullptr )
	{
		dst.w = clip->w;
		dst.h = clip->h;
	}
	else
	{
		SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
		//dst.w = TILE_SIZE;
		//dst.h = TILE_SIZE;
	}

	renderTexture(tex, ren, dst, clip);
}


/**
 * Отображает текст в текстуру для отрисовки
 * @param message Сообщение, которое хотим отобразить
 * @param fontFile Шрифт, который хотим использовать
 * @param color Цвет, в который хотим окрасить текст
 * @param fontSize Размер шрифта
 * @param renderer Рендерер, в который нужно загрузить текстуру
 * @return SDL_Texture, содержащий отрендеренный текст или nullptr, если что-то пошло не так
 */
SDL_Texture* renderText(const std::string& message, const std::string& fontFile,
		SDL_Color color, int fontSize, SDL_Renderer* renderer)
{
	// Открываем шрифт
	TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if ( font == nullptr )
	{
		logSDLError(std::cout, "TTF_OpenFont");
		return nullptr;
	}

	// Сначала нужно отобразить на поверхность с помощью TTF_RenderText, т.к как SDL_ttf умеет
	// только отображать на поверхность
	// Затем загрузить поверхность в текстуру
	SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if ( surf == nullptr )
	{
		TTF_CloseFont(font);
		logSDLError(std::cout, "TTF_RenderText");
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
	if ( texture == nullptr )
	{
		TTF_CloseFont(font);
		logSDLError(std::cout, "CreateTexture");
		return nullptr;
	}

	// Очистка поверхности и шрифта
	cleanup(surf);
	TTF_CloseFont(font);

	return texture;
}


#endif
