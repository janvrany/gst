#line 1 "./printf.in"
/*  -*- Mode: C -*-  */

/* printf.in --- printf clone for argv arrays
 * Copyright (C) 1998, 1999, 2000, 2002 Gary V. Vaughan
 * Originally by Gary V. Vaughan, 1998
 * This file is part of Snprintfv
 *
 * Snprintfv is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Snprintfv program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * As a special exception to the GNU General Public License, if you
 * distribute this file as part of a program that also links with and
 * uses the libopts library from AutoGen, you may include it under
 * the same distribution terms used by the libopts library.
 */

/* Code: */

#ifndef SNPRINTFV_SNPRINTFV_H
#define SNPRINTFV_SNPRINTFV_H 1

#include <snprintfv/compat.h>
#include <snprintfv/filament.h>
#include <snprintfv/stream.h>
#include <snprintfv/mem.h>

#ifdef HAVE_WCHAR_H
#  include <wchar.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* The type of each element in the table of printf specifiers. */
struct spec_entry;

typedef enum
{
  SNV_ERROR = -1,
  SNV_OK
}
snv_status;

/* Basic states required by the parser.  On initialisation the parser
   will be in SNV_STATE_BEGIN, and tokens will be parsed by the registered
   functions until the parser reached SNV_STATE_END. */
#define SNV_STATE_BEGIN		1
#define SNV_STATE_END		0

/* States needed to support:
   %[<number>$]<flags>[<width>|\*][.<precision>|\*]<modifiers><specifier> */
#define SNV_STATE_FLAG          (1 << 1)
#define SNV_STATE_WIDTH         (1 << 2)
#define SNV_STATE_PRECISION     (1 << 3)
#define SNV_STATE_MODIFIER      (1 << 4)
#define SNV_STATE_SPECIFIER     (1 << 5)

/* First state available to the user */
#define SNV_STATE_USER_FIRST    (1 << 8)

/* Mask for states available to the user */
#define SNV_STATE_USER_MASK     ~(SNV_STATE_USER_FIRST - 1)

typedef struct printf_info
{
  int count;			/* accumulated count, or SNV_ERROR */
  int state;			/* one of the defines above */
  Filament *error;		/* accumulated error details */

  const char *format;		/* pointer to format string */
  int argc;			/* number of arguments used by format */
  int argindex;			/* number of non-dollar arguments used so far */

  int dollar;			/* standard parser state, as in glibc */
  int prec;			/* from this field on, as in glibc */
  int width;

  snv_pointer extra;
  int type;

  char spec;
  char pad;
  unsigned is_long_double:1;
  unsigned is_char:1;
  unsigned is_short:1;
  unsigned is_long:1;
  unsigned alt:1;
  unsigned space:1;
  unsigned left:1;
  unsigned showsign:1;
  unsigned group:1;
  unsigned wide:1;

  const union printf_arg *args;
} printf_info;

/**
 * printf_arg:
 * @pa_char: an unsigned %char
 * @pa_wchar: a %wchar_t
 * @pa_short_int: a %short integer
 * @pa_int: an %int
 * @pa_long_int: a %long integer
 * @pa_long_long_int: the widest signed integer type in use on the host
 * @pa_u_short_int: an unsigned %short integer
 * @pa_u_int: an unsigned %int
 * @pa_u_long_int: an unsigned %long integer
 * @pa_u_long_long_int: the widest unsigned integer type in use on the host
 * @pa_float: a %float
 * @pa_double: a %double
 * @pa_long_double: a long %double, or a simple %double if it is the widest floating-point type in use on the host
 * @pa_string: a %const pointer to %char
 * @pa_wstring: a %const pointer to %wchar_t
 * @pa_pointer: a generic pointer
 *
 * The various kinds of arguments that can be passed to printf.
 */
typedef union printf_arg
{
  unsigned char pa_char;
  snv_wchar_t pa_wchar;
  short int pa_short_int;
  int pa_int;
  long int pa_long_int;
  intmax_t pa_long_long_int;
  unsigned short int pa_u_short_int;
  unsigned int pa_u_int;
  unsigned long int pa_u_long_int;
  uintmax_t pa_u_long_long_int;
  float pa_float;
  double pa_double;
  long double pa_long_double;
  const char *pa_string;
  const snv_wchar_t *pa_wstring;
  snv_constpointer pa_pointer;
} printf_arg;

