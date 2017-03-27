/**
 * Kabuk shell interface program
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>


#define MAX_LINE       80 /* 80 chars per line, per command, should be enough. */

int parseCommand(char inputBuffer[], char *args[],int *background);


void insertHistory(char history[10][41][41], char* args[41]);
void shiftHistory(char history[10][41][41]);
void searchHistory(char history[10][41][41],char* results[], char* s);
void printHistory(char history[10][41][41]);
void initHistory(char history[10][41][41]);

int bcscrypto(char * FILENAME, char * NEW_FILENAME, int num, int type);


int main(void)
{
  char inputBuffer[MAX_LINE];           /* buffer to hold the command entered */
  int background;                       /* equals 1 if a command is followed by '&' */
  char *args[MAX_LINE/2 + 1];         /* command line (of 80) has max of 41 arguments */
  pid_t child;                /* process id of the child process */
  int status;               /* result from execv system call*/
  int shouldrun = 1;
  int cdreturn;
  char pass[30];
  char command[200] = "\0";
  char *oPID="";
  
  int i, upper;
  //static int wantPassword;
  static int *insmodCount;
  /* Added variables */
  char commandpath[41];

  char pathlst[500];
  char seperator[1] = ":";
  char *tokenpath;
  char ttok[100];
  char amg[100];
  static int wantPassword = 0;
      	static int loadedBefore = 0;

  /* History terms */
  char history[10][41][41];
  char* historySearchResults[41];
  initHistory(history);
  strcpy(pass, "");
  
  //wantPassword = 0;
  
  /* goodMorning Thing */
  
  

  while (shouldrun){                /* Program terminates normally inside setup */
 
    background = 0;
    
    shouldrun = parseCommand(inputBuffer,args,&background);       /* get next command */
    

   


   if (strncmp(inputBuffer, "exit", 4) == 0){
      
      shouldrun = 0;     /* Exiting from kabuk*/
      if(pass[0]!='\0') // If password is not null it it removes the module
	    {
      sprintf(command, "echo %s | sudo -S rmmod ./process/ata.ko", pass);
      system(command);
	    } 
    }


   //if(strncmp(inputBuffer, "processInfo", 11)==0)wantPassword++;
    
    
    if (shouldrun) {
      /* If the first character is ! the command taken from history */
      if(strncmp("!",args[0],1)==0){
        searchHistory(history,args,args[0]);
      }
      /* command and arguments are inserted into history*/
      else{
        shiftHistory(history);
        insertHistory(history,args);
      }
        
      //Story begins
      child = fork();
      if(child==0){

        
        /* If the command is history, the last 10 commands are printed. */ 
        if(strcmp("history", args[0])==0){
            printHistory(history);
            exit(0);
        }
        /* Handling the good morning */
       else if(strncmp(inputBuffer, "goodMorning", 11) == 0){
          /* Initializing dummies */
          FILE *gMFile;
          int tLength = strlen(args[1]);
          char commanda[500]="\0";
          
          /* Fixing time into correct format */
          strcat(commanda, "\n");
          if(tLength==5){strncat(commanda, args[1]+3,2);} else if(tLength==4){strncat(commanda, args[1]+2,2);}
          else {printf("Wrong time value."); exit(1);}
          
          strcat(commanda, " ");

          if(tLength==5)strncat(commanda, args[1],2); else {strcat(commanda,"0");strncat(commanda, args[1],1);}

          strcat(commanda," * * * export DISPLAY=:0 && rhythmbox ");
          strcat(commanda, args[2]);
          strcat(commanda, " \n");

          /* the command is written into file to be used with crontab */

          gMFile = fopen("b1241561c2434463d48b7df9ceb8f9bf.txt","w");
           
           fprintf(gMFile,"%s",commanda); 
           fclose(gMFile);

           execl("/usr/bin/crontab","crontab","b1241561c2434463d48b7df9ceb8f9bf.txt",NULL);
           perror("execl() failure!\n\n");
          
         
        } 

        /* Here our special command is*/
        /* It is used for encryption and decryption with a selected password. */
        else if(strcmp("bcma",args[0]) == 0){
          bcsmascrypto(args[1],args[2],atoi(args[3]),0);
        }
        else if(strcmp("mabc",args[0]) == 0){
          bcsmascrypto(args[1],args[2],atoi(args[3]),1);
        }
	     
        else if (strcmp("cd", args[0])!=0 && strcmp("processInfo",args[0]) != 0)
        {  
            /* the given command is search in all the paths.
               When it is found it gets executed */
          	strcpy(pathlst, getenv("PATH"));
            tokenpath = strtok(pathlst, seperator);
             
            
             while( tokenpath != NULL ){  
                strcpy(ttok,tokenpath);
                strcat(ttok,"/");
                strcat(ttok,args[0]);	
                if(execv(ttok,args)==0) exit(0);
                tokenpath = strtok(NULL, seperator);
                if(tokenpath==NULL){
                   printf("There is no such command!\n");
                }
             }
             
        }

      }
      
      else if(child>0){
      	
        
        /* arranging wait according to instructions */
        if(background==1)
          wait(NULL);

       
        /*
        for the processInfo handling (next 40 lines), if the user has not inputted password before it wants the password. Else it does not want and tells
        that he is in sudo mode. If the user wants the install the tasks with same pid it tells the user that "this PID is alrady loaded". If the user wants
        to install another tahs with another pid it first removes the module hen installs it with the new PID.
        */
      	 if(strncmp(inputBuffer, "processInfo", 11) == 0) 
      	{
      		
        	 if(wantPassword==0){
              printf("\nEnter password:");
          	  scanf("%s",pass);
        	 }
        	 else {printf("\nIn sudo mode.\n");}


      	 	 if(args[1]!=NULL)
      	   {
        	         	wantPassword = 13;
      		       // printf("args1 null degiiil");
      		        printf("%s\n", oPID);
      	   	   if(strcmp(oPID,args[1])==0)
      	       {
                    printf("\nthis PID is already loaded\n");
      	       }
      	       else
               {      

                 if(loadedBefore > 0 ){
                      //loadedBefore = 0;
                  oPID = args[1];
                      sprintf(command, "echo %s | sudo -S rmmod ./process/ata.ko", pass);
                      system(command);
                   }
                     
                 else{
                      //loadedBefore=13;
                      
      	              sprintf(command, "echo %s | sudo -S insmod ./process/ata.ko myint=%s", pass, args[1]); 
      	            	system(command); 
                      system("dmesg");}
      	        
      		    }
      	    }

          }
        /* cd is handled */
        else if (strcmp("cd", args[0])==0)
        {
          if (chdir(args[1]) == 0)
          {
            printf("Current directory is changed.\n");
          }
          else{
            printf("Error: Current directory is not changed.\n");
          }
        }
       

       


       

        
      }
      /*
  After reading user input, the steps are 
  (1) Fork a child process using fork()
  (2) the child process will invoke execv()
  (3) if command included &, parent will invoke wait()
       */
    }


    if(strcmp("processInfo",args[0])==0 && wantPassword==0)wantPassword=13;
     
     if(strcmp("processInfo",args[0])==0 && (strcmp(oPID, args[1])==0 || loadedBefore>0)){pass[0]='\0';}
      //if(strcmp("processInfo",args[0])==0 && strcmp(oPID, args[1])!=0 && loadedBefore>0){loadedBefore=0;}
      //if(strcmp("processInfo",args[0])==0 && strcmp(oPID, args[1])!=0 && loadedBefore==0){loadedBefore=13;}
     //if(strcmp("processInfo",args[0])==0 && strcmp(oPID, args[1])==0){oPID = args[1];}
     //if(strcmp("processInfo",args[0])==0 && strcmp(oPID, args[1])!=0){oPID = args[1]; loadedBefore=0;}

  }
  if(strcmp("goodMorning",args[0])==0)unlink("/home/bcs/Desktop/feryo/b1241561c2434463d48b7df9ceb8f9bf.txt");
  return 0;
}

