
typedef struct _A
{
    int x;
} A;

typedef struct _B
{
   int y[2];
} B;

typedef struct _C
{
    union
    {
        A        s;
        B        r;
    };
} C;
