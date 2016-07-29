#ifndef __param_utils_h__
#define __param_utils_h__

#include <cstdio>
#include <bot_core/bot_core.h>
#include <bot_param/param_client.h>


/*
 * returns
 *  sz[2]   = {width, height}
 *  v[5]    = {fx, fy, cx, cy, distortion}
 *  normalized by the size
 */
void corvis_get_calibration_atan(BotParam* p,
        char* key, double sz[2], double v[5])
{
    if(!p)
    {
        fprintf(stderr, "BotParam is NULL in corvis_get_calibration_atan\n");
        exit(1);
    }

    char* t1;
    sprintf(t1, "cameras.%s", key);
    if(bot_param_has_key(p, t1))
    {
        char* t2 = strdup(t1);
        sprintf(t2, "%s.width", t2);
        sz[0] = bot_param_get_double_or_fail(p, t2);
        free(t2);

        t2 = strdup(t1);
        sprintf(t2, "%s.height", t2);
        sz[1] = bot_param_get_double_or_fail(p, t2);
        free(t2);

        t2 = strdup(t1);
        sprintf(t2, "%s.params", t2);
        bot_param_get_double_array_or_fail(p, t2, v, 5);
        free(t2);
    }
    else
    {
        fprintf(stderr, "Could not find: %s, quitting\n", t1);
        exit(1);
    }
    free(t1);
}

#endif
