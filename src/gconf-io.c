/*   GConf2 input/output handling for GTimeTracker - a time tracker
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

#include <gconf/gconf-client.h>
#include <glib.h>

#include "app.h"
#include "cur-proj.h"
#include "gconf-io.h"
#include "plug-in.h"
#include "prefs.h"
#include "timer.h"

extern int save_count; /* XXX */

#define GTT_GCONF "/apps/GnoTimeDebug"

/* ======================================================= */
/* XXX Should use changesets */
/* XXX warnings should be graphical */
#define CHKERR(rc,err_ret,dir) {                                       \
   if (FALSE == rc) {                                                  \
      printf ("GTT: GConf: Warning: set %s failed: ", dir);            \
      if (err_ret) printf ("%s", err_ret->message);                    \
      printf ("\n");                                                   \
   }                                                                   \
}

#define SETBOOL(dir,val) {                                             \
   gboolean rc;                                                        \
   GError *err_ret= NULL;                                              \
                                                                       \
   rc = gconf_client_set_bool (client, GTT_GCONF dir, val, &err_ret);  \
   CHKERR (rc,err_ret,dir);                                            \
}

#define SETINT(dir,val) {                                              \
   gboolean rc;                                                        \
   GError *err_ret= NULL;                                              \
                                                                       \
   rc = gconf_client_set_int (client, GTT_GCONF dir, val, &err_ret);   \
   CHKERR (rc,err_ret,dir);                                            \
}

#define SETSTR(dir,val) {                                              \
   gboolean rc;                                                        \
   GError *err_ret= NULL;                                              \
                                                                       \
   rc = gconf_client_set_string (client, GTT_GCONF dir, val, &err_ret); \
   CHKERR (rc,err_ret,dir);                                            \
}

#define UNSET(dir) {                                                   \
   gboolean rc;                                                        \
   GError *err_ret= NULL;                                              \
                                                                       \
   rc = gconf_client_unset (client, GTT_GCONF dir, &err_ret);          \
   CHKERR (rc,err_ret,dir);                                            \
}

/* ======================================================= */

#define CHKGET(gcv,err_ret,dir,default_val)                            \
	if ((NULL == gcv) || (FALSE == GCONF_VALUE_TYPE_VALID(gcv->type))) {\
	   retval = default_val;                                            \
      printf ("GTT: GConf: Warning: get %s failed: ", dir);            \
      if (err_ret) printf ("%s\n\t", err_ret->message);                \
      printf ("Using default value\n");                                \
   }

#define GETBOOL(dir,default_val) ({                                    \
   gboolean retval;                                                    \
   GError *err_ret= NULL;                                              \
	GConfValue *gcv;                                                    \
	gcv = gconf_client_get (client, GTT_GCONF dir, &err_ret);           \
	CHKGET (gcv,err_ret, dir, default_val)                              \
	else retval = gconf_value_get_bool (gcv);                           \
	retval;                                                             \
})

#define F_GETINT(dir,default_val) ({                                   \
   int retval;                                                         \
   GError *err_ret= NULL;                                              \
	GConfValue *gcv;                                                    \
	gcv = gconf_client_get (client, dir, &err_ret);                     \
	CHKGET (gcv,err_ret, dir, default_val)                              \
	else retval = gconf_value_get_int (gcv);                            \
	retval;                                                             \
})

#define GETINT(dir,default_val) F_GETINT (GTT_GCONF dir, default_val)

#define F_GETSTR(dir,default_val) ({                                   \
   const char *retval;                                                 \
   GError *err_ret= NULL;                                              \
	GConfValue *gcv;                                                    \
	gcv = gconf_client_get (client, dir, &err_ret);                     \
	CHKGET (gcv,err_ret, dir, default_val)                              \
	else retval = gconf_value_get_string (gcv);                         \
	retval;                                                             \
})

#define GETSTR(dir,default_val) F_GETSTR (GTT_GCONF dir, default_val)

/* ======================================================= */
/* Save only the GUI configuration info, not the actual data */
/* XXX this should really use GConfChangeSet */

