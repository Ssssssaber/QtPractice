#include "nii_test.h"

static void
nii_backtrace (void)
{
#if 0
    void *array[1024];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 1024);
    strings = backtrace_symbols(array, size);

    fprintf(stderr,"Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++) fprintf(stderr, "  %s\n", strings[i]);

    fflush(stderr);
    free(strings);
    sleep(1);
#else
    char buf[1024];
    pid_t dying_pid = getpid();
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork() while collecting backtrace:");
    }
    else if (child_pid == 0) {
        const char* argv[] = {"sh", "-c", buf, NULL};
        sprintf(buf, "gdb -p %d -batch -ex 'thread apply all bt' 2>/dev/null", dying_pid);
        execve("/bin/sh", (char**)argv, NULL);
        _exit(1);
    }
    else waitpid(child_pid, NULL, 0);
#endif
}

static void
print_usage (char **argv)
{
    printf("Usage: %s [options]\n"
           "Options:\n"
           "  -A, --accel_freq N       accelarator output frequency (default %d)\n"
           "  -a, --accel_range N      accelarator range (default %d)\n"
           "  -s, --accel_avr N        accelarator averaging number (default %d)\n"
           "  -G, --gyro_freq N        gyroscope output frequency (default %d)\n"
           "  -g, --gyro_range N       gyroscope range (default %d)\n"
           "  -j, --gyro_avr N         gyroscope averaging number (default %d)\n"
           "  -M, --magnet_freq N      magnet output frequency (default %d)\n"
           "  -m, --magnet_avr N       magnet averaging number (default %d)\n"
           "  -h, --help               print usage and exit\n"
           "  -n, --nv08c_freq N       NV08C output frequency (default %d)\n"
           "  -P, --press_freq N       pressure output frequency (default %d)\n"
           "  -p, --press_avr N        pressure averaging number (default %d)\n"
           "  -l, --lost N             print warning message if more than N packets were lost at once\n"
           "  -e, --erase              erase EEPROM configuration data (4 kBytes)\n"
           "  -d, --dump <filename>    dump EEPROM configuration data to file\n"
           "  -w, --write <filename>   fill EEPROM from file\n"
           "  -r, --reset              reset NV08C\n",
           argv[0],
           params.accel_freq, params.accel_range, params.accel_avr,
           params.gyro_freq, params.gyro_range, params.gyro_avr,
           params.magnet_duty, params.magnet_avr,
           params.nv08c_freq,
           params.press_freq, params.press_filter);
}

static struct option options[] = {
    { "accel_freq"   , required_argument , 0, 'A' },
    { "accel_range"  , required_argument , 0, 'a' },
    { "accel_avr"    , required_argument , 0, 's' },
    { "gyro_freq"    , required_argument , 0, 'G' },
    { "gyro_range"   , required_argument , 0, 'g' },
    { "gyro_avr"     , required_argument , 0, 'j' },
    { "magnet_freq"  , required_argument , 0, 'M' },
    { "magnet_avr"   , required_argument , 0, 'm' },
    { "nv08c_freq"   , required_argument , 0, 'n' },
    { "press_freq"   , required_argument , 0, 'P' },
    { "press_avr"    , required_argument , 0, 'p' },
    { "lost"         , required_argument , 0, 'l' },
    { "dump"         , required_argument , 0, 'd' },
    { "write"        , required_argument , 0, 'w' },
    { "erase"        , no_argument       , 0, 'e' },
    { "reset"        , no_argument       , 0, 'r' },
    { "help"         , no_argument       , 0, 'h' },
    {0, 0, 0, 0}
};

static int
calculate_duration (int freq, int avr, int d)
{
    uint64_t duration;

    switch ( freq ) {
    case 0: duration = 1000000 * d; break;
    case 1: duration = 500000 * d; break;
    case 2: duration = 250000 * d; break;
    }

    switch ( avr ) {
    case 8: duration <<= 8; break;
    case 7: duration <<= 7; break;
    case 6: duration <<= 6; break;
    case 5: duration <<= 5; break;
    case 4: duration <<= 4; break;
    case 3: duration <<= 3; break;
    case 2: duration <<= 2; break;
    case 1: duration <<= 1; break;
    }

    return duration;
}

