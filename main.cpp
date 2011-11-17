#include <iostream>
#include <cstdlib>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_keyboard.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <sys/time.h>

using namespace std;

#define GROWTH 5

#define FRAME_PERIOD_ms 90000

#define MIN_LEN 3
#define MAX_LEN 1024

#define HEAD 0
#define HEAD_SIZE 24

#define STEP (HEAD_SIZE/2)

#define FIELD_GRID_WIDTH 30
#define FIELD_GRID_HEIGHT 30

#define PLAYUGROUND_WIDTH  ( FIELD_GRID_WIDTH * HEAD_SIZE )
#define PLAYUGROUND_HEIGHT ( FIELD_GRID_HEIGHT * HEAD_SIZE )

#define X_START_POS ( FIELD_GRID_WIDTH  / 2 ) * HEAD_SIZE  - ( 3 * HEAD_SIZE )
#define Y_START_POS ( FIELD_GRID_HEIGHT / 2 ) * HEAD_SIZE  - ( 3 * HEAD_SIZE )

#define X_START_POS2 ( FIELD_GRID_WIDTH  / 2 ) * HEAD_SIZE + ( 3 * HEAD_SIZE )
#define Y_START_POS2 ( FIELD_GRID_HEIGHT / 2 ) * HEAD_SIZE + ( 3 * HEAD_SIZE )

#define GET_TIME(t) { \
    struct timeval currTime;\
    gettimeofday( &currTime , NULL );\
    t = currTime.tv_usec; \
    }

#define GET_MAX(a , b) { (a > b) ? a : b }

#define ERROR(er) {printf("\nERROR :: %s :: Line %d :: %s \n", __FUNCTION__ ,__LINE__, er);fflush(stdout);}

#define tuk {printf("%d\n", __LINE__);fflush(stdout);}
#define tuk_bk {while(1)tuk}
/*
    check if ptr has a native type type's sizeof
    => ptr is most probably nativa or pointer
    then check for address
*/
#define print_ptr(ptr)                          \
{                                               \
  printf("At line %d, ptr = ", __LINE__);       \
  if(sizeof(ptr) == sizeof(int))                \
    if(&ptr)printf("%p\n", &ptr);               \
      else printf("NULL ptr\n");                \
  fflush(stdout);                               \
}

#define AVOID 0

#define FONT_SIZE 21


///=======================================
///     Enums
///=======================================
typedef enum {
  SP,
  MP,
  AI,
  EX,

  GM_MAX
}GM;

typedef enum {
  NoneOfThem,
  Pepeliashka,
  BigBadSister
}Snakes;

///=======================================
///         Globals
///=======================================
static int len;
static int len2;
static SDL_Rect body[MAX_LEN];
static SDL_Rect body2[MAX_LEN];
static GM Game_mode;
static bool game_paused;
static unsigned long timeNew, timeOld;
static SDL_Surface *screen;
static SDL_Color tColor = {0,150,0};
static SDL_Color tColorActive = {150,0,150};
static TTF_Font *font;


///=======================================
///        Classes and Structures
///=======================================

class KeyboardHandler {
public:
        void handleKeyboardEvent(SDL_Event Event) { 
                  if ( 0 < Event.key.keysym.sym && 400 > Event.key.keysym.sym)
                     keyState[Event.key.keysym.sym] = Event.key.state;
        }

        bool isPressed( SDLKey keyCode) {
            if( SDL_PRESSED == keyState[keyCode])
            {
                keyState[keyCode] = SDL_RELEASED;
                return true;
            }
            else 
                return false;
        }

        bool isReleased( SDLKey keyCode) {
                return ( SDL_RELEASED ==  keyState[keyCode] );
        }

//         void unsetKey( SDLKey keyCode ){
//             keyState[keyCode] = SDL_RELEASED;
//         }
private:
        Uint8 keyState[400];
};

