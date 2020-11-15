/*
  the bare scull char module

 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>  /* kmalloc() */
#include <linux/fs.h>  /* everything about file system */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>  /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <linux/uaccess.h>  /* copy_*_user */

#include "scull.h"  /* local definitions */
#include "access_ok_version.h"
#include "proc_ops_version.h"

/*
 *  Our parameters which can be set at load time.
 */

int scull_major = SCULL_MAJOR;
