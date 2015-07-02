/*
 * rf Class Core
 *
 * Copyright (C) 2005 John Lenz <lenz@cs.wisc.edu>
 * Copyright (C) 2005-2007 Richard Purdie <rpurdie@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/timer.h>

#include "rf.h"

static struct class *rf_class;

static ssize_t enabled_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct rf_classdev *rf_cdev = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", rf_cdev->enabled);
}

static ssize_t enabled_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct rf_classdev *rf_cdev = dev_get_drvdata(dev);
	bool state;
	ssize_t ret;

	mutex_lock(&rf_cdev->rf_access);

	ret = strtobool(buf, &state);
	if (ret)
		goto unlock;

	rf_set_enabled(rf_cdev, state);

	ret = size;
unlock:
	mutex_unlock(&rf_cdev->rf_access);
	return ret;
}
static DEVICE_ATTR_RW(enabled);

static ssize_t brightness_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct rf_classdev *rf_cdev = dev_get_drvdata(dev);
	unsigned int val;

	mutex_lock(&rf_cdev->rf_access);

	rf_get_brightness(rf_cdev, &val);

	mutex_unlock(&rf_cdev->rf_access);

	return sprintf(buf, "%u\n", val);
}
static DEVICE_ATTR_RO(brightness);

static ssize_t offset_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct rf_classdev *rf_cdev = dev_get_drvdata(dev);
	unsigned int val;

	mutex_lock(&rf_cdev->rf_access);

	rf_get_offset(rf_cdev, &val);

	mutex_unlock(&rf_cdev->rf_access);

	return sprintf(buf, "%u\n", val);
}
static DEVICE_ATTR_RO(offset);

static struct attribute *rf_class_attrs[] = {
	&dev_attr_enabled.attr,
	&dev_attr_brightness.attr,
	&dev_attr_offset.attr,
	NULL,
};

static const struct attribute_group rf_group = {
	.attrs = rf_class_attrs,
};

static const struct attribute_group *rf_groups[] = {
	&rf_group,
	NULL,
};

static int match_name(struct device *dev, const void *data)
{
	if (!dev_name(dev))
		return 0;
	return !strcmp(dev_name(dev), (char *)data);
}

static int rf_classdev_next_name(const char *init_name, char *name,
				  size_t len)
{
	unsigned int i = 0;
	int ret = 0;

	strlcpy(name, init_name, len);

	while (class_find_device(rf_class, NULL, name, match_name) &&
	       (ret < len))
		ret = snprintf(name, len, "%s_%u", init_name, ++i);

	if (ret >= len)
		return -ENOMEM;

	return i;
}

/**
 * rf_classdev_register - register a new object of rf_classdev class.
 * @parent: The device to register.
 * @rf_cdev: the rf_classdev structure for this device.
 */
int rf_classdev_register(struct device *parent, struct rf_classdev *rf_cdev)
{
	char name[64];
	int ret;

	ret = rf_classdev_next_name(rf_cdev->name, name, sizeof(name));
	if (ret < 0)
		return ret;

	rf_cdev->dev = device_create_with_groups(rf_class, parent, 0,
				rf_cdev, rf_cdev->groups, "%s", name);
	if (IS_ERR(rf_cdev->dev))
		return PTR_ERR(rf_cdev->dev);

	if (ret)
		dev_warn(parent, "range finder %s renamed to %s due to name collision",
				rf_cdev->name, dev_name(rf_cdev->dev));

	mutex_init(&rf_cdev->rf_access);
	/* add to the list of rf */
	down_write(&rf_list_lock);
	list_add_tail(&rf_cdev->node, &rf_list);
	up_write(&rf_list_lock);

	dev_info(parent, "Registered range finder device: %s\n",
			rf_cdev->name);

	return 0;
}
EXPORT_SYMBOL(rf_classdev_register);

/**
 * rf_classdev_unregister - unregisters a object of rf_properties class.
 * @rf_cdev: the rf device to unregister
 *
 * Unregisters a previously registered via rf_classdev_register object.
 */
void rf_classdev_unregister(struct rf_classdev *rf_cdev)
{
	device_unregister(rf_cdev->dev);

	down_write(&rf_list_lock);
	list_del(&rf_cdev->node);
	up_write(&rf_list_lock);

	mutex_destroy(&rf_cdev->rf_access);
}
EXPORT_SYMBOL(rf_classdev_unregister);

static void devm_rf_classdev_release(struct device *dev, void *res)
{
	rf_classdev_unregister(*(struct rf_classdev **)res);
}

/**
 * devm_rf_classdev_register - resource managed rf_classdev_register()
 * @parent: The device to register.
 * @rf_cdev: the rf_classdev structure for this device.
 */
int devm_rf_classdev_register(struct device *parent,
			      struct rf_classdev *rf_cdev)
{
	struct rf_classdev **dr;
	int rc;

	dr = devres_alloc(devm_rf_classdev_release, sizeof(*dr), GFP_KERNEL);
	if (!dr)
		return -ENOMEM;

	rc = rf_classdev_register(parent, rf_cdev);
	if (rc) {
		devres_free(dr);
		return rc;
	}

	*dr = rf_cdev;
	devres_add(parent, dr);

	return 0;
}
EXPORT_SYMBOL(devm_rf_classdev_register);

static int devm_rf_classdev_match(struct device *dev, void *res, void *data)
{
	struct rf_cdev **p = res;

	if (WARN_ON(!p || !*p))
		return 0;

	return *p == data;
}

/**
 * devm_rf_classdev_unregister() - resource managed rf_classdev_unregister()
 * @parent: The device to unregister.
 * @rf_cdev: the rf_classdev structure for this device.
 */
void devm_rf_classdev_unregister(struct device *dev,
				  struct rf_classdev *rf_cdev)
{
	WARN_ON(devres_release(dev,
			       devm_rf_classdev_release,
			       devm_rf_classdev_match, rf_cdev));
}
EXPORT_SYMBOL(devm_rf_classdev_unregister);

static int __init rf_init(void)
{
	rf_class = class_create(THIS_MODULE, "range_finders");
	if (IS_ERR(rf_class))
		return PTR_ERR(rf_class);
	rf_class->dev_groups = rf_groups;
	return 0;
}

static void __exit rf_exit(void)
{
	class_destroy(rf_class);
}

subsys_initcall(rf_init);
module_exit(rf_exit);

MODULE_AUTHOR("Michael Allwright");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Range Finder Class Interface");

