#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

int counter = 0;/*Global variable to keep track of tree depth*/
/*Dots takes in a char const pointer as parameter and returns true if the file name passed is a '.' or '..'*/
int Dots(char const* name)
{
   return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 );
}
int compare(const void *node1, const void *node2)
{
   struct dirent *ptr1 = *(struct dirent * const *)node1;
   struct dirent *ptr2 = *(struct dirent * const *)node2;
   return strcmp(ptr1->d_name, ptr2->d_name);
}
int List(char const *dirname, struct dirent ***current)
{
   //struct dirent **current;
   int limit, limit_set;
   limit = 50;
   limit_set = 50;

   current[0] = (struct dirent**)malloc(sizeof(struct dirent*)*limit);
   int i;
   int n;
   DIR* dirp;
   struct dirent* list;

   
   if(!(dirp = opendir(dirname)))
   {
      fprintf(stderr, "[error opening dir]\n");
   }
   
   i = 0;
   while((list = readdir(dirp)) != NULL)
   {
        /*
         if(i == limit_set)
         {  
            current[0] = (struct dirent**)realloc(current[0], sizeof(struct dirent*)*limit);
            limit_set = limit_set + limit;
         }
         */
          current[0][i] = list;
         //printf("%s, %s\n", current[0][i]->d_name, list->d_name);
         i++;
   }
   //closedir(dirp);
   current[0][i] = NULL;

   qsort(current[0], i, sizeof(struct dirent*), compare);
   closedir(dirp);
   
   return i;

}
/*AccessFiles takes in the path of the file opens that file using stat() and printing out the permissions for that file*/
/*At this pointer file will have been already opened with opendir if it opendir failed than it won't get to this point so didn't see reason for error*/
void AccessFile(char const* path)
{
   struct stat fileStat;
   if(stat(path, &fileStat) == 0)
   {
       printf("[");
       printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
       printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
       printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
       printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
       printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
       printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
       printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
       printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
       printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
       printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
       printf("]");
   }
   
}
/*Print_Hidden_files does exactly that, takes in the struct dirent pointer, and char const pointer as parameters*/
/*This function simply checks to see if Dots() returns false, and prints out all files and directories including hidden files.  */
void Print_Hidden_files(struct dirent *current, char const *dirname)
{
    int i;

    if(!Dots(current->d_name) && current->d_type == DT_DIR)
      {
        
         if(counter == 0)
         {
            printf("|-- ");
            printf("%s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            printf("%s\n", current->d_name);

         }
      }
     else if(!Dots(current->d_name) && current->d_type != DT_DIR)
      {
         
         if(counter == 0)
         {
            
            printf("|-- ");
            printf("%s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            printf("%s\n", current->d_name);
            

         }
          
      }

}
/*PrintHiddenFiles_with_permissions does exactly that.  Takes in struct dirent pointer, and filepath as char const pointer*/
/*It will then check to see if directory and if it is print out that directory with proper depth and printing out permissions*/
/*If it is not a directory allocate memory to store new filepath and call Accessfile() to get permissions and print out accordingly*/
void PrintHiddenfiles_with_permissions(struct dirent *current, char const *dirname)
{
  char *subdir2;
  int i;


  if(!Dots(current->d_name) && current->d_type == DT_DIR)
      {
         
         if(counter == 0)
         {
            printf("|-- ");
            AccessFile(dirname);
            printf(" %s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            AccessFile(dirname);
            printf(" %s\n", current->d_name);

         }   
      }
    else if(!Dots(current->d_name) && current->d_type != DT_DIR)
      {
         
          subdir2 = malloc(strlen(dirname) + strlen(current->d_name) + 2);

           strcpy(subdir2, dirname);
           strcat(subdir2, "/");
           strcat(subdir2, current->d_name);
         
         if(counter == 0)
         {
            
            printf("|-- ");
            AccessFile(subdir2);
            printf(" %s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            AccessFile(subdir2);
            printf(" %s\n", current->d_name);
            

         }
         free(subdir2);
          
      }
}
/*Print_Without_Hidden_Files Prints all files excluding all hidden files and directories*/
/*It will print out the appropriate depth of the tree with each file*/
void Print_Without_Hidden_Files(struct dirent *current, char const *dirname)
{

      int i;

      if(!Dots(current->d_name) && current->d_type == DT_DIR && current->d_name[0] != '.')
      {
        
         if(counter == 0)
         {
            printf("|-- ");
            printf("%s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            printf("%s\n", current->d_name);

         }
      }
      else if(!Dots(current->d_name) && current->d_type != DT_DIR && current->d_name[0] != '.')
          {
             
             if(counter == 0)
             {
                
                printf("|-- ");
                printf("%s\n", current->d_name);
             }
             else if(counter > 0)
             {
                for(i = 0; i < counter; i++)
                {
                   printf("|   ");
                }
                printf("|-- ");
                printf("%s\n", current->d_name);
                

             }
              
          }
}
/*Print_Permissions_Without_HiddenFiles prints all the files with there permissions that are not hidden files*/
/*If it's not a directory it will malloc memory for the filepath, If file is directory or not AccessFile() will be called to print out the permissions for each file using stat*/
void Print_Permissions_Without_HiddenFiles(struct dirent *current, char const *dirname)
{
    char *subdir2;
    int i;

    if(!Dots(current->d_name) && current->d_type == DT_DIR && current->d_name[0] != '.')
      {
         
         if(counter == 0)
         {
            printf("|-- ");
            AccessFile(dirname);
            printf(" %s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            AccessFile(dirname);
            printf(" %s\n", current->d_name);

         }   
      }
     else if(!Dots(current->d_name) && current->d_type != DT_DIR && current->d_name[0] != '.')
      {
         
          subdir2 = malloc(strlen(dirname) + strlen(current->d_name) + 2);

           strcpy(subdir2, dirname);
           strcat(subdir2, "/");
           strcat(subdir2, current->d_name);
         
         if(counter == 0)
         {
            
            printf("|-- ");
            AccessFile(subdir2);
            printf(" %s\n", current->d_name);
         }
         else if(counter > 0)
         {
            for(i = 0; i < counter; i++)
            {
               printf("|   ");
            }
            printf("|-- ");
            AccessFile(subdir2);
            printf(" %s\n", current->d_name);
            

         }
         free(subdir2);
          
      }
}
/*PrintFile takes in struct dirent pointer for current file node, filpath, and hidden file flag*/
/*If it the hidden file flag is set to true it will call Print_Hidden_files() to print the hidden files*/
/*If hidden flag is false it will printout files without hidden files*/
void PrintFile(struct dirent *current, char const *dirname, int H_FLAG)
{
  
    if(H_FLAG == 0)
    {
      Print_Hidden_files(current, dirname);
    }
    else
    {
      Print_Without_Hidden_Files(current, dirname);
    }
      
}
/*PrintFile_with_permissions takes in struct dirent pointer for file node, filepath, and hidden file flag*/
/*If the hidden file flag is set to true it will call function to print permissions for all files including hidden files and directories*/
/*If the hidden file flag is set to false it will call function to print permissions for all files except hidden files and directories*/
void PrintFile_with_permissions(struct dirent *current, char const *dirname, int H_FLAG)
{
      
    if(H_FLAG == 0)
    {
       PrintHiddenfiles_with_permissions(current, dirname);
    }
    else
    {
       Print_Permissions_Without_HiddenFiles(current, dirname);
    }
  
}
/*PrintRegularFile is for when a regular file gets passed as argment into command line, it will printout file by itself, and with permissions if flag is set*/
void PrintRegularFile(char const *dirname, int L_FLAG)
{
	if(L_FLAG == 0)
	{
		AccessFile(dirname);
		printf(" %s\n", dirname);
	}
	else
	{
		printf("%s\n", dirname);
	}
}
/*PrintTree is the main function in this program that takes in the filepath as parameter along with the long format flag, and hidden file flag*/
/*It calls the built in scandir function which does a lot of cool stuff, it takes in a filename, double pointer of type struct dirent, and alphasor*/
/*This function will take the filepath go through and do all the opendir(), and readdir() to get all the contents of the directory passed in*/
/*It will then create an array of double pointers of type dirent struct and sort all the files in alphbetic order*/
/*It will return an integer value of all the files and directories in that current directory*/
/*This function is recursive so it will keep going through all the directories and there files recursively it will print out based on if long format flat is true or false*/
/*It will then malloc everytime a directory is found it mallocs memory and for the filepath, updates depth counter, and recalls function recursively with updated filepath*/
void PrintTree(char const* dirname, int L_FLAG, int H_FLAG)
{
   char* subdir;
   int i, Num_of_Files;
	 DIR* dirp;
   struct dirent **current;
   if(!(dirp = opendir(dirname)) )
	{
    printf("We could not open directory\n");
		return;
	}
   //Num_of_Files = scandir(dirname, &current, NULL, alphasort);
   Num_of_Files = List(dirname, &current);
   for(i = 0; i < Num_of_Files; i++)
   {
      if(L_FLAG == 0)
      {
        PrintFile_with_permissions(current[i], dirname, H_FLAG);
      }
      else
      {
        PrintFile(current[i], dirname, H_FLAG);
      }
      
      if ( current[i]->d_type == DT_DIR && !(Dots(current[i]->d_name)) && current[i]->d_name[0] != '.')
      {
         
         subdir = malloc(strlen(dirname) + strlen(current[i]->d_name) + 2);

         strcpy(subdir, dirname);
         strcat(subdir, "/");
         strcat(subdir, current[i]->d_name);
		
         counter = counter + 1;
         PrintTree(subdir, L_FLAG, H_FLAG);
         counter = counter - 1;
         
         free(subdir);
         
      }
      else if(current[i]->d_type == DT_DIR && !(Dots(current[i]->d_name)) && H_FLAG == 0 && current[i]->d_name[0] == '.')
      {
          subdir = malloc(strlen(dirname) + strlen(current[i]->d_name) + 2);

         strcpy(subdir, dirname);
         strcat(subdir, "/");
         strcat(subdir, current[i]->d_name);
			
         counter = counter + 1;
         PrintTree(subdir, L_FLAG, H_FLAG);
         counter = counter - 1;
         
         free(subdir);
      }
		//free(current[i]);
		
   }
  free(current);
  closedir(dirp); 
}
/*My main function has a lot of flag checking*/
/*my Main functions will start program at current directory, if user did not put in any files or switchs, if user put in no files and one switch, or if user put in no files and two switches*/
/*Before all that it loops through all the command line arguments to check and set flags accordingly*/
/*Then at end it will go through and calling my PrintTree() function on every file and directory input by user and apply the flags that were set or not set*/
int main(int argc, char* argv[])
{
   char *dirname;
   int L_FLAG, H_FLAG;
   int arg, i;
	struct stat filetest;
   L_FLAG = 1;
   H_FLAG = 1;
  
   if(argc < 2)
   {
      printf(".\n");
      PrintTree(".", L_FLAG, H_FLAG);
   }
   else
   {

      for(arg = 1; arg < argc; arg++)
      {
          dirname = argv[arg];
          if(strcmp(dirname, "-l") == 0)
           {
              L_FLAG = 0;
           }
           else if(strcmp(dirname, "-a") == 0)
           {
              H_FLAG = 0;
           }
           else if(strcmp(dirname, "-la") == 0 || strcmp(dirname, "-al") == 0)
           {
              L_FLAG = 0;
              H_FLAG = 0;
           }
			  else if(dirname[0] == '-')
			  {
					i = 1;
					while(dirname[i] != '\0')
					{
						if(dirname[i] == 'l')
						{
							L_FLAG = 0;
						}
						else if(dirname[i] == 'a')
						{
							H_FLAG = 0;
						}
						i++;
					}									
			  }
      }
      for (arg = 1; arg < argc; arg++) 
      {
          dirname = argv[arg];
           if((argc == 2  && L_FLAG == 0 && H_FLAG != 0) || (argc == 2 && H_FLAG == 0 && L_FLAG != 0) || (argc == 2  && L_FLAG == 0 && H_FLAG == 0))
           {
              printf(".\n");
              PrintTree(".", L_FLAG, H_FLAG);
              printf("\n");
              return 0;
           }
           else if(argc == 3 && H_FLAG == 0 && L_FLAG == 0 && (strcmp(argv[2], "-l") == 0 || strcmp(argv[2], "-a") == 0))
           {
              printf(".\n");
              PrintTree(".", L_FLAG, H_FLAG);
              printf("\n");
              return 0;
           }
           else if (dirname[0] != '-')
           {
                 
						if(stat(dirname, &filetest) == 0)
						{
							if(S_ISREG(filetest.st_mode))
							{
								PrintRegularFile(dirname, L_FLAG);
							}
							else if(S_ISDIR(filetest.st_mode))
							{
								PrintRegularFile(dirname, L_FLAG);
                    	   PrintTree(dirname, L_FLAG, H_FLAG);
							}	
							
						}
						else
						{
							fprintf(stderr, "%s :Does not exist in directory\n", dirname);
						}
            
           }
   
      }       
   }
   printf("\n");
   

   return 0;
}
