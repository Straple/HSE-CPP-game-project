
/*		
*       +------+           +------+
*       |sprite|    ---->  |object|
*       +------+           +------+
*/

enum sprite_t : u8 {

	// world

	SP_BUSH,
	SP_TREE,
	SP_LOG,

	SP_FIREPLACE,

	SP_GRASS_BACKGROUND,

	// player

	SP_PLAYER,

	// enemies

	SP_BAT,
	SP_BAT_INVERSE,

	SP_SLIME,

	// effects

	SP_HIT_EFFECT,
	SP_DEATH_EFFECT,

	// shadows

	SP_LARGE_SHADOW,
	SP_MEDIUM_SHADOW,
	SP_SMALL_SHADOW,

	SP_SLIME_MEDIUM_SHADOW,
	SP_SLIME_LARGE_SHADOW,

	// UI
	SP_CURSOR,
	SP_FOCUS_CURSOR,

	SP_COUNT,
};

sprite Sprites[sprite_t::SP_COUNT];

void read_sprites() {

	std::cout << std::fixed << std::setprecision(4);

#define read(type, name)\
{\
	/*Timer time;*/\
	Sprites[type] = sprite(std::string("Sprites/") + name);\
	/*std::cout << "read sprite: " << time << " " << #type << "\n";*/\
}

	// world
	{
		read(SP_BUSH, "World/bush.png");
		read(SP_TREE, "World/tree.png");
		read(SP_LOG, "World/log.png");

		read(SP_FIREPLACE, "World/fireplace.png");
		
		read(SP_GRASS_BACKGROUND, "World/grass_background.png");
	}
	
	// player
	{
		read(SP_PLAYER, "Player/player.png");
	}

	// enemies
	{
		read(SP_BAT, "Enemies/bat.png");
		read(SP_BAT_INVERSE, "Enemies/bat_inverse.png");

		read(SP_SLIME, "Enemies/slime.eng");
	}

	// effects
	{
		read(SP_HIT_EFFECT, "Effects/hit_effect.png");
		read(SP_DEATH_EFFECT, "Effects/death_effect.png");
	}

	// shadow
	{
		read(SP_LARGE_SHADOW, "Shadows/large_shadow.png");
		read(SP_MEDIUM_SHADOW, "Shadows/medium_shadow.png");
		read(SP_SMALL_SHADOW, "Shadows/small_shadow.png");

		read(SP_SLIME_MEDIUM_SHADOW, "Shadows/slime_medium_shadow.eng");
		read(SP_SLIME_LARGE_SHADOW, "Shadows/slime_large_shadow.eng");
	}

	// UI
	{
		read(SP_CURSOR, "UI/cursor.eng");
		read(SP_FOCUS_CURSOR, "UI/focus_cursor.eng");
	}

#undef read
}
