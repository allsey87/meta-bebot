/*
 * Rangefinder Class Core
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include "rf.h"

DECLARE_RWSEM(rf_list_lock);
EXPORT_SYMBOL(rf_list_lock);

LIST_HEAD(rf_list);
EXPORT_SYMBOL(rf_list);

void rf_set_enabled(struct rf_classdev *rf_cdev,
		 	bool enabled)
{
	int ret = 0;
	
	ret = rf_cdev->enabled_set(rf_cdev, enabled);
	
	if (ret < 0)
		dev_dbg(rf_cdev->dev, "Setting rangefinder enable failed (%d)\n",
			ret);
}
EXPORT_SYMBOL(rf_set_enabled);

void rf_get_enabled(struct rf_classdev *rf_cdev,
		 	bool *enabled)
{
	int ret = 0;
	
	ret = rf_cdev->enabled_get(rf_cdev, enabled);
	
	if (ret < 0)
		dev_dbg(rf_cdev->dev, "Getting rangefinder enable failed (%d)\n",
			ret);
}
EXPORT_SYMBOL(rf_get_enabled);

void rf_get_brightness(struct rf_classdev *rf_cdev,
		 	unsigned int *val)
{
	int ret = 0;
	
	ret = rf_cdev->brightness_get(rf_cdev, val);
	
	if (ret < 0)
		dev_dbg(rf_cdev->dev, "Getting rangefinder brightness failed (%d)\n",
			ret);
}
EXPORT_SYMBOL(rf_get_brightness);

void rf_get_offset(struct rf_classdev *rf_cdev,
		 	unsigned int *val)
{
	int ret = 0;
	
	ret = rf_cdev->offset_get(rf_cdev, val);
	
	if (ret < 0)
		dev_dbg(rf_cdev->dev, "Getting rangefinder offset failed (%d)\n",
			ret);
}
EXPORT_SYMBOL(rf_get_offset);