static void
parse_args (int argc, char **argv)
{
    int index = 0, ret = 0;
    int d, ts = 0;

    for (;;) {
        switch ( getopt_long(argc, argv, "A:G:M:P:a:d:g:j:l:m:n:p:s:w:ehr", options, &index) ) {
        case -1:
            if ( ts ) {
                accel_duration = calculate_duration (params.accel_freq, params.accel_avr, ts);
                gyro_duration = calculate_duration (params.gyro_freq, params.gyro_avr, ts);
            }
            return;
        case 0:  break;
        case 'A':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 2 ) {
                fprintf(stderr, "Accelarator frequency option must be in 0-2 range\n");
                goto error;
            }
            params.accel_freq = d;
            break;
        case 'G':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 2 ) {
                fprintf(stderr, "Gyroscope frequency option must be in 0-2 range\n");
                goto error;
            }
            params.gyro_freq = d;
            break;
        case 'M':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 3 ) {
                fprintf(stderr, "Magnet frequency option must be in 0-3 range\n");
                goto error;
            }
            params.magnet_duty = d;
            break;
        case 'P':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 3 ) {
                fprintf(stderr, "Pressure frequency option must be in 0-3 range\n");
                goto error;
            }
            params.press_freq = d;
            break;
        case 'a':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 2 ) {
                fprintf(stderr, "Accelarator range option must be in 0-2 range\n");
                goto error;
            }
            params.accel_range = d;
            break;
        case 'd':
            dump = optarg;
            break;
        case 'e':
            erase_EEPROM = 1;
            break;
        case 'g':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 3 ) {
                fprintf(stderr, "Gyroscope range option must be in 0-3 range\n");
                goto error;
            }
            params.gyro_range = d;
            break;
        case 'h':
            goto usage;
        case 'j':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 8 ) {
                fprintf(stderr, "Gyroscope avarage option must be in 0-8 range\n");
                goto error;
            }
            params.gyro_avr = d;
            break;
        case 'l':
            if ( sscanf(optarg,"%d",&ts) != 1 ) goto error;
            if ( 0 > ts || 200 < ts ) {
                fprintf(stderr, "'lost' option must be in 0-200 range\n");
                goto error;
            }
            break;
        case 'm':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 8 ) {
                fprintf(stderr, "Magnet average option must be in 0-8 range\n");
                goto error;
            }
            params.magnet_avr = d;
            break;
        case 'n':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 3 ) {
                fprintf(stderr, "NV08C frequency option must be in 0-3 range\n");
                goto error;
            }
            params.nv08c_freq |= d;
            break;
        case 'p':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 8 ) {
                fprintf(stderr, "Pressure average option must be in 0-8 range\n");
                goto error;
            }
            params.press_filter = d;
            break;
        case 'r':
            params.nv08c_freq |= 4;
            break;
        case 's':
            if ( sscanf(optarg,"%d",&d) != 1 ) goto error;
            if ( d < 0 || d > 8 ) {
                fprintf(stderr, "Accelarator average option must be in 0-8 range\n");
                goto error;
            }
            params.accel_avr = d;
            break;
        case 'w':
            conf = optarg;
            break;
        case '?':
            goto error;
        default:
            abort();
        }
    }

error:

    fprintf(stderr, "Error: wrong options\n");
    ret = EXIT_FAILURE;

usage:
    print_usage(argv);
    exit(ret);
}

