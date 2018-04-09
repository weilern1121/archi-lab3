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
    st_printf_octal3,
    st_printf_error
} printf_state;
//TODO - the struct I created
typedef struct {
    char* fs;
    enum printf_state cur_state;
    /* Any extra required args */
} state_args;

//the struct from the web- the return value
typedef struct {
    int printed_chars;
    enum printf_state next_state;
}state_result;
/*
enum state_arr_digs{
    st_printf_init=0,
    st_printf_percent=1,
    st_printf_error=2,
    A_high_state_handler=65,
    dig_9_state_handler=57,
    dig_8_state_handler=56,
    dig_7_state_handler=55,
    dig_6_state_handler=54,
    dig_5_state_handler=53,
    dig_4_state_handler=52,
    dig_3_state_handler=51,
    dig_2_state_handler=50

};*/

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

state_result init_state_result_st_printf_percent(){
    state_result stateResult;
    stateResult.printed_chars=0;
    stateResult.next_state=st_printf_percent;
    return  stateResult;
}

state_result percent_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                   int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad, char *string_value) {
    state_result state_result=init_state_result_st_printf_percent();
    return  state_result;
}

//TODO - the handler funcs
state_result init_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result output;
    output.printed_chars=0;
    switch (*state->fs) {
        case '%':
            output=percent_state_handler(args, out_printed_chars,state, int_value,if_input_minus,num_of_digs, given_width, d, if_minus_left, zero_pad,string_value);
            return output;
        default:
            putchar(*state->fs);
            //++(*out_printed_chars);
            output.printed_chars++;
    }
    output.next_state=st_printf_init;
    return output;
}

state_result d_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *int_value = va_arg(args, int);
    state_result stateResult;
    int k=0;
    stateResult.printed_chars=0;
    if(*int_value<0){
        //int_value*=-1;
        *if_input_minus=1;
    }
    *num_of_digs=findn(*int_value);
    *d=(*given_width)-(*num_of_digs);

    //if if_minus==1 -> need to decrease one more char - for the '-'
    if(*if_input_minus==1)
        *d-=1;

    //if d>0 -> there is a need to pad
    if(*d>0) {
        if(*int_value<0){
            *int_value*=-1;
            //if_input_minus=1;
        }

         if(*if_minus_left==1) {
            putchar('-');
            stateResult.printed_chars += print_int(*int_value, 10, digit);
            for (; k < *d; k++) {
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if (*zero_pad == 1)
                    putchar('0');
                else
                    putchar(' ');
            }

        }
            //padding to the right
        else{
            if(*if_input_minus==1)
                putchar('-');
            for (; k < *d; k++) {
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if (*zero_pad == 1)
                    putchar('0');
                else
                    putchar(' ');
            }
            stateResult.printed_chars += print_int(*int_value, 10, digit);
            // if(zero_pad==0)
            //    putchar('#');
        }
    }
        //else- regular func (without padding)
    else{
        if(*if_input_minus==1){
            *int_value*=-1;
            putchar('-');
        }
        stateResult.printed_chars += print_int(*int_value, 10, digit);
    }
    stateResult.next_state=st_printf_init;
    return  stateResult;
}


state_result b_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 2, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result o_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 8, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result u_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 10, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result x_low_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                 int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 16, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result x_high_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                  int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 16, DIGIT);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result A_high_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                   int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    int *int_arr;
    char *char_arr;
    char** str_arr;
    int arr_len, j = 0;
    //char *string_value;

    ++state->fs;
    switch (*state->fs) {
        case 'd':
            int_arr = va_arg(args, int*);
            arr_len = va_arg(args, int);
            putchar('{');
            for (; j < arr_len; j++) {
                if(int_arr[j]>=0)
                    stateResult.printed_chars += print_int(int_arr[j], 10, digit);
                else{
                    putchar('-');
                    stateResult.printed_chars += print_int(int_arr[j]*(-1), 10, digit);
                }
                if (j != arr_len - 1){
                    putchar(',');
                    putchar(' ');

                }

            }
            putchar('}');
            break;

        case 'u':
            int_arr = va_arg(args, int*);
            arr_len = va_arg(args, int);
            putchar('{');
            for (; j < arr_len; j++) {
                stateResult.printed_chars += print_int(int_arr[j], 10, digit);
                if (j != arr_len - 1){
                    putchar(',');
                    putchar(' ');
                }
            }
            putchar('}');
            break;

        case 'b':
            int_arr = va_arg(args, int*);
            arr_len = va_arg(args, int);
            putchar('{');
            for (; j < arr_len; j++) {
                stateResult.printed_chars += print_int(int_arr[j], 2, digit);
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
                stateResult.printed_chars += print_int(int_arr[j], 8, digit);
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
                stateResult.printed_chars += print_int(char_arr[j], 16, digit);
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
                stateResult.printed_chars += print_int(char_arr[j], 16, DIGIT);
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
    stateResult.next_state=st_printf_init;
    return  stateResult;
}


state_result c_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 16, DIGIT);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result minus_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value) {
//if_minus_left=0->positive , if_minus_left=1->negative
    *if_minus_left=1;
    state_result state_result;
    state_result.printed_chars = 0;
    state_result.next_state = st_printf_percent;
    return state_result;
}


