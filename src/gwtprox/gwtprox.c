// SPDX-License-Identifier: GPL-2.0-only

#include <gwtprox/gwtprox.h>

#include <string.h>
#include <errno.h>

int gwtp_cfg_init(struct gwtp_cfg *cfg)
{
	memset(cfg, 0, sizeof(*cfg));
	return 0;
}

int gwtp_cfg_add_out_src_addr(struct gwtp_cfg *cfg, const char *addr)
{
	size_t new_size, alloc_size;
	char **tmp_list;

	new_size = cfg->nr_out_src_addr + 1;
	alloc_size = new_size * sizeof(*cfg->out_src_addr);
	tmp_list = realloc(cfg->out_src_addr, alloc_size);
	if (!tmp_list)
		return -ENOMEM;

	cfg->out_src_addr = tmp_list;
	cfg->out_src_addr[cfg->nr_out_src_addr] = strdup(addr);
	if (!cfg->out_src_addr[cfg->nr_out_src_addr])
		return -ENOMEM;

	cfg->nr_out_src_addr = new_size;
	return 0;
}

int gwtp_cfg_set_tcp_bind(struct gwtp_cfg *cfg, const char *addr, uint16_t port)
{
	if (cfg->tcp_bind_addr)
		free(cfg->tcp_bind_addr);

	cfg->tcp_bind_addr = strdup(addr);
	if (!cfg->tcp_bind_addr)
		return -ENOMEM;

	cfg->tcp_bind_port = port;
	return 0;
}

int gwtp_cfg_set_udp_bind(struct gwtp_cfg *cfg, const char *addr, uint16_t port)
{
	if (cfg->udp_bind_addr)
		free(cfg->udp_bind_addr);

	cfg->udp_bind_addr = strdup(addr);
	if (!cfg->udp_bind_addr)
		return -ENOMEM;

	cfg->udp_bind_port = port;
	return 0;
}

void gwtp_cfg_destroy(struct gwtp_cfg *cfg)
{
	if (cfg->out_src_addr) {
		size_t i;

		for (i = 0; i < cfg->nr_out_src_addr; i++)
			free(cfg->out_src_addr[i]);
		free(cfg->out_src_addr);
	}

	if (cfg->tcp_bind_addr)
		free(cfg->tcp_bind_addr);

	if (cfg->udp_bind_addr)
		free(cfg->udp_bind_addr);

	memset(cfg, 0, sizeof(*cfg));
}

static int gwtp_cfg_copy(struct gwtp_cfg *dst, const struct gwtp_cfg *src)
{
	size_t i;
	int err;

	err = gwtp_cfg_init(dst);
	if (err)
		return err;

	dst->event_mode = src->event_mode;
	if (src->tcp_bind_addr) {
		err = gwtp_cfg_set_tcp_bind(dst, src->tcp_bind_addr,
					    src->tcp_bind_port);
		if (err)
			goto out;
	}

	if (src->udp_bind_addr) {
		err = gwtp_cfg_set_udp_bind(dst, src->udp_bind_addr,
					    src->udp_bind_port);
		if (err)
			goto out;
	}

	if (src->nr_out_src_addr) {
		for (i = 0; i < src->nr_out_src_addr; i++) {
			const char *addr = src->out_src_addr[i];

			err = gwtp_cfg_add_out_src_addr(dst, addr);
			if (err)
				goto out;
		}
	}

out:
	if (err)
		gwtp_cfg_destroy(dst);

	return err;
}

int gwtp_ctx_init(struct gwtp_ctx *ctx, const struct gwtp_cfg *cfg)
{
	int ret;

	memset(ctx, 0, sizeof(*ctx));

	ret = gwtp_cfg_copy(&ctx->cfg, cfg);
	if (ret)
		return ret;

	// Init tcp

	// Init udp

	// Init epoll

	// Start event loop

	return 0;
}

static int close_se(int *fd)
{
	if (*fd >= 0) {
		close(*fd);
		*fd = -1;
	}

	return 0;
}

void gwtp_ctx_destroy(struct gwtp_ctx *ctx)
{
	close_se(&ctx->tcp_fd);
	close_se(&ctx->udp_fd);

	if (ctx->cfg.event_mode == GWTP_EVENT_POLL) {
		close_se(&ctx->epoll.evt_fd);
		close_se(&ctx->epoll.epl_fd);
		free(ctx->epoll.events);
	}

	gwtp_cfg_destroy(&ctx->cfg);
	memset(ctx, 0, sizeof(*ctx));
}
