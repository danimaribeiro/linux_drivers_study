#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>

//probe function
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

static struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
}

static int __init pen_init(void) {
	int ret = -1;
	printk(KERN_INFO "Registering driver with kernel\n");
	ret = usb_register(&pen_driver);
	printk(KERN_INFO "Registragion complete");
	return ret;
}

static void __exit pen_exit(void){
	usb_deregister(&pen_driver);
	printk(KERN_INFO "Unregistered the driver\n");
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danimar");
MODULE_DESCRIPTION("USB from Danimar");
