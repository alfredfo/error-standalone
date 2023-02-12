// https://github.com/void-linux/void-packages/blob/master/srcpkgs/musl-legacy-compat/files/error.h

#ifndef _ERROR_H_
#define _ERROR_H_

#define _GNU_SOURCE /* Needed for program_invocation_name */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(__GNUC__) && defined(__has_builtin)
# if __has_builtin(__builtin_va_arg_pack)
#  define HAS_BUILTIN_VA_ARG_PACK
# endif
#endif

#if defined __has_attribute
#  if __has_attribute (always_inline)
#    define ATTR_ALWAYS_INLINE __attribute__((always_inline))
#  endif
#endif

#ifndef ATTR_ALWAYS_INLINE
# define ATTR_ALWAYS_INLINE
#endif

static unsigned int error_message_count = 0;

static ATTR_ALWAYS_INLINE inline void error(int status, int errnum, const char* format, ...)
{
	/* should be fflush(stdout), but that's unspecified if stdout has been closed;
	 * stick with fflush(NULL) for simplicity (glibc checks if the fd is still valid) */
	fflush(NULL);

	fprintf(stderr, "%s: ", program_invocation_name);

	#ifdef HAS_BUILTIN_VA_ARG_PACK
	fprintf(stderr, format, __builtin_va_arg_pack());
	#else
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	#endif

	if (errnum)
		fprintf(stderr, ": %s", strerror(errnum));
	fprintf(stderr, "\n");
	error_message_count++;
	if (status)
		exit(status);
}

static int error_one_per_line = 0;

static ATTR_ALWAYS_INLINE inline void error_at_line(int status, int errnum, const char *filename,
		unsigned int linenum, const char *format, ...)
{
	if (error_one_per_line) {
		static const char *old_filename;
		static int old_linenum;
		if (linenum == old_linenum && filename == old_filename)
			return;
		old_filename = filename;
		old_linenum = linenum;
	}

	fprintf(stderr, "%s: %s:%u: ", program_invocation_name, filename, linenum);

	#ifdef HAS_BUILTIN_VA_ARG_PACK
	fprintf(stderr, format, __builtin_va_arg_pack());
	#else
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	#endif

	if (errnum)
		fprintf(stderr, ": %s", strerror(errnum));
	fprintf(stderr, "\n");
	error_message_count++;
	if (status)
		exit(status);
}


#endif	/* _ERROR_H_ */
