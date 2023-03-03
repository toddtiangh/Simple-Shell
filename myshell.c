#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#include "myshell_parser.h"

void signitHandler(int signum) 
{
	signal(SIGINT, signitHandler);
	fflush(stdout);
}

int main(int argc, char * argv[])
{
	struct pipeline *new_pipeline;
	struct pipeline_command *pipeline_ptr; // this increment through pipes without changing new_pipeline->commands....
	char buffer[MAX_LINE_LENGTH]; //cont. I was assigning pipeline_commands->commands= pipeline->commands->next but that was causing memory leaks 
	char * n = "-n";
	bool n_flag = false;
	bool exit_flag = false; // alternative exit method
	char * terminate = "exit\n\0";
	if(argc > 1 && strcmp(n, argv[1]) == 0) // for -n
	{
		n_flag = true;
	}
	signal(SIGINT, signitHandler);

	while(1)
	{	
		if(n_flag)
		{

		}
		else 
		{
			printf("%s", "my_shell$");
		}
		const char * command;
		if((command = fgets(buffer, MAX_LINE_LENGTH, stdin)) == NULL) // for ctrl d
		{
			break;
		}
		if(strcmp(command, terminate) == 0)
		{
			exit_flag = true;
			break;

		}
		if(strlen(command) <= 2) // for blank input <= 2 because of \n
		{
			continue;
		}
		else 
		{
			new_pipeline = pipeline_build(command);
			pipeline_ptr = new_pipeline->commands;
			bool inflag= false, outflag = false; // r-in and r-out state flags
			int startpipe = 1, endpipe = 0, middlepipe = 0; // identify where we are in the pipeline
			pid_t pid;
			int status;
			int fd[2], out_fd, in_fd;	
			while(pipeline_ptr->next != NULL) // runs until final command block
			{
				pipe(fd);

				pid = fork();
				if(pid < 0)
				{
					perror("ERROR");
				}
				if (pid == 0) // child
				{
					if(pipeline_ptr->redirect_in_path) inflag = true;
					
					if(inflag && startpipe == 1) // only run on the first command block
					{
						in_fd = open(pipeline_ptr->redirect_in_path, O_RDONLY, 0); // read
						if(out_fd < 0)
						{
							perror("ERROR");
							exit(0);
						}
						dup2(in_fd, STDIN_FILENO); // copy contents to stdin and close in_fd
						close(in_fd);
						inflag = false;
					}
	
					if(startpipe == 1 && endpipe == 0) // if we are in the first pipe, take our out end and copy it to stdout
					{
						dup2(fd[1], STDOUT_FILENO);
					}
					else if(startpipe == 0 && middlepipe != 0)
					{
						dup2(middlepipe, STDIN_FILENO); // if we are in middle pipes we want to take the read of the prev pipe and copy 
						dup2(fd[1], STDOUT_FILENO); // cont.. to stdin, the copy our out end to stdout again
					}
			           	if(execvp(pipeline_ptr->command_args[0], pipeline_ptr->command_args) < 0) //error check for execvp
					{
						perror("ERROR");
						exit(0);
					}
				}
				else // parent
				{	if(!new_pipeline->is_background) // we only wait if we are not in background
					{
						waitpid(pid, &status, 0);
					}	
				}
				startpipe = 0; // after our first pipe, startpipe will always be 0
				pipeline_ptr = pipeline_ptr->next; // we advance our command ptr our next command block
				close(fd[1]); // we close the out end of our pipe
				middlepipe = fd[0]; // reassign middlepipe to the read of our current pipe for input in the next pipe
			}

			endpipe = 1; // endpipe flag as we break out of while loop
			pipe(fd);

			pid = fork();
			if(pid == 0)
			{
				if(pipeline_ptr->redirect_out_path) outflag = true; 
				if(pipeline_ptr->redirect_in_path) inflag = true;

				if(inflag)
				{
					in_fd = open(pipeline_ptr->redirect_in_path, O_RDONLY, 0);
					if(in_fd < 0)
					{
						perror("ERROR");
						exit(0);
					}
					dup2(in_fd, STDOUT_FILENO);
					close(in_fd);
					inflag = false;
				}
				if(outflag) // here we can handle both single commands (cat < x > y) or if there is an write in the last command
				{
					out_fd = creat(pipeline_ptr->redirect_out_path, 0644);	
					if(out_fd < 0) // for some reason open with flags (O_TRUNC, O_WRONGLY, O_CREAT) not working
					{
						perror("ERROR");
						exit(0);
					}
					dup2(out_fd, STDOUT_FILENO); // copy contents to stdout and close out_fd
					close(out_fd);
					outflag = false;
				}

				dup2(middlepipe, STDIN_FILENO); // here we only need to take the read of previous pipe and copy it to stdin

				if(execvp(pipeline_ptr->command_args[0], pipeline_ptr->command_args) < 0)
				{
					perror("ERROR");
					exit(0);
				}
			}	
			else
			{	
				if(!new_pipeline->is_background)
				{
					waitpid(pid, &status, 0);
				}
			}
			if(middlepipe != 0) // here we close middlepipe
			{
				close(middlepipe);
			}
			if(endpipe == 1) // we close out read end of pipe
			{
				close(fd[0]);
			}
			close(fd[1]); // finally close out end of pipe
		}
		pipeline_free(new_pipeline); // free our pipeline
	}
	if(exit_flag)
	{
		exit(0);	
	}
	return 0;
}