/** 
 * The parseCommand function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings. 
 */

int parseCommand(char inputBuffer[], char *args[],int *background)
{
    int length,   /* # of characters in the command line */
      i,    /* loop index for accessing inputBuffer array */
      start,    /* index where beginning of next command parameter is */
      ct,         /* index of where to place the next parameter into args[] */
      command_number; /* index of requested command number */
    
    ct = 0;
  
    /* read what the user enters on the command line */
    do {
    printf("kabuk> ");
    fflush(stdout);
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE); 
    }
    while (inputBuffer[0] == '\n'); /* swallow newline characters */
  
    /**
     *  0 is the system predefined file descriptor for stdin (standard input),
     *  which is the user's screen in this case. inputBuffer by itself is the
     *  same as &inputBuffer[0], i.e. the starting address of where to store
     *  the command that is read, and length holds the number of characters
     *  read in. inputBuffer is not a null terminated C-string. 
     */    
    start = -1;
    if (length == 0)
      exit(0);            /* ^d was entered, end of user command stream */
    
    /** 
     * the <control><d> signal interrupted the read system call 
     * if the process is in the read() system call, read returns -1
     * However, if this occurs, errno is set to EINTR. We can check this  value
     * and disregard the -1 value 
     */

    if ( (length < 0) && (errno != EINTR) ) {
      perror("error reading the command");
      exit(-1);           /* terminate with error code of -1 */
    }
    
    /**
     * Parse the contents of inputBuffer
     */
    
    for (i=0;i<length;i++) { 
      /* examine every character in the inputBuffer */
      
      switch (inputBuffer[i]){
      case ' ':
      case '\t' :               /* argument separators */
  if(start != -1){
    args[ct] = &inputBuffer[start];    /* set up pointer */
    //printf("%s\n", args[ct]);
    ct++;
  }
  inputBuffer[i] = '\0'; /* add a null char; make a C string */
  start = -1;
  break;
  
      case '\n':                 /* should be the final char examined */
  if (start != -1){
    args[ct] = &inputBuffer[start];     

    ct++;
  }
  inputBuffer[i] = '\0';
  args[ct] = NULL; /* no more arguments to this command */
  break;
  
      default :             /* some other character */
  if (start == -1)
    start = i;
  if (inputBuffer[i] == '&') {
    *background  = 1;
    inputBuffer[i-1] = '\0';
  }
      } /* end of switch */
    }    /* end of for */
    
    /**
     * If we get &, don't enter it in the args array
     */
    
    if (*background)
      args[--ct] = NULL;
    
    args[ct] = NULL; /* just in case the input line was > 80 */
    
    return 1;
    
} /* end of parseCommand routine */



