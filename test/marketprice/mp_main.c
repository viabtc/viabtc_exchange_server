/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/16, create
 */

#include "ut_redis.h"

const char *__process__ = "marketprice";
const char *__version__ = "0.1.0";


int main(int argc, char *argv[])
{
    printf("process: %s version: %s, compile date: %s %s\n", __process__, __version__, __DATE__, __TIME__);

    struct timeval timeout = { 3, 0 };
    redisContext *redis = redisConnectWithTimeout("127.0.0.1", 26379, timeout);
    if (redis == NULL || redis->err) {
        if (redis) {
            redisFree(redis);
        }
    }
    redisReply *reply = redisCommand(redis, "SENTINEL get-master-addr-by-name %s", "mymaster");
    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
        if (reply) {
            freeReplyObject(reply);
            redisFree(redis);
        }
    }

    return 0;
}

