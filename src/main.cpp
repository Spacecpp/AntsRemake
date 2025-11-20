#include "Alegrinho.h"
#include "init.h"
#include "globals.h"
#include "GameManager.h"



#ifdef PLATFORM_WEB
void logic_func()
{
	if (gameManager)
		gameManager->do_logic();
}


void draw_func()
{
	if (gameManager)
		gameManager->draw();
}
#endif


int main() 
{
	
	if (!program_init() )
	{
		printf("Failed to init game\n");
		return -1;
	}
	
	// auto destruct
	{
		GameManager G;
		gameManager = &G;
		
		#ifdef PLATFORM_WEB
			run_main_loop(logic_func, draw_func);
		#else
			G.main_loop();
		#endif
	}
	
	program_shutdown();
	
	return 0;

}
