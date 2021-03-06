#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "menu_core.h"
#include "menu_home.h"
#include "navigation.h"
#include "sha256.h"

#include "menu_encrypt_algo.h"
#include "menu_encrypt_output.h"
#include "menu_encrypt_input.h"
#include "checkInput.h"

#include "checkInput.h"
#include "caesar.h"
#include "morse.h"
#include "oneTimePad.h"
#include "trithemius.h"
#include "vigenere.h"

int extern global_algo;
int extern global_method;
int extern global_options;
int extern global_output;

#define STRING 4096
char extern global_text[STRING];
char extern global_checksum[100];

char encrypt_key[STRING];
char encrypted[STRING];

void menu_encrypt() {
    menu_encrypt_algo(1, global_algo);
}

void encrypt_print(char *output) {
    switch (global_output)
    {
    case 1:
        menu_clear();

        menu_header();
        menu_tab(4);
        printf("║ The encrypted message is:                                                     ║\n");
        menu_line(1);
        printf("\n");
        printf("%s\n", output);
        printf("\n");
        menu_line(1);
        printf("║                              Press S to continue                              ║\n");
        menu_line(1);
        menu_footer();

        int input;
        input = navigation();

        break;
    
    case 2:
        ;
        FILE *file_output;
    
        file_output = fopen("encryptet_message.txt", "w"); //Creat encryptet_message.txt file

        if(file_output == NULL) //Abort if file is not created
            {
                printf("Unable to create file.\n");
                exit(EXIT_FAILURE);
            }

        fputs(output, file_output);
        fclose(file_output);

        menu_clear();

        menu_header();
        menu_tab(4);
        menu_line(1);
        printf("║ The encrypted message was written to the file encryptet_message.txt           ║\n");
        menu_line(5);
        printf("║                              Press S to continue                              ║\n");
        menu_line(1);
        menu_footer();

        input = navigation();
        break;
    }
}

void menu_encrypt_error() {
    
    menu_clear();

    menu_header();
    menu_tab(4);
    menu_line(1);
    printf("║ Error: Unable to encrypt the content                                          ║\n");
    menu_line(5);
    printf("║                              Press S to continue                              ║\n");
    menu_line(1);
    menu_footer();

    int input;
	input = navigation();

    if (input == 0) //Enter
    {
        menu_encrypt_input(1,1);
    } else {
        menu_encrypt_error();
    }
}

