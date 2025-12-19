#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include "functions.h"

sfBool isColliding(sfSprite* a, sfSprite* b)
{
    sfFloatRect ra = sfSprite_getGlobalBounds(a);
    sfFloatRect rb = sfSprite_getGlobalBounds(b);
    return sfFloatRect_intersects(&ra, &rb, NULL);
}

void gameOver(void)
{
	
}