static void
nii_close (void)
{
    __atomic_store_n(&running, 0, __ATOMIC_SEQ_CST);
    if ( __atomic_test_and_set(&exit_flag, __ATOMIC_SEQ_CST) )
        return;
    if ( __atomic_load_n(&ctrl_thread_valid, __ATOMIC_SEQ_CST) ) {
        pthread_cancel(ctrl_thread);
        pthread_join(ctrl_thread, NULL);
    }
    libnii_close(handle);
    if ( 0 <= dump_fd ) close(dump_fd);
    if ( 0 <= conf_fd ) close(conf_fd);
}

static void
nii_exit (int status)
{
    nii_close();
    _exit(status);
}

static void
nii_print_lock (void)
{
    pthread_mutex_lock(&print_mutex);
}

static void
nii_print_unlock (void)
{
    pthread_mutex_unlock(&print_mutex);
}

static void
nii_print (const char *format, ...)
{
    va_list arg;
    pthread_mutex_lock(&print_mutex);
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);
    pthread_mutex_unlock(&print_mutex);
}

static void
nii_print_error (const char *format, ...)
{
    va_list arg;
    pthread_mutex_lock(&print_mutex);
    printf("\033[31mError: ");
    fflush(stdout);
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
    printf("\033[37m");
    fflush(stdout);
    pthread_mutex_unlock(&print_mutex);
}

static void
print_xyz (char c, libnii_xyz_data_t *xyz, int num)
{
    nii_print("%c  %3d  %10d %10d %10d %20lu\n", c, num, (int)xyz->x, (int)xyz->y, (int)xyz->z, xyz->ts);
}

static void
print_pressure (libnii_press_data_t *p, int num)
{
    int T = p->temperature / 100;
    int t = p->temperature - T * 100;
    if (0 > t) t = -t;
    nii_print( "   %3d  %u Pa\t%d.%02d C %29lu\n", num, p->pressure, T, t, p->ts );
}

int
calculate_order_base10 (int d)
{
    int n = 1;

    while (d != 0) {
        d /= 10;
        n *= 10;
    }

    return n;
}

static double
convert_to_double ( int sign, int mant)
{
    return ( (double)sign + (double)mant / ((double) calculate_order_base10 (mant)) );
}

static void
print_gps (libnii_gps_data_t *g, int num)
{
    double d;
    char mode = 'x';

    switch ((g->valid >> 1) & 3) {
    case 0: mode = 'A'; break;
    case 1: mode = 'D'; break;
    case 2: mode = 'E'; break;
    case 3: mode = 'N'; break;
    }

    nii_print("@  %3d  %c,%c,%d  %d %02d.%02d.20%02d %02d:%02d:%02d.%03d",
              num, ( g->valid & 1 ) ? 'A' : 'V', mode, g->valid >> 4, g->satelites,
              g->day, g->month, g->year, g->hours, g->min, g->sec, g->ms);
    nii_print(" %d:%d.%d%c", g->lat, g->latm, g->latm_, (char)g->NS);
    nii_print(" %d:%d.%d%c", g->lon, g->lonm, g->lonm_, (char)g->WE);
    d = convert_to_double (g->alt, g->alt_);
    d *= 0.3048; // convert feet to meters
    nii_print(" %fm", d);
    d = convert_to_double (g->speed, g->speed_);
    d *= 0.514; // convert knots to m/s
    nii_print(" %fm/s", d);
    d = convert_to_double (g->course, g->course_);
    nii_print(" %fGr\t%lu\n", d, g->ts);
}

static void
print_pps (libnii_pps_data_t *g, int num)
{
    nii_print("1p %3d\t\t\t\t\t\t%lu\n", num, *g);
}

