#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <asm/uaccess.h>


unsigned long *sys_call_table = (unsigned long*) 0xffffffff81801540;
//hard coded address... 64 bits are very big



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jody");
MODULE_DESCRIPTION("Sys-Open Hook");

asmlinkage int (*getuid_call)(void);

asmlinkage int (*original_call) (const char* __user, int, int);
//store our old call so we can still call it

asmlinkage int our_sys_open(const char* __user filename, int flags, int mode)
{
	
	printk("sys-open HAS BEEN CALLED for FILE:%s by UserID:%d", filename,getuid_call());
	
		

	//sys-open is still at old pointer, so let's call it
	return original_call(filename, flags, mode);
}

int make_rw(unsigned long address)
{
    //read write perms set
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    if(pte->pte &~ _PAGE_RW)
       pte->pte |= _PAGE_RW;
    return 0;
}

int make_ro(unsigned long address)
{
    //read only
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte = pte->pte &~ _PAGE_RW;
    return 0;
} 


static int __init hook_init(void)
{
	make_rw((unsigned long)sys_call_table);
	printk(KERN_INFO "Starting to Hook...\n");
	
	
   	original_call = (void*)*(sys_call_table + __NR_open);
	*(sys_call_table + __NR_open) = (unsigned long)our_sys_open;

	getuid_call = (void*)*(sys_call_table + __NR_getuid);

	make_ro((unsigned long)sys_call_table);


	printk(KERN_INFO "Sys-Open Hook Started\n");

	return 0;
}

static void __exit hook_cleanup(void)
{
    make_rw((unsigned long)sys_call_table);
    *(sys_call_table + __NR_open) = (unsigned long)original_call;
    make_ro((unsigned long)sys_call_table);
    printk("Exiting Hook and Registering Original Sys-Call");
}



module_init(hook_init);
module_exit(hook_cleanup);