/**
 * PRINTF_ERROR:
 * @pi: A pointer to the current state for the parser
 * @str: The error message
 *
 * Append an error that will be returned by printf_strerror.
 */
#define PRINTF_ERROR(pi, str)                                         \
        printf_error(pi, __FILE__, __LINE__, SNV_ASSERT_FMT, str);

typedef int printf_function (STREAM *stream, struct printf_info *pparser, union printf_arg const * args);

typedef int printf_arginfo_function (struct printf_info *pparser, size_t n, int *argtypes);

/**
 * spec_entry:
 * @spec: the specifier character that was matched
 * @type: when @arg is NULL, the type of the only argument to the specifier
 * @fmt: the handler function to actually print the arguments to the specifier
 * @arg: the handler function to tell %printf about the types of the arguments to the specifier
 * @user: the user data for the specifier, accessible to the handler function
 *
 * This is returned by register_printf_function. 
 */
typedef struct spec_entry
{
  int spec;
  int unused;  /* for binary compatibility */
  int type;
  printf_function *fmt;
  printf_arginfo_function *arg;
  snv_pointer user;
}
spec_entry;

/**
 * register_callback_function: printf.h
 * @spec: the character which will trigger the functions, cast to an unsigned int.
 * @fmt: the handler function to actually print the arguments to the specifier
 * @arg: the handler function to tell %printf about the types of the arguments to the specifier
 * 
 * Register the pair made of @fmt and @arg, so that it is called
 * when @spec is encountered in a format string.  If you create
 * a shared library with an entry point named
 * %snv_register_printf_funcs, and put the library in the
 * search path given by the environment library %LTDL_LIBRARY_PATH,
 * that entry point will be called when %libsnprintfv is initialized,
 * passing a pointer to this kind of function (actually, a pointer
 * to %register_printf_function) to it.  This functionality is only
 * present when the library is installed, not when it is built as
 * a convenience library.
 * 
 * Return value:
 * Returns %NULL if @func was not successfully registered, a
 * %spec_entry with the information on the function if it was.
 **/
typedef spec_entry *register_callback_function (unsigned spec, printf_function *func, printf_arginfo_function *arginfo);

/* Codes to determine basic types.

   These values cover all the standard format specifications.
   Users can add new values after PA_LAST for their own types.  */

enum
{
  PA_INT,			/* int */
  PA_CHAR,			/* int, cast to char */
  PA_WCHAR,			/* wide char */
  PA_STRING,			/* const char *, a '\0'-terminated string */
  PA_WSTRING,			/* const snv_wchar_t *, wide character string */
  PA_POINTER,			/* void * */
  PA_FLOAT,			/* float */
  PA_DOUBLE,			/* double */
  PA_LAST,
  PA_UNKNOWN = -1
};

/* Flag bits that can be set in a type. */
#define PA_TYPE_MASK		0x00ff
#define	PA_FLAG_MASK		~SNV_TYPE_MASK

#define	PA_FLAG_LONG_LONG	(1 << 8)
#define	PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define	PA_FLAG_LONG		(1 << 9)
#define	PA_FLAG_SHORT		(1 << 10)
#define PA_FLAG_UNSIGNED	(1 << 11)
#define	PA_FLAG_CHAR		(1 << 12)
#define	PA_FLAG_PTR		(1 << 13)

/**
 * SNV_EMIT:
 * @ch: the character to be printed
 * @stream: the stream on which to print
 * @count: a variable to be updated with the count of printed
 * characters
 *
 * Maintain the count while putting @ch in @stream, also be careful about
 * handling %NULL stream if the handler is being called purely to count
 * output size.
 **/
#define SNV_EMIT(ch, stream, count) \
  SNV_STMT_START { \
    if (stream) \
      { \
	if (count >= 0) \
	  { \
	    int m_status = stream_put((ch), stream); \
	    count = m_status < 0 ? m_status : count + m_status; \
	  } \
      } \
    else \
      { \
        (void)(ch); \
	count++; \
      } \
  } SNV_STMT_END

