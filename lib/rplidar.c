#include <string.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "rplidar.h"

// https://bucket-download.slamtec.com/6494fd238cf5e0d881f56d914c6d1f355c0f582a/LR001_SLAMTEC_rplidar_protocol_v2.4_en.pdf

#define REQ_START UINT8_C(0xA5)
#define SCAN_PACKET_SZ 5

static const int a1m1_speed = B115200;

static void rp_lidar_log(const char *restrict fmt, ...)
{
    va_list args;
    fprintf(stderr, "RpLidar error: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

bool rp_lidar_init_ctx(struct RpLidarCtx *ctx)
{
    if (!ctx ||
        !ctx->response_buffer_size ||
        ctx->response_buffer_size % SCAN_PACKET_SZ)
        return false;
    struct termios tty_settings;
    cfmakeraw(&tty_settings);
    if (cfsetospeed(&tty_settings, a1m1_speed) < 0) {
        rp_lidar_log("Failed to set output speed: %s\n", strerror(errno));
        return false;
    }
    if (cfsetispeed(&tty_settings, a1m1_speed) < 0) {
        rp_lidar_log("Failed to set input speed: %s\n", strerror(errno));
        return false;
    }
    if (tcsetattr(ctx->fd, TCSANOW, &tty_settings) < 0) {
        rp_lidar_log("Failed to set UART settings: %s\n",
                     strerror(errno));
        return false;
    }
    ctx->rp = ctx->response_buffer;
    ctx->wp = ctx->response_buffer;

    return true;
}

static void rp_lidar_send_request(const struct RpLidarCtx *ctx, enum RpLidarRequest req)
{
    const uint16_t data = (((uint16_t)req) << 8) | ((uint16_t)REQ_START);
    write(ctx->fd, &data, sizeof(data));
}

void rp_lidar_begin_scan(struct RpLidarCtx *ctx)
{
    rp_lidar_send_request(ctx, RPL_REQ_SCAN);
    uint8_t resp[4];
    read(ctx->fd, &resp, sizeof(resp));
}
