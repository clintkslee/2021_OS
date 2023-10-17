#include <linux/kernel.h>
#include <linux/syscalls.h>

asmlinkage long sys_my_mul(long num1, long num2)
{
	printk("sys_my_mul CALLED, %ld * %ld\n", num1, num2);
	return (num1*num2);
}

SYSCALL_DEFINE2(my_mul, long, num1, long, num2)
{
	return sys_my_mul(num1, num2);
}
