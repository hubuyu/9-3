#include <linux/input.h> 
#include <linux/module.h> 
#include <linux/init.h> 
#include <linux/interrupt.h> 
#include <linux/gpio.h> 
#include <linux/irq.h> 

#define PIN		25	
#define BUTTON_IRQ	gpio_to_irq(PIN)

static struct input_dev *button_dev; 


static irqreturn_t button_interrupt(int irq, void *dummy) 
{	
	int data ;
	data = gpio_get_value(PIN);
	printk("test irq data-%d\n", data); 
	input_report_key(button_dev, BTN_0, data); 
	input_sync(button_dev); 
	return IRQ_HANDLED; 
} 


static int __init button_init(void) 
{ 
	int error; 

	if (request_irq(BUTTON_IRQ, button_interrupt, IRQ_TYPE_EDGE_BOTH , "button", NULL)) { 
//	if (request_irq(BUTTON_IRQ, button_interrupt, IRQ_TYPE_EDGE_FALLING , "button", NULL)) { 
		printk(KERN_ERR "button.c: Can't allocate irq %d\n", BUTTON_IRQ); 
        return -EBUSY; 
    } 

	button_dev = input_allocate_device(); 
	if (!button_dev) { 
		printk(KERN_ERR "button.c: Not enough memory\n"); 
		error = -ENOMEM; 
		goto err_free_irq; 
	} 

	button_dev->evbit[0] = BIT_MASK(EV_KEY); 
	button_dev->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0); 
	button_dev->name = "button"; //input: Unspecified device as /devices/virtual/input/input7
	error = input_register_device(button_dev); 
	if (error) { 
		printk(KERN_ERR "button.c: Failed to register device\n"); 
		goto err_free_dev; 
	} 

 return 0; 

 err_free_dev: 
	input_free_device(button_dev); 
 err_free_irq: 
	free_irq(BUTTON_IRQ, button_interrupt); 
 return error; 
} 


static void __exit button_exit(void) 
{ 
    input_unregister_device(button_dev); 
	free_irq(BUTTON_IRQ, NULL); 
} 


module_init(button_init); 
module_exit(button_exit); 

MODULE_AUTHOR("hubin");
MODULE_LICENSE("GPL");