#line 269 "./printf.in"
/**
 * printf_generic_info:   
 * @pinfo: the current state information for the format
 * string parser.
 * @n: the number of available slots in the @argtypes array
 * @argtypes: the pointer to the first slot to be filled by the
 * function
 *
 * An example implementation of a %printf_arginfo_function, which
 * takes the basic type from the type given in the %spec_entry
 * and adds flags depending on what was parsed (e.g. %PA_FLAG_SHORT
 * is %pparser->is_short and so on).
 *
 * Return value:
 * Always 1.
 */
extern int printf_generic_info (struct printf_info *const pinfo, size_t n, int *argtypes);


/**
 * printf_generic:   
 * @stream: the stream (possibly a struct printfv_stream appropriately
 * cast) on which to write output.
 * @pinfo: the current state information for the format string parser.
 * @args: the pointer to the first argument to be read by the handler
 *
 * An example implementation of a %printf_function, used to provide easy
 * access to justification, width and precision options.
 *
 * Return value:
 * The number of characters output.
 **/
extern int printf_generic (STREAM *stream, struct printf_info *const pinfo, union printf_arg const *args);


#line 270 "./printf.in"
/**
 * register_printf_function:  
 * @spec: the character which will trigger @func, cast to an unsigned int.
 * @fmt: the handler function to actually print the arguments to the specifier
 * @arg: the handler function to tell %printf about the types of the arguments to the specifier
 * 
 * Register the pair made of @fmt and @arg, so that it is called
 * when @spec is encountered in a format string.
 * 
 * Return value:
 * Returns %NULL if @func was not successfully registered, a
 * %spec_entry with the information on the function if it was.
 **/
extern spec_entry * register_printf_function (unsigned spec, printf_function *fmt, printf_arginfo_function *arg);


/**
 * printf_strerror:  
 *
 * Communicate information on the last error in a printf
 * format string.
 *
 * Return value:
 * A string describing the last error which occurred during the
 * parsing of a printf format string.  It is the responsibility
 * of the caller to free the string.
 */
extern char * printf_strerror (void);


/**
 * printf_error:  
 * @pinfo: pointer to the current parser state.
 * @file: file where error was detected.
 * @line: line where error was detected.
 * @func1: " (" if function is supplied by compiler.
 * @func2: function where error was detected, if supplied by compiler.
 * @func3: ")" if function is supplied by compiler.
 * @error_message: new error message to append to @pinfo.
 * 
 * The contents of @error_message are appended to the @pinfo internal
 * error string, so it is safe to pass static strings or recycle the
 * original when this function returns.
 * 
 * Return value:
 * The address of the full accumulated error message in @pinfo is
 * returned.
 **/
extern char * printf_error (struct printf_info *pinfo, const char *file, int line, const char *func1, const char *func2, const char *func3, const char *error_message);


/**
 * parse_printf_format:  
 * @format: a % delimited format string.
 * @n: the size of the @argtypes vector
 * @argtypes: a vector of ints, to be filled with the argument types from @format
 * 
 * Returns information about the number and types of
 * arguments expected by the template string @format.
 * The argument @n specifies the number of elements in the array
 * @argtypes.  This is the maximum number of elements that
 * the function will try to write.
 *
 * Return value:
 * The total number of arguments required by @format.  If this
 * number is greater than @n, then the information returned
 * describes only the first @n arguments.  If you want information
 * about additional arguments, allocate a bigger array and call
 * this function again. If there is an error, then %SNV_ERROR
 * is returned instead.
 **/
extern size_t parse_printf_format (const char *format, int n, int *argtypes);


