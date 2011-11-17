#include <iostream>
#include <cstdlib>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_keyboard.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_image.h"
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

#define tuk {printf("%d\n", __LINE__);fflush(stdout);}

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

///=======================================
///     Enums
///=======================================
typedef enum {
  SP,
  MP,
  AI,
  EX
}GM;

typedef enum {
  NoneOfThem,
  Pepeliashka,
  BigBadSister
}Snakes;

///=======================================
///     Function prototypes
///=======================================

void run();
void run_sp();
void run_ai();
GM main_menu();
Snakes gotCrash();

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

int main(int argc, char **argv) {
    srand(time(NULL));
    int ret = SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_SetVideoMode(PLAYUGROUND_WIDTH, PLAYUGROUND_HEIGHT, 8, SDL_DOUBLEBUF | SDL_ANYFORMAT );
    
    Game_mode = main_menu(); 
    
    if( !ret )
    {
        memset(body , 0 , sizeof(SDL_Rect)*MAX_LEN);

        /// define the 'initial' snake: head and tail
        for( len = HEAD ; len < MIN_LEN ; len++ )
        {
            body[len].h = body[len].w = HEAD_SIZE;
            body[len].y = Y_START_POS;
            body[len].x = X_START_POS - HEAD_SIZE *  len ;
        }

        

        switch(Game_mode)
        {
            case SP:
                run_sp();
                break;

            case MP:
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
                /// @todo add AI to second snake...
                memset(body2 , 0 , sizeof(SDL_Rect)*MAX_LEN);
                for ( len2 = HEAD; len2 < MIN_LEN ; len2++)
                {
                    body2[len2].h = body2[len2].w = HEAD_SIZE;
                    body2[len2].y = Y_START_POS2;
                    body2[len2].x = X_START_POS2 - HEAD_SIZE *  len2 ;
                }
                run_ai();
                break;
        }
    }

    SDL_Quit();
    return 0;
}

void run()
{
    bool haveFood = false;
    SDL_Event event;
    SDL_Surface *head, *head2, *food, *bckg, *tmp, *background;
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
    SDL_Surface *head, *food, *bckg, *tmp;
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
    tmp = IMG_Load("./rana.gif");
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
    SDL_Surface *head, *head2, *food, *bckg, *tmp, *background;
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
          }else
          {
             Ystep2 = 0;
             Xstep2 = -STEP; 
          }
          if ( body2[HEAD].y < foodRect.y )
          {
             Ystep2 = STEP;
             Xstep2 = 0;
          }else
          {
             Ystep2 = -STEP;
             Xstep2 = 0;
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

GM main_menu()
{
  SDL_Surface *tmp, *cur, *menu;
  SDL_Rect rMenu, rCur;
  SDL_Event event;
  int cStep;

  /// Get time
  GET_TIME(timeOld);
  
  /// load cursor
  tmp = IMG_Load("./glavaaa.gif");
  cur = SDL_DisplayFormat( tmp );
  SDL_FreeSurface( tmp );
  
  /// load Menu
  tmp = IMG_Load("./Menu.gif");
  menu = SDL_DisplayFormat( tmp );
  SDL_FreeSurface( tmp );
  
  rCur.h = cur->h;
  rCur.w = cur->w;
  
  rMenu.h = menu->h;
  rMenu.w = menu->w;
  rMenu.x = (PLAYUGROUND_WIDTH / 2) - (rMenu.w /2 );
  rMenu.h = (PLAYUGROUND_HEIGHT / 2) - (rMenu.h / 2);
 
  rCur.x = rMenu.x - (HEAD_SIZE * 2);
  rCur.y = rMenu.y;
  
  KeyboardHandler keyHandler;
 
  cStep = rMenu.h/4; - HEAD_SIZE;
  
  while(1)
  {
    while( SDL_PollEvent( &event ) )
    {
      keyHandler.handleKeyboardEvent( event);
    }
    
    if (keyHandler.isPressed(SDLK_UP) && (rCur.y > rMenu.y))
    {
        rCur.y -= cStep;
    }else if ( keyHandler.isPressed(SDLK_DOWN) && ((rCur.y + HEAD_SIZE) < (rMenu.y+rMenu.h)) )
    {
        rCur.y += cStep;
    }
    
    if (keyHandler.isPressed(SDLK_RETURN))
    {
        return (GM)((rCur.y - rMenu.y) / cStep); 
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
    SDL_BlitSurface( menu, NULL, screen, &rMenu);
    SDL_BlitSurface( cur, NULL, screen, &rCur);
    SDL_Flip(screen);
    }
  }
  
  SDL_FreeSurface(screen);
  SDL_FreeSurface(menu);
  SDL_FreeSurface(cur);
}