void
gtt_gconf_save (void)
{
	GList *node;
	char s[120];
	int i;
	int x, y, w, h;
	const char *xpn;
	
	GConfClient *client;

	client = gconf_client_get_default ();

	/* ------------- */
	/* save the window location and size */
	gdk_window_get_origin(app_window->window, &x, &y);
	gdk_window_get_size(app_window->window, &w, &h);
	SETINT ("/Geometry/Width", w);
	SETINT ("/Geometry/Height", h);
	SETINT ("/Geometry/X", x);
	SETINT ("/Geometry/Y", y);

	{
		int vp, hp;
		notes_area_get_pane_sizes (global_na, &vp, &hp);
		SETINT ("/Geometry/VPaned", vp);
		SETINT ("/Geometry/HPaned", hp);
	}
	/* ------------- */
	/* save the configure dialog values */
	SETBOOL ("/Display/ShowSecs", config_show_secs);
	SETBOOL ("/Display/ShowStatusbar", config_show_statusbar);
	SETBOOL ("/Display/ShowSubProjects", config_show_subprojects);
	SETBOOL ("/Display/ShowTableHeader", config_show_clist_titles);
	SETBOOL ("/Display/ShowTimeCurrent", config_show_title_current);
	SETBOOL ("/Display/ShowTimeDay", config_show_title_day);
	SETBOOL ("/Display/ShowTimeYesterday", config_show_title_yesterday);
	SETBOOL ("/Display/ShowTimeWeek", config_show_title_week);
	SETBOOL ("/Display/ShowTimeLastWeek", config_show_title_lastweek);
	SETBOOL ("/Display/ShowTimeMonth", config_show_title_month);
	SETBOOL ("/Display/ShowTimeYear", config_show_title_year);
	SETBOOL ("/Display/ShowTimeEver", config_show_title_ever);
	SETBOOL ("/Display/ShowDesc", config_show_title_desc);
	SETBOOL ("/Display/ShowTask", config_show_title_task);
	SETBOOL ("/Display/ShowEstimatedStart", config_show_title_estimated_start);
	SETBOOL ("/Display/ShowEstimatedEnd", config_show_title_estimated_end);
	SETBOOL ("/Display/ShowDueDate", config_show_title_due_date);
	SETBOOL ("/Display/ShowSizing", config_show_title_sizing);
	SETBOOL ("/Display/ShowPercentComplete", config_show_title_percent_complete);
	SETBOOL ("/Display/ShowUrgency", config_show_title_urgency);
	SETBOOL ("/Display/ShowImportance", config_show_title_importance);
	SETBOOL ("/Display/ShowStatus", config_show_title_status);

	xpn = ctree_get_expander_state (global_ptw);
	SETSTR ("/Display/ExpanderState", xpn);

	/* ------------- */
	SETBOOL ("/Toolbar/ShowIcons", config_show_tb_icons);
	SETBOOL ("/Toolbar/ShowTexts", config_show_tb_texts);
	SETBOOL ("/Toolbar/ShowTips", config_show_tb_tips);
	SETBOOL ("/Toolbar/ShowNew", config_show_tb_new);
	SETBOOL ("/Toolbar/ShowCCP", config_show_tb_ccp);
	SETBOOL ("/Toolbar/ShowJournal", config_show_tb_journal);
	SETBOOL ("/Toolbar/ShowProp", config_show_tb_prop);
	SETBOOL ("/Toolbar/ShowTimer", config_show_tb_timer);
	SETBOOL ("/Toolbar/ShowPref", config_show_tb_pref);
	SETBOOL ("/Toolbar/ShowHelp", config_show_tb_help);
	SETBOOL ("/Toolbar/ShowExit", config_show_tb_exit);

	/* ------------- */
	if (config_shell_start) {
		SETSTR ("/Actions/StartCommand", config_shell_start);
	} else {
		UNSET ("/Actions/StartCommand");
	}

	if (config_shell_stop) {
		SETSTR ("/Actions/StopCommand", config_shell_stop);
	} else {
		UNSET ("/Actions/StopCommand");
	}

	/* ------------- */
	SETBOOL ("/LogFile/Use", config_logfile_use);
	if (config_logfile_name) {
		SETSTR ("/LogFile/Filename", config_logfile_name);
	} else {
		UNSET ("/LogFile/Filename");
	}
		
	if (config_logfile_start) {
		SETSTR ("/LogFile/Entry", config_logfile_start);
	} else {
		SETSTR ("/LogFile/Entry", "");
	}
		
	if (config_logfile_stop) {
		SETSTR ("/LogFile/EntryStop", config_logfile_stop);
	} else {
		SETSTR ("/LogFile/EntryStop", "");
	}

	SETINT ("/LogFile/MinSecs", config_logfile_min_secs);

	/* ------------- */
	SETSTR ("/Data/URL", config_data_url);
	SETINT ("/Data/SaveCount", save_count);

	/* ------------- */
	w = 0;
	for (i = 0; -1< w; i++) 
	{
	   gboolean rc;
		GError *err_ret= NULL;

		g_snprintf(s, sizeof (s), GTT_GCONF"/CList/ColumnWidth%d", i);
		w = ctree_get_col_width (global_ptw, i);
		if (0 > w) break;
		rc = gconf_client_set_int(client, s, w, &err_ret);
		CHKERR(rc,err_ret,s);
	}

	/* ------------- */
	g_snprintf(s, sizeof (s), "%ld", time(0));
	SETSTR ("/Misc/LastTimer", s);
	SETINT ("/Misc/IdleTimeout", config_idle_timeout);
	SETINT ("/Misc/AutosavePeriod", config_autosave_period);
	SETINT ("/Misc/TimerRunning", timer_is_running());
	SETINT ("/Misc/CurrProject", gtt_project_get_id (cur_proj));
	SETINT ("/Misc/NumProjects", -1);

	/* Write out the customer report info */
	i = 0;
	for (node = gtt_plugin_get_list(); node; node=node->next)
	{
	   gboolean rc;
		GError *err_ret= NULL;

		GttPlugin *plg = node->data;
	   g_snprintf(s, sizeof (s), GTT_GCONF"/Reports/%d/Name", i);
		rc = gconf_client_set_string(client, s, plg->name, &err_ret);
		CHKERR (rc,err_ret,s);

	   g_snprintf(s, sizeof (s), GTT_GCONF"/Reports/%d/Path", i);
		rc = gconf_client_set_string(client, s, plg->path, &err_ret);
		CHKERR (rc,err_ret,s);

	   g_snprintf(s, sizeof (s), GTT_GCONF"/Reports/%d/Tooltip", i);
		rc = gconf_client_set_string(client, s, plg->tooltip, &err_ret);
		CHKERR (rc,err_ret,s);

		i++;
	}
	SETINT ("/Misc/NumReports", i);

   /* Sync to file.
	 * XXX if this fails, the error is serious, and tehre should be a 
	 * graphical popup.
	 */
   {
		GError *err_ret= NULL;
		gconf_client_suggest_sync (client, &err_ret);
		if (NULL != err_ret)
		{
			printf ("GTT: GConf: Sync Failed\n");
		}
	}
}

