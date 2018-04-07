/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_octal2,
    st_printf_octal3
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper(unsigned int n, int radix, const char *digit) {
    int result;

    if (n < radix) {
        putchar(digit[n]);
        return 1;
    } else {
        result = print_int_helper(n / radix, radix, digit);
        putchar(digit[n % radix]);
        return 1 + result;
    }
}

int print_int(unsigned int n, int radix, const char *digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }

    if (n > 0) {
        return print_int_helper(n, radix, digit);
    }
    if (n == 0) {
        putchar('0');
        return 1;
    } else {
        putchar('-');
        return 1 + print_int_helper(-n, radix, digit);
    }
}

int findn(int num)
{
    int output = 0;
    while(num) {
        num /= 10;
        output++;
    }
    return output;
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
    int chars_printed = 0;
    int int_value = 0;
    char *string_value;
    char char_value;
    //char octal_char;
    va_list args;
    enum printf_state state;

    va_start(args, fs);

    state = st_printf_init;
    //additional arrays for case 'A'
    int *int_arr;
    char *char_arr;
    char** str_arr;
    int arr_len, j = 0,given_width=0, d=0, k=0, num_of_digs=0;
    //if_minus=0->positive->right padding, if_minus=1->negative->left padding
    int if_minus_left=0;
    //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
    int zero_pad=0;
    //0 ->input possitive , 1->input negative
    int if_input_minus=0;



    for (; *fs != '\0'; ++fs) {
        switch (state) {
            case st_printf_init:
                switch (*fs) {
                    case '%':
                        state = st_printf_percent;
                        break;

                    default:
                        putchar(*fs);
                        ++chars_printed;
                }
                break;

            case st_printf_percent:
                switch (*fs) {
                    case '%':
                        putchar('%');
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case 'd':
                        int_value = va_arg(args, int);
                        if(int_value<0){
                            //int_value*=-1;
                            if_input_minus=1;
                        }
                        num_of_digs=findn(int_value);
                        d=given_width-num_of_digs;
                        /*
                        printf("d=%d\n",d);
                        printf("num_of_digs=%d\n",num_of_digs);
                        printf("int_value=%d\n",int_value);
                        printf("if_minus=%d\n",if_input_minus);
                        */
                        //if if_minus==1 -> need to decrease one more char - for the '-'
                        if(if_input_minus==1)
                            d--;
                       //printf("d=%d\n",d);
                        //if d>0 -> there is a need to pad
                        if(d>0) {
                            if(int_value<0){
                                int_value*=-1;
                                //if_input_minus=1;
                            }
                            //padding to the left
                            /*
                            printf("if_minus_left=%d\n",if_minus_left);
                            printf("zero_pad=%d\n",zero_pad);
                            */
                            if(if_minus_left==1) {
                                putchar('-');
                                chars_printed += print_int(int_value, 10, digit);
                                for (; k < d; k++) {
                                    //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                                    if (zero_pad == 1)
                                        putchar('0');
                                    else
                                        putchar(' ');
                                }
                                
                            }
                            //padding to the right
                            else{
                                if(if_input_minus==1)
                                    putchar('-');
                                //printf("int_value= %d",int_value);
                                
                                for (; k < d; k++) {
                                    //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                                    if (zero_pad == 1)
                                        putchar('0');
                                    else
                                        putchar(' ');
                                }
                                chars_printed += print_int(int_value, 10, digit);
                               // if(zero_pad==0)
                                //    putchar('#');
                            }
                        }
                        //else- regular func (without padding)
                        else{
                            if(if_input_minus==1){
                                int_value*=-1;
                                putchar('-');
                            }
                            chars_printed += print_int(int_value, 10, digit);
                        }
                        state = st_printf_init;
                        break;

                    case 'b':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 2, digit);
                        state = st_printf_init;
                        break;

                    case 'o':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 8, digit);
                        state = st_printf_init;
                        break;

                    case 'x':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 16, digit);
                        state = st_printf_init;
                        break;

                    case 'X':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 16, DIGIT);
                        state = st_printf_init;
                        break;

                    case 's':
                        string_value = va_arg(args, char *);
                        d= (int) (given_width - strlen(string_value));
                        //if if_minus==1 -> need to decrease one more char - for the '-'
                        if(if_minus_left==1)
                            d--;
                        //if d>0 -> there is a need to pad
                        if(d>0){
                            //padding to the left
                            if(if_minus_left==1){
                                for(;k<=d;k++){
                                    //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                                    if(zero_pad==1)
                                        putchar('0');
                                    else
                                        putchar(' ');
                                    //printf("k=%d\n",k);
                                }
                                //printf("k=%d\n",k);
                                //print the input string
                                while (*string_value) {
                                    chars_printed++;
                                    putchar(*string_value);
                                    string_value++;
                                }
                            }
                                //padding to the right
                            else{
                                //print the input string
                                while (*string_value) {
                                    chars_printed++;
                                    putchar(*string_value);
                                    string_value++;
                                }
                                //padding
                                for(;k<d;k++){
                                    //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                                    if(zero_pad==1)
                                        putchar('0');
                                    else
                                        putchar(' ');
                                }
                                putchar('#');
                            }
                        }
                        //else->there was no need to pad - normal s func
                        else{
                            while (*string_value) {
                                chars_printed++;
                                putchar(*string_value);
                                string_value++;
                            }
                        }
                        state = st_printf_init;
                        break;

                    case 'u':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 10, digit);
                        state = st_printf_init;
                        break;

                    case 'c':
                        char_value = (char) va_arg(args, int);
                        putchar(char_value);
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case '-':
                        //if_minus_left=0->positive , if_minus_left=1->negative
                        if_minus_left=1;
                        break;

                    case '0':
                        //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                        if(given_width==0)
                            zero_pad=1;
                        given_width+=10*given_width;
                        break;
                    case '1':
                        given_width+=10*given_width+1;
                        break;
                    case '2':
                        given_width+=10*given_width+2;
                        break;
                    case '3':
                        given_width+=10*given_width+3;
                        break;
                    case '4':
                        given_width+=10*given_width+4;
                        break;
                    case '5':
                        given_width+=10*given_width+5;
                        break;
                    case '6':
                        given_width+=10*given_width+6;
                        break;
                    case '7':
                        given_width+=10*given_width+7;
                        break;
                    case '8':
                        given_width+=10*given_width+8;
                        break;
                    case '9':
                        given_width+=10*given_width+9;
                        break;



                    case 'A':
                        ++fs;
                        switch (*fs) {
                            case 'd':
                                int_arr = va_arg(args, int*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    if(int_arr[j]>=0)
                                        chars_printed += print_int(int_arr[j], 10, digit);
                                    else{
                                        putchar('-');
                                        chars_printed += print_int(int_arr[j]*(-1), 10, digit);
                                    }
                                    if (j != arr_len - 1){
                                                putchar(',');
                                                putchar(' ');
                                                
                                    }
                                    
                                }
                                putchar('}');
                                //printf("arr=%d , ken=%d",int_arr,arr_len);
                                break;

                            case 'u':
                                int_arr = va_arg(args, int*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    chars_printed += print_int(int_arr[j], 10, digit);
                                    if (j != arr_len - 1){
                                        putchar(',');
                                        putchar(' ');
                                    }
                                }
                                putchar('}');
                                //printf("arr=%d , ken=%d",int_arr,arr_len);
                                break;

                            case 'b':
                                int_arr = va_arg(args, int*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    chars_printed += print_int(int_arr[j], 2, digit);
                                    if (j != arr_len - 1){
                                     putchar(',');
                                        putchar(' ');   
                                    }                                        
                                }
                                putchar('}');
                                break;

                            case 'o':
                                int_arr = va_arg(args, int*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    chars_printed += print_int(int_arr[j], 8, digit);
                                    if (j != arr_len - 1){
                                     putchar(',');
                                        putchar(' ');   
                                    }                   
                                }
                                putchar('}');
                                break;

                            case 'x':
                                char_arr = va_arg(args, char*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    //toy_printf("%d",char_arr[j]);
                                    chars_printed += print_int(char_arr[j], 16, digit);
                                    if (j != arr_len - 1){
                                     putchar(',');
                                        putchar(' ');   
                                    }             
                                }
                                putchar('}');
                                break;

                            case 'X':
                                char_arr = va_arg(args, char*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    chars_printed += print_int(char_arr[j], 16, DIGIT);
                                    if (j != arr_len - 1){
                                     putchar(',');
                                        putchar(' ');   
                                    }             
                                }
                                putchar('}');
                                break;

                            case 's':
                                str_arr = va_arg(args, char**);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    putchar('"');
                                    toy_printf("%s",str_arr[j]);
                                    //chars_printed += print_int(str_arr[j], 16, DIGIT);
                                    putchar('"');
                                    if (j != arr_len - 1){
                                     putchar(',');
                                        putchar(' ');   
                                    }             
                                }
                                putchar('}');
                                break;
                            case 'c':
                                string_value=va_arg(args,char*);
                                arr_len = va_arg(args, int);
                                putchar('{');
                                for (; j < arr_len; j++) {
                                    toy_printf("%c", string_value[j]);
                                    //chars_printed += print_int(str_arr[j], 16, DIGIT);
                                    if (j != arr_len - 1){
                                     putchar(',');
                                        putchar(' ');   
                                    }             
                                }
                                putchar('}');
                                break;

                            default:break;
                        }


                        state = st_printf_init;
                        break;

                    default:
                        toy_printf("Unhandled format %%%c...\n", *fs);
                        exit(-1);
                }
                break;

            default:
                toy_printf("toy_printf: Unknown state -- %d\n", (int) state);
                exit(-1);
        }

    }

    va_end(args);

    return chars_printed;
}





