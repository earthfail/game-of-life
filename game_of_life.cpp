#include<iostream>
#include<fstream>
#include<cstdlib> //for cleaning the console
#include<stdio.h>
#include<string>
//TIL: code::blocks take the definition of a function from where it executes
/*define the width and hight of the board that suits my terminal
  for future:know how to get the height and width of the window
  or better know how to use buffers in opengl and print stuff in them
  */
#define HEIGHT 38
#define WIDTH 100
// cmd can print square but it is not supported on my ubuntu computer
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define DASH static_cast<char>(22)
#define CELL static_cast<char>(254)
#define CLEAR "cls"
#else
#define DASH '-'
#define CELL '*'
#define CLEAR "clear"
#endif
enum State{DEAD,ALIVE};//0 and 1 is converted to false and true
namespace board
{
bool get_file(std::ifstream& coordinates,bool *b1,int height=HEIGHT,int width=WIDTH);
//get the places of alive cell from the file and make everything else DEAD
void show(const bool* b1,int height=HEIGHT,int width=WIDTH);
//show the values of b1
int count_neighbors(const bool* b1,int row,int col,int height=HEIGHT,int width=WIDTH);
//count alive neighbors to a cell
void update(const bool* b1,bool* b2,int height=HEIGHT,int width=WIDTH);
//update b2 according to the rules in wiki
void copy(bool* dist,const bool* source,int height=HEIGHT,int width=WIDTH);
//copy values of b2 to b1
bool compare(const bool* b1,const bool* b3,int height=HEIGHT,int width=WIDTH);
//compare two buffers *NOTE*: b3 because not to be confused with b2 that help in updating
}

// ---------MAIN---------
int main()
{
    bool *mainboard= new bool[HEIGHT*WIDTH],*secboard=new bool[HEIGHT*WIDTH],
    *haltboard=new bool[HEIGHT*WIDTH];
    std::ifstream coordination;
    std::string choices;
    int iterations=100,i;
    char choice;
    do
      {
        i=0;//give the player 100 move
        std::cout<<"options: "<<"(g)lider  "<<"(l)ightweight spaceship  "
                 <<"(b)eacon  "<<"bee(h)ive  "<<"(t)oad  "<<"(e)xit"<<std::endl;
        std::cin>>choice;
        std::cin.ignore(10,'\n');
        switch(choice)
          {
          case 'g': choices="glider.txt"; break;
          case 'l': choices="lwss.txt"; break;
          case 'b': choices="beacon.txt"; break;
          case 'h': choices="beehive.txt"; break;
          case 't': choices="toad.txt"; break;
          case 'e': std::cout<<"Game over"<<std::endl; break;
          default:  choices="-";
            std::cout<<"Please enter a valid input"<<std::endl;
          }
        if(choice=='e') break;//end of game
        if(choices[0]=='-') continue;//loop another time
        coordination.open(choices.c_str());
        if(!board::get_file(coordination,mainboard))//something is wrong
          {
            delete[] mainboard;
            delete[] secboard;
            delete[] haltboard;
            coordination.close();
            return 1;
          }
        while(i<iterations)//show the board and update it
          {
            board::show(mainboard);
            board::copy(haltboard,mainboard);//saving the old board

            board::update(mainboard,secboard);
            board::copy(mainboard,secboard);

            if(board::compare(mainboard,haltboard))//the board didn't change
              break;
            choice=static_cast<char>(getchar());//to delay the results
            if(choice=='e') {choice='-'; break;}//the player wants a new game
            //system(CLEAR);//clear the console *NOT RECOMMENDED
            i++;
          }
        std::cout<<"iterations:"<<i<<std::endl;
        //close the file where the pattern coordinates to use the variable ifstream again
        coordination.close();
      }while(choice!='e');
  //clean up the memory
  delete[] mainboard;
  delete[] secboard;
  delete[] haltboard;

  return 0;
}

bool board::get_file(std::ifstream& coordinates,bool *b1,int height,int width)
{//in the future :make the function return bool and return false when there is an exception
    int i,j;
    for(i=0;i<height;i++)//everything is DEAD
        for(j=0;j<width;j++)
            b1[i*width+j]=DEAD;

    while(!coordinates.eof())//set the specific cell to alive
    {
    coordinates>>i;
    coordinates.ignore(1,' ');
    coordinates>>j;
    coordinates.ignore(1,'\n');
    if(i>=0 && i<height && j>=0 && j<width)
        b1[i*width+j]=ALIVE;
    else
        return false;
    }
    return true;
}
void board::show(const bool* b1,int height,int width)
{
    unsigned char dash=DASH,cell=CELL;
    //for more professional work.
  for(int i=0;i<height;i++)
    {
    for(int j=0;j<width;j++)//bound the row from up
        std::cout<<" "<<dash;
    std::cout<<std::endl;

    for(int j=0;j<width;j++)//bound the cell from left
        if(b1[i*width+j]==ALIVE)
            std::cout<<"|"<<cell;
        else
            std::cout<<"| ";

    std::cout<<"|"<<std::endl;
    //bound the last cell from right and move to a new row
    }
    for(int j=0;j<width;j++)//bound the last row from down and move to a new line
        std::cout<<" "<<dash;
    std::cout<<std::endl;
}
int board::count_neighbors(const bool* b1,int row,int col,int height,int width)
{
  int count=0;
  for(int i=row-1;i<=row+1;i++)
    for(int j=col-1;j<=col+1;j++)//check a 3 by 3 square around the cell
      if(i>=0 && i<height && j>=0 && j<width)//the index is in range
        {
          if(i==row && j==col)//we can't count the cell we got
            continue;
          if(b1[i*width+j]==ALIVE)
            count++;
        }
  return count;
}
void board::update(const bool* b1,bool* b2,int height,int width)
{
  int neighbors;//saving the number of alive
  for(int i=0;i<height;i++)
    for(int j=0;j<width;j++)
      {
        neighbors=board::count_neighbors(b1,i,j,height,width);
        switch(neighbors)
          {//rules:
          case 0:
          case 1: b2[i*width+j]=DEAD; break;
            //less than two neighbors must die
          case 2: if(b1[i*width+j]==DEAD)
                    {b2[i*width+j]=DEAD;
                    break;}
          case 3: b2[i*width+j]=ALIVE; break;
//if the cell if alive and has neighbors of two to three or its dead and has exactly three neighbors
          case 4:
          case 5:
          case 6:
          case 7:// more than three neighbors -> dies
          case 8: b2[i*width+j]=DEAD; break;
          }
      }
}
void board::copy(bool* dist,const bool* source,int height,int width)
{
  for(int i=0;i<height;i++)
    for(int j=0;j<width;j++)
      dist[i*width+j]=source[i*width+j];
}
bool board::compare(const bool* b1,const bool* b3,int height,int width)
{
  for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            if(b1[i*width+j]!=b3[i*width+j])//bound difference
                return false;
  return true;//didn't find any difference
}
