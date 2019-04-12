/*
 * Geanyuniq - A geany plugin to delete duplicate lines.
 *
 * Copyright (C) 2012-2013 by Emil Brink <emil@obsession.se>.
 *
 * This file is part of Geanyuniq.
 *
 * Geanyuniq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Geanyuniq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Geanyuniq.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include <gdk/gdkkeysyms.h>

#include "geanyplugin.h"

#include "bloomfilter.h"

#define	MNEMONIC_NAME	"geanyuniq"

GeanyPlugin         *geany_plugin;
GeanyData           *geany_data;
GeanyFunctions      *geany_functions;

PLUGIN_VERSION_CHECK(147)

PLUGIN_SET_INFO("Geanyuniq",
		"A plugin to automatically delete duplicate lines in the entire document, named after the \"uniq\" shell command.",
		"2.1",
		"Emil Brink <emil@obsession.se>")

enum {
	KEY_GEANY_UNIQ_ADJACENT,
	KEY_GEANY_UNIQ_GLOBAL,
	NUM_KEYS
};

static struct {
	GtkWidget	*adjacent_item;
	GtkWidget	*global_item;

	GeanyKeyGroup	*key_group;
} geany_uniq;

/* -------------------------------------------------------------------------------------------------------------- */

/* A more sensical API for getting a aline of text from Scintilla. I don't understand
 * how returning a 0-length string for "end of buffer" makes sense to anyone. Not
 * const clean, since Scintilla isn't.
*/
static gchar * my_sci_get_line(ScintillaObject *sci, gint linenumber)
{
	gchar	*text = sci_get_line(sci, linenumber);

	if(text == NULL || *text == '\0')
	{
		g_free(text);
		return NULL;
	}
	return text;
}

/* -------------------------------------------------------------------------------------------------------------- */

/* Remove adjacent duplicate lines from the given position range. Positions are "rounded" to the lines they're in. */
static guint geany_uniq_adjacent_range(ScintillaObject *sci, gint pos_start, gint pos_end)
{
	const gint	line_start = sci_get_line_from_position(sci, pos_start);
	const gint	line_end = sci_get_line_from_position(sci, pos_end);
	gchar		*here = NULL;
	gint		line = line_start, global_line = line;
	guint		count = 0;
	GString		*prev;

	if((prev = g_string_sized_new(512)) == NULL)
		return 0;
	while(global_line <= line_end && (here = my_sci_get_line(sci, line)) != NULL)
	{
		const gboolean	skip = (prev->len == 0) || (strcmp(here, prev->str) != 0);

		if(skip)
		{
			g_string_assign(prev, here);
			line++;
		}
		else
		{
			const gint	pos = sci_get_position_from_line(sci, line);

			sci_set_selection_start(sci, pos);
			sci_set_selection_end(sci, pos + sci_get_line_length(sci, line));
			sci_replace_sel(sci, "");
			count++;
		}
		global_line++;
		g_free(here);
	}
	g_string_free(prev, TRUE);

	return count;
}

/* -------------------------------------------------------------------------------------------------------------- */

typedef struct {
	ScintillaObject	*sci;
	gint		global_line;
	gint		line_start, line_end;
} GlobalInfo;

static gboolean cb_contains(gpointer user, const gchar *string, gsize length)
{
	const GlobalInfo	*gi = user;
	gint			i;
	gboolean		ret = FALSE;

	for(i = gi->line_start; !ret && i < gi->line_end; i++)
	{
		gchar	*here = sci_get_line(gi->sci, i);
		if(i == gi->global_line)
			continue;
		ret = strcmp(here, string) == 0;
		g_free(here);
	}
	return ret;
}

