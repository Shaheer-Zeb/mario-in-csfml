/* 	Date: 29 November, 2025
	Author: ShaheerK */

#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "functions.h"
#include <unistd.h>

#define GAME_CLOSED 2

int main(void)
{
	sfVideoMode mode = {1200, 720, 32};
	sfRenderWindow* window = sfRenderWindow_create(mode, "Mario", sfResize | sfClose, NULL);
	int windowWidth = mode.width;

	// App Icon
	sfImage* icon = sfImage_createFromFile("images/MarioGameIcon.png");
	if (icon == NULL)
	{
		printf("Unable to open the Mario icon\n");
		return EXIT_FAILURE;
	}

	sfRenderWindow_setIcon(
		window,
		sfImage_getSize(icon).x,
		sfImage_getSize(icon).y,
		sfImage_getPixelsPtr(icon)
		);

    // Background
    sfTexture* bgTexture = sfTexture_createFromFile("images/background.png", NULL);
    if (bgTexture == NULL) 
    {
    	printf("Unable to open the background image.\n");
    	return EXIT_FAILURE;
    }

    sfSprite* bgSprite = sfSprite_create();
    sfSprite_setTexture(bgSprite, bgTexture, sfTrue);

    sfVector2u ws = sfRenderWindow_getSize(window);
    sfVector2u ts = sfTexture_getSize(bgTexture);

    sfSprite_setScale(bgSprite, (sfVector2f){
        (float)ws.x / ts.x,
        (float)ws.y / ts.y
    });

    // Mario
    sfTexture* marioTexture = sfTexture_createFromFile("images/marioSprite.png", NULL);
    sfTexture* marioLeft = sfTexture_createFromFile("images/marioSpriteFacingLeft.png", NULL);
    sfSprite* marioSprite = sfSprite_create();
    sfSprite_setTexture(marioSprite, marioTexture, sfTrue);
    
    float marioSize = 0.45f;
    sfSprite_setScale(marioSprite, (sfVector2f){marioSize, marioSize});
    
    float groundY = 535;
    sfSprite_setPosition(marioSprite, (sfVector2f){110, groundY});

    float marioWidth = sfTexture_getSize(marioTexture).x * marioSize - 200;
  
    // Goomba Textures
    sfTexture* goombaLeft = sfTexture_createFromFile("images/enemies/goombaSpritesheet.png", NULL);
   // sfTexture* goombaRight = sfTexture_createFromFile("images/enemies/goombaSpritesheetLeft.png", NULL);

    sfSprite* goombaSprite = sfSprite_create();
    sfSprite_setTexture(goombaSprite, goombaLeft, sfTrue);
    float goombaSize = 0.4f;
    sfSprite_setScale(goombaSprite, (sfVector2f){goombaSize, goombaSize});

    srand(time(NULL));
    int randX = rand() % windowWidth;
    sfSprite_setPosition(goombaSprite, (sfVector2f){randX, 570});

    float goombaWidth = sfTexture_getSize(goombaLeft).x * 0.4f - 200;
    
    // Implementing the health system and the score system
	int marioHealth = 3;
	int score = 0;
	sfClock* hitClock = sfClock_create();    // prevents instant repeated damage
	sfClock* stompClock = sfClock_create();
	
	// Health bar
	sfRectangleShape* healthBar = sfRectangleShape_create();
	sfRectangleShape_setSize(healthBar, (sfVector2f){120, 20});
	sfRectangleShape_setFillColor(healthBar, sfRed);
	sfRectangleShape_setPosition(healthBar, (sfVector2f){10, 10});


    // Music
    sfMusic* music = sfMusic_createFromFile("sounds/bgm.ogg");
    if (!music) 
    	return EXIT_FAILURE;
	sfMusic_play(music);

    // Jump sound
    sfSoundBuffer* jumpBuffer = sfSoundBuffer_createFromFile("sounds/jumpSound.ogg");
    sfSound* jumpSound = sfSound_create();
    sfSound_setBuffer(jumpSound, jumpBuffer);
    
    // Goomba Dies Sound
    sfSoundBuffer *goombaDieBuffer = sfSoundBuffer_createFromFile("sounds/goombaDies.wav");
    sfSound *goombaDieSound = sfSound_create();
    sfSound_setBuffer(goombaDieSound, goombaDieBuffer);
    
    // Mario Dies sound
    sfSoundBuffer *marioDieBuffer = sfSoundBuffer_createFromFile("sounds/marioDiesSound.ogg");
    sfSound *marioDieSound = sfSound_create();
    sfSound_setBuffer(marioDieSound, marioDieBuffer);
    
    // Loading the font
    sfFont *font = sfFont_createFromFile("fonts/poppins.ttf");
    if (!font)
    {
    	printf("Failed to load the font\n");
    	return EXIT_FAILURE;
    }

	sfText *scoreText = sfText_create();
    
	sfText_setFont(scoreText, font);
	sfText_setString(scoreText, "Score: 0");
	sfText_setCharacterSize(scoreText, 28);
	sfText_setFillColor(scoreText, sfWhite);
	sfText_setPosition(scoreText, (sfVector2f){20, 80});

    // Jump mechanic
    float velocityY = 0;
    float gravity = 0.10f;
    int isJumping = 0;

    // Movement 
    float marioSpeed = 0.7f;
    float goombaSpeed = 0.15f;
    
    // Spritesheet of Mario and Goomba
	sfIntRect marioFrame = {0, 0, 180, 220};
	sfIntRect goombaFrame = {0, 0, 193, 161};
	
	int marioCurrentFrame = 0;
	int goombaCurrentFrame = 0;
	
	sfClock* animClock = sfClock_create();   // controls animation speed
	sfClock* goombaAnimClock = sfClock_create();

	
	sfSprite_setTextureRect(marioSprite, marioFrame);
	sfSprite_setTextureRect(goombaSprite, goombaFrame);

	// Setting the score and the high score
	FILE *highScorePtr = fopen("high_score.txt", "r+");
	if (!highScorePtr)
	{
		printf("Unable to open the High Score file.\n");
		return EXIT_FAILURE;
	}
	char fileHighScoreBuffer[100];

	if (fgets(fileHighScoreBuffer, sizeof(fileHighScoreBuffer), highScorePtr) == NULL)
	{
		printf("Failed to read high score.\n");
		fclose(highScorePtr);
		return EXIT_FAILURE;
	}

	long int highScore = strtol(fileHighScoreBuffer, NULL, 10);
	printf("High Score: %ld\n", highScore);
	
	sfText *highScoreText = sfText_create();
	sfText_setFont(highScoreText, font);
	sfText_setString(highScoreText, "High Score: 0");
	sfText_setCharacterSize(highScoreText, 28);
	sfText_setFillColor(highScoreText, sfWhite);
	sfText_setPosition(highScoreText, (sfVector2f){20, 40});
	
	char highScoreBuffer[64];
	
	sprintf(highScoreBuffer, "High Score: %ld\n", highScore);
	sfText_setString(highScoreText, highScoreBuffer);
	
	// Flags that ensure that speed doesn't increase with each iteration
	int flagFor15 = 0;
	int flagFor30 = 0;
	int flagFor50 = 0;
	
	// Trying to create a filter to display when the Goomaba dies
	sfRectangleShape *colorFilter = sfRectangleShape_create();
	sfRectangleShape_setSize(colorFilter, (sfVector2f) {1428, 720});
	sfRectangleShape_setPosition(colorFilter, (sfVector2f){0, 0});
	sfColor filterColor = {255, 100, 100, 50};
	sfRectangleShape_setFillColor(colorFilter, filterColor);
	
	// Game Over Mechanic
	sfRectangleShape *gameOverScreen = sfRectangleShape_create();
	sfRectangleShape_setSize(gameOverScreen, (sfVector2f) {1428, 720});
	sfRectangleShape_setPosition(gameOverScreen, (sfVector2f){0, 0});
	sfColor gameOverScreenColor = {255, 50, 60, 150};
	sfRectangleShape_setFillColor(gameOverScreen, gameOverScreenColor);
	
	sfText *gameOverText = sfText_create();
	sfText_setFont(gameOverText, font);
	sfText_setString(gameOverText, "Game Over! Press Enter to Continue");
	sfText_setCharacterSize(gameOverText, 40);
	sfText_setFillColor(gameOverText, sfWhite);
	sfText_setPosition(gameOverText, (sfVector2f){330, 300});
	
	int gameOver = 0;
	

    // Main game loop
    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
		if (sfRenderWindow_pollEvent(window, &event) && event.type == sfEvtClosed)
			sfRenderWindow_close(window);

        sfVector2f marioPos = sfSprite_getPosition(marioSprite);
        sfVector2f goombaPos = sfSprite_getPosition(goombaSprite);

        // Mario RIGHT movement
        if (sfKeyboard_isKeyPressed(sfKeyRight) &&
            marioPos.x <= 1300 &&
            (
                (!isColliding(marioSprite, goombaSprite)
                ||
                marioPos.y < groundY
            ))
        )
        {
            sfSprite_move(marioSprite, (sfVector2f){ marioSpeed, 0 });
			if (sfTime_asSeconds(sfClock_getElapsedTime(animClock)) > 0.12f)
    		{
        		marioCurrentFrame = (marioCurrentFrame + 1) % 3;
        		marioFrame.left = marioCurrentFrame * 180;
        		sfSprite_setTextureRect(marioSprite, marioFrame);
        		sfClock_restart(animClock);
    		}
			sfSprite_setTexture(marioSprite, marioTexture, sfFalse);
			sfSprite_setScale(marioSprite, (sfVector2f){marioSize, marioSize});

        }

        // Mario LEFT movement
        else if (sfKeyboard_isKeyPressed(sfKeyLeft) && marioPos.x >= -8 && (!isColliding(marioSprite, goombaSprite)))
		{
    		sfSprite_setTexture(marioSprite, marioLeft, sfFalse);
			sfSprite_move(marioSprite, (sfVector2f){ -marioSpeed, 0 });

			if (sfTime_asSeconds(sfClock_getElapsedTime(animClock)) > 0.12f)
			{
				marioCurrentFrame = (marioCurrentFrame + 1) % 3;
				marioFrame.left = marioCurrentFrame * 180;
				sfSprite_setTextureRect(marioSprite, marioFrame);
				sfClock_restart(animClock);
			}


			sfSprite_setScale(marioSprite, (sfVector2f){ marioSize, marioSize });
		}


        // Jump
        if (sfKeyboard_isKeyPressed(sfKeySpace) && !isJumping)
        {
            velocityY = -7;
            isJumping = 1;
            sfSound_play(jumpSound);
        }

        velocityY += gravity;
        sfSprite_move(marioSprite, (sfVector2f){0, velocityY});

        // Ground collision
        marioPos = sfSprite_getPosition(marioSprite);
        if (marioPos.y >= groundY)
        {
            marioPos.y = groundY;
            sfSprite_setPosition(marioSprite, marioPos);
            velocityY = 0;
            isJumping = 0;
        }
        
		sfFloatRect marioBounds = sfSprite_getGlobalBounds(marioSprite);
		sfFloatRect goombaBounds = sfSprite_getGlobalBounds(goombaSprite);
		
		if (sfFloatRect_intersects(&marioBounds, &goombaBounds, 0))
		{
			float marioBottom = marioBounds.top + marioBounds.height;
			float goombaTop = goombaBounds.top;

			if (marioBottom < goombaTop + 15 && velocityY > 0)
			{
            	// Mario stomped Goomba
				sfSound_play(goombaDieSound); 
            	score++;
            	
            	velocityY = -7;   // bounce Mario upward
            	
				int newX;
				do 
				{
					newX = rand() % windowWidth;
				} while (abs(newX - marioPos.x) < 200);
				goombaFrame.left = 2 * 193; // squashed frame
				sfSprite_setTextureRect(goombaSprite, goombaFrame);
				sfSprite_setPosition(goombaSprite, (sfVector2f){sfSprite_getPosition(goombaSprite).x, 580});
				
				sfRenderWindow_drawSprite(window, goombaSprite, NULL);
				
            	// usleep(250000); // if you want to pause the game for half a second after killing Goomba 

            	sfSprite_setPosition(goombaSprite, (sfVector2f){ newX, 570 });

            	printf("SCORE: %d\n", score);
        	}

        	else if (sfTime_asSeconds(sfClock_getElapsedTime(hitClock)) > 1.0f)
        	{
            	marioHealth--;
            	sfClock_restart(hitClock);
            	
            	sfRenderWindow_drawRectangleShape(window, colorFilter, NULL);
            	sfRenderWindow_display(window);
            	usleep(250000);
            	float randomMarioPosX;
            	do 
				{
					randomMarioPosX = rand() % windowWidth;
				} while (abs(randomMarioPosX - goombaPos.x) < 200);
            	sfSprite_setPosition(marioSprite, (sfVector2f){randomMarioPosX, groundY});

            	printf("HEALTH: %d\n", marioHealth);

            	// Knock back Mario
            	sfSprite_move(marioSprite, (sfVector2f){-20, 0});

            	if (marioHealth <= 0)
            	{
            		printf("GAME OVER\n");
            		sfMusic_stop(music);
                	sfSound_play(marioDieSound);
					sfRenderWindow_drawRectangleShape(window, gameOverScreen, NULL);
					sfRenderWindow_drawText(window, gameOverText, NULL);
					sfRenderWindow_display(window);
					gameOver = 1;
					
					// Checking if the High Score needs to be updated
		
					if (score > (int)highScore)
					{
						printf("New High Score\n");
						fclose(highScorePtr);
		
						highScorePtr = fopen("high_score.txt", "w+"); // using w mode so that the previous high score gets truncated
						fprintf(highScorePtr, "%d", score);
					}
					
					sfEvent eventForGameOver;
            		while (sfRenderWindow_isOpen(window))
            		{
            			if (sfRenderWindow_pollEvent(window, &eventForGameOver) && eventForGameOver.type == sfEvtClosed)
						{
							sfRenderWindow_close(window);
						}

            			
						if (sfKeyboard_isKeyPressed(sfKeyEnter))
						{
							marioHealth = 3;
							score = 0;
							goombaSpeed = 0.15f;
							flagFor15 = flagFor30 = flagFor50 = 0;
							sfMusic_play(music);
							break;
						}
						else if (sfKeyboard_isKeyPressed(sfKeyX)) // pressing X will exit the game
						{
							return GAME_CLOSED;
						}
						
					}
            	}
        	}
    	}

        // Animate Goomba
		if (sfTime_asSeconds(sfClock_getElapsedTime(goombaAnimClock)) > 0.25f)
		{
			goombaCurrentFrame = (goombaCurrentFrame + 1) % 2;
			goombaFrame.left = goombaCurrentFrame * 193;
			sfSprite_setTextureRect(goombaSprite, goombaFrame);
			sfClock_restart(goombaAnimClock);
		}

		// Goomba AI movement
		if (marioPos.x + marioWidth / 2 < goombaPos.x + goombaWidth / 2)
		{
			// Move LEFT
			sfSprite_move(goombaSprite, (sfVector2f){ -goombaSpeed, 0 });
			sfSprite_setScale(goombaSprite, (sfVector2f){ goombaSize, goombaSize });
		}
		else
		{
			// Move RIGHT
			sfSprite_move(goombaSprite, (sfVector2f){ goombaSpeed, 0 });
			sfSprite_setScale(goombaSprite, (sfVector2f){ -goombaSize, goombaSize });
		}


		// Displaying the health bar
		sfVector2f newSize = { marioHealth * 40, 20 };
		sfRectangleShape_setSize(healthBar, newSize);

		// Setting the score
		char scoreBuffer[64];
		sprintf(scoreBuffer, "Score: %d", score);
		sfText_setString(scoreText, scoreBuffer);
		
		// Adding the Goomaba speed increase mechanic
		if (score > 15 && flagFor15 == 0)
		{
			goombaSpeed += 0.1f;
			flagFor15 = 1;
		}
		if (score > 30 && flagFor30 == 0)
		{
			goombaSpeed += 0.2f;
			flagFor30 = 1;
		}
		if (score > 50 && flagFor50 == 0)
		{
			goombaSpeed += 0.1f;
			flagFor50 = 1;
		}


        // Draw
        sfRenderWindow_drawSprite(window, bgSprite, NULL);
        sfRenderWindow_drawSprite(window, marioSprite, NULL);
		sfRenderWindow_drawRectangleShape(window, healthBar, NULL);
		sfRenderWindow_drawText(window, scoreText, NULL);
		sfRenderWindow_drawText(window, highScoreText, NULL);
        sfRenderWindow_drawSprite(window, goombaSprite, NULL);
        sfRenderWindow_display(window);
    }

    // Cleanup
    sfSprite_destroy(bgSprite);
    sfSprite_destroy(marioSprite);
    sfTexture_destroy(marioLeft);
    sfSprite_destroy(goombaSprite);
   
    sfTexture_destroy(bgTexture);
    sfTexture_destroy(marioTexture);
    sfTexture_destroy(goombaLeft);
    //sfTexture_destroy(goombaRight);

    sfSound_destroy(jumpSound);
    sfSoundBuffer_destroy(jumpBuffer);
    sfMusic_destroy(music);
    sfSoundBuffer_destroy(marioDieBuffer);
    sfSound_destroy(marioDieSound);

    sfImage_destroy(icon);
    sfRenderWindow_destroy(window);
    
	sfClock_destroy(hitClock);
	sfClock_destroy(stompClock);
	sfClock_destroy(goombaAnimClock);
	
	sfFont_destroy(font);
	sfText_destroy(scoreText);
	sfText_destroy(highScoreText);
	sfText_destroy(gameOverText);
	
	fclose(highScorePtr);


    return EXIT_SUCCESS;
}

