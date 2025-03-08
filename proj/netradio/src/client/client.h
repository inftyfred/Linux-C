#ifndef CLIENT_H__
#define CLIENT_H__

#define DEFAULT_PLAYERCMD   "/usr/bin/ffplay -nodisp -autoexit -i - > /dev/null"

struct client_conf_st
{
    char *rcvport;
    char *mgroup;
    char *player_cmd;
};

extern struct client_conf_st client_conf;

#endif