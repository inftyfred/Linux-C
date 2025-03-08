#ifndef SERVERCONF_H__
#define SERVERCONF_H__

#define DEFAULT_MEDIADIR    "/var/media"
#define DEFAULT_IF          "eth0"

typedef enum
{
    RUN_DAEMON = 1,
    RUN_FOREGROUND
}RUNMODE;

struct server_conf_st
{
    char *rcvport;
    char *mgroup;
    char *media_dir;
    RUNMODE runmode;
    char *ifname;
};

extern struct server_conf_st server_conf;

#endif