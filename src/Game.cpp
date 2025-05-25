#ifndef GAME_CPP
#define GAME_CPP


#include "../includes/Game.hpp"


int Cell::GetAliveNeighbours(void) const
{
	int alive_counter = 0;

	for ( auto& neighbour_addr : neighbours_addrs )
	{
		Cell* neighbour_cell = *neighbour_addr;
		if ( neighbour_cell->GetState() == ALIVE_CELL )
			alive_counter++;
	}

	return alive_counter;
}

bool Cell::DoLifeCycle(void)
{
	int alives_count = GetAliveNeighbours();

	if ( alives_count != 3 )
	{
		return true;
	}

	return false;
}

bool AliveCell::DoLifeCycle(void)
{
	int alives_count = GetAliveNeighbours();

	if ( (alives_count < 2) || (alives_count > 3) )
	{
		return false;
	}

	return true;
}

bool DeadCell::DoLifeCycle(void)
{
	int alives_count = GetAliveNeighbours();

	if ( alives_count != 3 )
	{
		return true;
	}

	return false;
}

Cell::Cell(CellParams cparams, const std::list<Cell**>& na_list, SDL_Rect area, int cell_state, int idx, SDL_Texture* tex, SDL_Renderer* ren)
{
	size.x = area.x;
	size.y = area.y;
	size.w = area.w;
	size.h = area.h;

	SetState(cell_state);
	SetIndex(idx);
	SetTextureObject(tex);
	SetRenderer(ren);

	neighbours_addrs = na_list;
	params = cparams;
}

void Cell::Render(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, int w, int h) const
{
	if ( tex == nullptr || ren == nullptr )
	{
		std::cout << "[Cell::Render]" << "(" << this << "): " << "Unable to render cause texture or renderer objects have null pointer!" << std::endl;
		return;
	}

	if ( (w < 1) || (h < 1) || (x < 0) || (y < 0) )
	{
		std::cout << "[Cell::Render]" << "(" << this << "): " << "Params passed to render function have invalid values!" << std::endl;
		return;
	}

	renderTexture(texture, renderer, x, y, w, h);
}

bool Cell::IsPointInCell(SDL_Point p)
{
	int a = size.x + size.w - 1;
	int b = size.y + size.h - 1;

	if	(
			(p.x >= size.x) && (p.x <= a) &&
			(p.y >= size.y) && (p.y <= b)
		)
	{
		return true;
	}

	return false;
}








Field::Field() : cell_x_count(ceil(float(0) / 1)), cell_y_count(ceil(float(0) / 1))
{
}

Field::Field(FieldParams fparams, SDL_Rect size, int f_type, SDL_Texture* tex, SDL_Renderer* ren)
	: cell_x_count(ceil(float(fparams.width) / fparams.cparams.tile_size)), cell_y_count(ceil(float(fparams.height) / fparams.cparams.tile_size))
{
	params = fparams;

	this->size.x = size.x;
	this->size.y = size.y;
	this->size.w = size.w;
	this->size.h = size.h;

	field_type = f_type;

	max_cells_count = cell_x_count * cell_y_count;
	cells_states = new int[max_cells_count];
	cells_arr = new Cell*[max_cells_count];

	int tile_size = fparams.cparams.tile_size;

	for ( int i = 0; i < max_cells_count; ++i )
	{
		SDL_Rect cell_area;
		cell_area.x = i % cell_x_count * tile_size + CTRL_PANEL_WIDTH + OFFSET_X;
		cell_area.y = i / cell_x_count * tile_size + OFFSET_Y;
		cell_area.w = tile_size;
		cell_area.h = tile_size;

		std::list<Cell**> neighbours_list = (field_type == FIELD_TYPE_WITH_BORDERS) ? GetNeighboursAddrs(i) : GetNeighboursAddrs2(i);

		cells_arr[i] = new Cell(params.cparams, neighbours_list, cell_area, EMPTY_CELL, i, tex, ren);
		cells_states[i] = EMPTY_CELL;
		cells_arr[i]->Render(tex, ren, cell_area.x, cell_area.y, tile_size, tile_size);
	}

	/*
	for ( int i = 0; i < max_cells_count; ++i )
	{
		std::cout << "\n==============" << std::endl;
		std::cout << "Cell #" << (i+1) << ":" << std::endl;
		std::cout << "=============="<< std::endl;
		std::cout << "(X,Y) = " << "(" << cells_arr[i]->GetX() << "," << cells_arr[i]->GetY() << ")" << std::endl;
		std::cout << "State: " << cells_arr[i]->GetState() << std::endl;
		std::cout << "Index: " << cells_arr[i]->GetIndex() << "\n" << std::endl;
	}
	*/

	std::cout << "\nmax_cells_count   =   " << max_cells_count << std::endl;
	std::cout << "cell_x_count      =   " << cell_x_count << std::endl;
	std::cout << "cell_y_count      =   " << cell_y_count << "\n" << std::endl;

}

