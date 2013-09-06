#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>

#define SCULL_MAJOR 0

int scull_major = SCULL_MAJOR;

//called on device insertion if and only no other driver has got first
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id){
	printk(KERN_INFO "Danimar Pen Drive plugged in\n");
	return 0; //indicates we will manage this device
}

static void pen_disconnect(struct usb_interface *interface){
	printk(KERN_INFO "Danimar Pen Drive removed\n");
}

//usb_device_id
static struct usb_device_id pen_table[] = {
	//id
	{ USB_DEVICE(0x80ee, 0x0021)},
	{}
};
MODULE_DEVICE_TABLE(usb, pen_table);

//usb driver
static struct usb_driver pen_driver = {
	.name = "Danimar USB Driver",
	.id_table = pen_table, //usb_device_id
	.probe = pen_probe,
	.disconnect = pen_disconnect
};

static void *scull_seq_start(struct seq_file *s, loff_t *pos)
{
	if (*pos >= scull_nr_devs)
		return NULL; /* No more to read */
	return scull_devices + *pos;
}

static void *scull_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	if (*pos >= scull_nr_devs)
		return NULL;
	return scull_devices + *pos;
}

static void scull_seq_stop(struct seq_file *sfile, void *v)
{
}

static int scull_seq_show(struct seq_file *s, void *v)
{
	struct scull_dev *dev = (struct scull_dev *) v;
	struct scull_qset *d;
	int i;
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	seq_printf(s, "\nDevice %i: qset %i, q %i, sz %li\n", (int) (dev - scull_devices), dev->qset,	dev->quantum, dev->size);

	for (d = dev->data; d; d = d->next) { /* scan the list */

		seq_printf(s, " item at %p, qset at %p\n", d, d->data);
		if (d->data && !d->next) /* dump only the last item */
			for (i = 0; i < dev->qset; i++) {
			if (d->data[i])
				seq_printf(s, " % 4i: %8p\n", i, d->data[i]);
		}
	}
	up(&dev->sem);
	return 0;
}

static struct seq_operations scull_seq_ops = {
	.start = scull_seq_start,
	.next = scull_seq_next,
	.stop = scull_seq_stop,
	.show = scull_seq_show
};

static int scull_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &scull_seq_ops);
}

static struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.ioctl = scull_ioctl,
	.open = scull_proc_open,
	.release = scull_release,
}

static int __init scull_init(void) {
    int result, i;
    dev_t dev = 0;
	/*
	* Get a range of minor numbers to work with, asking for a dynamic
	* major unless directed otherwise at load time.
	*/
    printk(KERN_INFO "Getting \n");
    if (scull_major) {
            dev = MKDEV(scull_major, scull_minor);
            result = register_chrdev_region(dev, scull_nr_devs, "scull");
    } else {
            result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
                            "scull");
            scull_major = MAJOR(dev);
    }
    if (result < 0) {
            printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
            return result;
    }


	printk(KERN_INFO "Registragion complete");
	return result;
}

static void __exit scull_exit(void){
	printk(KERN_INFO "Unregistered the driver\n");
}

module_init(scull_init);
module_exit(scull_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danimar");
MODULE_DESCRIPTION("USB from Danimar");
