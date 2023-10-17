#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage long sys_my_sub(long num1, long num2)
{
	printk("sys_my_sub CALLED, %ld - %ld\n", num1, num2);
	return (num1-num2);
}

SYSCALL_DEFINE2(my_sub, long, num1, long, num2)
{
	return sys_my_sub(num1, num2);
}