void encrypt() {
    FILE *file_checksum;

    switch (global_options) //Check if geneation of a checksum is required
        {
        case 1:
            calcSHA256(global_text);
            file_checksum = fopen("checksum.txt", "w"); //Creat checksum.txt file

            if(file_checksum == NULL) //Abort if file is not created
                {
                    printf("Unable to create file.\n");
                    exit(EXIT_FAILURE);
                }

            fputs(global_checksum, file_checksum);
            fclose(file_checksum);
            break;
        }

        menu_clear();

        menu_header();
        menu_tab(4);

        menu_line(1);

        char *output;
        switch (global_algo)
        {
        case 1: //Caesar 
            printf("║ Please enter a key to encrypt the content. The key should only contain the    ║\n");
            printf("║ numbers 0-9.                                                                  ║\n");
            menu_line(5);
            menu_footer_open();
            printf("║ Please enter a key                                                            ║\n");
            scanf("%s", encrypt_key);
            if (checkInput(encrypt_key, 3) == false) {   //Checks if the key consists only of numbers
                menu_clear();

                menu_header();
                menu_tab(4);
                menu_line(1);
                printf("║ Error: The entered key has the wrong format.                                  ║\n");
                menu_line(5);
                printf("║                              Press S to continue                              ║\n");
                menu_line(1);
                menu_footer();

                int input;
                input = navigation();
                
                menu_encrypt_output(1, global_output); //Calls the output menu again to enter a correct key
            }

            int temp_key;
            temp_key = atoi(encrypt_key); //Convert char to int

            output = caesarEncrypt(global_text, temp_key);

            if (output != NULL) {
                encrypt_print(output);

            } else {
                menu_encrypt_error();
            }
            break;

        case 2:; //Morse
            output = morseEncrypt(global_text);

            if (output != NULL) {
                encrypt_print(output);

            } else {
                menu_encrypt_error();
            }
            break;

        case 3: //Trithemius
            output = trithemiusEncrypt(global_text);

            if (output != NULL) {
                encrypt_print(output);

            } else {
                menu_encrypt_error();
            }
            break;

        case 4: //Vigenere
            printf("║ Please enter a key to encrypt the content. The key should only contain the    ║\n");
            printf("║ keys on the keyboard (ASCII Characters between 32 - 126).                     ║\n");
            menu_line(3);
            menu_footer_open();
            printf("║ Please enter a key                                                            ║\n");
            scanf("%[^\n]", encrypt_key); //scan until user presses enter (until newline char)

            if (checkASCII(encrypt_key) == false) {  //Only accept the printable ASCII characters (codes 32 to 126)
                menu_clear();

                menu_header();
                menu_tab(4);
                menu_line(1);
                printf("║ Error: The entered key has the wrong format.                                  ║\n");
                menu_line(5);
                printf("║                              Press S to continue                              ║\n");
                menu_line(1);
                menu_footer();

                int input;
                input = navigation();

                menu_encrypt_output(1, global_output); //Calls the output menu again to enter a correct key
            }

            output = vigenereEncrypt(global_text, encrypt_key);

            if (output != NULL) {
                encrypt_print(output);

            } else {
                menu_encrypt_error();
            }
            break;

        case 5: //OneTimePad
            printf("║ Please enter a key to encrypt the content. The key should only contain the    ║\n");
            printf("║ keys on the keyboard (ASCII Characters between 32 - 126).                     ║\n");
            printf("║ The key must be as long as the entered message. If you want to generate one,  ║\n");
            printf("║ leave the input empty.                                                        ║\n");
            menu_line(1);
            menu_footer_open();
            printf("║ Please enter a key                                                            ║\n");
            scanf("%[^\n]", encrypt_key); //scan until user presses enter (until newline char)

            if (strcmp(encrypt_key, "") == 0){  //Generates a key, if the user doesn't give a key
                char *encrypt_key_ptr = createKey(strlen(global_text));

                strcpy(encrypt_key, encrypt_key_ptr);

                output = otpEncrypt(global_text, encrypt_key);

            } else if (checkASCII(encrypt_key) == false) {  //Only accept the printable ASCII characters (codes 32 to 126)
                menu_clear();

                menu_header();
                menu_tab(4);
                menu_line(1);
                printf("║ Error: The entered key has the wrong format.                                  ║\n");
                menu_line(5);
                printf("║                              Press S to continue                              ║\n");
                menu_line(1);
                menu_footer();

                int input;
                input = navigation();
                
                menu_encrypt_output(1, global_output); //Calls the output menu again to enter a correct key
            }
            else{
                if (strlen(global_text) != strlen(encrypt_key)) { //If the key is not the same length as the content to encrypt
                    menu_clear();

                    menu_header();
                    menu_tab(4);
                    menu_line(1);
                    printf("║ Error: The entered key has not the same length as the content to encrypt.     ║\n");
                    menu_line(5);
                    printf("║                              Press S to continue                              ║\n");
                    menu_line(1);
                    menu_footer();

                    int input;
                    input = navigation();
                    
                    menu_encrypt_output(1, global_output); //Calls the output menu again to enter a correct key
                } else {
                    output = otpEncrypt(global_text, encrypt_key);
                }
            }

            if (output != NULL) {
                char temp[] = "The key is ";
                
                strcat(temp, encrypt_key);
                strcat(temp, "\n\n");
                strcat(temp, output);

                encrypt_print(temp);

            } else {
                menu_encrypt_error();
            }
            break;
        }
        menu_home(1);

}