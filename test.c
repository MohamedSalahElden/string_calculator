/*
 * test.c
 *	description : simple string calculator
 *  Created on: Dec 29, 2019
 *      Author: SALAH
 */


#include "lcd.h"
#include "keypad.h"

uint8 calculate(uint8 *str , uint8 size);

int main(void){
	LCD_init();			/*initialize LCD (see the function in LCD.c)*/
	uint8 arr[100] ; 	/*array to store the string user will enter*/
	int i = 0;
	while(1){
		arr[i] = KEYPAD_getCharacter(); /*get character form user using keypad module*/
		arr[i+1] = '\0';				/*always terminate the string using null terminator \0*/
		LCD_clearScreen();				/*clear LCD to write updates*/
		/*
		 * the if else statements responsible for shifting the string in case of
		 * number of characters user enters greater than the LCD can print in one line
		 * so the line is shifted left
		 * the shifting means to start printing string from arr + (i-15)
		 * so if the string = 				12+13+14+15+16+17+18+19
		 * the else statemnt prints the string from +15+16+17+18+19
		 * */
		if(i<15){LCD_displayString(arr);		}
		else	{LCD_displayString(arr+(i-15));	}
		/*--------process and calculate the string -----------*/
		uint8 result = calculate(arr , i);
		/*--------print the result simultanously at line 3 of LCD ---------*/
		LCD_goTo(3,0);
		LCD_displayString("result = ");
		LCD_displayNumber(result);
		/*------- increase the counter to keep track of location of printing
		 * ------ and location of entering new character in the array
		 * */
		i++;
		/*----- (busy loop) do noting while user didn't release his hand from the button -------*/
		while(!KEYPAD_keyRelaesed());
	}
}

/*
 * function to check if the element of the string is number of operator
 * */
uint8 isNumber(uint8 character){
	if((character >= '0')&&(character <= '9'))return 1;
	return 0;
}
/*
 * function to make one calculation per time according to
 * left operand l
 * right operand r
 * operator o
 *
 * */
uint8 subCalculate(uint8 l , uint8 r , uint8 o){
	switch (o){
	case '+' :
		return l+r;
		break;
	case '-' :
		return l-r;
		break;
	case '*' :
		return l*r;
		break;
	case '/' :
		return l/r;
		break;
	default:
		break;
	}
}
/*
 * name : calculate
 * argument : *str -> pointer to array of character that represent a string of
 * 						numbers and operators
 * 						ex : "20+30-50*4/100"
 * discription : 1- the algorithm first takes the string as an input *str
 * 				 2- divide the string into two arrays
 * 					num_arr -> includes all numbers in the string (in form of integers not characters)
 * 					op_arr  -> includes all operators arranged as user enter them
 * 					ex   : 10 + 20 + 30 * 2 + 5 / 2
 * 					*str    = ['1','0','+','2','0','+','3','0','*','2','+','5','/','2']
 * 					num_arr = [10 , 20 , 30 , 2 , 5 , 2]
 * 					op_arr  = ['+' , '+' , '*' , '+' , '/']
 * 				 3- once the arrays constructed the calculation loop starts
 * 				 	- loop until the op_arr is empty
 * 				 	- check if there is a multiplication * or division / to start from it's location
 * 				 		and store this location at startAt variable
 * 				 	- num_arr[startAt] = calculate (num_arr[startAt] , num_arr[startAt + 1] , op_arr[startAt]
 * 				 	- then shift num_arr , op_arr left one location starting from startAt
 * 				 	- continue till op_arr is empty
 * 				 	ex      : 1 + 2 * 3
 * 				 	num_arr = 1 , 2 , 3
 * 				 	op_arr  = + , *
 * 				 	startAt = 1   (this is the location of multiply operation)
 * 				 	num_arr = 1 , 6 , 3
 * 				 	op_arr  = + , *
 * 				 	then shift left both of them
 * 				 	num_arr = 1 , 6
 * 				 	op_arr  = +
 * 				 	op_arr isn't empty >>>
 * 				 	num_arr = 7 , 6
 * 				 	op_arr  = +
 * 				 	shift lift
 * 				 	num_arr = 7
 * 				 	op_arr  =
 * 				 	op_arr  = empty then terminate
 * */
uint8 calculate(uint8 *str , uint8 size){
	uint8 op_arr[100] ;
	uint8 num_arr[100] ;
	uint8 temp = 0;
	uint8 op_counter = 0;
	uint8 num_counter = 0;
	uint8 i = 0;
	/*------------convert string to 2 arrays ------------*/
	while(str[i] != '\0'){
		if(isNumber(str[i])){
			temp = (temp*10)+(str[i]-48);
		}
		else{
			num_arr[num_counter] = temp;
			temp = 0;
			num_counter++;
			op_arr[op_counter] = str[i];
			op_counter++;
		}
		i++;
	}
	/*-----------calculate till the number of operation ends---------- */
	while(op_counter-1 > 0){
		uint8 startAt = 0;
		i = 0;
		/*--------check for * or / to start calculation from -------*/
		while(i < op_counter-1){
			if(op_arr[i] == '*' | op_arr[i] == '/'){
				startAt = i;
			}
			i++;
		}
		/*-------calculate and write the result in num_arr ---------*/
		num_arr[startAt] = subCalculate(num_arr[startAt] , num_arr[startAt+1] , op_arr[startAt]);
		op_counter--;
		/*----------shift left both num_arr , op_arr -----------*/
		uint8 r;
		for(r=startAt+1 ; r<99 ; r++){
			num_arr[r] = num_arr[r+1];
		}
		for(r=startAt ; r<99 ; r++){
			op_arr[r] = op_arr[r+1];
		}
	}
	/*----------return then result of the string calculation -------*/
	return num_arr[0];
}