state_result dig_0_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                 int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad, char *string_value){
    //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
    if(*given_width==0)
        *zero_pad=1;
    *given_width+=10*(*given_width);
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_1_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+1;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_2_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+2;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_3_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+3;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_4_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+4;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_5_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+5;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_6_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+6;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_7_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+7;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_8_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value){
    *given_width+=(*given_width)*10+8;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}

state_result dig_9_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                 int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad, char *string_value){
    *given_width+=(*given_width)*10+9;
    state_result state_result=init_state_result_st_printf_percent();
    return state_result;
}


state_result s_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value) {
    state_result stateResult;
    stateResult.printed_chars = 0;
    int k=0;
    string_value = va_arg(args, char *);
    *int_value = va_arg(args, int);
    *d= (int) (*given_width - strlen(string_value));
    //if if_minus==1 -> need to decrease one more char - for the '-'
    if(*if_minus_left==1)
        *d-=1;
    //if d>0 -> there is a need to pad
    if(*d>0){
        //padding to the left
        if(*if_minus_left==1){
            for(;k<=*d;k++){
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if(*zero_pad==1)
                    putchar('0');
                else
                    putchar(' ');
                //printf("k=%d\n",k);
            }
            //printf("k=%d\n",k);
            //print the input string
            while (*string_value) {
                //TODO- not sure if printed_chars++ or out_printed_chars++
                stateResult.printed_chars++;
                putchar(*string_value);
                string_value++;
            }
        }
            //padding to the right
        else{
            //print the input string
            while (*string_value) {
                stateResult.printed_chars++;
                putchar(*string_value);
                string_value++;
            }
            //padding
            for(;k<*d;k++){
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if(*zero_pad==1)
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
            stateResult.printed_chars++;
            putchar(*string_value);
            string_value++;
        }
    }
    stateResult.next_state = st_printf_init;
    return stateResult;
}

state_result error_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                 int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value) {
    state_result stateResult;
    stateResult.printed_chars=0;
    stateResult.next_state=st_printf_error;
    return  stateResult;
}

state_result (*state_arr[128])(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                               int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value);

state_result st_printf_error_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value) {

    toy_printf("Unhandled format %%%c...\n", *state->fs);
    exit(-1);
}

state_result print_percent_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                         int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value) {
    //int int_value = 0;
    //char *string_value;
    //int d = 0, j = 0;
    /*
    index for locals:
    if_minus=0->positive->right padding, if_minus=1->negative->left padding
    zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
    0 ->input possitive , 1->input negative
    */
    state_result sr;

    return state_arr[*(state->fs)](args, out_printed_chars,state, int_value,if_input_minus,num_of_digs, given_width, d, if_minus_left, zero_pad,string_value);
}


/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */


