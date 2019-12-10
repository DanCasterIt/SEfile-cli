/*! \file  SEfile-cli.h
 *  \brief This file includes constants, return values and public
 *         functions used for implementing a command line - CLI interface.
 *   \authors Daniele Castro
 *   \date 04/30/2019
 *
 * In this library you will find wrappers to the SEfile functions,
 * in order to manage encrypted files using the SECube Board.
 * In this files are also reported the constant used as parameter
 * and all the possible return values.
 */

#include <stdio.h>
#include <string.h>
#include "wrapper.h"

#ifndef SEFILE_SEFILE_CLI_H
#define SEFILE_SEFILE_CLI_H

static char *options[] = {
        "-pe", //peripheral
        "-i",  //input file/string
        "-o",  //output file
        "-c",  //cipher file
        "-pa", //password
        "-d"   //directory
};

static char *commands[] = {
        "list",  //list cipher files
        "wrcff", //write cipher file from file
        "wrcfs", //write cipher file from string
        "wrffc", //write file from cipher
        "wrsfc", //write string from cipher
        "--help" //prints usage informations
};

/**
 * @brief This the main function.
 * @param [in] argc is the number of arguments passed to the program
 * @param [in] *argv[] is a string array containing the arguments string
 */
int main(int argc, char *argv[]);
/**
 * @brief This function Shows a list of decrypted file names of
 *        encrypted files in a given directory.
 * @param [in] *peripheral is the windows drive letter (ex: D) or
 *        partition path for Linux.
 * @param [in] *password is the SEfile firmware password
 * @param [in] *directory is the directory path
 */
void list(char *peripheral, char *password, char *directory);
/**
 * @brief This function writes a cipher file starting from a
 *        binary file: it crypts the content of the binary file into a cipher one.
 * @param [in] *peripheral is the windows drive letter (ex: D) or
 *        partition path for Linux.
 * @param [in] *password is the SEfile firmware password
 * @param [in] *file_path is the string containing the
 *        binary file path
 * @param [in] *cipher_file_path is the string containing the
 *        cipher file path
 */
void wrcff(char *peripheral, char *password, char *file_path, char *cipher_file_path);
/**
 * @brief This function writes a cipher file from a string:
 *        it crypts the content of the string into a cipher file.
 * @param [in] *peripheral is the windows drive letter (ex: D) or
 *        partition path for Linux.
 * @param [in] *password is the SEfile firmware password
 * @param [in] *string is the input string
 * @param [in] *cipher_file_path is the string containing the
 *        cipher file path
 */
void wrcfs(char *peripheral, char *password, char *string, char *cipher_file_path);
/**
 * @brief This function writes a file from a cipher one:
 *        decrypts the content of the cypher file into a binary file.
 * @param [in] *peripheral is the windows drive letter (ex: D) or
 *        partition path for Linux.
 * @param [in] *password is the SEfile firmware password
 * @param [in] *file_path is the string containing the
 *        binary file path
 * @param [in] *cipher_file_path is the string containing the
 *        cipher file path
 */
void wrffc(char *peripheral, char *password, char *file_path, char *cipher_file_path);
/**
 * @brief This function writes a string from a cipher file:
 *        decrypts the content of the cypher file into a string.
 * @param [in] *peripheral is the windows drive letter (ex: D) or
 *        partition path for Linux.
 * @param [in] *password is the SEfile firmware password
 * @param [in] *cipher_file_path is the string containing the
 *        cipher file path
 */
void wrsfc(char *peripheral, char *password, char *cipher_file_path);
/**
 * @brief This function Shows an help message with some instruction
 *        on how to use it and few usage examples.
 */
void help(void);

#endif //SEFILE_SEFILE_CLI_H
