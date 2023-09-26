#include "ku_connector_wayland.c"
#include "ku_gen_css.c"
#include "ku_gen_html.c"
#include "ku_gen_type.c"
#include "ku_renderer_soft.c"
#include "ku_window.c"
#include "mt_map.c"
#include "mt_path.c"
#include "mt_string.c"
#include "vh_button.c"
#include "vh_slider.c"

char*                command;
struct monitor_info* monitor;
wl_window_t*         wlwindow;
int                  shown = 0;
int                  drawn = 0;
uint32_t             color = 0xFF00FFFF;

ku_window_t* kuwindow  = NULL;
ku_rect_t    dirtyrect = {0};

char* img_path;
char* css_path;
char* html_path;

void init(wl_event_t ev)
{
    mt_log_debug("INIT, EVENT : %i", ev.id);

    monitor = ev.monitors[0];

    mt_log_debug("NAME %s", monitor->name);
}

int button_event(vh_button_event_t ev)
{
    printf("event button div %s state %i\n", ev.view->id, ev.vh->state);
    fflush(stdout);

    return 0;
}
void slider_event(vh_slider_event_t ev)
{
    printf("event slider div %s ratio %.6f normalized %i\n", ev.view->id, ev.ratio, (int) (ev.ratio * 100.0));
    fflush(stdout);
}

// create layer id layer0 width 500 height 300 color FF0000FF margin 100 anchor lt
// load html src examples/html/main.html target layer0
// set ratio element volslider value 0.5
// set state element wifibtn value on

void update(ku_event_t ev)
{
    if (ev.type == KU_EVENT_STDIN)
    {
	if (ev.text[0] == '\n')
	{
	    if (strlen(command) > 0)
	    {
		mt_vector_t* toks  = mt_string_tokenize(command, " ");
		mt_map_t*    pairs = mt_map_new();
		for (size_t index = 0; index < toks->length; index += 1)
		{
		    if (index % 2 == 1)
		    {
			MPUT(pairs, toks->data[index - 1], toks->data[index]);
		    }
		}
		REL(toks);

		if (mt_map_get(pairs, "create") != NULL)
		{
		    char* type = MGET(pairs, "create");

		    if (strcmp(type, "layer") == 0)
		    {
			int   width  = 500;
			int   height = 300;
			int   margin = 0;
			char* anchor = "";

			char* width_s  = MGET(pairs, "width");
			char* height_s = MGET(pairs, "height");
			char* margin_s = MGET(pairs, "margin");
			char* anchor_s = MGET(pairs, "anchor");
			char* color_s  = MGET(pairs, "color");

			if (color_s) color = strtol(color_s, NULL, 16);
			if (width_s) width = atoi(width_s);
			if (height_s) height = atoi(height_s);
			if (anchor_s) anchor = anchor_s;
			if (margin_s) margin = atoi(margin_s);

			wlwindow = ku_wayland_create_generic_layer(
			    monitor,
			    width,
			    height,
			    margin,
			    anchor);

			ku_wayland_show_window(wlwindow);
		    }
		}
		else if (mt_map_get(pairs, "load") != NULL)
		{
		    char* type = MGET(pairs, "load");

		    if (strcmp(type, "html") == 0)
		    {
			char* url = MGET(pairs, "url");

			mt_vector_t* view_list = VNEW();

			ku_gen_html_parse(html_path, view_list);                  // create view structure
			ku_gen_css_apply(view_list, css_path, img_path);          // apply css
			ku_gen_type_apply(view_list, button_event, slider_event); // generate built-in types

			ku_view_t* view_base = mt_vector_head(view_list);

			ku_view_set_frame(view_base, (ku_rect_t){0.0, 0.0, (float) 500, (float) 300});

			kuwindow = ku_window_create(500, 300, 1); // kineti ui view holder window

			ku_window_add(kuwindow, view_base);
			ku_view_describe(view_base, 0);
		    }
		}
		else if (mt_map_get(pairs, "set") != NULL)
		{
		    char* type = MGET(pairs, "set");

		    if (strcmp(type, "slider") == 0)
		    {
		    }
		}

		REL(pairs);

		mt_log_debug("COMMAND %s", command);

		command = mt_string_reset(command);
	    }
	}
	else
	{
	    command = mt_string_append(command, ev.text);
	}
    }
    else if (ev.type == KU_EVENT_WINDOW_SHOWN)
    {
	shown = 1;
    }

    if (shown && wlwindow->frame_cb == NULL)
    {
	if (wlwindow)
	{
	    if (kuwindow)
	    {
		ku_window_event(kuwindow, ev);

		ku_rect_t dirty = ku_window_update(kuwindow, 0);

		if (dirty.w > 0 && dirty.h > 0)
		{
		    ku_rect_t sum = ku_rect_add(dirty, dirtyrect);

		    /* mt_log_debug("drt %i %i %i %i", (int) dirty.x, (int) dirty.y, (int) dirty.w, (int) dirty.h); */
		    /* mt_log_debug("drt prev %i %i %i %i", (int) wcp.dirtyrect.x, (int) wcp.dirtyrect.y, (int) wcp.dirtyrect.w, (int) wcp.dirtyrect.h); */
		    /* mt_log_debug("sum aftr %i %i %i %i", (int) sum.x, (int) sum.y, (int) sum.w, (int) sum.h); */

		    ku_renderer_software_render(kuwindow->views, &wlwindow->bitmap, sum);

		    ku_wayland_request_frame(wlwindow);

		    ku_wayland_draw_window(wlwindow, 0, 0, wlwindow->width, wlwindow->height);

		    dirtyrect = dirty;
		}
	    }
	    else
	    {
		ku_bitmap_blend_rect(&wlwindow->bitmap, (int) 0, (int) 0, (int) wlwindow->width, (int) wlwindow->height, color);
		ku_wayland_draw_window(wlwindow, 0, 0, wlwindow->width, wlwindow->height);
	    }
	}
    }
}