int toy_printf(char *fs, ...) {
    int j=0, chars_printed = 0, given_width=0,num_of_digs=0,zero_pad=0,if_input_minus=0, if_minus_left=0,int_value=0,d=0;
    state_args state;
    char* string_value;
    va_list args;
    state.fs=fs;
    va_start(args, state.fs);
    state.cur_state=st_printf_init;
    state_result state_result;

    //initial the array
    for (; j < 128; j++)
        state_arr[j] = error_state_handler;

    //set the st_ special args
    state_arr[0]= init_state_handler;
    state_arr[1]=print_percent_state_handler;
    state_arr[4]=st_printf_error_handler;

    //set the specific values to the chars tht we use- location by ascii number
    state_arr[65] = A_high_state_handler;
    state_arr[57] = dig_9_state_handler;
    state_arr[56] =dig_8_state_handler;
    state_arr[55] =dig_7_state_handler;
    state_arr[54] =dig_6_state_handler;
    state_arr[53] =dig_5_state_handler;
    state_arr[52] =dig_4_state_handler;
    state_arr[51] =dig_3_state_handler;
    state_arr[50] =dig_2_state_handler;
    state_arr[49] =dig_1_state_handler;
    state_arr[48] = dig_0_state_handler;
    state_arr[45] =minus_state_handler;
    state_arr[37] = percent_state_handler;
    state_arr[100] = d_state_handler;
    state_arr[98] =b_state_handler;
    state_arr[111] = o_state_handler;
    state_arr[120] = x_low_state_handler;
    state_arr[88] =  x_high_state_handler;
    state_arr[115] = s_state_handler;
    state_arr[117] =  u_state_handler;
    state_arr[99] =  c_state_handler;


    for (; *(state.fs) != '\0'; ++(state.fs)) {
        state_result=state_arr[state.cur_state]( args, &chars_printed, &state, &int_value,
        &if_input_minus, &num_of_digs, &given_width, &d, &if_minus_left, &zero_pad,string_value);
        state.cur_state=state_result.next_state;
        chars_printed+=state_result.printed_chars;


    }

    va_end(args);

    return chars_printed;
}

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

state_result percent_state_handler(va_list args, int* out_printed_chars, state_args *state) {
    state_result output;
    output.printed_chars=0;
    output.next_state=st_printf_percent;
    return  output;
}

//TODO - the handler funcs
state_result init_state_handler(va_list args, int* out_printed_chars, state_args *state){
    state_result output;
    output.printed_chars=0;
    switch (*state->fs) {
        case '%':
            output=percent_state_handler(args,out_printed_chars,state);
            return output;
        default:
            putchar(*state->fs);
            //++(*out_printed_chars);
            output.printed_chars++;
    }
    output.next_state=st_printf_init;
    return output;
}

state_result d_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    *int_value = va_arg(args, int);
    state_result stateResult;
    int k=0;
    stateResult.printed_chars=0;
    if(*int_value<0){
        //int_value*=-1;
        *if_input_minus=1;
    }
    *num_of_digs=findn(*int_value);
    *d=(*given_width)-(*num_of_digs);

    //if if_minus==1 -> need to decrease one more char - for the '-'
    if(*if_input_minus==1)
        *d-=1;

    //if d>0 -> there is a need to pad
    if(*d>0) {
        if(*int_value<0){
            *int_value*=-1;
            //if_input_minus=1;
        }

        else if(*if_minus_left==1) {
            putchar('-');
            stateResult.printed_chars += print_int(*int_value, 10, digit);
            for (; k < d; k++) {
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if (*zero_pad == 1)
                    putchar('0');
                else
                    putchar(' ');
            }

        }
            //padding to the right
        else{
            if(*if_input_minus==1)
                putchar('-');
            for (; k < *d; k++) {
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if (*zero_pad == 1)
                    putchar('0');
                else
                    putchar(' ');
            }
            stateResult.printed_chars += print_int(*int_value, 10, digit);
            // if(zero_pad==0)
            //    putchar('#');
        }
    }
        //else- regular func (without padding)
    else{
        if(*if_input_minus==1){
            *int_value*=-1;
            putchar('-');
        }
        stateResult.printed_chars += print_int(*int_value, 10, digit);
    }
    stateResult.next_state=st_printf_init;
    return  stateResult;
}


state_result b_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 2, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result o_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 8, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result u_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 10, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result x_low_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                             int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 16, digit);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result x_high_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                 int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 16, DIGIT);
    stateResult.next_state=st_printf_init;
    return stateResult;
}

state_result c_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                  int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad){
    state_result stateResult;
    stateResult.printed_chars=0;
    *int_value = va_arg(args, int);
    stateResult.printed_chars += print_int(*int_value, 16, DIGIT);
    stateResult.next_state=st_printf_init;
    return stateResult;
}


