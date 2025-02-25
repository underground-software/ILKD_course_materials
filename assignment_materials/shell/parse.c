#include "parse.h"

static char *buf;

struct cmd_iter
{
	uint16_t start;
	uint8_t len;
	uint8_t type;
};

static struct cmd_iter args[1 << 8];
static uint8_t next_arg;

struct pipe_iter
{
	uint8_t start, end;
};

static struct pipe_iter cmds[1 << 8];
static uint8_t next_cmd;

struct span
{
	char *start, *end;
};

static bool panic;

static enum arg_type redir_type(char c)
{
	switch(c)
	{
	case '<':
		return ARG_INPUT_REDIR;
	case '>':
		return ARG_OUTPUT_REDIR;
	default:
		return ARG_NORMAL;
	}
}

static struct cmd_iter parse_arg(struct span input)
{
	struct cmd_iter ret = {};
	/* ensure we have at least one character, failing this should be impossible */
	if(input.start >= input.end)
		goto panic;
	ret.type = redir_type(*input.start);
	if(ret.type != ARG_NORMAL)
		do
			++input.start;
		while(input.start != input.end && *input.start == ' ');
	ptrdiff_t signed_start = input.start - buf;
	if(signed_start < 0)
		goto panic;
	/* safe to cast because start >= 0 */
	size_t start = (size_t)signed_start;
	/* well defined to cast between unsigned types though truncation may occur */
	ret.start = (uint16_t)start;
	/* check whether it did by upcasting again and seeing if we lost information during the round trip */
	if(start != (size_t)ret.start)
		goto panic;
	/* jump through same hoops for length but with uint8_t */
	ptrdiff_t signed_length = input.end - input.start;
	if(signed_length < 0)
		goto panic;
	size_t length = (size_t)signed_length;
	ret.len = (uint8_t)length;
	if(length != (size_t)ret.len)
		goto panic;
	return ret;
panic:
	panic = true;
	return ret;
}

static void push_arg(struct cmd_iter arg)
{
	if(panic)
		return;
	args[next_arg++] = arg;
	if(!next_arg) /* overflow! */
		panic = true;
}

static struct pipe_iter parse_cmd(struct span input)
{
	struct pipe_iter ret =
	{
		.start = next_arg,
	};
	enum
	{
		BETWEEN_ARGS,
		BEFORE_REDIR,
		WITHIN_ARG,
	}
	state = BETWEEN_ARGS;
	struct span output;
	for(char *ptr = input.start; ptr != input.end; ++ptr)
	{
		char c = *ptr;
		switch(state)
		{
		case BETWEEN_ARGS:
			if(c == ' ')
				break;
			if(c == '<' || c == '>')
			{
				output.start = ptr;
				state = BEFORE_REDIR;
				break;
			}
			output.start = ptr;
			state = WITHIN_ARG;
			break;
		case BEFORE_REDIR:
			if(c == ' ')
				break;
			//empty redirection, not our problem
			if(c == '<' || c == '>')
			{
				output.end = ptr;
				push_arg(parse_arg(output));
				break;
			}
			state = WITHIN_ARG;
			break;
		case WITHIN_ARG:
			if(c != ' ' && c != '<' && c != '>')
				break;
			output.end = ptr;
			push_arg(parse_arg(output));
			if(c == ' ')
			{
				state = BETWEEN_ARGS;
				break;
			}
			output.start = ptr;
			state = BEFORE_REDIR;
			break;
		default:
			unreachable();
		}
	}
	if(BETWEEN_ARGS != state)
	{
		output.end = input.end;
		push_arg(parse_arg(output));
	}
	ret.end = next_arg;
	return ret;
}

static void push_cmd(struct pipe_iter cmd)
{
	if(panic)
		return;
	cmds[next_cmd++] = cmd;
	if(!next_cmd) /* overflow! */
		panic = true;
}

static void parse_pipeline(struct span input)
{
	struct span output =
	{
		.start = input.start
	};
	for(char *ptr = input.start; ptr != input.end; ++ptr)
	{
		if('|' != *ptr)
			continue;
		output.end = ptr;
		push_cmd(parse_cmd(output));
		output.start = ptr + 1;
	}
	output.end = input.end;
	push_cmd(parse_cmd(output));
}

bool parse_line(char *data, uint16_t size, struct pipeline *out)
{

	panic = true; /* stop iter functions from working if early return occurs */
	if(!size)
		return false;
	if(data[size - 1] != '\n')
		return false;
	struct span line =
	{
		.start = data,
		.end = data+ size - 1,
	};
	for(char *ptr = line.start; ptr != line.end; ++ptr)
		if( *ptr < ' ' || '~' < *ptr )
			return false;
	buf = data;
	next_arg = 0;
	next_cmd = 0;
	panic = false;
	parse_pipeline(line);
	if(panic)
		return false;
	*out = (struct pipeline)
	{
		.cmd_iter = &cmds[0],
		.num_cmds = next_cmd,
	};
	return true;
}

struct command pipe_iter_next(struct pipe_iter **iter)
{
	struct pipe_iter *ptr = *iter;
	struct command ret =
	{
		.arg_iter = &args[ptr->start],
		.num_args = ptr->end - ptr->start,
	};
	++*iter;
	return ret;
}

struct argument cmd_iter_next(struct cmd_iter **iter)
{
	struct cmd_iter *ptr = *iter;
	/* null terminate. this reaches into the next span, but there must be padding */
	buf[ptr->start + ptr->len] = '\0';
	struct argument ret =
	{
		.data = &buf[ptr->start],
		.type = ptr->type,
	};
	++*iter;
	return ret;
}
