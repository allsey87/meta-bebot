#ifndef _MEDIA_OV5640_H
#define _MEDIA_OV5640_H

#include <media/v4l2-subdev.h>

struct ov5640_platform_data {
	const char *reg_avdd; /* Analog power regulator id */
	const char *reg_dovdd; /* I/O power regulator id */

	int (*pre_poweron)(struct v4l2_subdev *subdev);
	int (*post_poweroff)(struct v4l2_subdev *subdev);
};

#endif
