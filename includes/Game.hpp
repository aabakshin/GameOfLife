#ifndef GAME_HPP
#define GAME_HPP


#include "services.hpp"
#include "SDL_ext.hpp"
#include <string>
#include <array>
#include <list>


enum
{
			PROGRAM_PARAMS_COUNT	=								  3,
			DEFAULT_TILE_SIZE		=								 20,
			CTRL_PANEL_WIDTH		=							    400,
			MIN_FIELD_WIDTH			=							    800,
			MIN_WORKSPACE_HEIGHT	=							    600,
			OFFSET_X				=								  5,
			OFFSET_Y				=							  	  5,
//			FRAME_FIELD_WIDTH		=		 FIELD_WIDTH + OFFSET_X * 2,
//			WORKSPACE_WIDTH			=	   FIELD_WIDTH+CTRL_PANEL_WIDTH,
//			SCREEN_WIDTH			=    WORKSPACE_WIDTH + OFFSET_X * 2,
//			SCREEN_HEIGHT			=   WORKSPACE_HEIGHT + OFFSET_Y * 2
};

enum
{
			DEFAULT_BASE_SIMULATION_DELAY				=							   1000,
			DEFAULT_SIMULATION_SPEED_MULTIPLIER			=								  5,
			MIN_SIMULATION_SPEED_MULTIPLIER				=								  1,
			MAX_SIMULATION_SPEED_MULTIPLIER				=								200
};

enum
{
			EMPTY_CELL				=								  0,
			ALIVE_CELL				=								  1,
			DEAD_CELL				=								  2
};

enum
{
			TEXTURES_COUNT					=								  6,
			BACKGROUND_FRAME_TEXTURE		=								  0,
			CONTROL_PANEL_FRAME_TEXTURE		=								  1,
			CONTROL_PANEL_TEXTURE			=								  2,
			CELL_TEXTURE					=								  3,
			ALIVE_CELL_TEXTURE				=								  4,
			DEAD_CELL_TEXTURE				=								  5
};

enum
{
			MAX_CELL_NEIGHBOURS				=								  8
};

enum
{
			CELL_POS_LEFT_TOP_CORNER		=								  0,
			CELL_POS_RIGHT_TOP_CORNER		=								  1,
			CELL_POS_LEFT_BOTTOM_CORNER		=								  2,
			CELL_POS_RIGHT_BOTTOM_CORNER	=								  3,
			CELL_POS_TOP_EDGE			    =								  4,
			CELL_POS_BOTTOM_EDGE			=								  5,
			CELL_POS_LEFT_EDGE				=								  6,
			CELL_POS_RIGHT_EDGE			    =								  7,
			CELL_POS_INNER					=								  8
};


enum field_type
{
	FIELD_TYPE_WITH_BORDERS			=			1,
	FIELD_TYPE_TOR					=			2
};

struct CellParams
{
	int tile_size;
};

struct FieldParams
{
	int width;
	int height;
	field_type ftype;
	CellParams cparams;
};

struct GameParams
{
	bool paused;
	bool started;
	int base_simulation_delay;
	int simulation_speed_multiplier;
	FieldParams fparams;
	long long unsigned int density;
};



class Cell
{
	CellParams params;
	SDL_Rect size;
	int state;
	int index;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	std::list<Cell**> neighbours_addrs;
public:
	Cell(CellParams cparams, const std::list<Cell**>& na_list, SDL_Rect area, int cell_state = EMPTY_CELL, int idx = -1, SDL_Texture* tex = nullptr, SDL_Renderer* ren = nullptr);
	int GetState(void) const { return this->state; }
	int GetIndex(void) const { return this->index; }
	int GetX(void) const { return this->size.x; }
	int GetY(void) const { return this->size.y; }
	SDL_Point GetPos(void) const { SDL_Point p { this->size.x, this->size.y }; return p; }
	int GetWidth(void) const { return this->size.w; }
	int GetHeight(void) const { return this->size.h; }
	SDL_Rect GetArea(void) const { return this->size; }
	SDL_Texture* GetTextureObject(void) const { return texture; }
	void SetState(int state) { ( (state >= EMPTY_CELL) && (state <= DEAD_CELL) ) ? this->state = state : this->state = EMPTY_CELL; }
	void SetIndex(int idx) { (idx < 0) ? 0 : this->index = idx; }
	void SetX(int x_val) { this->size.x = x_val; }
	void SetY(int y_val) { this->size.y = y_val; }
	void SetPos(SDL_Point p) { this->size.x = p.x; this->size.y = p.y; }
	void SetWidth(int w_val) { this->size.w = w_val; }
	void SetHeight(int h_val) { this->size.h = h_val; }
	void SetArea(SDL_Rect r) { this->size.x = r.x; this->size.y = r.y; this->size.w = r.w; this->size.h = r.h; }
	void SetTextureObject(const SDL_Texture* new_texture) { this->texture = const_cast<SDL_Texture*>(new_texture); }
	void Render(SDL_Texture* tex, SDL_Renderer* ren, int x, int y, int w, int h) const;
	int GetAliveNeighbours(void) const;
	bool IsPointInCell(SDL_Point p);
	virtual bool DoLifeCycle(void);
	virtual ~Cell() {}
private:
	Cell() {}
	Cell(const Cell& old_cell) {}
	Cell(Cell&& old_cell) {}
	void operator=(const Cell& old_cell) {}
	SDL_Renderer* GetRendererObject(void) const { return renderer; }
	void SetRenderer(const SDL_Renderer* new_renderer) { this->renderer = const_cast<SDL_Renderer*>(new_renderer); }
};


