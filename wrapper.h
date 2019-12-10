/*! \file  wrapper.h
 *  \brief This file includes constants, return values and public
 *         functions used for easily interface with SEfile APIs.
 *   \authors Daniele Castro
 *   \date 04/30/2019
 *
 * In this library you will find wrappers to the SEfile functions,
 * in order to manage encrypted files using the SECube Board.
 * In this files are also reported the constant used as parameter
 * and all the possible return values.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "SEfile.h"


#ifndef WRAPPER_H
#define WRAPPER_H

#define MAX_SHOWN_FILE 20 /**< Maximum number of files to be shown */
#define BUFFER_LENGHT 1024*512 /**< Maximum length for an input buffer */

/**
 * @brief This function prints a list of available devices
 *        ex: D, E in Windows or the mount point in linux.
 *        In Linux only already mounted devices will be shown.
 * @return The function returns a se3_disco_it Discovery iterator
 *         data structure.
 */
se3_disco_it show_and_choose_devices(void);
/**
 * @brief This function chooses a device given an ASCII capital
 *        letter ex: D, E in Windows or the mount point in linux.
 *        In Linux only already mounted devices will be shown.
 * @param [in] drive letter in Windows, ex: D, E
 *        or the mount point in linux.
 * @return The function returns a se3_disco_it Discovery iterator
 *         data structure.
 */
se3_disco_it choose_devices(char *drive);
/**
 * @brief This function handshakes the SEcube USB connection
 *        with the PC.
 * @param [in] *password is the pointer to an already allocated
 *        ASCII string containing the device password.
 * @param [in] it is the Discovery iterator data structure
 *        initialized by \ref show_and_choose_devices() or
 *        \ref choose_devices()
 * @return The function returns a SEcube Communication session
 *         structure.
 */
se3_session init_device(char *password, se3_disco_it it);
/**
 * @brief This function opens the encrypted file.
 * @param [in] *s is the SEcube Communication session
 *         structure initialized by \ref init_device()
 * @param [in] *file_path is the pointer to an already allocated
 *        ASCII string containing the cipher file path.
 * @param [in] mode The mode in which the file should be created.
 *        See \ref Mode_Defines.
 * @param [in] creation Define if the file should be created or it should already exist.
 *        See \ref Access_Defines.
 * @return The SEFILE_FHANDLE data structure used to access encrypted
 *         files @hideinitializer in which the file handle to the new
 *         opened file is placed after a success, NULL in case of failure.
 */
SEFILE_FHANDLE open_cipher_file(se3_session *s, char* file_path, int32_t mode, int32_t creation);
/**
 * @brief This function reads a binary file and writes an encrypted
 *        version.
 * @param [in] *s is the SEcube Communication session
 *         structure initialized by \ref init_device()
 * @param [in] *fd is the binary file pointer.
 * @param [in] *sefile_file is the SEFILE_FHANDLE data structure
 *        used to access encrypted files @hideinitializer in which
 *        the file handle to the new opened file is placed after a
 *        success, NULL in case of failure.
 */
void write_cipher_file_from_file(se3_session *s, FILE *fd, SEFILE_FHANDLE *sefile_file);
/**
 * @brief This function reads a binary uint8_t buffer and writes an
 *        encrypted file.
 * @param [in] *s is the SEcube Communication session
 *         structure initialized by \ref init_device()
 * @param [in] *buffer is the binary buffer pointer.
 * @param [in] buffer_len is the binary buffer lenght.
 * @param [in] *sefile_file is the SEFILE_FHANDLE data structure
 *        used to access encrypted files @hideinitializer in which
 *        the file handle to the new opened file is placed after a
 *        success, NULL in case of failure.
 */
void write_cipher_file_from_buffer(se3_session *s, uint8_t *buffer, int buffer_len, SEFILE_FHANDLE *sefile_file);
/**
 * @brief This function lists all the encrypted files in the
 *        directory with decrypted file names.
 * @param [in] *path is the ASCII path where to list the encrypted
 *        files.
 */
void list_cipher_files_in_directory(char* path);
/**
 * @brief This function reads an encrypted file and writes a decrypted
 *        version.
 * @param [in] *s is the SEcube Communication session
 *         structure initialized by \ref init_device()
 * @param [in] *fd is the binary file pointer.
 * @param [in] *sefile_file is the SEFILE_FHANDLE data structure
 *        used to access encrypted files @hideinitializer in which
 *        the file handle to the new opened file is placed after a
 *        success, NULL in case of failure.
 */
void write_binary_file_from_cipher_file(se3_session *s, FILE *fd, SEFILE_FHANDLE *sefile_file);
/**
 * @brief This function reads an encrypted file and writes a decrypted
 *        buffer.
 * @param [in] *s is the SEcube Communication session
 *         structure initialized by \ref init_device()
 * @param [out] *buffer is the binary buffer pointer.
 * @param [in] buffer_len is the binary buffer lenght.
 * @param [in] *sefile_file is the SEFILE_FHANDLE data structure
 *        used to access encrypted files @hideinitializer in which
 *        the file handle to the new opened file is placed after a
 *        success, NULL in case of failure.
 * @return The the number of bytes ridden.
 */
int write_buffer_from_cipher_file(se3_session *s, uint8_t *buffer, int buffer_len, SEFILE_FHANDLE *sefile_file);
/**
 * @brief This function closes the SEcube USB connection.
 *        directory with decrypted file names.
 * @param [in] *s is the SEcube Communication session
 *         structure initialized by \ref init_device()
 */
void close_device(se3_session *s);

#endif //WRAPPER_H