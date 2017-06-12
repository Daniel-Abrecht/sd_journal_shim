#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/uio.h>

int sd_journal_printv( int priority, const char *format, va_list ap ){
  vsyslog( priority, format, ap );
  return 0;
}

int sd_journal_print( int priority, const char *format, ... ){
  va_list ap;
  va_start(ap,format);
  int ret = sd_journal_printv( priority, format, ap );
  va_end(ap);
  return ret;
}

int sd_journal_sendv( const struct iovec *iov, int n ){
  if( n <= 0 )
    return -EINVAL;
  int message_len = 0;
  const char* message = 0;
  int priority = LOG_INFO;
  for( int i=0; i<n; i++ ){
    if( iov[i].iov_len <= 1 )
      continue;
    if( iov[i].iov_len >= strlen("MESSAGE=") && !strncmp( "MESSAGE=", iov[i].iov_base, strlen("MESSAGE=") ) ){
      message = (char*)iov[i].iov_base + strlen("MESSAGE=");
      message_len = iov[i].iov_len - strlen("MESSAGE=");
      if( !message_len )
        return -EINVAL;
    }else if( iov[i].iov_len >= strlen("PRIORITY=") && !strncmp( "PRIORITY=", iov[i].iov_base, strlen("PRIORITY=") ) ){
      int tmp = 0;
      size_t m = m = iov[i].iov_len - strlen("PRIORITY=");
      const char* p = (char*)iov[i].iov_base + strlen("PRIORITY=");
      if( m ) continue;
      for( size_t j=0; j<m; j++ ){
        if( !isdigit(p[j]) )
          goto next;
        int old = tmp;
        tmp *= 10;
        tmp += p[j] - '0';
        if( tmp < old ) // overflow
          continue;
      }
      priority = tmp;
    }
   next:;
  }
  if( !message )
    return -EINVAL;
  return sd_journal_print( priority, "%s", message );
}

int sd_journal_send( const char *format, ... ){
  if( !format )
    return -EINVAL;
  int ret = 0;
  char *message=0, *priority=0;
  struct iovec iov[2];
  va_list ap;
  va_start(ap,format);
  {
    const char* line = format;
    for( size_t n, eq; *line; line += n ){
      for( n=0, eq=0; line[n] && line[n] != '\n'; n++ )
        if( line[n] == '=' )
          eq = n;
      if( !eq )
        continue;
      char** str = 0;
      if( !strncmp("MESSAGE=",line,strlen("MESSAGE=")) ){
        str = &message;
      }else if( !strncmp("PRIORITY=",line,strlen("PRIORITY=")) ){
        str = &priority;
      }
      char* tmp = 0;
      if(!str){
        str = &tmp;
        line += eq;
      }
      if( vasprintf( str, line, ap ) == -1 )
        continue;
      if( tmp )
        free(tmp);
    }
  }
  va_end(ap);
  if( message ){
    size_t i = 0;
    iov[i].iov_base = message;
    iov[i].iov_len = strlen(message);
    i++;
    if( priority ){
      iov[i].iov_base = priority;
      iov[i].iov_len = strlen(priority);
      i++;
    }
    ret = sd_journal_sendv(iov,i);
  }else{
    ret = -EINVAL;
  }
  if( message )
    free( message );
  if( priority )
    free( priority );
  return ret;
}


int sd_journal_perror( const char *message ){
  if( message && *message ){
    return sd_journal_print( LOG_ERR, "%s: %s", message, strerror(errno) );
  }else{
    return sd_journal_print( LOG_ERR, "%s", strerror(errno) );
  }
  return 0;
}