static void
data_cb (void *user_ptr, enum libnii_data_type type, int packet_number, void *data)
{
    (void) user_ptr;

    if ( __atomic_load_n(&silent, __ATOMIC_SEQ_CST)) return;

    switch ( type ) {
    case LIBNII_STATUS_DATA:
    {
        int i = 7;
        uint8_t *d = (uint8_t *) data;
        nii_print_lock();
        printf("*  %3d", packet_number);
        do
            printf("  %d", *d++);
        while (--i);
        printf("\n");
        fflush(stdout);
        nii_print_unlock();
    }
    break;
    case LIBNII_ACCEL_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        uint64_t diff, ts = xyz->ts;
        print_xyz('A', xyz, packet_number);
        if ( accel_ts && accel_duration ) {
            if ( ts > accel_ts ) diff = ts - accel_ts;
            else                 diff = accel_ts - ts;
            if ( diff > accel_duration )
                nii_print_error("accel timeout: %lu\n", diff);
        }
        accel_ts = ts;
    }
    break;
    case LIBNII_GYRO_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        uint64_t diff, ts = xyz->ts;
        print_xyz('G', xyz, packet_number);
        if ( gyro_ts && gyro_duration ) {
            if ( ts > gyro_ts ) diff = ts - gyro_ts;
            else                diff = gyro_ts - ts;
            if ( diff > gyro_duration )
                nii_print_error("gyro timeout:  %lu\n", diff);
        }
        gyro_ts = ts;
    }
    break;
    case LIBNII_MAGNET_DATA:
        print_xyz('M', (libnii_xyz_data_t *) data, packet_number);
        break;
    case LIBNII_PRESSURE_DATA:
        print_pressure((libnii_press_data_t *) data, packet_number);
        break;
    case LIBNII_GPS_DATA:
        print_gps((libnii_gps_data_t *) data, packet_number);
        break;
    case LIBNII_PPS_DATA:
        print_pps((libnii_pps_data_t *) data, packet_number);
        break;
    }
}

static void
error_cb (void *user_ptr, int error_code)
{
    static int prev = LIBNII_SUCCESS;
    (void) user_ptr;
    if ( LIBNII_SUCCESS != error_code && error_code != prev )
        nii_print_error("code %d: %s\n", error_code, libnii_strerror(error_code));
    prev = error_code;
}

