#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define allchar "|&<>"
#define endline "\n\t\r "
#define pipe "|"
#define ampersand "&"
#define rin "<"
#define rout ">"

bool isCorrect(int ascii) // i know this is stupid :( accepts letters and dashes (might add more later on depending on bucket of chars)
{
        if (ascii > 64  && ascii < 128)
        {
                return true;
        }
	else if (ascii > 32 && ascii <= 64)
	{
		return true;
	}
	else if (ascii == 32)
	{
		return false;
	}
        else
        {
                return false;
        }
}

void parser(char* tokenline, struct pipeline* pipeline) 
{
        struct pipeline_command* new_command = malloc(sizeof(struct pipeline_command));
	for(int i = 0; i < MAX_ARGV_LENGTH; i++)
	{
		new_command->command_args[i] = NULL;
	}
	new_command->redirect_in_path = NULL;
	new_command->redirect_out_path = NULL;
	new_command->next = NULL;
       	// initializing our command struct and filling it with NULL values

        int str_loc = -1; // bandaid solution that keeps track of where we are in our tokens in order to detect <> chars
        char * tokenline_c = malloc(strlen(tokenline));
	memcpy(tokenline_c, tokenline, strlen(tokenline)); // duplicate our tokenline for a malleable version since we need tokenline for str_loc
        char * token;
        bool in = false; // in and out states in order to prevent mistakes like <>< x. keeps the latest in or out state
        bool out = false;
        while((token = strtok_r(tokenline, allchar, &tokenline))) // lets tokenize
        {
                char str[strlen(token)]; // this holds our str commands kind of like a str accumulator 
                strncpy(str, "", strlen(token)+1); // we fill this with blanks
                int j = 0; // this increments everytime we add a command to command args
                for(int i = 0; token[i] != '\0'; i++) // this loops through the token and does the following
                {
                        str_loc++; // update our position in our original tokenline by one
                        char ch = token[i]; // assigns our first ch in our token
                        int ascii = ch; // gets the ascii value for our above functions
                        if(tokenline_c[str_loc] == '<') // checks if it is <> chars in order to set states
                        {
                                in = true;
				out = false;
                        }
                        if (tokenline_c[str_loc] == '>')
                        {
                                out = true;
				in = false;
                        }
                        if(!isCorrect(ascii)) // our ascii filter returns false if we find a space 
                        {
                                if(strlen(str) > 0 && in) // then we check if we have a string built up, if we dont we continue
                                {				// if we do then we want to check if our in and out states are active
					new_command->redirect_in_path = malloc(strlen(str)+1);
                                        memcpy(new_command->redirect_in_path, str, strlen(str)+1);
                                        in = false;
					strncpy(str, "", strlen(token));
                                }
                                else if(strlen(str) > 0 && out) // if they are then we will store our token in either rin or rout
                                {				// and reset the string to blank
					new_command->redirect_out_path = malloc(strlen(str)+1);
					memcpy(new_command->redirect_out_path, str, strlen(str)+1);
                                        out = false;
					strncpy(str, "", strlen(token));
                                }
                                else if(strlen(str) > 0) // otherwise we put it into our command args and increment j;
                                {
                                        new_command->command_args[j] = malloc(strlen(str)+1);
					memcpy(new_command->command_args[j], str, strlen(str)+1);
                                        strncpy(str, "", strlen(token));
                                        j++;
                                }
                                continue;
                        }
                        else 
                        {
                           	strncat(str, &ch, 1); // this adds each char to our string if our IsCorrect function returns true
                        }
                        if(token[i + 1] == '\0') // bandaid: if we are one ch from the end, we want to store any str we have built up
                        {                        // in our new_command using the same filtering logic
				if(strlen(str) > 0 && in)
				{
					new_command->redirect_in_path = malloc(strlen(str)+1);
					memcpy(new_command->redirect_in_path, str, strlen(str)+1);
					in = false;
				}

				else if(strlen(str) > 0 && out)
				{
				 	new_command->redirect_out_path = malloc(strlen(str)+1);
					memcpy(new_command->redirect_out_path, str, strlen(str)+1);
					out = false;	
				}
				else if(strlen(str) > 0)
                                {
                                        new_command->command_args[j] = malloc(strlen(str)+1);
					memcpy(new_command->command_args[j], str, strlen(str)+1);
                                }
                        }
                }
        }
	if(pipeline->commands == NULL) // if this is our first command pipe then we point our pipe at this command
	{
        	pipeline->commands = new_command;  
	}
	else // otherwise we want to go down our command pipeline to find the next available slot
	{
		struct pipeline_command * temp_command;

		temp_command = pipeline->commands;
		while(temp_command->next != NULL)
		{
			temp_command = temp_command->next;
		}
		temp_command->next = new_command;
	}
	free(tokenline_c);	
}       

struct pipeline *pipeline_build(const char *command_line)
{	
        struct pipeline* new_pipeline = malloc(sizeof(struct pipeline)); // Create pipeline
	if(strlen(command_line) <= 2 || strlen(command_line) > MAX_LINE_LENGTH + 1) // Checks for no input and overload
	{
		new_pipeline = NULL;
		return new_pipeline;
	}

        new_pipeline->commands = NULL;
        new_pipeline->is_background = false;

        char* new_line = strdup(command_line);	// first we duplicate our command_line to make a malleable version to do some tokenizing
        char* token_line;
	char * testline = new_line;
        if(strstr(testline, ampersand)) // First we skim off the ampersand *note we probably didnt need this line since im strtok_r 
					// all chars anyways 
        {
                new_pipeline->is_background = true; // sets is_background to true if we find an ampersand
                strtok(testline, ampersand); // now we get rid of it
        }
        while((token_line = strtok_r(testline, pipe, &testline))) // main loop which seperates in instance by pipes
        {
                parser(token_line, new_pipeline); // here we go to pain
        }
	free(new_line);
        return new_pipeline; 
} 
    
void pipeline_free(struct pipeline *pipeline)
{   
        struct pipeline_command * temp_command;
	struct pipeline_command * next_command;
	if(pipeline == NULL) // so if we have a null pipeline we just want to free our structs and return
	{
		free(pipeline);
		return;
	}	
	
	temp_command = pipeline->commands;
	while(temp_command)
	{
		for(int i = 0; temp_command->command_args[i] != NULL; i++) // using our two struct pointers we go through and free every
									// all paths and args in each command block
		{
			free(temp_command->command_args[i]);
		}
		free(temp_command->redirect_in_path);
		free(temp_command->redirect_out_path);
		next_command = temp_command->next;
		free(temp_command);
		temp_command = next_command;
	}
	// go my child you are freeeeee
	free(pipeline);
}

