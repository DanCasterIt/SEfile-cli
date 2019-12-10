/** \file wrapper.c
 *  \brief In this file you will find the implementation
 *         of the public function already described in
 *         \ref wrapper.h
 *   \authors Daniele Castro
 *   \date 04/30/2019
 */

#include "wrapper.h"

//prints a list of available devices ex: D:, E:
se3_disco_it show_and_choose_devices(void) {
    se3_disco_it it;
	int i, e = -1;
    L0_discover_init(&it);
    printf("Avaliable devices list:\n");
    for(i = 0; L0_discover_next(&it) == 1; i++) {
        printf("Device [% 2d]: %s\n", i, (char *) it.device_info.path);
    }
    if(i == 0)    {
        fprintf(stderr, "ERROR: no available devices found.\n");
        exit(-1);
    }
    while(e < 0 || e > i)	{
    	printf("Choose device number: ");
    	scanf("%d", &e);
    	if(e < 0 || e > i)	printf("ERROR: enter a device number in the list.\n");
	}
    L0_discover_init(&it);
    for(i = 0; i < e; i++)	L0_discover_next(&it);
    return it;
}

//chooses a device given an ASCII letter
se3_disco_it choose_devices(char *drive)    {
    se3_disco_it it;
    L0_discover_init(&it);
    while(L0_discover_next(&it) == 1)   {
        if(strcmp(drive, (char *) it.device_info.path) == 0) break;
    }
    return it;
}

//handshakes the SEcube USB connection
se3_session init_device(char *password, se3_disco_it it)   {
    se3_device dev;
    se3_session s;
    uint16_t ret;
    if(!dev.opened) {
        if((ret = L0_open(&dev, &(it.device_info), SE3_TIMEOUT)) != SE3_OK) {
            fprintf(stderr, "ERROR: L0_open() - error code: 0x%X\n", ret);
            exit(-1);
        }
    }
    uint8_t *pin = (uint8_t *)calloc(32, 1);
    memcpy(pin, password, strlen(password));
    if((ret = L1_login(&s, &dev, pin, SE3_ACCESS_USER)) != SE3_OK) {
        free(pin);
        if(ret == SE3_ERR_PIN) fprintf(stderr, "Wrong password.\n");
        else {
            fprintf(stderr, "ERROR: L1_login()\n");
            exit(-1);
        }
    } else {
        free(pin);
        //printf("Correct password.\n");
    }
    if(L1_crypto_set_time(&s, (uint32_t) time(0))) {
        fprintf(stderr, "ERROR: L1_crypto_set_time()\n");
        exit(-1);
    }
    if(secure_init(&s, -1, SE3_ALGO_MAX + 1)) {
        fprintf(stderr, "ERROR: secure_init()\n");
        exit(-1);
    }
    return s;
}

//opens the encrypted file
SEFILE_FHANDLE open_cipher_file(se3_session *s, char* file_path, int32_t mode, int32_t creation)   {
    SEFILE_FHANDLE sefile_file;
    int ret;
    //open cipher file
    if ((ret = secure_open(file_path, &sefile_file, mode, creation))) {
        if (ret == SEFILE_SIGNATURE_MISMATCH) {
            fprintf(stderr, "ERROR: secure_open() - Signature Mismatch\n");
            close_device(s);
            exit(-1);
        }
        else {
            fprintf(stderr, "ERROR: secure_open() - code: 0x%X\n", ret);
            close_device(s);
            exit(-1);
        }
    }
    return sefile_file;
}

