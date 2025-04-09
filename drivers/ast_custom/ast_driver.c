#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/*dfnsjdhfndsf*/

MODULE_LICENSE("GPL");

static  char *whom = "world";
module_param(whom, charp, 0644);
MODULE_PARM_DESC(whom, "Recipient of the hello message");

static int howmany = 1;
module_param(howmany, int, 0644);
MODULE_PARM_DESC(howmany, "Number of greetings");

static int __init hello_init(void)
{
    int i;
    for(i = 0; i < howmany; i++)
    {
        pr_alert("Hello from AST Driver Init: %s %d\n", whom, i);
    }
    
    return 0;
}

static void __exit hello_exit(void)
{
    pr_alert("Goodbye from AST Driver Init: %s\n", whom);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_DESCRIPTION("Hello World Module");
MODULE_AUTHOR("Anton Stapenhorst");