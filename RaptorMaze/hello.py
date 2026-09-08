kernel_module = open('/proc/raptormazedriver')

greeting = kernel_module.readline();
print(greeting)

kernel_module.close()
