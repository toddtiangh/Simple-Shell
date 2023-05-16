#ifndef MYSHELL_PARSER_H
#define MYSHELL_PARSER_H
#include <stdbool.h>

#define MAX_LINE_LENGTH 512

#define MAX_ARGV_LENGTH (MAX_LINE_LENGTH / 2 + 1)

struct pipeline_command {
	char *command_args[MAX_ARGV_LENGTH]; /* List of pointers to each
						argument for a command. The
						first entry is the command
						name.
	char *redirect_in_path; /* Name of a file to redirect in from, or NULL
				   if there is no stdin redirect */
	char *redirect_out_path; /* Name of a file to redirect out to, or NULL if
				    there is no stdout redirect */
	struct pipeline_command *next; /* Pointer to the next command in the
					  pipeline. NULL if this is the last
					  command in the pipeline */
};

/*
 * Represents a collection of commands that are connected through a pipeline.
 */
struct pipeline {
	struct pipeline_command *commands; /* Pointer to the first command in
					      the pipeline*/
	bool is_background; /* True if this pipeline should execute in the
			       background */
};

void pipeline_free(struct pipeline *pipeline);

struct pipeline *pipeline_build(const char *command_line);

#endif /* MYSHELL_PARSER_H */
