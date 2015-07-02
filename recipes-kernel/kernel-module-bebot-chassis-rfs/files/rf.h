/*
 * Rangefinder Core
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __RF_H_INCLUDED
#define __RF_H_INCLUDED

#include <linux/device.h>
#include <linux/rwsem.h>

struct rf_classdev {
	const char		*name;
	bool			enabled;

	/* Set RF enabled */
	int	(*enabled_set)(struct rf_classdev *rf_cdev, bool enabled);
	
	/* Get RF enabled */
	int 	(*enabled_get)(struct rf_classdev *rf_cdev, bool *enabled);

	/* Get RF brightness value */
	int 	(*brightness_get)(struct rf_classdev *rf_cdev, unsigned int *val);

	/* Get RF offset value */
	int 	(*offset_get)(struct rf_classdev *rf_cdev, unsigned int *val);

	struct device		*dev;
	const struct attribute_group	**groups;

	/* Rangefinder Device list */
	struct list_head	 node;

	/* Ensures consistent access to the Rangefinder Class device */
	struct mutex		rf_access;
};

extern int rf_classdev_register(struct device *parent,
				 struct rf_classdev *rf_cdev);
extern int devm_rf_classdev_register(struct device *parent,
				      struct rf_classdev *rf_cdev);
extern void rf_classdev_unregister(struct rf_classdev *rf_cdev);
extern void devm_rf_classdev_unregister(struct device *parent,
					 struct rf_classdev *rf_cdev);

extern void rf_set_enabled(struct rf_classdev *rf_cdev, bool enabled);
extern void rf_get_enabled(struct rf_classdev *rf_cdev, bool *enabled);
extern void rf_get_brightness(struct rf_classdev *rf_cdev, unsigned int *val);
extern void rf_get_offset(struct rf_classdev *rf_cdev, unsigned int *val);

extern struct rw_semaphore rf_list_lock;
extern struct list_head rf_list;

#endif	/* __RF_H_INCLUDED */