//reads a binary file and writes an encrypted version
void write_cipher_file_from_file(se3_session *s, FILE *fd, SEFILE_FHANDLE *sefile_file)    {
    int ret;
    uint8_t buffer[BUFFER_LENGHT];
    //get the file size
    fseek(fd, 0, SEEK_END);
    size_t length = ftell(fd);
    rewind(fd);
    while(1)   {
        if(length > BUFFER_LENGHT)    {
            //read binary file
            length = length - fread(buffer, 1, BUFFER_LENGHT, fd);
            //write cipher file
            if ((ret = secure_write(sefile_file, buffer, BUFFER_LENGHT))) {
                if (ret == SEFILE_SIGNATURE_MISMATCH) fprintf(stderr, "ERROR: secure_write() - Signature Mismatch\n");
                else fprintf(stderr, "ERROR: secure_write() - code: 0x%X\n", ret);
                fclose(fd);
                secure_close(sefile_file);
                close_device(s);
                exit(-1);
            }
        } else  {
            //read binary file
            fread(buffer, 1, length, fd);
            //write cipher file
            if ((ret = secure_write(sefile_file, buffer, length))) {
                if (ret == SEFILE_SIGNATURE_MISMATCH) fprintf(stderr, "ERROR: secure_write() - Signature Mismatch\n");
                else fprintf(stderr, "ERROR: secure_write() - code: 0x%X\n", ret);
                fclose(fd);
                secure_close(sefile_file);
                close_device(s);
                exit(-1);
            }
            break;
        }
    }
}

//reads a binary uint8_t buffer and writes an encrypted file
void write_cipher_file_from_buffer(se3_session *s, uint8_t *buffer, int buffer_len, SEFILE_FHANDLE *sefile_file) {
    int ret;
    if ((ret = secure_write(sefile_file, buffer, buffer_len))) {
        if (ret == SEFILE_SIGNATURE_MISMATCH) fprintf(stderr, "ERROR: secure_write() - Signature Mismatch\n");
        else fprintf(stderr, "ERROR: secure_write() - code: 0x%X\n", ret);
        secure_close(sefile_file);
        close_device(s);
        exit(-1);
    }
}

//lists all the encrypted files in the directory with decrypted file names
void list_cipher_files_in_directory(char* path)  {
    char fileList[MAX_SHOWN_FILE*MAX_PATHNAME] = {0}, *pFile = NULL;
    uint32_t filelistLength = 0;
    size_t cnt = 0, i = 0;
    if(secure_ls(path, fileList, &filelistLength))  fprintf(stderr, "ERROR: secure_ls()\n");
    pFile = fileList;
    cnt = (size_t)filelistLength;
    while(cnt > 0)  {
        printf("File[% 3u]: %s\n", i, pFile);
        i++;
        cnt-=(strlen(pFile)+1);
        pFile+=(strlen(pFile)+1);
    }
}

//reads an encrypted file and writes a decrypted version
void write_binary_file_from_cipher_file(se3_session *s, FILE *fd, SEFILE_FHANDLE *sefile_file) {
    int ret;
    uint32_t nBytesRead = 0;
    uint8_t buffer[BUFFER_LENGHT];
    while((ret = secure_read(sefile_file,  buffer, BUFFER_LENGHT, &nBytesRead)) == 0 && nBytesRead > 0) {
        fwrite(buffer, nBytesRead, 1, fd);
    }
    if( ret == SEFILE_SIGNATURE_MISMATCH ) {
        fprintf(stderr, "ERROR: secure_read() - Signature Mismatch\n");
        fclose(fd);
        secure_close(sefile_file);
        close_device(s);
        exit(-1);
    } else if(ret > 0)  {
        fprintf(stderr, "ERROR: secure_read() - code: 0x%Xu\n", ret);
        fclose(fd);
        secure_close(sefile_file);
        close_device(s);
        exit(-1);
    }
}

//reads an encrypted file and writes a decrypted buffer
int write_buffer_from_cipher_file(se3_session *s, uint8_t *buffer, int buffer_len, SEFILE_FHANDLE *sefile_file) {
    int ret;
    uint32_t nBytesRead = 0;
    ret = secure_read(sefile_file,  buffer, buffer_len, &nBytesRead);
    if( ret == SEFILE_SIGNATURE_MISMATCH ) {
        fprintf(stderr, "ERROR: secure_read() - Signature Mismatch\n");
        secure_close(sefile_file);
        close_device(s);
        exit(-1);
    } else if(ret > 0)  {
        fprintf(stderr, "ERROR: secure_read() - code: 0x%Xu\n", ret);
        secure_close(sefile_file);
        close_device(s);
        exit(-1);
    }
    return nBytesRead;
}

//closes the SEcube USB connection
void close_device(se3_session *s)  {
    secure_finit();
    if (s->logged_in) L1_logout(s);
}
