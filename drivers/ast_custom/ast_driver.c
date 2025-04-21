#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/i2c.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Stapenhorst");
MODULE_DESCRIPTION("This driver controls the LP55231 LED module");

static struct i2c_client *led_client;

/* Probe and remove functions */
static int led_probe(struct i2c_client *client);
static void led_remove(struct i2c_client *client);


static struct of_device_id my_of_led_ids[] = {
    {
        .compatible = "ti,lp55231",
    },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_of_led_ids);


static struct i2c_device_id my_led_ids[] = {
    {"my_led", 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, my_led_ids);

static struct i2c_driver led_driver = {
    .probe = led_probe,
    .remove = led_remove,
    .id_table = my_led_ids,
    .driver = {
        .name = "my_led",
        .of_match_table = my_of_led_ids,
    },
};

static struct proc_dir_entry *proc_file;

static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	long val;
	if(0 == kstrtol(user_buffer, 0, &val))
    {

    }
	return count;
}

/**
 * @brief Read ADC value
 */
static ssize_t my_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{
	return count;
}

static struct proc_ops fops = {
	.proc_write = my_write,
};

static int enable_all_leds(struct i2c_client *client)
{
    struct i2c_msg msg;
    int error;

    u8 buf[2];

    /* First, enable the IC */
    buf[0] = 0x00;
    buf[1] = 0x40;
    msg.addr = client->addr;
    msg.flags = 0;
    msg.len = 2;
    msg.buf = buf;

    error = i2c_transfer(client->adapter, &msg, 1);

    if(error < 0)
    {
        return error;
    }

    /* Enable the Charge Pump */
    buf[0] = 0x36;
    buf[1] = 0x5B;

    error = i2c_transfer(client->adapter, &msg, 1);

    if(error < 0)
    {
        return error;
    }

    buf[1] = 0x80;
    for(u32 i = 0; i < 9; i++)
    {
        buf[0] = 0x16 + i;

        error = i2c_transfer(client->adapter, &msg, 1);

        if(error < 0)
        {
            return error;
        }
        
    }
    
    return 0;
}

static int led_probe(struct i2c_client *client)
{
    printk("Probing I2C LED device LP55231...\n");
    
    if(client->addr != 0x32)
    {
        printk("Got wrong I2C address... \n");
        return -1;
    }

    led_client = client;

    /* Enable all LEDs */
    int error = enable_all_leds(client);

    if(error < 0)
    {
        printk("LP55231 - Failed to do init sequence: %u\n", error);
        return -1;
    }

    /* Creating procfs file */
	proc_file = proc_create("myled", 0666, NULL, &fops);
	if(proc_file == NULL) {
		printk("LED driver - Error creating /proc/myadc\n");
		return -ENOMEM;
	}

    return 0;


}

static void led_remove(struct i2c_client *client)
{
    printk("Unloading I2C LED device LP55231...\n");
    proc_remove(proc_file);
}

module_i2c_driver(led_driver);