/*
* Простейший модуль ядра
*/
#include <linux/module.h>
#include <linux/kernel.h>
//#include <linux/init.h>

#define DRIVER_AUTHOR "Alex K <aleks.kryp@mail.ru>"
#define DRIVER_DESC "A sample driver"

int init_module(void){
    pr_info("Hello world!!!\n");
    return 0;
}

void cleanup_module(void){
    pr_info("Goodbye world!!!\n");
}

//module_init(init_module);
//module_exit(cleanup_module);
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");