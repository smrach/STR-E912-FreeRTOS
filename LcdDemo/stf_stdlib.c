/**
 * <b>File:</b> stf_stdlib.c
 *
 * <b>Project:</b> STR91x Eclipse demo
 *
 * <b>Description:</b> Lite implementation of the semi-hosted C runtime library.
 *                     TODO: to be completed.
 *
 * <b>Created:</b> 27/08/2008
 *
 * <dl>
 * <dt><b>Author</b>:</dt>
 * <dd>Stefano Oliveri</dd>
 * <dt><b>E-mail:</b></dt>
 * <dd>software@stf12.net</dd>
 * </dl>
 */

#if 0

#include <stddef.h>
#include <sys/stat.h>

/**
 * The "__close" function should close the file corresponding to
 * "handle".  It should return 0 on success and nonzero on failure.
 *
 * @param handle
 * @return
 */
int _close(int handle)
{
  return 0;
}

/**
 * The "__lseek" function makes the next file operation (__read or
 * __write) act on a new location.  The parameter "whence" specifies
 * how the "offset" parameter should be interpreted according to the
 * following table:
 *
 *  0 (=SEEK_SET) - Goto location "offset".
 *  1 (=SEEK_CUR) - Go "offset" bytes from the current location.
 *  2 (=SEEK_END) - Go to "offset" bytes from the end.
 *
 * This function should return the current file position, or -1 on
 * failure.
 *
 * @param handle
 * @param offset
 * @param whence
 * @return
 */
long _lseek(int handle, long offset, int whence)
{
  return -1;
}


int outbyte( int ch );

/**
 *
 * @param handle
 * @param buffer
 * @param size
 * @return
 */
size_t _write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  if (buffer == 0)
  {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
    return 0;
  }

  // Check for supported handle
//  if (handle != _STDOUT && handle != _STDERR)
//  {
//    return _ERROR;
//  }

  for (/* Empty */; size != 0; --size)
  {
    if (outbyte(*buffer++) < 0)
    {
      return -1;
    }

    ++nChars;
  }

  return nChars;
}

int _low_level_getchar()
{
	return -1;
}

/**
 * The "__read" function reads a number of bytes, at most "size" into
 * the memory area pointed to by "buffer".  It returns the number of
 * bytes read, 0 at the end of the file, or _LLIO_ERROR if failure
 * occurs.
 *
 * The template implementation below assumes that the application
 * provides the function "_low_level_getchar".  It should return a
 * character value, or -1 on failure.
 *
 * @param handle
 * @param buffer
 * @param size
 * @return
 */
size_t _read(int handle, unsigned char * buffer, size_t size)
{
  int nChars = 0;

  // Check for supported handle
//  if (handle != _LLIO_STDIN)
//  {
//    return _LLIO_ERROR;
//  }

  for (/* Empty */; size > 0; --size)
  {
    int c = _low_level_getchar();
    if (c < 0)
      break;

    *buffer++ = c;
    ++nChars;
  }

  return nChars;
}

/**
 * Get information about an open file.
 *
 * @param fd File descriptor of open file.
 * @param buffer Pointer to structure to store results.
 * @return Return 0 if the file-status information is obtained.
 * A return value of –1 indicates an error, in which case errno is set to EBADF,
 * indicating an invalid file descriptor.
 */
int _fstat(int fd, struct stat *buffer)
{
	return -1;
}

/**
 * The function determines whether fd is associated with a character device
 * (a terminal, console, printer, or serial port).
 *
 * @param fd File descriptor referring to device to be tested.
 * @return a nonzero value if the descriptor is associated with a character device, 0 otherwise.
 */
int _isatty(int fd)
{
	return 0;
}

void *_sbrk( ptrdiff_t nbytes )
{
	return 0;
}

#endif
