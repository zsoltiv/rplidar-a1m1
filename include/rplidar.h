#ifndef RPLIDAR_H
#define RPLIDAR_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

enum RpLidarRequest {
    RPL_REQ_START,
    RPL_REQ_STOP = UINT8_C(0x25),
    RPL_REQ_RESET = UINT8_C(0x40),
    RPL_REQ_SCAN = UINT8_C(0x20),
};
struct RpLidarCtx {
    uint8_t *response_buffer;
    uint8_t *rp, *wp;
    size_t response_buffer_size;
    int fd;
};
struct RpLidar;

bool rp_lidar_init_ctx(struct RpLidarCtx *ctx);
void rp_lidar_begin_scan(struct RpLidarCtx *ctx);

#endif /* RPLIDAR_H */
