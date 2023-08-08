#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <math.h>

#define MAX 10
#define MAX_THREADS 56

typedef unsigned char byte;

const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

typedef struct
{
    int start_len;
    int end_len;
    byte *hash1;
    _Atomic int *found;
} ThreadPool;

typedef struct
{
    pthread_t thread;
    int thread_id;
    ThreadPool *pool;
} ThreadInfo;

/*
 * This procedure generates all combinations of possible letters
 */
void *iterate(void *arg)
{

    ThreadInfo *info = (ThreadInfo *)arg;
    ThreadPool *pool = info->pool;
    int i, j;
    int len;
    char str[MAX + 1] = {0};
    byte *hash1 = pool->hash1;
    _Atomic int *found = pool->found;
    byte hash2[MD5_DIGEST_LENGTH];
    for (len = pool->start_len; len <= pool->end_len && !(*found); ++len)
    {
        for (i = 0; i < len; ++i)
        {
            str[i] = letters[0];
        }
        while (!atomic_load(found))
        {
            // fprintf(stderr, "Checking %s\n", str);
            MD5((byte *)str, len, hash2);
            if (memcmp(hash1, hash2, MD5_DIGEST_LENGTH) == 0)
            {
                if (!atomic_load(found))
                {
                    printf("found: %s\n", str);
                    atomic_store(found, 1);
                }
                break;
            }
            for (i = len - 1; i >= 0; --i)
            {
                if (str[i] == letters[strlen(letters) - 1])
                {
                    str[i] = letters[0];
                }
                else
                {
                    for (j = 0; j < strlen(letters); ++j)
                    {
                        if (str[i] == letters[j])
                        {
                            str[i] = letters[j + 1];
                            break;
                        }
                    }
                    break;
                }
            }
            if (i < 0)
            {
                break;
            }
        }
        if (atomic_load(found))
        {
            break;
        }
    }
}

/*
 * Convert hexadecimal string to hash byte.
 */
void strHex_to_byte(char *str, byte *hash)
{
    char *pos = str;
    int i;
    for (i = 0; i < MD5_DIGEST_LENGTH / sizeof *hash; i++)
    {
        sscanf(pos, "%2hhx", &hash[i]);
        pos += 2;
    }
}

int main(int argc, char **argv)
{
    int r;
    char hash1_str[2 * MD5_DIGEST_LENGTH + 1];

    r = scanf("%s", hash1_str);
    if (r != 1)
    {
        printf("Invalid input\n");
        return 1;
    }

    byte hash1[MD5_DIGEST_LENGTH];

    strHex_to_byte(hash1_str, hash1);
    int max_len = MAX;

    int num_cpus = get_nprocs();
    // int num_cpus = 56;

    // nprocs() might return wrong amount inside of a container.
    // Use MAX_CPUS instead, if available.
    char *max_cpus = getenv("MAX_CPUS");
    if (max_cpus != NULL)
    {
        num_cpus = atoi(max_cpus);
    }
    // fprintf(stderr, "Number of CPUs: %d\n", num_cpus);

    _Atomic int found = 0;

    int num_threads = (max_len < num_cpus) ? max_len : num_cpus;
    int len_per_t = (max_len + num_threads - 1) / num_threads;

    ThreadInfo thread_pool[num_threads];
    ThreadPool pools[num_threads];

    int active_threads = 0;
    int i;
    for (i = 0; i < num_threads; i++)
    {
        int start_len = (i * len_per_t) + 1;
        int end_len = start_len + len_per_t - 1;
        if (end_len > max_len)
        {
            end_len = max_len;
        }
        // fprintf(stderr, "start: %d\n", start_len);
        // fprintf(stderr, "end: %d\n", end_len);
        pools[i].start_len = start_len;
        pools[i].end_len = end_len;
        pools[i].hash1 = hash1;
        pools[i].found = &found;
        thread_pool[i].pool = &pools[i];
        pthread_create(&thread_pool[i].thread, NULL, &iterate, (void *)&thread_pool[i]);
        active_threads++;
    }

    for (i = 0; i < active_threads; i++)
    {
        pthread_join(thread_pool[i].thread, NULL);
    }

    if (!found)
    {
        printf("Could not find a matching string.\n");
    }

    return 0;
}
