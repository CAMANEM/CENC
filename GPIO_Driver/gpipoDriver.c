#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("pipo_OS");
MODULE_DESCRIPTION("A gpio driver for CNC like machine for placing SMD");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "gpipoDriver"
#define DRIVER_CLASS "MyModuleClass"

/*_______________________________GPIO DEFINITIONS____________________________________*/

// 4, 17, 27, 22

#define Stepper1In_1 516
#define Stepper1In_2 529
#define Stepper1In_3 539
#define Stepper1In_4 534

// 5, 6, 13, 19

#define Stepper2In_1 517
#define Stepper2In_2 518
#define Stepper2In_3 525
#define Stepper2In_4 531

// 26, 21, 20, 16
// deberia ser 16, 20, 21, 26
/*
#define Stepper3In_1 538
#define Stepper3In_2 533
#define Stepper3In_3 532
#define Stepper3In_4 528
*/
#define Stepper3In_1 528 // GPIO16
#define Stepper3In_2 532 // GPIO20
#define Stepper3In_3 533 // GPIO21
#define Stepper3In_4 538 // GPIO26

// 23

#define Picker 535

/**
 * @brief Read data out of the buffer
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char tmp[3] = " \n";

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	/* Read value of button */
	printk("Value of button: %d\n", gpio_get_value(Picker));
	tmp[0] = gpio_get_value(Picker) + '0';

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, &tmp, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	/* Setting the LED */
	switch(value) {
		case '0':
		//stepper1
			gpio_set_value(Stepper1In_1, 0);
			gpio_set_value(Stepper1In_2, 0);
			gpio_set_value(Stepper1In_3, 0);
			gpio_set_value(Stepper1In_4, 0);
			
		//stepper2
			gpio_set_value(Stepper2In_1, 0);
			gpio_set_value(Stepper2In_2, 0);
			gpio_set_value(Stepper2In_3, 0);
			gpio_set_value(Stepper2In_4, 0);
			
		//stepper3
			gpio_set_value(Stepper3In_1, 0);
			gpio_set_value(Stepper3In_2, 0);
			gpio_set_value(Stepper3In_3, 0);
			gpio_set_value(Stepper3In_4, 0);
			msleep(10);
			break;
		case '1':
			gpio_set_value(Stepper1In_1, 1);
			gpio_set_value(Stepper1In_2, 0);
			gpio_set_value(Stepper1In_3, 1);
			gpio_set_value(Stepper1In_4, 0);
			msleep(10);
			break;
		case '2':
			gpio_set_value(Stepper1In_1, 0);
			gpio_set_value(Stepper1In_2, 1);
			gpio_set_value(Stepper1In_3, 1);
			gpio_set_value(Stepper1In_4, 0);
			msleep(10);
			break;
		case '3':
			gpio_set_value(Stepper1In_1, 0);
			gpio_set_value(Stepper1In_2, 1);
			gpio_set_value(Stepper1In_3, 0);
			gpio_set_value(Stepper1In_4, 1);
			msleep(10);
			break;
		case '4':
			gpio_set_value(Stepper1In_1, 1);
			gpio_set_value(Stepper1In_2, 0);
			gpio_set_value(Stepper1In_3, 0);
			gpio_set_value(Stepper1In_4, 1);
			msleep(10);
			break;
		case '5':
			gpio_set_value(Stepper2In_1, 1);
			gpio_set_value(Stepper2In_2, 0);
			gpio_set_value(Stepper2In_3, 1);
			gpio_set_value(Stepper2In_4, 0);
			msleep(20);
			break;
		case '6':
			gpio_set_value(Stepper2In_1, 0);
			gpio_set_value(Stepper2In_2, 1);
			gpio_set_value(Stepper2In_3, 1);
			gpio_set_value(Stepper2In_4, 0);
			msleep(20);
			break;
		case '7':
			gpio_set_value(Stepper2In_1, 0);
			gpio_set_value(Stepper2In_2, 1);
			gpio_set_value(Stepper2In_3, 0);
			gpio_set_value(Stepper2In_4, 1);
			msleep(20);
			break;
		case '8':
			gpio_set_value(Stepper2In_1, 1);
			gpio_set_value(Stepper2In_2, 0);
			gpio_set_value(Stepper2In_3, 0);
			gpio_set_value(Stepper2In_4, 1);
			msleep(20);
			break;
		case '9':
			gpio_set_value(Stepper3In_1, 1);
			gpio_set_value(Stepper3In_2, 0);
			gpio_set_value(Stepper3In_3, 1);
			gpio_set_value(Stepper3In_4, 0);
			msleep(5);
			break;
		case 'a':
			gpio_set_value(Stepper3In_1, 0);
			gpio_set_value(Stepper3In_2, 1);
			gpio_set_value(Stepper3In_3, 1);
			gpio_set_value(Stepper3In_4, 0);
			msleep(5);
			break;
		case 'b':
			gpio_set_value(Stepper3In_1, 0);
			gpio_set_value(Stepper3In_2, 1);
			gpio_set_value(Stepper3In_3, 0);
			gpio_set_value(Stepper3In_4, 1);
			msleep(5);
			break;
		case 'c':
			gpio_set_value(Stepper3In_1, 1);
			gpio_set_value(Stepper3In_2, 0);
			gpio_set_value(Stepper3In_3, 0);
			gpio_set_value(Stepper3In_4, 1);
			msleep(5);
			break;
		
		case 'd':
			gpio_set_value(Picker, 0);
			msleep(10);
			break;
			
		case 'e':
			gpio_set_value(Picker, 1);
			msleep(10);
			break;
		
		default:
			printk("Invalid Input!\n");
			break;
	}

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create( DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

/*_______________________________GPIO REQUESTS_______________________________________*/


	/* GPIO Stepper1In_1 init */
	if(gpio_request(Stepper1In_1, "rpi-gpio-Stepper1In_1")) {
		printk("Can not allocate GPIO Stepper1In_1\n");
		goto AddError;
	}
	/* Set GPIO Stepper1In_1 direction */
	if(gpio_direction_output(Stepper1In_1, 0)) {
		printk("Can not set GPIO Stepper1In_1 to output!\n");
		goto GpioStepper1In_1Error;
	}

	/* GPIO Stepper1In_2 init */
	if(gpio_request(Stepper1In_2, "rpi-gpio-Stepper1In_2")) {
		printk("Can not allocate GPIO Stepper1In_2\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper1In_2, 0)) {
		printk("Can not set GPIO Stepper1In_2 to output!\n");
		goto GpioStepper1In_2Error;
	}

	/* GPIO Stepper1In_3 init */
	if(gpio_request(Stepper1In_3, "rpi-gpio-Stepper1In_3")) {
		printk("Can not allocate GPIO Stepper1In_3\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper1In_3, 0)) {
		printk("Can not set GPIO Stepper1In_3 to output!\n");
		goto GpioStepper1In_3Error;
	}

	/* GPIO Stepper1In_4 init */
	if(gpio_request(Stepper1In_4, "rpi-gpio-Stepper1In_4")) {
		printk("Can not allocate GPIO Stepper1In_4\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper1In_4, 0)) {
		printk("Can not set GPIO Stepper1In_4 to output!\n");
		goto GpioStepper1In_4Error;
	}

	/* GPIO Stepper2In_1 init */
	if(gpio_request(Stepper2In_1, "rpi-gpio-Stepper2In_1")) {
		printk("Can not allocate GPIO Stepper2In_1\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper2In_1, 0)) {
		printk("Can not set GPIO Stepper2In_1 to output!\n");
		goto GpioStepper2In_1Error;
	}

	/* GPIO Stepper2In_2 init */
	if(gpio_request(Stepper2In_2, "rpi-gpio-Stepper2In_2")) {
		printk("Can not allocate GPIO Stepper2In_2\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper2In_2, 0)) {
		printk("Can not set GPIO Stepper2In_2 to output!\n");
		goto GpioStepper2In_2Error;
	}

	/* GPIO Stepper2In_3 init */
	if(gpio_request(Stepper2In_3, "rpi-gpio-Stepper2In_3")) {
		printk("Can not allocate GPIO Stepper2In_3\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper2In_3, 0)) {
		printk("Can not set GPIO Stepper2In_3 to output!\n");
		goto GpioStepper2In_3Error;
	}

	/* GPIO Stepper2In_4 init */
	if(gpio_request(Stepper2In_4, "rpi-gpio-Stepper2In_4")) {
		printk("Can not allocate GPIO Stepper2In_4\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper2In_4, 0)) {
		printk("Can not set GPIO Stepper2In_4 to output!\n");
		goto GpioStepper2In_4Error;
	}

	/* GPIO Stepper3In_1 init */
	if(gpio_request(Stepper3In_1, "rpi-gpio-Stepper3In_1")) {
		printk("Can not allocate GPIO Stepper3In_1\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper3In_1, 0)) {
		printk("Can not set GPIO Stepper3In_1 to output!\n");
		goto GpioStepper3In_1Error;
	}

	/* GPIO Stepper3In_2 init */
	if(gpio_request(Stepper3In_2, "rpi-gpio-Stepper3In_2")) {
		printk("Can not allocate GPIO Stepper3In_2\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper3In_2, 0)) {
		printk("Can not set GPIO Stepper3In_2 to output!\n");
		goto GpioStepper3In_2Error;
	}

	/* GPIO Stepper3In_3 init */
	if(gpio_request(Stepper3In_3, "rpi-gpio-Stepper3In_3")) {
		printk("Can not allocate GPIO Stepper3In_3\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper3In_3, 0)) {
		printk("Can not set GPIO Stepper3In_3 to output!\n");
		goto GpioStepper3In_3Error;
	}

	/* GPIO Stepper3In_4 init */
	if(gpio_request(Stepper3In_4, "rpi-gpio-Stepper3In_4")) {
		printk("Can not allocate GPIO Stepper3In_4\n");
		goto AddError;
	}
	if(gpio_direction_output(Stepper3In_4, 0)) {
		printk("Can not set GPIO Stepper3In_4 to output!\n");
		goto GpioStepper3In_4Error;
	}


	/* GPIO Picker init */
	if(gpio_request(Picker, "rpi-gpio-Picker")) {
		printk("Can not allocate GPIO Picker\n");
		goto AddError;
	}

	/* Set GPIO Picker direction */
	if(gpio_direction_output(Picker, 0)) {
		printk("Can not set GPIO Picker to output!\n");
		goto GpioPickerError;
	}



	return 0;
	
	
/*___________________________GPIO REQUEST ERROR HANDLER_____________________________*/
	
GpioStepper1In_1Error: 
	gpio_free(Stepper1In_1);

GpioStepper1In_2Error:
	gpio_free(Stepper1In_2);

GpioStepper1In_3Error:
	gpio_free(Stepper1In_3);

GpioStepper1In_4Error:
	gpio_free(Stepper1In_4);

GpioStepper2In_1Error:
	gpio_free(Stepper2In_1);

GpioStepper2In_2Error:
	gpio_free(Stepper2In_2);

GpioStepper2In_3Error:
	gpio_free(Stepper2In_3);

GpioStepper2In_4Error:
	gpio_free(Stepper2In_4);

GpioStepper3In_1Error:
	gpio_free(Stepper3In_1);

GpioStepper3In_2Error:
	gpio_free(Stepper3In_2);

GpioStepper3In_3Error:
	gpio_free(Stepper3In_3);

GpioStepper3In_4Error:
	gpio_free(Stepper3In_4);

GpioPickerError:
	gpio_free(Picker);

	
/*_______________________________GPIO REQUESTS_______________________________________*/

AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
	
	
	
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	gpio_set_value(4, 0);
	
	
	gpio_free(Stepper1In_1);
	gpio_free(Stepper1In_2);
	gpio_free(Stepper1In_3);
	gpio_free(Stepper1In_4);
	
	gpio_free(Stepper2In_1);
	gpio_free(Stepper2In_2);
	gpio_free(Stepper2In_3);
	gpio_free(Stepper2In_4);
	
	gpio_free(Stepper3In_1);
	gpio_free(Stepper3In_2);
	gpio_free(Stepper3In_3);
	gpio_free(Stepper3In_4);
	
	gpio_free(Picker);
	
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


