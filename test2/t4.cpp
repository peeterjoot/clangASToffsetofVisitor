
typedef struct _FOO
{
    int x;
} FOO;

typedef struct _BAR
{
   int y[2];
} BAR;

typedef struct _MOO
{
    union
    {
        FOO        foo;
        BAR        bar;
    };
} MOO;
