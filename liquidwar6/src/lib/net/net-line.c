/*
  Liquid War 6 is a unique multiplayer wargame.
  Copyright (C)  2005, 2006, 2007, 2008, 2009, 2010  Christian Mauduit <ufoot@ufoot.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  

  Liquid War 6 homepage : http://www.gnu.org/software/liquidwar6/
  Contact author        : ufoot@ufoot.org
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "net.h"
#include "net-internal.h"

#define CHAR_CR '\x0d'
#define CHAR_LF '\x0a'
#define CHAR_0 '\0'
#define TRAIL_SIZE 2

static char trail[TRAIL_SIZE + 1] = { CHAR_CR, CHAR_LF, CHAR_0 };

/**
 * lw6net_recv_line_tcp:
 *
 * @sock: the socket descriptor
 *
 * Receives a line terminated by LF ("\n", chr(10)) or
 * CR/LF ("\r\n", chr(10)chr(13)) on a TCP socket, that is,
 * stream oriented. If there's no complete line
 * available, function returns immediately with NULL. Same
 * if socket is closed, broken, whatever. Only if there's
 * something consistent will the function return non-NULL.
 *
 * Return value: a dynamically allocated string with the
 *   content received. The tailing (CR)/LF is stripped.
 */
char *
lw6net_recv_line_tcp (int sock)
{
  char *ret = NULL;
  int line_size = 0;
  float line_delay;
  int wanted_size;
  int available_size;
  int trail_size;
  char *pos_lf;
  char *line_buf = NULL;

  if (sock >= 0)
    {
      line_size = _lw6net_global_context->const_data.line_size;
      line_delay = _lw6net_global_context->const_data.line_delay;
      line_buf = LW6SYS_CALLOC (line_size + 3);
      if (line_buf)
	{
	  available_size =
	    lw6net_tcp_peek (sock, line_buf, line_size + 2, 0.0f);
	  if (available_size > 0)
	    {
	      pos_lf = strchr (line_buf, CHAR_LF);
	      if (pos_lf)
		{
		  trail_size = (pos_lf > line_buf
				&& pos_lf[-1] == CHAR_CR) ? 2 : 1;
		  wanted_size = (pos_lf + 1 - line_buf);
		  pos_lf[1 - trail_size] = CHAR_0;
		  ret = lw6sys_str_copy (line_buf);
		  if (ret)
		    {
		      lw6sys_str_cleanup (ret);
		    }

		  // remove data from queue
		  lw6net_tcp_recv (sock, line_buf, wanted_size,
				   line_delay, 0);
		}
	    }
	  LW6SYS_FREE (line_buf);
	}
    }

  return ret;
}

/**
 * lw6net_send_line_tcp:
 *
 * @sock: the socket descriptor
 * @line: the line to be sent, without the "\n" at the end
 *
 * Sends a line terminated by LF ("\n", chr(10)) on a TCP
 * socket, that is, stream oriented. The "\n" is automatically
 * added, do not bother sending it.
 *
 * Return value: non-zero if success
 */
int
lw6net_send_line_tcp (int sock, char *line)
{
  int ret = 0;
  int line_size = 0;
  float line_delay;
  int wanted_size = 0;

  if (sock >= 0 && line)
    {
      line_size = _lw6net_global_context->const_data.line_size;
      line_delay = _lw6net_global_context->const_data.line_delay;
      wanted_size = strlen (line);
      if (wanted_size > line_size)
	{
	  lw6sys_log (LW6SYS_LOG_WARNING,
		      _("stripping line \"%s\" of size %d, limit is %d"),
		      line, wanted_size, line_size);
	  wanted_size = line_size;
	}

      ret =
	lw6net_tcp_send (sock, line, wanted_size, line_delay, 0)
	&& lw6net_tcp_send (sock, trail, TRAIL_SIZE, line_delay, 0);
    }

  return ret;
}

/**
 * lw6net_recv_line_udp:
 *
 * @sock: the socket descriptor
 * @incoming_ip: the IP address of the sender (returned)
 * @incoming_port: the IP port of the sender (returned)
 *
 * Receives a line terminated by LF ("\n", chr(10)) or
 * CR/LF ("\r\n", chr(10)chr(13)) on a UDP socket, that is,
 * datagram oriented. If there's no complete line
 * available, function returns immediately with NULL. Same
 * if socket is closed, broken, whatever. Only if there's
 * something consistent will the function return non-NULL.
 * By-value parameters allow the caller to know where the
 * data come from.
 *
 * Return value: a dynamically allocated string with the
 *   content received. The tailing (CR)/LF is stripped.
 */
char *
lw6net_recv_line_udp (int sock, char **incoming_ip, int *incoming_port)
{
  char *ret = NULL;
  int line_size = 0;
  int wanted_size;
  int available_size;
  int trail_size;
  char *pos_lf;
  char *line_buf = NULL;

  if (sock >= 0)
    {
      line_size = _lw6net_global_context->const_data.line_size;
      line_buf = LW6SYS_CALLOC (line_size + 3);
      if (line_buf)
	{
	  available_size =
	    lw6net_udp_peek (sock, line_buf, line_size + 2,
			     incoming_ip, incoming_port);
	  if (incoming_ip && (*incoming_ip))
	    {
	      LW6SYS_FREE (*incoming_ip);
	    }
	  if (available_size > 0)
	    {
	      pos_lf = strchr (line_buf, CHAR_LF);
	      if (pos_lf)
		{
		  trail_size = (pos_lf > line_buf
				&& pos_lf[-1] == CHAR_CR) ? 2 : 1;
		  wanted_size = (pos_lf + 1 - line_buf);
		  pos_lf[1 - trail_size] = CHAR_0;
		  ret = lw6sys_str_copy (line_buf);
		  if (ret)
		    {
		      lw6sys_str_cleanup (ret);
		    }

		  // remove data from queue
		  lw6net_udp_recv (sock, line_buf, wanted_size,
				   incoming_ip, incoming_port);
		}
	    }
	  LW6SYS_FREE (line_buf);
	}
    }

  return ret;
}

/**
 * lw6net_send_line_udp:
 *
 * @sock: the socket descriptor
 * @line: the line to be sent, without the "\n" at the end
 * @ip: the IP address of the target
 * @port: the IP port of the target
 *
 * Sends a line terminated by LF ("\n", chr(10)) on a UDP
 * socket, that is, datagram oriented. The "\n" is automatically
 * added, do not bother sending it.
 *
 * Return value: the number of bytes sent, 0 if failure
 */
int
lw6net_send_line_udp (int sock, char *line, char *ip, int port)
{
  int ret = 0;
  int line_size = 0;
  float line_delay;
  int wanted_size = 0;
  char *trailed_line;

  if (sock >= 0 && line)
    {
      line_size = _lw6net_global_context->const_data.line_size;
      line_delay = _lw6net_global_context->const_data.line_delay;
      wanted_size = strlen (line);
      if (wanted_size > line_size)
	{
	  lw6sys_log (LW6SYS_LOG_WARNING,
		      _("stripping line \"%s\" of size %d, limit is %d"),
		      line, wanted_size, line_size);
	  wanted_size = line_size;
	}

      trailed_line = lw6sys_str_concat (line, trail);
      if (trailed_line)
	{
	  ret =
	    lw6net_udp_send (sock, trailed_line,
			     strlen (trailed_line), ip, port);
	  LW6SYS_FREE (trailed_line);
	}
    }

  return ret;
}