void destroy()
{
    mt_log_debug("DESTROY");
}

int main(int argc, char* argv[])
{
    mt_log_use_colors(isatty(STDERR_FILENO));
    mt_log_level_info();
    mt_time(NULL);

    printf("Kinetic UI Daemon v" KUID_VERSION
	   " by Milan Toth ( www.milgra.com )\n");

    const char* usage =
	"Usage: wcp [options]\n"
	"\n"
	"  -h, --help                            Show help message and quit.\n"
	"  -v, --verbose                         Increase verbosity of messages, defaults to errors and warnings only.\n"
	"\n";

    const struct option long_options[] =
	{
	    {"help", no_argument, NULL, 'h'},
	    {"verbose", no_argument, NULL, 'v'}};

    int verbose      = 0;
    int option       = 0;
    int option_index = 0;

    while ((option = getopt_long(argc, argv, "vhr:f:m:a:", long_options, &option_index)) != -1)
    {
	switch (option)
	{
	    case '?': printf("parsing option %c value: %s\n", option, optarg); break;
	    case 'v': verbose = 1; break;
	    default: fprintf(stderr, "%s", usage); return EXIT_FAILURE;
	}
    }

    char  cwd[PATH_MAX] = {"~"};
    char* res           = getcwd(cwd, sizeof(cwd));

    if (res == NULL)
	mt_log_error("getcwd error");

    char* wrk_path = mt_path_new_normalize(cwd);

    css_path  = mt_path_new_append(wrk_path, "example/html/main.css");  // REL 9
    html_path = mt_path_new_append(wrk_path, "example/html/main.html"); // REL 10
    img_path  = mt_path_new_append(wrk_path, "example/img/");           // REL 9

    srand((unsigned int) time(NULL));

    if (verbose) mt_log_inc_verbosity();

    command = mt_string_new_cstring("");

    ku_wayland_init(init, update, destroy, 0);

    // cleanup

#ifdef MT_MEMORY_DEBUG
    mt_memory_stats();
#endif
}
