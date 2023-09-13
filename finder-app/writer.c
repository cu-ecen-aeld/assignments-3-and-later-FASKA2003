//Author : AYOUB EL MAHFOUDI

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#define NUM_PARAMS 2

/*
* Writes an input string to a file. Assumes directory is created by caller
* Exits and logs on file creation error.
* Params :
*   Writefile - string location of file
*   writestr - string to write to file
* Return : 
*   exits with 1 on file creation error;
*/

void write (char *writefile, char *writestr)
{
  FILE *fp;
  fp = fopen(writefile, "w");
  if (fp != NULL)
  {
    fprintf(fp, "%s", writestr);
    fclose(fp);
  }
  else{
    syslog(LOG_ERR, "Error creating file.");
    exit(1);
  }
  printf("%s written to %s.\n",writestr,writefile);
  syslog(LOG_DEBUG, "%s written to %s.",writestr,writefile);
}

//main function
int main(int argc, char *argv[])
{
  openlog("AESD second assignment", LOG_CONS, LOG_USER);

  if (argc != (NUM_PARAMS + 1))
  {
    syslog(LOG_ERR, "Incorrect number of parameters, should be %d.", NUM_PARAMS);
    return 1;
  }
  
  //write the arg[2] to the argv[1] file
  write(argv[1], argv[2]);
  closelog();
  return 0;
}
