/** \file SEfile-cli.c
 *  \brief In this file you will find the implementation
 *         of the public function already described in
 *         \ref SEfile-cli.h
 *   \authors Daniele Castro
 *   \date 04/30/2019
 */

#include "SEfile-cli.h"

int main(int argc, char *argv[]) {
    int i, e, cmd = -1;
    char *opts[6] = {NULL};
    for(i = 0; i < argc ; i++)   {
        for(e = 0; e < 6; e++)  {
            if(!strcmp(commands[e], argv[i]))   cmd = e;
            if(!strcmp(options[e], argv[i]))    {
                opts[e] = argv[i+1];
            }
        }
    }
    //DEBUG
//    printf("%s\n", commands[cmd]);
//    for(i = 0; i < 6; i++)  printf("opts[% 2d] = %s\n", i, opts[i]);
    switch(cmd) {
        case 0:
            list(opts[0], opts[4], opts[5]);
            break;
        case 1:
            wrcff(opts[0], opts[4], opts[1], opts[3]);
            break;
        case 2:
            wrcfs(opts[0], opts[4], opts[1], opts[3]);
            break;
        case 3:
            wrffc(opts[0], opts[4], opts[2], opts[3]);
            break;
        case 4:
            wrsfc(opts[0], opts[4], opts[3]);
            break;
        case 5:
            help();
            break;
        default:
            printf("No valid command found.\n\n");
            help();
            break;
    }
    return 0;
}

void list(char *peripheral, char *password, char *directory)  {
    //device opening
    se3_disco_it it;
    if(peripheral == NULL)    it = show_and_choose_devices();
    else    it = choose_devices(peripheral);
    if(password == NULL) {
        printf("Enter the device password: ");
        char psswrd[32];
        scanf("%s", psswrd);
        password = psswrd;
        fflush(stdin);
    }
    se3_session s = init_device(password, it);
    //list cipher files in the directory
    list_cipher_files_in_directory(directory);
    //closing device
    close_device(&s);
}

void wrcff(char *peripheral, char *password, char *file_path, char *cipher_file_path)    {
    //device opening
    se3_disco_it it;
    if(peripheral == NULL)    it = show_and_choose_devices();
    else    it = choose_devices(peripheral);
    if(password == NULL) {
        printf("Enter the device password: ");
        char psswrd[32];
        scanf("%s", psswrd);
        password = psswrd;
        fflush(stdin);
    }
    se3_session s = init_device(password, it);
    //cipher file creation & opening, file opening
    FILE *fd = fopen(file_path, "rb");
    SEFILE_FHANDLE sefile_file = open_cipher_file(&s, cipher_file_path, SEFILE_WRITE, SEFILE_NEWFILE);
    //cipher file writing
    //printf("writing cipher file...\n");
    write_cipher_file_from_file(&s, fd, &sefile_file);
    //closing files
    fclose(fd);
    secure_close(&sefile_file);
    //closing device
    close_device(&s);
}

void wrcfs(char *peripheral, char *password, char *string, char *cipher_file_path) {
    //device opening
    se3_disco_it it;
    if(peripheral == NULL)    it = show_and_choose_devices();
    else    it = choose_devices(peripheral);
    if(password == NULL) {
        printf("Enter the device password: ");
        char psswrd[32];
        scanf("%s", psswrd);
        password = psswrd;
        fflush(stdin);
    }
    se3_session s = init_device(password, it);
    //cipher file creation & opening, file opening
    SEFILE_FHANDLE sefile_file = open_cipher_file(&s, cipher_file_path, SEFILE_WRITE, SEFILE_NEWFILE);
    //cipher file writing
    if(string == NULL)  {
        //printf("Please, input a string: ");
        string = (char*)malloc(BUFFER_LENGHT * sizeof(char));
        gets(string);
    }
    //printf("writing cipher file...\n");
    write_cipher_file_from_buffer(&s, string, strlen(string), &sefile_file);
    //closing files
    secure_close(&sefile_file);
    //closing device
    close_device(&s);
}

