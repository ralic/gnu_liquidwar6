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
#include <expat.h>

#include "cfg.h"
#include "cfg-internal.h"

char *
lw6cfg_xml_element (lw6hlp_type_t type)
{
  char *xml_type = "";

  switch (type)
    {
    case LW6HLP_TYPE_INT:
      xml_type = LW6CFG_XML_INT;
      break;
    case LW6HLP_TYPE_BOOL:
      xml_type = LW6CFG_XML_BOOL;
      break;
    case LW6HLP_TYPE_FLOAT:
      xml_type = LW6CFG_XML_FLOAT;
      break;
    case LW6HLP_TYPE_STR:
      xml_type = LW6CFG_XML_STRING;
      break;
    case LW6HLP_TYPE_COLOR:
      xml_type = LW6CFG_XML_COLOR;
      break;
    default:
      xml_type = "";
      break;
    }

  return xml_type;
}

void
lw6cfg_read_xml_int (char *xml_key, char *xml_value, char *target_key,
		     int *value)
{
  if (!strcasecmp (xml_key, target_key))
    {
      if (value)
	{
	  (*value) = lw6sys_atoi (xml_value);
	}
    }
}

void
lw6cfg_read_xml_bool (char *xml_key, char *xml_value, char *target_key,
		      int *value)
{
  if (!strcasecmp (xml_key, target_key))
    {
      if (value)
	{
	  (*value) = lw6sys_atoi (xml_value);
	  if (!(*value))
	    {
	      /*
	       * OK, trying to convert the value to a number gives 0,
	       * we just try if it could be a string like "true", "yes"
	       * on on.
	       * Note that prefered value is "true" 8-)
	       */
	      (*value) = lw6sys_atob (xml_value);
	    }
	}
    }
}

void
lw6cfg_read_xml_float (char *xml_key, char *xml_value, char *target_key,
		       float *value)
{
  if (!strcasecmp (xml_key, target_key))
    {
      if (value)
	{
	  (*value) = lw6sys_atof (xml_value);
	}
    }
}

void
lw6cfg_read_xml_string (char *xml_key, char *xml_value, char *target_key,
			char **value)
{
  if (!strcasecmp (xml_key, target_key))
    {
      if (value)
	{
	  if (*value)
	    {
	      /*
	       * This implies that *value is zeroed at initialisation,
	       * or filled with a dynamically allocated string.
	       */
	      LW6SYS_FREE (*value);
	      (*value) = NULL;
	    }
	  (*value) = lw6sys_str_copy (xml_value);
	}
    }
}

void
lw6cfg_read_xml_color (char *xml_key, char *xml_value,
		       char *target_key, lw6sys_color_8_t * value)
{
  if (!strcasecmp (xml_key, target_key))
    {
      (*value) = lw6sys_color_a_to_8 (xml_value);
    }
}

static void XMLCALL
element_start (void *data, const char *element, const char **attr)
{
  _lw6cfg_xml_callback_t *callback;
  char *key = NULL;
  char *value = NULL;
  char **at;

  callback = (_lw6cfg_xml_callback_t *) data;

  if (callback && callback->func)
    {
      for (at = (char **) attr; (*at) != NULL; at += 2)
	{
	  if (strcasecmp (LW6CFG_XML_KEY, (*at)) == 0)
	    {
	      key = *(at + 1);
	    }
	  if (strcasecmp (LW6CFG_XML_VALUE, (*at)) == 0)
	    {
	      value = *(at + 1);
	    }
	}
      if (key && value)
	{
	  callback->func (callback->data, (char *) element, key, value);
	}
    }
  else
    {
      lw6sys_log (LW6SYS_LOG_WARNING, _("XML callback function is NULL"));
    }
}

static void XMLCALL
element_end (void *data, const char *el)
{

}

int
lw6cfg_read_key_value_xml_file (char *filename,
				void (*callback_func) (void *callback_data,
						       char *element,
						       char *key,
						       char *value),
				void *callback_data)
{
  int ret = 0;
  char *content = NULL;
  _lw6cfg_xml_callback_t *callback;

  callback = LW6SYS_MALLOC (sizeof (_lw6cfg_xml_callback_t));
  if (callback)
    {
      callback->func = callback_func;
      callback->data = callback_data;

      content = lw6sys_read_file_content (filename);
      if (content)
	{
	  int length = strlen (content);
	  XML_Parser parser;

	  parser = XML_ParserCreate (NULL);
	  if (parser)
	    {
	      XML_SetElementHandler (parser, element_start, element_end);
	      XML_SetUserData (parser, (void *) callback);

	      if (XML_Parse (parser, content, length, 1) != XML_STATUS_ERROR)
		{
		  ret = 1;
		}
	      else
		{
#ifdef LW6_MS_WINDOWS
		  /*
		   * For some reason there's an error ad end-of-file because
		   * of a ^M after </liquidwar6>. This message is only
		   * INFO and not warning on MS-Windows, this avoids
		   * stupid warning at startup.
		   */
		  lw6sys_log (LW6SYS_LOG_INFO,
			      _
			      ("parse error reading XML file \"%s\" at line %d: \"%s\""),
			      filename,
			      XML_GetCurrentLineNumber (parser),
			      XML_ErrorString (XML_GetErrorCode (parser)));
#else
		  lw6sys_log (LW6SYS_LOG_WARNING,
			      _
			      ("parse error reading XML file \"%s\" at line %d: \"%s\""),
			      filename,
			      XML_GetCurrentLineNumber (parser),
			      XML_ErrorString (XML_GetErrorCode (parser)));
#endif
		}
	      XML_ParserFree (parser);
	    }
	  LW6SYS_FREE (content);
	}
      LW6SYS_FREE (callback);
    }

  return ret;
}

