#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include <unistd.h>

using namespace std;



//unicode draughts pieces naming
const char bl[4]="\u26c2";
const char bk[4]="\u26c3";
const char wh[4]="\u26c0";
const char wk[4]="\u26c1";

//function prototypes
int printboard(struct piece *board[8][8]);
int checkwin(struct piece *board[8][8]);
int move(int AIsetting, bool player, bool jreq, struct piece *board[8][8], struct piece *empty);
int jump_check(int i, int j, bool player, struct piece *board[8][8]);
int king_check(bool player, struct piece *board[8][8], struct piece *whiteking, struct piece *blackking);


//definition of piece structure
struct piece {
bool empty;
int type;
bool king;
};

//main program
int main(){
//initializing pieces and game status variables
bool player=0;
int AIsetting=0, jump_count=0, error=0;
bool win=0;
srand(time(NULL));
struct piece *board[8][8];

struct piece empty;
empty.empty=1;
empty.type=2;
empty.king=0;

struct piece black;
black.empty=0;
black.type=1;
black.king=0;

struct piece white;
white.empty=0;
white.type=0;
white.king=0;

struct piece whiteking;
whiteking.empty=0;
whiteking.type=0;
whiteking.king=1;

struct piece blackking;
blackking.empty=0;
blackking.type=1;
blackking.king=1;

printf("\n\n\n\nWelcome to DaftDraughts!!\n\n\n\n");


//Board Initialization
for(int i=0;i<8;i++){
	for(int j=0;j<8;j++){

		if(((i+j)%2)==0){
		board[i][j]=NULL;
		}
		if((i<3)&&(((i+j)%2)==1)){
		board[i][j]=&black;	
		}
		if((i>4)&&(((i+j)%2)==1)){
		board[i][j]=&white;	
		}
		if((i>=3)&&(i<=4)&&(((i+j)%2)==1))board[i][j]=&empty;
	}
}

char ai='x';
printf("\n\nWant to play the AI (y/n)? ");

while((!(cin >> ai))||(ai!='Y'&&ai!='y'&&ai!='n'&&ai!='N')){
	cin.clear();
	cin.ignore();
	printf("\nInvalid entry. Try again: ");
}

if(ai==('y'|'Y')){
	printf("\n\nAI plays white or black (w/b)? ");
	while(!(cin >> ai)||(ai!='W'&&ai!='w'&&ai!='B'&&ai!='b')){
		cin.clear();
		cin.ignore();
		printf("\nInvalid entry. Try again: ");
	}
	if(ai==('W'|'w'))AIsetting=1;
	else if(ai==('B'|'b'))AIsetting=2;
}

//turn request cycle print board, ask for moves until you get a valid one, check if the game is ended, update whose turn it is and repeat
	while(win==0){
		printboard(board);
		for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			jump_count+=jump_check(i,j,player, board);
		}
		}
		do{
		error = move(AIsetting, player, jump_count, board, &empty);
		}while(error!=1);
		win=checkwin(board);
		bool kswitch=king_check(player, board, &whiteking, &blackking);
		
			if(kswitch==1){
				for(int i=0;i<20000;i++){
				printf("KING ME!!!!!!!!!!!!!");
				}
			}
		player=((player+1)%2);
		jump_count=0;		
	}
}

