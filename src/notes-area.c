/*   Notes Area display of project notes for GTimeTracker
 *   Copyright (C) 2003 Linas Vepstas <linas@linas.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <glade/glade.h>
#include <gnome.h>

#include "proj.h"
#include "notes-area.h"
#include "util.h"

struct NotesArea_s
{
	GladeXML *gtxml;
	GtkPaned *vpane;   /* top level pane */
	GtkContainer *ctree_holder;   /* scrolled widget that holds ctree */

	GtkEntry *proj_title;
	GtkEntry *proj_desc;
	GtkTextView *proj_notes;
	
	GtkEntry *task_memo;
	GtkTextView *task_notes;

	GttProject *proj;
	gboolean ignore_events;
};

/* ============================================================== */

static void
task_memo_changed (GtkEntry *entry, NotesArea *na)
{
	GttTask *tsk;
	const char * str;
	if (NULL == na->proj) return;
	if (na->ignore_events) return;
	
	str = gtk_entry_get_text (entry);
	tsk = gtt_project_get_first_task (na->proj);
	gtt_task_set_memo (tsk, str);
}

/* ============================================================== */

static void
task_notes_changed (GtkTextBuffer *entry, NotesArea *na)
{
	GttTask *tsk;
	const char * str;
	if (NULL == na->proj) return;
	if (na->ignore_events) return;
	
	str = xxxgtk_textview_get_text (na->task_notes);
	tsk = gtt_project_get_first_task (na->proj);
	gtt_task_set_notes (tsk, str);
}

/* ============================================================== */

static void
proj_title_changed (GtkEntry *entry, NotesArea *na)
{
	const char * str;
	if (NULL == na->proj) return;
	if (na->ignore_events) return;
	
	str = gtk_entry_get_text (entry);
	gtt_project_set_title (na->proj, str);
}


/* ============================================================== */

static void
proj_desc_changed (GtkEntry *entry, NotesArea *na)
{
	const char * str;
	if (NULL == na->proj) return;
	if (na->ignore_events) return;
	
	str = gtk_entry_get_text (entry);
	gtt_project_set_desc (na->proj, str);
}


/* ============================================================== */

static void
proj_notes_changed (GtkTextBuffer *entry, NotesArea *na)
{
	const char * str;
	if (NULL == na->proj) return;
	if (na->ignore_events) return;
	
	str = xxxgtk_textview_get_text (na->proj_notes);
	gtt_project_set_notes (na->proj, str);
}

/* ============================================================== */

#define CONNECT_ENTRY(GLADE_NAME,CB)  ({                           \
	GtkEntry * entry;                                               \
	entry = GTK_ENTRY(glade_xml_get_widget (gtxml, GLADE_NAME));    \
	g_signal_connect (G_OBJECT (entry), "changed",                  \
	                G_CALLBACK (CB), dlg);                          \
   entry; })

#define CONNECT_TEXT(GLADE_NAME,CB)  ({                            \
	GtkTextView *tv;                                                \
	GtkTextBuffer *buff;                                            \
	tv = GTK_TEXT_VIEW(glade_xml_get_widget (gtxml, GLADE_NAME));   \
	buff = gtk_text_view_get_buffer (tv);                           \
	g_signal_connect (G_OBJECT (buff), "changed",                   \
	                G_CALLBACK (CB), dlg);                          \
   tv; })


NotesArea *
notes_area_new (void)
{
	NotesArea *dlg;
	GladeXML *gtxml;

	dlg = g_new0 (NotesArea, 1);

	gtxml = gtt_glade_xml_new ("glade/notes.glade", "top window");
	dlg->gtxml = gtxml;
	
	dlg->vpane = GTK_PANED(glade_xml_get_widget (gtxml, "notes vpane"));
	dlg->ctree_holder = GTK_CONTAINER(glade_xml_get_widget (gtxml, "ctree holder"));

	dlg->proj_title = CONNECT_ENTRY ("proj title entry", proj_title_changed);
	dlg->proj_desc = CONNECT_ENTRY ("proj desc entry", proj_desc_changed);
	dlg->task_memo = CONNECT_ENTRY ("diary entry", task_memo_changed);

	dlg->proj_notes = CONNECT_TEXT ("proj notes textview", proj_notes_changed);
	dlg->task_notes = CONNECT_TEXT ("diary notes textview", task_notes_changed);
	
	gtk_widget_show (GTK_WIDGET(dlg->vpane));

	dlg->proj = NULL;
	dlg->ignore_events = FALSE;

	return dlg;
}

/* ============================================================== */
/* This routine copies data from the data engine, and pushes it 
 * into the GUI.  
 */ 

static void
notes_area_do_set_project (NotesArea *na, GttProject *proj)
{
	const char * str;
	GttTask *tsk;
	
	if (!na) return;

	if (!proj) return; // xxx should clear fields instead

	/* Calling gtk_entry_set_text makes 'changed' events happen,
	 * which causes us to get the entry text, which exposes a gtk
	 * bug.  So we work around the bug and save cpu time by ignoring
	 * change events during a mass update. */
	na->ignore_events = TRUE;
	
	na->proj = proj;
	
	/* Fetch data from the data engine, stuff it into the GUI. */
	str = gtt_project_get_title (proj);
	gtk_entry_set_text (na->proj_title, str);
	
	str = gtt_project_get_desc (proj);
	gtk_entry_set_text (na->proj_desc, str);

	str = gtt_project_get_notes (proj);
	xxxgtk_textview_set_text (na->proj_notes, str);

	tsk = gtt_project_get_first_task (proj);
	str = gtt_task_get_memo (tsk);
	gtk_entry_set_text (na->task_memo, str);
	
	str = gtt_task_get_notes (tsk);
	xxxgtk_textview_set_text (na->task_notes, str);

	na->ignore_events = FALSE;
}

/* ============================================================== */

static void
redraw (GttProject *prj, gpointer data)
{
	NotesArea *na = data;
	notes_area_do_set_project (na, prj);
}

/* ============================================================== */

void
notes_area_set_project (NotesArea *na, GttProject *proj)
{
	gtt_project_remove_notifier (na->proj, redraw, na);
	notes_area_do_set_project (na, proj);
	gtt_project_add_notifier (proj, redraw, na);
}

/* ============================================================== */

GtkWidget *
notes_area_get_widget (NotesArea *nadlg)
{
	if (!nadlg) return NULL;
	return GTK_WIDGET(nadlg->vpane);
}

void 
notes_area_add_ctree (NotesArea *nadlg, GtkWidget *ctree)
{
	if (!nadlg) return;

	gtk_container_add (nadlg->ctree_holder, ctree);
	gtk_widget_show_all (GTK_WIDGET(nadlg->ctree_holder));
}

/* ========================= END OF FILE ======================== */