Field::Field(const Field& f) : cell_x_count(ceil(float(0) / 1)), cell_y_count(ceil(float(0) / 1))
{
}

Field::Field(Field&& f) : cell_x_count(ceil(float(0) / 1)), cell_y_count(ceil(float(0) / 1))
{
}

Field::~Field()
{
	if ( cells_states )
		delete[] cells_states;

	for ( int i = 0; i < max_cells_count; ++i )
		if ( cells_arr[i] )
			delete cells_arr[i];

	if ( cells_arr )
		delete[] cells_arr;
}

int Field::PointToIdx(SDL_Point p) const
{
	for ( int i = 0; i < max_cells_count; ++i )
	{
		if ( cells_arr[i]->IsPointInCell(p) )
			return i;
	}

	return -1;
}

void Field::SetCell(int idx, int cell_state, SDL_Texture* cell_tex, SDL_Renderer* ren)
{
	for ( int i = 0; i < max_cells_count; ++i )
	{
		if ( i == idx )
		{
			if ( cells_arr[i]->GetState() == cell_state )
			{
				//std::cout << "Cell is already set!" << std::endl;
				return;
			}

			SDL_Rect cell_area = cells_arr[i]->GetArea();
			std::list<Cell**> neighbours_list = (field_type == FIELD_TYPE_WITH_BORDERS) ? GetNeighboursAddrs(i) : GetNeighboursAddrs2(idx);

			Cell* new_cell = nullptr;
			if ( cell_state == ALIVE_CELL )
				new_cell = new AliveCell(params.cparams, neighbours_list, cell_area, cell_state, idx, cell_tex, ren);
			else if ( cell_state == DEAD_CELL )
				new_cell = new DeadCell(params.cparams, neighbours_list, cell_area, cell_state, idx, cell_tex, ren);
			else
				new_cell = new Cell(params.cparams, neighbours_list, cell_area, cell_state, idx, cell_tex, ren);

			Cell* tmp = cells_arr[i];
			delete tmp;

			cells_arr[i] = new_cell;
			cells_states[i] = cell_state;
			int tile_size = params.cparams.tile_size;
			cells_arr[i]->Render(cell_tex, ren, cell_area.x, cell_area.y, tile_size, tile_size);
			return;
		}
	}
}

bool Field::CheckCellsStates(SDL_Texture* alive_cell_tex, SDL_Texture* dead_cell_tex, SDL_Renderer* ren)
{
	int dead_cells_count = 0;
	bool field_changed_state = false;
	bool finish_simulation = false;


	for ( int i = 0; i < max_cells_count; ++i )
	{
		int state = cells_arr[i]->GetState();
		bool dlc = cells_arr[i]->DoLifeCycle();
		SDL_Rect cell_area = cells_arr[i]->GetArea();
		std::list<Cell**> neighbours_list = (field_type == FIELD_TYPE_WITH_BORDERS) ? GetNeighboursAddrs(i) : GetNeighboursAddrs2(i);
		int tile_size = params.cparams.tile_size;


		if ( state == ALIVE_CELL )
		{
			if ( !dlc )
			{
				DeadCell* new_cell = new DeadCell(params.cparams, neighbours_list, cell_area, DEAD_CELL, i, dead_cell_tex, ren);

				AliveCell* tmp = dynamic_cast<AliveCell*>(cells_arr[i]);
				delete tmp;

				cells_arr[i] = new_cell;
				cells_states[i] = DEAD_CELL;
				cells_arr[i]->Render(dead_cell_tex, ren, cell_area.x, cell_area.y, tile_size, tile_size);
				field_changed_state = true;
				++dead_cells_count;
			}
			else
			{
				cells_arr[i]->Render(alive_cell_tex, ren, cell_area.x, cell_area.y, tile_size, tile_size);
			}

			continue;
		}

		if ( (state == DEAD_CELL) || (state == EMPTY_CELL) )
		{
			if ( !dlc )
			{
				AliveCell* new_cell = new AliveCell(params.cparams, neighbours_list, cell_area, ALIVE_CELL, i, alive_cell_tex, ren);

				if ( state == DEAD_CELL )
				{
					DeadCell* tmp = dynamic_cast<DeadCell*>(cells_arr[i]);
					delete tmp;
				}
				else
				{
					Cell* tmp = cells_arr[i];
					delete tmp;
				}

				cells_arr[i] = new_cell;
				cells_states[i] = ALIVE_CELL;
				cells_arr[i]->Render(alive_cell_tex, ren, cell_area.x, cell_area.y, tile_size, tile_size);
				field_changed_state = true;
			}
			else
			{
				cells_arr[i]->Render(dead_cell_tex, ren, cell_area.x, cell_area.y, tile_size, tile_size);
				++dead_cells_count;
			}
		}
	}

	if ( (dead_cells_count >= max_cells_count) || (!field_changed_state) )
		finish_simulation = true;

	return finish_simulation;
}

