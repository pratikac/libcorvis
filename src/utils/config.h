#ifndef __config_h__
#define __config_h__

#include <cstdio>
#include <bot_core/bot_core.h>
#include <bot_param/param_client.h>

class corvis_config_t
{
    public:
        lcm_t* lcm;
        BotParam* param;

        corvis_config_t(int keep_updated = 0)
        {
            lcm = bot_lcm_get_global(NULL);
            param = bot_param_new_from_server(lcm, keep_updated);
            if(!param)
            {
                fprintf(stderr, "Could not find parameter server, quit\n");
                exit(1);
            }
        }
       
        ~corvis_config_t()
        {

        }
};

#endif