/**
 * stream_printfv:  
 * @stream: an initialised stream structure.
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to @format, and write
 * the results to @stream.  If @stream is %NULL, only count the
 * number of characters needed to output the format.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int stream_printfv (STREAM *stream, const char *format, snv_constpointer const *ap);


/**
 * stream_vprintf:  
 * @stream: an initialised stream structure.
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to @stream.  If @stream is %NULL, only count the
 * number of characters needed to output the format.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int stream_vprintf (STREAM *stream, const char *format, va_list ap);


/**
 * stream_printf:  
 * @stream: an initialised stream structure.
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to @stream.  If @stream is %NULL, only count the
 * number of characters needed to output the format.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int stream_printf SNV_GNUC_PRINTF((STREAM * stream, const char *format, ...), 2, 3);


/**
 * snv_fdputc:  
 * @ch: A single character to be added to @stream.
 * @stream: The stream in which to write @ch.
 * 
 * A StreamPut function for use in putting characters
 * into STREAMs holding a file descriptor.
 * 
 * Return value:
 * The value of @ch that has been put in @stream, or -1 in case of
 * an error (errno will be set to indicate the type of error).
 **/
extern int snv_fdputc (int ch, STREAM *stream);


/**
 * snv_dprintf:  
 * @fd: an open file descriptor.
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to the file descriptor @fd.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_dprintf SNV_GNUC_PRINTF((int fd, const char *format, ...), 2, 3);


/**
 * snv_vdprintf:  
 * @fd: an open file descriptor.
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to the file descriptor @fd.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_vdprintf (int fd, const char *format, va_list ap);


/**
 * snv_dprintfv:  
 * @fd: an open file descriptor.
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to @format, and write
 * the results to file descriptor @fd.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_dprintfv (int fd, const char *format, snv_constpointer const args[]);


/**
 * snv_fileputc:  
 * @ch: A single character to be added to @stream.
 * @stream: The stream in which to write @ch.
 * 
 * A StreamPut function for use in putting characters
 * into STREAMs holding a FILE*.
 * 
 * Return value: 
 * The value of @ch that has been put in @stream.
 **/
extern int snv_fileputc (int ch, STREAM *stream);


/**
 * snv_printf:  
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to the standard output stream.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_printf SNV_GNUC_PRINTF((const char *format, ...), 1, 2);


/**
 * snv_vprintf:  
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to the standard output stream.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_vprintf (const char *format, va_list ap);


/**
 * snv_printfv:  
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to the string @format,
 * and write the result to the standard output stream.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_printfv (const char *format, snv_constpointer const args[]);


/**
 * snv_fprintf:  
 * @file: a stdio.h FILE* stream.
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to the @file stream.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_fprintf SNV_GNUC_PRINTF((FILE * file, const char *format, ...), 2, 3);


/**
 * snv_vfprintf:  
 * @file: a stdio.h FILE* stream.
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to the @file stream.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_vfprintf (FILE *file, const char *format, va_list ap);


/**
 * snv_fprintfv:  
 * @file: a stdio.h FILE* stream.
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to @format, and write
 * the results to @file.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_fprintfv (FILE *file, const char *format, snv_constpointer const args[]);


/**
 * snv_bufputc:  
 * @ch: A single character to be added to @stream.
 * @stream: The stream in which to write @ch.
 * 
 * A StreamPut function for use in putting characters
 * into STREAMs holding a char buffer.
 * 
 * Return value:
 * The value of @ch that has been put in @stream.
 **/
extern int snv_bufputc (int ch, STREAM *stream);

/**
 * snv_sprintf:  
 * @buffer: a preallocated char* buffer.
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to the string @buffer.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_sprintf SNV_GNUC_PRINTF((char buffer[], const char *format, ...), 2, 3);


/**
 * snv_vsprintf:  
 * @buffer: a preallocated char* buffer.
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to the string @buffer.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_vsprintf (char buffer[], const char *format, va_list ap);


/**
 * snv_sprintfv:  
 * @buffer: a preallocated char* buffer.
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to @format, and write
 * the results to the string @buffer.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_sprintfv (char buffer[], const char *format, snv_constpointer const args[]);


/**
 * snv_snprintf:  
 * @buffer: a preallocated char* buffer.
 * @limit: the maximum number of characters to write into @buffer.
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to the string @buffer, truncating the formatted string
 * if it reaches @limit characters in length.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_snprintf SNV_GNUC_PRINTF((char buffer[], unsigned long limit, const char *format, ...), 3, 4);


/**
 * snv_vsnprintf:  
 * @buffer: a preallocated char* buffer.
 * @limit: the maximum number of characters to write into @buffer.
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to the string @buffer, truncating the formatted string
 * if it reaches @limit characters in length.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_vsnprintf (char buffer[], unsigned long limit, const char *format, va_list ap);


/**
 * snv_snprintfv:  
 * @buffer: a preallocated char* buffer.
 * @limit: the maximum number of characters to write into @buffer.
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to @format, and write
 * the results to the string @buffer, truncating the formatted string
 * if it reaches @limit characters in length.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_snprintfv (char buffer[], unsigned long limit, const char *format, snv_constpointer const args[]);


/**
 * snv_filputc:  
 * @ch: A single character to be added to @stream.
 * @stream: The stream in which to write @ch.
 * 
 * A StreamPut function for use in putting characters
 * into STREAMs holding a Filament*.
 * 
 * Return value: 
 * The value of @ch that has been put in @stream.
 **/
