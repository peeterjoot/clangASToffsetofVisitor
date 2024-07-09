#include <stdio.h>

struct r
{
   int x;
};

struct s
{
   int x;
   struct r r;
};

int main()
{
   printf( "HELLO WORLD\n" );

   return 0;
}
