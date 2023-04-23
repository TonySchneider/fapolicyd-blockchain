/*
 * blockchain-backend.c - file backend
 * Copyright (c) 2020 Red Hat Inc.
 * All Rights Reserved.
 *
 * This software may be freely redistributed and/or modified under the
 * terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1335, USA.
 *
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
