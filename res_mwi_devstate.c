/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2014, Schmooze Com, Inc.
 *
 * Jason Parker <jason.parker@schmoozecom.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*** MODULEINFO
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision: $")

#include "asterisk/app.h"
#include "asterisk/devicestate.h"
#include "asterisk/module.h"
#include "asterisk/stasis_message_router.h"
#include "asterisk/stasis.h"

static struct stasis_subscription *mwi_sub;

static void mwi_update_cb(void *data, struct stasis_subscription *sub,
				    struct stasis_message *msg)
{
	struct ast_mwi_state *mwi_state;

	if (!stasis_subscription_is_subscribed(sub) || ast_mwi_state_type() != stasis_message_type(msg)) {
		return;
	}

	mwi_state = stasis_message_data(msg);
	if (!mwi_state) {
		return;
	}

	if (mwi_state->new_msgs > 0) {
		ast_debug(1, "Sending inuse devstate change for MWI:%s\n", mwi_state->uniqueid);
		ast_devstate_changed(AST_DEVICE_INUSE, AST_DEVSTATE_CACHABLE, "MWI:%s", mwi_state->uniqueid);
	} else {
		ast_debug(1, "Sending not inuse devstate change for MWI:%s\n", mwi_state->uniqueid);
		ast_devstate_changed(AST_DEVICE_NOT_INUSE, AST_DEVSTATE_CACHABLE, "MWI:%s", mwi_state->uniqueid);
	}
}

static int mwi_cached_cb(void *obj, void *arg, int flags)
{
	struct stasis_message *msg = obj;
	mwi_update_cb(NULL, mwi_sub, msg);

	return 0;
}

static int unload_module(void) {
	mwi_sub = stasis_unsubscribe(mwi_sub);

	return AST_MODULE_LOAD_SUCCESS;
}

static int load_module(void) {
	RAII_VAR(struct ao2_container *, cached, NULL, ao2_cleanup);

	if (!(mwi_sub = stasis_subscribe(ast_mwi_topic_all(), mwi_update_cb, NULL))) {
		return AST_MODULE_LOAD_DECLINE;
	}

	cached = stasis_cache_dump(ast_mwi_state_cache(), NULL);
	ao2_callback(cached, OBJ_NODATA, mwi_cached_cb, NULL);

	return AST_MODULE_LOAD_SUCCESS;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "MWI Device State Subscriptions",
		.load = load_module,
		.unload = unload_module,
		.load_pri = AST_MODPRI_DEVSTATE_PROVIDER,
	       );
