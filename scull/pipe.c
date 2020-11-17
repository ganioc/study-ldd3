/*
*   pipe.c , fifo driver for scull
*
*/

#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h> /* printk(), min() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>

#include "proc_ops_version.h"

#include "scull.h"  /* local definitions */

struct scull_pipe {
	wait_queue_head_t inq, outq; /* read and write queues */
	char *buffer, *end;  /* begin of buf, end of buf */
	int buffersize;     /* used in pointer arithmetic */
	char *rp, *wp;      /* where to read, where to write */
	int nreaders, nwriters; /* number of openings for r/w */
	struct fasync_struct *async_queue; /* asynchronous readers */
	struct mutex lock;     /* mutual exclusion mutex */
	struct cdev cdev;      /* char device structure */
};

/* parameters */
static int scull_p_nr_devs = SCULL_P_NR_DEVS; /* number of pipe devices */
int scull_p_buffer = SCULL_P_BUFFER;  /* buffer size */
dev_t scull_p_devno;     /* Our first device number */

module_param(scull_p_nr_devs, int , 0); /* FIXME check perms */
module_param(scull_p_buffer, int, 0);

static struct scull_pipe *scull_p_devices;

static int scull_p_fasync(int fd, struct file *filp, int mode);
static int spacefree(struct scull_pipe *dev);

/*
 * Open and close
 */
static int scull_p_open(struct inode *inode, struct file *filp){
	struct scull_pipe *dev;
	dev = container_of(inode->i_cdev, struct scull_pipe, cdev);
	filp->private_data = dev;

	if( mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	if( !dev->buffer){
		/* allocate the buffer */
		dev->buffer = kmalloc(scull_p_buffer, GFP_KERNEL);
		if(!dev->buffer) {
			mutex_unlock(&dev->lock);
			return -ENOMEM;
		}
	}
	dev->buffersize = scull_p_buffer;
	dev->end = dev->buffer + dev->buffersize;
	dev->rp = dev->wp = dev->buffer; /* rd and wr from the beginning */

	/* use f_mode , not f_flags: it's cleaner (fs/open.c tells why) */
	if(filp->f_mode & FMODE_READ)
		dev->nreaders++;
	if(filp->f_mode & FMODE_WRITE)
		dev->nwriters++;
	mutex_unlock(&dev->lock);

	return nonseekable_open(inode,filp);
}

static int scull_p_release(struct inode *inode, struct file *filp){
	struct scull_pipe *dev = filp->private_data;
	
}
