#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct pipe_iter;
struct cmd_iter;

struct pipeline
{
	/* pass to pipe_iter_next to receive struct command */
	struct pipe_iter *cmd_iter;
	/* max number of times it can be iterated */
	size_t num_cmds;
};

struct command
{
	/* pass to cmd_iter_next to receive struct argument */
	struct cmd_iter *arg_iter;
	/* max number of times it can be iterated */
	size_t num_args;
};

enum arg_type
{
	ARG_NORMAL,
	ARG_INPUT_REDIR,
	ARG_OUTPUT_REDIR,
};

struct argument
{
	/* null terminated */
	char *data;
	/* one of enum arg_type */
	size_t type;
};

/* iter is a by-reference in/out argument and must be nonnull always.
it is undefined to invoke next more than the maximum number of times */

struct command pipe_iter_next(struct pipe_iter **iter);
struct argument cmd_iter_next(struct cmd_iter **iter);

/* parse one line of shell input. Data must be mutable because it is modified during parsing.
Returns true if it is able to parse the line and provides output value in out. Returns false
on error and out is not modified. The input array must consist of only printable ascii values
(0x20-0x7E) except for the final byte which must be a newline (0x0A). The data is not null
terminated and the size must include the the trailing newline */
bool parse_line(char *data, uint16_t size, struct pipeline *out);

#endif//PARSER_H