class MenuItem {
  public:
  bool active;
  MenuItem(const char * txt, GM Mode){
    this->text = txt;
    this->active = false;
    this->Mode = Mode;
  }
  SDL_Surface* printText(){
    if (!active)
    {
      return TTF_RenderText_Solid(font, text, tColor);
    }
    else
    {
      return TTF_RenderText_Solid(font, text, tColorActive);
    }
  }
  GM GetMode(){
    return this->Mode;
  }
  private:
  const char* text;
  GM Mode;
  
};


///=======================================
///     Function prototypes
///=======================================

int init();
void run();
void run_sp();
void run_ai();
GM main_menu();
Snakes gotCrash();



int main(int argc, char **argv) {
	int running = 1;
	srand(time(NULL));
    int ret = init();
    
    

    if( !ret )
    {

    	while (running)
    	{
    		Game_mode = main_menu();

			switch(Game_mode)
			{
				case SP:
					// Init Snake 1
					memset(body , 0 , sizeof(SDL_Rect)*MAX_LEN);
					for( len = HEAD ; len < MIN_LEN ; len++ )
					{
						body[len].h = body[len].w = HEAD_SIZE;
						body[len].y = Y_START_POS;
						body[len].x = X_START_POS - HEAD_SIZE *  len ;
					}
					run_sp();
					break;

				case MP:
					// Init Snake 1
					memset(body , 0 , sizeof(SDL_Rect)*MAX_LEN);
					for( len = HEAD ; len < MIN_LEN ; len++ )
					{
						body[len].h = body[len].w = HEAD_SIZE;
						body[len].y = Y_START_POS;
						body[len].x = X_START_POS - HEAD_SIZE *  len ;
					}

					// Init Snake 2
					memset(body2 , 0 , sizeof(SDL_Rect)*MAX_LEN);
					for ( len2 = HEAD; len2 < MIN_LEN ; len2++)
					{
						body2[len2].h = body2[len2].w = HEAD_SIZE;
						body2[len2].y = Y_START_POS2;
						body2[len2].x = X_START_POS2 - HEAD_SIZE *  len2 ;
					}
					run();
					break;

				case AI:
					// Init Snake 1
					memset(body , 0 , sizeof(SDL_Rect)*MAX_LEN);
					for( len = HEAD ; len < MIN_LEN ; len++ )
					{
						body[len].h = body[len].w = HEAD_SIZE;
						body[len].y = Y_START_POS;
						body[len].x = X_START_POS - HEAD_SIZE *  len ;
					}

					// Init Snake 2
					memset(body2 , 0 , sizeof(SDL_Rect)*MAX_LEN);
					for ( len2 = HEAD; len2 < MIN_LEN ; len2++)
					{
						body2[len2].h = body2[len2].w = HEAD_SIZE;
						body2[len2].y = Y_START_POS2;
						body2[len2].x = X_START_POS2 - HEAD_SIZE *  len2 ;
					}
					run_ai();
					break;

				case EX:
					running = 0;
					break;

				default:
					break;
			}
        }
    }

    SDL_Quit();
    return 0;
}

