kernel_module = open('/proc/raptormazedriver')

greeting = kernel_module.read(75);
print(greeting)

kernel_module.close()
