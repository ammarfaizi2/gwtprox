// SPDX-License-Identifier: GPL-2.0-only
#ifndef GWTPROX__GWTPROX_H
#define GWTPROX__GWTPROX_H

#include <stdint.h>
#include <stddef.h>
#include <sys/epoll.h>

enum {
	GWTP_EVENT_POLL = 0,
	GWTP_EVENT_LAST,
};

struct gwtp_cfg {
	uint8_t event_mode;
	char *tcp_bind_addr;
	char *udp_bind_addr;
	char **out_src_addr;
	uint16_t tcp_bind_port;
	uint16_t udp_bind_port;
	uint16_t nr_out_src_addr;
};

struct gwtp_ctx {
	int	tcp_fd;
	int	udp_fd;

	union {
		struct {
			int epl_fd;
			int evt_fd;
			struct epoll_event *events;
			size_t nr_events;
		} epoll;
	};

	struct gwtp_cfg cfg;
};

int gwtp_cfg_init(struct gwtp_cfg *cfg);
int gwtp_cfg_add_out_src_addr(struct gwtp_cfg *cfg, const char *addr);
int gwtp_cfg_set_tcp_bind(struct gwtp_cfg *cfg, const char *addr, uint16_t port);
int gwtp_cfg_set_udp_bind(struct gwtp_cfg *cfg, const char *addr, uint16_t port);
void gwtp_cfg_destroy(struct gwtp_cfg *cfg);

int gwtp_ctx_init(struct gwtp_ctx *ctx, const struct gwtp_cfg *cfg);
int gwtp_ctx_run(struct gwtp_ctx *ctx);
void gwtp_ctx_destroy(struct gwtp_ctx *ctx);

#endif /* #ifndef GWTPROX__GWTPROX_H */