static void *
ctrl_thread_func (void *arg)
{
    int r, major, minor, patch;
    struct libnii_device *dev = (struct libnii_device *) arg;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    while ( ! libnii_is_connected(dev) ) {
        usleep(10000);
    }

    __atomic_store_n(&silent, 1, __ATOMIC_SEQ_CST);

    if ( erase_EEPROM ) {
        nii_print("Erasing EEPROM config page ... ");
        r = libnii_erase_eeprom(dev);
        if ( LIBNII_SUCCESS != r )
            nii_print_error("failed to erase EEPROM config page: %s\n", libnii_strerror(r));
        else {
            erase_EEPROM = 0;
            nii_print(" DONE\n");
        }
    }

    if ( dump ) {
        dump_fd = open(dump, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if ( 0 > dump_fd ) nii_print_error("failed to open dump file '%s': %s\n",
                            dump, strerror(errno));
    }

    if ( 0 <= dump_fd ) {
        int i, err = 0;
        uint64_t value;
        nii_print("Dumping EEPROM config page to file '%s' ... ", dump);
        for ( i = 0; i < 4096 / (int)sizeof(uint64_t); i++ ) {
            r = libnii_read_eeprom(dev, (uint16_t) i, &value);
            if ( LIBNII_SUCCESS != r ) {
                nii_print_error("failed to read EEPROM address %04x(%d): %s\n", (uint16_t)i, i, libnii_strerror(r));
                err = 1;
                break;
            }
            r = write(dump_fd, (void *)&value, sizeof(value));
            if ( sizeof(value) != r ) {
                err = 1;
                nii_print_error("failed to write dump file: %s\n", strerror(errno));
                break;
            }
        }
        close(dump_fd);
        if ( !err ) nii_print(" DONE\n");
        dump_fd = -1;
    }

    if ( conf ) {
        conf_fd = open(conf, O_RDONLY | O_CLOEXEC);
        if ( 0 > conf_fd ) nii_print_error("failed to open config file '%s': %s\n",
                            conf, strerror(errno));
    }

    if ( 0 <= conf_fd ) {
        int i, err = 0;
        union {
            uint64_t word;
            uint8_t data[sizeof(uint64_t)];
        } value;
        nii_print("Writing content of file '%s' to config EEPROM ... ", conf);
        for ( i = 0; i < 4096 / (int)sizeof(uint64_t); i++ ) {
            r = read(conf_fd, value.data, sizeof(uint64_t));
            if ( sizeof(uint64_t) != r ) {
                if ( !r ) break;
                if ( 0 > r ) {
                    nii_print_error("failed to read eeprom config file: %s\n", strerror(errno));
                    err = 1;
                    break;
                }
                do value.data[r - 1] = 0xff; while (++r < (int)sizeof(uint64_t));
            }
            r = libnii_write_eeprom(dev, (uint16_t) i, value.word);
            if ( LIBNII_SUCCESS != r ) {
                err = 1;
                nii_print_error("failed to write EEPROM address %04x(%d): %s\n", (uint16_t) i, i, libnii_strerror(r));
                break;
            }
        }
        close(conf_fd);
        if ( !err ) nii_print(" DONE\n");
        conf_fd = -1;
    }

    __atomic_store_n(&silent, 0, __ATOMIC_SEQ_CST);

    r = libnii_get_version_0(dev, &major, &minor, &patch);
    if ( LIBNII_SUCCESS != r )
        nii_print_error("failed to get version with zero packet: %s\n", libnii_strerror(r));

    r = libnii_get_version(dev, &major, &minor, &patch);
    if ( LIBNII_SUCCESS != r )
        nii_print_error("failed to get version: %s\n", libnii_strerror(r));
    else
        nii_print("FW version: %d.%d.%d\n", major, minor, patch);

    r = libnii_set_params(dev, &params);
    if ( LIBNII_SUCCESS != r )
        nii_print_error("failed to set parameters: %s\n", libnii_strerror(r));

    __atomic_store_n(&ctrl_thread_valid, 0, __ATOMIC_SEQ_CST);

    return NULL;
}

static void
sigusr1_handler (int signum)
{
    (void) signum;
    nii_backtrace();
    nii_exit(EXIT_FAILURE);
}

static void
sigsegv_handler (int signum)
{
    (void) signum;
    fprintf(stderr, "Segmentation fault!\n\n");
    nii_backtrace();
    _exit(EXIT_FAILURE);
}

static int
init_signals ( void )
{
    int fd = -1;
    sigset_t mask;

    signal(SIGUSR1, sigusr1_handler);
    signal(SIGSEGV, sigsegv_handler);

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGUSR2);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
        nii_print_error("failed to block signals: %s\n", strerror(errno));
    else {
        fd = signalfd (-1, &mask, SFD_CLOEXEC);
        if ( 0 > fd )
            nii_print_error("failed to get signals file descriptor: %s\n", strerror(errno));
    }

    return fd;
}

