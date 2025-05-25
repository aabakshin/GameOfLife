#include "includes/Game.hpp"

static const char* default_textures_path = "resources";

static int CheckFieldWidthParam(const char* field_width_str, int display_width, int display_height)
{
	char* endptr = nullptr;
	int field_width = strtol(field_width_str, &endptr, 10);
	if ( (field_width < MIN_FIELD_WIDTH) || (field_width > display_width - CTRL_PANEL_WIDTH - OFFSET_X * 2) )
	{
		std::cout << "Field width parameter is invalid!" << std::endl;
		std::cout << "Valid values in range [" << MIN_FIELD_WIDTH << " - " << display_width - CTRL_PANEL_WIDTH - OFFSET_X * 2 << "]" << std::endl;
		std::cout << "Set default value!" << std::endl;
		return 0;
	}

	return field_width;
}

static int CheckFieldHeightParam(const char* field_height_str, int display_width, int display_height)
{
	char* endptr = nullptr;
	int field_height = strtol(field_height_str, &endptr, 10);
	if ( (field_height < MIN_WORKSPACE_HEIGHT) || (field_height > display_height - OFFSET_Y * 2) )
	{
		std::cout << "Field height parameter is invalid!" << std::endl;
		std::cout << "Valid values in range [" << MIN_WORKSPACE_HEIGHT << " - " << display_height - OFFSET_Y * 2 << "]" << std::endl;
		std::cout << "Set default value!" << std::endl;
		return 0;
	}

	return field_height;
}

static int CheckSimSpeedMultiplierParam(const char* sim_speed_mul_str)
{
	char* endptr = nullptr;
	int sim_speed_multiplier = strtol(sim_speed_mul_str, &endptr, 10);

	if ( (sim_speed_multiplier < MIN_SIMULATION_SPEED_MULTIPLIER) || (sim_speed_multiplier > MAX_SIMULATION_SPEED_MULTIPLIER) )
	{
		std::cout << "Speed multiplier parameter is invalid!" << std::endl;
		std::cout << "Valid values in range [" << MIN_SIMULATION_SPEED_MULTIPLIER << " - " << MAX_SIMULATION_SPEED_MULTIPLIER << "]" << std::endl;
		std::cout << "Set default value!" << std::endl;
		return 0;
	}

	return sim_speed_multiplier;
}

int main(int argc, char* argv[])
{
	Game game;


	if ( !game.InitLibraries() )
	{
		return 1;
	}

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	auto display_width = dm.w;
	auto display_height = dm.h;

	std::cout << "Current screen resolution: " << display_width << "x" << display_height << std::endl;

	int field_width = 0;
	int field_height = 0;
	int sim_speed_multiplier = 1;
	char* textures_path = nullptr;

	switch ( argc )
	{
		case 1:
			field_width = MIN_FIELD_WIDTH;
			field_height = MIN_WORKSPACE_HEIGHT;
			sim_speed_multiplier = MIN_SIMULATION_SPEED_MULTIPLIER;
			textures_path = const_cast<char*>(default_textures_path);
			break;
		case 2:
			if ( (field_width = CheckFieldWidthParam(argv[1], display_width, display_height)) == 0 )
				field_width = MIN_FIELD_WIDTH;
			field_height = MIN_WORKSPACE_HEIGHT;
			sim_speed_multiplier = MIN_SIMULATION_SPEED_MULTIPLIER;
			textures_path = const_cast<char*>(default_textures_path);
			break;
		case 3:
			if ( (field_width = CheckFieldWidthParam(argv[1], display_width, display_height)) == 0 )
				field_width = MIN_FIELD_WIDTH;
			if ( (field_height = CheckFieldHeightParam(argv[2], display_width, display_height)) == 0 )
				field_height = MIN_WORKSPACE_HEIGHT;
			sim_speed_multiplier = MIN_SIMULATION_SPEED_MULTIPLIER;
			textures_path = const_cast<char*>(default_textures_path);
			break;
		case 4:
			if ( (field_width = CheckFieldWidthParam(argv[1], display_width, display_height)) == 0 )
				field_width = MIN_FIELD_WIDTH;
			if ( (field_height = CheckFieldHeightParam(argv[2], display_width, display_height)) == 0 )
				field_height = MIN_WORKSPACE_HEIGHT;
			if ( (sim_speed_multiplier = CheckSimSpeedMultiplierParam(argv[3])) == 0 )
				sim_speed_multiplier = MIN_SIMULATION_SPEED_MULTIPLIER;
			textures_path = const_cast<char*>(default_textures_path);
			break;
		default:
			if ( (field_width = CheckFieldWidthParam(argv[1], display_width, display_height)) == 0 )
				field_width = MIN_FIELD_WIDTH;
			if ( (field_height = CheckFieldHeightParam(argv[2], display_width, display_height)) == 0 )
				field_height = MIN_WORKSPACE_HEIGHT;
			if ( (sim_speed_multiplier = CheckSimSpeedMultiplierParam(argv[3])) == 0 )
				sim_speed_multiplier = MIN_SIMULATION_SPEED_MULTIPLIER;
			textures_path = argv[4];
	}


	std::cout << "\nCurrent game settings:" << std::endl;
	std::cout << "- Field width:                " << field_width << std::endl;
	std::cout << "- Field height:               " << field_height << std::endl;
	std::cout << "- Simulation speed multiplier: " << sim_speed_multiplier << std::endl;


	if ( !game.CreateWindow("Game Of Life", field_width + CTRL_PANEL_WIDTH + OFFSET_X * 2, field_height + OFFSET_Y * 2) )
	{
		return 1;
	}

	if ( !game.CreateRenderer() )
	{
		return 1;
	}

	if ( !game.LoadTextures(textures_path, TEXTURES_COUNT) )
	{
		return 1;
	}

	if ( !game.InitGameState(field_width, field_height, sim_speed_multiplier) )
	{
		return 1;
	}

	if ( !game.Run() )
	{
		return 1;
	}

	getchar();

	return 0;
}
