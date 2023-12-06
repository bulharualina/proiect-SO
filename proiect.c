#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <sys/wait.h>

#define BMP_HEADER_SIZE 54

void nume_fisier(int output_file,char *file_path){
    char *file_name = strrchr(file_path, '/');
dprintf(output_file, "nume fisier: %s\n", file_name+1);
}

void nume_legatura(int output_file,char *link_path){
    char *file_name = strrchr(link_path, '/');
dprintf(output_file, "nume legatura: %s\n", file_name+1);
}

void nume_director(int output_file,char *dir_path){
    char *file_name = strrchr(dir_path, '/');
dprintf(output_file, "nume director: %s\n", file_name+1);
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

void informatii_bmp(int input_file,int output_file)
{
    uint8_t header[BMP_HEADER_SIZE];
    ssize_t bytes_read = read(input_file, header, BMP_HEADER_SIZE);
    if (bytes_read != BMP_HEADER_SIZE) {
        perror("Eroare citire header fisier BMP");
        close(input_file);
        exit(2);
    }

    dprintf(output_file, "inaltime: %d\n", *(int32_t*)&header[18]);
    dprintf(output_file, "lungime: %d\n", *(int32_t*)&header[22]);
}

int numara_linii_fisier(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Eroare deschidere fisier");
        return -1;
    }

    int numar_linii = 0;
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[0] != '\n') {
            numar_linii++;
        }
    }

    fclose(file);
    return numar_linii;
}

void convert_gri(const char *file_path) {
    FILE *input_file = fopen(file_path, "r+");
    if (input_file == NULL) {
        perror("Error opening input file");
        return;
    }

    uint8_t header[BMP_HEADER_SIZE];
    fread(header, sizeof(uint8_t), BMP_HEADER_SIZE, input_file); 

    int32_t width = *(int32_t *)&header[18];
    int32_t height = *(int32_t *)&header[22];

    fseek(input_file, BMP_HEADER_SIZE, SEEK_SET);//sarim peste header sa ajungem la datele pixelilor

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // Citirea valorilor RGB ale unui pixel
            uint8_t pixel[3];
            fread(pixel, sizeof(uint8_t), 3, input_file);

            // Calculul valorii tonului de gri
            uint8_t grey_value = (uint8_t)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);

            // Aplicarea conversiei pentru pixel
            uint8_t grey_pixel[3] = {grey_value, grey_value, grey_value};
            fseek(input_file, -3, SEEK_CUR); // Repozitionare la începutul pixelului
            fwrite(grey_pixel, sizeof(uint8_t), 3, input_file);
        }
    }

    fclose(input_file);
}

void process_regular_file(char *file_path, char *output_dir,int file_out) {
    struct stat file_stat;
    int r;
    
    if ((r = stat(file_path, &file_stat)) == -1) {
        fprintf(stderr, "File error\n");
        return;
    }

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s/%s_statistica.txt", output_dir, strrchr(file_path, '/') + 1);
    
    int output_file = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (output_file < 0) {
            perror("Error opening output file");
            return;
    }

    nume_fisier(output_file,file_path);
    dimensiune(output_file,file_stat);
    identif_utiliz(output_file, file_stat);
    detalii_timp(output_file,file_stat);
    contor_legaturi(output_file,file_stat);
    drept_acces(output_file,file_stat);
 
    int numar_linii = numara_linii_fisier(output_filename);
    dprintf(file_out, "Numar de linii %s: %d\n",output_filename, numar_linii);
    
    if (close(output_file) != 0) {
        perror("Error close input file");
    }
}

void process_file_bmp(char *file_path, char *output_dir,int file_out) {
    struct stat file_stat;
    int r;
    

    if ((r = stat(file_path, &file_stat)) == -1) {
        fprintf(stderr, "File error\n");
        return;
    }

    int input_file;
    if ((input_file = open(file_path, O_RDONLY)) < 0) {
        perror("Error opening input file");
        return;
    }

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s/%s_statistica.txt", output_dir, strrchr(file_path, '/') + 1);
    
    int output_file = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (output_file < 0) {
            perror("Error opening output file");
            return;
    }
    
    nume_fisier(output_file,file_path);
    informatii_bmp(input_file,output_file);
    dimensiune(output_file,file_stat);
    identif_utiliz(output_file, file_stat);
    detalii_timp(output_file,file_stat);
    contor_legaturi(output_file,file_stat);
    drept_acces(output_file,file_stat);
    
    
    int numar_linii = numara_linii_fisier(output_filename);
    dprintf(file_out, "Numar de linii %s: %d\n",output_filename, numar_linii);

    

    if (close(input_file) != 0) {
        perror("Error close input file");
    }
    if (close(output_file) != 0) {
        perror("Error close input file");
    }
}