void wrffc(char *peripheral, char *password, char *file_path, char *cipher_file_path)   {
    //device opening
    se3_disco_it it;
    if(peripheral == NULL)    it = show_and_choose_devices();
    else    it = choose_devices(peripheral);
    if(password == NULL) {
        printf("Enter the device password: ");
        char psswrd[32];
        scanf("%s", psswrd);
        password = psswrd;
        fflush(stdin);
    }
    se3_session s = init_device(password, it);
    //cipher file opening
    SEFILE_FHANDLE sefile_file = open_cipher_file(&s, cipher_file_path, SEFILE_READ, SEFILE_OPEN);
    FILE *fd = fopen(file_path, "wb+");
    //file writing
    //printf("writing binary file...\n");
    write_binary_file_from_cipher_file(&s, fd, &sefile_file);
    //closing files
    fclose(fd);
    secure_close(&sefile_file);
    //closing device
    close_device(&s);
}

void wrsfc(char *peripheral, char *password, char *cipher_file_path)  {
    char string[BUFFER_LENGHT] = {'\0'};
    //device opening
    se3_disco_it it;
    if(peripheral == NULL)    it = show_and_choose_devices();
    else    it = choose_devices(peripheral);
    if(password == NULL) {
        printf("Enter the device password: ");
        char psswrd[32];
        scanf("%s", psswrd);
        password = psswrd;
        fflush(stdin);
    }
    se3_session s = init_device(password, it);
    //cipher file opening
    SEFILE_FHANDLE sefile_file = open_cipher_file(&s, cipher_file_path, SEFILE_READ, SEFILE_OPEN);
    //file writing
    //printf("writing string...\n");
    write_buffer_from_cipher_file(&s, string, BUFFER_LENGHT, &sefile_file);
    printf("%s\n", string);
    //closing files
    secure_close(&sefile_file);
    //closing device
    close_device(&s);
}

void help(void)  {
    printf("Usage: SEfile-cli command [options]\n");
    printf("\n");
    printf("SEfile-cli is a commandline tool to manage encrypted files\n");
    printf("using SEcube microcontroller as crypto-engine.\n");
    printf("\n");
    printf("commands:\n");
    printf("  list   - list decrypted file names of encrypted files\n");
    printf("  wrcff  - writes a cipher file from a file\n");
    printf("  wrcfs  - writes a cipher file from a string\n");
    printf("  wrffc  - writes a file from a cipher one\n");
    printf("  wrsfc  - writes a string from a cipher file\n");
    printf("  --help - shows this message\n");
    printf("\n");
    printf("options:\n");
    printf("  -pe - device drive letter for windows (ex: \"D\") or path for linux\n");
    printf("  -i  - input file or string\n");
    printf("  -o  - output file\n");
    printf("  -c  - input or output cipher file\n");
    printf("  -pa - device password\n");
    printf("  -d  - directory to list\n");
    printf("\n");
    printf("usage examples:\n");
    printf("  SEfile-cli wrcfs -pa test -i \"Hello world!\" -c cipher_file_out.txt\n");
    printf("  on Windows: SEfile-cli wrcff -pe D -pa test -i text_file_in.txt -c cipher_file_out.txt\n");
    printf("  on Linux: SEfile-cli wrffc -pe /mnt/sdb1 -pa test -o text_file_out.txt -c cipher_file_out.txt\n");
    printf("\n");
    printf("For all the commands if -pe is not specified all the available\n");
    printf("devices will be displayed and one of them must be chosen.\n");
    printf("\n");
    printf("For all the commands if -pa is not specified a password will be\n");
    printf("asked.\n");
    printf("\n");
    printf("When no string with -i is passed to wrcfs, it will expect one from\n");
    printf("the standard input.");
    printf("\n");
    printf("\n");
    printf("Written by Daniele Castro on 04/30/2019.\n");
}