std::list<Cell**> Field::GetNeighboursAddrs(int cell_idx)
{
	std::list<Cell**> addresses;

	switch ( GetCellPosByIdx(cell_idx) )
	{
		case CELL_POS_LEFT_TOP_CORNER:
			addresses.push_back(cells_arr + 1);
			addresses.push_back(cells_arr + cell_x_count);
			addresses.push_back(cells_arr + cell_x_count + 1);
			break;
		case CELL_POS_RIGHT_TOP_CORNER:
			addresses.push_back(cells_arr + cell_x_count - 1 - 1);
			addresses.push_back(cells_arr + cell_x_count - 1 + cell_x_count);
			addresses.push_back(cells_arr + cell_x_count - 1 + cell_x_count - 1);
			break;
		case CELL_POS_LEFT_BOTTOM_CORNER:
			{
				Cell** lbc = cells_arr + max_cells_count - cell_x_count;
				addresses.push_back(lbc + 1);
				addresses.push_back(lbc - cell_x_count);
				addresses.push_back(lbc - cell_x_count + 1);
			}
			break;
		case CELL_POS_RIGHT_BOTTOM_CORNER:
			addresses.push_back(cells_arr + max_cells_count - 1 - 1);
			addresses.push_back(cells_arr + max_cells_count - 1 - cell_x_count);
			addresses.push_back(cells_arr + max_cells_count - 1 - cell_x_count - 1);
			break;
		case CELL_POS_TOP_EDGE:
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx - 1 + cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx + 1 + cell_x_count);
			break;
		case CELL_POS_BOTTOM_EDGE:
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx - 1 - cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + 1 - cell_x_count);
			break;
		case CELL_POS_LEFT_EDGE:
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count + 1);
			break;
		case CELL_POS_RIGHT_EDGE:
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1);
			break;
		case CELL_POS_INNER:
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count + 1);
	}

	return addresses;
}

