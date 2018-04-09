#include "toy_stdio.h"


int main(int argc, char *argv[]) {


    //---- part a+b --------

    toy_printf("\n---------- part a+b ---------------\n");

    int chars_printed=toy_printf("Hex unsigned: %x\n", 10);
    chars_printed+=toy_printf("Octal unsigned: %d\n", -1);
    chars_printed+=toy_printf("Octal unsigned: %d\n", 1);

    chars_printed+=toy_printf("Octal unsigned: %x\n", -10);

    chars_printed+=toy_printf("Unsigned value: %u\n", 1);

    chars_printed+=toy_printf("Unsigned value: %u\n", -1);

//---part c---------
    toy_printf("\n---------- part   c   ---------------\n");

    int integers_array[] = {-1,2,3,4,5};
    char * strings_array[] = {"This", "is", "array", "of", "strings"};
    int array_size = 5;
    toy_printf("Print array of integers: %Ad\n", integers_array, array_size);
    toy_printf("Print array of strings: %As\n", strings_array, array_size);

//-------  part d ------
    toy_printf("\n---------- part  d  ---------------\n");

    toy_printf("Non-padded string: %2s\n", "str");
    toy_printf("Right-padded string: %6s\n", "str");
    toy_printf("Left-added string: %-6s\n", "str");
    toy_printf("With numeric placeholders: %05d\n", -1);
    toy_printf("With numeric placeholders: %5d\n", -1);
    toy_printf("\n---------- end ---------------\n");

//-------   end of part d-----

}
