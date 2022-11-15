# CH58x-CMAKE-Clion-Template

## FreeRTOS
FreeRTOS 需要注意以下几点：

1. 配置文件路径为工程目录下 `FreeRTOS/portable/FreeRTOSConfig.h`
2. 本移植例程默认使用了硬件压栈（不可关闭），未使用中断嵌套，中断函数最好使用 `__attribute__((section(".highcode"))) `修饰，保证运行速度
    > 我猜测任务栈和中断栈是用的一个，假如中断没有及时出栈，可能会影响到任务栈的运行。`__attribute__((section(".highcode")))` 这个修饰符是为了让代码运行在 RAM 里面，让中断更快运行完成尽快出栈。目前建议尽量少用中断。
3. 使能中断嵌套会导致每个中断执行会多约 8 个指令周期，中断嵌套使能通过工程右键 -> properties -> c/c++ Build -> settings -> tool settings -> GNU RISC-V Cross Assembler -> Preprocessor 右边输入框Defined symbols中的 ENABLE_INTERRUPT_NEST=0 修改为 ENABLE_INTERRUPT_NEST=1 即可。
    > 对于 CMakeLists 而言，加入 `add_definitions(-DENABLE_INTERRUPT_NEST=1)` 就好
4. 用户中断函数不需要再使用 `__attribute__((interrupt("WCH-Interrupt-fast")))` 或者 `__attribute__((interrupt()))` 修饰，中断入口已统一为汇编函数，在汇编函数中调用用户中断函数。库自带的 `HardFault` 中断除外，用户只需要修改自己编写的中断函数即可。
5. CH583 系列上电运行默认的栈为编译后剩余的 RAM 空间。所有统一入口的中断会把栈修改为 LD 文件中提供的 `__freertos_irq_stack_top`，所以中断中可以使用的最大栈空间为 RAM 剩余空间。所以请一定要预留 RAM 空间给栈使用。
6. 中断中使用了 CSR 寄存器 mscratch 作为 sp 指针的临时寄存器，所以用户不可以再自行使用 mscratch 寄存器。
7. 不建议使用蓝牙，使用蓝牙的话可以创建一个优先级仅比 `IDLE TASK` 高一级的任务，循环运行 `TMOS_SystemProcess`，不主动退出任务，由其他更高优先级任务抢断。由于蓝牙中断为库内中断函数，使用了免表中断方式，所以无法改为统一栈，所以如果使用蓝牙的话，每个任务栈需要额外加上蓝牙中断中使用的大小，约128字节。
8. 本工程的 `Startup.S` 文件和 Ld 文件都是独立的，用户中断向量表可见工程目录下 `FreeRTOS/portable/port.c`，用户中断总入口 `user_interrupt_handler` 和 `SysTick_Handler` 就在该文件中。
9. FreeRTOS 任务中函数的打印最好使用 `main.c` 中提供的 `APP_Printf` 进行，不然可能会导致 `HardFault`。