void run()
{
    bool haveFood = false;
    SDL_Event event;
    SDL_Surface *head, *head2, *food, *tmp, *background;
    SDL_Rect bckgRect, foodRect;
    game_paused = false;

    int Xstep = STEP;
    int Ystep = 0;
    int Xstep2 = -STEP;
    int Ystep2 = 0;

    bool go = true;

    foodRect.h = foodRect.w = HEAD_SIZE;

    bckgRect.x = bckgRect.y = 0;
    bckgRect.w = PLAYUGROUND_WIDTH;
    bckgRect.h = PLAYUGROUND_HEIGHT;

    SDL_EnableUNICODE(true);

    if ( NULL == screen ){ 
        return;
    }

    /// load snake's head
    tmp = IMG_Load("./glavaaa.gif");
    head = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// load snake's head
    tmp = IMG_Load("./glavaaa2.gif");
    head2 = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// load food
    tmp = IMG_Load("./apple2.gif");
    food = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// Load Background
    tmp = IMG_Load( "background.png");
    background = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    KeyboardHandler keyHandler;

    GET_TIME(timeOld);

    while( go )
    {
        /// @todo export in a new thread
        while( SDL_PollEvent( &event ) )
        {
          keyHandler.handleKeyboardEvent( event);
        }

        if(keyHandler.isPressed(SDLK_w) && 0 == Ystep  ){              //Up - Down movement of worm
          Ystep = -STEP;
          Xstep = 0;
        }else if (keyHandler.isPressed(SDLK_s) && 0 == Ystep ){
          Ystep = STEP;
          Xstep = 0;
        }

        if(keyHandler.isPressed(SDLK_a) && 0 == Xstep ){              //Left - Right movement of worm
          Ystep = 0;
          Xstep = -STEP;
        }else if(keyHandler.isPressed(SDLK_d) && 0 == Xstep ){
          Ystep = 0;
          Xstep = STEP;
        }

        if(keyHandler.isPressed(SDLK_UP) && 0 == Ystep2 ){              //Up - Down movement of second worm
          Ystep2 = -STEP;
          Xstep2 = 0;
        }else if (keyHandler.isPressed(SDLK_DOWN) && 0 == Ystep2 ){
          Ystep2 = STEP;
          Xstep2 = 0;
        }

        if(keyHandler.isPressed(SDLK_LEFT) && 0 == Xstep2 ){              //Left - Right movement of second worm
          Ystep2 = 0;
          Xstep2 = -STEP;
        }else if(keyHandler.isPressed(SDLK_RIGHT) && 0 == Xstep2 ){
          Ystep2 = 0;
          Xstep2 = STEP;
        }

        if(keyHandler.isPressed(SDLK_ESCAPE) ){         ///Exit
          break;
        }

        if ( keyHandler.isPressed(SDLK_p) )
        {
            game_paused = !game_paused;
        }

        SDL_Delay(1);
        GET_TIME( timeNew );
        if( timeNew - timeOld > FRAME_PERIOD_ms && !game_paused)
        {
            GET_TIME( timeOld );
            switch( gotCrash() )
            {
              case Pepeliashka:
              {
//                   len = MIN_LEN;
//                   go = false;
                  break;
              }
              case BigBadSister:
              {
//                   len2 = MIN_LEN;
//                   go = false;
                  break;
              }
              default:
                 break;
            }

            /// Is Pepeliashka having dinner ?
            if( haveFood && HEAD_SIZE > abs(body[HEAD].x - foodRect.x)  && HEAD_SIZE > abs(body[HEAD].y - foodRect.y) )
            {/// Yes !
                haveFood = false;
                len += GROWTH;
                /// Pepeliashka is growing ( but never gets older ) !
                for( int i = len - GROWTH; i < len; i++ )
                {
                  body[ i ].x = body[i - 1].x;
                  body[ i ].y = body[i - 1].y;
                }
            }

            /// Is Big Bad Sister having dinner ?
            if( haveFood && HEAD_SIZE > abs(body2[HEAD].x - foodRect.x)  && HEAD_SIZE > abs(body2[HEAD].y - foodRect.y) )
            {/// Yes !
                haveFood = false;
                len2 += GROWTH;
                /// Big Bad Sister is growing ( but never gets older ) !
                for( int i = len2 - GROWTH; i < len2; i++ )
                {
                  body2[ i ].x = body2[i - 1].x;
                  body2[ i ].y = body2[i - 1].y;
                }
            }

            /// redraw playground
            SDL_FillRect( screen, NULL, 0);
            SDL_BlitSurface( background , NULL, screen, &bckgRect );

            /// draw snake
            for(int i = 0; i < len; i++)
                SDL_BlitSurface( head , NULL, screen, &body[i] );

            /// draw snake 2
            for(int i = 0; i < len2; i++)
                SDL_BlitSurface( head2 , NULL, screen, &body2[i] );

            /// @todo export in a separate function
            {
                /// add a new piece of food to playground
                while( !haveFood )
                {
                    foodRect.x = ( rand() % (FIELD_GRID_WIDTH - 1)  ) * HEAD_SIZE;
                    foodRect.y = ( rand() % (FIELD_GRID_HEIGHT - 1) ) * HEAD_SIZE;

                    int max_len = GET_MAX( len, len2 );
                    /// check if these (above) coordinates are a free cell or not
                    for( int i = HEAD; i < max_len; i++ )
                    {
                        if( i < len )
                        {
                            if( foodRect.x == body[i].x && foodRect.y == body[i].y )
                            {
                                haveFood = false;
                                break;
                            }
                            else
                                haveFood = true;
                        }

                        if( i < len2 )
                        {
                            if( foodRect.x == body2[i].x && foodRect.y == body2[i].y )
                            {
                                haveFood = false;
                                break;
                            }
                            else
                                haveFood = true;
                        }

                    }
                }
            }
            SDL_BlitSurface( food , NULL, screen, &foodRect );
            SDL_Flip( screen );

            /// @todo export in a separate function
            {
                /// calculate tail position ( each part separately )
                for(int i = len - 1; i ; i--)
                {
                    body[i].x = body[i - 1].x;
                    body[i].y = body[i - 1].y;
                }

                /// move snake's head by 
                body[ HEAD ].x += Xstep;
                body[ HEAD ].y += Ystep;

                /// calculate tail position second snake ( each part separately )
                for(int i = len2 - 1; i ; i--)
                {
                    body2[i].x = body2[i - 1].x;
                    body2[i].y = body2[i - 1].y;
                }

                /// move snake's head by 
                body2[ HEAD ].x += Xstep2;
                body2[ HEAD ].y += Ystep2;

            }
        }
//         SDL_Delay(75);
    }
    SDL_FreeSurface( screen );
    SDL_FreeSurface( head  );
    SDL_FreeSurface( head2  );
}