/* ======================================================= */

gboolean
gtt_gconf_exists (void)
{
	gboolean rc;
	GConfClient *client;
	GError *err_ret= NULL;

	client = gconf_client_get_default ();

	rc = gconf_client_dir_exists (client, GTT_GCONF, &err_ret);
	if (err_ret) printf ("duude err %s\n", err_ret->message);

	return rc;
}
	
void
gtt_gconf_load (void)
{
	char s[256];
	int i, num;
	int _n, _c, _j, _p, _t, _o, _h, _e;
	gboolean rc;
	GConfClient *client;
	GError *err_ret= NULL;

	client = gconf_client_get_default ();

	rc = gconf_client_dir_exists (client, GTT_GCONF, &err_ret);
	if (err_ret) printf ("duude err %s\n", err_ret->message);

#if 0
	/* If already running, and we are over-loading a new file,
	 * then save the currently running project, and try to set it
	 * running again ... */
	if (gtt_project_get_title(cur_proj) && (!first_proj_title)) 
	{
		/* We need to strdup because title is freed when 
		 * the project list is destroyed ... */
		first_proj_title = g_strdup (gtt_project_get_title (cur_proj));
	}

	_n = config_show_tb_new;
	_c = config_show_tb_ccp;
	_j = config_show_tb_journal;
	_p = config_show_tb_prop;
	_t = config_show_tb_timer;
	_o = config_show_tb_pref;
	_h = config_show_tb_help;
	_e = config_show_tb_exit;

	/* Get last running project */
   cur_proj_id = GETINT("/Misc/CurrProject", -1);

   config_idle_timeout = GETINT ("/Misc/IdleTimeout", 300);
   config_autosave_period = GETINT ("/Misc/AutosavePeriod", 60);

	/* Reset the main window width and height to the values 
	 * last stored in the config file.  Note that if the user 
	 * specified command-line flags, then the command line 
	 * over-rides the config file. */
	if (!geom_place_override) 
	{
		int x, y;
		x = GETINT ("/Geometry/X", 10);
		y = GETINT ("/Geometry/Y", 10);
		gtk_widget_set_uposition(GTK_WIDGET(app_window), x, y);
	}
	if (!geom_size_override)
	{
		int w, h;
		w = GETINT ("/Geometry/Width", 442);
		h = GETINT ("/Geometry/Height", 272);

		gtk_window_set_default_size(GTK_WINDOW(app_window), w, h);
	}

	{
		int vp, hp;
		vp = GETINT ("/Geometry/VPaned", 250);
		hp = GETINT ("/Geometry/HPaned", 220);
		notes_area_set_pane_sizes (global_na, vp, hp);
	}

	config_show_secs            = GETBOOL ("/Display/ShowSecs", FALSE);
	config_show_clist_titles    = GETBOOL ("/Display/ShowTableHeader", FALSE);
	config_show_subprojects     = GETBOOL ("/Display/ShowSubProjects", TRUE);
	config_show_statusbar       = GETBOOL ("/Display/ShowStatusbar", TRUE);

	config_show_title_ever      = GETBOOL ("/Display/ShowTimeEver", TRUE);
	config_show_title_day       = GETBOOL ("/Display/ShowTimeDay", TRUE);
	config_show_title_yesterday = GETBOOL ("/Display/ShowTimeYesterday", FALSE);
	config_show_title_week      = GETBOOL ("/Display/ShowTimeWeek", FALSE);
	config_show_title_lastweek  = GETBOOL ("/Display/ShowTimeLastWeek", FALSE);
	config_show_title_month     = GETBOOL ("/Display/ShowTimeMonth", FALSE);
	config_show_title_year      = GETBOOL ("/Display/ShowTimeYear", FALSE);
	config_show_title_current   = GETBOOL ("/Display/ShowTimeCurrent", FALSE);
	config_show_title_desc      = GETBOOL ("/Display/ShowDesc", TRUE);
	config_show_title_task      = GETBOOL ("/Display/ShowTask", TRUE);
	config_show_title_estimated_start = GETBOOL ("/Display/ShowEstimatedStart", FALSE);
	config_show_title_estimated_end = GETBOOL ("/Display/ShowEstimatedEnd", FALSE);
	config_show_title_due_date = GETBOOL ("/Display/ShowDueDate", FALSE);
	config_show_title_sizing   = GETBOOL ("/Display/ShowSizing", FALSE);
	config_show_title_percent_complete = GETBOOL ("/Display/ShowPercentComplete", FALSE);
	config_show_title_urgency    = GETBOOL ("/Display/ShowUrgency", TRUE);
	config_show_title_importance = GETBOOL ("/Display/ShowImportance", TRUE);
	config_show_title_status     = GETBOOL ("/Display/ShowStatus", FALSE);
	ctree_update_column_visibility (global_ptw);


	/* ------------ */
	config_show_tb_icons   = GETBOOL ("/Toolbar/ShowIcons", TRUE);
	config_show_tb_texts   = GETBOOL ("/Toolbar/ShowTexts", TRUE);
	config_show_tb_tips    = GETBOOL ("/Toolbar/ShowTips", TRUE);
	config_show_tb_new     = GETBOOL ("/Toolbar/ShowNew", TRUE);
	config_show_tb_ccp     = GETBOOL ("/Toolbar/ShowCCP", FALSE);
	config_show_tb_journal = GETBOOL ("/Toolbar/ShowJournal", TRUE);
	config_show_tb_prop    = GETBOOL ("/Toolbar/ShowProp", TRUE);
	config_show_tb_timer   = GETBOOL ("/Toolbar/ShowTimer", TRUE);
	config_show_tb_pref    = GETBOOL ("/Toolbar/ShowPref", FALSE);
	config_show_tb_help    = GETBOOL ("/Toolbar/ShowHelp", TRUE);
	config_show_tb_exit    = GETBOOL ("/Toolbar/ShowExit", TRUE);

	/* ------------ */
	config_shell_start = GETSTR ("/Actions/StartCommand", 
	      "echo start id=%D \\\"%t\\\"-\\\"%d\\\" %T  %H-%M-%S hours=%h min=%m secs=%s");
	config_shell_stop = GETSTR ("/Actions/StopCommand", "
	      "echo stop id=%D \\\"%t\\\"-\\\"%d\\\" %T  %H-%M-%S hours=%h min=%m secs=%s");

	/* ------------ */
	config_logfile_use   = GETBOOL ("/LogFile/Use", FALSE);
	config_logfile_name  = GETSTR ("/LogFile/Filename", NULL);
	config_logfile_start = GETSTR ("/LogFile/Entry", 
	    g_strdup(_("project %t started"))); 
	config_logfile_stop = GETSTR ("/LogFile/EntryStop", 
		 g_strdup(_("stopped project %t")));
	config_logfile_min_secs = GETINT ("/LogFile/MinSecs", 3);

	/* ------------ */
	save_count = GETINT ("/Data/SaveCount", 0);
	config_data_url = GETSTR ("/Data/URL", XML_DATA_FILENAME);

	/* ------------ */
	num = 0;
	for (i = 0; -1 < num; i++) 
	{
		g_snprintf(s, sizeof (s), GTT_GCONF"/CList/ColumnWidth%d", i);
		num = F_GETINT (s, -1);
		if (-1 < num)
		{
			ctree_set_col_width (global_ptw, i, num);
		}
	}

	/* Read in the user-defined report locations */
	num = GETINT ("/Misc/NumReports", 0);
	if (0 < num)
	{
		for (i = num-1; i >= 0 ; i--) 
		{
			GttPlugin *plg;
			const char * name, *path, *tip;

			g_snprintf(s, sizeof (s), GTT_CONF"/Report/%d/Name", i);
			name = F_GETSTR (s, "");
			g_snprintf(s, sizeof (s), GTT_CONF"/Report/%d/Path", i);
			path = F_GETSTR(s, "");
			g_snprintf(s, sizeof (s), GTT_CONF"/Report/%d/Tooltip", i);
			tip = F_GETSTR(s, "");
			plg = gtt_plugin_new (name, path);
			plg->tooltip = g_strdup (tip);
		}
	} 

	run_timer = GETINT ("/Misc/TimerRunning", 0);
	last_timer = (unsigned long) GETINT ("/Misc/LastTimer", -1);

	/* redraw the GUI */
	if (config_show_statusbar)
	{
		gtk_widget_show(status_bar);
	}
	else
	{
		gtk_widget_hide(status_bar);
	}

	update_status_bar();
	if ((_n != config_show_tb_new) ||
	    (_c != config_show_tb_ccp) ||
	    (_j != config_show_tb_journal) ||
	    (_p != config_show_tb_prop) ||
	    (_t != config_show_tb_timer) ||
	    (_o != config_show_tb_pref) ||
	    (_h != config_show_tb_help) ||
	    (_e != config_show_tb_exit)) 
	{
		update_toolbar_sections();
	}
#endif
}

/* =========================== END OF FILE ========================= */
