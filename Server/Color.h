#ifndef COLOR_H
#define COLOR_H

/*
 *	Some of ANSI ESCAPE CODE COLORS 
 *	003[X[;X;X;X;...] <-- ESCAPE CODE 
 *	
 *	X is a parameter  
 */


/* Normal text color */
 
#define BLACK_N "\033[0;30m"
#define RED_N "\033[0;31m"
#define GREEN_N "\033[0;32m"
#define YELLOW_N "\033[0;33m"
#define BLUE_N "\033[0;34m"
#define MAGENTA_N "\033[0;35m"
#define CYAN_N "\033[0;36m"
#define WHITE_N "\033[0;37m"


/* Bold text color */
#define BLACK_B "\033[1;30m"
#define RED_B "\033[1;31m"
#define GREEN_B "\033[1;32m"
#define YELLOW_B "\033[1;33m"
#define BLUE_B "\033[1;34m"
#define MAGENTA_B "\033[1;35m"
#define CYAN_B "\033[1;36m"
#define WHITE_B "\033[1;37m"


#define BLACK_U "\033[4;30m"
#define RED_U "\033[4;31m"
#define GREEN_U "\033[4;32m"
#define YELLOW_U "\033[4;33m"
#define BLUE_U "\033[4;34m"
#define MAGENTA_U "\033[4;35m"
#define CYAN_U "\033[4;36m"
#define WHITE_U "\033[4;37m"


#define RESET "\033[0m"


#endif
