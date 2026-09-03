# ENGG1100 双绞盘控制器

适用硬件：

- Arduino UNO R4 Minima
- Keyestudio KS0063 / Jaycar XC4492 L298N 双路直流电机驱动板
- 三脚单刀双掷、中心断开的 SPDT ON-OFF-ON（I-O-II）摇臂开关
- 两台直流减速马达

## 功能

| 开关位置 | 动作 |
|---|---|
| I | 两台马达放绳 |
| O | 两台马达停止 |
| II | 两台马达收绳 |

程序还会进行 35 ms 开关消抖，并在换向前停止 150 ms。上电、开关断线以及两个输入同时有效时均停止马达。

## 控制信号接线

| Arduino UNO R4 | L298N / 开关 |
|---|---|
| D5 | IN1 |
| D6 | IN2 |
| D7 | IN3 |
| D8 | IN4 |
| D9 | ENA |
| D10 | ENB |
| D2 | 开关 I 挡触点 |
| D3 | 开关 II 挡触点 |
| GND | 开关公共端、L298N GND |
| 5V | L298N +5V 逻辑电源（见下方电源要求） |

根据背面照片，三个焊片横向排列，中间焊片是公共端，两个外侧焊片是 I/II 挡触点：

1. 中间焊片接 Arduino GND。
2. I 挡对应的外侧焊片接 D2。
3. II 挡对应的外侧焊片接 D3。
4. O 挡时，中间焊片与两个外侧焊片均不导通。

从背面观看时左右会相对正面镜像，而且有些摇臂开关的内部触点动作方向与按压侧相反。焊接前请断电用万用表通断挡确认两个外侧焊片；如果实测 I/II 功能颠倒，直接交换 D2、D3 两根线即可。程序使用 Arduino 内部上拉电阻，不需要额外上拉或下拉电阻。

## 马达与电源接线

1. 马达 A 接驱动板左侧马达输出，马达 B 接右侧马达输出。
2. 马达电源正极接 `VMS`，负极接 `GND`；马达电源不能从 Arduino 的 5V 引脚获取。
3. Arduino GND 与 L298N GND 必须共地。
4. 推荐拔下驱动板的 `5V-EN` 跳帽，再由 Arduino `5V` 给 L298N 的 `+5V` 逻辑端供电。不要让板载 78M05 和 Arduino 5V 同时反向供电。
5. 如果保留 `5V-EN` 并用板载稳压器，就不要再把驱动板 `+5V` 接到 Arduino `5V`；两块板仍须共地。

## 首次测试

1. 先拆下绳子或让绞盘完全无负载，并将 `MOTOR_PWM` 保持为 180。
2. 上传 `ENGG1100_Arduino_Winch.ino`，打开 115200 波特率串口监视器。
3. 检查 I/O/II 三挡及两台马达方向。
4. 如果只有一台马达方向相反，修改对应的 `MOTOR_A_REVERSED` 或 `MOTOR_B_REVERSED`，不要同时修改两处。
5. 确认方向后再装绳，并逐步调整 `MOTOR_PWM`（0-255）。

本程序没有行程限位。开关停在 I 或 II 时，马达会持续运行；绳子到端点后必须立刻拨回 O。最终装置建议增加两端限位开关，并确认每台马达的堵转电流不超过驱动板每路额定能力。

## 参考资料

- [Jaycar XC4492 官方手册](https://media.jaycar.com.au/product/resources/XC4492_manualMain_154666.pdf)
- [Keyestudio KS0063 官方文档](https://docs.keyestudio.com/projects/KS0063/en/latest/docs/KS0063.html)
- [Arduino UNO R4 Minima 官方资料](https://docs.arduino.cc/hardware/uno-r4-minima)
