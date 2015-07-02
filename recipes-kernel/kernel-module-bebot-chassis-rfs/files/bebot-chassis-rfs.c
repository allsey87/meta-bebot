/*
 * Range finder Driver for the BeBot MID Case
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of_device.h>

#include "rf.h"

struct bebot_chassis_rfs_devtype {
	int nr_rfs;
};

static const struct bebot_chassis_rfs_devtype bebot_chassis_rfs_rev1_devtype = {
	.nr_rfs		= 6,
};

static const struct bebot_chassis_rfs_devtype bebot_chassis_rfs_rev2_devtype = {
	.nr_rfs		= 12,
};


static struct of_device_id bebot_chassis_rfs_dt_ids[] = {
	{ .compatible = "upb,bebot-chassis-rfs", .data = &bebot_chassis_rfs_rev2_devtype, },
	{ }
};
MODULE_DEVICE_TABLE(of, bebot_chassis_rfs_dt_ids);

static const struct i2c_device_id bebot_chassis_rfs_i2c_id_table[] = {
	{ "bebot-chassis-rfs",	(kernel_ulong_t)&bebot_chassis_rfs_rev2_devtype, },
	{ }
};
MODULE_DEVICE_TABLE(i2c, bebot_chassis_rfs_i2c_id_table);

struct bebot_rf {
	struct rf_classdev rf_cdev;
	int rf_num; /* 0 .. 11 potentially */
	bool registered;
	char name[32];
};

struct bebot_chassis_rfs {
	int rf_num;
	char name[32];
	struct bebot_rf *rfs;
};

static int bebot_chassis_rfs_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct bebot_chassis_rfs_devtype *devtype;
	struct device_node *np = client->dev.of_node, *child;
	struct bebot_chassis_rfs *bc_rfs;
	struct bebot_rf *rfs;
	int res, i;

	if(!client->dev.of_node) {
		dev_err(&client->dev, "could not find device tree node\n");
		return -ENODEV;
	} else {
		const struct of_device_id *of_id =
			of_match_device(bebot_chassis_rfs_dt_ids, &client->dev);
	
		devtype = (struct bebot_chassis_rfs_devtype *)of_id->data;

		dev_info(&client->dev, "Found %d range finders\n", devtype->nr_rfs);
	}

	bc_rfs = devm_kzalloc(&client->dev, sizeof(*bc_rfs), GFP_KERNEL);
	if (!bc_rfs)
		return -ENOMEM;

	rfs = devm_kzalloc(&client->dev, devtype->nr_rfs * sizeof(*rfs), GFP_KERNEL);
	if (!rfs)
		return -ENOMEM;

	bc_rfs->rfs = rfs;

	for_each_child_of_node(np, child) {
		u32 reg;
		int res;
		res = of_property_read_u32(child, "reg", &reg);
		if ((res != 0) || (reg >= devtype->nr_rfs))
			continue;

		rfs[reg].rf_cdev.name = of_get_property(child, "label", NULL) ? : child->name;
	 	res = rf_classdev_register(&client->dev, &rfs[reg].rf_cdev);
		if(res != 0) {
			goto exit;
		} else {
			rfs[reg].registered = true;
		}
	}
	return 0;
exit:
	for (i = 0; i < devtype->nr_rfs; i++) {
		if(rfs[i].registered)
			rf_classdev_unregister(&rfs[i].rf_cdev);
	}

	return res;
}

static int bebot_chassis_rfs_remove(struct i2c_client *client)
{
	return 0;
}

static struct i2c_driver bebot_chassis_rfs_i2c_driver = {
	.driver = {
		.name 		= "bebot-chassis-rfs",
		.owner		= THIS_MODULE,
		.of_match_table = of_match_ptr(bebot_chassis_rfs_dt_ids),
	},
	.probe		= bebot_chassis_rfs_probe,
	.remove		= bebot_chassis_rfs_remove,
	.id_table	= bebot_chassis_rfs_i2c_id_table,
};

module_i2c_driver(bebot_chassis_rfs_i2c_driver);

MODULE_ALIAS("i2c:bebot-chassis-rfs");
MODULE_DESCRIPTION("Range finder driver for the BeBot MID Chassis");
MODULE_AUTHOR("Michael Allwright <allsey87@gmail.com>");
MODULE_LICENSE("GPL");
