

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdbool.h>

      
bool ext_match(const char *name, const char *ext)
{
	size_t nl = strlen(name), el = strlen(ext);
	return nl >= el && !strcmp(name + nl - el, ext);
}

void drept_acces(char *filename){

    struct stat fs;
    int r;
    
    printf("Obtaining permission mode for '%s':\n",filename);
    r = stat(filename,&fs);
    if( r==-1 )
    {
        fprintf(stderr,"File error\n");
        exit(1);
    }

    /* file permissions are kept in the st_mode member */
    printf("Permission bits: %X\n",fs.st_mode);
    printf("Size: %lX\n",fs.st_size);
 printf("User id: %d\n",fs.st_uid);

    }

void parcurge(char *nume_dir, int nivel)
{
    DIR *dir;
    struct dirent *in;
    char *nume;
    struct stat info;
    char cale[PATH_MAX], cale_link[PATH_MAX + 1], spatii[PATH_MAX];
    int n;
    
    memset(spatii, ' ', 2*nivel);
    spatii[2*nivel]='\0';

    if(!(dir = opendir(nume_dir)))
    {
        printf("%s: ", nume_dir); fflush(stdout); 
        perror("opendir");
        exit(1);
    }

    printf("%sDIR %s:\n", spatii, nume_dir);
    
    while((in = readdir(dir))>0)
    {
        nume = in->d_name;
    
        if(strcmp(nume, ".") == 0 || strcmp(nume, "..")==0)
           continue;
    
        snprintf(cale, sizeof(cale), "%s/%s", nume_dir, nume); 
        
        if(ext_match(nume,".bmp"))
        {
        	printf("\t %s \t",nume); //dacă este un fișier obișnuit cu extensia .bmp vor scrie informațiile de mai sus
        	drept_acces(cale);
        }else{
        //dacă este un fișier obișnuit, dar fără extensia .bmp,va scrie aceleași informații ca și în cazul anterior, însă fără lungime și înălțime
} 
    
        if(lstat(cale, &info)<0) 
        {
          printf("%s: ", cale);  fflush(stdout);
          perror("eroare la lstat");
          exit(1);
        }

        if(S_ISDIR(info.st_mode))
          parcurge(cale, nivel + 1);
        else
        if(S_ISLNK(info.st_mode))
        {
          n = readlink(cale, cale_link, sizeof(cale_link));
          cale_link[n]='\0';
          printf("%s  %s -> %s\n", spatii, cale, cale_link);
        }
        else
        {
          printf("%s  %s", spatii, cale);
          if(info.st_mode & S_IXUSR || info.st_mode & S_IXGRP || info.st_mode & S_IXOTH)
            printf("*");
          printf("\n");
        }
        
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
      printf("Mod de utilizare: %s director\n", argv[0]);
      exit(1);
    }
    
    parcurge(argv[1], 0);
    
    return 0;
}
