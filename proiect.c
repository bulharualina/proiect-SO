#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define BMP_HEADER_SIZE 54

void nume_fisier(int output_file,char *file_name){
dprintf(output_file, "nume fisier: %s\n", file_name);
}

void detalii_timp(int output_file,  struct stat file_stat) {

  time_t timpModificare = file_stat.st_mtime;
   struct tm *timeinfo = localtime(&timpModificare);
    char time_str[20];
    strftime(time_str, 20, "%d.%m.%Y", timeinfo);
    dprintf(output_file,"timpul ultimei modificari :  %s \n", time_str);
}

void drept_acces(int output_file,struct stat file_stat){
    dprintf(output_file, "drepturi de acces user: %c%c%c\n",
                        file_stat.st_mode & S_IRUSR ? 'R' : '-',
                        file_stat.st_mode & S_IWUSR ? 'W' : '-',
                        file_stat.st_mode & S_IXUSR ? 'X' : '-');
    dprintf(output_file, "drepturi de acces grup: %c%c%c\n",
                        file_stat.st_mode & S_IRGRP ? 'R' : '-',
                        file_stat.st_mode & S_IWGRP ? 'W' : '-',
                        file_stat.st_mode & S_IXGRP ? 'X' : '-');
    dprintf(output_file, "drepturi de acces altii: %c%c%c\n",
                        file_stat.st_mode & S_IROTH ? 'R' : '-',
                        file_stat.st_mode & S_IWOTH ? 'W' : '-',
                        file_stat.st_mode & S_IXOTH ? 'X' : '-');
}

void identif_utiliz(int output_file, struct stat file_stat) {
    dprintf(output_file, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
}



void dimensiune(int output_file,struct stat file_stat){
    dprintf(output_file, "dimensiune: %ld bytes\n", file_stat.st_size);
}

void contor_legaturi(int output_file,  struct stat file_stat) {
    dprintf(output_file, "contorul de legaturi: %ld\n", file_stat.st_nlink);
}

void informatii_bmp(int output_file,uint8_t header[])
{
   
    dprintf(output_file, "inaltime: %d\n", *(int32_t*)&header[18]);
    dprintf(output_file, "lungime: %d\n", *(int32_t*)&header[22]);
}

int main(int argc, char *argv[]) {
    
    int input_file, output_file;
     struct stat file_stat;
    int r;
    if (argc != 2) {
        printf("Usage: %s <fisier_intrare>\n", argv[0]);
       exit(-1);
    }

    if(( input_file = open(argv[1], O_RDONLY)) < 0){
		printf("Error opening input file");
		exit(2);
	}


    r = stat(argv[1],&file_stat);
    if( r==-1 )
    {
        fprintf(stderr,"File error\n");
        exit(1);
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


    nume_fisier(output_file,argv[1]);
    informatii_bmp(output_file,header);
    dimensiune(output_file,file_stat);
    identif_utiliz(output_file, file_stat);
    detalii_timp(output_file,file_stat);
    contor_legaturi(output_file,file_stat);
    drept_acces(output_file,file_stat);
   
    
    if(close(output_file) != 0){
        perror("Error close output file");
    }
    return 0;
}
