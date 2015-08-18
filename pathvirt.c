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

// NOTE: DMTCP_PATH_PREFIX env variables cannot exceed 1024 characters in length
// TODO
static char old_path_prefix_list[1024];
static char new_path_prefix_list[1024];

/* static int startswith(const char *target, const char *prefix) { */
/*     // invert strncmp ret to return true if startswith and false else */
/*     return !strncmp(target, prefix, strlen(prefix)); */
/* } */


/*
 * clfind - returns first index in colonlist which is a prefix for path
 */
static int clfind(char *colonlist, const char *path)
{
    int index = 0;
    char *element = colonlist, *colon;

    // while there is a colon present, loop
    while (colon = strchr(element, ':')) {
        /* check if element is a prefix of path. here, colon - element is
           an easy way to calculate the length of the element in the list
           to use as the size parameter to strncmp */
        if (strncmp(path, element, colon - element) == 0)
            return index;

        /* move element to point to next element */
        element = colon + 1;

        /* bump index count */
        index++;

    }

    // process the last element in the list
    if (strncmp(path, element, strlen(element)) == 0)
        return index;

    // not found
    return -1;
}

/*
 * clget - returns pointer to element in colonlist at index i
 *         and NULL if not found
 */
char *clget(char *colonlist, unsigned int i)
{
    int curr_ind = 0;
    char *element = colonlist, *colon;

    /* iterate through elements until last one */
    while (colon = strchr(element, ':')) {
        if (curr_ind == i)
            return element;
        element = colon + 1;
        curr_ind++;
    }

    /* last element */
    if (curr_ind == i)
        return element;

    /* not found */
    return NULL;
}

/*
 * clgetsize - returns size of an element at index i in colonlist
 *             and -1 if not found
 */
static ssize_t clgetsize(char *colonlist, const unsigned int i)
{
    char *element = clget(colonlist, i);
    if (element) {
        char *colon = strchr(element, ':');
        return colon ? colon - element : strlen(element);
    }
    return -1;
}

int fopen64(const char *path, const char *mode) {

    puts(path);
    if (!should_swap) {
        puts("not swapping");
        return NEXT_FNC(fopen64)(path, mode);
    }
    puts("swapping");

    // should swap
    int index = clfind(old_path_prefix_list, path);
    if (index == -1)
        return NEXT_FNC(fopen64)(path,mode);

    // found it in old list
    char *new = clget(new_path_prefix_list, index);
    if (new == NULL) {
        return NEXT_FNC(fopen64)(path, mode);
    }

    // determine element length
    char *colon = strchr(new, ':');
    size_t element_sz = colon ? colon - new : strlen(new);

    char newcpy[element_sz + 1];
    memcpy(newcpy, new, element_sz);
    newcpy[element_sz] = '\0';
    puts(newcpy);

    // found corresponding new version

    /* plus 1 is for safety slash we include between the new prefix and the
       unchanged rest of the path. this is in case their environment
       variable doesn't end with a slash. in the "worst" case,
       there will be two extra slashes if the new prefix ends with a slash
       and the old one doesn't */
    size_t newpathsize = (strlen(path) - clgetsize(old_path_prefix_list, index)) + strlen(newcpy) + 1;
    char newpath[newpathsize + 1];
    snprintf(newpath, sizeof newpath, "%s/%s", newcpy,
             path + clgetsize(old_path_prefix_list, index));
    puts(newpath);
    return NEXT_FNC(fopen64)(newpath, mode);

}
int open(const char *path, int oflag, mode_t mode) {

    puts(path);
    if (!should_swap) {
        puts("not swapping");
        return NEXT_FNC(open)(path, oflag, mode);
    }
    puts("swapping");
    // should swap
    int index = clfind(old_path_prefix_list, path);
    if (index == -1)
        return NEXT_FNC(open)(path, oflag, mode);
    puts("1");

    // found it in old list
    char *new = clget(new_path_prefix_list, index);
    if (new == NULL) {
        return NEXT_FNC(open)(path, oflag, mode);
    }

    // determine element length
    char *colon = strchr(new, ':');
    size_t element_sz = colon ? colon - new : strlen(new);

    char newcpy[element_sz + 1];
    memcpy(newcpy, new, element_sz);
    newcpy[element_sz - 1] = '\0';

    // found corresponding new version

    // plus 1 is for safety slash we include between new_path_prefix_list a
    size_t newpathsize = (strlen(path) - strlen(old_path_prefix_list)) + strlen(newcpy) + 1;
    char newpath[newpathsize + 1];
    snprintf(newpath, sizeof newpath, "%s/%s", newcpy,
             path + strlen(old_path_prefix_list));
    /* puts(newpath); */
    puts("2");
    return NEXT_FNC(open)(newpath, oflag, mode);


#if 0
    if (should_swap && startswith(path, old_path_prefix_list)) {
        // plus 1 is for safety slash we include between new_path_prefix_list a
        size_t newpathsize = (strlen(path) - strlen(old_path_prefix_list)) + strlen(new_path_prefix_list) + 1;
        char newpath[newpathsize + 1];
        snprintf(newpath, sizeof newpath, "%s/%s", new_path_prefix_list,
                 path + strlen(old_path_prefix_list));
        /* puts(newpath); */
        return NEXT_FNC(open)(newpath, oflag, mode);
    } else {
        return NEXT_FNC(open)(path, oflag, mode);
    }
#endif
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
            strncpy(old_path_prefix_list, old_env, sizeof old_path_prefix_list);
            old_path_prefix_list[strlen(old_env)] = 0;
        }

        break;
    }
    case DMTCP_EVENT_RESTART:
    {
        /* necessary since we don't know how many bytes dmtcp_get_restart_env
           will write */
        memset(new_path_prefix_list, 0, sizeof new_path_prefix_list);

        /* Try to get the value of ENV_DPP from new environment variables,
         * passed in on restart */
        int ret = dmtcp_get_restart_env(ENV_DPP, new_path_prefix_list,
                                        sizeof(new_path_prefix_list) - 1);
        printf("%d\n", ret);
        if (ret == -1) {
            /* env var did not exist. no new prefix given, so do nothing */
            break;
        } else if (ret == -2) {
            // TODO
            /* need to allocate more memory and retry */
        }

        /* check if an initial DMTCP_PATH_PREFIX was even supplied
         * (old_path_prefix is initialized to zeros on start, and would only
         * contain something if something had been written there */
        if (*old_path_prefix_list)
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