/* Here are our history handling processes.
   We divided handling into smaller parts. */

/* Insert history adds new comment into 0th row*/
void insertHistory(char history[10][41][41], char* args[41])
{
    int j,k;
        for(j=0; j<41; j++)
        {
            for(k=0; k<41; k++)
            {
                history[0][j][k] = '\0';
            }
        }
    for(j=0; j<41; j++)
    {
        if(args[j] == NULL)
            break;
            strcpy(history[0][j], args[j]);
    }
}

/* this function shifts ith row to (i+1)th row. */
void shiftHistory(char history[10][41][41])
{
    int i, j,k;
    for(i=8; i>=0; i--)
    {
        for(j=0; j<41; j++)
        {
            for(k=0; k<41; k++)
                history[i+1][j][k] = '\0';
            strcpy(history[i+1][j], history[i][j]);
        }
    }  
}
/* this function initilizes all the elements to NULL in history matrix. */
void initHistory(char history[10][41][41])
{
    int i,j,k;
    for(i=0; i<10; i++)
    {
        for(j=0; j<41; j++)
        {
            for(k=0; k<41; k++)
            {
                history[i][j][k] = '\0';
            }
        }
        
    }
}

/*this function searches the history accordingly and changes the given args array. */
void searchHistory(char history[10][41][41],char* results[], char* s)
{
    int i=0; int k=0;
    int j,p;
    while (history[i][0][0]) {i++;k++;}
    //printf("%d\n", k);
    /* check */
    if(k>=10){
      if(strcmp(s,"!!")==0 || strcmp(s,"!10")==0) p=0;
      else if(strcmp(s,"!9")==0) p=1;
      else if(strcmp(s,"!8")==0) p=2;
      else if(strcmp(s,"!7")==0) p=3;
      else if(strcmp(s,"!6")==0) p=4;
      else if(strcmp(s,"!5")==0) p=5;
      else if(strcmp(s,"!4")==0) p=6;
      else if(strcmp(s,"!3")==0) p=7;
      else if(strcmp(s,"!2")==0) p=8;
      else if(strcmp(s,"!1")==0) p=9;
    }
    else{
      if(strcmp(s,"!!")==0) p=0;
      else if(strcmp(s,"!9")==0) p=k-9;
      else if(strcmp(s,"!8")==0) p=k-8;
      else if(strcmp(s,"!7")==0) p=k-7;
      else if(strcmp(s,"!6")==0) p=k-6;
      else if(strcmp(s,"!5")==0) p=k-5;
      else if(strcmp(s,"!4")==0) p=k-4;
      else if(strcmp(s,"!3")==0) p=k-3;
      else if(strcmp(s,"!2")==0) p=k-2;
      else if(strcmp(s,"!1")==0) p=k-1;
      
    }
      if(p<0 || p>9)printf("Error: There is no entry with this number.\n"); 
      for(j=0; j<41; j++)
      {   
          if(history[p][j][0]=='\0')
              results[j] = NULL;
          else
              results[j]= history[p][j];
      }
    
   //printf("%s\n", results[0]);
   
}

void printHistory(char history[10][41][41])
{
    int i=0;
    int j,k;
    while (history[i][0][0]) {i++;k++;}
    if(k>10) k = 10;
    
    for(i=0; i<10; i++)
    {
       
        if(k>0) printf("\n%d ",k);
        for(j=0; j<41; j++)
        {
            if(history[i][j][0] == '\0')
                break;
            printf("%s ", history[i][j]);
        }
       
        k-=1;
    }
}


/* this function encrypts and decrypts the given file with respect to a given integer.*/
int bcsmascrypto(char * FILENAME, char * NEW_FILENAME, int num, int type)
{
   FILE *input, *output; 
   char Byte;
   char newByte;
   int n;
   int i=0;
   num = abs(num)+13;

   input = fopen(FILENAME,"rb");
   output = fopen(NEW_FILENAME, "w");

   if(input==NULL) printf("Error: Can't Open inFile\n");

   if(output==NULL){printf("Error: Can't open outFile.\n"); exit(-1);}
   else
   {
      printf("File Opened,\nPlease Wait\n");
       while(1)
       {
          printf("*");
          if(Byte!=EOF){
              Byte=fgetc(input);

              if(type==1) newByte=Byte-num; else newByte=Byte+num;
              
              fputc(newByte,output);
          }

          else{printf("\nProcess ended successfully.\n"); exit(0);}
       }
   fclose(input);
   fclose(output);

   }
  }
