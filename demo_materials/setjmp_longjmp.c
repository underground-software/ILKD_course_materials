#include <stdio.h>
#include <setjmp.h>

static jmp_buf buffer;

static void foo(void);
static void bar(void);

static void foo(void)
{
	puts("In foo");
	bar();
	puts("foo: this will not be printed");
}

static void bar(void)
{
	puts("In bar");
	longjmp(buffer, 333);  // Jump back to where setjmp was called
}

int main(void)
{
	switch (setjmp(buffer)) {
        // setjmp returns 0 when saving context
	case 0:
		puts("Calling foo");
		foo();
		puts("main: this will not be printed");
		break;
	// longjmp lands here here since we return 333 in bar()
	case 333:
		puts("Returned to main from bar via longjmp");
		longjmp(buffer, 334);
		puts("no second longjmp :(");
		break;
	case 334:
		puts("more longjump");
		break;
	default:
		puts("unexpected return value from setjmp");
		break;
	}

	return 0;
}