// if type is NULL, will be guessed automatically
static void
write_xml (FILE * f, char *type, char *key, char *value)
{
  char *tmp = NULL;
  char *pos = NULL;
  char *guessed_type = NULL;
  char *help_string = NULL;
  char *hlp_about = NULL;
  lw6hlp_type_t hlp_type = LW6HLP_TYPE_STR;
  char *hlp_default_value = NULL;
  int hlp_min_value = 0;
  int hlp_max_value = 0;

  if (lw6cfg_must_be_saved (key))
    {
      hlp_about =
	lw6hlp_about (&hlp_type, &hlp_default_value, &hlp_min_value,
		      &hlp_max_value, key);
      if (hlp_about)
	{
	  if (hlp_default_value)
	    {
	      if (hlp_min_value != 0 || hlp_max_value != 0)
		{
		  tmp =
		    lw6sys_new_sprintf
		    ("\"%s\" (default=\"%s\", min=\"%d\", max=\"%d\"): %s",
		     key, hlp_default_value, hlp_min_value, hlp_max_value,
		     hlp_about);
		}
	      else
		{
		  tmp =
		    lw6sys_new_sprintf ("\"%s\" (default=\"%s\"): %s", key,
					hlp_default_value, hlp_about);
		}
	    }
	  else
	    {
	      if (hlp_min_value != 0 || hlp_max_value != 0)
		{
		  tmp =
		    lw6sys_new_sprintf ("\"%s\" (min=\"%d\", max=\"%d\"): %s",
					key, hlp_min_value, hlp_max_value,
					hlp_about);
		}
	      else
		{
		  tmp = lw6sys_new_sprintf ("\"%s\": %s", key, hlp_about);
		}
	    }
	}
      if (tmp)
	{
	  help_string =
	    lw6sys_str_reformat (tmp, LW6SYS_REFORMAT_XML_PREFIX,
				 LW6SYS_REFORMAT_XML_NB_COLUMNS);
	  LW6SYS_FREE (tmp);
	}
      switch (hlp_type)
	{
	case LW6HLP_TYPE_STR:
	  guessed_type = LW6CFG_XML_STRING;
	  break;
	case LW6HLP_TYPE_INT:
	  guessed_type = LW6CFG_XML_INT;
	  break;
	case LW6HLP_TYPE_BOOL:
	  guessed_type = LW6CFG_XML_BOOL;
	  break;
	case LW6HLP_TYPE_FLOAT:
	  guessed_type = LW6CFG_XML_FLOAT;
	  break;
	case LW6HLP_TYPE_COLOR:
	  guessed_type = LW6CFG_XML_COLOR;
	  break;
	default:
	  guessed_type = NULL;	// LW6HLP_TYPE_VOID
	}
      if (guessed_type)
	{
	  if (help_string)
	    {
	      fprintf (f, "  <!--%s", lw6sys_eol ());
	      fprintf (f, "%s", help_string);
	      fprintf (f, "  -->%s", lw6sys_eol ());
	    }
	  if (type && strcasecmp (type, guessed_type))
	    {
	      lw6sys_log (LW6SYS_LOG_WARNING,
			  ("inconsistency in XML entry type (\"%s\": \"%s!=%s\""),
			  key, type, guessed_type);
	    }
	  if (!type)
	    {
	      type = guessed_type;
	    }
	  fprintf (f, "  <%s %s=\"%s\" %s=\"", type, LW6CFG_XML_KEY,
		   key, LW6CFG_XML_VALUE);
	  for (pos = value; (*pos) != '\0'; ++pos)
	    {
	      switch (*pos)
		{
		case '<':
		  fprintf (f, "&lt;");
		  break;
		case '>':
		  fprintf (f, "&gt;");
		  break;
		case '&':
		  fprintf (f, "&amp;");
		  break;
		case '"':
		  fprintf (f, "&quot;");
		  break;
		case '\'':
		  fprintf (f, "&apos;");
		  break;
		default:
		  fprintf (f, "%c", (*pos));
		}
	    }
	  fprintf (f, "\"/>%s", lw6sys_eol ());
	}
      if (help_string)
	{
	  LW6SYS_FREE (help_string);
	}
    }
}

void
lw6cfg_write_xml_int (FILE * f, char *key, int value)
{
  char *str_value = NULL;

  str_value = lw6sys_itoa (value);
  if (str_value)
    {
      write_xml (f, LW6CFG_XML_INT, key, str_value);
      LW6SYS_FREE (str_value);
    }
}

void
lw6cfg_write_xml_bool (FILE * f, char *key, int value)
{
  char *str_value = NULL;

  str_value = lw6sys_btoa (value);
  if (str_value)
    {
      write_xml (f, LW6CFG_XML_BOOL, key, str_value);
      LW6SYS_FREE (str_value);
    }
}

void
lw6cfg_write_xml_float (FILE * f, char *key, float value)
{
  char *str_value = NULL;

  str_value = lw6sys_ftoa (value);
  if (str_value)
    {
      write_xml (f, LW6CFG_XML_FLOAT, key, str_value);
      LW6SYS_FREE (str_value);
    }
}

void
lw6cfg_write_xml_string (FILE * f, char *key, char *value)
{
  write_xml (f, LW6CFG_XML_STRING, key, value);
}

void
lw6cfg_write_xml_color (FILE * f, char *key, lw6sys_color_8_t value)
{
  char *str_value = NULL;

  str_value = lw6sys_color_8_to_a (value);
  if (str_value)
    {
      write_xml (f, LW6CFG_XML_COLOR, key, str_value);
      LW6SYS_FREE (str_value);
    }
}

void
lw6cfg_write_xml_guess_type (FILE * f, char *key, char *value)
{
  write_xml (f, NULL, key, value);
}
