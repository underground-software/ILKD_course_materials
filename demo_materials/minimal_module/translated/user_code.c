#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct example
{
	char *message;
	size_t size;
};

static struct example *example_create(const char *msg)
{
	struct example *ex = malloc(sizeof *ex);
	if(!ex)
		goto out;
	ex->size = strlen(msg);
	ex->message = strdup(msg);
	if(!ex->message)
		goto out_free;
	return ex;
out_free:
	free(ex);
	ex = NULL;
out:
	return ex;
}

static void example_destroy(struct example *ex)
{
	free(ex->message);
	free(ex);
}

static bool example_update_message(struct example *ex, const char *msg)
{
	size_t size = strlen(msg);
	char *data = strdup(msg);
	if(!data)
		return false;
	free(ex->message);
	ex->message = data;
	ex->size = size;
	return true;
}

static char *example_get_message(struct example *ex)
{
	return ex->message;
}

int main(void)
{
	struct example *ex = example_create("hello");
	if(!ex)
		err(1, "unable to allocate memory");
	printf("%s\n", example_get_message(ex));
	if(!example_update_message(ex, "goodbye")) {
		int temperrno = errno;
		example_destroy(ex);
		errno = temperrno;
		err(1, "unable to update");
	}
	printf("%s\n", example_get_message(ex));
	example_destroy(ex);
	return 0;
}