class AliveCell : public Cell
{
public:
	AliveCell(CellParams cparams, const std::list<Cell**>& na_arr, SDL_Rect area, int cell_state = EMPTY_CELL, int idx = -1, SDL_Texture* tex = nullptr, SDL_Renderer* ren = nullptr)
		: Cell(cparams, na_arr, area, cell_state, idx, tex, ren) {}
	virtual bool DoLifeCycle(void);
	virtual ~AliveCell() {}
private:
	void operator=(const AliveCell& old_cell) {}
};


class DeadCell : public Cell
{
public:
	DeadCell(CellParams cparams, const std::list<Cell**>& na_arr, SDL_Rect area, int cell_state = EMPTY_CELL, int idx = -1, SDL_Texture* tex = nullptr, SDL_Renderer* ren = nullptr)
		: Cell(cparams, na_arr, area, cell_state, idx, tex, ren) {}
	virtual bool DoLifeCycle(void);
	virtual ~DeadCell() {}
private:
	void operator=(const DeadCell& old_cell) {}
};





class Field
{
	FieldParams params;
	SDL_Rect size;
	const int cell_x_count;
	const int cell_y_count;
	int max_cells_count;
	int* cells_states;
	Cell** cells_arr;
	int field_type;
public:
	Field(FieldParams fparams, SDL_Rect size, int f_type, SDL_Texture* tex, SDL_Renderer* ren);
	SDL_Rect GetArea(void) const { return size; }
	SDL_Point GetPos(void) const { SDL_Point p; p.x = size.x; p.y = size.y; return p; }
	int GetX(void) const { return size.x; }
	int GetY(void) const { return size.y; }
	int GetWidth(void) const { return size.w; }
	int GetHeight(void) const { return size.h; }
	int GetCellsCount_X(void) const { return cell_x_count; }
	int GetCellsCount_Y(void) const { return cell_y_count; }
	int GetMaxCellsCount(void) const { return max_cells_count; }
	int PointToIdx(SDL_Point p) const;
	void SetCell(int idx, int cell_state, SDL_Texture* cell_tex, SDL_Renderer* ren);
	bool CheckCellsStates(SDL_Texture* alive_cell_tex, SDL_Texture* dead_cell_tex, SDL_Renderer* ren);
	~Field();
private:
	Field();
	Field(const Field& f);
	Field(Field&& f);
	void operator=(const Field& f) {}
	std::list<Cell**> GetNeighboursAddrs(int cell_idx);
	std::list<Cell**> GetNeighboursAddrs2(int cell_idx);
	int GetCellPosByIdx(int cell_idx);
};






class Game
{
	GameParams state;
	SDL_Window* window;
	SDL_Renderer* renderer;
	const SDL_Texture** textures_list;
	int textures_list_size;
	Field* field;
public:
	Game();
	int InitLibraries(void);
	SDL_Window* CreateWindow(const char* window_name, int screen_width, int screen_height);
	SDL_Renderer* CreateRenderer(void);
	const SDL_Texture** LoadTextures(const char* textures_path, int textures_list_size);
	Field* CreateField(void);
	int InitGameState(int field_width, int field_height, int sim_speed_mul);
	int Run(void);
	bool IsPointInField(SDL_Point p);
	bool IsPointInControlPanel(SDL_Point p);
	~Game();
private:
	Game(const Game& g);
	Game(Game&& g);
	void operator=(const Game& g) {}
};

#endif