state_result s_state_handler(va_list args, int* out_printed_chars, state_args *state, int *int_value,
                                  int *if_input_minus, int *num_of_digs, int *given_width, int *d, int *if_minus_left, int *zero_pad,char *string_value) {
    state_result stateResult;
    stateResult.printed_chars = 0;
    int k=0;
    *int_value = va_arg(args, int);
    *string_value = va_arg(args, char *);
    *d= (int) (*given_width - strlen(*string_value));
    //if if_minus==1 -> need to decrease one more char - for the '-'
    if(*if_minus_left==1)
        *d-=1;
    //if d>0 -> there is a need to pad
    if(*d>0){
        //padding to the left
        if(*if_minus_left==1){
            for(;k<=*d;k++){
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if(*zero_pad==1)
                    putchar('0');
                else
                    putchar(' ');
                //printf("k=%d\n",k);
            }
            //printf("k=%d\n",k);
            //print the input string
            while (*string_value) {
                //TODO- not sure if printed_chars++ or out_printed_chars++
                stateResult.printed_chars++;
                putchar(*string_value);
                string_value++;
            }
        }
            //padding to the right
        else{
            //print the input string
            while (*string_value) {
                stateResult.printed_chars++;
                putchar(*string_value);
                string_value++;
            }
            //padding
            for(;k<*d;k++){
                //zero_pad=0->no need to 0-pad, zero_pad=1->need to pad with 0's
                if(*zero_pad==1)
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
            stateResult.printed_chars++;
            putchar(*string_value);
            //TODO- not sure that need to change to *string_value
            string_value++;
        }
    }
    stateResult.next_state = st_printf_init;
    return stateResult;
}

state_result print_percent_state_handler(va_list args, int* out_printed_chars, state_args *state) {
    int chars_printed = 0;
    int int_value = 0;
    char *string_value;
    char char_value;
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
    //state_result state_result1;

    switch (*(state->fs)) {
        case '%':
            /*
            //putchar('%');
            //++chars_printed;
            state->cur_state= st_printf_init;
            state_result1=percent_state_handler(args,out_printed_chars,state);
            return  state_result1;
             */
            return percent_state_handler(args,out_printed_chars,state);

            case 'd':

            //state->cur_state = st_printf_init;
            //break;
                return  d_state_handler(args,out_printed_chars,state,&int_value,
                                        &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);

        case 'b':
            /*
            state->cur_state = st_printf_init;
            break;
             */
            return b_state_handler(args,out_printed_chars,state,&int_value,
                                   &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);

            case 'o':
            /*int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 8, digit);
            state->cur_state = st_printf_init;
            break;*/
                return o_state_handler(args,out_printed_chars,state,&int_value,
                                       &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);

        case 'x':
            /*int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 16, digit);
            state->cur_state = st_printf_init;
            break;*/
            return x_low_state_handler(args,out_printed_chars,state,&int_value,
                                   &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);


        case 'X':
            /*int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 16, DIGIT);
            state->cur_state = st_printf_init;
            break;*/
            return x_high_state_handler(args,out_printed_chars,state,&int_value,
                                       &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);


        case 's':
            /*
            state->cur_state = st_printf_init;
            break;*/
            return s_state_handler(args,out_printed_chars,state,&int_value,
                                        &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad,string_value);

        case 'u':
            /*int_value = va_arg(args, int);
            chars_printed += print_int(int_value, 10, digit);
            state->cur_state = st_printf_init;
            break;*/
            return u_state_handler(args,out_printed_chars,state,&int_value,
                                        &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);


        case 'c':
            /*
            state->cur_state = st_printf_init;
            break;*/
            return c_state_handler(args,out_printed_chars,state,&int_value,
                                   &if_input_minus,&num_of_digs,&given_width,&d,&if_minus_left,&zero_pad);



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
            ++state->fs;
            switch (*state->fs) {
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


            state->cur_state = st_printf_init;
            break;

        default:
            toy_printf("Unhandled format %%%c...\n", *state->fs);
            exit(-1);
    }

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
    //enum printf_state state;

    va_start(args, fs);

    //state = st_printf_init;
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

    //TODO - adittion
    state_args state;
    state.cur_state=st_printf_init;
    state_result state_result;
    for (; *(state.fs) != '\0'; ++(state.fs)) {
        switch (state.cur_state) {
            case st_printf_init:
                state_result=init_state_handler(args, &chars_printed, &state);
                state.cur_state=state_result.next_state;
                chars_printed+=state_result.printed_chars;
                //state.cur_state = init_state_handler(args, &chars_printed, &state);
                //chars_printed += handler_printed_chars;
                break;

            case st_printf_percent:
                state_result=print_percent_state_handler(args, &chars_printed, &state);
                state.cur_state=state_result.next_state;
                chars_printed+=state_result.printed_chars;
                //state.cur_state = print_percent_state_handler(args, &chars_printed, &state);
                break;

            default:
                toy_printf("toy_printf: Unknown state -- %d\n", (int) state.cur_state);
                exit(-1);
        }

    }

    va_end(args);

    return chars_printed;
}





