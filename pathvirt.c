/* NOTE:  if you just want to insert your own code at the time of checkpoint
 *  and restart, there are two simpler additional mechanisms:
 *  dmtcpaware, and the MTCP special hook functions:
 *    mtcpHookPreCheckpoint, mtcpHookPostCheckpoint, mtcpHookRestart
 */

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include "dmtcp.h"

#define ENV_DPP "DMTCP_PATH_PREFIX"

/* paths should only be swapped on restarts (not on initial run), so this flag */
/* is set on restart */
static int should_swap;

// NOTE: DMTCP_PATH_PREFIX env variables cannot exceed 512 characters in length
// TODO
static char old_path_prefix[512];
static char new_path_prefix[512];

static int startswith(const char *target, const char *prefix) {
    // invert strncmp ret to return true if startswith and false else
    return !strncmp(target, prefix, strlen(prefix));
}

int open(const char *path, int oflag, mode_t mode) {
    if (should_swap && startswith(path, old_path_prefix)) {
        // plus 1 is for safety slash we include between new_path_prefix a
        size_t newpathsize = (strlen(path) - strlen(old_path_prefix)) + strlen(new_path_prefix) + 1;
        char newpath[newpathsize + 1];
        snprintf(newpath, sizeof newpath, "%s/%s", new_path_prefix,
                 path + strlen(old_path_prefix));
        puts(newpath);
        puts(old_path_prefix);
        printf("%p\n", old_path_prefix);
        return NEXT_FNC(open)(newpath, oflag, mode);
    } else {
        return NEXT_FNC(open)(path, oflag, mode);
    }
}

void dmtcp_event_hook(DmtcpEvent_t event, DmtcpEventData_t *data)
{
    /* NOTE:  See warning in plugin/README about calls to printf here. */
    switch (event) {
    case DMTCP_EVENT_INIT:
    {
        /* On init, check if they've specified paths to virtualize via
           DMTCP_PATH_PREFIX env */
        char *old_env = getenv(ENV_DPP);
        if (old_env) {
            /* if so, save it to buffer */
            strncpy(old_path_prefix, old_env, sizeof old_path_prefix);
            old_path_prefix[strlen(old_env)] = 0;
        }

        break;
    }
    case DMTCP_EVENT_RESTART:
    {
        /* necessary since we don't know how many bytes dmtcp_get_restart_env
           will write */
        memset(new_path_prefix, 0, sizeof new_path_prefix);

        /* Try to get the value of ENV_DPP from new environment variables,
         * passed in on restart */
        int ret = dmtcp_get_restart_env(ENV_DPP, new_path_prefix,
                                        sizeof(new_path_prefix) - 1);
        printf("%d\n", ret);
        if (ret == -1) {
            /* env var did not exist. no new prefix given, so do nothing */
            break;
        } else if (ret == -2) {
            /* need to allocate more memory and retry */
        }

        /* check if an initial DMTCP_PATH_PREFIX was even supplied
         * (old_path_prefix is initialized to zeros on start, and would only
         * contain something if something had been written there */
        if (*old_path_prefix)
            should_swap = 1;

        /* else */
        /*     puts("no old path given!"); */

        break;
    }

    case DMTCP_EVENT_WRITE_CKPT:
        printf("\n*** The plugin %s is being called before checkpointing. ***\n",
           __FILE__);
        break;
    case DMTCP_EVENT_RESUME:
        printf("*** The plugin %s has now been checkpointed. ***\n", __FILE__);
        break;
    default:
    ;
    }

    /* Call this next line in order to pass DMTCP events to later plugins. */
    DMTCP_NEXT_EVENT_HOOK(event, data);
}
