#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define BMP_HEADER_SIZE 54


void informatii(int output_file,uint8_t header[],char *file_name)
{
    dprintf(output_file, "nume fisier: %s\n", file_name);
    dprintf(output_file, "inaltime: %d\n", *(int32_t*)&header[18]);
    dprintf(output_file, "lungime: %d\n", *(int32_t*)&header[22]);
    dprintf(output_file, "dimensiune: %d bytes\n", *(int32_t*)&header[2]);
    dprintf(output_file, "identificatorul utilizatorului: %d\n", *(int16_t*)&header[40]);
    dprintf(output_file, "timpul ultimei modificari: %ld\n", *(time_t*)&header[26]); // de modificat
    dprintf(output_file, "contorul de legaturi: %d\n", (int)header[28]);
    dprintf(output_file, "drepturi de acces user: RWX\n");
    dprintf(output_file, "drepturi de acces grup: R--\n");
    dprintf(output_file, "drepturi de acces altii: ---\n");
}


int main(int argc, char *argv[]) {
    
    int input_file, output_file;

    if (argc != 2) {
        printf("Usage: %s <fisier_intrare>\n", argv[0]);
       exit(-1);
    }

    if(( input_file = open(argv[1], O_RDONLY)) < 0){
		printf("Error opening input file");
		exit(2);
	}

    uint8_t header[BMP_HEADER_SIZE];
    ssize_t bytes_read = read(input_file, header, BMP_HEADER_SIZE);
    if (bytes_read != BMP_HEADER_SIZE) {
        perror("Eroare citire header fisier BMP");
        close(input_file);
        return 1;
    }

   
    if(close(input_file) != 0){
        perror("Error close input_file");
    }

    if((output_file=open("statistica.txt", O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) < 0){
      		printf("Error creating destination file\n");
      		exit(3);
    }


    // Scrierea informațiilor în fișierul de statistici
    informatii(output_file,header,argv[1]);

    if(close(output_file) != 0){
        perror("Error close output file");
    }
    return 0;
}
