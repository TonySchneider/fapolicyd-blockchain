/*
 * blockchain-backend.c - file backend
 * Authors:
 *   Tony Schneider <tony.schneider@kyndryl.com>
 */

#include "config.h"

#include <stddef.h>

#include "fapolicyd-backend.h"
#include "llist.h"
#include "message.h"
#include "trust-file.h"
#include "blockchain-helpers.h"


static int file_init_backend(void);
static int file_load_list(const conf_t *conf);
static int file_destroy_backend(void);

backend blockchain_backend =
{
	"blockchain",
	file_init_backend,
	file_load_list,
	file_destroy_backend,
	{ 0, 0, NULL },
};



static int file_load_list(const conf_t *conf)
{
	msg(LOG_DEBUG, "Loading blockchain backend");
	list_empty(&blockchain_backend.list);
	load_trusted_file_names(&blockchain_backend.list);
	return 0;
}

static int file_init_backend(void)
{
	list_init(&blockchain_backend.list);
	return 0;
}

static int file_destroy_backend(void)
{
	list_empty(&blockchain_backend.list);
	return 0;
}