std::list<Cell**> Field::GetNeighboursAddrs2(int cell_idx)
{
	std::list<Cell**> addresses;

	switch ( GetCellPosByIdx(cell_idx) )
	{
		case CELL_POS_LEFT_TOP_CORNER:
			addresses.push_back(cells_arr + 1);
			addresses.push_back(cells_arr + cell_x_count - 1);
			addresses.push_back(cells_arr + cell_x_count - 1 + cell_x_count);
			addresses.push_back(cells_arr + max_cells_count - 1);
			addresses.push_back(cells_arr + max_cells_count - cell_x_count);
			addresses.push_back(cells_arr + max_cells_count - cell_x_count + 1);
			addresses.push_back(cells_arr + cell_x_count);
			addresses.push_back(cells_arr + cell_x_count + 1);
			break;
		case CELL_POS_RIGHT_TOP_CORNER:
			addresses.push_back(cells_arr + cell_x_count - 1 - 1);
			addresses.push_back(cells_arr + cell_x_count - 1 + cell_x_count);
			addresses.push_back(cells_arr + cell_x_count - 1 + cell_x_count - 1);
			addresses.push_back(cells_arr);
			addresses.push_back(cells_arr + cell_x_count);
			addresses.push_back(cells_arr + max_cells_count - cell_x_count);
			addresses.push_back(cells_arr + max_cells_count - 1);
			addresses.push_back(cells_arr + max_cells_count - 1 - 1);
			break;
		case CELL_POS_LEFT_BOTTOM_CORNER:
			{
				Cell** lbc = cells_arr + max_cells_count - cell_x_count;
				addresses.push_back(lbc + 1);
				addresses.push_back(lbc - cell_x_count);
				addresses.push_back(lbc - cell_x_count + 1);
				addresses.push_back(cells_arr);
				addresses.push_back(cells_arr + 1);
				addresses.push_back(cells_arr + cell_x_count - 1);
				addresses.push_back(cells_arr + max_cells_count - 1);
				addresses.push_back(cells_arr + max_cells_count - 1 - cell_x_count);
			}
			break;
		case CELL_POS_RIGHT_BOTTOM_CORNER:
			{
				Cell** rbc = cells_arr + max_cells_count - 1;
				addresses.push_back(rbc - 1);
				addresses.push_back(rbc - cell_x_count);
				addresses.push_back(rbc - cell_x_count - 1);
				addresses.push_back(rbc + 1 - cell_x_count - cell_x_count);
				addresses.push_back(rbc + 1 - cell_x_count);
				addresses.push_back(cells_arr);
				addresses.push_back(cells_arr + cell_x_count - 1);
				addresses.push_back(cells_arr + cell_x_count - 1 - 1);
			}
			break;
		case CELL_POS_TOP_EDGE:
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx - 1 + cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx + 1 + cell_x_count);
			addresses.push_back(cells_arr + max_cells_count - cell_x_count + cell_idx - 1);
			addresses.push_back(cells_arr + max_cells_count - cell_x_count + cell_idx);
			addresses.push_back(cells_arr + max_cells_count - cell_x_count + cell_idx + 1);
			break;
		case CELL_POS_BOTTOM_EDGE:
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx - 1 - cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + 1 - cell_x_count);
			addresses.push_back(cells_arr + cell_idx - (max_cells_count - cell_x_count));
			addresses.push_back(cells_arr + cell_idx - (max_cells_count - cell_x_count) - 1);
			addresses.push_back(cells_arr + cell_idx - (max_cells_count - cell_x_count) + 1);
			break;
		case CELL_POS_LEFT_EDGE:
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count + 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1 + cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1 - cell_x_count);
			break;
		case CELL_POS_RIGHT_EDGE:
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1 - cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1 + cell_x_count);
			break;
		case CELL_POS_INNER:
			addresses.push_back(cells_arr + cell_idx - 1);
			addresses.push_back(cells_arr + cell_idx + 1);
			addresses.push_back(cells_arr + cell_idx - cell_x_count);
			addresses.push_back(cells_arr + cell_idx + cell_x_count);
			addresses.push_back(cells_arr + cell_idx - cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx - cell_x_count + 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count - 1);
			addresses.push_back(cells_arr + cell_idx + cell_x_count + 1);
	}

	return addresses;
}

int Field::GetCellPosByIdx(int cell_idx)
{
	if ( cell_idx == 0 )
		return CELL_POS_LEFT_TOP_CORNER;

	if ( cell_idx == (cell_x_count-1) )
		return CELL_POS_RIGHT_TOP_CORNER;

	if ( cell_idx == (max_cells_count-1) )
		return CELL_POS_RIGHT_BOTTOM_CORNER;

	if ( cell_idx == (max_cells_count-cell_x_count) )
		return CELL_POS_LEFT_BOTTOM_CORNER;

	if ( (cell_idx % cell_x_count) == 0 )
		return CELL_POS_LEFT_EDGE;

	if ( ((cell_idx+1) % cell_x_count) == 0 )
		return CELL_POS_RIGHT_EDGE;

	if ( (cell_idx > 0) && (cell_idx < cell_x_count-1) )
		return CELL_POS_TOP_EDGE;

	if ( (cell_idx > max_cells_count-cell_x_count) && (cell_idx < max_cells_count-1) )
		return CELL_POS_BOTTOM_EDGE;

	return CELL_POS_INNER;
}










Game::Game()
{
	window = nullptr;
	renderer = nullptr;
	textures_list = nullptr;
	textures_list_size = 0;
	field = nullptr;
}

Game::Game(const Game& g)
{
}

Game::Game(Game&& g)
{
}