void run_sp()
{
    bool haveFood = false;
    SDL_Event event;
    SDL_Surface *head, *food, *tmp;
    SDL_Rect bckgRect, foodRect;
    game_paused = false;

    int Xstep = STEP;
    int Ystep = 0;

    bool go = true;

    foodRect.h = foodRect.w = HEAD_SIZE;

    bckgRect.x = bckgRect.y = 0;
    bckgRect.w = PLAYUGROUND_WIDTH;
    bckgRect.h = PLAYUGROUND_HEIGHT;

    SDL_EnableUNICODE(true);

    if ( NULL == screen ){
        return;
    }

    /// load snake's head
    tmp = IMG_Load("./glavaaa.gif");
    head = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// load food
    tmp = IMG_Load("./apple2.gif");
    food = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    KeyboardHandler keyHandler;

    GET_TIME( timeOld );

    while( go )
    {
      /// @todo export in a new thread
        while( SDL_PollEvent( &event ) )
        {
          keyHandler.handleKeyboardEvent( event);
        }

        if(keyHandler.isPressed(SDLK_w) && 0 == Ystep  ){              //Up - Down movement of worm
          Ystep = -STEP;
          Xstep = 0;
        }else if (keyHandler.isPressed(SDLK_s) && 0 == Ystep ){
          Ystep = STEP;
          Xstep = 0;
        }

        if(keyHandler.isPressed(SDLK_a) && 0 == Xstep ){              //Left - Right movement of worm
          Ystep = 0;
          Xstep = -STEP;
        }else if(keyHandler.isPressed(SDLK_d) && 0 == Xstep ){
          Ystep = 0;
          Xstep = STEP;
        }

        if(keyHandler.isPressed(SDLK_ESCAPE) ){                  //Exit
            break;
        }

        if ( keyHandler.isPressed(SDLK_p) )
        {
            game_paused = !game_paused ;
        }

        SDL_Delay(1);
        GET_TIME( timeNew );

        if( timeNew - timeOld > FRAME_PERIOD_ms && !game_paused)
        {
            GET_TIME( timeOld );
            switch( gotCrash() )
            {
              case Pepeliashka:
              {
                  len = MIN_LEN;
//                   go = false;
                  break;
              }

              default:
                 break;
            }

            /// Is Pepeliashka having dinner ?
            if( haveFood && HEAD_SIZE > abs(body[HEAD].x - foodRect.x)  && HEAD_SIZE > abs(body[HEAD].y - foodRect.y) )
            {/// Yes !
                haveFood = false;
                len += GROWTH;
                /// Pepeliashka is growing ( but never gets older ) !
                for( int i = len - GROWTH; i < len; i++ )
                {
                  body[ i ].x = body[i - 1].x;
                  body[ i ].y = body[i - 1].y;
                }
            }

            /// redraw playground
            SDL_FillRect( screen, NULL, 0);

            /// draw snake
            for(int i = 0; i < len; i++)
                SDL_BlitSurface( head , NULL, screen, &body[i] );

            /// @todo export in a separate function
            {
                /// add a new piece of food to playground
                while( !haveFood )
                {
                    foodRect.x = ( rand() % (FIELD_GRID_WIDTH - 1)  ) * HEAD_SIZE;
                    foodRect.y = ( rand() % (FIELD_GRID_HEIGHT - 1) ) * HEAD_SIZE;

                    /// check if these (above) coordinates are a free cell or not
                    for( int i = HEAD; i < len; i++ )
                    {
                        if( foodRect.x == body[i].x && foodRect.y == body[i].y)
                        {
                            haveFood = false;
                            break;
                        }
                        else
                            haveFood = true;
                    }

              }
            }
            SDL_BlitSurface( food , NULL, screen, &foodRect );
            SDL_Flip( screen );

            /// @todo export in a separate function
            {
                /// calculate tail position ( each part separately )
                for(int i = len - 1; i ; i--)
                {
                    body[i].x = body[i - 1].x;
                    body[i].y = body[i - 1].y;
                }

                /// move snake's head by
                body[ HEAD ].x += Xstep;
                body[ HEAD ].y += Ystep;
            }
        }
//         SDL_Delay(75);
    }
    SDL_FreeSurface( screen );
    SDL_FreeSurface( head  );
}