extern int snv_filputc (int ch, STREAM *stream);

/**
 * snv_asprintf:  
 * @result: the address of a char * variable.
 * @format: a % delimited format string.
 * @va_alist: a varargs/stdargs va_list.
 * 
 * Format the elements of @va_alist according to @format, and write
 * the results to an internally allocated buffer whose address is
 * stored in @result (and should be freed by the caller) unless
 * there is an error.
 *
 * Yes, this interface is cumbersome and totally useless.  It would
 * have been better to simply return the allocated address, but
 * it turns out that somebody wasn't thinking much when adding 
 * asprintf to libiberty a few years ago.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_asprintf SNV_GNUC_PRINTF((char **result, const char *format, ...), 2, 3);


/**
 * snv_vasprintf:  
 * @result: the address of a char * variable.
 * @format: a % delimited format string.
 * @ap: a varargs/stdargs va_list.
 * 
 * Format the elements of @ap according to @format, and write
 * the results to an internally allocated buffer whose address is
 * stored in @result (and should be freed by the caller) unless
 * there is an error.
 * 
 * Above moaning for asprintf applies here too.
 *
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_vasprintf (char **result, const char *format, va_list ap);


/**
 * snv_asprintfv:  
 * @result: the address of a char * variable.
 * @format: a % delimited format string.
 * @args: a vector of argument addresses to match @format.
 * 
 * Format the elements of @args according to @format, and write
 * the results to an internally allocated buffer whose address is
 * stored in @result (and should be freed by the caller) unless
 * there is an error.
 * 
 * Above moaning for asprintf applies here too.
 * 
 * Return value:
 * The number of characters written is returned, unless there is
 * an error, when %SNV_ERROR is returned.
 **/
extern int snv_asprintfv (char **result, const char *format, snv_constpointer const args[]);


#line 271 "./printf.in"

/* If you don't want to use snprintfv functions for *all* of your string
   formatting API, then define COMPILING_SNPRINTFV_C and use the snv_
   prefix for the entry points below. */
#ifndef COMPILING_PRINTF_C
#undef printf
#undef vprintf
#undef dprintf
#undef vdprintf
#undef fprintf
#undef vfprintf
#undef sprintf
#undef vsprintf
#undef snprintf
#undef vsnprintf
#undef asprintf
#undef vasprintf
#undef asprintfv
#undef dprintfv
#undef fprintfv
#undef sprintfv
#undef printfv
#undef snprintfv
#define printf          snv_printf
#define vprintf         snv_vprintf
#define dprintf         snv_dprintf
#define vdprintf        snv_vdprintf
#define fprintf         snv_fprintf
#define vfprintf        snv_vfprintf
#define sprintf         snv_sprintf
#define vsprintf        snv_vsprintf
#define snprintf        snv_snprintf
#define vsnprintf       snv_vsnprintf
#define asprintf        snv_asprintf
#define vasprintf       snv_vasprintf
#define asprintfv	snv_asprintfv
#define dprintfv	snv_dprintfv
#define fprintfv	snv_fprintfv
#define sprintfv	snv_sprintfv
#define printfv		snv_printfv
#define snprintfv	snv_snprintfv
#endif				/* !COMPILING_SNPRINTFV_C */
#ifdef __cplusplus
}
#endif

#endif				/* SNPRINTFV_SNPRINTFV_H */

/* snprintfv.h ends here */