//This is where I check for legal moves, handle input errors, and eventually move the pieces
int move(int AIsetting, bool player, bool jreq, struct piece *board[8][8],struct piece *empty){
	char oldletter='\0';
	int oldnumber='\0';
	char newletter='\0';
	int newnumber='\0';
	int x1,y1,x2,y2;
	if(AIsetting==((int)player+1)){

		do{
		x1=rand() % 8;
		y1=rand() % 8;
		}while(((x1+y1)%2)!=1);

//DEBUGGER: this king handling needs improvement or the program will favor non-king pieces
		if(board[x1][y1]->king==1){
		x2=(x1-2) + rand() % 4;
		y2=(y1-2) + rand() % 4;
		}else if(jreq==0){
			if(player==0){
			x2=x1-1;
			y2=(y1-1)+((rand()%2)*2);
			}
			if(player==1){
			x2=x1+1;
			y2=(y1-1)+((rand()%2)*2);
			}
		}else if(jreq==1){
			if(player==0){
			x2=x1-2;
			y2=(y1-2)+4*(rand()%2);
			}
			if(player==1){
			x2=x1+2;
			y2=(y1-2)+4*(rand()%2);
			}
		}
		printf("AI requests to move (%d, %d) to (%d, %d)",x1,y1,x2,y2);
		usleep(1000);
	}else{
		if(player==0)printf("\nTime for %s to get moving: ","WHITE");
		else{
		printf("\nLooks like %s is in charge at this point: ","BLACK");	
		}
		cin >> oldletter >> oldnumber >> newletter >> newnumber;
		if((int)oldletter<73)y1=(int)oldletter-65;
		if((int)oldletter>96)y1=(int)oldletter-97;
		if((int)newletter<73)y2=(int)newletter-65;
		if((int)newletter>96)y2=(int)newletter-97;
		x1=7-(oldnumber-1);
		x2=7-(newnumber-1);
		
	}

	//generic error handling
	if((x1>7)||(x2>7)||(y1>7)||(y2>7)||(x1<0)||(x2<0)||(y1<0)||(y2<0)){printf("Stay on the board!");return 3;}
	if(board[x1][y1]==NULL||board[x2][y2]==NULL){
	printf("\nThat's not a valid square.\n");
	return 0;
	}

	if(player!=board[x1][y1]->type){
	printf("\nYou don't own that piece.\n");
	return 0;
	}

	//dealing with jumps (ignoring most jump-specific error-handling)
	if(jreq>0){	
		if((abs(x1-x2)==2)&&abs(y1-y2)==2&&(board[x2][y2]->empty==1)&&(board[(x2+x1)/2][(y2+y1)/2]->type==((int)player+1)%2)){
			struct piece *buffer;
			buffer=board[x2][y2];
			board[x2][y2]=board[x1][y1];
			board[x1][y1]=buffer;
			board[(x1+x2)/2][(y1+y2)/2]=empty;
			if(1==jump_check(x2,y2,player,board)){
				printboard(board);
				printf("\nLOOKIE THERE --- A DOUBLE JUMP OPENS UP!!\n");
				return 0;
			}else{return 1;}
		}
		printf("\nYou're supposed to make a JUMP, bruh.\n");
		return 9;
	}else{
	//dealing with moving to empty space, including some errors for moving too far or in the wrong direction
		if((board[x1][y1]->king!=1)){
			if((board[x1][y1]->type==1)&&((x2-x1)!=1||(abs(y2-y1)!=1))){
			printf("\nYou can't move that way, sillypants.\n");
			return 2;	
			}else if((board[x1][y1]->type==0)&&((x1-x2)!=1||abs(y2-y1)!=1)){
			printf("\nDon't mess around trying to move that way, goof.\n");
			return 2;
			}
		}

		if((board[x1][y1]->king==1)&&(abs(x1-x2)!=1||abs(y1-y2)!=1)){
		printf("\nJust cause you're a king don't make you always right.\n");
		return 4;
		}

		if((board[x2][y2]->empty)==1){
			struct piece *buffer;
			buffer=board[x2][y2];
			board[x2][y2]=board[x1][y1];
			board[x1][y1]=buffer;
			return 1;
		}
	//If this is still going, it means we missed all the returns, so it's time for a generic message for new input.
	}
	printf("\nThat move isn't registering.\n");
}

int printboard(struct piece *board[8][8]){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n         ");
	for(int i=0;i<8;i++){
	printf(" %c ",(char)(i+65));
	}
	printf("\n");
	for(int i=0;i<8;i++){
		printf("       %d ",(8-i));
			for(int j=0;j<8;j++){
				if((board[i][j]==0)||(board[i][j]->empty)==1){
					if(board[i][j]==0)printf("%s","\u2591\u2591\u2591");
					else{printf("   ");
					}
				}
				else{
					if(board[i][j]->type==0&&board[i][j]->king==0)printf(" %s ",wh);
					if(board[i][j]->type==1&&board[i][j]->king==0)printf(" %s ",bl);
					if(board[i][j]->type==0&&board[i][j]->king==1)printf(" %s ",wk);
					if(board[i][j]->type==1&&board[i][j]->king==1)printf(" %s ",bk);
				} 
		
			}
		printf("\n");
	}
	printf("\n\n\n");
}

int checkwin(struct piece *board[8][8]){
	int numwhite=0;
	int numblack=0;
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
		if(board[i][j]!=NULL){
		if(board[i][j]->type==0)numwhite++;
		if(board[i][j]->type==1)numblack++;		
		}
		}
	}
	if((numwhite||numblack)==0){
		printf("That's it.GOOD GAME!!!!!");
		return 1;
	}
}

int jump_check(int i, int j, bool player, struct piece *board[8][8]){
	if((i+j)%2==1){
		if(board[i][j]->type==player){
			if((board[i][j]->king==1)||player==0){
				if((i-2)>=0&&(j-2)>=0){if(board[i-2][j-2]->empty==1&&(board[i-1][j-1]->type)==((player+1)%2))return 1;}
				if((i-2)>=0&&(j+2)<=7){if(board[i-2][j+2]->empty==1&&(board[i-1][j+1]->type)==((player+1)%2))return 1;}
			}

			if((board[i][j]->king==1)||player==1){
				if((i+2)<8&&(j-2)>=0){if(board[i+2][j-2]->empty==1&&board[i+1][j-1]->type==((player+1)%2))return 1;}
				if((i+2)<8&&(j+2)<8){if(board[i+2][j+2]->empty==1&&board[i+1][j+1]->type==((player+1)%2))return 1;}
			
			}
		}
	}
	return 0;
}

int king_check(bool player, struct piece *board[8][8], struct piece *whiteking, struct piece *blackking){
	if(player==0){
		for(int j=1;j<8;j=(j+2)){
			if(board[0][j]->type==0&&board[0][j]->king==0){
				board[0][j]=whiteking;
				return 1;
			}
		}
	}
	if(player==1){
		for(int j=0;j<8;j=(j+2)){
			if(board[7][j]->type==1&&board[7][j]->king==0){
				board[7][j]=blackking;
				return 1;
			}
		}
	}

return 0;

}