void run_ai()
{
    bool haveFood = false;
    SDL_Event event;
    SDL_Surface *head, *head2, *food, *tmp, *background;
    SDL_Rect bckgRect, foodRect;
    game_paused = false;

    int Xstep = STEP;
    int Ystep = 0;
    int Xstep2 = -STEP;
    int Ystep2 = 0;

    bool go = true;

    foodRect.h = foodRect.w = HEAD_SIZE;

    bckgRect.x = bckgRect.y = 0;
    bckgRect.w = PLAYUGROUND_WIDTH;
    bckgRect.h = PLAYUGROUND_HEIGHT;

    SDL_EnableUNICODE(true);

    if ( NULL == screen ){
        return;
    }

    /// load snake's head
    tmp = IMG_Load("./glavaaa.gif");
    head = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// load snake's head
    tmp = IMG_Load("./glavaaa2.gif");
    head2 = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// load food
    tmp = IMG_Load("./rana.gif");
    food = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    /// Load Background
    tmp = IMG_Load( "background.png");
    background = SDL_DisplayFormat( tmp );
    SDL_FreeSurface( tmp );

    KeyboardHandler keyHandler;

    GET_TIME(timeOld);

    while( go )
    {
        /// @todo export in a new thread
        while( SDL_PollEvent( &event ) )
        {
          keyHandler.handleKeyboardEvent( event);
        }

        if(keyHandler.isPressed(SDLK_w) && 0 == Ystep  ){              //Up - Down movement of worm
          Ystep = -STEP;
          Xstep = 0;
        }else if (keyHandler.isPressed(SDLK_s) && 0 == Ystep ){
          Ystep = STEP;
          Xstep = 0;
        }

        if(keyHandler.isPressed(SDLK_a) && 0 == Xstep ){              //Left - Right movement of worm
          Ystep = 0;
          Xstep = -STEP;
        }else if(keyHandler.isPressed(SDLK_d) && 0 == Xstep ){
          Ystep = 0;
          Xstep = STEP;
        }

        
        { ///Second snake AI move
            if ( body2[HEAD].x < foodRect.x )
            {
              Ystep2 = 0;
              Xstep2 = STEP;
            }else if ( body2[HEAD].x > foodRect.x )
            {
              Ystep2 = 0;
              Xstep2 = -STEP;
            }
            if ( body2[HEAD].y < foodRect.y )
            {
              Ystep2 = STEP;
              Xstep2 = 0;
            }else if ( body2[HEAD].y > foodRect.y )
            {
              Ystep2 = -STEP;
              Xstep2 = 0;
            }
            if (AVOID){
            for (int i = 1; i <= len; i++)
            {
              if ( abs( (body2[HEAD].x + (2*Xstep2)) - body[i].x) == 0 )//  STEP)
              {
                if ( abs( (body2[HEAD].y + STEP) - body[i].y) == 0 )//  STEP)
                {
                  Ystep2 = -STEP;
                  Xstep2 = 0;
                }else
                {
                  Ystep2 = STEP;
                  Xstep2 = 0;
                }
              }
              if ( abs( (body2[HEAD].y + (2*Ystep2)) - body[i].y) == 0 )//  STEP)
              {
                if ( abs( (body2[HEAD].x + STEP) - body[i].y) == 0 )//  STEP)
                {
                  Ystep2 = 0;
                  Xstep2 = -STEP;
                }else
                {
                  Ystep2 = 0;
                  Xstep2 = STEP;
                }
              }

            }
           }
        }



        if(keyHandler.isPressed(SDLK_ESCAPE) ){         ///Exit
          break;
        }

        if ( keyHandler.isPressed(SDLK_p) )
        {
            game_paused = !game_paused;
        }

        SDL_Delay(1);
        GET_TIME( timeNew );
        if( timeNew - timeOld > FRAME_PERIOD_ms && !game_paused)
        {
            GET_TIME( timeOld );
            switch( gotCrash() )
            {
              case Pepeliashka:
              {
//                   len = MIN_LEN;
//                   go = false;
                  break;
              }
              case BigBadSister:
              {
//                   len2 = MIN_LEN;
//                   go = false;
                  break;
              }
              default:
                 break;
            }

            /// Is Pepeliashka having dinner ?
            if( haveFood && HEAD_SIZE > abs(body[HEAD].x - foodRect.x)  && HEAD_SIZE > abs(body[HEAD].y - foodRect.y) )
            {/// Yes !
                haveFood = false;
                len += GROWTH;
                /// Pepeliashka is growing ( but never gets older ) !
                for( int i = len - GROWTH; i < len; i++ )
                {
                  body[ i ].x = body[i - 1].x;
                  body[ i ].y = body[i - 1].y;
                }
            }

            /// Is Big Bad Sister having dinner ?
            if( haveFood && HEAD_SIZE > abs(body2[HEAD].x - foodRect.x)  && HEAD_SIZE > abs(body2[HEAD].y - foodRect.y) )
            {/// Yes !
                haveFood = false;
                len2 += GROWTH;
                /// Big Bad Sister is growing ( but never gets older ) !
                for( int i = len2 - GROWTH; i < len2; i++ )
                {
                  body2[ i ].x = body2[i - 1].x;
                  body2[ i ].y = body2[i - 1].y;
                }
            }

            /// redraw playground
            SDL_FillRect( screen, NULL, 0);
            SDL_BlitSurface( background , NULL, screen, &bckgRect );

            /// draw snake
            for(int i = 0; i < len; i++)
                SDL_BlitSurface( head , NULL, screen, &body[i] );

            /// draw snake 2
            for(int i = 0; i < len2; i++)
                SDL_BlitSurface( head2 , NULL, screen, &body2[i] );

            /// @todo export in a separate function
            {
                /// add a new piece of food to playground
                while( !haveFood )
                {
                    foodRect.x = ( rand() % (FIELD_GRID_WIDTH - 1)  ) * HEAD_SIZE;
                    foodRect.y = ( rand() % (FIELD_GRID_HEIGHT - 1) ) * HEAD_SIZE;

                    int max_len = GET_MAX( len, len2 );
                    /// check if these (above) coordinates are a free cell or not
                    for( int i = HEAD; i < max_len; i++ )
                    {
                        if( i < len )
                        {
                            if( foodRect.x == body[i].x && foodRect.y == body[i].y )
                            {
                                haveFood = false;
                                break;
                            }
                            else
                                haveFood = true;
                        }

                        if( i < len2 )
                        {
                            if( foodRect.x == body2[i].x && foodRect.y == body2[i].y )
                            {
                                haveFood = false;
                                break;
                            }
                            else
                                haveFood = true;
                        }

                    }
                }
            }
            SDL_BlitSurface( food , NULL, screen, &foodRect );
            SDL_Flip( screen );

            /// @todo export in a separate function
            {
                /// calculate tail position ( each part separately )
                for(int i = len - 1; i ; i--)
                {
                    body[i].x = body[i - 1].x;
                    body[i].y = body[i - 1].y;
                }

                /// move snake's head by
                body[ HEAD ].x += Xstep;
                body[ HEAD ].y += Ystep;

                /// calculate tail position second snake ( each part separately )
                for(int i = len2 - 1; i ; i--)
                {
                    body2[i].x = body2[i - 1].x;
                    body2[i].y = body2[i - 1].y;
                }

                /// move snake's head by
                body2[ HEAD ].x += Xstep2;
                body2[ HEAD ].y += Ystep2;

            }
        }
//         SDL_Delay(75);
    }
    SDL_FreeSurface( screen );
    SDL_FreeSurface( head  );
    SDL_FreeSurface( head2  );
}




