#include "ku_connector_wayland.c"
#include "mt_string.c"

char*                command;
struct monitor_info* monitor;
wl_window_t*         wlwindow;
int                  shown = 0;
int                  drawn = 0;

void init(wl_event_t ev)
{
    mt_log_debug("INIT, EVENT : %i", ev.id);

    monitor = ev.monitors[0];

    mt_log_debug("NAME %s", monitor->name);
}

void update(ku_event_t ev)
{
    mt_log_debug("UPDATE %i", ev.type);
    if (ev.type == KU_EVENT_STDIN)
    {
	if (ev.text[0] == '\n')
	{
	    if (strcmp(command, "CREATE") == 0)
	    {
		wlwindow = ku_wayland_create_generic_layer(monitor, 1200, 900, 0, "lt");

		ku_wayland_show_window(wlwindow);

		mt_log_debug("COMMAND %s", command);
	    }

	    command = mt_string_reset(command);
	}
	else
	{
	    command = mt_string_append(command, ev.text);
	}
    }

    if (ev.type == KU_EVENT_WINDOW_SHOWN)
    {
	mt_log_debug("WINDOW SHOW");
	shown = 1;
    }

    if (shown && wlwindow->frame_cb == NULL)
    {
	mt_log_debug("REQUEST FRAME");
	ku_bitmap_blend_rect(&wlwindow->bitmap, (int) 0, (int) 0, (int) 500, (int) 300, 0x55FF00FF);
	ku_wayland_draw_window(wlwindow, 0, 0, 500, 300);
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

    srand((unsigned int) time(NULL));

    if (verbose) mt_log_inc_verbosity();

    command = mt_string_new_cstring("");

    ku_wayland_init(init, update, destroy, 0);

    // cleanup

#ifdef MT_MEMORY_DEBUG
    mt_memory_stats();
#endif
}
