#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage long sys_my_add(long num1, long num2)
{
	printk("sys_my_add CALLED, %ld + %ld\n", num1, num2);
	return (num1+num2);
}

SYSCALL_DEFINE2(my_add, long, num1, long, num2)
{
	return sys_my_add(num1, num2);
}