Snakes gotCrash()
{
    /// Check if first worm is out of the playground
    if( body[HEAD].x + STEP > PLAYUGROUND_WIDTH || body[HEAD].x < 0 )
        return Pepeliashka;

    if( body[HEAD].y + STEP > PLAYUGROUND_HEIGHT || body[HEAD].y < 0 )
        return Pepeliashka;

    
    if (Game_mode != SP){
      /// Check if first worm is has bitten itself or is bitten by the other worm
      for(int i = HEAD + 1; i < len; i++)
      {
            if  ( abs(body[HEAD].x - body[i].x) < STEP &&  abs(body[HEAD].y - body[i].y) < STEP )
              return Pepeliashka;
            if  ( abs(body2[HEAD].x - body[i].x) < STEP && abs(body2[HEAD].y - body[i].y) < STEP){
              len = i;
              return Pepeliashka;
                  }
      }
    }else{
      for(int i = HEAD + 1; i < len; i++)
      {
            if( body[HEAD].x == body[i].x &&  body[HEAD].y == body[i].y )
              return Pepeliashka;
      }
    }
    if (Game_mode != SP){ /// if second worm exists
      /// Check if second worm is out of the playground
      if( body2[HEAD].x + STEP > PLAYUGROUND_WIDTH || body2[HEAD].x < 0 )
          return BigBadSister;

      if( body2[HEAD].y + STEP > PLAYUGROUND_HEIGHT || body2[HEAD].y < 0 )
          return BigBadSister;

      /// Check if second worm is has bitten itself or is bitten by the other worm
      for(int i = HEAD + 1; i < len2; i++)
      {
          if ( abs(body2[HEAD].x - body2[i].x) < STEP &&  abs(body2[HEAD].y - body2[i].y) < STEP )
            return BigBadSister;
          if ( abs(body[HEAD].x - body2[i].x) < STEP && abs(body[HEAD].x - body2[i].y) < STEP){
            len2=i;
            return BigBadSister;
          }
      }
    }

    return NoneOfThem;
}
#define MENU_OPTS 4
GM main_menu()
{
  SDL_Surface *txt[GM_MAX];
  SDL_Rect rMenu;
  SDL_Event event;
  int activeItem;
  /// Get time
  GET_TIME(timeOld);
  
  rMenu.h = 20;
  rMenu.w = 100;
  rMenu.x = (PLAYUGROUND_WIDTH / 2) - rMenu.w ;
  rMenu.y = (PLAYUGROUND_HEIGHT / 2) - rMenu.h*2 ;
  
  KeyboardHandler keyHandler;

  MenuItem *mItems[GM_MAX];
//   mItems = new MenuItem*[GM_MAX];
  mItems[0] = new MenuItem("{ Single Player }", SP);
  mItems[1] = new MenuItem("{ Multy Player }", MP);
  mItems[2] = new MenuItem("{ Player VS AI }", AI);
  mItems[3] = new MenuItem("{ Exit }", EX);

  mItems[0]->active = true;
  activeItem = 0;
  
  while(1)
  {
    while( SDL_PollEvent( &event ) )
    {
      keyHandler.handleKeyboardEvent( event);
    }
    
    if (keyHandler.isPressed(SDLK_UP) && (activeItem > 0) )
    {
          mItems[activeItem]->active = false;
          mItems[--activeItem]->active = true;
    }else if ( keyHandler.isPressed(SDLK_DOWN) && (activeItem < GM_MAX - 1) )
    {
          mItems[activeItem]->active = false;
          mItems[++activeItem]->active = true;
    }
    
    if (keyHandler.isPressed(SDLK_RETURN))
    {   
        return mItems[activeItem]->GetMode();
    }
    
    if (keyHandler.isPressed(SDLK_ESCAPE))
    {
        return EX;
    }

    SDL_Delay(50);
    GET_TIME( timeNew );
    if( timeNew - timeOld > FRAME_PERIOD_ms)
    {
    SDL_FillRect( screen, NULL, 0);



    for( int i = 0; i < GM_MAX; i++ )
    {
        rMenu.y = ((PLAYUGROUND_HEIGHT / 2) - rMenu.h*2) + (32 * i);
        txt[i] = mItems[i]->printText() ;
        SDL_BlitSurface( txt[i], NULL, screen, &rMenu);
    }
    SDL_Flip(screen);
    }
  }
  
  SDL_FreeSurface(screen);
  for( int i = 0; i < GM_MAX; i++ )
  SDL_FreeSurface(txt[i]);
}

///  Init everything and return != 0 if error
int init()
{
  int ret = 0;

  ret = SDL_Init(SDL_INIT_VIDEO);
  if ( ret )
  {
      ERROR("SDL failed to init");
      return ret;
  }
  screen = SDL_SetVideoMode(PLAYUGROUND_WIDTH, PLAYUGROUND_HEIGHT, 8, SDL_DOUBLEBUF | SDL_ANYFORMAT );
  if ( !screen ){
      ERROR("SDL failed to set the video mode");
      return -1;
  }
  ret = TTF_Init();
  if ( ret )
  {
      ERROR("SDL_TTF failed to init");
      return ret;
  }
  font = TTF_OpenFont("./Purisa.ttf", FONT_SIZE);
  if ( !font )
  {
    ERROR("Error Loading text");
    return -1;
  }
  
  
  return ret;
}