/* Remove duplicate lines from the given position range. Positions are "rounded" to the lines they're in. */
static guint geany_uniq_global_range(ScintillaObject *sci, gint pos_start, gint pos_end)
{
	BloomFilter	*bf;
	GlobalInfo	gi;
	const gint	num_lines = sci_get_line_count(sci);
	gint		line;
	gchar		*here = NULL;
	guint		count = 0;

	if((bf = bloom_filter_new_with_probability(1e-4f, num_lines, cb_contains, &gi)) == NULL)
		return 0;

	gi.sci = sci;
	gi.line_start = sci_get_line_from_position(sci, pos_start);
	gi.line_end = sci_get_line_from_position(sci, pos_end);
	gi.global_line = line = gi.line_start;
	while(gi.global_line <= gi.line_end && (here = my_sci_get_line(sci, line)) != NULL)
	{
		const gssize	length = sci_get_line_length(sci, line);
		const gboolean	skip = bloom_filter_size(bf) == 0 || !bloom_filter_contains(bf, here, length);

		if(skip)
		{
			/* This line seems unique; remember it in the filter and move to the next one. */
			bloom_filter_insert(bf, here, length);
			line++;
			gi.global_line++;
		}
		else
		{	/* Line was not unique, delete it. */
			const gint	pos = sci_get_position_from_line(sci, line);

			sci_set_selection_start(sci, pos);
			sci_set_selection_end(sci, pos + sci_get_line_length(sci, line));
			sci_replace_sel(sci, "");
			count++;
		}
		g_free(here);
	}
	bloom_filter_destroy(bf);

	return count;
}

/* -------------------------------------------------------------------------------------------------------------- */

/* Remove duplicate lines, leaving the first line.
 * If global is true, globally duplicate lines are removed, else only adjacent lines are considered.
*/
static void run_geany_uniq(gboolean global)
{
	GeanyDocument	*doc;
	ScintillaObject	*sci;
	gint		pos_start, pos_end;
	guint		count;

	if((doc = document_get_current()) == NULL)
		return;
	if((sci = doc->editor->sci) == NULL)
		return;

	if(sci_has_selection(sci))
	{
		pos_start = sci_get_selection_start(sci);
		pos_end = sci_get_selection_end(sci);
	}
	else
	{
		pos_start = 0;
		pos_end = sci_get_length(sci);
	}

	sci_start_undo_action(sci);
	if(global)
		count = geany_uniq_global_range(sci, pos_start, pos_end);
	else
		count = geany_uniq_adjacent_range(sci, pos_start, pos_end);
	sci_end_undo_action(sci);

	if(count > 0)
		msgwin_status_add("Geanyuniq deleted %u duplicate lines.", count);
}

/* -------------------------------------------------------------------------------------------------------------- */

static void cb_menu_item_activated(GtkWidget *wid, gpointer user)
{
	run_geany_uniq(GPOINTER_TO_INT(user));
}

static gboolean cb_key_group_callback(guint key_id)
{
	switch(key_id)
	{
	case KEY_GEANY_UNIQ_ADJACENT:
		run_geany_uniq(FALSE);
		return TRUE;
	case KEY_GEANY_UNIQ_GLOBAL:
		run_geany_uniq(TRUE);
		break;
	}
	return FALSE;
}

/* -------------------------------------------------------------------------------------------------------------- */

void plugin_init(GeanyData *geany_data)
{
	/* This plugin implements a single command, coupled with a menu item. */
	geany_uniq.adjacent_item = ui_image_menu_item_new(NULL, _("Delete Adjacent Duplicate Lines"));
	g_signal_connect(G_OBJECT(geany_uniq.adjacent_item), "activate", G_CALLBACK(cb_menu_item_activated), GINT_TO_POINTER(FALSE));
	gtk_widget_show_all(geany_uniq.adjacent_item);
	gtk_container_add(GTK_CONTAINER(geany->main_widgets->tools_menu), geany_uniq.adjacent_item);

	geany_uniq.global_item = ui_image_menu_item_new(NULL, _("Delete All Duplicate Lines"));
	g_signal_connect(G_OBJECT(geany_uniq.global_item), "activate", G_CALLBACK(cb_menu_item_activated), GINT_TO_POINTER(TRUE));
	gtk_widget_show_all(geany_uniq.global_item);
	gtk_container_add(GTK_CONTAINER(geany->main_widgets->tools_menu), geany_uniq.global_item);

	geany_uniq.key_group = plugin_set_key_group(geany_plugin, MNEMONIC_NAME, NUM_KEYS, cb_key_group_callback);
	keybindings_set_item(geany_uniq.key_group, KEY_GEANY_UNIQ_ADJACENT, NULL, 0, 0, "geany-uniq-uniq", _("Delete Duplicate Lines"), geany_uniq.adjacent_item);
	keybindings_set_item(geany_uniq.key_group, KEY_GEANY_UNIQ_GLOBAL, NULL, 0, 0, "geany-uniq-global", _("Delete All Duplicate Lines"), geany_uniq.global_item);
}

void plugin_cleanup(void)
{
	gtk_widget_destroy(geany_uniq.adjacent_item);
	gtk_widget_destroy(geany_uniq.global_item);
}