Game::~Game()
{
	for ( int i = 0; i < TEXTURES_COUNT; ++i )
		cleanup(const_cast<SDL_Texture*>(textures_list[i]));

	if ( textures_list )
		delete[] textures_list;

	if ( field )
		delete field;

	cleanup(renderer, window);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int Game::InitLibraries(void)
{
	// Подключение модулей библиотеки SDL(в данном случае всех)
	if ( SDL_Init(SDL_INIT_EVERYTHING) == -1 )
	{
		logSDLError(std::cout, "SDL_Init: ");
		return 0;
	}

	// Инициализация модуля работы с PNG форматом изображений из библиотеки SDL_image
	if ( (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG )
	{
		logSDLError(std::cout, "IMG_Init: ");
		SDL_Quit();
		return 0;
	}

	// Инициализация библиотеки SDL_ttf
	if ( TTF_Init() != 0 )
	{
		logSDLError(std::cout, "TTF_Init");
		IMG_Quit();
		SDL_Quit();
		return 0;
	}

	return 1;
}

SDL_Window* Game::CreateWindow(const char* window_name, int screen_width, int screen_height)
{
	// Создаём окно рабочей области
	window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			screen_width, screen_height, SDL_WINDOW_SHOWN);
	if ( window == nullptr )
	{
		logSDLError(std::cout, "SDL_CreateWindow Error: ");
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return nullptr;
	}

	return window;
}

SDL_Renderer* Game::CreateRenderer(void)
{
	// Создаём объект рендерера(отрисовщик текстур) и прикрепляем его к окну
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if ( renderer == nullptr )
	{
		logSDLError(std::cout, "SDL_CreateRenderer Error: ");
		cleanup(window);
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return nullptr;
	}

	return renderer;
}

const SDL_Texture** Game::LoadTextures(const char* textures_path, int textures_list_size)
{
	if ( textures_list_size < 1 )
	{
		std::cout << "[Game::LoadTextures]" << "(" << this << "): " << "An error has occured during loading textures\n" << std::endl;
		return nullptr;
	}

	std::string res_path = getResourcePath(textures_path);
	textures_list = new const SDL_Texture*[textures_list_size]
	{
			loadTexture(res_path + "background_frame.png", renderer),
			loadTexture(res_path + "control_panel_frame.png", renderer),
			loadTexture(res_path + "control_panel.png", renderer),
			loadTexture(res_path + "cell.png", renderer),
			loadTexture(res_path + "alive_cell.png", renderer),
			loadTexture(res_path + "dead_cell.png", renderer)
	};
	this->textures_list_size = textures_list_size;

	int err_textures_load = 0;
	for ( int i = 0; i < textures_list_size; ++i )
	{
		if ( textures_list[i] == nullptr )
		{
			err_textures_load = 1;
			break;
		}
	}

	if ( err_textures_load )
	{
		std::cout << "[Game::LoadTextures]" << "(" << this << "): " << "An error has occured during loading textures\n" << std::endl;
		for ( int i = 0; i < TEXTURES_COUNT; ++i )
			cleanup(const_cast<SDL_Texture*>(textures_list[i]));
		cleanup(renderer, window);
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return nullptr;
	}

	return textures_list;
}

Field* Game::CreateField(void)
{
	SDL_Rect field_size;
	field_size.x = CTRL_PANEL_WIDTH + OFFSET_X;
	field_size.y = OFFSET_Y;
	field_size.w = state.fparams.width;
	field_size.h = state.fparams.height;

	field = new Field(state.fparams, field_size, state.fparams.ftype, const_cast<SDL_Texture*>(textures_list[CELL_TEXTURE]), renderer);

	return field;
}

int Game::InitGameState(int field_width, int field_height, int sim_speed_mul)
{
	state.started = false;
	state.paused = true;
	state.base_simulation_delay = DEFAULT_BASE_SIMULATION_DELAY;
	state.simulation_speed_multiplier = sim_speed_mul;
	state.fparams.ftype = FIELD_TYPE_WITH_BORDERS;
	state.fparams.width = field_width;
	state.fparams.height = field_height;
	state.fparams.cparams.tile_size = DEFAULT_TILE_SIZE;
	state.density = 0;

	return 1;
}

int Game::Run(void)
{
	// Отображение сцены
	SDL_RenderClear(renderer);
	renderTexture(const_cast<SDL_Texture*>(textures_list[CONTROL_PANEL_TEXTURE]), renderer, OFFSET_X, OFFSET_Y, CTRL_PANEL_WIDTH, state.fparams.height);
	renderTexture(const_cast<SDL_Texture*>(textures_list[BACKGROUND_FRAME_TEXTURE]), renderer, CTRL_PANEL_WIDTH, 0, state.fparams.width + OFFSET_X * 2, state.fparams.height + OFFSET_Y * 2);
	renderTexture(const_cast<SDL_Texture*>(textures_list[CONTROL_PANEL_FRAME_TEXTURE]), renderer, 0, 0, CTRL_PANEL_WIDTH + OFFSET_X * 2, state.fparams.height + OFFSET_Y * 2);

	if ( !CreateField() )
	{
		std::cout << "[Game::Run](" << this << "): " << "Unable to create a game field" << std::endl;
		return 0;
	}
	SDL_RenderPresent(renderer);


	bool quit = false;
	SDL_Event event;

	while ( !quit )
	{
		if ( state.started && !state.paused )
		{
			SDL_RenderClear(renderer);
			renderTexture(const_cast<SDL_Texture*>(textures_list[CONTROL_PANEL_TEXTURE]), renderer, OFFSET_X, OFFSET_Y, CTRL_PANEL_WIDTH, state.fparams.height);
			renderTexture(const_cast<SDL_Texture*>(textures_list[BACKGROUND_FRAME_TEXTURE]), renderer, CTRL_PANEL_WIDTH, 0, state.fparams.width + OFFSET_X * 2, state.fparams.height + OFFSET_Y * 2);
			renderTexture(const_cast<SDL_Texture*>(textures_list[CONTROL_PANEL_FRAME_TEXTURE]), renderer, 0, 0, CTRL_PANEL_WIDTH + OFFSET_X * 2, state.fparams.height + OFFSET_Y * 2);

			quit = field->CheckCellsStates(const_cast<SDL_Texture*>(textures_list[ALIVE_CELL_TEXTURE]), const_cast<SDL_Texture*>(textures_list[DEAD_CELL_TEXTURE]), renderer);
			SDL_RenderPresent(renderer);
			SDL_Delay(state.base_simulation_delay / state.simulation_speed_multiplier);
		}

		while( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )
				quit = true;

			if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_ESCAPE:
						quit = true;
				}
			}

			if ( event.type == SDL_MOUSEBUTTONDOWN )
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				SDL_Point p {x, y};
				//std::cout << "( " << x << ", " << y << " )" << std::endl;

				if ( state.paused )
				{
					if ( IsPointInField(p) )
					{
						int cell_idx = field->PointToIdx(p);
						if ( cell_idx > -1 )
						{
							if ( event.button.button == SDL_BUTTON_LEFT )
								field->SetCell(cell_idx, ALIVE_CELL, const_cast<SDL_Texture*>(textures_list[ALIVE_CELL_TEXTURE]), renderer);
							else if (event.button.button == SDL_BUTTON_RIGHT )
								field->SetCell(cell_idx, EMPTY_CELL, const_cast<SDL_Texture*>(textures_list[CELL_TEXTURE]),renderer);
							SDL_RenderPresent(renderer);
						}
					}
					else if ( IsPointInControlPanel(p))
					{
						if ( event.button.button == SDL_BUTTON_LEFT )
						{
							state.started = true;
							state.paused = false;
							std::cout << "The simulation has been started!" << std::endl;
						}
					}
				}
				else
				{
					if ( IsPointInControlPanel(p))
					{
						if ( event.button.button == SDL_BUTTON_RIGHT )
						{
							state.paused = true;
							std::cout << "The simulation has been paused." << std::endl;
						}
					}
				}
			}
		}
		SDL_Delay(5);
	}

	std::cout << "The simulation has been finished." << std::endl;

	return 1;
}

bool Game::IsPointInField(SDL_Point p)
{
	int a = field->GetX() + field->GetWidth() - 1;
	int b = field->GetY() + field->GetHeight() - 1;

	if  (
			(p.x >= field->GetX()) && (p.x <= a) &&
			(p.y >= field->GetY()) && (p.y <= b)
		)
	{
		return true;
	}

	return false;
}

bool Game::IsPointInControlPanel(SDL_Point p)
{
	return !IsPointInField(p);
}

#endif