static void
data_send (void *user_ptr, enum libnii_data_type type, int packet_number, void *data)
{
    (void) user_ptr;

    if ( __atomic_load_n(&silent, __ATOMIC_SEQ_CST)) return;

    switch ( type ) {
    /* case LIBNII_STATUS_DATA:
    {
        int i = 7;
        uint8_t *d = (uint8_t *) data;
        nii_print_lock();
        printf("*  %3d", packet_number);
        do
            printf("  %d", *d++);
        while (--i);
        printf("\n");
        fflush(stdout);
        nii_print_unlock();
    }
        break; */
    case LIBNII_ACCEL_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        uint64_t diff, ts = xyz->ts;
        print_xyz('A', xyz, packet_number);
        if ( accel_ts && accel_duration ) {
            if ( ts > accel_ts ) diff = ts - accel_ts;
            else                 diff = accel_ts - ts;
            if ( diff > accel_duration )
                nii_print_error("accel timeout: %lu\n", diff);
        }
        accel_ts = ts;
    }
    break;
    case LIBNII_GYRO_DATA:
    {
        libnii_xyz_data_t *xyz = (libnii_xyz_data_t *) data;
        uint64_t diff, ts = xyz->ts;
        print_xyz('G', xyz, packet_number);
        if ( gyro_ts && gyro_duration ) {
            if ( ts > gyro_ts ) diff = ts - gyro_ts;
            else                diff = gyro_ts - ts;
            if ( diff > gyro_duration )
                nii_print_error("gyro timeout:  %lu\n", diff);
        }
        gyro_ts = ts;
    }
    break;
        case LIBNII_MAGNET_DATA:
        print_xyz('M', (libnii_xyz_data_t *) data, packet_number);
        break;
        /*
    case LIBNII_PRESSURE_DATA:
        print_pressure((libnii_press_data_t *) data, packet_number);
        break;
    case LIBNII_GPS_DATA:
        print_gps((libnii_gps_data_t *) data, packet_number);
        break;
    case LIBNII_PPS_DATA:
        print_pps((libnii_pps_data_t *) data, packet_number);
        break; */
    default:
        printf("Other type data were received.\n");
    }
}

int start(int argc, char **argv)
{
    enum libnii_error err;
    int signal_fd;

    //ProfilerStart("nii.prof");

    parse_args (argc, argv);

    signal_fd = init_signals();
    if ( 0 > signal_fd ) return EXIT_FAILURE;

    /*err = */libnii_open(&handle, VENDOR_ID, PRODUCT_ID, LIBNII_MATCH_ANY, LIBNII_MATCH_ANY,
                      data_send, error_cb, handle);

    /*if ( LIBNII_SUCCESS != err ) {
        nii_print_error("failed to open: %s\n", libnii_strerror(err));
        close(signal_fd);
        return EXIT_FAILURE;
    }*/
    //qDebug() << "HELLO!";

    /*
    ctrl_thread_valid = ! pthread_create(&ctrl_thread, NULL, ctrl_thread_func, (void *)handle);
    if ( ! ctrl_thread_valid ) {
        close(signal_fd);
        nii_close();
        return EXIT_FAILURE;
    }
    */
    //__atomic_store_n(&silent, 1, __ATOMIC_SEQ_CST);
    /*

    //nii_exit(EXIT_SUCCESS);

    while ( __atomic_load_n(&running, __ATOMIC_SEQ_CST) ) {

        int r;
        size_t size;
        fd_set fds_r;
        struct signalfd_siginfo fdsi;
        struct timeval select_timeout;

        FD_ZERO(&fds_r);
        FD_SET(signal_fd, &fds_r);

        select_timeout.tv_sec  = 1;
        select_timeout.tv_usec = 0;

        r = select(signal_fd + 1, &fds_r, NULL, NULL, &select_timeout);
        if ( ! r ) continue;
        if ( 0 > r ) {
            if (EINTR == errno) continue;
            fprintf(stderr, "signal_fd select: %s\n", strerror(errno));
            nii_exit(EXIT_FAILURE);
        }

        if ( ! FD_ISSET(signal_fd, &fds_r) ) continue;

        size = read(signal_fd, &fdsi, sizeof(struct signalfd_siginfo));
        if (size != sizeof(struct signalfd_siginfo)) {
            fprintf(stderr, "wrong signal info size: %s\n", strerror(errno));
            nii_exit(EXIT_FAILURE);
        }

        if ( fdsi.ssi_signo != SIGUSR2 )
            printf("\nReceived signal %d\n", fdsi.ssi_signo);

        switch (fdsi.ssi_signo) {
        case SIGQUIT:
        case SIGTERM:
        case SIGINT:
        case SIGUSR2:
            //ProfilerStop();
            nii_exit(EXIT_SUCCESS);
        }
    }
    */
    //close(signal_fd);
    while(1);
    nii_close();
    return EXIT_FAILURE;
}
