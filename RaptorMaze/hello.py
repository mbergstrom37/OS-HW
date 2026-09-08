kernel_module = open('/proc/helloworlddriver')

greeting = kernel_module.readline();
print(greeting)

kernel_module.close()