void procesare_legatura_simbolica(char *link_path, int file_out,char *output_dir) {
    struct stat link_stat;
    ssize_t link_size;
    char target_path[256];

    if (lstat(link_path, &link_stat) == -1) {
        fprintf(stderr, "Error lstat: %s\n", link_path);
        return;
    }

    link_size = readlink(link_path, target_path, sizeof(target_path) - 1);
    if (link_size == -1) {
        fprintf(stderr, "Error readlink: %s\n", link_path);
        return;
    }
    target_path[link_size] = '\0';

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s/%s_statistica.txt", output_dir, strrchr(link_path, '/') + 1);
    
    int output_file = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (output_file < 0) {
            perror("Error opening output file");
            return;
    }

    nume_legatura(output_file,link_path);
    dimensiune(output_file,link_stat);
    dprintf(output_file, "dimensiune fisier target: %ld\n", link_size);
    drept_acces(output_file,link_stat);
    
   
    int numar_linii = numara_linii_fisier(output_filename);
    dprintf(file_out, "Numar de linii %s: %d\n",output_filename, numar_linii);

    if (close(output_file) != 0) {
        perror("Error close input file");
    }
}

void info_director(int file_out, struct stat dir_stat,char entry_path[], char *output_dir) {
    
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s/%s_statistica.txt", output_dir, strrchr(entry_path, '/') + 1);
    int output_file = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (output_file < 0) {
            perror("Error opening output file");
            return;
    }

    nume_director(output_file,entry_path);
    identif_utiliz(output_file, dir_stat); 
    drept_acces(output_file,dir_stat);
    

  
    int numar_linii = numara_linii_fisier(output_filename);
    dprintf(file_out, "Numar de linii %s: %d\n",output_filename, numar_linii);

    if (close(output_file) != 0) {
        perror("Error close input file");
    }
}


void procesare_director(char *dir_path, int output_file,char *output_dir) {
    DIR *dir_in, *dir_out;
    struct dirent *entry;
    struct stat dir_stat;
    


    if ((dir_in = opendir(dir_path)) == NULL) {
        perror("Error opening input directory");
        return;
    }

    if ((dir_out = opendir(output_dir)) == NULL) {
        perror("Error opening output directory");
        return;
    }


    while ((entry = readdir(dir_in)) != NULL) {
        char entry_path[256];
        snprintf(entry_path, sizeof(entry_path) + 1, "%s/%s", dir_path, entry->d_name);
            pid_t pid = fork();
            if(pid<0){
                printf("Eroare la fork");
                exit(2);
            }
            if(pid == 0){
            if (entry->d_type == DT_REG) {
                char file_path[256];
                snprintf(file_path, sizeof(file_path)+1, "%s/%s", dir_path, entry->d_name);
                
                if (strstr(file_path, ".bmp") != NULL )
                {
                    process_file_bmp(file_path,output_dir,output_file);
                    
                    convert_gri(file_path);   
                   
                }else{                
                process_regular_file(file_path,output_dir,output_file);
                
                }    
            }
            else if(entry->d_type == DT_LNK){
           
                char link_path[256];
                snprintf(link_path, sizeof(link_path) + 1, "%s/%s", dir_path, entry->d_name);
                procesare_legatura_simbolica(link_path, output_file,output_dir);
            }
            else if(entry->d_type == DT_DIR){
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    exit(0);
                }

                if (lstat(entry_path, &dir_stat) == -1) {
                    fprintf(stderr, "Error lstat: %s\n", entry_path);
                    exit(1);
                }
                
                info_director(output_file, dir_stat,entry_path,output_dir);
                
            }
           
            exit(0);
            }
        
    }
    
    int status;
    pid_t childPid;
    while ((childPid = wait(&status)) > 0) 
    {
        if (WIFEXITED(status)) 
        {
            
            printf("Child process with PID %d exited with status %d\n", childPid, WEXITSTATUS(status));
        }
    }
    
    closedir(dir_in);
    closedir(dir_out);
}



int main(int argc, char *argv[]) {
    
     int output_file;
    if (argc != 3) {
        printf("Usage: %s <director_intrare> <director_iesire>\n", argv[0]);
        exit(-1);
    }

    if ((output_file = open("statistica.txt", O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) < 0) {
        printf("Error creating destination file\n");
        exit(3);
    }
   
    procesare_director(argv[1], output_file,argv[2]);
    
    if (close(output_file) != 0) {
        perror("Error close output file");
    }

    return 0;
}

