/*
 * Copyright (C) 1997-1998 Stuart Parmenter and Elliot Lee
 * All rights reserved.
 *
 * This file is part of the Gnome Library.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
  @NOTATION@
 */

#include "config.h"

#include "libgnome.h"
#include "gnome-sound.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <glib.h>

#ifdef HAVE_CANBERRA
#include <canberra.h>
#endif

G_GNUC_INTERNAL void _gnome_sound_set_enabled (gboolean);

#ifdef HAVE_CANBERRA

static gboolean gnome_sound_enabled = TRUE;
static ca_context *global_context = NULL;

static ca_context *
get_ca_context (const char *hostname)
{
  int rv;

  if (global_context != NULL)
    return global_context;

  if ((rv = ca_context_create (&global_context)) != CA_SUCCESS)
    {
      g_warning ("Failed to create canberra context: %s\n", ca_strerror (rv));
      global_context = NULL;
      return NULL;
    }

  if (hostname != NULL)
    {
      ca_context_change_props (global_context,
                               CA_PROP_APPLICATION_PROCESS_HOST, hostname,
                               NULL);
    }

  return global_context;
}

#endif

void G_GNUC_INTERNAL
_gnome_sound_set_enabled (gboolean enabled)
{
#ifdef HAVE_CANBERRA
  gnome_sound_enabled = enabled;
#endif
}

/**
 * gnome_sound_sample_load:
 * @sample_name: The name of the sample.
 * @filename: The filename where the audio is stored.
 *
 * Loads the audio from @filename and load it into the canberra cache for later
 * playing. Programs will rarely want to call this function directly. Use
 * gnome_sound_play() instead for fire and forget sound playing.
 *
 * Returns: -1 or -2
 *
 * @Deprecated: 2.30: Use ca_context_cache() or ca_context_cache_full() instead
 */
int
gnome_sound_sample_load(const char *sample_name, const char *filename)
{
#ifdef HAVE_CANBERRA
  ca_context *context;
  int rv;

  g_return_val_if_fail (sample_name != NULL, -2);

  if (!gnome_sound_enabled)
    return -2;

  if(!filename || !*filename)
    return -2;

  if ((context = get_ca_context (NULL)) == NULL)
    return -1;

  rv = ca_context_cache (context,
                         CA_PROP_MEDIA_NAME, sample_name,
                         CA_PROP_MEDIA_FILENAME, filename,
                         NULL);
  

  if (rv != CA_SUCCESS)
    g_warning ("Failed to cache sample '%s' from '%s': %s\n",
               sample_name, filename, ca_strerror (rv));
                    
  return -1;
#else
  return -1;
#endif
}

/**
 * gnome_sound_play:
 * @filename: File containing the sound sample.
 *
 * Plays the audio stored in @filename, if possible. Fail quietly if playing is
 * not possible (due to missing sound support or for other reasons).
 *
 * @Deprecated: 2.30: Use ca_context_play(), ca_gtk_play_for_widget() or ca_gtk_play_for_event() instead
 */
void
gnome_sound_play (const char * filename)
{
#ifdef HAVE_CANBERRA
  ca_context *context;
  int rv;

  if (!gnome_sound_enabled)
    return;

  if(!filename || !*filename)
    return;

  if ((context = get_ca_context (NULL)) == NULL)
    return;

  rv = ca_context_play (context, 0,
                        CA_PROP_MEDIA_FILENAME, filename,
                        NULL);

  if (rv != CA_SUCCESS)
    g_warning ("Failed to play file '%s': %s\n",
               filename, ca_strerror (rv));
#endif
}

/**
 * gnome_sound_init:
 * @hostname: Hostname where esd daemon resides.
 *
 * Initialize the esd connection.
 *
 * @Deprecated: 2.30
 */
void
gnome_sound_init(const char *hostname)
{
#ifdef HAVE_CANBERRA
  get_ca_context (hostname);
#endif
}

/**
 * gnome_sound_shutdown:
 *
 * Shuts down the gnome sound support.
 *
 * @Deprecated: 2.30
 */
void
gnome_sound_shutdown(void)
{
#ifdef HAVE_ESD
  if (global_context != NULL)
    {
      ca_context_destroy (global_context);
      global_context = NULL;
    }
#endif
}

/**
 * gnome_sound_connection_get:
 *
 * Returns: -1
 *
 * @Deprecated: 2.30
 **/
int
gnome_sound_connection_get (void)
{
	return -1;
}
