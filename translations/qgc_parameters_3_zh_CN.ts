<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en">
  <context>
    <name>VehicleParameters</name>
    <message>
      <source> BendyRuler will avoid changing bearing unless ratio of previous margin from obstacle (or fence) to present calculated margin is atleast this much.</source>
      <translation> BendyRuler 将避免改变方位角，除非之前距离障碍物（或栅栏）的距离与当前计算出的距离的比率至少是这么多。</translation>
    </message>
    <message>
      <source> SDCard Fence size</source>
      <translation> SD卡围栏尺寸</translation>
    </message>
    <message>
      <source>"Superexponential" factor for refining the input curve shape tuned using RO_YAW_EXPO. 0 Pure Expo function 0.7 reasonable shape enhancement for intuitive stick feel 0.95 very strong bent input curve only near maxima have effect</source>
      <translation>用于细化使用 RO_YAW_EXPO 调整的输入曲线形状的“超指数”因子。 0 Pure Expo功能 0.7 合理的形状增强，带来直观的棒感 0.95 非常强的弯曲输入曲线，仅在最大值附近才有效果</translation>
    </message>
    <message>
      <source>(unused) HORIZON_SIDEBARS</source>
      <translation>（未使用）HORIZON_SIDEBARS</translation>
    </message>
    <message>
      <source>- Turtle Mode enabled via AUX1</source>
      <translation>- 通过 AUX1 启用海龟模式</translation>
    </message>
    <message>
      <source>0 - UAVCAN disabled.
1 - Enables support for UAVCAN sensors without dynamic node ID allocation and firmware update.
2 - Enables support for UAVCAN sensors with dynamic node ID allocation and firmware update.
3 - Enables support for UAVCAN sensors and actuators with dynamic node ID allocation and firmware update. Also sets the motor control outputs to UAVCAN.</source>
      <translation>0 - UAVCAN 已禁用。 1 - 支持 UAVCAN 传感器，无需动态节点 ID 分配和固件更新。 2 - 支持具有动态节点 ID 分配和固件更新的 UAVCAN 传感器。 3 - 通过动态节点 ID 分配和固件更新支持 UAVCAN 传感器和执行器。还将电机控制输出设置为 UAVCAN。</translation>
    </message>
    <message>
      <source>0 for none, any other for the RC channel to be used to control pan (yaw) movements</source>
      <translation>0 为无，任何其他为 RC 通道，用于控制平移（偏航）运动</translation>
    </message>
    <message>
      <source>0.5 Hz</source>
      <translation>0.5 Hz</translation>
    </message>
    <message>
      <source>0: System has no magnetometer, preflight checks should pass without one. 1-N: Require the presence of N magnetometer sensors for check to pass.</source>
      <translation>0：系统没有磁力计，即使没有磁力计，飞行前检查也应该通过。 1-N：需要存在 N 个磁力计传感器才能通过检查。</translation>
    </message>
    <message>
      <source>0:Disable mode change following fence action until fence breach is cleared. When bit 1 is set the allowable flight areas is the union of all polygon and circle fence areas instead of the intersection, which means a fence breach occurs only if you are outside all of the fence areas.</source>
      <translation>0：在栅栏动作之后禁用模式更改，直到栅栏突破被清除。当位 1 被设置时，允许的飞行区域是所有多边形和圆形围栏区域的并集而不是交集，这意味着仅当您位于所有围栏区域之外时才会发生围栏突破。</translation>
    </message>
    <message>
      <source>0x84</source>
      <translation>0x84</translation>
    </message>
    <message>
      <source>1 byte bitmap of IMUs to use in EKF2. A separate instance of EKF2 will be started for each IMU selected. Set to 1 to use the first IMU only (default), set to 2 to use the second IMU only, set to 3 to use the first and second IMU. Additional IMU's can be used up to a maximum of 6 if memory and processing resources permit. There may be insufficient memory and processing resources to run multiple instances. If this occurs EKF2 will fail to start.</source>
      <translation>在 EKF2 中使用的 IMU 的 1 字节位图。将为每个选定的 IMU 启动一个单独的 EKF2 实例。设置为 1 时仅使用第一个 IMU（默认），设置为 2 时仅使用第二个 IMU，设置为 3 时使用第一个和第二个 IMU。如果内存和处理资源允许，可以使用额外的 IMU，最多可达 6。可能没有足够的内存和处理资源来运行多个实例。如果发生这种情况，EKF2 将无法启动。</translation>
    </message>
    <message>
      <source>1 byte bitmap of which EKF3 instances will use the output from the EKF-GSF yaw estimator that has been turned on by the EK3_GSF_RUN parameter. If the inertial navigation calculation stops following the GPS, then the vehicle code can request EKF3 to attempt to resolve the issue, either by performing a yaw reset if enabled by this parameter by switching to another EKF3 instance. Additionally the EKF3 will  initiate a reset internally if navigation is lost for more than EK3_GSF_DELAY milli seconds.</source>
      <translation>1 字节位图，其中 EKF3 实例将使用已由 EK3_GSF_RUN 参数打开的 EKF-GSF 偏航估计器的输出。如果惯性导航计算在 GPS 之后停止，则车辆代码可以请求 EKF3 尝试解决问题，方法是通过切换到另一个 EKF3 实例来执行偏航重置（如果此参数启用）。此外，如果导航丢失时间超过 EK3_GSF_DELAY 毫秒，EKF3 将在内部启动重置。</translation>
    </message>
    <message>
      <source>10 Hz</source>
      <translation>10 Hz</translation>
    </message>
    <message>
      <source>1090ES_out</source>
      <translation>1090ES_out</translation>
    </message>
    <message>
      <source>12.5 Hz</source>
      <translation>12.5 Hz</translation>
    </message>
    <message>
      <source>134 8N1</source>
      <translation>134 8N1</translation>
    </message>
    <message>
      <source>150Hz</source>
      <translation>150Hz</translation>
    </message>
    <message>
      <source>19200 8N1</source>
      <translation>19200 8N1</translation>
    </message>
    <message>
      <source>1st GPS type</source>
      <translation>第一代GPS型</translation>
    </message>
    <message>
      <source>20 Hz</source>
      <translation>20 Hz</translation>
    </message>
    <message>
      <source>230400 8N1</source>
      <translation>230400 8N1</translation>
    </message>
    <message>
      <source>280 Hz</source>
      <translation>280 Hz</translation>
    </message>
    <message>
      <source>2D fix</source>
      <translation>2D 修复</translation>
    </message>
    <message>
      <source>2nd harmonic</source>
      <translation>二次谐波</translation>
    </message>
    <message>
      <source>300 8N1</source>
      <translation>300 8N1</translation>
    </message>
    <message>
      <source>3D Clockwise</source>
      <translation>3D 顺时针</translation>
    </message>
    <message>
      <source>3D velocity</source>
      <translation>3D 速度</translation>
    </message>
    <message>
      <source>4 byte bitmap of log types to enable</source>
      <translation>4 要启用的日志类型的字节位图</translation>
    </message>
    <message>
      <source>4M</source>
      <translation>4M</translation>
    </message>
    <message>
      <source>5-Key OSD</source>
      <translation>5-钥匙 OSD</translation>
    </message>
    <message>
      <source>50x18</source>
      <translation>50x18</translation>
    </message>
    <message>
      <source>5th Sensor</source>
      <translation>第 5 个传感器</translation>
    </message>
    <message>
      <source>68 Hz</source>
      <translation>68 Hz</translation>
    </message>
    <message>
      <source>75Kts</source>
      <translation>75Kts</translation>
    </message>
    <message>
      <source>8S Battery</source>
      <translation>8S电池</translation>
    </message>
    <message>
      <source>A bitmask of which EKF2 instances run an independant EKF-GSF yaw estimator to provide a backup yaw estimate that doesn't rely on magnetometer data. This estimator uses IMU, GPS and, if available, airspeed data. EKF-GSF yaw estimator data for the primary EKF2 instance will be logged as GSF0 and GSF1 messages. Use of the yaw estimate generated by this algorithm is controlled by the EK2_GSF_USE_MASK and EK2_GSF_RST_MAX parameters. To run the EKF-GSF yaw estimator in ride-along and logging only, set EK2_GSF_USE_MASK to 0.</source>
      <translation>EKF2 实例运行独立的 EKF-GSF 偏航估计器的位掩码，以提供不依赖于磁力计数据的备份偏航估计。该估算器使用 IMU、GPS 以及空速数据（如果可用）。主 EKF2 实例的 EKF-GSF 偏航估计器数据将记录为 GSF0 和 GSF1 消息。该算法生成的偏航估计的使用由 EK2_GSF_USE_MASK 和 EK2_GSF_RST_MAX 参数控制。要仅在行驶和记录中运行 EKF-GSF 偏航估算器，请将 EK2_GSF_USE_MASK 设置为 0。</translation>
    </message>
    <message>
      <source>A bitmask which controls what channels can move while the safety switch has not been pressed</source>
      <translation>控制未按下安全开关时可以移动哪些通道的位掩码</translation>
    </message>
    <message>
      <source>A non-zero, positive value specifies the time in seconds, within which the vehicle is expected to take off after arming. In case the vehicle didn't takeoff within the timeout it disarms again. A negative value disables autmoatic disarming triggered by a pre-takeoff timeout.</source>
      <translation>非零正值指定车辆在解锁后预计起飞的时间（以秒为单位）。如果车辆在超时内没有起飞，它会再次上锁。负值将禁用由起飞前超时触发的自动解除功能。</translation>
    </message>
    <message>
      <source>A reference value of zero disables dynamic updates on the Harmonic Notch Filter and a positive value enables dynamic updates on the Harmonic Notch Filter.  For throttle-based scaling, this parameter is the reference value associated with the specified frequency to facilitate frequency scaling of the Harmonic Notch Filter. For RPM and ESC telemetry based tracking, this parameter is set to 1 to enable the Harmonic Notch Filter using the RPM sensor or ESC telemetry set to measure rotor speed.  The sensor data is converted to Hz automatically for use in the Harmonic Notch Filter.  This reference value may also be used to scale the sensor data, if required.  For example, rpm sensor data is required to measure heli motor RPM. Therefore the reference value can be used to scale the RPM sensor to the rotor RPM.</source>
      <translation>参考值为零会禁用谐波陷波滤波器的动态更新，正值会启用谐波陷波滤波器的动态更新。  对于基于节流阀的缩放，此参数是与指定频率关联的参考值，以方便谐波陷波滤波器的频率缩放。对于基于 RPM 和 ESC 遥测的跟踪，此参数设置为 1，以启用谐波陷波滤波器，使用 RPM 传感器或 ESC 遥测装置来测量转子速度。  传感器数据自动转换为 Hz，以便在谐波陷波滤波器中使用。  如果需要，该参考值还可用于缩放传感器数据。  例如，测量直升机电机 RPM 需要转速传感器数据。因此，参考值可用于将 RPM 传感器缩放至转子 RPM。</translation>
    </message>
    <message>
      <source>A warning is triggered if the currently estimated wind speed is above this value. Warning is sent periodically (every 1 minute). Set to -1 to disable.</source>
      <translation>如果当前估计的风速高于该值，则会触发警告。定期发送警告（每 1 分钟）。设置为 -1 以禁用。</translation>
    </message>
    <message>
      <source>A13</source>
      <translation>A13</translation>
    </message>
    <message>
      <source>ACCEL5 Failure</source>
      <translation>ACCEL5故障</translation>
    </message>
    <message>
      <source>ACRVOLT_EN</source>
      <translation>ACRVOLT_EN</translation>
    </message>
    <message>
      <source>ADSB Avoidance En</source>
      <translation>ADSB 回避 En</translation>
    </message>
    <message>
      <source>ADSB stream rate to ground station</source>
      <translation>ADSB 到地面站的码流速率</translation>
    </message>
    <message>
      <source>ADSB vehicle list size</source>
      <translation>ADSB车辆清单尺寸</translation>
    </message>
    <message>
      <source>ADSB-Out Ident Configuration</source>
      <translation>ADSB-输出识别配置</translation>
    </message>
    <message>
      <source>AFBR Rangefinder Mode</source>
      <translation>AFBR 测距仪模式</translation>
    </message>
    <message>
      <source>AHRS AutoTrim</source>
      <translation>AHRS 自动微调</translation>
    </message>
    <message>
      <source>AHRS data rate</source>
      <translation>AHRS 数据速率</translation>
    </message>
    <message>
      <source>AHRS type</source>
      <translation>AHRS型</translation>
    </message>
    <message>
      <source>AHRS/EKF origin will be set to this longitude if not already set</source>
      <translation>如果尚未设置，AHRS/EKF 原点将设置为该经度</translation>
    </message>
    <message>
      <source>AIS vessel list size</source>
      <translation>AIS 船舶清单尺寸</translation>
    </message>
    <message>
      <source>ALTITUDE_EN</source>
      <translation>ALTITUDE_EN</translation>
    </message>
    <message>
      <source>APM TriggerPin0</source>
      <translation>APM 触发引脚0</translation>
    </message>
    <message>
      <source>APM TriggerPin8</source>
      <translation>APM 触发引脚8</translation>
    </message>
    <message>
      <source>APM2-A3</source>
      <translation>APM2-A3</translation>
    </message>
    <message>
      <source>APM2-PulsedLightI2C</source>
      <translation>APM2-PulsedLightI2C</translation>
    </message>
    <message>
      <source>ARMVtol</source>
      <translation>ARMV托尔</translation>
    </message>
    <message>
      <source>ASPD2_Y</source>
      <translation>ASPD2_Y</translation>
    </message>
    <message>
      <source>ATTITUDE_FAST</source>
      <translation>ATTITUDE_FAST</translation>
    </message>
    <message>
      <source>AUAV L05D</source>
      <translation>AUAV L05D</translation>
    </message>
    <message>
      <source>AUTO Mode</source>
      <translation>AUTO模式</translation>
    </message>
    <message>
      <source>AUTO_ALWAYS</source>
      <translation>AUTO_ALWAYS</translation>
    </message>
    <message>
      <source>AUX function to kill engine. This can be activated either with a RCn_OPTION and a R/C switch or with a ground station auxilliary function</source>
      <translation>AUX 功能为终止引擎。这可以通过 RCn_OPTION 和 R/C 开关或地面站辅助功能来激活</translation>
    </message>
    <message>
      <source>AUX4 Passthrough RC channel</source>
      <translation>AUX4 直通 RC 通道</translation>
    </message>
    <message>
      <source>AUXOUT4</source>
      <translation>AUXOUT4</translation>
    </message>
    <message>
      <source>AVG_CELL_VOLTAGE</source>
      <translation>AVG_CELL_VOLTAGE</translation>
    </message>
    <message>
      <source>Above this height weathervaning is permitted.  If RNGFND_LANDING is enabled or terrain is enabled then this parameter sets height AGL. Otherwise this parameter sets height above home.  Set zero to ignore minimum height requirement to activate weathervaning</source>
      <translation>高于这个高度是允许风向标的。  如果启用 RNGFND_LANDING 或启用地形，则此参数设置高度 AGL。否则，此参数设置高于家庭的高度。  设置为零以忽略激活风向标的最低高度要求</translation>
    </message>
    <message>
      <source>Absolute maximum for all climb rate controlled modes. In manually piloted modes full stick deflection commands this velocity. For default autonomous velocity see MPC_Z_V_AUTO_UP</source>
      <translation>所有爬升率控制模式的绝对最大值。在手动驾驶模式下，全操纵杆偏转控制该速度。对于默认自主速度，请参阅 MPC_Z_V_AUTO_UP</translation>
    </message>
    <message>
      <source>Absolute roll threshold for quad-chute triggering in FW mode. Above this the vehicle will transition back to MC mode and execute behavior defined in COM_QC_ACT. Set to 0 do disable this threshold.</source>
      <translation>FW 模式下四槽触发的绝对横滚阈值。在此之上，车辆将转换回 MC 模式并执行 COM_QC_ACT 中定义的行为。设置为 0 会禁用此阈值。</translation>
    </message>
    <message>
      <source>Accel (vertical) Target notch filter index, zero disables. If upgrading from 4.6 this is Previously _ACCZ_NTF.</source>
      <translation>加速（垂直）目标陷波滤波器索引，零禁用。如果从 4.6 升级，则为以前的 _ACCZ_NTF。</translation>
    </message>
    <message>
      <source>Accel 2 scaling factor</source>
      <translation>Accel 2 比例因子</translation>
    </message>
    <message>
      <source>Accel 5 motor noise factor</source>
      <translation>Accel 5 电机噪声系数</translation>
    </message>
    <message>
      <source>AccelZD</source>
      <translation>加速ZD</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller I gain maximum</source>
      <translation>加速度（垂直）控制器 I 增益最大值</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller PD sum maximum</source>
      <translation>加速度（垂直）控制器 PD 总和最大值</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller feed forward. If upgrading from 4.6 this is _ACCZ_FF * 0.1.</source>
      <translation>加速度（垂直）控制器前馈。如果从 4.6 升级，则为 _ACCZ_FF * 0.1。</translation>
    </message>
    <message>
      <source>Acceleration compensation based on GPS velocity</source>
      <translation>基于GPS速度的加速度补偿</translation>
    </message>
    <message>
      <source>Acceleration of shove to vehicle in y axis</source>
      <translation>y 轴上推向车辆的加速度</translation>
    </message>
    <message>
      <source>Accelerometer 0 X-axis scaling factor</source>
      <translation>加速度计 0 X 轴比例因子</translation>
    </message>
    <message>
      <source>Accelerometer 1 X-axis offset</source>
      <translation>加速度计 1 X 轴偏移</translation>
    </message>
    <message>
      <source>Accelerometer 1 rotation relative to airframe</source>
      <translation>加速度计 1 相对于机身的旋转</translation>
    </message>
    <message>
      <source>Accelerometer 2 Z-axis offset</source>
      <translation>加速度计 2 Z 轴偏移</translation>
    </message>
    <message>
      <source>Accelerometer 3 X-axis offset</source>
      <translation>加速度计 3 X 轴偏移</translation>
    </message>
    <message>
      <source>Accelerometer 3 rotation relative to airframe</source>
      <translation>加速度计 3 相对于机身的旋转</translation>
    </message>
    <message>
      <source>Accelerometer bias learning limit</source>
      <translation>加速度计偏差学习限制</translation>
    </message>
    <message>
      <source>Accelerometer error threshold</source>
      <translation>加速度计误差阈值</translation>
    </message>
    <message>
      <source>Accelerometer offset temperature ^1 polynomial coefficient - X axis</source>
      <translation>加速度计偏移温度^1多项式系数-X轴</translation>
    </message>
    <message>
      <source>Accelerometer offset temperature ^3 polynomial coefficient - Z axis</source>
      <translation>加速度计偏移温度^3多项式系数-Z轴</translation>
    </message>
    <message>
      <source>Accelerometer scaling of X axis.  Calculated during acceleration calibration routine</source>
      <translation>X 轴的加速度计缩放。  在加速度校准例程期间计算</translation>
    </message>
    <message>
      <source>Accelerometer z noise density</source>
      <translation>加速度计 z 噪声密度</translation>
    </message>
    <message>
      <source>Accelerometer2 scaling of X axis</source>
      <translation>加速度计2 X轴缩放</translation>
    </message>
    <message>
      <source>Accelerometer3 offsets of X axis</source>
      <translation>加速度计3 X轴偏移</translation>
    </message>
    <message>
      <source>Accelerometer3 scaling of Y axis</source>
      <translation>加速度计3 Y轴缩放</translation>
    </message>
    <message>
      <source>Acceptance Radius</source>
      <translation>接受半径</translation>
    </message>
    <message>
      <source>Acro Mode</source>
      <translation>特技模式</translation>
    </message>
    <message>
      <source>Acro Throttle Mid</source>
      <translation>特技油门中</translation>
    </message>
    <message>
      <source>Acro body roll max rate setpoint</source>
      <translation>Acro 身体侧倾最大速率设定点</translation>
    </message>
    <message>
      <source>Acro mode maximum roll rate</source>
      <translation>Acro 模式最大横滚速率</translation>
    </message>
    <message>
      <source>Acro mode yaw super expo factor</source>
      <translation>Acro 模式偏航超级曝光系数</translation>
    </message>
    <message>
      <source>Action on geofence breach</source>
      <translation>针对地理围栏违规采取的行动</translation>
    </message>
    <message>
      <source>Action the system takes when the estimated position has an accuracy below the specified threshold. See COM_POS_LOW_EPH to set the failsafe threshold. The failsafe action is only executed if the vehicle is in auto mission or auto loiter mode, otherwise it is only a warning.</source>
      <translation>当估计位置的精度低于指定阈值时，系统将采取行动。请参见 COM_POS_LOW_EPH 设置故障安全阈值。仅当车辆处于自动任务或自动盘旋模式时才会执行故障保护操作，否则仅发出警告。</translation>
    </message>
    <message>
      <source>ActiveForSafetyEnable</source>
      <translation>主动安全启用</translation>
    </message>
    <message>
      <source>Actuator5</source>
      <translation>执行器5</translation>
    </message>
    <message>
      <source>Address on the bus</source>
      <translation>巴士上的地址</translation>
    </message>
    <message>
      <source>Aeron</source>
      <translation>埃龙</translation>
    </message>
    <message>
      <source>After the auto-tuning sequence is completed, a new set of gains is available and can be applied immediately or after landing.</source>
      <translation>自动调整序列完成后，可以立即或在着陆后应用一组新的增益。</translation>
    </message>
    <message>
      <source>Air Mode</source>
      <translation>空气模式</translation>
    </message>
    <message>
      <source>Airbrake</source>
      <translation>气闸</translation>
    </message>
    <message>
      <source>Airmode_On_Arm</source>
      <translation>空中模式_On_Arm</translation>
    </message>
    <message>
      <source>Airspeed and GPS speed difference that gives a warning</source>
      <translation>空速和 GPS 速度差异发出警告</translation>
    </message>
    <message>
      <source>Airspeed failsafe start delay</source>
      <translation>空速故障安全启动延迟</translation>
    </message>
    <message>
      <source>Airspeed measurement delay relative to IMU measurements</source>
      <translation>相对于 IMU 测量的空速测量延迟</translation>
    </message>
    <message>
      <source>Airspeed pitot tube failure pressure</source>
      <translation>空速皮托管失效压力</translation>
    </message>
    <message>
      <source>Airspeed sensor ID, taking into account its type, bus and instance</source>
      <translation>空速传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>Airspeed signflip</source>
      <translation>空速信号翻转</translation>
    </message>
    <message>
      <source>Alarm Inverted</source>
      <translation>报警反转</translation>
    </message>
    <message>
      <source>All internal receivers</source>
      <translation>所有内部接收器</translation>
    </message>
    <message>
      <source>All+FullIMU</source>
      <translation>全部+全IMU</translation>
    </message>
    <message>
      <source>Allow DroneCAN InfoAux to be from a different CAN node</source>
      <translation>允许 DroneCAN InfoAux 来自不同的 CAN 节点</translation>
    </message>
    <message>
      <source>Allow Takeoff Without Raising Throttle</source>
      <translation>允许在不提高油门的情况下起飞</translation>
    </message>
    <message>
      <source>Allow disarming via switch/stick/button on multicopters in manual thrust modes</source>
      <translation>允许在手动推力模式下通过多旋翼飞行器上的开关/操纵杆/按钮上锁</translation>
    </message>
    <message>
      <source>Allow weathervaning</source>
      <translation>允许风向标</translation>
    </message>
    <message>
      <source>Allows enabling or disabling of pre-arming checks of receiver, accelerometer, barometer, compass and GPS</source>
      <translation>允许启用或禁用接收器、加速度计、气压计、罗盘和 GPS 的预配置检查</translation>
    </message>
    <message>
      <source>Allows setting the home position after takeoff</source>
      <translation>允许在起飞后设置起始位置</translation>
    </message>
    <message>
      <source>Allows to start the vehicle by throwing it into the air.</source>
      <translation>允许通过将车辆扔到空中来启动车辆。</translation>
    </message>
    <message>
      <source>Allows you to enable (1) or disable (0) the Preland simulation</source>
      <translation>允许您启用 (1) 或禁用 (0) Preland 模拟</translation>
    </message>
    <message>
      <source>Allows you to enable (1) or disable (0) the gripper servo simulation</source>
      <translation>允许您启用 (1) 或禁用 (0) 夹具舵机模拟</translation>
    </message>
    <message>
      <source>AllyStar</source>
      <translation>盟友之星</translation>
    </message>
    <message>
      <source>Altitude</source>
      <translation>海拔高度</translation>
    </message>
    <message>
      <source>Altitude and Circle</source>
      <translation>高度和圆</translation>
    </message>
    <message>
      <source>Altitude during Landing at which vehicle slows to LAND_SPEED</source>
      <translation>着陆时车辆减速至 LAND_SPEED 时的高度</translation>
    </message>
    <message>
      <source>Altitude for 3. step of slow landing</source>
      <translation>3 的海拔高度。缓慢着陆的步骤</translation>
    </message>
    <message>
      <source>Altitude max frame type</source>
      <translation>最大海拔高度 框架类型</translation>
    </message>
    <message>
      <source>Altitude slope maximum height</source>
      <translation>海拔坡度最大高度</translation>
    </message>
    <message>
      <source>Altitude where the landing gear will be deployed. This should be lower than the RETRACT_ALT. If zero then altitude is not used for deploying landing gear. Only applies when vehicle is armed.</source>
      <translation>起落架部署的高度。这应该低于 RETRACT_ALT。如果为零，则高度不用于展开起落架。仅在车辆解锁时适用。</translation>
    </message>
    <message>
      <source>Always off</source>
      <translation>始终关闭</translation>
    </message>
    <message>
      <source>Amount of curvature in throttle curve: 0 is linear, 1 is cubic</source>
      <translation>油门曲线曲率量：0为线性，1为三次</translation>
    </message>
    <message>
      <source>Amphenol AUAV differential / absolute pressure sensor (external I2C)</source>
      <translation>安费诺AUAV差压/绝压传感器（外置I2C）</translation>
    </message>
    <message>
      <source>An RC input channel to use for flaps control. If this is set to a RC channel number then that channel will be used for manual flaps control. When enabled, the percentage of flaps is taken as the percentage travel from the TRIM value of the channel to the MIN value of the channel. A value above the TRIM values will give inverse flaps (spoilers). This option needs to be enabled in conjunction with a FUNCTION setting on an output channel to one of the flap functions. When a FLAP_IN_CHANNEL is combined with auto-flaps the higher of the two flap percentages is taken.</source>
      <translation>用于襟翼控制的 RC 输入通道。如果将其设置为 RC 通道号，则该通道将用于手动襟翼控制。启用后，襟翼百分比被视为从通道的 TRIM 值到通道的 MIN 值的行程百分比。高于 TRIM 值的值将产生反向襟翼（扰流板）。该选项需要与襟翼功能之一的输出通道上的 FUNCTION 设置结合启用。当 FLAP_IN_CHANNEL 与自动襟翼结合使用时，取两个襟翼百分比中的较高者。</translation>
    </message>
    <message>
      <source>An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
Set to "Custom Euler Angle" to define the rotation using CAL_MAG3_ROLL, CAL_MAG3_PITCH and CAL_MAG3_YAW.</source>
      <translation>内部传感器将强制设置 -1 的值，因此 GCS 仅应在该值大于或等于零时尝试配置旋转。设置为“自定义欧拉角”以使用 CAL_MAG3_ROLL、CAL_MAG3_PITCH 和 CAL_MAG3_YAW 定义旋转。</translation>
    </message>
    <message>
      <source>Analog Devices ADIS16448 IMU Orientation(external SPI)</source>
      <translation>Analog Devices ADIS16448 IMU 方向（外部 SPI）</translation>
    </message>
    <message>
      <source>Analog or PWM input pin that rangefinder is connected to. Airspeed ports can be used for Analog input, AUXOUT can be used for PWM input. When using analog pin 103, the maximum value of the input in 3.3V.</source>
      <translation>测距仪连接的模拟或 PWM 输入引脚。 Airspeed 端口可用于模拟输入，AUXOUT 可用于 PWM 输入。当使用模拟引脚 103 时，3.3V 中输入的最大值。</translation>
    </message>
    <message>
      <source>Angle Limit (to maintain altitude) Time Constant</source>
      <translation>角度限制（维持高度） 时间常数</translation>
    </message>
    <message>
      <source>Angle left/right from the commanded setpoint by which the collision prevention algorithm can choose to change the setpoint direction</source>
      <translation>与命令设定点的左/右角度，碰撞预防算法可以选择改变设定点方向</translation>
    </message>
    <message>
      <source>Angular Velocity Max for Roll</source>
      <translation>横滚最大角速度</translation>
    </message>
    <message>
      <source>Angular rate</source>
      <translation>角速率</translation>
    </message>
    <message>
      <source>Any pitch setpoint below this value is translated to a forward force by the fixed-wing forward actuation if
VT_FWD_TRHUST_EN is set.</source>
      <translation>如果设置了 VT_FWD_TRHUST_EN，则任何低于该值的俯仰设定点都会通过固定翼前向驱动转换为前向力。</translation>
    </message>
    <message>
      <source>Applies in any altitude controlled flight mode. Should be set accordingly to achieve FW_T_CLMB_MAX.</source>
      <translation>适用于任何高度控制飞行模式。应进行相应设置以实现FW_T_CLMB_MAX。</translation>
    </message>
    <message>
      <source>Apply the estimated scale in air</source>
      <translation>应用空气中的估计比例</translation>
    </message>
    <message>
      <source>Arm Checks to Peform (bitmask)</source>
      <translation>Arm 检查以执行（位掩码）</translation>
    </message>
    <message>
      <source>Arm length for swash plate servo 2</source>
      <translation>斜盘舵机 2 臂长</translation>
    </message>
    <message>
      <source>ArmDisarm (4.1 and lower)</source>
      <translation>ArmDisarm（4.1 及更低版本）</translation>
    </message>
    <message>
      <source>Arming check</source>
      <translation>解锁检查</translation>
    </message>
    <message>
      <source>Arming options</source>
      <translation>解锁选项</translation>
    </message>
    <message>
      <source>Assume ACC_BODYFIX is perfectly aligned to the vehicle</source>
      <translation>假设 ACC_BODYFIX 与车辆完美对齐</translation>
    </message>
    <message>
      <source>Attitude control input time constant (aka smoothing)</source>
      <translation>姿态控制输入时间常数（又名平滑）</translation>
    </message>
    <message>
      <source>Auto Declination</source>
      <translation>自动偏角</translation>
    </message>
    <message>
      <source>Auto mode options</source>
      <translation>自动模式选项</translation>
    </message>
    <message>
      <source>Auto-bind time</source>
      <translation>自动绑定时间</translation>
    </message>
    <message>
      <source>Auto: use gyro from optical flow message if available, internal gyro otherwise. Internal: always use internal gyro</source>
      <translation>自动：如果可用，则使用光流消息中的陀螺仪，否则使用内部陀螺仪。内部：始终使用内部陀螺仪</translation>
    </message>
    <message>
      <source>AutoEnableOnlyWhenArmed</source>
      <translation>仅在解锁时自动启用</translation>
    </message>
    <message>
      <source>AutoTune maximum response gain</source>
      <translation>自动调节最大响应增益</translation>
    </message>
    <message>
      <source>Autoland mode options</source>
      <translation>自动着陆模式选项</translation>
    </message>
    <message>
      <source>Automatic switchover to GPS reporting best lock</source>
      <translation>自动切换到 GPS 报告最佳锁定</translation>
    </message>
    <message>
      <source>Automatically calibrate barometer based on the GNSS height</source>
      <translation>根据GNSS高度自动校准气压计</translation>
    </message>
    <message>
      <source>Automatically set external rotations</source>
      <translation>自动设置外旋转</translation>
    </message>
    <message>
      <source>Autorecording enabled</source>
      <translation>自动录音已启用</translation>
    </message>
    <message>
      <source>Aux global position (AGP) sensor aiding</source>
      <translation>辅助全球定位 (AGP) 传感器辅助</translation>
    </message>
    <message>
      <source>AuxAuth</source>
      <translation>辅助验证</translation>
    </message>
    <message>
      <source>Auxiliary global position sensor 0 delay (to IMU)</source>
      <translation>辅助全球位置传感器 0 延迟（至 IMU）</translation>
    </message>
    <message>
      <source>Auxiliary global position sensor 3 aiding</source>
      <translation>辅助全球位置传感器 3 提供帮助</translation>
    </message>
    <message>
      <source>Avoid_ADSB</source>
      <translation>避免_ADSB</translation>
    </message>
    <message>
      <source>Avoidance maximum acceleration</source>
      <translation>回避最大加速度</translation>
    </message>
    <message>
      <source>Axes to tune</source>
      <translation>要调整的轴</translation>
    </message>
    <message>
      <source>Axis of rotor 10 thrust vector, Y body axis component</source>
      <translation>转子轴10推力矢量，Y本体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 3 thrust vector, X body axis component</source>
      <translation>转子轴3推力矢量，X体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 5 thrust vector, Z body axis component</source>
      <translation>转子轴5推力矢量，Z体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 8 thrust vector, Y body axis component</source>
      <translation>转子轴8推力矢量，Y本体轴分量</translation>
    </message>
    <message>
      <source>Azimuth position on swashplate 2 for servo 3 with the front of the heli being 0 deg</source>
      <translation>舵机 3 斜盘 2 上的方位位置，直升机前部为 0 deg</translation>
    </message>
    <message>
      <source>BATT_BAR_EN</source>
      <translation>BATT_BAR_EN</translation>
    </message>
    <message>
      <source>BATVOLT_Y</source>
      <translation>BATVOLT_Y</translation>
    </message>
    <message>
      <source>BBBMini Pin P8.14</source>
      <translation>BBBMini 引脚 P8.14</translation>
    </message>
    <message>
      <source>BLHAMPS_Y</source>
      <translation>BLHAMPS_Y</translation>
    </message>
    <message>
      <source>BLHeli ESC</source>
      <translation>BL合力ESC</translation>
    </message>
    <message>
      <source>BLHeli internal interface test</source>
      <translation>BLHeli内部接口测试</translation>
    </message>
    <message>
      <source>BLHeli32/Kiss+EDT</source>
      <translation>BLHeli32/吻+EDT</translation>
    </message>
    <message>
      <source>BMM350 ODR rate</source>
      <translation>BMM350 ODR 汇率</translation>
    </message>
    <message>
      <source>BQ40Z50 based</source>
      <translation>基于BQ40Z50</translation>
    </message>
    <message>
      <source>Back-Left</source>
      <translation>左后</translation>
    </message>
    <message>
      <source>Balance Bot pitch trim angle</source>
      <translation>平衡机器人俯仰配平角</translation>
    </message>
    <message>
      <source>Band E</source>
      <translation>E带</translation>
    </message>
    <message>
      <source>Baro ID2</source>
      <translation>巴罗ID2</translation>
    </message>
    <message>
      <source>Baro temperature factor</source>
      <translation>气压温度系数</translation>
    </message>
    <message>
      <source>Barometer 0 offset</source>
      <translation>气压计 0 偏移</translation>
    </message>
    <message>
      <source>Barometer 3 calibration device ID</source>
      <translation>气压计3校准装置ID</translation>
    </message>
    <message>
      <source>Barometer calibration maximum temperature</source>
      <translation>气压计校准最高温度</translation>
    </message>
    <message>
      <source>Barometer freeze</source>
      <translation>气压计冻结</translation>
    </message>
    <message>
      <source>Barometer offset temperature ^1 polynomial coefficients</source>
      <translation>气压计偏移温度^1多项式系数</translation>
    </message>
    <message>
      <source>Barometer warmup temperature time constant</source>
      <translation>气压计预热温度时间常数</translation>
    </message>
    <message>
      <source>Barometer3 sensor ID, taking into account its type, bus and instance</source>
      <translation>Barometer3 传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>Base antenna Y position offset</source>
      <translation>底座天线 Y 位置偏移</translation>
    </message>
    <message>
      <source>Battery</source>
      <translation>电池</translation>
    </message>
    <message>
      <source>Battery 2 Current ADC Channel</source>
      <translation>电池 2 当前 ADC 通道</translation>
    </message>
    <message>
      <source>Battery 3 monitoring source</source>
      <translation>电池3监控源</translation>
    </message>
    <message>
      <source>Battery Monitor</source>
      <translation>电池监视器</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT3__FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT3__FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT7__FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT7__FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTB__FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTB__FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTF__FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTF__FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT5_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT5_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTD_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTD_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT2__ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT2__ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT6__ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT6__ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATTA__ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATTA__ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATTE__ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATTE__ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery compensation index</source>
      <translation>电池补偿指标</translation>
    </message>
    <message>
      <source>Battery estimator index</source>
      <translation>电池估算器索引</translation>
    </message>
    <message>
      <source>Battery monitor options</source>
      <translation>电池监视器选项</translation>
    </message>
    <message>
      <source>Battery voltage compensation maximum voltage</source>
      <translation>电池电压补偿最大电压</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATT3_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATT3_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATT3_LOW_TIMER 参数指定的时间，则车辆将执行 BATT3_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATTB_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATTB_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATTB_LOW_TIMER 参数指定的时间，则车辆将执行 BATTB_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATT3_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATT3_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATT3_LOW_TIMER 参数指定的时间，则车辆将执行 BATT3_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATTB_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATTB_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATTB_LOW_TIMER 参数指定的时间，则车辆将执行 BATTB_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage to trigger failsafe. Set to 0 to disable battery voltage failsafe. If the battery voltage drops below this voltage then the copter will RTL</source>
      <translation>触发故障保护的电池电压。设置为 0 以禁用电池电压故障保护。如果电池电压低于此电压，则飞行器将 RTL</translation>
    </message>
    <message>
      <source>Baudrate for the Radio Controller Serial Port</source>
      <translation>无线电控制器串行端口的波特率</translation>
    </message>
    <message>
      <source>Baudrate used by default for serial communication between PX4
and SBG Systems INS through sbgECom driver.</source>
      <translation>PX4 和 SBG 系统 INS 通过 sbgECom 驱动程序之间的串行通信默认使用的波特率。</translation>
    </message>
    <message>
      <source>Beacon systems rotation from north in degrees</source>
      <translation>信标系统从北旋转（以度为单位）</translation>
    </message>
    <message>
      <source>Beidou</source>
      <translation>北斗</translation>
    </message>
    <message>
      <source>BendyRuler will resist changing current bearing if the change in bearing is over this angle</source>
      <translation>如果方位角变化超过此角度，BendyRuler 将阻止更改当前方位角</translation>
    </message>
    <message>
      <source>BenewakeTFS20L</source>
      <translation>北醒TFS20L</translation>
    </message>
    <message>
      <source>BiCopter</source>
      <translation>双旋翼机</translation>
    </message>
    <message>
      <source>Bitmap of what Logger backend types to enable. Block-based logging is available on SITL and boards with dataflash chips. Multiple backends can be selected.</source>
      <translation>要启用的记录器后端类型的位图。基于块的日志记录可在 SITL 和带有数据闪存芯片的板上使用。可以选择多个后端。</translation>
    </message>
    <message>
      <source>Bitmap of which IMUs to log batch data for. This option takes effect on the next reboot.</source>
      <translation>要记录批次数据的 IMU 的位图。该选项在下次重新启动时生效。</translation>
    </message>
    <message>
      <source>Bitmask for configuring this telemetry channel. For having effect on all channels, set the relevant mask in all MAVx_OPTIONS parameters. Keep in mind that part of the flags may require a reboot to take action. Unicast blocks forwarding of broadcasts to/from this link, but allows packets addressed to a learned system/component pair. A missing or zero target system or component is a broadcast. Unicast sends heartbeat but does not start the normal telemetry streams; devices can request messages with MAV_CMD_SET_MESSAGE_INTERVAL or MAV_CMD_REQUEST_MESSAGE. Local processing is unaffected.</source>
      <translation>用于配置此遥测通道的位掩码。为了对所有通道生效，请在所有 MAVx_OPTIONS 参数中设置相关掩码。请记住，部分标志可能需要重新启动才能执行操作。单播阻止向该链路转发广播或从该链路转发广播，但允许发送至已知系统/组件对的数据包。丢失或零目标系统或组件是广播。单播发送心跳但不启动正常的遥测流；设备可以通过 MAV_CMD_SET_MESSAGE_INTERVAL 或 MAV_CMD_REQUEST_MESSAGE 请求消息。本地处理不受影响。</translation>
    </message>
    <message>
      <source>Bitmask of AIS logging options</source>
      <translation>AIS 日志记录选项的位掩码</translation>
    </message>
    <message>
      <source>Bitmask of additional options for battery, radio, &amp; GCS failsafes. 0 (default) disables all options.</source>
      <translation>电池、无线电和 GCS 故障保护附加选项的位掩码。 0（默认）禁用所有选项。</translation>
    </message>
    <message>
      <source>Bitmask of harmonic frequencies to apply Harmonic Notch Filter to. This option takes effect on the next reboot. A maximum of 3 harmonics can be used at any one time.</source>
      <translation>要应用谐波陷波滤波器的谐波频率的位掩码。该选项在下次重新启动时生效。任一时间最多可使用 3 谐波。</translation>
    </message>
    <message>
      <source>Bitmask of options to use with airspeed.</source>
      <translation>与空速一起使用的选项位掩码。</translation>
    </message>
    <message>
      <source>Bitmask of the control transports permitted to manage the VTX. AP_VideoTX represents a single VTX, so when more than one transport is present (for example a CRSF VTX on the receiver and an MSP VTX on the goggles) this selects which one owns it. Clear a transport's bit to stop it taking control of the VTX.</source>
      <translation>允许管理 VTX 的控制传输的位掩码。 AP_VideoTX 代表单个 VTX，因此当存在多个传输时（例如接收器上的 CRSF VTX 和护目镜上的 MSP VTX），将选择哪一个拥有它。清除传输位以阻止其控制 VTX。</translation>
    </message>
    <message>
      <source>Bitmask to enable failsafe options</source>
      <translation>用于启用故障安全选项的位掩码</translation>
    </message>
    <message>
      <source>Bitmask with one set for channel to be transmitted as a ESC command over UAVCAN</source>
      <translation>具有一组位掩码，用于通过 UAVCAN 作为 ESC 命令进行传输</translation>
    </message>
    <message>
      <source>Blackbox speed</source>
      <translation>黑匣子速度</translation>
    </message>
    <message>
      <source>Board Heater I gain</source>
      <translation>板式加热器 I 增益</translation>
    </message>
    <message>
      <source>Board heater temp lower margin</source>
      <translation>板加热器温度下限</translation>
    </message>
    <message>
      <source>Board rotation X (roll) offset</source>
      <translation>板旋转X（横滚）偏移</translation>
    </message>
    <message>
      <source>Body frame Yaw/Pitch/Roll</source>
      <translation>机身框架 偏航/俯仰/横滚</translation>
    </message>
    <message>
      <source>BoostThrottle</source>
      <translation>加速油门</translation>
    </message>
    <message>
      <source>Box Width</source>
      <translation>箱体宽度</translation>
    </message>
    <message>
      <source>BrushedBiPolar</source>
      <translation>拉丝双极</translation>
    </message>
    <message>
      <source>Bus address of second rangefinder</source>
      <translation>第二个测距仪的总线地址</translation>
    </message>
    <message>
      <source>Bus1(external)</source>
      <translation>总线1（外部）</translation>
    </message>
    <message>
      <source>Bushed motor reverse 3 omni motor 3</source>
      <translation>衬套电机反向 3 全向电机 3</translation>
    </message>
    <message>
      <source>Button Pin 4 Options</source>
      <translation>按钮引脚 4 选项</translation>
    </message>
    <message>
      <source>Buzzer pin</source>
      <translation>蜂鸣器引脚</translation>
    </message>
    <message>
      <source>CADDX</source>
      <translation>CADDX</translation>
    </message>
    <message>
      <source>CAN based Pitot tube</source>
      <translation>基于CAN的皮托管</translation>
    </message>
    <message>
      <source>CAN2</source>
      <translation>CAN2</translation>
    </message>
    <message>
      <source>CANTester LoopRate</source>
      <translation>CANTester循环速率</translation>
    </message>
    <message>
      <source>CELL_VOLT_EN</source>
      <translation>CELL_VOLT_EN</translation>
    </message>
    <message>
      <source>CLIMBEFF_EN</source>
      <translation>CLIMBEFF_EN</translation>
    </message>
    <message>
      <source>CMTC Height</source>
      <translation>CMTC高度</translation>
    </message>
    <message>
      <source>COMPASS_EN</source>
      <translation>COMPASS_EN</translation>
    </message>
    <message>
      <source>CRSSHAIR_EN</source>
      <translation>CRSSHAIR_EN</translation>
    </message>
    <message>
      <source>CURRENT2_EN</source>
      <translation>CURRENT2_EN</translation>
    </message>
    <message>
      <source>CYRF6936</source>
      <translation>CYRF6936</translation>
    </message>
    <message>
      <source>Calibration temperature for 1st gyroscope</source>
      <translation>第一陀螺仪的校准温度</translation>
    </message>
    <message>
      <source>Camera Auto Focus</source>
      <translation>相机自动对焦</translation>
    </message>
    <message>
      <source>Camera Mount instance</source>
      <translation>相机支架实例</translation>
    </message>
    <message>
      <source>Camera Zoom</source>
      <translation>相机变焦</translation>
    </message>
    <message>
      <source>Camera horizontal field of view. 0 if unknown</source>
      <translation>相机水平视野。 0（如果未知）</translation>
    </message>
    <message>
      <source>Camera servo OFF PWM value</source>
      <translation>相机舵机OFF PWM值</translation>
    </message>
    <message>
      <source>Camera trigger activation time</source>
      <translation>相机触发激活时间</translation>
    </message>
    <message>
      <source>Camera1 Thermal Gain</source>
      <translation>相机1热增益</translation>
    </message>
    <message>
      <source>Camera1 Video Stream Horizontal FOV</source>
      <translation>摄像头1 视频流水平 FOV</translation>
    </message>
    <message>
      <source>Camera1 Video Stream Type</source>
      <translation>相机1视频流类型</translation>
    </message>
    <message>
      <source>Can be set in flight to force termination of the heartbeat signal</source>
      <translation>可以在飞行中设置强制终止心跳信号</translation>
    </message>
    <message>
      <source>Can be used for parameter tuning with the RC. This one is further referenced as the 3th parameter channel. Set to 0 to deactivate *</source>
      <translation>可用于 RC 的参数调整。该通道进一步被称为第三个参数通道。设置为 0 以停用*</translation>
    </message>
    <message>
      <source>Capture to file</source>
      <translation>捕获到文件</translation>
    </message>
    <message>
      <source>Ch1</source>
      <translation>Ch1</translation>
    </message>
    <message>
      <source>Ch4</source>
      <translation>Ch4</translation>
    </message>
    <message>
      <source>Chan11</source>
      <translation>陈11</translation>
    </message>
    <message>
      <source>Chan4</source>
      <translation>陈4</translation>
    </message>
    <message>
      <source>Change the default downlink sensor id (SPort only)</source>
      <translation>更改默认下行链路传感器 ID（仅限 SPort）</translation>
    </message>
    <message>
      <source>Channel 1</source>
      <translation>通道1</translation>
    </message>
    <message>
      <source>Channel 14</source>
      <translation>通道14</translation>
    </message>
    <message>
      <source>Channel 21</source>
      <translation>频道 21</translation>
    </message>
    <message>
      <source>Channel 29</source>
      <translation>频道 29</translation>
    </message>
    <message>
      <source>Channel 6 Tuning</source>
      <translation>通道 6 调谐</translation>
    </message>
    <message>
      <source>Channel to show receive RSSI signal strength, or zero for disabled</source>
      <translation>显示接收 RSSI 信号强度的通道，或为零表示禁用</translation>
    </message>
    <message>
      <source>Channel13</source>
      <translation>通道13</translation>
    </message>
    <message>
      <source>Channel20</source>
      <translation>通道20</translation>
    </message>
    <message>
      <source>Channel29</source>
      <translation>通道29</translation>
    </message>
    <message>
      <source>Channel7</source>
      <translation>通道7</translation>
    </message>
    <message>
      <source>Check and update if needed</source>
      <translation>如果需要检查并更新</translation>
    </message>
    <message>
      <source>Checks prior to arming motor. This is a bitmask of checks that will be performed before allowing arming. For most users it is recommended to leave this at the default of 1 (all checks enabled). You can select whatever checks you prefer by adding together the values of each check type to set this parameter. For example, to only allow arming when you have GPS lock and no RC failsafe you would set ARMING_CHECK to 72.</source>
      <translation>在启动电机之前进行检查。这是在允许装备之前执行的检查位掩码。对于大多数用户，建议将其保留为默认值 1（启用所有检查）。您可以通过将每种检查类型的值相加来设置此参数来选择您喜欢的任何检查。例如，要仅在有 GPS 锁定且没有 RC 故障保护时允许解锁，您可以将 ARMING_CHECK 设置为 72。</translation>
    </message>
    <message>
      <source>Circle</source>
      <translation>圆</translation>
    </message>
    <message>
      <source>Circle control</source>
      <translation>圆圈控制</translation>
    </message>
    <message>
      <source>Circuit breaker for USB link check</source>
      <translation>USB链路检查用断路器</translation>
    </message>
    <message>
      <source>Cl factor</source>
      <translation>氯因子</translation>
    </message>
    <message>
      <source>Climb slope minimum height</source>
      <translation>爬坡最小高度</translation>
    </message>
    <message>
      <source>Closest allowed projected distance before W_ACTION is undertaken</source>
      <translation>进行 W_ACTION 之前允许的最近投影距离</translation>
    </message>
    <message>
      <source>Coefficient describing linear relationship between
Z component of magnetometer in body frame axis
and either current or throttle depending on value of CAL_MAG_COMP_TYP.
Unit for throttle-based compensation is [G] and
for current-based compensation [G/kA]</source>
      <translation>描述机体框架轴上磁力计的 Z 分量与电流或油门之间线性关系的系数，具体取决于 CAL_MAG_COMP_TYP 的值。基于油门的补偿的单位是[G]，基于电流的补偿的单位是[G/kA]</translation>
    </message>
    <message>
      <source>Collective Blade Pitch at Zero Thrust</source>
      <translation>零推力时的总距叶片桨距</translation>
    </message>
    <message>
      <source>Collective pitch curve at position 4</source>
      <translation>4 位置处的总距桨距曲线</translation>
    </message>
    <message>
      <source>Compass</source>
      <translation>罗盘</translation>
    </message>
    <message>
      <source>Compass device id with 2nd order priority</source>
      <translation>具有第二优先级的罗盘设备 ID</translation>
    </message>
    <message>
      <source>Compass is attached via an external cable</source>
      <translation>罗盘通过外部电缆连接</translation>
    </message>
    <message>
      <source>Compass offsets in milligauss on the Y axis</source>
      <translation>Y 轴上的罗盘偏移（以毫高斯为单位）</translation>
    </message>
    <message>
      <source>Compass per-motor2 Y</source>
      <translation>每电机罗盘2 Y</translation>
    </message>
    <message>
      <source>Compass soft-iron diagonal X component</source>
      <translation>罗盘软铁对角X分量</translation>
    </message>
    <message>
      <source>Compass2 device id expected</source>
      <translation>需要 Compass2 设备 ID</translation>
    </message>
    <message>
      <source>Compass2 soft-iron diagonal Y component</source>
      <translation>Compass2软铁对角Y分量</translation>
    </message>
    <message>
      <source>Compass3 is attached via an external cable</source>
      <translation>Compass3 通过外部电缆连接</translation>
    </message>
    <message>
      <source>Compass3 soft-iron diagonal Z component</source>
      <translation>Compass3软铁对角Z分量</translation>
    </message>
    <message>
      <source>Compass7 device id</source>
      <translation>罗盘7设备ID</translation>
    </message>
    <message>
      <source>Compensation for Y axis of motor1</source>
      <translation>电机Y轴补偿1</translation>
    </message>
    <message>
      <source>CompleteTransition if Q_TRANS_FAIL</source>
      <translation>如果 Q_TRANS_FAIL 则完成转换</translation>
    </message>
    <message>
      <source>Computer Vision and Avoidance</source>
      <translation>计算机视觉与规避</translation>
    </message>
    <message>
      <source>Configure compass so it is attached externally. This is auto-detected on most boards. Set to 1 if the compass is externally connected. When externally connected the COMPASS_ORIENT option operates independently of the AHRS_ORIENTATION board orientation option. If set to 0 or 1 then auto-detection by bus connection can override the value. If set to 2 then auto-detection will be disabled.</source>
      <translation>配置罗盘，使其连接到外部。大多数板上都会自动检测到这一点。如果罗盘是外部连接的，则设置为 1。当外部连接时，COMPASS_ORIENT 选项独立于 AHRS_ORIENTATION 板方向选项运行。如果设置为 0 或 1，则通过总线连接自动检测可以覆盖该值。如果设置为 2，则自动检测将被禁用。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run EULER-NAV BAHRS.</source>
      <translation>配置EULER-NAV BAHRS在哪个串口上运行。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Iridium (with MAVLink).</source>
      <translation>配置在哪个串行端口上运行 Iridium（使用 MAVLink）。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Main GPS.</source>
      <translation>配置在哪个串口上运行Main GPS。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Secondary GPS.</source>
      <translation>配置在哪个串行端口上运行辅助 GPS。</translation>
    </message>
    <message>
      <source>Configure second compass so it is attached externally. This is auto-detected on PX4 and Pixhawk. If set to 0 or 1 then auto-detection by bus connection can override the value. If set to 2 then auto-detection will be disabled.</source>
      <translation>配置第二个罗盘，使其连接到外部。这是在 PX4 和 Pixhawk 上自动检测到的。如果设置为 0 或 1，则通过总线连接自动检测可以覆盖该值。如果设置为 2，则自动检测将被禁用。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the GPS 2 Serial Port.
Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 GPS 2 串口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the TELEM 2 Serial Port.
Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 TELEM 2 串口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the Wifi Port Serial Port.
Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 Wifi 端口串行端口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the output rate of u-blox GPS receivers (protocol v27+). When set to 0, automatic rate selection is used based on the receiver model. Default rates: M9N=8Hz, F9P L1L2=5Hz, F9P L1L5=5Hz, Others=10Hz. Note: Higher rates reduce satellite count (e.g., &gt;8Hz limits to 16 SVs on M9N). Max rates vary by model and RTK mode: F9P L1L2=5-7Hz, F9P L1L5=7-8Hz, X20=25Hz. High rates at 115200 baud may cause dropouts.</source>
      <translation>配置 u-blox GPS 接收器的输出速率（协议 v27+）。当设置为 0 时，将根据接收器型号使用自动速率选择。默认费率：M9N=8Hz、F9P、L1L2=5Hz、F9P、L1L5=5Hz、其他=10Hz。注意：较高的速率会减少卫星数量（例如，&gt;8Hz 限制为 M9N 上的 16 SV）。最大速率因型号和 RTK 模式而异：F9P L1L2=5-7Hz、F9P L1L5=7-8Hz、X20=25Hz。 115200 波特率的高速率可能会导致丢包。</translation>
    </message>
    <message>
      <source>Configures whether arming is allowed without GNSS, for modes that require a global position
(specifically, in those modes when a check defined by EKF2_GPS_CHECK fails).
The settings deny arming and warn, allow arming and warn, or silently allow arming.</source>
      <translation>配置是否允许在没有 GNSS 的情况下对需要全局位置的模式进行解锁（特别是在 EKF2_GPS_CHECK 定义的检查失败时的模式中）。设置拒绝解锁并警告、允许解锁并警告或静默允许解锁。</translation>
    </message>
    <message>
      <source>Console protocol selection</source>
      <translation>控制台协议选择</translation>
    </message>
    <message>
      <source>Continue if in Auto on RC and/or GCS failsafe</source>
      <translation>如果 RC 和/或 GCS 故障安全处于自动状态，则继续</translation>
    </message>
    <message>
      <source>Continue the mission even after comms are recovered (does not go to the mission item at the time comms were lost)</source>
      <translation>即使通讯恢复后仍继续执行任务（通讯丢失时不会转到任务项目）</translation>
    </message>
    <message>
      <source>Control Surface 0 pitch torque scaling</source>
      <translation>控制面 0 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 1 pitch torque scaling</source>
      <translation>控制面 1 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 2 pitch torque scaling</source>
      <translation>控制面 2 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 3 pitch torque scaling</source>
      <translation>控制面 3 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 4 pitch torque scaling</source>
      <translation>控制面 4 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 5 pitch torque scaling</source>
      <translation>控制面 5 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 6 pitch torque scaling</source>
      <translation>控制面 6 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control Surface 7 pitch torque scaling</source>
      <translation>控制面 7 桨距扭矩缩放</translation>
    </message>
    <message>
      <source>Control allocation method</source>
      <translation>控制分配方法</translation>
    </message>
    <message>
      <source>Control over UART options. The InvertRX option controls invert of the receive pin. The InvertTX option controls invert of the transmit pin. The HalfDuplex option controls half-duplex (onewire) mode, where both transmit and receive is done on the transmit wire.</source>
      <translation>控制 UART 选项。 InvertRX 选项控制接收引脚的反转。 InvertTX 选项控制发送引脚的反转。 HalfDuplex 选项控制半双工（单线）模式，其中发送和接收均在发送线上完成。</translation>
    </message>
    <message>
      <source>Control surface launch lock enabled</source>
      <translation>已启用控制表面启动锁定</translation>
    </message>
    <message>
      <source>Control what protocol Serial9 port should be used for. Note that the Frsky options require external converter hardware. See the wiki for details.</source>
      <translation>控制 Serial9 端口应使用什么协议。请注意，Frsky 选项需要外部转换器硬件。详细信息请参阅维基百科。</translation>
    </message>
    <message>
      <source>Controller integrator during takeoff</source>
      <translation>起飞期间的控制器积分器</translation>
    </message>
    <message>
      <source>Controls enabling monitoring of the battery's voltage and current</source>
      <translation>可监控电池电压和电流的控件</translation>
    </message>
    <message>
      <source>Controls if the autopilot should automatically configure the GPS based on the parameters and default settings</source>
      <translation>控制自动驾驶仪是否应根据参数和默认设置自动配置 GPS</translation>
    </message>
    <message>
      <source>Controls startup configuration behavior. 1 writes configuration once then clears to 0. 2 checks current configuration and writes only if needed.</source>
      <translation>控制启动配置行为。 1 写入配置一次，然后清零至 0。 2 检查当前配置并仅在需要时写入。</translation>
    </message>
    <message>
      <source>Controls the number of steps between minimum and maximum joystick gain when the gain is adjusted using buttons. Set to 1 to always use JS_GAIN_DEFAULT.</source>
      <translation>当使用按钮调整增益时，控制最小和最大操纵杆增益之间的步数。设置为 1 以始终使用 JS_GAIN_DEFAULT。</translation>
    </message>
    <message>
      <source>Controls use of forward motor in vtol modes. If this is zero then the forward motor will not be used for position control in VTOL modes. A value of 0.05 is a good place to start if you want to use the forward motor for position control. No forward motor will be used in QSTABILIZE or QHOVER modes. Use QLOITER for position hold with the forward motor.</source>
      <translation>控制垂直起降模式下前进电机的使用。如果为零，则正转电机将不会用于 VTOL 模式下的位置控制。如果您想使用正向电机进行位置控制，0.05 的值是一个很好的起点。 QSTABILIZE 或 QHOVER 模式下不会使用正向电机。使用 QLOITER 与正转电机进行位置保持。</translation>
    </message>
    <message>
      <source>Controls what types of LEDs will be enabled</source>
      <translation>控制将启用哪些类型的 LED</translation>
    </message>
    <message>
      <source>Controls which axis are being excited.  Set to non-zero to see more parameters</source>
      <translation>控制哪个轴被激励。  设置为非零以查看更多参数</translation>
    </message>
    <message>
      <source>Converts the difference between desired forward speed and actual speed into an acceleration target that is passed to the pitch angle controller.</source>
      <translation>将所需前进速度与实际速度之间的差值转换为加速度目标，并传递给桨距角控制器。</translation>
    </message>
    <message>
      <source>Correct when using range finder height</source>
      <translation>使用测距仪高度时正确</translation>
    </message>
    <message>
      <source>Crash Dump arming check active</source>
      <translation>故障转储启用检查已激活</translation>
    </message>
    <message>
      <source>Crash velocity minimum</source>
      <translation>最小碰撞速度</translation>
    </message>
    <message>
      <source>Crossfire</source>
      <translation>交叉火力</translation>
    </message>
    <message>
      <source>Cruise throttle during landing approach (percentage)</source>
      <translation>着陆进近时巡航油门（百分比）</translation>
    </message>
    <message>
      <source>Current-based compensation (battery_status instance 1)</source>
      <translation>基于电流的补偿（battery_status实例1）</translation>
    </message>
    <message>
      <source>Custom Euler Angle</source>
      <translation>定制欧拉角</translation>
    </message>
    <message>
      <source>Custom orientation roll offset</source>
      <translation>自定义方向辊偏移</translation>
    </message>
    <message>
      <source>Custom2</source>
      <translation>定制2</translation>
    </message>
    <message>
      <source>Cutoff frequency for angular acceleration (D-Term filter)</source>
      <translation>角加速度的截止频率（D 项滤波器）</translation>
    </message>
    <message>
      <source>Cyphal ESC 2 zubax feedback port ID</source>
      <translation>Cyphal ESC 2 zubax 反馈端口 ID</translation>
    </message>
    <message>
      <source>Cyphal GPS publication port ID</source>
      <translation>Cyphal GPS 发布端口 ID</translation>
    </message>
    <message>
      <source>D gain for the speed PID controller velocity component</source>
      <translation>速度 PID 控制器速度分量的 D 增益</translation>
    </message>
    <message>
      <source>DDS UDP port</source>
      <translation>DDS UDP 端口</translation>
    </message>
    <message>
      <source>DHCP client</source>
      <translation>DHCP客户端</translation>
    </message>
    <message>
      <source>DIA_Y in the magnetometer soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>磁力计软铁校准矩阵中的DIA_Y：[[DIA_X、ODI_X、ODI_Y]、[ODI_X、DIA_Y、ODI_Z]、[ODI_Y、ODI_Z、DIA_Z]]</translation>
    </message>
    <message>
      <source>DIST_X</source>
      <translation>DIST_X</translation>
    </message>
    <message>
      <source>DRIFT Mode</source>
      <translation>DRIFT模式</translation>
    </message>
    <message>
      <source>DShot output rate for all outputs as a multiple of the loop rate. 0 sets the output rate to be fixed at 1Khz for low loop rates. This value should never be set below 500Hz.</source>
      <translation>所有输出的 DShot 输出速率为循环速率的倍数。 0 将输出速率设置为固定为 1Khz，以实现低循环速率。该值绝不能设置为低于 500Hz。</translation>
    </message>
    <message>
      <source>Damping Gain</source>
      <translation>阻尼增益</translation>
    </message>
    <message>
      <source>Damping ratio for L1 control. Increase this in increments of 0.05 if you are getting overshoot in path tracking. You should not need a value below 0.7 or above 0.85.</source>
      <translation>L1 控制的阻尼比。如果您在路径跟踪中出现超调，请以 0.05 为增量增加此值。您不需要低于 0.7 或高于 0.85 的值。</translation>
    </message>
    <message>
      <source>Dataman disabled</source>
      <translation>数据人已禁用</translation>
    </message>
    <message>
      <source>Deadreckoning Altitude Min</source>
      <translation>航位推算最低海拔高度</translation>
    </message>
    <message>
      <source>Deadreckoning flight timeout</source>
      <translation>航位推算飞行超时</translation>
    </message>
    <message>
      <source>Deck Size</source>
      <translation>甲板尺寸</translation>
    </message>
    <message>
      <source>Deepstall approach extension</source>
      <translation>深失速进场延伸</translation>
    </message>
    <message>
      <source>Deepstall slope a</source>
      <translation>深失速坡度a</translation>
    </message>
    <message>
      <source>Default acceptance radius, overridden by acceptance radius of waypoint if set. For fixed wing the npfg switch distance is used for horizontal acceptance.</source>
      <translation>默认接受半径，如果设置，则被航路点的接受半径覆盖。对于固定翼，npfg 开关距离用于水平验收。</translation>
    </message>
    <message>
      <source>Default horizontal velocity limit</source>
      <translation>默认水平速度限制</translation>
    </message>
    <message>
      <source>Default relay state</source>
      <translation>默认继电器状态</translation>
    </message>
    <message>
      <source>Default value of true airspeed used in EKF-GSF AHRS calculation</source>
      <translation>EKF-GSF AHRS 计算中使用的真实空速默认值</translation>
    </message>
    <message>
      <source>Define if this servo is used for additional control.</source>
      <translation>定义该舵机是否用于附加控制。</translation>
    </message>
    <message>
      <source>Defines a cone above home which determines maximum climb</source>
      <translation>在 home 上方定义一个圆锥体，确定最大爬升</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL10_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL10_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL18_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL18_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL25_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL25_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL32_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL32_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL3_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL3_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL47_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL47_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL54_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL54_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL61_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL61_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL0_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL0_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL17_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL17_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL24_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL24_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL31_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL31_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL39_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL39_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL46_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL46_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL53_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL53_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL60_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL60_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL{n}_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL{n}_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL16_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL16_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL23_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL23_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL30_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL30_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL38_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL38_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL45_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL45_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL52_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL52_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL5_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL5_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL9_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL9_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL15_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL15_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL22_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL22_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL2_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL2_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL37_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL37_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL44_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL44_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL51_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL51_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL59_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL59_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL8_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL8_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the collective pitch at the interval position 1 for a given thrust setpoint.
Use negative values if the swash plate needs to move down to provide upwards thrust.</source>
      <translation>定义给定推力设定值的间隔位置 1 处的总距桨距。如果斜盘需要向下移动以提供向上的推力，请使用负值。</translation>
    </message>
    <message>
      <source>Defines the delay between the current time and the delayed-time horizon. This value should be at least as large as the largest EKF2_XXX_DELAY parameter.</source>
      <translation>定义当前时间和延迟时间范围之间的延迟。该值至少应与最大 EKF2_XXX_DELAY 参数一样大。</translation>
    </message>
    <message>
      <source>Defines the half-angle of a cone centered around the destination position that affects the altitude at which the vehicle returns.</source>
      <translation>定义以目标位置为中心的圆锥体的半角，该半角会影响车辆返回的高度。</translation>
    </message>
    <message>
      <source>Defines the minimum D gain</source>
      <translation>定义最小 D 增益</translation>
    </message>
    <message>
      <source>Defines the output throttle at the interval position 3.</source>
      <translation>定义间隔位置 3 处的输出油门。</translation>
    </message>
    <message>
      <source>Defines the slew rate of the puller/pusher throttle during transitions.
Zero will deactivate the slew rate limiting and thus produce an instant throttle
rise to the transition throttle VT_F_TRANS_THR.</source>
      <translation>定义转换过程中拉/推油门的转换速率。零将停用转换速率限制，从而产生瞬时油门上升至过渡油门 VT_F_TRANS_THR。</translation>
    </message>
    <message>
      <source>Defines the tilt angle when the servo is at the maximum. An angle of zero means upwards.</source>
      <translation>定义舵机最大时的倾斜角度。角度为零意味着向上。</translation>
    </message>
    <message>
      <source>Defines which ODR rate to use during data polling.</source>
      <translation>定义数据轮询期间使用的 ODR 速率。</translation>
    </message>
    <message>
      <source>Degrees of elevator added for full throttle application. Increase to compensate for throttle causing down pitch.</source>
      <translation>为全油门应用添加了升降舵度数。增加以补偿导致俯仰下降的油门。</translation>
    </message>
    <message>
      <source>Delay after reaching ground idle when armed to allow ESC startup to complete.</source>
      <translation>准备好后达到接地空闲后的延迟，以允许 ESC 启动完成。</translation>
    </message>
    <message>
      <source>Delay before switching back to using airspeed sensor if checks indicate sensor is good.
Set to a negative value to disable the re-enabling in flight.</source>
      <translation>如果检查表明传感器良好，则在切换回使用空速传感器之前延迟。设置为负值可禁用飞行中的重新启用。</translation>
    </message>
    <message>
      <source>Demix heli servos and send roll/pitch/collective/yaw</source>
      <translation>混合直升机舵机系统并发送横滚/俯仰/总距/偏航</translation>
    </message>
    <message>
      <source>Derating speed</source>
      <translation>降额速度</translation>
    </message>
    <message>
      <source>Describes your hardware RF In/Out capabilities.</source>
      <translation>描述您的硬件 RF 输入/输出功能。</translation>
    </message>
    <message>
      <source>Destination for GPS_INJECT_DATA MAVLink packets</source>
      <translation>GPS_INJECT_DATA MAVLink 数据包的目的地</translation>
    </message>
    <message>
      <source>Determines if the gyro reading updates are stopped when for an IMU simulated failure by ACCELx_FAIL params</source>
      <translation>通过 ACCELx_FAIL 参数确定 IMU 模拟故障时陀螺仪读数更新是否停止</translation>
    </message>
    <message>
      <source>Determines which of the accuracy measures Horizontal position, Vertical Position and Speed are used to calculate the weighting on each GPS receiver when soft switching has been selected by setting GPS_AUTO_SWITCH to 2</source>
      <translation>当通过设置 GPS_AUTO_SWITCH 至 2 选择软开关时，确定使用水平位置、垂直位置和速度中的哪一个精度测量来计算每个 GPS 接收器上的权重</translation>
    </message>
    <message>
      <source>Device ID of simulated compass 4</source>
      <translation>模拟罗盘4的装置ID</translation>
    </message>
    <message>
      <source>Device ID of the magnetometer this calibration applies to.</source>
      <translation>该校准适用的磁力计设备ID。</translation>
    </message>
    <message>
      <source>Differential pressure sensor analog scaling</source>
      <translation>差压传感器模拟缩放</translation>
    </message>
    <message>
      <source>Differential thrust in forward flight is enabled via VT_FW_DIFTHR_EN.</source>
      <translation>向前飞行时的差动推力通过 VT_FW_DIFTHR_EN 启用。</translation>
    </message>
    <message>
      <source>DifferentialSpoilerRight1</source>
      <translation>差速器扰流板右1</translation>
    </message>
    <message>
      <source>Digital pin number for 4th relay control. Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.</source>
      <translation>第四个继电器控制的数字引脚号。给出了一些常见值，但请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。</translation>
    </message>
    <message>
      <source>Digital pin number for first relay control. This is the pin used for camera shutter control. Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.</source>
      <translation>用于第一个继电器控制的数字引脚号。这是用于相机快门控制的引脚。给出了一些常见值，但请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。</translation>
    </message>
    <message>
      <source>Digital pin that enables/disables rangefinder measurement for an analog rangefinder. A value of -1 means no pin. If this is set, then the pin is set to 1 to enable the rangefinder and set to 0 to disable it. This can be used to ensure that multiple sonar rangefinders don't interfere with each other.</source>
      <translation>用于启用/禁用模拟测距仪测距仪测量的数字引脚。 -1 的值表示无引脚。如果已设置，则该引脚将设置为 1 以启用测距仪，并设置为 0 以禁用它。这可用于确保多个声纳测距仪不会相互干扰。</translation>
    </message>
    <message>
      <source>Direct feed forward from rate setpoint to control surface output</source>
      <translation>从速率设定点直接前馈到控制面输出</translation>
    </message>
    <message>
      <source>Direction of Flight</source>
      <translation>飞行方向</translation>
    </message>
    <message>
      <source>Disable CrashDump Arming check</source>
      <translation>禁用 CrashDump 准备检查</translation>
    </message>
    <message>
      <source>Disable Qassist-based on synthetic airspeed even if airspeed sensor is used</source>
      <translation>即使使用空速传感器，也禁用基于合成空速的 Qassist</translation>
    </message>
    <message>
      <source>Disable manual control</source>
      <translation>禁用手动控制</translation>
    </message>
    <message>
      <source>Disable redline governor</source>
      <translation>禁用红线调节器</translation>
    </message>
    <message>
      <source>Disable this if the system has no GPS.
If disabled, the sensors hub will not process sensor_gps,
and GPS will not be available for the rest of the system.</source>
      <translation>如果系统没有 GPS，则禁用此功能。如果禁用，传感器集线器将不会处理sensor_gps，并且 GPS 将不可用于系统的其余部分。</translation>
    </message>
    <message>
      <source>Disable yaw imbalance warning</source>
      <translation>禁用偏航不平衡警告</translation>
    </message>
    <message>
      <source>DisableFIFO</source>
      <translation>禁用先进先出</translation>
    </message>
    <message>
      <source>Disabled</source>
      <translation>禁用</translation>
    </message>
    <message>
      <source>Disables motor PWM output when disarmed</source>
      <translation>上锁时禁用电机 PWM 输出</translation>
    </message>
    <message>
      <source>DisarmOnLowThrottle</source>
      <translation>低油门解除</translation>
    </message>
    <message>
      <source>DisplayPort</source>
      <translation>显示端口</translation>
    </message>
    <message>
      <source>Displays RC link signal strength in dBm</source>
      <translation>显示 RC 链路信号强度（以 dBm 为单位）</translation>
    </message>
    <message>
      <source>Displays airspeed reported directly from primary airspeed sensor</source>
      <translation>显示直接从主空速传感器报告的空速</translation>
    </message>
    <message>
      <source>Displays average cell voltage (MSP OSD only)</source>
      <translation>显示平均电池电压（仅限 MSP OSD）</translation>
    </message>
    <message>
      <source>Displays crosstrack error</source>
      <translation>显示交叉轨迹错误</translation>
    </message>
    <message>
      <source>Displays flight efficiency (mAh/km or /mi)</source>
      <translation>显示飞行效率（mAh/km 或 /mi）</translation>
    </message>
    <message>
      <source>Displays main battery resting voltage</source>
      <translation>显示主电池静止电压</translation>
    </message>
    <message>
      <source>Displays relative direction to HOME (MSP OSD only)</source>
      <translation>显示相对于 HOME 的方向（仅限 MSP OSD）</translation>
    </message>
    <message>
      <source>Displays the current RC link active antenna</source>
      <translation>显示当前RC链路活动天线</translation>
    </message>
    <message>
      <source>Distance (in cm) from the second range finder to the ground</source>
      <translation>第二个测距仪到地面的距离（厘米）</translation>
    </message>
    <message>
      <source>Distance based, always on</source>
      <translation>基于距离，始终基于</translation>
    </message>
    <message>
      <source>Distance from landing point (HOME) to final approach waypoint created by script in the opposite direction of initial takeoff</source>
      <translation>从着陆点 (HOME) 到脚本在初始起飞相反方向创建的最终进近航路点的距离</translation>
    </message>
    <message>
      <source>Distance that autopilot's should maintain from the fence in the horizontal plane to avoid a breach. If set to 0 then FENCE_MARGIN is used.</source>
      <translation>自动驾驶仪与围栏在水平面上应保持的距离，以避免突破。如果设置为 0，则使用 FENCE_MARGIN。</translation>
    </message>
    <message>
      <source>Do not apply the new gains (logging only)</source>
      <translation>不应用新增益（仅记录）</translation>
    </message>
    <message>
      <source>Do not stabilize PositionXY</source>
      <translation>不稳定 PositionXY</translation>
    </message>
    <message>
      <source>Dock mode heading correction weight</source>
      <translation>停靠模式航向修正重量</translation>
    </message>
    <message>
      <source>Don't log Dt stats</source>
      <translation>不记录 Dt 统计数据</translation>
    </message>
    <message>
      <source>Downed</source>
      <translation>被击落</translation>
    </message>
    <message>
      <source>Driving mode for switch position 4 (1491 to 1620)</source>
      <translation>开关位置 4（1491 至 1620）的驱动模式</translation>
    </message>
    <message>
      <source>DroneCAN Hardpoint ID 11</source>
      <translation>DroneCAN 硬点 ID 11</translation>
    </message>
    <message>
      <source>DroneCAN Hardpoint ID 5</source>
      <translation>DroneCAN 硬点 ID 5</translation>
    </message>
    <message>
      <source>DroneCAN Serial1 index</source>
      <translation>DroneCAN Serial1 索引</translation>
    </message>
    <message>
      <source>DroneCAN sensor ID to assign to this backend</source>
      <translation>DroneCAN 传感器 ID 分配给该后端</translation>
    </message>
    <message>
      <source>Dual antenna heading</source>
      <translation>双天线航向</translation>
    </message>
    <message>
      <source>Duration of choking during startup</source>
      <translation>启动过程中窒息的持续时间</translation>
    </message>
    <message>
      <source>During the choking and the starting phase, the throttle value is set to this value.</source>
      <translation>在阻塞和启动阶段，油门值设置为该值。</translation>
    </message>
    <message>
      <source>ECU command output rate</source>
      <translation>ECU命令输出速率</translation>
    </message>
    <message>
      <source>EFI DLA fuel scale</source>
      <translation>EFI DLA 燃油秤</translation>
    </message>
    <message>
      <source>EFI INF-Inject options</source>
      <translation>EFI INF-注入选项</translation>
    </message>
    <message>
      <source>EKF Pos Source</source>
      <translation>EKF 位置源</translation>
    </message>
    <message>
      <source>EKF Source Rangefinder Max</source>
      <translation>EKF 源测距仪最大</translation>
    </message>
    <message>
      <source>EKF sensor logging IMU mask</source>
      <translation>EKF 传感器记录 IMU 面罩</translation>
    </message>
    <message>
      <source>EKF2 selector maximum accumulated velocity threshold for comparing accelerometers. Accumulated velocity error larger than this will result in the sensor being declared faulty.</source>
      <translation>EKF2 选择器用于比较加速度计的最大累积速度阈值。累积速度误差大于此值将导致传感器被宣告有故障。</translation>
    </message>
    <message>
      <source>EPM</source>
      <translation>EPM</translation>
    </message>
    <message>
      <source>ESC 1</source>
      <translation>ESC 1</translation>
    </message>
    <message>
      <source>ESC 17</source>
      <translation>ESC 17</translation>
    </message>
    <message>
      <source>ESC 24</source>
      <translation>ESC 24</translation>
    </message>
    <message>
      <source>ESC 31</source>
      <translation>ESC 31</translation>
    </message>
    <message>
      <source>ESC Calibration</source>
      <translation>电调校准</translation>
    </message>
    <message>
      <source>ESC Telemetry Index to write to</source>
      <translation>要写入的 ESC 遥测索引</translation>
    </message>
    <message>
      <source>ESCAMPS_EN</source>
      <translation>ESCAMPS_EN</translation>
    </message>
    <message>
      <source>ESCTEMP_Y</source>
      <translation>ESCTEMP_Y</translation>
    </message>
    <message>
      <source>Early landing configuration deployment</source>
      <translation>提前落地配置部署</translation>
    </message>
    <message>
      <source>ElevatorWithInput</source>
      <translation>电梯输入</translation>
    </message>
    <message>
      <source>EmergencySurface</source>
      <translation>应急面</translation>
    </message>
    <message>
      <source>Enable</source>
      <translation>启用</translation>
    </message>
    <message>
      <source>Enable Actuator Testing</source>
      <translation>启用执行器测试</translation>
    </message>
    <message>
      <source>Enable Compass</source>
      <translation>启用罗盘</translation>
    </message>
    <message>
      <source>Enable EFI DLA driver</source>
      <translation>启用 EFI DLA 驱动程序</translation>
    </message>
    <message>
      <source>Enable FMU SD card detection check</source>
      <translation>启用 FMU SD 卡检测检查</translation>
    </message>
    <message>
      <source>Enable GPS2 output of NMEA heading HDT sentence or UBLOX_RELPOSNED</source>
      <translation>使能 GPS2 输出 NMEA 标题 HDT 句子或 UBLOX_RELPOSNED</translation>
    </message>
    <message>
      <source>Enable I2C input protocol UBX</source>
      <translation>启用 I2C 输入协议 UBX</translation>
    </message>
    <message>
      <source>Enable INA238 Power Monitor</source>
      <translation>启用 INA238 功率监视器</translation>
    </message>
    <message>
      <source>Enable Manual Autorotations</source>
      <translation>启用手动自动旋转</translation>
    </message>
    <message>
      <source>Enable MovingBaselineData publication</source>
      <translation>启用 MovingBaselineData 发布</translation>
    </message>
    <message>
      <source>Enable RC Termination only in manual control modes</source>
      <translation>仅在手动控制模式下启用 RC 终止</translation>
    </message>
    <message>
      <source>Enable Scripting</source>
      <translation>启用脚本</translation>
    </message>
    <message>
      <source>Enable UAVCAN GPS Relative subscription. ardupilot::gnss::RelPosHeading</source>
      <translation>启用 UAVCAN GPS 相对订阅。 ardupilot::gnss::RelPosHeading</translation>
    </message>
    <message>
      <source>Enable UAVCAN RTCM stream publication uavcan::equipment::gnss::RTCMStream</source>
      <translation>启用 UAVCAN RTCM 流发布 uavcan::equipment::gnss::RTCMStream</translation>
    </message>
    <message>
      <source>Enable UAVCAN button subscription. ardupilot::indication::Button</source>
      <translation>启用 UAVCAN 按钮订阅。 ardupilot::指示::按钮</translation>
    </message>
    <message>
      <source>Enable UAVCAN mag subscription.
uavcan::equipment::ahrs::MagneticFieldStrength
uavcan::equipment::ahrs::MagneticFieldStrength2</source>
      <translation>启用 UAVCAN 杂志订阅。 uavcan::设备::ahrs::MagneticFieldStrength uavcan::设备::ahrs::MagneticFieldStrength2</translation>
    </message>
    <message>
      <source>Enable Zenoh</source>
      <translation>启用 Zenoh</translation>
    </message>
    <message>
      <source>Enable attitude locking when sticks are released. If set to 2 then quaternion based locking is used if the yaw rate controller is enabled. Quaternion based locking will hold any attitude</source>
      <translation>释放摇杆时启用姿态锁定。如果设置为 2，则在启用偏航率控制器的情况下使用基于四元数的锁定。基于四元数的锁定将保持任何态度</translation>
    </message>
    <message>
      <source>Enable automatic lower bound on the NPFG period</source>
      <translation>启用 NPFG 周期的自动下限</translation>
    </message>
    <message>
      <source>Enable checks on ESCs that report telemetry</source>
      <translation>启用对报告遥测数据的 ESC 的检查</translation>
    </message>
    <message>
      <source>Enable factory calibration mode</source>
      <translation>启用工厂校准模式</translation>
    </message>
    <message>
      <source>Enable flow control on serial 4. You must have the RTS and CTS pins connected to your radio. The standard DF13 6 pin connector for a 3DR radio does have those pins connected. If this is set to 2 then flow control will be auto-detected by checking for the output buffer filling on startup.</source>
      <translation>启用串行 4 上的流量控制。您必须将 RTS 和 CTS 引脚连接到您的无线电。用于 3DR 无线电的标准 DF13 6 引脚连接器确实已连接这些引脚。如果设置为 2，则将通过在启动时检查输出缓冲区填充来自动检测流量控制。</translation>
    </message>
    <message>
      <source>Enable ground control station telemetry failsafe. Failsafe will trigger after FS_LONG_TIMEOUT seconds of no MAVLink heartbeat messages. There are three possible enabled settings. Seeing FS_GCS_ENABL to 1 means that GCS failsafe will be triggered when the aircraft has not received a MAVLink HEARTBEAT message. Setting FS_GCS_ENABL to 2 means that GCS failsafe will be triggered on either a loss of HEARTBEAT messages, or a RADIO_STATUS message from a MAVLink enabled 3DR radio indicating that the ground station is not receiving status updates from the aircraft, which is indicated by the RADIO_STATUS.remrssi field being zero (this may happen if you have a one way link due to asymmetric noise on the ground station and aircraft radios).Setting FS_GCS_ENABL to 3 means that GCS failsafe will be triggered by Heartbeat(like option one), but only in AUTO mode. WARNING: Enabling this option opens up the possibility of your plane going into failsafe mode and running the motor on the ground it it loses contact with your ground station. If this option is enabled on an electric plane then you should enable ARMING_REQUIRED.</source>
      <translation>启用地面控制站遥测故障保护。故障保护将在 FS_LONG_TIMEOUT 秒没有 MAVLink 心跳消息后触发。共有三种可能的启用设置。看到FS_GCS_ENABL到1意味着当飞机没有收到MAVLink HEARTBEAT消息时，GCS故障保护将被触发。将 FS_GCS_ENABL 设置为 2 意味着 GCS 故障保护将在 HEARTBEAT 消息丢失或来自启用 MAVLink 的 3DR 无线电的 RADIO_STATUS 消息时触发，表明地面站未从飞机接收状态更新，这由 RADIO_STATUS.remrssi 字段为零表示（如果由于地面站和飞机无线电上的不对称噪声而有单向链路，则可能会发生这种情况）。 FS_GCS_ENABL 到 3 意味着 GCS 故障保护将由 Heartbeat 触发（如选项一），但仅限于 AUTO 模式。 WARNING：启用此选项可以使您的飞机进入故障安全模式并在与地面站失去联系时在地面上运行电机。如果在电动飞机上启用此选项，则应启用 ARMING_REQUIRED。</translation>
    </message>
    <message>
      <source>Enable landing gear control</source>
      <translation>启用起落架控制</translation>
    </message>
    <message>
      <source>Enable notch filter</source>
      <translation>启用陷波滤波器</translation>
    </message>
    <message>
      <source>Enable or disable a stuck tether simulation</source>
      <translation>启用或禁用卡住系绳模拟</translation>
    </message>
    <message>
      <source>Enable or disable the buzzer. Only for Linux and PX4 based boards.</source>
      <translation>启用或禁用蜂鸣器。仅适用于基于 Linux 和 PX4 的板。</translation>
    </message>
    <message>
      <source>Enable or disable the use of the compass (instead of the GPS) for determining heading</source>
      <translation>启用或禁用使用罗盘（而不是 GPS）来确定航向</translation>
    </message>
    <message>
      <source>Enable post-filter FFT</source>
      <translation>启用后置滤波器 FFT</translation>
    </message>
    <message>
      <source>Enable rate gain compression</source>
      <translation>启用速率增益压缩</translation>
    </message>
    <message>
      <source>Enable serial flow control for instance 1</source>
      <translation>为实例 1 启用串行流控制</translation>
    </message>
    <message>
      <source>Enable simulated GPS jamming</source>
      <translation>启用模拟 GPS 干扰</translation>
    </message>
    <message>
      <source>Enable software throttling of mavlink on instance 1</source>
      <translation>在实例 1 上启用 mavlink 的软件限制</translation>
    </message>
    <message>
      <source>Enable target distance</source>
      <translation>启用目标距离</translation>
    </message>
    <message>
      <source>Enable the fast Rate thread</source>
      <translation>启用快速线程</translation>
    </message>
    <message>
      <source>Enable throttle scale by battery level</source>
      <translation>根据电池电量启用油门比例</translation>
    </message>
    <message>
      <source>Enable use of yaw stick for nudging the wheel during runway ground roll</source>
      <translation>允许在跑道地面滑跑期间使用偏航杆轻推车轮</translation>
    </message>
    <message>
      <source>Enable yaw rate controller for aerobatic flight</source>
      <translation>为特技飞行启用偏航率控制器</translation>
    </message>
    <message>
      <source>Enable/disable auto tuning using a manual control AUX input</source>
      <translation>使用手动控制 AUX 输入启用/禁用自动调谐</translation>
    </message>
    <message>
      <source>EnableBTFLFonts</source>
      <translation>启用BTF字体</translation>
    </message>
    <message>
      <source>EnableGPSAffinity</source>
      <translation>启用GPS亲和力</translation>
    </message>
    <message>
      <source>EnablePPP Ethernet gateway</source>
      <translation>启用 PPP 以太网网关</translation>
    </message>
    <message>
      <source>Enabled (conditional mode)</source>
      <translation>启用（条件模式）</translation>
    </message>
    <message>
      <source>Enabled always Land</source>
      <translation>始终降落</translation>
    </message>
    <message>
      <source>Enabled if above MPC_LAND_ALT1</source>
      <translation>如果高于 MPC_LAND_ALT1，则启用</translation>
    </message>
    <message>
      <source>Enabled on telemetry loss</source>
      <translation>遥测丢失时启用</translation>
    </message>
    <message>
      <source>Enabled/disable stopping at fence</source>
      <translation>启用/禁用在栅栏处停止</translation>
    </message>
    <message>
      <source>Enables automatic launch detection based on measured acceleration. Use for hand- or catapult-launched vehicles.
Not compatible with runway takeoff.</source>
      <translation>启用基于测量的加速度的自动启动检测。用于手动或弹射发射的车辆。与跑道起飞不兼容。</translation>
    </message>
    <message>
      <source>Enables optional Loiter mode behaviors</source>
      <translation>启用可选的盘旋模式行为</translation>
    </message>
    <message>
      <source>Enables temperature sensor logging</source>
      <translation>启用温度传感器记录</translation>
    </message>
    <message>
      <source>Enables user input during LAND mode, the landing phase of RTL, and auto mode landings.</source>
      <translation>在 LAND 模式、RTL 着陆阶段和自动模式着陆期间启用用户输入。</translation>
    </message>
    <message>
      <source>End frequency of the injected signal</source>
      <translation>注入信号的结束频率</translation>
    </message>
    <message>
      <source>EnforcePreArmChecks</source>
      <translation>强制预解锁检查</translation>
    </message>
    <message>
      <source>Ensure:
gripper: NAV_CMD_DO_GRIPPER
has released before continuing mission.
winch: CMD_DO_WINCH
has delivered before continuing mission.
gimbal: CMD_DO_GIMBAL_MANAGER_PITCHYAW
has reached the commanded orientation before beginning to take pictures.</source>
      <translation>确保：夹具：NAV_CMD_DO_GRIPPER 在继续执行任务之前已释放。绞车：CMD_DO_WINCH 在继续执行任务之前已交付。云台：CMD_DO_GIMBAL_MANAGER_PITCHYAW已到达指令方向，开始拍照。</translation>
    </message>
    <message>
      <source>Error(PreArm)</source>
      <translation>错误（预臂）</translation>
    </message>
    <message>
      <source>Explicitly defines the per cell internal resistance for battery 1</source>
      <translation>明确定义电池 1 的每节电池内阻</translation>
    </message>
    <message>
      <source>Exponential factor for tuning the input curve shape. 0 Purely linear input curve 1 Purely cubic input curve</source>
      <translation>用于调整输入曲线形状的指数因子。 0 纯线性输入曲线 1 纯三次输入曲线</translation>
    </message>
    <message>
      <source>External AHRS sensors bitmask</source>
      <translation>外部 AHRS 传感器位掩码</translation>
    </message>
    <message>
      <source>External Mode 1</source>
      <translation>外部模式 1</translation>
    </message>
    <message>
      <source>External Motor Governor Setpoint</source>
      <translation>外部电机调速器设定值</translation>
    </message>
    <message>
      <source>External mode identifier 1</source>
      <translation>外部模式标识符 1</translation>
    </message>
    <message>
      <source>External only (0xFF00)</source>
      <translation>仅外部（0xFF00）</translation>
    </message>
    <message>
      <source>ExternalNav</source>
      <translation>外部Nav</translation>
    </message>
    <message>
      <source>Extra TECS options</source>
      <translation>额外的 TECS 选项</translation>
    </message>
    <message>
      <source>Extra time delay per main loop</source>
      <translation>每个主循环的额外时间延迟</translation>
    </message>
    <message>
      <source>FBWA taildragger channel</source>
      <translation>FBWA尾拖通道</translation>
    </message>
    <message>
      <source>FETtecOneWire</source>
      <translation>FETtecOneWire</translation>
    </message>
    <message>
      <source>FFT SNR reference threshold in dB at which a signal is determined to be present.</source>
      <translation>FFT SNR 确定存在信号的参考阈值（以 dB 为单位）。</translation>
    </message>
    <message>
      <source>FFT learned bandwidth at hover</source>
      <translation>FFT 悬停时学习带宽</translation>
    </message>
    <message>
      <source>FFT window size</source>
      <translation>FFT窗口尺寸</translation>
    </message>
    <message>
      <source>FLTMODE_EN</source>
      <translation>FLTMODE_EN</translation>
    </message>
    <message>
      <source>FPort Pad</source>
      <translation>F端口垫</translation>
    </message>
    <message>
      <source>FW Mixer Roll</source>
      <translation>FW 搅拌辊</translation>
    </message>
    <message>
      <source>Failsafe Options</source>
      <translation>故障保护选项</translation>
    </message>
    <message>
      <source>Failsafe screen</source>
      <translation>故障保护屏幕</translation>
    </message>
    <message>
      <source>Fallback options</source>
      <translation>后备选项</translation>
    </message>
    <message>
      <source>Fast descend: minimum altitude error</source>
      <translation>快速下降：最小高度误差</translation>
    </message>
    <message>
      <source>Feed-forward compensation to automatically add rudder input when collective pitch is increased. Can be positive or negative depending on mechanics.</source>
      <translation>前馈补偿可在总距增加时自动添加舵输入。可以是正值也可以是负值，具体取决于机制。</translation>
    </message>
    <message>
      <source>Fence Auto-Enable</source>
      <translation>围栏自动启用</translation>
    </message>
    <message>
      <source>Fence Return Altitude</source>
      <translation>围栏返回高度</translation>
    </message>
    <message>
      <source>Fence not enabled</source>
      <translation>围栏未启用</translation>
    </message>
    <message>
      <source>Fifth Gyro bias (Z-axis)</source>
      <translation>第五陀螺仪偏置（Z轴）</translation>
    </message>
    <message>
      <source>File and MAVLink</source>
      <translation>文件和 MAVLink</translation>
    </message>
    <message>
      <source>Filter frequency for flow data</source>
      <translation>流量数据的过滤频率</translation>
    </message>
    <message>
      <source>First DroneCAN GPS NODE ID</source>
      <translation>第一架 DroneCAN GPS NODE ID</translation>
    </message>
    <message>
      <source>First IMU</source>
      <translation>第一个IMU</translation>
    </message>
    <message>
      <source>First battery</source>
      <translation>第一块电池</translation>
    </message>
    <message>
      <source>First order polynomial fit term</source>
      <translation>一阶多项式拟合项</translation>
    </message>
    <message>
      <source>FirstEKF</source>
      <translation>第一EKF</translation>
    </message>
    <message>
      <source>Five PWMs</source>
      <translation>五个 PWM</translation>
    </message>
    <message>
      <source>Fixed wing tiltrotor gain</source>
      <translation>固定翼倾转旋翼增益</translation>
    </message>
    <message>
      <source>Fixed-wing land detector: Max vertiacal velocity threshold</source>
      <translation>固定翼陆地探测器：最大垂直速度阈值</translation>
    </message>
    <message>
      <source>FixedWingPitchP</source>
      <translation>固定翼桨距P</translation>
    </message>
    <message>
      <source>Flap 1 speed</source>
      <translation>襟翼1速度</translation>
    </message>
    <message>
      <source>Flaperon1</source>
      <translation>襟副翼1</translation>
    </message>
    <message>
      <source>Flare holdoff height</source>
      <translation>火炬阻挡高度</translation>
    </message>
    <message>
      <source>Flight Mode 6</source>
      <translation>飞行模式6</translation>
    </message>
    <message>
      <source>Flight mode for switch position 6 (1750 to 2049)</source>
      <translation>开关位置6（1750至2049）的飞行模式</translation>
    </message>
    <message>
      <source>Flight mode when Channel 5 pwm is &gt;=1750</source>
      <translation>通道 5 pwm &gt;=1750 时的飞行模式</translation>
    </message>
    <message>
      <source>FlightAxis options</source>
      <translation>飞行轴选项</translation>
    </message>
    <message>
      <source>Flightmode channel</source>
      <translation>飞行模式频道</translation>
    </message>
    <message>
      <source>FlowHold (horizontal) P gain.</source>
      <translation>FlowHold（水平）P 增益。</translation>
    </message>
    <message>
      <source>FlowHold Integrator Max</source>
      <translation>FlowHold 积分器最大</translation>
    </message>
    <message>
      <source>Flybar Mode Selector</source>
      <translation>Flybar 模式选择器</translation>
    </message>
    <message>
      <source>Follow altitude type</source>
      <translation>跟随高度类型</translation>
    </message>
    <message>
      <source>Follow offsets in meters down.  If positive, this vehicle will fly below the lead vehicle</source>
      <translation>遵循向下米数的偏移。  如果为正，则该车辆将飞到领先车辆下方</translation>
    </message>
    <message>
      <source>Follow offsets should not be left as default (zero) if FOLL_ENABLE = 1. 3 or less to prevent arming. -1 to disable.</source>
      <translation>如果 FOLL_ENABLE = 1，则跟随偏移不应保留为默认值（零）。 3 或以下以防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>Follow target's mavlink system id</source>
      <translation>跟踪目标的 mavlink 系统 ID</translation>
    </message>
    <message>
      <source>For linear wind profile,wind is reduced by (Altitude-WIND_T_ALT) x this value</source>
      <translation>对于线性风廓线，风会减少 (Altitude-WIND_T_ALT) x 该值</translation>
    </message>
    <message>
      <source>For systems a INA238 Power Monitor, this should be set to true</source>
      <translation>对于 INA238 功率监视器系统，应设置为 true</translation>
    </message>
    <message>
      <source>Force IS_Flying</source>
      <translation>原力IS_飞行</translation>
    </message>
    <message>
      <source>Force effective control inputs to trim positions and prevent arming</source>
      <translation>控制输入回中并禁止解锁</translation>
    </message>
    <message>
      <source>Force to the vehicle over a period of time</source>
      <translation>在一段时间内对车辆施加的力</translation>
    </message>
    <message>
      <source>Forward (roll) axis with origin relative to vehicle centre of gravity</source>
      <translation>前向（侧倾）轴，其原点相对于车辆重心</translation>
    </message>
    <message>
      <source>Forward Speed Controller P Gain</source>
      <translation>前进速度控制器 P 增益</translation>
    </message>
    <message>
      <source>Forward throttle battery compensation index</source>
      <translation>前进油门电池补偿指标</translation>
    </message>
    <message>
      <source>Forward throttle external limit slew rate</source>
      <translation>前进油门外部限制摆率</translation>
    </message>
    <message>
      <source>Fourth Gyro bias (X-axis)</source>
      <translation>第四陀螺仪偏置（X 轴）</translation>
    </message>
    <message>
      <source>Fourth Relay Pin</source>
      <translation>第四个继电器引脚</translation>
    </message>
    <message>
      <source>Frame Type</source>
      <translation>机架类型</translation>
    </message>
    <message>
      <source>Frequency at the start of the sweep</source>
      <translation>扫描开始时的频率</translation>
    </message>
    <message>
      <source>Front</source>
      <translation>正面</translation>
    </message>
    <message>
      <source>Fuel level voltage multiplier</source>
      <translation>燃油位电压倍增器</translation>
    </message>
    <message>
      <source>Full stick deflection leads to this rate.</source>
      <translation>全杆偏转导致此速率。</translation>
    </message>
    <message>
      <source>Function assigned to this servo. Setting this to Disabled(0) will setup this output for control by auto missions or MAVLink servo set commands. any other value will enable the corresponding function</source>
      <translation>分配给该舵机器的功能。将此设置为禁用 (0) 将设置此输出以通过自动任务或 MAVLink 舵机设置命令进行控制。任何其他值将启用相应的功能</translation>
    </message>
    <message>
      <source>Fusion reset mode for sensor 2</source>
      <translation>传感器 2 的融合复位模式</translation>
    </message>
    <message>
      <source>GCS timeout to start sendin Rockblock messages</source>
      <translation>GCS 开始发送 Rockblock 消息超时</translation>
    </message>
    <message>
      <source>GNSS kinematic (requires motion, e.g. a GNSS velocity)</source>
      <translation>GNSS 运动学（需要运动，例如 GNSS 速度）</translation>
    </message>
    <message>
      <source>GPGGA</source>
      <translation>GPGGA</translation>
    </message>
    <message>
      <source>GPS 0 subscription port ID</source>
      <translation>GPS 0 订阅端口 ID</translation>
    </message>
    <message>
      <source>GPS 1 Hz</source>
      <translation>GPS 1 Hz</translation>
    </message>
    <message>
      <source>GPS 1 Velocity Error Offsets in NED (X-axis)</source>
      <translation>GPS 1 NED 中的速度误差偏移（X 轴）</translation>
    </message>
    <message>
      <source>GPS 1 lag</source>
      <translation>GPS 1 滞后</translation>
    </message>
    <message>
      <source>GPS 2 Byteloss</source>
      <translation>GPS 2 字节丢失</translation>
    </message>
    <message>
      <source>GPS 2 Position</source>
      <translation>GPS 2 位置</translation>
    </message>
    <message>
      <source>GPS 2 antenna phase center position relative to the body frame origin (Y-axis)</source>
      <translation>GPS 2 天线相位中心相对于机身框架原点（Y轴）的位置</translation>
    </message>
    <message>
      <source>GPS 2D vel and 2D pos</source>
      <translation>GPS 2D 速度和 2D 位置</translation>
    </message>
    <message>
      <source>GPS Configuration</source>
      <translation>GPS配置</translation>
    </message>
    <message>
      <source>GPS Heading</source>
      <translation>GPS 标题</translation>
    </message>
    <message>
      <source>GPS Node ID 2</source>
      <translation>GPS 节点 ID 2</translation>
    </message>
    <message>
      <source>GPS Node id for first-discovered GPS.Renamed in 4.6 and later to GPS1_CAN_NODEID.</source>
      <translation>GPS 首次发现的 GPS 的节点 ID。在 4.6 中重命名，后来更名为 GPS1_CAN_NODEID。</translation>
    </message>
    <message>
      <source>GPS Options bitmask</source>
      <translation>GPS 选项位掩码</translation>
    </message>
    <message>
      <source>GPS accuracy check scaler (%)</source>
      <translation>GPS 精度检查定标器 (%)</translation>
    </message>
    <message>
      <source>GPS antenna phase center position relative to the body frame origin (Y-axis)</source>
      <translation>GPS天线相位中心相对于机身框架原点（Y轴）的位置</translation>
    </message>
    <message>
      <source>GPS failure loiter time</source>
      <translation>GPS故障盘旋时间</translation>
    </message>
    <message>
      <source>GPS horizontal velocity measurement noise (m/s)</source>
      <translation>GPS水平速度测量噪声（m/s）</translation>
    </message>
    <message>
      <source>GPS measurement delay (msec)</source>
      <translation>GPS 测量延迟（毫秒）</translation>
    </message>
    <message>
      <source>GPS preflight check</source>
      <translation>GPS 飞行前检查</translation>
    </message>
    <message>
      <source>GPS vehicle only</source>
      <translation>仅限 GPS 车辆</translation>
    </message>
    <message>
      <source>GPS vertical velocity measurement noise scaler</source>
      <translation>GPS垂直速度测量噪声定标器</translation>
    </message>
    <message>
      <source>GPSLAT_EN</source>
      <translation>GPSLAT_EN</translation>
    </message>
    <message>
      <source>GPS_SATS</source>
      <translation>GPS_SATS</translation>
    </message>
    <message>
      <source>GUIDED</source>
      <translation>引导</translation>
    </message>
    <message>
      <source>Gain from demanded rate to aileron output. </source>
      <translation>从要求的速率到副翼输出的增益。 </translation>
    </message>
    <message>
      <source>Gain of P controller Y</source>
      <translation>P控制器Y的增益</translation>
    </message>
    <message>
      <source>Gate size for GNSS position fusion</source>
      <translation>GNSS位置融合的浇口尺寸</translation>
    </message>
    <message>
      <source>Gate size for auxiliary global position sensor 3 fusion</source>
      <translation>辅助全球位置传感器 3 fusion 的门尺寸</translation>
    </message>
    <message>
      <source>Gate size for true airspeed fusion</source>
      <translation>真正空速融合的门尺寸</translation>
    </message>
    <message>
      <source>Generator Options</source>
      <translation>发电机选项</translation>
    </message>
    <message>
      <source>Geofence source</source>
      <translation>地理围栏来源</translation>
    </message>
    <message>
      <source>Ghost telemetry enable</source>
      <translation>幽灵遥测启用</translation>
    </message>
    <message>
      <source>Glide slope threshold</source>
      <translation>滑翔斜率阈值</translation>
    </message>
    <message>
      <source>Glitch offsets of simulated GPS 2 sensor (X-axis)</source>
      <translation>模拟 GPS 2 传感器的毛刺偏移（X 轴）</translation>
    </message>
    <message>
      <source>Glitch offsets of simulated GPS velocity in NED. The reported position also moves continuously with this velocity, so position and velocity stay consistent (e.g. to simulate GPS spoofing that drifts the position). The accumulated position offset is cleared when all three components are zero at a GPS update, so when changing axes set the new component before zeroing the old one (Z-axis)</source>
      <translation>NED 中模拟 GPS 速度的毛刺偏移。报告的位置也以该速度连续移动，因此位置和速度保持一致（例如，模拟使位置漂移的 GPS 欺骗）。当 GPS 更新时所有三个分量均为零时，累积位置偏移量将被清除，因此当更改轴时，请在将旧分量（Z 轴）归零之前设置新分量</translation>
    </message>
    <message>
      <source>Global gain of the controller. This gain scales the P, I and D terms of the controller: output = MC_ROLLRATE_K * (MC_ROLLRATE_P * error + MC_ROLLRATE_I * error_integral + MC_ROLLRATE_D * error_derivative) Set MC_ROLLRATE_P=1 to implement a PID in the ideal form. Set MC_ROLLRATE_K=1 to implement a PID in the parallel form.</source>
      <translation>控制器的全局增益。该增益可缩放控制器的 P、I 和 D 项：输出 = MC_ROLLRATE_K * (MC_ROLLRATE_P * 误差 + MC_ROLLRATE_I * error_integral + MC_ROLLRATE_D * error_derivative) 设置 MC_ROLLRATE_P=1 以实现理想形式的 PID。设置MC_ROLLRATE_K=1，以并行形式实现PID。</translation>
    </message>
    <message>
      <source>Go directly to landing sequence via DO_RETURN_PATH_START mission item</source>
      <translation>通过 DO_RETURN_PATH_START 任务项目直接进入着陆序列</translation>
    </message>
    <message>
      <source>Governor Droop Compensator</source>
      <translation>调速器下垂补偿器</translation>
    </message>
    <message>
      <source>Gravity vector fusion</source>
      <translation>重力矢量融合</translation>
    </message>
    <message>
      <source>Gripper Release PWM</source>
      <translation>夹具释放 PWM</translation>
    </message>
    <message>
      <source>Ground</source>
      <translation>地面</translation>
    </message>
    <message>
      <source>Ground effect altitude for multicopters</source>
      <translation>多旋翼飞行器的地效高度</translation>
    </message>
    <message>
      <source>Ground steering rate in degrees per second for full rudder stick deflection</source>
      <translation>舵杆完全偏转的地面转向速率（以度/秒为单位）</translation>
    </message>
    <message>
      <source>Guided mode timeout after which vehicle will stop or return to level if no updates are received from caller.  Only applicable during velocity, acceleration or angle control</source>
      <translation>引导模式超时，如果没有收到呼叫者的更新，车辆将停止或返回水平。  仅适用于速度、加速度或角度控制期间</translation>
    </message>
    <message>
      <source>Gyro 2 motor noise factor</source>
      <translation>陀螺仪2电机噪声系数</translation>
    </message>
    <message>
      <source>Gyro Bias</source>
      <translation>陀螺仪偏置</translation>
    </message>
    <message>
      <source>Gyro calibration minimum temperature</source>
      <translation>陀螺仪校准最低温度</translation>
    </message>
    <message>
      <source>Gyro filter cutoff frequency</source>
      <translation>陀螺滤波器截止频率</translation>
    </message>
    <message>
      <source>Gyro rate offset temperature ^0 polynomial coefficient - Y axis</source>
      <translation>陀螺仪速率偏移温度^0多项式系数-Y轴</translation>
    </message>
    <message>
      <source>Gyro rate offset temperature ^3 polynomial coefficient - X axis</source>
      <translation>陀螺仪速率偏移温度^3多项式系数-X轴</translation>
    </message>
    <message>
      <source>Gyro2 offsets of X axis</source>
      <translation>Gyro2 X轴偏移</translation>
    </message>
    <message>
      <source>Gyro3 offsets of X axis</source>
      <translation>Gyro3 X轴偏移</translation>
    </message>
    <message>
      <source>Gyroscope 0 Y-axis offset</source>
      <translation>陀螺仪 0 Y轴偏移</translation>
    </message>
    <message>
      <source>Gyroscope 1 calibration device ID</source>
      <translation>陀螺仪1校准装置ID</translation>
    </message>
    <message>
      <source>Gyroscope 2 Z-axis offset</source>
      <translation>陀螺仪2 Z轴偏移</translation>
    </message>
    <message>
      <source>Gyroscope 3 Y-axis offset</source>
      <translation>陀螺仪 3 Y轴偏移</translation>
    </message>
    <message>
      <source>H</source>
      <translation>H</translation>
    </message>
    <message>
      <source>H3_140</source>
      <translation>H3_140</translation>
    </message>
    <message>
      <source>HEADING_EN</source>
      <translation>HEADING_EN</translation>
    </message>
    <message>
      <source>HFI EFI fuel density</source>
      <translation>HFI EFI 燃油密度</translation>
    </message>
    <message>
      <source>HITL enabled</source>
      <translation>HITL 启用</translation>
    </message>
    <message>
      <source>HOMEDIST_Y</source>
      <translation>HOMEDIST_Y</translation>
    </message>
    <message>
      <source>HORIZON_Y</source>
      <translation>HORIZON_Y</translation>
    </message>
    <message>
      <source>Halo6000 start auxilliary function</source>
      <translation>Halo6000启动辅助功能</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter attenuation</source>
      <translation>谐波陷波滤波器衰减</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter base center frequency in Hz. This is the center frequency for static notches, the center frequency for Throttle based notches at the reference thrust value, and the minimum limit of center frequency variation for all other notch types. This should always be set lower than half the backend gyro rate (which is typically 1Khz).</source>
      <translation>谐波陷波滤波器基本中心频率（以 Hz 为单位）。这是静态陷波的中心频率、参考推力值处基于节流阀的陷波的中心频率以及所有其他陷波类型的中心频率变化的最小限制。该值应始终设置为低于后端陀螺仪速率的一半（通常为 1Khz）。</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter dynamic frequency tracking mode. Dynamic updates can be throttle, RPM sensor, ESC telemetry or dynamic FFT based. Throttle-based updates should only be used with multicopters.</source>
      <translation>谐波陷波滤波器动态频率跟踪模式。动态更新可以基于油门、RPM 传感器、ESC 遥测或动态 FFT。基于油门的更新只能用于多旋翼飞行器。</translation>
    </message>
    <message>
      <source>Heading (Rover With Moving Base UART1 Connected To Autopilot, UART2 Connected To Moving Base)</source>
      <translation>航向（带有移动底座的流动站 UART1 连接到自动驾驶仪，UART2 连接到移动底座）</translation>
    </message>
    <message>
      <source>Heading/Yaw offset for dual antenna GPS</source>
      <translation>双天线 GPS 的航向/偏航偏移</translation>
    </message>
    <message>
      <source>Height (AGL) of the wings when the aircraft is on the ground</source>
      <translation>飞机在地面时机翼高度（AGL）</translation>
    </message>
    <message>
      <source>Height complementary filter frequency (radians/sec)</source>
      <translation>高度互补滤波器频率（弧度/秒）</translation>
    </message>
    <message>
      <source>Height rate filter crossover frequency</source>
      <translation>高度率滤波器交叉频率</translation>
    </message>
    <message>
      <source>HeliRSC</source>
      <translation>直升机RSC</translation>
    </message>
    <message>
      <source>Helicopter's collective pitch setting at mid-high (60%) collective stick input in Stabilize mode. Set this as a percent of collective range given by H_COL_MAX minus H_COL_MIN.</source>
      <translation>直升机的总距俯仰设置为中高 (60%)，在稳定模式下总距摇杆输入。将其设置为 H_COL_MAX 减去 H_COL_MIN 给出的总距范围的百分比。</translation>
    </message>
    <message>
      <source>Hexacopter</source>
      <translation>六轴飞行器</translation>
    </message>
    <message>
      <source>High Speed Long Range Mode</source>
      <translation>高速远距离模式</translation>
    </message>
    <message>
      <source>HighIsOn</source>
      <translation>高亮</translation>
    </message>
    <message>
      <source>HobbyWingESC</source>
      <translation>好翼ESC</translation>
    </message>
    <message>
      <source>Hold in Auto Mode</source>
      <translation>保持自动模式</translation>
    </message>
    <message>
      <source>Horizontal acceleration threshold used for heading observability check</source>
      <translation>用于航向可观测性检查的水平加速度阈值</translation>
    </message>
    <message>
      <source>Horizontal radius from return point within which special rules for return mode apply</source>
      <translation>从返回点开始的水平半径，在该范围内适用返回模式的特殊规则</translation>
    </message>
    <message>
      <source>HottTelem</source>
      <translation>霍特电信</translation>
    </message>
    <message>
      <source>Hyperbolic</source>
      <translation>双曲线</translation>
    </message>
    <message>
      <source>I term management based landed state</source>
      <translation>我用基于管理的土地国家术语</translation>
    </message>
    <message>
      <source>I2C-DLVR-60in</source>
      <translation>I2C-DLVR-60in</translation>
    </message>
    <message>
      <source>IC Engine Ignition</source>
      <translation>IC发动机点火</translation>
    </message>
    <message>
      <source>ICE Starter</source>
      <translation>ICE 启动器</translation>
    </message>
    <message>
      <source>ID of device using this MAVLink channel</source>
      <translation>使用该 MAVLink 通道的设备的 ID</translation>
    </message>
    <message>
      <source>IMU 1</source>
      <translation>IMU 1</translation>
    </message>
    <message>
      <source>IMU Offsets</source>
      <translation>IMU 偏移量</translation>
    </message>
    <message>
      <source>IMU enable mask</source>
      <translation>IMU 使能掩码</translation>
    </message>
    <message>
      <source>IMU gyro FFT minimum frequency</source>
      <translation>IMU 陀螺仪 FFT 最小频率</translation>
    </message>
    <message>
      <source>IMU integration rate</source>
      <translation>IMU积分率</translation>
    </message>
    <message>
      <source>IMU2</source>
      <translation>IMU2</translation>
    </message>
    <message>
      <source>INA220 Power Monitor Config</source>
      <translation>INA220 功率监视器配置</translation>
    </message>
    <message>
      <source>INA228 Power Monitor Shunt</source>
      <translation>INA228 功率监控分流器</translation>
    </message>
    <message>
      <source>INA3221 channel to return data for</source>
      <translation>INA3221 通道返回数据</translation>
    </message>
    <message>
      <source>IO override channel</source>
      <translation>IO 覆盖通道</translation>
    </message>
    <message>
      <source>IPv4 Port of the server to connect to</source>
      <translation>IPv4 要连接的服务器端口</translation>
    </message>
    <message>
      <source>IR-LOCK Sensor (external I2C)</source>
      <translation>IR-LOCK 传感器（外接I2C）</translation>
    </message>
    <message>
      <source>IST8310</source>
      <translation>IST8310</translation>
    </message>
    <message>
      <source>Idle Throttle Percentage during Autorotation</source>
      <translation>自转时怠速油门百分比</translation>
    </message>
    <message>
      <source>If COM_RC_OVERRIDE is enabled and the joystick input is moved more than this threshold the autopilot the pilot takes over control.</source>
      <translation>如果启用 COM_RC_OVERRIDE 并且操纵杆输入移动超过此阈值，则自动驾驶仪将接管控制权。</translation>
    </message>
    <message>
      <source>If MNT_MODE_OUT is MAVLink gimbal protocol v1, mount configure/control commands will be sent with this target ID.</source>
      <translation>如果 MNT_MODE_OUT 是 MAVLink 万向节协议 v1，则挂载配置/控制命令将与该目标 ID 一起发送。</translation>
    </message>
    <message>
      <source>If battery wattage (voltage * current) exceeds this value then the system will reduce max throttle (THR_MAX, TKOFF_THR_MAX and THR_MIN for reverse thrust) to satisfy this limit. This helps limit high current to low C rated batteries regardless of battery voltage. The max throttle will slowly grow back to THR_MAX (or TKOFF_THR_MAX ) and THR_MIN if demanding the current max and under the watt max. Use 0 to disable.</source>
      <translation>如果电池瓦数（电压 * 电流）超过此值，则系统将减小最大油门（用于反向推力的 THR_MAX、TKOFF_THR_MAX 和 THR_MIN）以满足此限制。无论电池电压如何，这都有助于限制低 C 额定值电池的高电流。如果要求当前最大值且低于最大功率，则最大油门将慢慢恢复到 THR_MAX（或 TKOFF_THR_MAX）和 THR_MIN。使用0禁用。</translation>
    </message>
    <message>
      <source>If enabled, failure detector will verify that for motors, a minimum amount of ESC current per throttle
level is being consumed.
Otherwise this indicates an motor failure.</source>
      <translation>如果启用，故障检测器将验证电机是否消耗了每个油门级别的最小 ESC 电流。否则，这表明电机出现故障。</translation>
    </message>
    <message>
      <source>If enabled, update the bootloader on the next boot. WARNING: do not cut the power during an update process, otherwise you will have to recover using some alternative method (e.g. JTAG). Instructions: - Insert an SD card - Enable this parameter - Reboot the board (plug the power or send a reboot command) - Wait until the board comes back up (or at least 2 minutes) - If it does not come back, check the file bootlog.txt on the SD card</source>
      <translation>如果启用，则在下次启动时更新引导加载程序。 WARNING：在更新过程中不要切断电源，否则您将不得不使用某种替代方法（例如JTAG）来恢复。说明： - 插入 SD 卡 - 启用此参数 - 重新启动开发板（插入电源或发送重启命令） - 等待开发板恢复正常（或至少 2 分钟） - 如果没有恢复，请检查 SD 卡上的 bootlog.txt 文件</translation>
    </message>
    <message>
      <source>If ethernet enabled and selected as configuration for MAVLink instance 1,
selected udp port will be set and used in MAVLink instance 1.</source>
      <translation>如果启用以太网并选择作为 MAVLink 实例 1 的配置，则选定的 udp 端口​​将在 MAVLink 实例 1 中设置和使用。</translation>
    </message>
    <message>
      <source>If ethernet is enabled and is the selected configuration for uXRCE-DDS,
the selected UDP port will be set and used.</source>
      <translation>如果以太网已启用并且是 uXRCE-DDS 的选定配置，则将设置并使用选定的 UDP 端口。</translation>
    </message>
    <message>
      <source>If non-negative, then this will be used instead of the online estimated internal resistance.</source>
      <translation>如果非负，则将使用该值代替在线估计的内阻。</translation>
    </message>
    <message>
      <source>If not set to None, this motor is tilted by the configured tilt servo.</source>
      <translation>如果未设置为“无”，则该电机将由配置的倾斜舵机器倾斜。</translation>
    </message>
    <message>
      <source>If set to 1 the control surfaces are locked at the disarmed value in multicopter mode.</source>
      <translation>如果设置为 1，控制面将在多旋翼模式下锁定在解除值。</translation>
    </message>
    <message>
      <source>If set to a non-zero value then this is an RC input channel number to use for giving IO manual control in case the main FMU microcontroller on a board with a IO co-processor fails. When this RC input channel goes above 1750 the FMU microcontroller will no longer be involved in controlling the servos and instead the IO microcontroller will directly control the servos. Note that IO manual control will be automatically activated if the FMU crashes for any reason. This parameter allows you to test for correct manual behaviour without actually crashing the FMU. This parameter is can be set to a non-zero value either for ground testing purposes or for giving the effect of an external override control board. Please also see the docs on OVERRIDE_SAFETY. Note that you may set OVERRIDE_CHAN to the same channel as FLTMODE_CH to get IO based override when in flight mode 6. Note that when override is triggered due to a FMU crash the 6 auxiliary output channels on Pixhawk will no longer be updated, so all the flight controls you need must be assigned to the first 8 channels.</source>
      <translation>如果设置为非零值，则这是一个 RC 输入通道号，用于在带有 IO 协处理器的电路板上的主 FMU 微控制器发生故障时，用于对 IO 进行手动控制。当 RC 输入通道高于 1750 时，FMU 微控制器将不再参与控制舵机，而是由 IO 微控制器直接控制舵机。请注意，如果 FMU 因任何原因崩溃，IO 手动控制将自动激活。此参数允许您测试正确的手动行为，而不会实际导致 FMU 崩溃。该参数可以设置为非零值，以用于地面测试目的或提供外部超越控制板的效果。另请参阅 OVERRIDE_SAFETY 上的文档。请注意，您可以将 OVERRIDE_CHAN 设置为与 FLTMODE_CH 相同的通道，以便在飞行模式 6 下获得基于 IO 的覆盖。请注意，当由于 FMU 崩溃而触发覆盖时，Pixhawk 上的 6 辅助输出通道将不再更新，因此您需要的所有飞行控制必须分配给第一个 8 通道。</translation>
    </message>
    <message>
      <source>If set to true, the autopilot is allowed to set its home position after takeoff
The true home position is back-computed if a local position is estimate if available.
If no local position is available, home is set to the current position.</source>
      <translation>如果设置为 true，则允许自动驾驶仪在起飞后设置其起始位置。如果估计本地位置（如果可用），则会反向计算真实起始位置。如果没有可用的本地位置，则将 home 设置为当前位置。</translation>
    </message>
    <message>
      <source>If stall prevention is enabled this speed is used to calculate the minimum airspeed while banking. It is also used during landing final as the minimum airspeed that can be demanded by the TECS, which allows using TECS_LAND_ARSPD or LAND_PF_ARSPD to achieve landings slower than AIRSPEED_MIN. If this is set to 0 then the stall speed is assumed to be the minimum airspeed speed. Typically set slightly higher then true stall speed.</source>
      <translation>如果启用失速预防，则该速度用于计算倾斜时的最小空速。它还在最终着陆期间用作 TECS 可以要求的最小空速，这允许使用 TECS_LAND_ARSPD 或 LAND_PF_ARSPD 实现比 AIRSPEED_MIN 慢的着陆。如果设置为 0，则假定失速速度为最小空速。通常设置为略高于真实失速速度。</translation>
    </message>
    <message>
      <source>If the difference between airspeed and ground speed is greater than this value the sensor will issue a warning. If 0 ARSPD_WIND_MAX is used.</source>
      <translation>如果空速和地速之间的差异大于该值，传感器将发出警告。如果使用 0 ARSPD_WIND_MAX。</translation>
    </message>
    <message>
      <source>If the main switch channel is in this range the
selected flight mode will be applied.</source>
      <translation>如果主开关通道在此范围内，则将应用所选的飞行模式。</translation>
    </message>
    <message>
      <source>If the vehicle is on ground, is not moving as determined by the motion test and the range finder is returning invalid or no data, then an assumed range value of EKF2_MIN_RNG will be used by the terrain estimator so that a terrain height estimate is available at the start of flight in situations where the range finder may be inside its minimum measurements distance when on ground.</source>
      <translation>如果车辆在地面上，根据运动测试确定没有移动，并且测距仪返回无效数据或没有数据，则地形估计器将使用 EKF2_MIN_RNG 的假设范围值，以便在测距仪在地面时可能位于其最小测量距离范围内的情况下，在飞行开始时可以获得地形高度估计。</translation>
    </message>
    <message>
      <source>If this is set to 1 then an RTL will change to QRTL when within RTL_RADIUS meters of the RTL destination, VTOL approach: vehicle will RTL at RTL alt and circle with a radius of Q_FW_LND_APR_RAD down to Q_RTL_ALT and then transition into the wind and QRTL, see 'AUTO VTOL Landing', QRTL Always: do a QRTL instead of RTL</source>
      <translation>设为 1 时，无人机进入距返航目标 RTL_RADIUS 米以内的区域后，将从 RTL 切换为 QRTL。垂直起降进近：先以 RTL 高度返航，再以 Q_FW_LND_APR_RAD 为半径盘旋下降至 Q_RTL_ALT，随后逆风转换并执行 QRTL，详见“AUTO VTOL Landing”。始终使用 QRTL：使用 QRTL 代替 RTL。</translation>
    </message>
    <message>
      <source>If this parameter is set to 1, then an RC loss will only cause the plane to terminate in manual control modes. If it is 0, then the plane will terminate in any flight mode.</source>
      <translation>如果该参数设置为1，则RC丢失只会导致飞机在手动控制模式下终止。如果是0，则飞机将终止于任何飞行模式。</translation>
    </message>
    <message>
      <source>Ignore</source>
      <translation>忽略</translation>
    </message>
    <message>
      <source>Ignore Streamrate (moved to MAVn_OPTIONS &gt;4.7)</source>
      <translation>忽略码率（移至 MAVn_OPTIONS &gt;4.7）</translation>
    </message>
    <message>
      <source>IgnoreDNANodeConflicts</source>
      <translation>忽略DNA节点冲突</translation>
    </message>
    <message>
      <source>In auto modes: default climb rate output by controller to achieve altitude setpoints.
In manual modes: maximum climb rate setpoint.</source>
      <translation>在自动模式下：控制器输出默认爬升率以达到高度设定值。在手动模式下：最大爬升率设定值。</translation>
    </message>
    <message>
      <source>Inclusion/Exclusion Circles+Polygons</source>
      <translation>包含/排除圆+多边形</translation>
    </message>
    <message>
      <source>Increase to make the estimator more responsive Decrease to make the estimator more robust to noise</source>
      <translation>增加以使估计器更具响应性 减少以使估计器对噪声更加鲁棒</translation>
    </message>
    <message>
      <source>Increment step for actuator 1</source>
      <translation>执行器 1 的增量步长</translation>
    </message>
    <message>
      <source>Index of the parameter to be displayed and modified</source>
      <translation>要显示和修改的参数索引</translation>
    </message>
    <message>
      <source>Informs ADSB vehicles of this vehicle's max speed capability</source>
      <translation>告知 ADSB 车辆该车辆的最大速度能力</translation>
    </message>
    <message>
      <source>Initial driving mode</source>
      <translation>初始驾驶模式</translation>
    </message>
    <message>
      <source>Initial variance of the relative landing target position in x and y direction</source>
      <translation>x、y方向相对着陆目标位置的初始方差</translation>
    </message>
    <message>
      <source>Input Pin B</source>
      <translation>输入引脚 B</translation>
    </message>
    <message>
      <source>Input signal type</source>
      <translation>输入信号类型</translation>
    </message>
    <message>
      <source>Integral gain for horizontal velocity error</source>
      <translation>水平速度误差的积分增益</translation>
    </message>
    <message>
      <source>Integrator Gain</source>
      <translation>积分器增益</translation>
    </message>
    <message>
      <source>IntelT265</source>
      <translation>英特尔T265</translation>
    </message>
    <message>
      <source>Internal Temperature Failsafe Enable</source>
      <translation>内部温度故障保护启用</translation>
    </message>
    <message>
      <source>InvertInput</source>
      <translation>反转输入</translation>
    </message>
    <message>
      <source>InvertedWindPointer</source>
      <translation>倒置风指针</translation>
    </message>
    <message>
      <source>JSON master instance</source>
      <translation>JSON主实例</translation>
    </message>
    <message>
      <source>Jerk limit of the horizontal kinematic path generation used to determine how quickly the estimate varies in acceleration</source>
      <translation>水平运动轨迹生成的加加速度限制，用于确定估计加速度的变化速度</translation>
    </message>
    <message>
      <source>Kalman Filter Accelerometer Noise, higher values weight the input from the camera more, accels less</source>
      <translation>卡尔曼滤波器加速度计噪声，值越高，相机输入的权重越大，加速度越小</translation>
    </message>
    <message>
      <source>KillIMU3</source>
      <translation>杀死IMU3</translation>
    </message>
    <message>
      <source>L25W28P5</source>
      <translation>L25W28P5</translation>
    </message>
    <message>
      <source>L65W59P5</source>
      <translation>L65W59P5</translation>
    </message>
    <message>
      <source>LED Brightness</source>
      <translation>LED 亮度</translation>
    </message>
    <message>
      <source>LOITER</source>
      <translation>位置保持</translation>
    </message>
    <message>
      <source>LTC2946</source>
      <translation>LTC2946</translation>
    </message>
    <message>
      <source>LW20/c</source>
      <translation>LW20/c</translation>
    </message>
    <message>
      <source>Land detection altitude change threshold</source>
      <translation>陆地检测高度变化阈值</translation>
    </message>
    <message>
      <source>Land offset right</source>
      <translation>土地向右偏移</translation>
    </message>
    <message>
      <source>Landing</source>
      <translation>正在降落</translation>
    </message>
    <message>
      <source>Landing abort min altitude</source>
      <translation>着陆中止最低高度</translation>
    </message>
    <message>
      <source>Landing flare altitude</source>
      <translation>着陆耀斑高度</translation>
    </message>
    <message>
      <source>Landing options bitmask</source>
      <translation>登陆选项位掩码</translation>
    </message>
    <message>
      <source>Landing target x measurements are scaled by this factor before being used</source>
      <translation>着陆目标 x 测量值在使用前按此系数缩放</translation>
    </message>
    <message>
      <source>LastRaw</source>
      <translation>最后的原始数据</translation>
    </message>
    <message>
      <source>Lateral channel</source>
      <translation>横向通道</translation>
    </message>
    <message>
      <source>Learn and Save</source>
      <translation>学习并保存</translation>
    </message>
    <message>
      <source>Leave Unchanged</source>
      <translation>保持不变</translation>
    </message>
    <message>
      <source>Left Flap</source>
      <translation>左襟翼</translation>
    </message>
    <message>
      <source>Legal max altitude for UAV/RPAS/drones in your jurisdiction</source>
      <translation>您所在管辖范围内 UAV/RPAS/无人机的法定最高高度</translation>
    </message>
    <message>
      <source>Len55_Wid45</source>
      <translation>Len55_Wid45</translation>
    </message>
    <message>
      <source>Length of aerobatic "box"</source>
      <translation>特技飞行“盒子”的长度</translation>
    </message>
    <message>
      <source>Level of aggressiveness of pitch and roll PID gains. Lower values result in a 'softer' tune. Level 6 recommended for most planes.</source>
      <translation>PID 的俯仰和横滚攻击性增益水平。较低的值会导致“更柔和”的曲调。建议大多数飞机使用 6 级。</translation>
    </message>
    <message>
      <source>Lever arm offset (m) in the X direction for antenna 2.
This will only be used if the device supports a dual antenna setup.</source>
      <translation>天线 2 的杠杆臂在 X 方向的偏移量 (m)。仅当设备支持双天线设置时才会使用此功能。</translation>
    </message>
    <message>
      <source>Lever arm offset (m) in the Z direction for the external GNSS receiver.
In the case of a dual antenna setup, this is antenna 1.</source>
      <translation>外部 GNSS 接收器 Z 方向的杠杆臂偏移 (m)。如果是双天线设置，则为 1 天线。</translation>
    </message>
    <message>
      <source>LidarLite-I2C</source>
      <translation>激光雷达Lite-I2C</translation>
    </message>
    <message>
      <source>LightWare-GRF</source>
      <translation>LightWare-GRF</translation>
    </message>
    <message>
      <source>Lights1</source>
      <translation>灯1</translation>
    </message>
    <message>
      <source>Limit allowed thrust e.g. for indoor test of overpowered vehicle.</source>
      <translation>限制允许推力，例如用于超载车辆的室内测试。</translation>
    </message>
    <message>
      <source>Limit of roll integrator gain in centi-degrees of servo travel. Servos are assumed to have +/- 4500 centi-degrees of travel, so a value of 3000 allows trim of up to 2/3 of servo travel range.</source>
      <translation>横滚积分器增益限制（以厘米为单位）舵机行程。假设舵机系统具有 +/- 4500 厘度的行程，因此 3000 的值允许调整最多 2/3 的舵机行程范围。</translation>
    </message>
    <message>
      <source>Limits the acceleration of the yaw setpoint to avoid large
control output and mixer saturation.</source>
      <translation>限制偏航设定点的加速度以避免大的控制输出和混合器饱和。</translation>
    </message>
    <message>
      <source>Linearize Swash 1 Servos</source>
      <translation>线性化斜盘 1 舵机系统</translation>
    </message>
    <message>
      <source>Load test type</source>
      <translation>负载测试类型</translation>
    </message>
    <message>
      <source>Locks control surfaces during pre-launch (armed) and until this time since launch has passed.
Only affects control surfaces that have corresponding flag set, and not active for runway takeoff.
Set to 0 to disable any surface locking after arming.</source>
      <translation>在启动前（解锁）期间锁定控制界面，直到启动结束后的这个时间。仅影响设置了相应标志的控制面，并且对于跑道起飞不活动。设置为 0 以在解锁后禁用任何表面锁定。</translation>
    </message>
    <message>
      <source>Log RC input bytes</source>
      <translation>记录 RC 输入字节</translation>
    </message>
    <message>
      <source>Log number for GPS:update_file()</source>
      <translation>GPS 的日志号：update_file()</translation>
    </message>
    <message>
      <source>LogAllFrames</source>
      <translation>记录所有帧</translation>
    </message>
    <message>
      <source>Logging Available</source>
      <translation>可用的日志记录</translation>
    </message>
    <message>
      <source>Logging rate for EAHRS devices</source>
      <translation>EAHRS 设备的记录速率</translation>
    </message>
    <message>
      <source>Loiter Angle Max</source>
      <translation>最大悬停角</translation>
    </message>
    <message>
      <source>Loiter braking acceleration in cm/s/s. Higher values stop the copter more quickly when the stick is centered.</source>
      <translation>盘旋制动加速度（以 cm/s/s 为单位）。当摇杆居中时，值越高，飞行器停止得越快。</translation>
    </message>
    <message>
      <source>Loiter maximum correction acceleration</source>
      <translation>盘旋最大修正加速度</translation>
    </message>
    <message>
      <source>Loiter maximum pilot requested lean angle. Set to zero for 2/3 of Q_P_ANGLE_MAX/Q_ANGLE_MAX. The maximum vehicle lean angle is still limited by Q_P_ANGLE_MAX/Q_ANGLE_MAX</source>
      <translation>盘旋飞行员要求的最大倾斜角度。 Q_P_ANGLE_MAX/Q_ANGLE_MAX 的 2/3 设置为零。车辆最大倾斜角仍受Q_P_ANGLE_MAX/Q_ANGLE_MAX限制</translation>
    </message>
    <message>
      <source>Loiter radius</source>
      <translation>盘旋半径</translation>
    </message>
    <message>
      <source>Long Range Mode</source>
      <translation>远距离模式</translation>
    </message>
    <message>
      <source>Lost Plane Sound</source>
      <translation>飞机失落的声音</translation>
    </message>
    <message>
      <source>Low battery voltage</source>
      <translation>电池电压低</translation>
    </message>
    <message>
      <source>Low voltage timeout</source>
      <translation>低电压超时</translation>
    </message>
    <message>
      <source>LowIsOn</source>
      <translation>低位开启</translation>
    </message>
    <message>
      <source>Lowest possible servo position in PWM microseconds for swashplate 2</source>
      <translation>对于斜盘 2，可能的最低舵机位置（以 PWM 微秒为单位）</translation>
    </message>
    <message>
      <source>MAC Address 3rd byte</source>
      <translation>MAC 地址第 3 个字节</translation>
    </message>
    <message>
      <source>MAC address 5th byte</source>
      <translation>MAC 地址第 5 个字节</translation>
    </message>
    <message>
      <source>MAG2 Failure</source>
      <translation>MAG2故障</translation>
    </message>
    <message>
      <source>MAG3 external compass orientation</source>
      <translation>MAG3 外置罗盘定向</translation>
    </message>
    <message>
      <source>MAIN 1 Disarmed Value</source>
      <translation>MAIN 1 上锁值</translation>
    </message>
    <message>
      <source>MAIN 2 Maximum Value</source>
      <translation>MAIN 2 最大值</translation>
    </message>
    <message>
      <source>MAIN 3 Output Function</source>
      <translation>MAIN 3 输出功能</translation>
    </message>
    <message>
      <source>MAIN 5 Disarmed Value</source>
      <translation>MAIN 5 上锁值</translation>
    </message>
    <message>
      <source>MAIN 6 Maximum Value</source>
      <translation>MAIN 6 最大值</translation>
    </message>
    <message>
      <source>MAIN 7 Output Function</source>
      <translation>MAIN 7 输出功能</translation>
    </message>
    <message>
      <source>MANUAL Mode</source>
      <translation>MANUAL模式</translation>
    </message>
    <message>
      <source>MAVLink (Gremsy/AVT)</source>
      <translation>MAVLink（Gremsy/AVT）</translation>
    </message>
    <message>
      <source>MAVLink Network Port for instance 0</source>
      <translation>MAVLink网络端口例如0</translation>
    </message>
    <message>
      <source>MAVLink SiK Radio ID</source>
      <translation>MAVLink SiK 无线电 ID</translation>
    </message>
    <message>
      <source>MAVLink Stream rate of ATTITUDE, SIMSTATE (SIM only), AHRS2, RPM, AOA_SSA, LANDING,ESC_TELEMETRY,EFI_STATUS, and PID_TUNING</source>
      <translation>ATTITUDE、SIMSTATE（仅限 SIM）、AHRS2、RPM、AOA_SSA、LANDING、ESC_TELEMETRY、EFI_STATUS 和 PID_TUNING 的 MAVLink 流速率</translation>
    </message>
    <message>
      <source>MAVLink airframe type</source>
      <translation>MAVLink机身类型</translation>
    </message>
    <message>
      <source>MAVLink only</source>
      <translation>仅限 MAVLink</translation>
    </message>
    <message>
      <source>MAVLink/Scripting/AP_Periph</source>
      <translation>MAVLink/脚本/AP_Periph</translation>
    </message>
    <message>
      <source>MAVlink1</source>
      <translation>MAVlink1</translation>
    </message>
    <message>
      <source>MESSAGE_X</source>
      <translation>MESSAGE_X</translation>
    </message>
    <message>
      <source>MOTOR_FRAME_TYPE_NYT_PLUS</source>
      <translation>MOTOR_FRAME_TYPE_NYT_PLUS</translation>
    </message>
    <message>
      <source>MPPT Powered off when disarmed</source>
      <translation>MPPT 上锁时断电</translation>
    </message>
    <message>
      <source>MS5837</source>
      <translation>MS5837</translation>
    </message>
    <message>
      <source>Mag measurement delay</source>
      <translation>磁力测量延迟</translation>
    </message>
    <message>
      <source>Magnetic field strength test selection</source>
      <translation>磁场有效性检查选项</translation>
    </message>
    <message>
      <source>Magnetometer 0 X-axis offset</source>
      <translation>磁力计0 X轴偏移</translation>
    </message>
    <message>
      <source>Magnetometer 0 Z-axis offset</source>
      <translation>磁力计0 Z轴偏移</translation>
    </message>
    <message>
      <source>Magnetometer 1 X Axis throttle compensation</source>
      <translation>磁力计 1 X轴油门补偿</translation>
    </message>
    <message>
      <source>Magnetometer 1 Z Axis throttle compensation</source>
      <translation>磁力计 1 Z轴油门补偿</translation>
    </message>
    <message>
      <source>Magnetometer 2 Custom Euler Roll Angle</source>
      <translation>磁力计 2 定制欧拉横滚角</translation>
    </message>
    <message>
      <source>Magnetometer 2 Y-axis offset</source>
      <translation>磁力计 2 Y 轴偏移</translation>
    </message>
    <message>
      <source>Magnetometer 2 rotation relative to airframe</source>
      <translation>磁力计 2 相对于机身的旋转</translation>
    </message>
    <message>
      <source>Magnetometer 3 Y Axis throttle compensation</source>
      <translation>磁力计 3 Y轴油门补偿</translation>
    </message>
    <message>
      <source>Magnetometer 3 calibration device ID</source>
      <translation>磁力计3校准装置ID</translation>
    </message>
    <message>
      <source>Magnetometer data maximum publication rate. This is an upper bound,
actual magnetometer data rate is still dependent on the sensor.</source>
      <translation>磁力计数据最大发布率。这是上限，实际磁力计数据速率仍然取决于传感器。</translation>
    </message>
    <message>
      <source>Magnetometer measurement gate size</source>
      <translation>磁力计测量门尺寸</translation>
    </message>
    <message>
      <source>Magnetometer offset temperature ^0 polynomial coefficient - Z axis</source>
      <translation>磁力计偏移温度^0多项式系数-Z轴</translation>
    </message>
    <message>
      <source>Magnetometer offset temperature ^3 polynomial coefficient - Y axis</source>
      <translation>磁力计偏移温度^3多项式系数-Y轴</translation>
    </message>
    <message>
      <source>Magnitude of maximum angular flow rate reliably measurable by the optical flow sensor</source>
      <translation>光流量传感器可可靠测量最大角流量的大小</translation>
    </message>
    <message>
      <source>Main rotor rpm setting that governor maintains when engaged.  Set to the rotor rpm your helicopter runs in flight. When a speed sensor is installed the rotor governor maintains this speed. For governor operation this should be set 10 rpm higher than the actual desired headspeed to allow for governor droop</source>
      <translation>调速器在接合时维持的主旋翼转速设置。  设置直升机在飞行中运行的旋翼转速。当安装速度传感器时，转子调速器保持该速度。对于调速器操作，应将 10 rpm 设置为高于实际所需的转速，以允许调速器下垂</translation>
    </message>
    <message>
      <source>MainOut6</source>
      <translation>主输出6</translation>
    </message>
    <message>
      <source>Maintain altitude or track target's altitude. When maintaining the altitude, the drone can crash into terrain when the target moves uphill. When tracking the target's altitude, the follow altitude FLW_TGT_HT should be high enough to prevent terrain collisions due to GPS inaccuracies of the target.</source>
      <translation>保持高度或跟踪目标高度。当保持高度时，当目标上坡时，无人机可能会坠入地形。跟踪目标高度时，跟踪高度 FLW_TGT_HT 应足够高，以防止由于 GPS 目标不准确而导致地形碰撞。</translation>
    </message>
    <message>
      <source>Manual R/C pass-through mask</source>
      <translation>手动遥控透罩</translation>
    </message>
    <message>
      <source>Manual input expo for pitch</source>
      <translation>手动输入展位</translation>
    </message>
    <message>
      <source>Manual mode specific options</source>
      <translation>手动模式特定选项</translation>
    </message>
    <message>
      <source>Manual yaw scale</source>
      <translation>手动偏航刻度</translation>
    </message>
    <message>
      <source>MappyDot Sensor 1 Rotation</source>
      <translation>MappyDot 传感器 1 旋转</translation>
    </message>
    <message>
      <source>MappyDot Sensor 7 Rotation</source>
      <translation>MappyDot 传感器 7 旋转</translation>
    </message>
    <message>
      <source>Marvelmind</source>
      <translation>奇迹心灵</translation>
    </message>
    <message>
      <source>Mask Channel 16 as a mode button</source>
      <translation>屏蔽通道 16 作为模式按钮</translation>
    </message>
    <message>
      <source>Mask Channel 7 as a mode button</source>
      <translation>屏蔽通道 7 作为模式按钮</translation>
    </message>
    <message>
      <source>Mask of channels which are dynamically reversible. This is used to configure ESCs in '3D' mode, allowing for the motor to spin in either direction. Do not use for channels selected with SERVO_BLH_RVMASK.</source>
      <translation>动态可逆的通道掩码。这用于将 ESC 配置为“3D”模式，允许电机沿任一方向旋转。请勿用于通过 SERVO_BLH_RVMASK 选择的通道。</translation>
    </message>
    <message>
      <source>Mask of channels which support bi-directional dshot. This is used for ESCs which have firmware that supports bi-directional dshot allowing fast rpm telemetry values to be returned for the harmonic notch.</source>
      <translation>支持双向 dshot 的通道掩码。这用于具有支持双向 dshot 的固件的 ESC，允许为谐波陷波返回快速转速遥测值。</translation>
    </message>
    <message>
      <source>Mavlink Serial Channel2</source>
      <translation>Mavlink串行通道2</translation>
    </message>
    <message>
      <source>Max</source>
      <translation>最大</translation>
    </message>
    <message>
      <source>Max altitude</source>
      <translation>最大海拔高度</translation>
    </message>
    <message>
      <source>Max manual yaw rate for Stabilized, Altitude, Position mode</source>
      <translation>稳定、高度、位置模式的最大手动偏航角速度</translation>
    </message>
    <message>
      <source>Max transmit rate</source>
      <translation>最大传输速率</translation>
    </message>
    <message>
      <source>MaxBotix MB12XX Sensor 12 Rotation</source>
      <translation>MaxBotix MB12XX 传感器 12 旋转</translation>
    </message>
    <message>
      <source>MaxBotix MB12XX Sensor 9 Rotation</source>
      <translation>MaxBotix MB12XX 传感器 9 旋转</translation>
    </message>
    <message>
      <source>Maximal tilt angle in Stabilized, Altitude and Altitude Cruise mode</source>
      <translation>稳定、定高和定高巡航模式下的最大倾斜角度</translation>
    </message>
    <message>
      <source>Maximum Bank Angle</source>
      <translation>最大倾斜角</translation>
    </message>
    <message>
      <source>Maximum Groundspeed</source>
      <translation>最大行驶速度</translation>
    </message>
    <message>
      <source>Maximum PWM output</source>
      <translation>PWM 最大输出</translation>
    </message>
    <message>
      <source>Maximum Roll Rate</source>
      <translation>最大横滚率</translation>
    </message>
    <message>
      <source>Maximum acceleration with which obstacles will be avoided with. Set zero to disable acceleration limits</source>
      <translation>避开障碍物的最大加速度。设置为零以禁用加速度限制</translation>
    </message>
    <message>
      <source>Maximum airspeed used for baro static pressure compensation</source>
      <translation>用于气压静压补偿的最大空速</translation>
    </message>
    <message>
      <source>Maximum allowed flight time</source>
      <translation>允许的最长飞行时间</translation>
    </message>
    <message>
      <source>Maximum amount of memory to allocate to AP_Logger-over-mavlink</source>
      <translation>分配给 AP_Logger-over-mavlink 的最大内存量</translation>
    </message>
    <message>
      <source>Maximum body frame acceleration allowed in the in speed controller. This limit defines a circular constraint in accel. Minimum used is 0.5 m/s/s.</source>
      <translation>速度控制器中允许的最大机体框架加速度。该限制定义了 Accel 中的循环约束。最小使用量为 0.5 m/s/s。</translation>
    </message>
    <message>
      <source>Maximum control throttle scaling value</source>
      <translation>最大控制油门比例值</translation>
    </message>
    <message>
      <source>Maximum demanded climb rate. Do not set higher than the climb speed at THR_MAX at TRIM_ARSPD_CM when the battery is at low voltage. Reduce value if airspeed cannot be maintained on ascent. Increase value if throttle does not increase significantly to ascend.</source>
      <translation>最大指令爬升率。此值不得高于电池电压较低时，以 THR_MAX 油门和 TRIM_ARSPD_CM 空速能够达到的爬升速度。若爬升时无法保持空速，应减小此值；若爬升时油门没有明显增加，可增大此值。</translation>
    </message>
    <message>
      <source>Maximum distance in metres that rangefinder can reliably read</source>
      <translation>测距仪可以可靠读取的最大距离（以米为单位）</translation>
    </message>
    <message>
      <source>Maximum forward acceleration to apply in speed controller.</source>
      <translation>适用于速度控制器的最大向前加速度。</translation>
    </message>
    <message>
      <source>Maximum horizontal and vertical jerk in Position/Altitude mode</source>
      <translation>位置/高度模式下的最大水平和垂直加加速度</translation>
    </message>
    <message>
      <source>Maximum horizontal velocity allowed in the landed state.
A factor of 0.7 is applied in case of airspeed-less flying
(either because no sensor is present or sensor data got invalid in flight).</source>
      <translation>着陆状态下允许的最大水平速度。 0.7 系数适用于无空速飞行的情况（因为没有传感器或传感器数据在飞行中无效）。</translation>
    </message>
    <message>
      <source>Maximum landing slope angle</source>
      <translation>最大着陆坡度角</translation>
    </message>
    <message>
      <source>Maximum length of the tether line in meters</source>
      <translation>系绳的最大长度（以米为单位）</translation>
    </message>
    <message>
      <source>Maximum manual roll angle</source>
      <translation>最大手动横滚角</translation>
    </message>
    <message>
      <source>Maximum number of comms loss events</source>
      <translation>最大通讯丢失事件数</translation>
    </message>
    <message>
      <source>Maximum number of search attempts</source>
      <translation>最大搜索尝试次数</translation>
    </message>
    <message>
      <source>Maximum physical roll angular position of the mount</source>
      <translation>安装座的最大物理横滚角位置</translation>
    </message>
    <message>
      <source>Maximum pitch during transition to auto fixed wing flight</source>
      <translation>过渡到自动固定翼飞行期间的最大俯仰角</translation>
    </message>
    <message>
      <source>Maximum rate gyro inconsistency between IMU units that will allow arming</source>
      <translation>IMU 单元之间允许解锁的最大速率陀螺仪不一致</translation>
    </message>
    <message>
      <source>Maximum roll angle before FailureDetector triggers the attitude_failure flag.
The flag triggers flight termination (if @CBRK_FLIGHTTERM = 0),
which sets outputs to their failsafe values.
On takeoff the flag triggers lockdown (irrespective of @CBRK_FLIGHTTERM),
which disarms motors but does not set outputs to failsafe values.
Setting this parameter to 0 disables the check</source>
      <translation>FailureDetector 触发 Attitude_failure 标志之前的最大横滚角度。该标志触发飞行终止（如果 @CBRK_FLIGHTTERM = 0），这会将输出设置为其故障安全值。起飞时，该标志会触发锁定（无论@CBRK_FLIGHTTERM如何），这会解除电机但不会将输出设置为故障安全值。将此参数设置为 0 将禁用检查</translation>
    </message>
    <message>
      <source>Maximum soaring altitude, relative to the home location</source>
      <translation>相对于家乡位置的最大翱翔高度</translation>
    </message>
    <message>
      <source>Maximum tangential velocity setting for generating the follow orbit trajectory</source>
      <translation>用于生成跟随轨道轨迹的最大切向速度设置</translation>
    </message>
    <message>
      <source>Maximum time (in seconds) before resetting setpoint</source>
      <translation>重置设定点之前的最长时间（以秒为单位）</translation>
    </message>
    <message>
      <source>Maximum upwards acceleration in climb rate controlled modes</source>
      <translation>爬升率控制模式下的最大向上加速度</translation>
    </message>
    <message>
      <source>Maximum value that the parameter currently being tuned with the transmitter's TRANSMITTER2_TUNING knob will be set to</source>
      <translation>当前使用发射机 TRANSMITTER2_TUNING 旋钮调谐的参数的最大值</translation>
    </message>
    <message>
      <source>Maximum vertical drift speed to use GPS</source>
      <translation>使用GPS的最大垂直漂移速度</translation>
    </message>
    <message>
      <source>Maximum yaw acceleration in autonomous modes</source>
      <translation>自主模式下的最大偏航加速度</translation>
    </message>
    <message>
      <source>Measured Longitudinal Position</source>
      <translation>测量的纵向位置</translation>
    </message>
    <message>
      <source>Measurement noise for auxiliary global position sensor 0</source>
      <translation>辅助全球位置传感器 0 的测量噪声</translation>
    </message>
    <message>
      <source>Measurement noise for range finder fusion</source>
      <translation>测距仪融合的测量噪声</translation>
    </message>
    <message>
      <source>Medium</source>
      <translation>中</translation>
    </message>
    <message>
      <source>Methods:
- one arm: request authorization and arm when authorization is received
- two step arm: 1st arm command request an authorization and
2nd arm command arm the drone if authorized
Used if arm authorization is requested by COM_ARM_AUTH_REQ.</source>
      <translation>解锁授权方式：
单步解锁：请求授权，收到授权后立即解锁。
两步解锁：第 1 次解锁指令请求授权，获得授权后，第 2 次解锁指令才会解锁无人机。
当 COM_ARM_AUTH_REQ 要求解锁授权时使用。</translation>
    </message>
    <message>
      <source>MicroStrain GNSS aiding source control</source>
      <translation>MicroStrain GNSS 帮助源控制</translation>
    </message>
    <message>
      <source>MicroStrain IMU data rate</source>
      <translation>MicroStrain IMU 数据速率</translation>
    </message>
    <message>
      <source>MicroStrain gyroscope range</source>
      <translation>MicroStrain 陀螺仪系列</translation>
    </message>
    <message>
      <source>MicroStrain7</source>
      <translation>微应变7</translation>
    </message>
    <message>
      <source>Min input value for RSSI reading</source>
      <translation>RSSI 读数的最小输入值</translation>
    </message>
    <message>
      <source>Minimum Airspeed</source>
      <translation>最小空速</translation>
    </message>
    <message>
      <source>Minimum GPS z standard deviation, uses reported EPV if greater</source>
      <translation>最小 GPS z 标准差，如果更大则使用报告的 EPV</translation>
    </message>
    <message>
      <source>Minimum Pitch Angle</source>
      <translation>最小俯仰角</translation>
    </message>
    <message>
      <source>Minimum Sink Rate (metres/sec)</source>
      <translation>最小下沉速率（米/秒）</translation>
    </message>
    <message>
      <source>Minimum altitude above terrain before turning upon entry</source>
      <translation>进入时转弯前的最低海拔高度</translation>
    </message>
    <message>
      <source>Minimum altitude for ADS-B avoidance. If the vehicle is below this altitude, no avoidance action will take place. Useful to prevent ADS-B avoidance from activating while below the tree line or around structures. Default of 0 is no minimum.</source>
      <translation>ADS-B 回避的最低高度。如果车辆低于此高度，则不会发生避让动作。有助于防止 ADS-B 回避系统在树线下方或建筑物周围激活。 0 的默认值没有最小值。</translation>
    </message>
    <message>
      <source>Minimum arming voltage on the first battery</source>
      <translation>第一个电池的最小解锁电压</translation>
    </message>
    <message>
      <source>Minimum continuous period without GPS failure required to mark a healthy GPS status. It can be reduced to speed up initialization, but it's recommended to keep this unchanged for a vehicle.</source>
      <translation>标记健康 GPS 状态所需的 GPS 不发生故障的最短连续时间。可以减少它以加快初始化速度，但建议车辆保持不变。</translation>
    </message>
    <message>
      <source>Minimum distance the vehicle should keep to all obstacles</source>
      <translation>车辆与所有障碍物应保持的最小距离</translation>
    </message>
    <message>
      <source>Minimum forward acceleration in m/s/s before arming the ground speed check in auto-takeoff. This is meant to be used for hand launches. Setting this value to 0 disables the acceleration test which means the ground speed check will always be armed which could allow GPS velocity jumps to start the engine. For hand launches and bungee launches this should be set to around 15. Also see TKOFF_ACCEL_CNT paramter for control of full "shake to arm".</source>
      <translation>在自动起飞中启用地速检查之前的最小向前加速度（以 m/s/s 为单位）。这是用于手动发射的。将此值设置为 0 将禁用加速测试，这意味着地面速度检查将始终处于启用状态，这可能允许 GPS 速度跳跃来启动发动机。对于手动发射和蹦极发射，应设置为 15 左右。另请参阅 TKOFF_ACCEL_CNT 参数以控制完整的“摇动手臂”。</translation>
    </message>
    <message>
      <source>Minimum height above ground the vehicle is allowed to descend to during Mission and RTL, excluding landing commands. Requires a distance sensor to be set up. Note: only prevents the vehicle from descending further, but does not force it to climb. Set to a negative value to disable.</source>
      <translation>在任务和 RTL 期间允许飞行器下降到的离地最小高度，不包括着陆命令。需要设置距离传感器。注意：仅阻止车辆进一步下降，但不强制其爬升。设置为负值以禁用。</translation>
    </message>
    <message>
      <source>Minimum number of satellites visible to use GPS for velocity based corrections attitude correction. This defaults to 6, which is about the point at which the velocity numbers from a GPS become too unreliable for accurate correction of the accelerometers.</source>
      <translation>使用 GPS 进行基于速度的修正姿态修正的可见卫星的最小数量。默认为 6，大约是 GPS 的速度数变得太不可靠而无法精确校正加速度计的点。</translation>
    </message>
    <message>
      <source>Minimum pitch angle during hover</source>
      <translation>悬停时的最小俯仰角</translation>
    </message>
    <message>
      <source>Minimum range finder altitude in m to trigger payload touchdown, set to zero to disable.</source>
      <translation>触发有效负载接地的最小测距仪高度（以米为单位），设置为零以禁用。</translation>
    </message>
    <message>
      <source>Minimum sink rate when at THR_MIN and TRIM_ARSPD_CM.</source>
      <translation>THR_MIN 和 TRIM_ARSPD_CM 时的最小下沉率。</translation>
    </message>
    <message>
      <source>Minimum throttle for simulated ins noise</source>
      <translation>模拟INS噪声的最小油门</translation>
    </message>
    <message>
      <source>Minimum value of the parameter to be displayed and modified</source>
      <translation>显示和修改参数的最小值</translation>
    </message>
    <message>
      <source>Mission</source>
      <translation>使命</translation>
    </message>
    <message>
      <source>Mission options bitmask</source>
      <translation>任务选项位掩码</translation>
    </message>
    <message>
      <source>Mixing of pitch and throttle correction for height and airspeed errors. Pitch controls altitude and throttle controls airspeed if set to 0. Pitch controls airspeed and throttle controls altitude if set to 2 (good for gliders). Blended if set to 1.</source>
      <translation>混合俯仰和油门修正高度和空速误差。如果设置为 0，则俯仰控制高度，油门控制空速。如果设置为 2（适合滑翔机），则俯仰控制空速，油门控制高度。如果设置为 1，则混合。</translation>
    </message>
    <message>
      <source>Mode slot 6</source>
      <translation>模式插槽 6</translation>
    </message>
    <message>
      <source>Mode1+2+3+4</source>
      <translation>型号1+2+3+4</translation>
    </message>
    <message>
      <source>Mode1+2+4+5</source>
      <translation>型号1+2+4+5</translation>
    </message>
    <message>
      <source>Mode1+3+4+5</source>
      <translation>型号1+3+4+5</translation>
    </message>
    <message>
      <source>Mode1+4+5+6</source>
      <translation>型号1+4+5+6</translation>
    </message>
    <message>
      <source>Mode2+3+4+5</source>
      <translation>型号2+3+4+5</translation>
    </message>
    <message>
      <source>Mode2+4+5+6</source>
      <translation>型号2+4+5+6</translation>
    </message>
    <message>
      <source>Mode3+4+5+6</source>
      <translation>型号3+4+5+6</translation>
    </message>
    <message>
      <source>Mode4+6</source>
      <translation>模式4+6</translation>
    </message>
    <message>
      <source>ModeAutoTune</source>
      <translation>自动调谐模式</translation>
    </message>
    <message>
      <source>ModeManual</source>
      <translation>模式手册</translation>
    </message>
    <message>
      <source>ModeTraining</source>
      <translation>模式训练</translation>
    </message>
    <message>
      <source>Modes available for sbgECom driver. In Sensors Only mode, use external IMU and magnetometer. In GNSS mode, use external GNSS in addition to sensors only mode. In INS mode, use external Kalman Filter in addition to GNSS mode. In INS mode, requires EKF2_EN 0. Keeping both enabled can lead to an unexpected behavior and vehicle instability.</source>
      <translation>sbgECom 驱动程序可用的模式。在仅传感器模式下，使用外部 IMU 和磁力计。在 GNSS 模式下，除仅传感器模式外，还可以使用外部 GNSS。在 INS 模式下，除 GNSS 模式外，还需使用外部卡尔曼滤波器。在INS模式下，需要EKF2_EN和0。保持两者启用可能会导致意外行为和车辆不稳定。</translation>
    </message>
    <message>
      <source>Module ID 15</source>
      <translation>模块 ID 15</translation>
    </message>
    <message>
      <source>Module ID 22</source>
      <translation>模块 ID 22</translation>
    </message>
    <message>
      <source>Module ID 3</source>
      <translation>模块 ID 3</translation>
    </message>
    <message>
      <source>Module ID 37</source>
      <translation>模块 ID 37</translation>
    </message>
    <message>
      <source>Module ID 44</source>
      <translation>模块 ID 44</translation>
    </message>
    <message>
      <source>Module ID 51</source>
      <translation>模块 ID 51</translation>
    </message>
    <message>
      <source>Module ID 59</source>
      <translation>模块 ID 59</translation>
    </message>
    <message>
      <source>Module IDs [0, 31] that you would like to request telemetry from</source>
      <translation>您想要请求遥测的模块 ID [0、31]</translation>
    </message>
    <message>
      <source>Module Param - Maximum voltage when CONTROL_MODE is set to Voltage</source>
      <translation>模块参数 - CONTROL_MODE 设置为电压时的最大电压</translation>
    </message>
    <message>
      <source>Moment coefficient of rotor 1</source>
      <translation>转子力矩系数1</translation>
    </message>
    <message>
      <source>Moment coefficient of rotor 7</source>
      <translation>转子力矩系数7</translation>
    </message>
    <message>
      <source>Motor 1</source>
      <translation>电机1</translation>
    </message>
    <message>
      <source>Motor 2 slew rate limit</source>
      <translation>电机 2 转换速率限制</translation>
    </message>
    <message>
      <source>Motor 6 slew rate limit</source>
      <translation>电机 6 转换速率限制</translation>
    </message>
    <message>
      <source>Motor Current Max Time Constant</source>
      <translation>电机电流最大时间常数</translation>
    </message>
    <message>
      <source>Motor Failure Time Threshold</source>
      <translation>电机故障时间阈值</translation>
    </message>
    <message>
      <source>Motor Thrust Asymmetry</source>
      <translation>电机推力不对称</translation>
    </message>
    <message>
      <source>Motor failure triggers only if the throttle threshold and the
current to throttle threshold are violated for this time.</source>
      <translation>仅当此次违反油门阈值和电流至油门阈值时，才会触发电机故障。</translation>
    </message>
    <message>
      <source>Motor interference compensation to compass2 for body frame Y axis</source>
      <translation>机身Y轴对罗盘2电机干扰补偿</translation>
    </message>
    <message>
      <source>Motor mask, allowing external simulators to mark motors</source>
      <translation>电机掩模，允许外部模拟器标记电机</translation>
    </message>
    <message>
      <source>Motor thrust curve exponent (from 0 for linear to 1.0 for second order curve)</source>
      <translation>电机推力曲线指数（从线性的 0 到二阶曲线的 1.0）</translation>
    </message>
    <message>
      <source>Motor14</source>
      <translation>电机14</translation>
    </message>
    <message>
      <source>Motor21</source>
      <translation>电机21</translation>
    </message>
    <message>
      <source>Motor29</source>
      <translation>电机29</translation>
    </message>
    <message>
      <source>Motor7</source>
      <translation>电机7</translation>
    </message>
    <message>
      <source>Motors EStopped</source>
      <translation>电机紧急停止</translation>
    </message>
    <message>
      <source>Mount POI distance max</source>
      <translation>安装 POI 最大距离</translation>
    </message>
    <message>
      <source>Mount Roll/Pitch Lock</source>
      <translation>云台横滚/俯仰锁定</translation>
    </message>
    <message>
      <source>Mount default operating mode on startup and after control is returned from autopilot</source>
      <translation>启动时以及从自动驾驶仪返回控制权后安装默认操作模式</translation>
    </message>
    <message>
      <source>Mount output mode</source>
      <translation>贴装输出方式</translation>
    </message>
    <message>
      <source>Mount yaw angle when in neutral position</source>
      <translation>处于中立位置时安装偏航角</translation>
    </message>
    <message>
      <source>Mount1Roll</source>
      <translation>安装一卷</translation>
    </message>
    <message>
      <source>Mount2 roll angle when in retracted position</source>
      <translation>Mount2 处于缩回位置时的侧倾角</translation>
    </message>
    <message>
      <source>Mount2's maximum physical tilt (pitch) angular position</source>
      <translation>Mount2的最大物理倾斜（俯仰）角位置</translation>
    </message>
    <message>
      <source>Mount2's minimum tilt angle</source>
      <translation>Mount2的最小倾斜角度</translation>
    </message>
    <message>
      <source>Mount2Roll</source>
      <translation>贴装2卷</translation>
    </message>
    <message>
      <source>Move Perpendicularly in 3D</source>
      <translation>在 3D 中垂直移动</translation>
    </message>
    <message>
      <source>Moving Median Filter</source>
      <translation>移动中值滤波器</translation>
    </message>
    <message>
      <source>Multi GPS primary instance</source>
      <translation>多GPS主实例</translation>
    </message>
    <message>
      <source>Multicopter autotune module enable</source>
      <translation>多旋翼自动调谐模块使能</translation>
    </message>
    <message>
      <source>Multiplied by period for conservative minimum period bounding (when period lower bounding is enabled). 1.0 bounds at marginal stability.</source>
      <translation>乘以保守最小周期边界的周期（当启用周期下限时）。 1.0 处于边际稳定性范围。</translation>
    </message>
    <message>
      <source>Multiplied by the current throttle and added to compass3's x-axis values to compensate for motor interference (Offset per Amp or at Full Throttle)</source>
      <translation>乘以当前油门并添加到罗盘3的x轴值以补偿电机干扰（每安培或全油门时的偏移）</translation>
    </message>
    <message>
      <source>Multiplied by the current throttle and added to the compass's y-axis values to compensate for motor interference (Offset per Amp or at Full Throttle)</source>
      <translation>乘以当前油门并添加到罗盘的 y 轴值以补偿电机干扰（每安培或全油门时的偏移）</translation>
    </message>
    <message>
      <source>Multiplying this factor with the current absolute wind estimate gives the airspeed offset
added to the minimum airspeed setpoint limit. This helps to make the
system more robust against disturbances (turbulence) in high wind.</source>
      <translation>将此系数乘以当前绝对风估计值即可得到添加到最小空速设定点限制的空速偏移。这有助于使系统在强风中抵抗干扰（湍流）更加稳健。</translation>
    </message>
    <message>
      <source>Must have value "1" if crashdump data is present on the system, or a prearm failure will be raised.  Do not set this parameter unless the risks of doing so are fully understood.  The presence of a crash dump means that the firmware currently installed has suffered a critical software failure which resulted in the autopilot immediately rebooting.  The crashdump file gives diagnostic information which can help in finding the issue, please contact the ArduPIlot support team.  If this crashdump data is present, the vehicle is likely unsafe to fly.  Check the ArduPilot documentation for more details.</source>
      <translation>如果系统上存在故障转储数据，则必须具有值“1”，否则将引发预报警故障。  除非完全了解这样做的风险，否则请勿设置此参数。  故障转储的存在意味着当前安装的固件遇到了严重的软件故障，导致自动驾驶仪立即重新启动。  故障转储文件提供了有助于查找问题的诊断信息，请联系 ArduPIlot 支持团队。  如果存在此故障转储数据，则飞行器可能无法安全飞行。  查看 ArduPilot 文档了解更多详细信息。</translation>
    </message>
    <message>
      <source>NET_P3</source>
      <translation>NET_P3</translation>
    </message>
    <message>
      <source>NMEA AIVDM message</source>
      <translation>NMEA AIVDM 消息</translation>
    </message>
    <message>
      <source>NPFG damping ratio</source>
      <translation>NPFG阻尼比</translation>
    </message>
    <message>
      <source>NUMERICAL_VARIO</source>
      <translation>NUMERICAL_VARIO</translation>
    </message>
    <message>
      <source>Navigation controller selection</source>
      <translation>导航控制器选择</translation>
    </message>
    <message>
      <source>NearlyAll</source>
      <translation>几乎全部</translation>
    </message>
    <message>
      <source>NeoPixelRGB</source>
      <translation>NeoPixelRGB</translation>
    </message>
    <message>
      <source>Never broadcast</source>
      <translation>从未播出过</translation>
    </message>
    <message>
      <source>No GPS</source>
      <translation>无 GPS 数据</translation>
    </message>
    <message>
      <source>No cone, always climb to RTL_RETURN_ALT above destination.</source>
      <translation>没有锥体，始终爬升到目的地上方的 RTL_RETURN_ALT。</translation>
    </message>
    <message>
      <source>No rescale</source>
      <translation>无需重新缩放</translation>
    </message>
    <message>
      <source>NoEmergency</source>
      <translation>无紧急情况</translation>
    </message>
    <message>
      <source>Non-GPS operation position uncertainty (m)</source>
      <translation>非GPS操作位置不确定度（m）</translation>
    </message>
    <message>
      <source>Normal Start-up</source>
      <translation>正常启动</translation>
    </message>
    <message>
      <source>Not Used</source>
      <translation>未使用</translation>
    </message>
    <message>
      <source>Notch Filter center frequency</source>
      <translation>陷波滤波器中心频率</translation>
    </message>
    <message>
      <source>Notch filter 2 frequency for gyro</source>
      <translation>陀螺仪陷波滤波器 2 频率</translation>
    </message>
    <message>
      <source>Notice</source>
      <translation>提示</translation>
    </message>
    <message>
      <source>Nudge approach angle</source>
      <translation>微移接近角</translation>
    </message>
    <message>
      <source>Number of APM board resets</source>
      <translation>APM板复位次数</translation>
    </message>
    <message>
      <source>Number of amps that a 1V reading on the current sensor corresponds to. With a Pixhawk using the 3DR Power brick this should be set to 17. For the Pixhawk with the 3DR 4in1 ESC this should be 17. For Synthetic Current sensor monitors, this is the maximum, full throttle current draw.</source>
      <translation>电流传感器上的 1V 读数对应的安培数。对于使用 3DR 电源块的 Pixhawk，应将其设置为 17。对于配备 3DR 4in1 ESC 的 Pixhawk，这应该是 17。对于合成电流传感器监视器，这是最大的全油门电流消耗。</translation>
    </message>
    <message>
      <source>Number of geofence points currently loaded</source>
      <translation>当前加载的地理围栏点数量</translation>
    </message>
    <message>
      <source>Number of polygon points saved in eeprom (do not update manually)</source>
      <translation>eeprom中保存的多边形点数（不要手动更新）</translation>
    </message>
    <message>
      <source>Number of seconds after completion of tune to auto-save. This is useful when using a 2 position switch for quicktune</source>
      <translation>调谐完成后自动保存的秒数。当使用 2 位置开关进行快速调谐时，这非常有用</translation>
    </message>
    <message>
      <source>Number of times board has been booted</source>
      <translation>单板已启动次数</translation>
    </message>
    <message>
      <source>OADatabase item timeout. The time an item will linger without any updates before it expires. Zero means never expires which is useful for a sent-once static environment but terrible for dynamic ones.</source>
      <translation>OADatabase 项目超时。项目在过期前未进行任何更新的情况下停留的时间。零意味着永不过期，这对于发送一次的静态环境很有用，但对于动态环境则很糟糕。</translation>
    </message>
    <message>
      <source>OADatabase will reject obstacle's if vehicle's altitude above home is below this parameter, in a 3 meter radius around home. Set 0 to disable this feature.</source>
      <translation>如果车辆高于家的高度低于此参数（在家周围 3 米半径内），OADatabase 将拒绝障碍物。设置 0 以禁用此功能。</translation>
    </message>
    <message>
      <source>ODI_Y in the compass3 soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>罗盘 3 软铁校准矩阵中的 ODI_Y 元素：[[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</translation>
    </message>
    <message>
      <source>OF Loiter kI</source>
      <translation>OF 盘旋 kI</translation>
    </message>
    <message>
      <source>OSD RC Stick commands</source>
      <translation>OSD RC 摇杆指令</translation>
    </message>
    <message>
      <source>OSD type. TXONLY makes the OSD parameter selection available to other modules even if there is no native OSD support on the board, for instance CRSF.</source>
      <translation>OSD型。即使板上没有本机 OSD 支持（例如 CRSF），TXONLY 也可以使 OSD 参数选择可供其他模块使用。</translation>
    </message>
    <message>
      <source>Observation noise for drag acceleration</source>
      <translation>阻力加速度的观测噪声</translation>
    </message>
    <message>
      <source>Offboard</source>
      <translation>船外</translation>
    </message>
    <message>
      <source>Offset applied to AHRS pitch used for in-flight pitch trimming. Correct ground leveling is better than changing this parameter.</source>
      <translation>应用于 AHRS 俯仰的偏移，用于飞行中俯仰微调。正确的地面平整比更改此参数更好。</translation>
    </message>
    <message>
      <source>Offset for yaw channel output in degrees</source>
      <translation>偏航通道输出的偏移（以度为单位）</translation>
    </message>
    <message>
      <source>Offset term</source>
      <translation>抵消项</translation>
    </message>
    <message>
      <source>Offset to be added to the compass x-axis values to compensate for metal in the frame</source>
      <translation>添加到罗盘 x 轴值的偏移量以补偿框架中的金属</translation>
    </message>
    <message>
      <source>On</source>
      <translation>开启</translation>
    </message>
    <message>
      <source>Onboard</source>
      <translation>机上</translation>
    </message>
    <message>
      <source>Only climb to at least RTL_DESCEND_ALT above destination.</source>
      <translation>仅爬升至高于目的地至少 RTL_DESCEND_ALT 的位置。</translation>
    </message>
    <message>
      <source>Only relevant in Velocity Mode. This is the velocity the controller will command at full throttle.</source>
      <translation>仅在速度模式下相关。这是控制器在全油门下命令的速度。</translation>
    </message>
    <message>
      <source>Only used with MPC_POS_MODE Direct velocity or MPC_ALT_MODE 2</source>
      <translation>仅与 MPC_POS_MODE 直接速度或 MPC_ALT_MODE 2 一起使用</translation>
    </message>
    <message>
      <source>Opflow Rate</source>
      <translation>操作流量率</translation>
    </message>
    <message>
      <source>Optical Flow data will only be used on the ground if the sensor reports a quality metric &gt;= EKF2_OF_QMIN_GND</source>
      <translation>仅当传感器报告质量指标 &gt;= EKF2_OF_QMIN_GND 时，光流数据才会在地面上使用</translation>
    </message>
    <message>
      <source>Optical flow data maximum publication rate. This is an upper bound,
actual optical flow data rate is still dependent on the sensor.</source>
      <translation>光流数据最大发布率。这是上限，实际光流数据速率仍然取决于传感器。</translation>
    </message>
    <message>
      <source>Optical flow measurement noise (rad/s)</source>
      <translation>光流测量噪声（rad/s）</translation>
    </message>
    <message>
      <source>Optical flow rotation (roll/pitch) noise gain</source>
      <translation>光流旋转（横滚/俯仰）噪声增益</translation>
    </message>
    <message>
      <source>OpticalFlow may be used if quality is above this threshold</source>
      <translation>如果质量高于此阈值，则可以使用 OpticalFlow</translation>
    </message>
    <message>
      <source>Options for ICE control</source>
      <translation>ICE 控制选项</translation>
    </message>
    <message>
      <source>Options for Pin 2. PWM input detects PWM above or below 1800/1200us instead of logic level. Invert changes HIGH state to be logic low voltage on pin, or below 1200us, if PWM input.</source>
      <translation>引脚 2 的选项。 PWM 输入检测 PWM 高于或低于 1800/1200us，而不是逻辑电平。反转将 HIGH 状态更改为引脚上的逻辑低电压，或低于 1200us（如果 PWM 输入）。</translation>
    </message>
    <message>
      <source>Options for temperature calibration</source>
      <translation>温度校准选项</translation>
    </message>
    <message>
      <source>Options to change behavior of terrain system. The Accept Old Terrain Data option can be used to accept terrain data generated from before the terrain database bugs were fixed. The bugs in the data were all fixed from 24th February 2026. If you really want to risk using the old terrain data then you can set this option, otherwise remove the old terrain data by formatting your microSD card or renaming the /APM/TERRAIN folder. Then downloaded updated data from https://terrain.ardupilot.org, or let the automatic terrain download repopulate your terrain data.</source>
      <translation>用于更改地形系统行为的选项。“接受旧地形数据”允许使用地形数据库错误修复之前生成的数据。数据中的相关错误已于 2026 年 2 月 24 日全部修复。确需承担风险继续使用旧数据时，可以启用此选项；否则应格式化 microSD 卡，或重命名 /APM/TERRAIN 文件夹以移除旧数据，然后从 https://terrain.ardupilot.org 下载更新的数据，或由自动地形下载功能重新填充。</translation>
    </message>
    <message>
      <source>OreoLED Theme</source>
      <translation>奥利奥LED主题</translation>
    </message>
    <message>
      <source>Original Position (Heading)</source>
      <translation>原始位置（标题）</translation>
    </message>
    <message>
      <source>Output 12</source>
      <translation>输出12</translation>
    </message>
    <message>
      <source>Output 2</source>
      <translation>输出2</translation>
    </message>
    <message>
      <source>Output 27</source>
      <translation>输出27</translation>
    </message>
    <message>
      <source>Output 5</source>
      <translation>输出5</translation>
    </message>
    <message>
      <source>Output Protocol Configuration for MAIN 5-8</source>
      <translation>MAIN 5-8 的输出协议配置</translation>
    </message>
    <message>
      <source>Output channels to be transmitted as servo over UAVCAN</source>
      <translation>通过 UAVCAN 作为舵机传输的输出通道</translation>
    </message>
    <message>
      <source>Output slew time for increasing throttle</source>
      <translation>增加油门的输出转换时间</translation>
    </message>
    <message>
      <source>Output16</source>
      <translation>输出16</translation>
    </message>
    <message>
      <source>Output23</source>
      <translation>输出23</translation>
    </message>
    <message>
      <source>Output30</source>
      <translation>输出30</translation>
    </message>
    <message>
      <source>Output9</source>
      <translation>输出9</translation>
    </message>
    <message>
      <source>Overall board orientation relative to the standard orientation for the board type. This rotates the IMU and compass readings to allow the board to be oriented in your vehicle at any 90 or 45 degree angle. This option takes affect on next boot. After changing you will need to re-level your vehicle.</source>
      <translation>相对于板类型的标准方向的整体板方向。这会旋转 IMU 和罗盘读数，使板能够在车辆中以任何 90 或 45 度角定向。该选项在下次启动时生效。更换后，您需要重新调平车辆。</translation>
    </message>
    <message>
      <source>Overrides PTCH_LIM_MIN_DEG in automatic throttle modes to reduce descent rates. Uses PTCH_LIM_MIN_DEG if set to 0. For proper TECS tuning, set to the angle that the aircraft can descend at without overspeeding.</source>
      <translation>在自动油门模式下覆盖 PTCH_LIM_MIN_DEG 以降低下降率。如果设置为 0，则使用 PTCH_LIM_MIN_DEG。为了正确调整 TECS，请将飞机设置为飞机可以在不超速的情况下下降的角度。</translation>
    </message>
    <message>
      <source>P gain for path error corrections</source>
      <translation>用于路径误差修正的 P 增益</translation>
    </message>
    <message>
      <source>PARAM2 tuning channel</source>
      <translation>参数 2 调节通道</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 1 Minimum Value</source>
      <translation>PCA9685 输出通道 1 最小值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 11</source>
      <translation>PCA9685 输出通道 11</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 12 Failsafe Value</source>
      <translation>PCA9685 输出通道 12 故障安全值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 13 Minimum Value</source>
      <translation>PCA9685 输出通道 13 最小值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 15</source>
      <translation>PCA9685 输出通道 15</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 16 Failsafe Value</source>
      <translation>PCA9685 输出通道 16 故障安全值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 2 Minimum Value</source>
      <translation>PCA9685 输出通道 2 最小值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 4</source>
      <translation>PCA9685 输出通道 4</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 5 Failsafe Value</source>
      <translation>PCA9685 输出通道 5 故障安全值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 6 Minimum Value</source>
      <translation>PCA9685 输出通道 6 最小值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 8</source>
      <translation>PCA9685 输出通道 8</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 9 Failsafe Value</source>
      <translation>PCA9685 输出通道 9 故障安全值</translation>
    </message>
    <message>
      <source>PCNC1</source>
      <translation>PCNC1</translation>
    </message>
    <message>
      <source>PID Error filter frequency in Hz</source>
      <translation>PID 误差滤波器频率（Hz）</translation>
    </message>
    <message>
      <source>PID Target notch filter index, zero disables</source>
      <translation>PID 目标陷波滤波器指数，零禁用</translation>
    </message>
    <message>
      <source>PMW3901 Optical Flow</source>
      <translation>PMW3901光流</translation>
    </message>
    <message>
      <source>PPP</source>
      <translation>PPP</translation>
    </message>
    <message>
      <source>PWM 50 Hz</source>
      <translation>PWM 50 Hz</translation>
    </message>
    <message>
      <source>PWM Aux 2</source>
      <translation>PWM 辅助 2</translation>
    </message>
    <message>
      <source>PWM Aux 3 Center Value</source>
      <translation>PWM 辅助 3 中心值</translation>
    </message>
    <message>
      <source>PWM Aux 4 Disarmed Value</source>
      <translation>PWM 辅助 4 上锁值</translation>
    </message>
    <message>
      <source>PWM Aux 5 Failsafe Value</source>
      <translation>PWM 辅助 5 故障安全值</translation>
    </message>
    <message>
      <source>PWM Aux 6 Maximum Value</source>
      <translation>PWM 辅助 6 最大值</translation>
    </message>
    <message>
      <source>PWM Aux 7 Minimum Value</source>
      <translation>PWM 辅助 7 最小值</translation>
    </message>
    <message>
      <source>PWM Aux 8 Output Function</source>
      <translation>PWM 辅助 8 输出功能</translation>
    </message>
    <message>
      <source>PWM Capture 2</source>
      <translation>PWM 捕捉 2</translation>
    </message>
    <message>
      <source>PWM Capture 3 Center Value</source>
      <translation>PWM 捕获 3 中心值</translation>
    </message>
    <message>
      <source>PWM Mode: Commands a fraction of battery voltage. This changes as the battery voltage changes. This is the least safe mode because the upper throttle limit is determined by the battery voltage. Voltage Mode: Commands a voltage. The motor will behave the same way throughout the life of a battery, assuming the commanded voltage is less than the battery voltage. You must set the MAX_VOLTS parameter. Velocity Mode: Closed-loop, commands a velocity. The controller will adjust the applied voltage so that the motor spins at the commanded velocity. This mode has faster reaction times. Only use this if you know the properties of your propeller. You must set the MAX_VELOCITY parameter.</source>
      <translation>PWM 模式：命令电池电压的一部分。这会随着电池电压的变化而变化。这是最不安全的模式，因为油门上限是由电池电压决定的。电压模式：命令电压。假设命令电压小于电池电压，电机在电池的整个使用寿命期间都会以相同的方式运行。您必须设置 MAX_VOLTS 参数。速度模式：闭环，命令速度。控制器将调整施加的电压，使电机以指令速度旋转。此模式的反应时间更快。仅当您了解螺旋桨的特性时才使用此功能。您必须设置 MAX_VELOCITY 参数。</translation>
    </message>
    <message>
      <source>PWM in microseconds sent to external gyro on ch7 when tail type is Servo w/ ExtGyro. A value of zero means to use H_GYR_GAIN</source>
      <translation>当尾部类型为带 ExtGyro 的舵机时，PWM 以微秒为单位发送到 ch7 上的外部陀螺仪。值为零表示使用 H_GYR_GAIN</translation>
    </message>
    <message>
      <source>PWM re-enable delay after arming when disarmed pwm was disabled</source>
      <translation>当禁用 pwm 上锁时，PWM 解锁后重新启用延迟</translation>
    </message>
    <message>
      <source>PWM value in microseconds sent to Gripper to release the cargo</source>
      <translation>PWM 值（以微秒为单位）发送到 Gripper 以释放货物</translation>
    </message>
    <message>
      <source>PWM value that the radio receiver will put on the RSSI_CHANNEL or RSSI_ANA_PIN when the signal strength is the weakest. Some radio receivers output inverted values so this value may be lower than RSSI_CHAN_HIGH</source>
      <translation>当信号强度最弱时，无线电接收器将在 RSSI_CHANNEL 或 RSSI_ANA_PIN 上设置的 PWM 值。某些无线电接收器输出反转值，因此该值可能低于 RSSI_CHAN_HIGH</translation>
    </message>
    <message>
      <source>PX4 AUX2</source>
      <translation>PX4 AUX2</translation>
    </message>
    <message>
      <source>PX4 Flow Optical Flow</source>
      <translation>PX4 流量光流</translation>
    </message>
    <message>
      <source>PX4IO ACC1</source>
      <translation>PX4IO ACC1</translation>
    </message>
    <message>
      <source>Parachute 3pos</source>
      <translation>降落伞 3 位</translation>
    </message>
    <message>
      <source>Parachute min altitude in meters above home</source>
      <translation>降落伞最低海拔高度（米）</translation>
    </message>
    <message>
      <source>Parachute release mechanism type (relay number in versions prior to 4.5, or servo). Values 0-3 all are relay. Relay number used for release is set by RELAYx_FUNCTION in 4.5 or later. </source>
      <translation>降落伞释放机构类型（4.5之前版本中的继电器编号，或舵机）。 0-3 均为继电器。用于释放的继电器编号由 4.5 或更高版本中的 RELAYx_FUNCTION 设置。 </translation>
    </message>
    <message>
      <source>Parameter maximum</source>
      <translation>参数最大值</translation>
    </message>
    <message>
      <source>Parameter version</source>
      <translation>参数版本</translation>
    </message>
    <message>
      <source>Path navigation roll slew rate limit</source>
      <translation>路径导航横滚转换速率限制</translation>
    </message>
    <message>
      <source>Pedestrian</source>
      <translation>行人</translation>
    </message>
    <message>
      <source>Percentage of normal rotor speed where flight is no longer possible. However currently the rotor runup/rundown is estimated using the RSC_RUNUP_TIME parameter.   Estimated rotor speed increases/decreases between 0 (rotor stopped) to 1 (rotor at normal speed) in the RSC_RUNUP_TIME in seconds. This parameter should be set so that the estimated rotor speed goes below critical in approximately 3 seconds.  So if you had a 10 second runup time then set RSC_CRITICAL to 70%.</source>
      <translation>无法再飞行的正常旋翼速度的百分比。然而，目前转子加速/减速是使用 RSC_RUNUP_TIME 参数估计的。   在 RSC_RUNUP_TIME 中，估计转子速度在 0（转子停止）到 1（转子正常速度）之间以秒为单位增加/减少。应设置此参数，以便估计转子速度在大约 3 秒内降至临界值以下。  因此，如果您有 10 第二次启动时间，则将 RSC_CRITICAL 设置为 70%。</translation>
    </message>
    <message>
      <source>Period in seconds of L1 tracking loop. This parameter is the primary control for agressiveness of turns in auto mode. This needs to be larger for less responsive airframes. The default of 20 is quite conservative, but for most RC aircraft will lead to reasonable flight. For smaller more agile aircraft a value closer to 15 is appropriate, or even as low as 10 for some very agile aircraft. When tuning, change this value in small increments, as a value that is much too small (say 5 or 10 below the right value) can lead to very radical turns, and a risk of stalling.</source>
      <translation>L1 跟踪循环的周期（以秒为单位）。该参数是自动模式下转弯主动性的主要控制。对于响应速度较慢的机身，该值需要更大。 20的默认设置相当保守，但对于大多数RC飞机来说会导致合理的飞行。对于较小且敏捷的飞机，接近 15 的值是合适的，对于某些非常敏捷的飞机，甚至低至 10。调整时，以较小的增量更改该值，因为太小的值（例如 5 或 10 低于正确值）可能会导致非常剧烈的转弯，并有失速的风险。</translation>
    </message>
    <message>
      <source>Peripheral via Actuator Set 6</source>
      <translation>通过执行器套件 6 的外围设备</translation>
    </message>
    <message>
      <source>Physical coefficient representing the friction with air particules. The greater this value, the slower the quad will move. Drag force function of velocity: D=-KDV*V. The maximum freefall velocity can be computed as V=10*MASS/KDV [m/s]</source>
      <translation>表示与空气颗粒的摩擦力的物理系数。该值越大，四边形的移动速度越慢。速度的曳力函数：D=-KDV*V。最大自由落体速度可计算为 V=10*MASS/KDV [m/s]</translation>
    </message>
    <message>
      <source>Pilot controlled yaw rate max.  Used in all pilot controlled modes except Acro</source>
      <translation>飞行员控制的最大偏航率  用于除 Acro 之外的所有飞行员控制模式</translation>
    </message>
    <message>
      <source>Pilot maximum vertical speed ascending</source>
      <translation>飞行员最大垂直速度上升</translation>
    </message>
    <message>
      <source>Pilot yaw rate control input time constant.  Low numbers lead to sharper response, higher numbers to softer response</source>
      <translation>飞行员偏航角速度控制输入时间常数。  数字越低，响应越敏锐，数字越大，响应越柔和</translation>
    </message>
    <message>
      <source>Pin number to use for feedback of weight on wheels condition. If set to -1 feedback is disabled.</source>
      <translation>用于反馈车轮重量状况的引脚号。如果设置为 -1 反馈将被禁用。</translation>
    </message>
    <message>
      <source>Pirouette compensation enabled</source>
      <translation>启用旋转补偿</translation>
    </message>
    <message>
      <source>Pitch Derivative FeedForward Gain</source>
      <translation>俯仰导数前馈增益</translation>
    </message>
    <message>
      <source>Pitch acceleration limit. Setting to zero disables input shaping.</source>
      <translation>俯仰加速度限制。设置为零会禁用输入整形。</translation>
    </message>
    <message>
      <source>Pitch arm length</source>
      <translation>俯仰臂长度</translation>
    </message>
    <message>
      <source>Pitch axis controller I gain.  Corrects long-term difference in desired pitch angle vs actual pitch angle</source>
      <translation>俯仰轴控制器 I 增益。  纠正所需桨距角与实际桨距角的长期差异</translation>
    </message>
    <message>
      <source>Pitch axis controller target frequency in Hz</source>
      <translation>俯仰轴控制器目标频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Pitch axis rate controller I gain maximum.  Constrains the maximum that the I term will output</source>
      <translation>俯仰轴速率控制器 I 增益最大。  限制 I 项输出的最大值</translation>
    </message>
    <message>
      <source>Pitch axis rate controller P gain.  Corrects in proportion to the difference between the desired pitch rate vs actual pitch rate output</source>
      <translation>俯仰轴速率控制器 P 增益。  根据所需俯仰速率与实际俯仰速率输出之间的差异按比例进行校正</translation>
    </message>
    <message>
      <source>Pitch axis rate controller target frequency in Hz</source>
      <translation>俯仰轴速率控制器目标频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Pitch control Error filter frequency in Hz</source>
      <translation>变桨控制 误差滤波器频率（Hz）</translation>
    </message>
    <message>
      <source>Pitch control P gain for BalanceBots.  Converts the error between the desired pitch (in radians) and actual pitch to a motor output (in the range -1 to +1)</source>
      <translation>BalanceBots 的俯仰控制 P 增益。  将所需俯仰（以弧度为单位）与实际俯仰之间的误差转换为电机输出（范围为 -1 至 +1）</translation>
    </message>
    <message>
      <source>Pitch control feed forward from current pitch angle</source>
      <translation>桨距控制从当前桨距角前馈</translation>
    </message>
    <message>
      <source>Pitch control slew rate limit</source>
      <translation>变桨控制转换速率限制</translation>
    </message>
    <message>
      <source>Pitch gain for manual inputs in attitude control mode</source>
      <translation>姿态控制模式下手动输入的俯仰增益</translation>
    </message>
    <message>
      <source>Pitch minimum movement time</source>
      <translation>俯仰最小移动时间</translation>
    </message>
    <message>
      <source>Pitch rate P gain</source>
      <translation>俯仰速率 P 增益</translation>
    </message>
    <message>
      <source>Pitch rate integral gain. Can be set to compensate static thrust difference or gravity center offset.</source>
      <translation>俯仰率积分增益。可设置补偿静态推力差或重心偏移。</translation>
    </message>
    <message>
      <source>Pitch slew rate limit</source>
      <translation>俯仰转换速率限制</translation>
    </message>
    <message>
      <source>Pitch-Axis</source>
      <translation>俯仰轴</translation>
    </message>
    <message>
      <source>Pitch270Roll180</source>
      <translation>俯仰270滚动180</translation>
    </message>
    <message>
      <source>Pitch90Roll90</source>
      <translation>俯仰90滚动90</translation>
    </message>
    <message>
      <source>Pivot turn rate</source>
      <translation>枢轴转动速率</translation>
    </message>
    <message>
      <source>Pixhawk 6.6ADC</source>
      <translation>Pixhawk 6.6ADC</translation>
    </message>
    <message>
      <source>Pixhawk AUXOUT5</source>
      <translation>皮克斯霍克 AUXOUT5</translation>
    </message>
    <message>
      <source>Pixhawk Pro</source>
      <translation>像素鹰专业版</translation>
    </message>
    <message>
      <source>Pixhawk-airspeed port</source>
      <translation>Pixhawk-空速端口</translation>
    </message>
    <message>
      <source>PixhawkAUX1</source>
      <translation>PixhawkAUX1</translation>
    </message>
    <message>
      <source>Plane AUTO Mode Landing Abort</source>
      <translation>飞机 AUTO 模式着陆中止</translation>
    </message>
    <message>
      <source>Plane Follow Scripting distance D gain</source>
      <translation>平面跟随脚本距离 D 增益</translation>
    </message>
    <message>
      <source>Plane Follow crosstrack PID controller D term</source>
      <translation>平面跟随交叉轨迹 PID 控制器 D 项</translation>
    </message>
    <message>
      <source>Plane landing abort for VTOL Payload Place or glide-slope go-around</source>
      <translation>VTOL 有效载荷放置或下滑道复飞的飞机着陆中止</translation>
    </message>
    <message>
      <source>PointObstacle</source>
      <translation>点障碍</translation>
    </message>
    <message>
      <source>Portable</source>
      <translation>便携式</translation>
    </message>
    <message>
      <source>PosZP</source>
      <translation>后ZP</translation>
    </message>
    <message>
      <source>Position Hold mode</source>
      <translation>位置保持模式</translation>
    </message>
    <message>
      <source>Position controller P gain</source>
      <translation>位置控制器P增益</translation>
    </message>
    <message>
      <source>Position of rotor 0 along Y body axis relative to center of gravity</source>
      <translation>转子 0 沿 Y 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 11 along X body axis relative to center of gravity</source>
      <translation>转子 11 沿 X 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 3 along Z body axis relative to center of gravity</source>
      <translation>转子 3 沿 Z 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 6 along Y body axis relative to center of gravity</source>
      <translation>转子 6 沿 Y 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 9 along X body axis relative to center of gravity</source>
      <translation>转子 9 沿 X 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Positive real value that configures the transmission frequency of the
HIGH_LATENCY2 stream for instance 1, configured in iridium mode.
This parameter has no effect if the instance mode is different from iridium.</source>
      <translation>配置 HIGH_LATENCY2 流的传输频率的正实数值，例如 1，在铱星模式下配置。如果实例模式与iridium不同，则该参数无效。</translation>
    </message>
    <message>
      <source>Postpone shooting if roll is greater than limit. (0=Disable, will shoot regardless of roll).</source>
      <translation>如果横滚大于限制，则推迟拍摄。 （0=禁用，无论横滚如何都会射击）。</translation>
    </message>
    <message>
      <source>Pre and post filter</source>
      <translation>前置和后置过滤器</translation>
    </message>
    <message>
      <source>PrecLand strictness</source>
      <translation>PrecLand严格性</translation>
    </message>
    <message>
      <source>Precision Landing Extra Options</source>
      <translation>精确着陆额外选项</translation>
    </message>
    <message>
      <source>Precland device center's height above sealevel</source>
      <translation>Precland装置中心海拔高度</translation>
    </message>
    <message>
      <source>Precland device orientation vector</source>
      <translation>Preclad 设备方向矢量</translation>
    </message>
    <message>
      <source>Precland device update rate</source>
      <translation>Precland 设备更新率</translation>
    </message>
    <message>
      <source>Pressure error coefficient in positive X direction (forward)</source>
      <translation>X正方向（正向）压力误差系数</translation>
    </message>
    <message>
      <source>Primary airspeed sensor</source>
      <translation>主空速传感器</translation>
    </message>
    <message>
      <source>Prio: MAVL 1 &gt; MAVL 2 &gt; RC</source>
      <translation>优先级：MAVL 1 &gt; MAVL 2 &gt; RC</translation>
    </message>
    <message>
      <source>Process noise for IMU accelerometer bias prediction</source>
      <translation>IMU 加速度计偏差预测的过程噪声</translation>
    </message>
    <message>
      <source>ProfiLED 1</source>
      <translation>轮廓1</translation>
    </message>
    <message>
      <source>ProfiLED_IOMCU</source>
      <translation>ProfileLED_IOMCU</translation>
    </message>
    <message>
      <source>Proportional gain for closed loop yaw rate controller</source>
      <translation>闭环偏航率控制器的比例增益</translation>
    </message>
    <message>
      <source>Proportional gain for vertical position error</source>
      <translation>垂直位置误差的比例增益</translation>
    </message>
    <message>
      <source>Proximity Avoidance</source>
      <translation>接近回避</translation>
    </message>
    <message>
      <source>Proximity sensor ignore angle 2</source>
      <translation>接近传感器忽略角度 2</translation>
    </message>
    <message>
      <source>Proximity sensor ignore width 4</source>
      <translation>接近传感器忽略宽度 4</translation>
    </message>
    <message>
      <source>Pseudo-inverse with sequential desaturation technique</source>
      <translation>伪逆序贯去饱和技术</translation>
    </message>
    <message>
      <source>Pump speed</source>
      <translation>泵速</translation>
    </message>
    <message>
      <source>Put CH14 to Duty-Cycle mode</source>
      <translation>将 CH14 置于占空比模式</translation>
    </message>
    <message>
      <source>Put CH7 to Duty-Cycle mode</source>
      <translation>将 CH7 置于占空比模式</translation>
    </message>
    <message>
      <source>Q mode forward throttle use</source>
      <translation>Q模式前进油门使用</translation>
    </message>
    <message>
      <source>QAssist 3pos</source>
      <translation>QAsist 3pos</translation>
    </message>
    <message>
      <source>QNH pressure</source>
      <translation>QNH压力</translation>
    </message>
    <message>
      <source>QUADROTOR</source>
      <translation>QUADROTOR</translation>
    </message>
    <message>
      <source>Quad-chute max pitch threshold</source>
      <translation>四溜槽最大俯仰阈值</translation>
    </message>
    <message>
      <source>Quadplane assistance altitude</source>
      <translation>四翼飞机辅助高度</translation>
    </message>
    <message>
      <source>Quadrature</source>
      <translation>正交</translation>
    </message>
    <message>
      <source>Quicktune auto filter enable</source>
      <translation>Quicktune 自动过滤器启用</translation>
    </message>
    <message>
      <source>Quicktune oscillation rate threshold</source>
      <translation>Quicktune 振荡率阈值</translation>
    </message>
    <message>
      <source>RC AUX 3</source>
      <translation>RC AUX 3</translation>
    </message>
    <message>
      <source>RC Channel to use for driving mode control</source>
      <translation>RC 用于驱动模式控制的通道</translation>
    </message>
    <message>
      <source>RC Input Channel to use as wind angle value</source>
      <translation>RC 输入通道用作风角值</translation>
    </message>
    <message>
      <source>RC Passthrough</source>
      <translation>RC 直通</translation>
    </message>
    <message>
      <source>RC channel 1 reverse</source>
      <translation>RC 通道 1 反向</translation>
    </message>
    <message>
      <source>RC channel 11 reverse</source>
      <translation>RC 通道 11 反向</translation>
    </message>
    <message>
      <source>RC channel 13 reverse</source>
      <translation>RC 通道 13 反向</translation>
    </message>
    <message>
      <source>RC channel 15 reverse</source>
      <translation>RC 通道 15 反向</translation>
    </message>
    <message>
      <source>RC channel 17 reverse</source>
      <translation>RC 通道 17 反向</translation>
    </message>
    <message>
      <source>RC channel 2 reverse</source>
      <translation>RC 通道 2 反向</translation>
    </message>
    <message>
      <source>RC channel 4 reverse</source>
      <translation>RC 通道 4 反向</translation>
    </message>
    <message>
      <source>RC channel 6 reverse</source>
      <translation>RC 通道 6 反向</translation>
    </message>
    <message>
      <source>RC channel 8 reverse</source>
      <translation>RC 通道 8 反向</translation>
    </message>
    <message>
      <source>RC channel to use to reset to last flight mode	after geofence takeover.</source>
      <translation>RC 通道用于在地理围栏接管后重置为上次飞行模式。</translation>
    </message>
    <message>
      <source>RC input option</source>
      <translation>RC 输入选项</translation>
    </message>
    <message>
      <source>RC maximum PWM pulse width. Typically 1000 is lower limit, 1500 is neutral and 2000 is upper limit.</source>
      <translation>RC PWM 最大脉冲宽度。通常，1000 为下限，1500 为中性，2000 为上限。</translation>
    </message>
    <message>
      <source>RC output</source>
      <translation>RC输出</translation>
    </message>
    <message>
      <source>RC trim (neutral) PWM pulse width. Typically 1000 is lower limit, 1500 is neutral and 2000 is upper limit.</source>
      <translation>RC 调整（中性）PWM 脉冲宽度。通常，1000 为下限，1500 为中性，2000 为上限。</translation>
    </message>
    <message>
      <source>RC8</source>
      <translation>RC8</translation>
    </message>
    <message>
      <source>RCIN11</source>
      <translation>RCIN11</translation>
    </message>
    <message>
      <source>RCIN15</source>
      <translation>RCIN15</translation>
    </message>
    <message>
      <source>RCIN3</source>
      <translation>RCIN3</translation>
    </message>
    <message>
      <source>RCIN5Scaled</source>
      <translation>RCIN5缩放</translation>
    </message>
    <message>
      <source>RCIN8/CameraTilt</source>
      <translation>RCIN8/相机倾斜</translation>
    </message>
    <message>
      <source>RC_ANT_Y</source>
      <translation>RC_ANT_Y</translation>
    </message>
    <message>
      <source>RC_SNR_EN</source>
      <translation>RC_SNR_EN</translation>
    </message>
    <message>
      <source>RESERVED</source>
      <translation>RESERVED</translation>
    </message>
    <message>
      <source>RNGF_EN</source>
      <translation>RNGF_EN</translation>
    </message>
    <message>
      <source>ROS2 RMW_ZENOH_CPP Domain id</source>
      <translation>ROS2 RMW_ZENOH_CPP 域 ID</translation>
    </message>
    <message>
      <source>ROTATION_PITCH_180_YAW_90</source>
      <translation>ROTATION_PITCH_180_YAW_90</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_180_YAW_135</source>
      <translation>ROTATION_ROLL_180_YAW_135</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_270_PITCH_270</source>
      <translation>ROTATION_ROLL_270_PITCH_270</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_90_PITCH_270</source>
      <translation>ROTATION_ROLL_90_PITCH_270</translation>
    </message>
    <message>
      <source>ROTATION_UPWARD_FACING</source>
      <translation>ROTATION_UPWARD_FACING</translation>
    </message>
    <message>
      <source>RPLidarA2</source>
      <translation>RP激光雷达A2</translation>
    </message>
    <message>
      <source>RPM range +/- governor rpm reference setting where governor is operational. If speed sensor fails or rpm falls outside of this range, the governor will disengage and return to throttle curve. Recommended range is 100</source>
      <translation>RPM 范围 +/- 调速器运行时的调速器转速参考设置。如果速度传感器发生故障或转速超出此范围，调速器将脱离并返回油门曲线。推荐范围是 100</translation>
    </message>
    <message>
      <source>RPM_X</source>
      <translation>RPM_X</translation>
    </message>
    <message>
      <source>RSSI pin's voltage received on the RSSI_ANA_PIN when the signal strength is the strongest. Some radio receivers put out inverted values so this value may be lower than RSSI_PIN_LOW. When using pin 103, the maximum value of the parameter is 3.3V.</source>
      <translation>当信号强度最强时，RSSI 引脚在 RSSI_ANA_PIN 上接收到的电压。一些无线电接收器输出反转值，因此该值可能低于 RSSI_PIN_LOW。当使用103 引脚时，该参数的最大值为3.3V。</translation>
    </message>
    <message>
      <source>RSSIDBM_Y</source>
      <translation>RSSIDBM_Y</translation>
    </message>
    <message>
      <source>RTK fixed</source>
      <translation>RTK 固定</translation>
    </message>
    <message>
      <source>RTL auto land</source>
      <translation>RTL自动着陆</translation>
    </message>
    <message>
      <source>RTL or Continue with Mission in Auto Mode (Deprecated in 4.0+-see FS_OPTIONS)</source>
      <translation>自动模式继续任务，否则返航</translation>
    </message>
    <message>
      <source>RTL will be entered when a thermal is exited and the plane is more than this distance from home. -1 to disable.</source>
      <translation>当热气流退出并且飞机离家的距离超过此距离时，将进入 RTL。 -1 禁用。</translation>
    </message>
    <message>
      <source>RTL_CLIMB_MIN should be &lt; 120m (400ft). 3 or less to prevent arming. -1 to disable.</source>
      <translation>RTL_CLIMB_MIN 应 &lt; 120m (400ft)。 3以下防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>RX_PullUp</source>
      <translation>RX_上拉</translation>
    </message>
    <message>
      <source>Rally Include Home</source>
      <translation>拉力赛包括主场</translation>
    </message>
    <message>
      <source>Range beacon measurement delay (msec)</source>
      <translation>距离信标测量延迟（毫秒）</translation>
    </message>
    <message>
      <source>Range finder can be used as the primary height source when below this percentage of its maximum range (see RNGFNDx_MAX_CM) and the primary height source is Baro or GPS (see EK3_SRCx_POSZ).  This feature should not be used for terrain following as it is designed for vertical takeoff and landing with climb above the range finder use height before commencing the mission, and with horizontal position changes below that height being limited to a flat region around the takeoff and landing point.</source>
      <translation>当测距仪低于其最大范围的该百分比（请参阅 RNGFNDx_MAX_CM）并且主要高度源是 Baro 或 GPS（请参阅 EK3_SRCx_POSZ）时，可以用作主要高度源。  此功能不应用于地形跟踪，因为它设计用于垂直起飞和着陆，在开始任务之前爬升至测距仪使用高度之上，并且低于该高度的水平位置变化仅限于起飞和着陆点周围的平坦区域。</translation>
    </message>
    <message>
      <source>Range of pitch channel output in degrees (only in AUX output mode)</source>
      <translation>俯仰通道输出范围（度）（仅在 AUX 输出模式下）</translation>
    </message>
    <message>
      <source>Range sensor pitch offset</source>
      <translation>距离传感器俯仰偏移</translation>
    </message>
    <message>
      <source>Rangefinder</source>
      <translation>测距仪</translation>
    </message>
    <message>
      <source>Rangefinder landing engagement distance</source>
      <translation>测距仪着陆交战距离</translation>
    </message>
    <message>
      <source>Rangefinder stop pin</source>
      <translation>测距仪止动销</translation>
    </message>
    <message>
      <source>Rate Pitch kI</source>
      <translation>速率节距 kI</translation>
    </message>
    <message>
      <source>Rate Roll/Pitch kI</source>
      <translation>横滚/俯仰速率 kI</translation>
    </message>
    <message>
      <source>Rate gyro bias process noise (rad/s)</source>
      <translation>速率陀螺仪偏置过程噪声 (rad/s)</translation>
    </message>
    <message>
      <source>RatePitch</source>
      <translation>速率音调</translation>
    </message>
    <message>
      <source>RateRollFF</source>
      <translation>速率滚动FF</translation>
    </message>
    <message>
      <source>RateYawI</source>
      <translation>偏航率</translation>
    </message>
    <message>
      <source>Ratio between rpm error devided by 1000 to how much normalized output gets added to correct for it. motor_command = throttle_curve + CA_HELI_RPM_P * (rpm_setpoint - rpm_measurement) / 1000</source>
      <translation>由 1000 除以的转速误差与添加多少标准化输出以对其进行校正之间的比率。电机命令 = 油门曲线 + CA_HELI_RPM_P * (rpm_设定点 - rpm_测量值) / 1000</translation>
    </message>
    <message>
      <source>Ratio of mass to drag coefficient measured along the Y body axis. This parameter enables estimation of wind drift for vehicles with bluff bodies and without propulsion forces in the X and Y direction (eg multicopters). The drag produced by this effect scales with speed squared.  Set to a postive value &gt; 1.0 to enable. A starting value is the mass in Kg divided by the side area. The predicted drag from the rotors is specified separately by the EK3_DRAG_MCOEF parameter.</source>
      <translation>沿 Y 体轴测量的质量与阻力系数之比。该参数能够估计具有钝体且在 X 和 Y 方向上没有推进力的车辆（例如多旋翼飞行器）的风漂移。这种效应产生的阻力与速度的平方成正比。  设置为正值 &gt; 1.0 即可启用。起始值是质量（公斤）除以侧面积。转子的预测阻力由 EK3_DRAG_MCOEF 参数单独指定。</translation>
    </message>
    <message>
      <source>Raw FIFO high-rate IMU (Gyro)</source>
      <translation>原始 FIFO 高速率 IMU（陀螺仪）</translation>
    </message>
    <message>
      <source>Raw sensor stream rate to ground station</source>
      <translation>到地面站的原始传感器流速率</translation>
    </message>
    <message>
      <source>Read data from file and stop on EOF</source>
      <translation>从文件中读取数据并停止在 EOF 上</translation>
    </message>
    <message>
      <source>RecordOrigin</source>
      <translation>记录来源</translation>
    </message>
    <message>
      <source>Reduce to make the hover thrust estimate more stable, increase if the real hover thrust is expected to change quickly over time.</source>
      <translation>减少以使悬停推力估计更加稳定，如果预计实际悬停推力随着时间的推移而快速变化，则增加。</translation>
    </message>
    <message>
      <source>Reference value associated with the specified frequency to facilitate frequency scaling of the Harmonic Notch Filter</source>
      <translation>与指定频率关联的参考值，以方便谐波陷波滤波器的频率缩放</translation>
    </message>
    <message>
      <source>Relay</source>
      <translation>继电器</translation>
    </message>
    <message>
      <source>Relay2 On/Off</source>
      <translation>继电器2 开/关</translation>
    </message>
    <message>
      <source>Release gripper on thrust loss</source>
      <translation>推力损失时释放夹具</translation>
    </message>
    <message>
      <source>Remove the TRIM_PITCH_CD on the OSD horizon</source>
      <translation>移除 OSD 地平线上的 TRIM_PITCH_CD</translation>
    </message>
    <message>
      <source>ReportOnly</source>
      <translation>仅报告</translation>
    </message>
    <message>
      <source>Require a takeoff</source>
      <translation>需要起飞</translation>
    </message>
    <message>
      <source>Required EPV to use GPS</source>
      <translation>使用 GPS 需要 EPV</translation>
    </message>
    <message>
      <source>Required esc firmware version</source>
      <translation>所需电调固件版本</translation>
    </message>
    <message>
      <source>Required throttle (at sea level, standard atmosphere) for level flight at FW_AIRSPD_TRIM</source>
      <translation>FW_AIRSPD_TRIM 水平飞行所需的油门（在海平面、标准大气压下）</translation>
    </message>
    <message>
      <source>Rescale to estimate</source>
      <translation>重新调整估计值</translation>
    </message>
    <message>
      <source>Reset position on startup</source>
      <translation>启动时重置位置</translation>
    </message>
    <message>
      <source>Resume if AUTO else Loiter</source>
      <translation>如果 AUTO 则恢复，否则盘旋</translation>
    </message>
    <message>
      <source>Retracted</source>
      <translation>撤回</translation>
    </message>
    <message>
      <source>Return mode destination and flight path (home location, rally point, mission landing pattern, reverse mission)</source>
      <translation>返回模式目的地和飞行路径（返航地点、集结点、任务着陆模式、反向任务）</translation>
    </message>
    <message>
      <source>Return to neutral angles on RC failsafe</source>
      <translation>在 RC 故障保护上返回中性角</translation>
    </message>
    <message>
      <source>Reverse Output Range for PCA9685 Output</source>
      <translation>PCA9685 输出的反向输出范围</translation>
    </message>
    <message>
      <source>Reverse Output Range for UAVCANv1</source>
      <translation>UAVCANv1 的反向输出范围</translation>
    </message>
    <message>
      <source>Reverse sense of elevator in FBWB and CRUISE modes. When set to 0 up elevator (pulling back on the stick) means to raise altitude. When set to 1, up elevator means to lower altitude.</source>
      <translation>FBWB 和 CRUISE 模式下电梯反向感应。当设置为 0 向上升降舵（拉回操纵杆）时意味着升高高度。当设置为1时，上行电梯意味着降低高度。</translation>
    </message>
    <message>
      <source>RichenPower Generator sim enable/disable</source>
      <translation>RichenPower Generator sim 启用/禁用</translation>
    </message>
    <message>
      <source>Right Spoiler</source>
      <translation>右扰流板</translation>
    </message>
    <message>
      <source>Rigid board mounting offset (roll, pitch, yaw in radians) applied as a rotation to the simulated accelerometers, gyros and compasses, as a tilted autopilot mounting would be. Intended to be cancelled by AHRS_TRIM. (Y-axis)</source>
      <translation>刚性板安装偏移（以弧度为单位的横滚、俯仰、偏航）作为模拟加速度计、陀螺仪和罗盘的旋转应用，就像倾斜的自动驾驶仪安装一样。打算由 AHRS_TRIM 取消。 （Y 轴）</translation>
    </message>
    <message>
      <source>Roboclaw Driver Channel 1 Output Function</source>
      <translation>Roboclaw 驱动器通道 1 输出功能</translation>
    </message>
    <message>
      <source>Robotis servo position min</source>
      <translation>Robotis 舵机位置最小值</translation>
    </message>
    <message>
      <source>Roll 180°</source>
      <translation>横滚180°</translation>
    </message>
    <message>
      <source>Roll 180°, Yaw 90°</source>
      <translation>横滚 180°、偏航 90°</translation>
    </message>
    <message>
      <source>Roll 90°</source>
      <translation>横滚90°</translation>
    </message>
    <message>
      <source>Roll 90°, Yaw 270°</source>
      <translation>横滚 90°、偏航 270°</translation>
    </message>
    <message>
      <source>Roll Target notch filter index</source>
      <translation>横滚目标陷波滤波器指数</translation>
    </message>
    <message>
      <source>Roll angle in GPS failure loiter mode.</source>
      <translation>GPS 故障盘旋模式下的侧倾角。</translation>
    </message>
    <message>
      <source>Roll axis rate controller I gain maximum</source>
      <translation>横滚轴速率控制器 I 增益最大值</translation>
    </message>
    <message>
      <source>Roll axis rate controller P gain. Corrects in proportion to the difference between the desired roll rate vs actual roll rate</source>
      <translation>横滚轴速率控制器 P 增益。根据所需侧倾速率与实际侧倾速率之间的差异按比例进行校正</translation>
    </message>
    <message>
      <source>Roll channel</source>
      <translation>横滚通道</translation>
    </message>
    <message>
      <source>Roll differential thrust factor in forward flight</source>
      <translation>向前飞行时的横滚推力差因数</translation>
    </message>
    <message>
      <source>Roll max acceleration</source>
      <translation>横滚最大加速度</translation>
    </message>
    <message>
      <source>Roll rate derivative gain</source>
      <translation>横滚率微分增益</translation>
    </message>
    <message>
      <source>Roll rate proportional gain</source>
      <translation>横滚率比例增益</translation>
    </message>
    <message>
      <source>Roll-Axis</source>
      <translation>横滚轴</translation>
    </message>
    <message>
      <source>Roll180Yaw225</source>
      <translation>横滚180偏航225</translation>
    </message>
    <message>
      <source>Roll270Pitch90</source>
      <translation>滚动270俯仰90</translation>
    </message>
    <message>
      <source>Roll90Pitch180</source>
      <translation>滚动90俯仰180</translation>
    </message>
    <message>
      <source>Roll90Yaw90</source>
      <translation>横滚90偏航90</translation>
    </message>
    <message>
      <source>Rotation left</source>
      <translation>向左旋转</translation>
    </message>
    <message>
      <source>Rotocraft</source>
      <translation>旋翼机</translation>
    </message>
    <message>
      <source>Rotor 5 tilt assignment</source>
      <translation>转子 5 倾斜分配</translation>
    </message>
    <message>
      <source>Rotor Speed Control Mode</source>
      <translation>转子调速方式</translation>
    </message>
    <message>
      <source>Rover Failsafe Options</source>
      <translation>流动站故障保护选项</translation>
    </message>
    <message>
      <source>Rover Quicktune auto filter enable</source>
      <translation>Rover Quicktune 自动过滤器启用</translation>
    </message>
    <message>
      <source>Rudder mixing in direct flight modes only (Manual / Stabilize / Acro)</source>
      <translation>仅在直飞模式下进行方向舵混合（手动/稳定/特技）</translation>
    </message>
    <message>
      <source>RunCam Hybrid/RunCam Thumb Pro</source>
      <translation>RunCam Hybrid/RunCam Thumb Pro</translation>
    </message>
    <message>
      <source>RunCam device type</source>
      <translation>RunCam 设备类型</translation>
    </message>
    <message>
      <source>Runs the simulation at multiples of normal speed. Do not use if realtime physics, like RealFlight, is being used</source>
      <translation>以正常速度的倍数运行模拟。如果正在使用实时物理（如 RealFlight），请勿使用</translation>
    </message>
    <message>
      <source>Rx-Only</source>
      <translation>仅接收</translation>
    </message>
    <message>
      <source>SBAS Mode</source>
      <translation>SBAS模式</translation>
    </message>
    <message>
      <source>SBUS default output rate</source>
      <translation>SBUS默认输出速率</translation>
    </message>
    <message>
      <source>SCurves used for navigation</source>
      <translation>用于导航的 S 曲线</translation>
    </message>
    <message>
      <source>SERIAL4</source>
      <translation>SERIAL4</translation>
    </message>
    <message>
      <source>SERVO11</source>
      <translation>SERVO11</translation>
    </message>
    <message>
      <source>SERVO8</source>
      <translation>SERVO8</translation>
    </message>
    <message>
      <source>SF/LW30/d</source>
      <translation>SF/LW30/d</translation>
    </message>
    <message>
      <source>SHT3x</source>
      <translation>SHT3x</translation>
    </message>
    <message>
      <source>SIM Channel 1 Output Function</source>
      <translation>SIM 通道 1 输出功能</translation>
    </message>
    <message>
      <source>SIM Channel 13 Output Function</source>
      <translation>SIM 通道 13 输出功能</translation>
    </message>
    <message>
      <source>SIM Channel 2 Output Function</source>
      <translation>SIM 通道 2 输出功能</translation>
    </message>
    <message>
      <source>SIM Channel 6 Output Function</source>
      <translation>SIM 通道 6 输出功能</translation>
    </message>
    <message>
      <source>SIM-on_hardware Relay Enable Mask</source>
      <translation>SIM-on_hardware 继电器使能掩码</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 2</source>
      <translation>SIM_GZ ESC 2</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 3 Failsafe Value</source>
      <translation>SIM_GZ ESC 3 故障安全值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 4 Minimum Value</source>
      <translation>SIM_GZ ESC 4 最小值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 6</source>
      <translation>SIM_GZ ESC 6</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 7 Failsafe Value</source>
      <translation>SIM_GZ ESC 7 故障安全值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 8 Minimum Value</source>
      <translation>SIM_GZ ESC 8 最小值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 2</source>
      <translation>SIM_GZ 舵机 2</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 3 Failsafe Value</source>
      <translation>SIM_GZ 舵机 3 故障安全值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 4 Minimum Value</source>
      <translation>SIM_GZ 舵机 4 最小值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 6</source>
      <translation>SIM_GZ 舵机 6</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 7 Failsafe Value</source>
      <translation>SIM_GZ 舵机 7 故障安全值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 8 Minimum Value</source>
      <translation>SIM_GZ 舵机 8 最小值</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 2</source>
      <translation>SIM_GZ 轮子 2</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 3 Failsafe Value</source>
      <translation>SIM_GZ 车轮 3 故障保护值</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 4 Minimum Value</source>
      <translation>SIM_GZ 车轮 4 最小值</translation>
    </message>
    <message>
      <source>SITL RC channel count</source>
      <translation>SITL RC 通道数</translation>
    </message>
    <message>
      <source>SITL vicon messages sent</source>
      <translation>SITL vicon 消息已发送</translation>
    </message>
    <message>
      <source>SITL vicon position on vehicle in Right direction</source>
      <translation>SITL vicon 在车辆上的正确方向位置</translation>
    </message>
    <message>
      <source>SITL vicon yaw added to reported yaw sent to vehicle</source>
      <translation>SITL vicon 偏航添加到发送至车辆的偏航报告中</translation>
    </message>
    <message>
      <source>SLCAN Timeout</source>
      <translation>SLCAN 超时</translation>
    </message>
    <message>
      <source>SMBus-Maxell</source>
      <translation>SMBus-麦克赛尔</translation>
    </message>
    <message>
      <source>SRE_180</source>
      <translation>SRE_180</translation>
    </message>
    <message>
      <source>STATS_X</source>
      <translation>STATS_X</translation>
    </message>
    <message>
      <source>SUMD</source>
      <translation>SUMD</translation>
    </message>
    <message>
      <source>Sagetech</source>
      <translation>赛捷科技</translation>
    </message>
    <message>
      <source>Sail Heel Error filter frequency in Hz</source>
      <translation>帆跟误差滤波器频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Sail Heel control I gain</source>
      <translation>我获得了帆跟控制</translation>
    </message>
    <message>
      <source>Sail Heel control feed forward</source>
      <translation>帆跟控制前馈</translation>
    </message>
    <message>
      <source>Sailboat Heel</source>
      <translation>帆船跟</translation>
    </message>
    <message>
      <source>Sailing vehicle max cross track error</source>
      <translation>帆船最大横向误差</translation>
    </message>
    <message>
      <source>Sample post-filtering</source>
      <translation>样本后过滤</translation>
    </message>
    <message>
      <source>Save Trim</source>
      <translation>保存微调</translation>
    </message>
    <message>
      <source>Scalar for how long past the expected takeoff time a takeoff should be considered as failed and the vehicle will switch to QLAND. If set to 0 there is no limit on takeoff time.</source>
      <translation>超过预期起飞时间多长时间后应将起飞视为失败并且飞行器将切换到 QLAND 的标量。如果设置为 0，则起飞时间没有限制。</translation>
    </message>
    <message>
      <source>Scale factor applied to the desired roll actuator command in full manual mode. This parameter allows to adjust the throws of the control surfaces.</source>
      <translation>在全手动模式下应用于所需横滚致动器命令的比例因子。该参数允许调整控制面的投掷。</translation>
    </message>
    <message>
      <source>Scale from PID output to control surface, for use where a single axis is actuated by both motors and Tilt/control surface on a copter style tailsitter, increase to favor control surfaces and reduce motor output by reducing gains</source>
      <translation>从 PID 输出到控制表面的缩放，适用于单轴由电机和直升机式后座上的倾斜/控制表面驱动的情况，增加以有利于控制表面并通过减少增益来减少电机输出</translation>
    </message>
    <message>
      <source>Scaler for yaw mixing</source>
      <translation>用于偏航混合的定标器</translation>
    </message>
    <message>
      <source>Scaling factor for first compass to compensate for sensor scaling errors. If this is 0 then no scaling is done</source>
      <translation>第一个罗盘的比例因子，用于补偿传感器比例误差。如果这是 0 则不进行缩放</translation>
    </message>
    <message>
      <source>Scheduling options</source>
      <translation>日程安排选项</translation>
    </message>
    <message>
      <source>Script 11</source>
      <translation>脚本11</translation>
    </message>
    <message>
      <source>Script 4</source>
      <translation>脚本4</translation>
    </message>
    <message>
      <source>Script11</source>
      <translation>脚本11</translation>
    </message>
    <message>
      <source>Script4</source>
      <translation>脚本4</translation>
    </message>
    <message>
      <source>Scripting Heap Size</source>
      <translation>脚本堆大小</translation>
    </message>
    <message>
      <source>Scripting User Parameter4</source>
      <translation>编写用户参数4脚本</translation>
    </message>
    <message>
      <source>Scripting10</source>
      <translation>脚本10</translation>
    </message>
    <message>
      <source>Scripting3</source>
      <translation>脚本3</translation>
    </message>
    <message>
      <source>Scroll rate in milliseconds for OSD messages longer than available character width. This is lower-bounded by the nominal loop rate of this module.</source>
      <translation>长于可用字符宽度的 OSD 消息的滚动速率（以毫秒为单位）。这是该模块标称循环速率的下限。</translation>
    </message>
    <message>
      <source>Second Encoder Input Pin A</source>
      <translation>第二个编码器输入引脚 A</translation>
    </message>
    <message>
      <source>Second Gyro bias on Z axis</source>
      <translation>Z 轴上的第二个陀螺仪偏置</translation>
    </message>
    <message>
      <source>Second Relay</source>
      <translation>第二次接力</translation>
    </message>
    <message>
      <source>Second button Pin</source>
      <translation>第二个按钮引脚</translation>
    </message>
    <message>
      <source>SecondCAN</source>
      <translation>第二CAN</translation>
    </message>
    <message>
      <source>Seconds (decimal) that roll has to exceed FD_FAIL_R before being considered as a failure.</source>
      <translation>横滚必须超过 FD_FAIL_R 的秒数（十进制）才会被视为失败。</translation>
    </message>
    <message>
      <source>Select a baudrate for the F9P's UART2 port. In GPS_UBX_MODE 1, 2, and 3, the F9P's UART2 port is configured to send/receive RTCM corrections. Set this to 57600 if you want to attach a telemetry radio on UART2.</source>
      <translation>选择 F9P 的 UART2 端口的波特率。在 GPS_UBX_MODE、1、2 和 3 中，F9P 的 UART2 端口配置为发送/接收 RTCM 校正。如果您想在 UART2 上连接遥测无线电，请将其设置为 57600。</translation>
    </message>
    <message>
      <source>Select the source of gnss aiding (GNSS/INS).</source>
      <translation>选择 gnss 辅助源（GNSS/INS）。</translation>
    </message>
    <message>
      <source>Select what should be output on MAIN 3.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 MAIN 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on MAIN 7.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 MAIN 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 11.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 11 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 15.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 15 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 4.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 8.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Aux 3.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PWM Aux 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Aux 7.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PWM Aux 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Capture 3.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 PWM 捕获 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 10.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 10 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 14.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 14 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 3.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 7.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ ESC 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ ESC 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ ESC 6.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ ESC 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Servo 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 舵机 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Servo 6.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 舵机 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Wheels 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 轮子 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on TAP ESC Output ESC 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 TAP ESC 输出 ESC 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on TAP ESC Output ESC 6.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 TAP ESC 输出 ESC 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN ESC 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN ESC 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN ESC 6.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN ESC 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN Servo 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN 舵机 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN Servo 6.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN 舵机 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 10.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 10 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 14.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 14 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 3.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 7.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL ESC Output ESC 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 VOXL ESC 输出 ESC 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL2 IO Output PWM Channel 2.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 VOXL2 IO 输出 PWM 通道 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL2 IO Output PWM Channel 6.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 VOXL2 IO 输出 PWM 通道 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 1.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 1 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 13.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 13 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 3.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 7.
The default failsafe value is set according to the selected function:
- 'Min' for ConstantMin
- 'Max' for ConstantMax
- 'Max' for Parachute
- ('Max'+'Min')/2 for Servos
- 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select which Output Protocol to use for outputs MAIN 3-4.
Custom PWM rates can be used by directly setting any value &gt;0.</source>
      <translation>选择用于输出 MAIN 3-4 的输出协议。可以通过直接设置&gt;0的任何值来使用自定义PWM速率。</translation>
    </message>
    <message>
      <source>Select which Output Protocol to use for outputs PWM Aux 7-8.
Custom PWM rates can be used by directly setting any value &gt;0.</source>
      <translation>选择用于输出 PWM Aux 7-8 的输出协议。可以通过直接设置&gt;0的任何值来使用自定义PWM速率。</translation>
    </message>
    <message>
      <source>Select which function if performed when CH8 is above 1800 pwm</source>
      <translation>选择当 CH8 高于 1800 pwm 时执行的功能</translation>
    </message>
    <message>
      <source>Select which position source should be used. Selecting GPS instead of global position makes sure that there is
no dependence on the position estimator
0 = global position, 1 = GPS</source>
      <translation>选择应使用哪个位置源。选择 GPS 而不是全局位置可确保不依赖于位置估计器 0 = 全局位置，1 = GPS</translation>
    </message>
    <message>
      <source>Selects parameter (normally a PID gain) that is being tuned with an RC transmitter's knob. The RC input channel used is assigned by setting RCx_OPTION to 219.</source>
      <translation>选择使用 RC 发射器旋钮调谐的参数（通常是 PID 增益）。通过将 RCx_OPTION 设置为 219 来分配所使用的 RC 输入通道。</translation>
    </message>
    <message>
      <source>Selects the algorithm used for logfile encryption</source>
      <translation>选择用于日志文件加密的算法</translation>
    </message>
    <message>
      <source>Send Explicit Arm Command</source>
      <translation>发送显式解锁命令</translation>
    </message>
    <message>
      <source>SendPosAsNamedValueFloat</source>
      <translation>SendPosAsNamedValueFloat</translation>
    </message>
    <message>
      <source>Sensor Bus</source>
      <translation>传感器总线</translation>
    </message>
    <message>
      <source>Sensor Source Identification is used to replace a specific instance of a system component's temperature report with the temp sensor's. Examples: TEMP_SRC = 1 (ESC), TEMP_SRC_ID = 1 will set the temp of ESC1. TEMP_SRC = 3 (BatteryIndex),TEMP_SRC_ID = 2 will set the temp of BATT2. TEMP_SRC = 4 (BatteryId/SerialNum),TEMP_SRC_ID=42 will set the temp of all batteries that have param BATTn_SERIAL = 42.</source>
      <translation>传感器源识别用于将系统组件的温度报告的特定实例替换为温度传感器的温度报告。示例：TEMP_SRC = 1 (ESC)、TEMP_SRC_ID = 1 将设置 ESC1 的温度。 TEMP_SRC = 3 (BatteryIndex),TEMP_SRC_ID = 2 将设置 BATT2 的温度。 TEMP_SRC = 4 (BatteryId/SerialNum),TEMP_SRC_ID=42 将设置参数 BATTn_SERIAL = 42 的所有电池的温度。</translation>
    </message>
    <message>
      <source>Sensor yaw alignment</source>
      <translation>传感器偏航对准</translation>
    </message>
    <message>
      <source>Sensors hub mag mode</source>
      <translation>传感器集线器磁力模式</translation>
    </message>
    <message>
      <source>Serial 3 flow control</source>
      <translation>串行3流量控制</translation>
    </message>
    <message>
      <source>Serial 7 flow control</source>
      <translation>串行7流量控制</translation>
    </message>
    <message>
      <source>Serial Configuration for DSM RC Input Driver</source>
      <translation>DSM RC 输入驱动器的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for InertialLabs</source>
      <translation>InertialLabs 的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for MAVLink (instance 2)</source>
      <translation>MAVLink 的串行配置（实例 2）</translation>
    </message>
    <message>
      <source>Serial Configuration for Secondary GPS</source>
      <translation>辅助 GPS 的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for uLanding Radar</source>
      <translation>uLanding 雷达的串行配置</translation>
    </message>
    <message>
      <source>Serial passthru timeout</source>
      <translation>串行直通超时</translation>
    </message>
    <message>
      <source>Serial-MS</source>
      <translation>串口-MS</translation>
    </message>
    <message>
      <source>Serial4 options</source>
      <translation>串行4选项</translation>
    </message>
    <message>
      <source>Serial7 options</source>
      <translation>串行7选项</translation>
    </message>
    <message>
      <source>Server IP 2</source>
      <translation>服务器 IP 2</translation>
    </message>
    <message>
      <source>Servo 1</source>
      <translation>舵机1</translation>
    </message>
    <message>
      <source>Servo 14</source>
      <translation>舵机14</translation>
    </message>
    <message>
      <source>Servo 2 Angle at Minimum</source>
      <translation>舵机 2 最小角度</translation>
    </message>
    <message>
      <source>Servo 26</source>
      <translation>舵机26</translation>
    </message>
    <message>
      <source>Servo 30</source>
      <translation>舵机30</translation>
    </message>
    <message>
      <source>Servo 5 Angle at Maximum</source>
      <translation>舵机 5 最大角度</translation>
    </message>
    <message>
      <source>Servo 7 Angle at Maximum</source>
      <translation>舵机 7 最大角度</translation>
    </message>
    <message>
      <source>Servo DShot ESC type</source>
      <translation>舵机DShot ESC型</translation>
    </message>
    <message>
      <source>Servo channel mask specifying FETtec ESC output.</source>
      <translation>指定 FETtec ESC 输出的舵机通道掩码。</translation>
    </message>
    <message>
      <source>Servo mount pitch angle output leads the vehicle angle by this amount of time based on current pitch rate. Increase until the servo is responsive but does not overshoot</source>
      <translation>根据当前俯仰角速度，舵机安装俯仰角输出领先车辆角度一段时间。增加直到舵机有响应但不超调</translation>
    </message>
    <message>
      <source>Servo with ExtGyro</source>
      <translation>带 ExtGyro 的舵机</translation>
    </message>
    <message>
      <source>Set CAN driver</source>
      <translation>设置CAN驱动</translation>
    </message>
    <message>
      <source>Set as Proximity sensor</source>
      <translation>设置为接近传感器</translation>
    </message>
    <message>
      <source>Set bits in the following positions to set which GPS accuracy metrics will be used to calculate the blending weight. Set to zero to disable and always used first GPS instance.
0 : Set to true to use speed accuracy
1 : Set to true to use horizontal position accuracy
2 : Set to true to use vertical position accuracy</source>
      <translation>在以下位置设置位以设置将使用哪些 GPS 精度度量来计算混合权重。设置为零以禁用并始终使用第一个 GPS 实例。 0：设置为 true 以使用速度精度 1：设置为 true 以使用水平位置精度 2：设置为 true 以使用垂直位置精度</translation>
    </message>
    <message>
      <source>Set level at which RC_LQ item will flash (%)</source>
      <translation>设置 RC_LQ 项目闪烁的级别 (%)</translation>
    </message>
    <message>
      <source>Set offboard loss failsafe mode</source>
      <translation>设置板外丢失故障安全模式</translation>
    </message>
    <message>
      <source>Set the camera type that is being used, certain cameras have custom functions that need further configuration, this enables that.</source>
      <translation>设置正在使用的相机类型，某些相机具有需要进一步配置的自定义功能，这可以实现这一点。</translation>
    </message>
    <message>
      <source>Set this switch to simulate losing telemetry from the other vehicle</source>
      <translation>设置此开关以模拟丢失其他车辆的遥测数据</translation>
    </message>
    <message>
      <source>Set to -1 to disable.
The minimum threshold for the speed measurement not to be interpreted as zero.</source>
      <translation>设置为 -1 以禁用。速度测量的最小阈值不应被解释为零。</translation>
    </message>
    <message>
      <source>Set to 0 to disable, 1 for maximum brightness</source>
      <translation>设置为 0 禁用，1 最大亮度</translation>
    </message>
    <message>
      <source>Set to decouple tilt from vertical acceleration.
This provides smoother flight but slightly worse tracking in position and auto modes.
Unset if accurate position tracking during dynamic maneuvers is more important than a smooth flight.</source>
      <translation>设置为将倾斜与垂直加速度分离。这提供了更平稳的飞行，但位置和自动模式的跟踪稍差。如果动态机动过程中准确的位置跟踪比平稳飞行更重要，请取消设置。</translation>
    </message>
    <message>
      <source>Set transmitter maximum transmit power (from 1 to 8)</source>
      <translation>设置发射机最大发射功率（从1到8）</translation>
    </message>
    <message>
      <source>Set_RatePitchDP</source>
      <translation>Set_RatePitchDP</translation>
    </message>
    <message>
      <source>Sets GPS lataral offset encoding</source>
      <translation>设置 GPS 横向偏移编码</translation>
    </message>
    <message>
      <source>Sets first 4 characters of a total of 8. Valid characters are A-Z, 0-9, " ". Example "PX4 " -&gt; 1347957792
For CALLSIGN shorter than 8 characters use the null terminator at the end '\0'.</source>
      <translation>设置总共 8 中的前 4 字符。有效字符为 A-Z、0-9、“”。示例 "PX4 " -&gt; 1347957792 对于短于 8 字符的 CALLSIGN，请在末尾使用空终止符 '\0'。</translation>
    </message>
    <message>
      <source>Sets the analog input pin that should be used for Current monitoring on AD7091R5.</source>
      <translation>设置用于 AD7091R5 电流监控的模拟输入引脚。</translation>
    </message>
    <message>
      <source>Sets the dual mode of the heli, either as tandem, transverse, or intermeshing/coaxial.</source>
      <translation>设置直升机的双模式，可以是串联、横向或啮合/同轴。</translation>
    </message>
    <message>
      <source>Sets the maximum number of times the EKF3 will be allowed to reset its yaw to the estimate from the EKF-GSF yaw estimator. No resets will be allowed unless the use of the EKF-GSF yaw estimate is enabled via the EK3_GSF_USE_MASK parameter.</source>
      <translation>设置允许 EKF3 将其偏航重置为 EKF-GSF 偏航估计器估计值的最大次数。除非通过 EK3_GSF_USE_MASK 参数启用 EKF-GSF 偏航估计，否则不允许重置。</translation>
    </message>
    <message>
      <source>Sets the number of standard deviations used by the innovation consistency test.</source>
      <translation>设置创新一致性测试使用的标准差数量。</translation>
    </message>
    <message>
      <source>Sets the threshold when the battery will be reported as critically low. This has to be lower than the low threshold. This threshold commonly will trigger RTL.</source>
      <translation>设置电池电量严重不足时的阈值。这必须低于低阈值。该阈值通常会触发 RTL。</translation>
    </message>
    <message>
      <source>Sets the throttle output (HeliRSC servo) in percent for the throttle curve at the minimum collective pitch position. The 0 percent collective is defined by H_COL_MIN. Example: if the setup has -2 degree to +10 degree collective pitch setup, this setting would correspond to -2 degree of pitch.</source>
      <translation>设置最小总距位置处油门曲线的油门输出（HeliRSC 舵机）百分比。 0 百分比总距由 H_COL_MIN 定义。示例：如果设置有 -2 度到 +10 度总距设置，则此设置将对应于 -2 桨距。</translation>
    </message>
    <message>
      <source>Sets the value of deadzone applied to negative baro innovations. Deadzone is enabled when EKF2_GND_EFF_DZ &gt; 0.</source>
      <translation>设置应用于负气压创新的死区值。当 EKF2_GND_EFF_DZ &gt; 0 时死区启用。</translation>
    </message>
    <message>
      <source>Setting an RC channel's _OPTION to this value will use it for trick selection</source>
      <translation>将 RC 通道的 _OPTION 设置为此值将使用它来选择技巧</translation>
    </message>
    <message>
      <source>Setting this parameter to 159753 will enable arming in fixed-wing
mode for VTOLs.
WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK</source>
      <translation>将此参数设置为 159753 将为 VTOL 启用固定翼模式下的解锁。 WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK</translation>
    </message>
    <message>
      <source>Setting this parameter to 894281 will disable the power valid
checks in the commander.
WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK</source>
      <translation>将此参数设置为 894281 将禁用命令器中的电源有效检查。 WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK</translation>
    </message>
    <message>
      <source>Setting this to Enabled(1) will enable the compass. Setting this to Disabled(0) will disable the compass</source>
      <translation>将其设置为启用（1）将启用罗盘。将其设置为“禁用”(0) 将禁用罗盘</translation>
    </message>
    <message>
      <source>Setup and expected use of the hardware. - Default: Use two receivers as completely separate instances. - Moving base: Use two receivers in a rover &amp; moving base setup for heading.</source>
      <translation>硬件的设置和预期使用。 - 默认：使用两个接收器作为完全独立的实例。 - 移动底座：在流动站和移动底座设置中使用两个接收器进行航向。</translation>
    </message>
    <message>
      <source>Ship landing Enable</source>
      <translation>船舶登陆启用</translation>
    </message>
    <message>
      <source>Should the relay default to on or off, this only applies to RELAYx_FUNC "Relay" (1). All other uses will pick the appropriate default output state from within the controlling function's parameters. Note that if INVERTED is set then the default is inverted.</source>
      <translation>如果继电器默认为打开或关闭，则这只适用于 RELAYx_FUNC“继电器”(1)。所有其他用途将从控制函数的参数中选择适当的默认输出状态。请注意，如果设置了 INVERTED，则默认值将被反转。</translation>
    </message>
    <message>
      <source>Sidebar vertical extension</source>
      <translation>侧边栏垂直延伸</translation>
    </message>
    <message>
      <source>Sim</source>
      <translation>辛</translation>
    </message>
    <message>
      <source>Simple mode types</source>
      <translation>简单模式类型</translation>
    </message>
    <message>
      <source>Simulated Airspeed sensor noise</source>
      <translation>模拟空速传感器噪声</translation>
    </message>
    <message>
      <source>Simulated Wind direction</source>
      <translation>模拟风向</translation>
    </message>
    <message>
      <source>Simulated battery capacity</source>
      <translation>模拟电池容量</translation>
    </message>
    <message>
      <source>Simulated battery voltage. Constant voltage when SIM_BATT_CAP_AH is 0, otherwise changing this parameter will re-initialize the state of charge of the battery based on this voltage versus the battery's maximum voltage (default is max voltage).</source>
      <translation>模拟电池电压。当 SIM_BATT_CAP_AH 为 0 时为恒定电压，否则更改此参数将根据此电压与电池最大电压（默认为最大电压）重新初始化电池的充电状态。</translation>
    </message>
    <message>
      <source>Simulated failure of MAG3</source>
      <translation>MAG3的模拟故障</translation>
    </message>
    <message>
      <source>Simulates distortion of magnetometer readings caused by motor current (X-axis)</source>
      <translation>模拟电机电流（X 轴）引起的磁力计读数失真</translation>
    </message>
    <message>
      <source>Simulator Battery minimal percentage</source>
      <translation>模拟器电池最小百分比</translation>
    </message>
    <message>
      <source>Sink rate for final landing stage</source>
      <translation>最后着陆阶段的下沉率</translation>
    </message>
    <message>
      <source>Siyi A8</source>
      <translation>四宜A8</translation>
    </message>
    <message>
      <source>Size of PWM increment on camera tilt servo</source>
      <translation>相机俯仰舵机PWM增量大小</translation>
    </message>
    <message>
      <source>Skip IMU consistency checks when ICE motor running</source>
      <translation>ICE电机运行时跳过IMU一致性检查</translation>
    </message>
    <message>
      <source>Skip airspeed offset calibration on startup for 2nd sensor</source>
      <translation>第二个传感器启动时跳过空速偏移校准</translation>
    </message>
    <message>
      <source>SkyPower EFI generator control function</source>
      <translation>SkyPower EFI发电机控制功能</translation>
    </message>
    <message>
      <source>SkyPower EFI start function</source>
      <translation>SkyPower EFI启动功能</translation>
    </message>
    <message>
      <source>SkyPower EFI update rate</source>
      <translation>SkyPower EFI 更新率</translation>
    </message>
    <message>
      <source>Slung Payload Sim enable/disable</source>
      <translation>悬挂有效负载 Sim 启用/禁用</translation>
    </message>
    <message>
      <source>Slung Payload line length</source>
      <translation>悬挂有效载荷线长度</translation>
    </message>
    <message>
      <source>Slung payload debug output, set to 1 to enable debug</source>
      <translation>悬挂有效负载调试输出，设置为 1 以启用调试</translation>
    </message>
    <message>
      <source>SmartRTL options</source>
      <translation>智能RTL选项</translation>
    </message>
    <message>
      <source>Soaring Enable</source>
      <translation>腾飞启用</translation>
    </message>
    <message>
      <source>Sonar conversion scale</source>
      <translation>声纳转换标尺</translation>
    </message>
    <message>
      <source>Sonar settle time</source>
      <translation>声纳稳定时间</translation>
    </message>
    <message>
      <source>Specific setting for airspeed when cruising.</source>
      <translation>巡航时空速的特定设置。</translation>
    </message>
    <message>
      <source>Specifies after how many seconds without sending data the DDS connection is reestablished.
A value less than one disables the TX rate timeout.</source>
      <translation>指定在不发送数据多少秒后重新建立 DDS 连接。小于 1 的值将禁用 TX 速率超时。</translation>
    </message>
    <message>
      <source>Specifies pin level that indicates buzzer should play</source>
      <translation>指定指示蜂鸣器应播放的引脚电平</translation>
    </message>
    <message>
      <source>Specifies the maximum gradient of the terrain below the vehicle when it is using range finder as a height reference</source>
      <translation>指定车辆使用测距仪作为高度参考时下方地形的最大坡度</translation>
    </message>
    <message>
      <source>Specifies vehicle's startup longitude</source>
      <translation>指定车辆的启动经度</translation>
    </message>
    <message>
      <source>Specify modes where manual control loss is ignored and no failsafe is triggered.
External modes requiring stick input will still failsafe.</source>
      <translation>指定忽略手动控制丢失且不触发故障保护的模式。需要摇杆输入的外部模式仍然会失效。</translation>
    </message>
    <message>
      <source>Speed after that the maximum degree of steering will start to derate. Set this speed to a maximum speed that a plane can do controlled turn at maximum angle of steering wheel without rolling to wing. If 0 then no derating is used.</source>
      <translation>此后的速度最大转向度将开始降低。将此速度设置为飞机可以在方向盘最大角度进行受控转弯而不滚动到机翼的最大速度。如果是 0，则不使用降额。</translation>
    </message>
    <message>
      <source>Speed control D gain.  Compensates for short-term change in desired speed vs actual</source>
      <translation>速度控制D增益。  补偿所需速度与实际速度的短期变化</translation>
    </message>
    <message>
      <source>Speed control I gain maximum.  Constraings the maximum motor output (range -1 to +1) that the I term will generate</source>
      <translation>速度控制我获得最大。  限制 I 项将生成的最大电机输出（范围 -1 至 +1）</translation>
    </message>
    <message>
      <source>Speed control Target notch filter index</source>
      <translation>速度控制 目标陷波滤波器指数</translation>
    </message>
    <message>
      <source>Speed control feed forward</source>
      <translation>速度控制前馈</translation>
    </message>
    <message>
      <source>Speed in m/s which the aircraft will attempt to maintain while descending during a WP mission</source>
      <translation>在 WP 任务期间飞机下降时尝试保持的速度（以米/秒为单位）</translation>
    </message>
    <message>
      <source>SpeedMismatchDisable</source>
      <translation>速度不匹配禁用</translation>
    </message>
    <message>
      <source>Spool down time</source>
      <translation>阀芯停机时间</translation>
    </message>
    <message>
      <source>Sprayer spinner servo pin</source>
      <translation>喷涂机旋转舵机销</translation>
    </message>
    <message>
      <source>Stab Yaw kP</source>
      <translation>稳定偏航 kP</translation>
    </message>
    <message>
      <source>Stabilize Mount2's roll angle</source>
      <translation>稳定Mount2的横滚角</translation>
    </message>
    <message>
      <source>Stabilize yaw for absolute/lock mode.</source>
      <translation>稳定绝对/锁定模式的偏航。</translation>
    </message>
    <message>
      <source>Standard VTOL</source>
      <translation>标准VTOL</translation>
    </message>
    <message>
      <source>Start at center rather than on perimeter</source>
      <translation>从中心开始而不是从周边开始</translation>
    </message>
    <message>
      <source>Start wind vane calibration by setting this to 1 or 2</source>
      <translation>将其设置为 1 或 2 开始风向标校准</translation>
    </message>
    <message>
      <source>Static pressure position error coefficient for the negative X axis</source>
      <translation>X轴负方向静压位置误差系数</translation>
    </message>
    <message>
      <source>Steering Time Constant</source>
      <translation>转向时间常数</translation>
    </message>
    <message>
      <source>Steering control Error notch filter index, zero disables</source>
      <translation>转向控制 误差陷波滤波器指数，零禁用</translation>
    </message>
    <message>
      <source>Steering control Target filter frequency in Hz</source>
      <translation>转向控制 目标滤波器频率（Hz）</translation>
    </message>
    <message>
      <source>Steering control angular deceleration maximum (in deg/s/s).  0 to disable deceleration limiting</source>
      <translation>转向控制角减速度最大值（以度/秒/秒为单位）。  0 禁用减速度限制</translation>
    </message>
    <message>
      <source>Steering feed forward</source>
      <translation>转向前馈</translation>
    </message>
    <message>
      <source>Stick input mode</source>
      <translation>摇杆输入模式</translation>
    </message>
    <message>
      <source>Stop logging to current file on disarm</source>
      <translation>上锁时停止记录到当前文件</translation>
    </message>
    <message>
      <source>Storage Priority</source>
      <translation>存储优先级</translation>
    </message>
    <message>
      <source>Stream rate of GLOBAL_POSITION_INT and LOCAL_POSITION_NED to ground station</source>
      <translation>GLOBAL_POSITION_INT和LOCAL_POSITION_NED到地面站的码流速率</translation>
    </message>
    <message>
      <source>Stream rate of SYS_STATUS, MEMINFO, MISSION_CURRENT, GPS_RAW_INT, NAV_CONTROLLER_OUTPUT, and LIMITS_STATUS to ground station</source>
      <translation>SYS_STATUS、MEMINFO、MISSION_CURRENT、GPS_RAW_INT、NAV_CONTROLLER_OUTPUT、LIMITS_STATUS到地面站的码流速率</translation>
    </message>
    <message>
      <source>Stuck: freeze the measurement to the current location Drift: add a linearly growing bias to the sensor data</source>
      <translation>卡住：将测量冻结到当前位置漂移：向传感器数据添加线性增长的偏差</translation>
    </message>
    <message>
      <source>SuperExpo factor for refining the input curve shape tuned using SC_ACRO_EXPO_Y.
0 Pure Expo function
0.7 reasonable shape enhancement for intuitive stick feel
0.95 very strong bent input curve only near maxima have effect</source>
      <translation>用于细化使用 SC_ACRO_EXPO_Y 调整的输入曲线形状的 SuperExpo 因子。 0 Pure Expo功能 0.7 合理的形状增强，带来直观的棒感 0.95 非常强的弯曲输入曲线，仅在最大值附近才有效果</translation>
    </message>
    <message>
      <source>Suppress speed scaling during auto takeoffs to be 1 or less to prevent oscillations without airspeed sensor.</source>
      <translation>将自动起飞期间的速度缩放抑制为 1 或更小，以防止在没有空速传感器的情况下出现振荡。</translation>
    </message>
    <message>
      <source>Surface Tracking Mode</source>
      <translation>表面跟踪模式</translation>
    </message>
    <message>
      <source>Surpress speed scaling during auto takeoffs to be 1 or less to prevent oscillations without airpseed sensor.</source>
      <translation>将自动起飞期间的速度缩放抑制为 1 或更低，以防止在没有 Airpseed 传感器的情况下出现振荡。</translation>
    </message>
    <message>
      <source>Swash 1 H3 Generic Servo 1 Position</source>
      <translation>斜盘 1 H3 通用舵机 1 位置</translation>
    </message>
    <message>
      <source>Swash 2 H3 Generic Servo 1 Position</source>
      <translation>斜盘 2 H3 通用舵机 1 位置</translation>
    </message>
    <message>
      <source>Swash servo position in PWM microseconds corresponding to zero collective pitch (or zero lift for Asymmetrical blades)</source>
      <translation>斜盘舵机位置（以 PWM 微秒为单位）对应于零总距桨距（或非对称叶片的零升力）</translation>
    </message>
    <message>
      <source>SwitchPos1</source>
      <translation>开关Pos1</translation>
    </message>
    <message>
      <source>SysID Target</source>
      <translation>系统ID目标</translation>
    </message>
    <message>
      <source>System identification Chirp Magnitude</source>
      <translation>系统识别线性调频幅度</translation>
    </message>
    <message>
      <source>SystemID</source>
      <translation>系统ID</translation>
    </message>
    <message>
      <source>TAP ESC Output ESC 3 Output Function</source>
      <translation>TAP ESC 输出 ESC 3 输出功能</translation>
    </message>
    <message>
      <source>TAP ESC Output ESC 7 Output Function</source>
      <translation>TAP ESC 输出 ESC 7 输出功能</translation>
    </message>
    <message>
      <source>TCP_MPEG</source>
      <translation>TCP_MPEG</translation>
    </message>
    <message>
      <source>TELEM 3</source>
      <translation>TELEM 3</translation>
    </message>
    <message>
      <source>TER_HGT_Y</source>
      <translation>TER_HGT_Y</translation>
    </message>
    <message>
      <source>TEST_HP</source>
      <translation>TEST_HP</translation>
    </message>
    <message>
      <source>TEST_MIN</source>
      <translation>TEST_MIN</translation>
    </message>
    <message>
      <source>TEST_UAVCAN_DNA</source>
      <translation>TEST_UAVCAN_DNA</translation>
    </message>
    <message>
      <source>THROTTLE_X</source>
      <translation>THROTTLE_X</translation>
    </message>
    <message>
      <source>TOFSENSE-M First Instance</source>
      <translation>TOFSENSE-M一例</translation>
    </message>
    <message>
      <source>TOFSENSE-M serial port config</source>
      <translation>TOFSENSE-M串口配置</translation>
    </message>
    <message>
      <source>TROne</source>
      <translation>特罗尼</translation>
    </message>
    <message>
      <source>TX_PullUp</source>
      <translation>TX_上拉</translation>
    </message>
    <message>
      <source>Tailsiter mask</source>
      <translation>尾部面具</translation>
    </message>
    <message>
      <source>Tailsitter VTOL transition angle</source>
      <translation>尾座 VTOL 过渡角</translation>
    </message>
    <message>
      <source>Tailsitter input type</source>
      <translation>尾座输入类型</translation>
    </message>
    <message>
      <source>Takeoff</source>
      <translation>起飞</translation>
    </message>
    <message>
      <source>Takeoff is not permitted until motors report at least this RPM.  Set to zero to disable check</source>
      <translation>在电机至少报告此 RPM 之前不允许起飞。  设置为零以禁用检查</translation>
    </message>
    <message>
      <source>Takeoff mode distance</source>
      <translation>起飞模式距离</translation>
    </message>
    <message>
      <source>Takeoff tail dragger elevator</source>
      <translation>起飞尾部牵引升降机</translation>
    </message>
    <message>
      <source>Takeoff time failure scalar</source>
      <translation>起飞时间故障标量</translation>
    </message>
    <message>
      <source>Target airspeed</source>
      <translation>目标空速</translation>
    </message>
    <message>
      <source>Target cruise speed in auto modes</source>
      <translation>自动模式下的目标巡航速度</translation>
    </message>
    <message>
      <source>Target vehicle's MAVLink system ID</source>
      <translation>目标车辆的MAVLink系统ID</translation>
    </message>
    <message>
      <source>Telem4</source>
      <translation>电信4</translation>
    </message>
    <message>
      <source>TelemetryRadioRSSI</source>
      <translation>遥测无线电RSSI</translation>
    </message>
    <message>
      <source>Temperature calibration min learned temperature</source>
      <translation>温度校准最小学习温度</translation>
    </message>
    <message>
      <source>Temperature sensor analog 3rd polynomial coefficient</source>
      <translation>温度传感器模拟第三多项式系数</translation>
    </message>
    <message>
      <source>Temperature that the 2nd accelerometer was calibrated at</source>
      <translation>第二个加速度计校准的温度</translation>
    </message>
    <message>
      <source>TeraRangerSerial</source>
      <translation>Tera Ranger系列</translation>
    </message>
    <message>
      <source>Terrain Enable</source>
      <translation>地形启用</translation>
    </message>
    <message>
      <source>Terrain estimation:
bit 0: Abort if terrain is not found
bit 1: Abort if terrain times out (after a first successful measurement)
The last estimate is always used as ground, whether the last valid measurement or the land waypoint, depending on the
selected abort criteria, until an abort condition is entered. If FW_LND_USETER == 0, these bits are ignored.
TODO: Extend automatic abort conditions
e.g. glide slope tracking error (horizontal and vertical)</source>
      <translation>地形估计： 位 0：如果未找到地形则中止 位 1：如果地形超时则中止（第一次成功测量后） 最后的估计始终用作地面，无论是最后一次有效测量还是陆地航路点，具体取决于所选的中止条件，直到输入中止条件。如果 FW_LND_USETER == 0，这些位将被忽略。 TODO：扩展自动中止条件，例如下滑道跟踪误差（水平和垂直）</translation>
    </message>
    <message>
      <source>Terrain random walk noise density, hilly/outdoor (0.1), flat/Indoor (0.001)</source>
      <translation>地形随机行走噪声密度，丘陵/室外 (0.1)，平坦/室内 (0.001)</translation>
    </message>
    <message>
      <source>TestChan4</source>
      <translation>测试Chan4</translation>
    </message>
    <message>
      <source>TestMotor4</source>
      <translation>测试电机4</translation>
    </message>
    <message>
      <source>Tether Simulation MAVLink System ID</source>
      <translation>Tether 模拟 MAVLink 系统 ID</translation>
    </message>
    <message>
      <source>The AP_Logger_File backend uses a buffer to store data before writing to the block device.  Raising this value may reduce "gaps" in your SD card logging.  This buffer size may be reduced depending on available memory.  PixHawk requires at least 4 kilobytes.  Maximum value available here is 64 kilobytes.</source>
      <translation>AP_Logger_File 后端在写入块设备之前使用缓冲区来存储数据。  提高该值可能会减少 SD 卡日志记录中的“间隙”。  该缓冲区大小可能会根据可用内存而减小。  PixHawk 至少需要 4 千字节。  此处可用的最大值为 64 千字节。</translation>
    </message>
    <message>
      <source>The GCS connection loss failsafe will only be entered after a timeout,
set by COM_DL_LOSS_T in seconds. Once the timeout occurs the selected
action will be executed.</source>
      <translation>GCS 连接丢失故障保护仅在超时（由 COM_DL_LOSS_T 以秒为单位设置）后才会进入。一旦发生超时，将执行所选操作。</translation>
    </message>
    <message>
      <source>The PSI range of the device</source>
      <translation>设备的 PSI 系列</translation>
    </message>
    <message>
      <source>The PWM level on the throttle input channel below which throttle failsafe triggers. Note that this should be well below the normal minimum for your throttle channel.</source>
      <translation>油门输入通道上的 PWM 电平，低于该电平时油门故障安全触发。请注意，这应该远低于油门通道的正常最小值。</translation>
    </message>
    <message>
      <source>The accelerometer bias state will be limited to +- this value</source>
      <translation>加速度计偏置状态将被限制为+-该值</translation>
    </message>
    <message>
      <source>The action to take on a short failsafe event. A short failsafe event can be triggered instantly by loss of RC control or by throttle value (see THR_FS_VALUE). If in CIRCLE or RTL mode this parameter is ignored. A short failsafe event in stabilization and manual modes will cause a change to CIRCLE mode if FS_SHORT_ACTN is 0 or 1, a change to FBWA mode with zero throttle if FS_SHORT_ACTN is 2, and a change to FBWB mode if FS_SHORT_ACTN is 4. In all other modes (AUTO, GUIDED and LOITER) a short failsafe event will cause no mode change if FS_SHORT_ACTN is set to 0, will cause a change to CIRCLE mode if set to 1, will change to FBWA mode with zero throttle if set to 2, or will change to FBWB if set to 4. Please see the documentation for FS_LONG_ACTN for the behaviour after FS_LONG_TIMEOUT seconds of failsafe. This parameter only applies to failsafes during fixed wing modes. Quadplane modes will switch to QLAND unless Q_OPTIONS bit 5(QRTL) or 20(RTL) are set.</source>
      <translation>短时故障保护事件的处理动作。遥控信号丢失或油门值异常（见 THR_FS_VALUE）可立即触发短时故障保护。在 CIRCLE 或 RTL 模式下忽略此参数。在自稳和手动模式下，FS_SHORT_ACTN 为 0 或 1 时切换至 CIRCLE，为 2 时切换至零油门的 FBWA，为 4 时切换至 FBWB。在其他模式（AUTO、GUIDED 和 LOITER）下，设为 0 时保持当前模式，为 1 时切换至 CIRCLE，为 2 时切换至零油门的 FBWA，为 4 时切换至 FBWB。故障持续 FS_LONG_TIMEOUT 秒之后的处理方式详见 FS_LONG_ACTN。此参数仅适用于固定翼模式；垂直起降模式默认切换至 QLAND，除非 Q_OPTIONS 第 5 位（QRTL）或第 20 位（RTL）已启用。</translation>
    </message>
    <message>
      <source>The altitude at which we should switch to Q_LAND_SPEED descent rate</source>
      <translation>我们应该切换到 Q_LAND_SPEED 下降率的高度</translation>
    </message>
    <message>
      <source>The angle is measured clockwise (as seen from top), with 0 pointing forwards (X axis).</source>
      <translation>该角度是顺时针测量的（从顶部看），0 指向前方（X 轴）。</translation>
    </message>
    <message>
      <source>The baud rate used for Serial4. The APM2 can support all baudrates up to 115, and also can support 500. The PX4 can support rates of up to 1500. If you setup a rate you cannot support on APM2 and then can't connect to your board you should load a firmware from a different vehicle type. That will reset all your parameters to defaults.</source>
      <translation>Serial4 使用的波特率。 APM2 可以支持高达 115 的所有波特率，也可以支持 500。 PX4 可支持高达 1500 的速率。如果您设置了 APM2 不支持的速率，然后无法连接到您的主板，您应该从不同的车辆类型加载固件。这会将您的所有参数重置为默认值。</translation>
    </message>
    <message>
      <source>The baud rate used on the Telem1 port. Most stm32-based boards can support rates of up to 1500. If you setup a rate you cannot support and then can't connect to your board you should load a firmware from a different vehicle type. That will reset all your parameters to defaults.</source>
      <translation>Telem1 端口上使用的波特率。大多数基于 stm32 的板卡可支持高达 1500 的速率。如果您设置了无法支持的速率，然后无法连接到您的主板，您应该从不同的车辆类型加载固件。这会将您的所有参数重置为默认值。</translation>
    </message>
    <message>
      <source>The bus address of the sensor, where applicable. Used for the I2C and DroneCAN sensors to allow for multiple sensors on different addresses.</source>
      <translation>传感器的总线地址（如果适用）。用于 I2C 和 DroneCAN 传感器，以允许不同地址上的多个传感器。</translation>
    </message>
    <message>
      <source>The capacity of the tank in litres</source>
      <translation>水箱容量（升）</translation>
    </message>
    <message>
      <source>The channel index (starting from 1 for channel 1) indicates
which channel should be used for reading pitch inputs from.
A value of zero indicates the switch is not assigned.</source>
      <translation>通道索引（通道 1 从 1 开始）指示应使用哪个通道来读取俯仰输入。值为零表示未分配开关。</translation>
    </message>
    <message>
      <source>The channel number where RSSI will be output by the radio receiver (5 and above).</source>
      <translation>无线电接收器（5 及以上）将输出 RSSI 的通道号。</translation>
    </message>
    <message>
      <source>The controller will increase the commanded airspeed to maintain this minimum groundspeed to the next waypoint.</source>
      <translation>控制器将增加指令空速，以保持到达下一个航路点的最小地速。</translation>
    </message>
    <message>
      <source>The deadzone above and below mid throttle in PWM microseconds. Used in AltHold, Loiter, PosHold flight modes</source>
      <translation>中油门上方和下方的死区（以 PWM 微秒为单位）。用于 AltHold、Loiter、PosHold 飞行模式</translation>
    </message>
    <message>
      <source>The descent speed for the final stage of landing in m/s</source>
      <translation>着陆最后阶段的下降速度，单位为米/秒</translation>
    </message>
    <message>
      <source>The distance to move sideways in ZigZag mode</source>
      <translation>ZigZag 模式下横向移动的距离</translation>
    </message>
    <message>
      <source>The ekf gyro bias states will be limited to within a range equivalent to +- of this value.</source>
      <translation>ekf 陀螺仪偏置状态将被限制在等于该值的+/-的范围内。</translation>
    </message>
    <message>
      <source>The forward velocity of the aircraft while stalled</source>
      <translation>飞机失速时的前进速度</translation>
    </message>
    <message>
      <source>The geometric ideal acceptance radius is multiplied by this factor to account for kinematic and dynamic effects. Higher value -&gt; The rover starts to cut the corner earlier.</source>
      <translation>几何理想接受半径乘以该系数以考虑运动学和动态效应。值较高 -&gt; 流动站较早开始切角。</translation>
    </message>
    <message>
      <source>The identifier of the ground station in the MAVLink protocol. Don't change this unless you also modify the ground station to match.</source>
      <translation>MAVLink 协议中地面站的标识符。除非您还修改地面站以匹配，否则请勿更改此设置。</translation>
    </message>
    <message>
      <source>The integration speed of the trajectory setpoint is linearly
reduced with the horizontal position tracking error. When the
error is above this parameter, the integration of the
trajectory is stopped to wait for the drone.
This value can be adjusted depending on the tracking
capabilities of the vehicle.</source>
      <translation>轨迹设定点的积分速度随着水平位置跟踪误差线性减小。当误差超过该参数时，停止轨迹积分等待无人机。该值可以根据车辆的跟踪能力进行调整。</translation>
    </message>
    <message>
      <source>The manual control loss failsafe will only be entered after a timeout, set by COM_RC_LOSS_T in seconds.</source>
      <translation>只有在超时（由 COM_RC_LOSS_T 以秒为单位设置）后才会进入手动控制丢失故障保护。</translation>
    </message>
    <message>
      <source>The maximum ascending vertical velocity the pilot may request in cm/s</source>
      <translation>飞行员可能要求的最大上升垂直速度（以厘米/秒为单位）</translation>
    </message>
    <message>
      <source>The maximum percentage speed change in airspeed reports that is allowed due to offset changes between calibraions before a warning is issued. This potential speed error is in percent of ASPD_FBW_MIN. 0 disables. Helps warn of calibrations without pitot being covered.</source>
      <translation>在发出警告之前，由于校准之间的偏移变化而允许的空速报告中的最大速度变化百分比。该潜在速度误差以 ASPD_FBW_MIN 的百分比表示。 0 禁用。有助于在不覆盖皮托管的情况下发出校准警告。</translation>
    </message>
    <message>
      <source>The maximum roll rate at full stick deflection in QACRO mode</source>
      <translation>QACRO 模式下全摇杆偏转时的最大横滚速率</translation>
    </message>
    <message>
      <source>The maximum vertical decent velocity in m/s. If 0 LAND_SPD_MS value is used.</source>
      <translation>最大垂直下降速度（米/秒）。如果使用 0 LAND_SPD_MS 值。</translation>
    </message>
    <message>
      <source>The maximum yaw rate at full stick deflection in ACRO mode. If this is zero then rudder is directly controlled by rudder stick input. This option is only available if you also set YAW_RATE_ENABLE to 1.</source>
      <translation>ACRO 模式下全摇杆偏转时的最大偏航角速度。如果为零，则舵由舵杆输入直接控制。仅当您同时将 YAW_RATE_ENABLE 设置为 1 时，此选项才可用。</translation>
    </message>
    <message>
      <source>The minimal time from arming the motors until moving the vehicle is possible is COM_SPOOLUP_TIME seconds. Goal: - Motors and propellers spool up to idle speed before getting commanded to spin faster - Timeout for ESCs and smart batteries to successfulyy do failure checks e.g. for stuck rotors before the vehicle is off the ground</source>
      <translation>从启动电机到可以移动车辆的最短时间为 COM_SPOOLUP_TIME 秒。目标： - 电机和螺旋桨在收到命令更快旋转之前先达到怠速 - ESC 和智能电池超时以成功执行故障检查，例如用于在车辆离开地面之前卡住的转子</translation>
    </message>
    <message>
      <source>The minimum simulated temperature that the calibration is valid for</source>
      <translation>校准有效的最低模拟温度</translation>
    </message>
    <message>
      <source>The module IDs [0, 31] that should be asked for telemetry. The data received from these IDs will be published via the esc_status topic.</source>
      <translation>遥测应请求的模块 ID [0、31]。从这些 ID 收到的数据将通过 esc_status 主题发布。</translation>
    </message>
    <message>
      <source>The number of mission mission items that has been loaded by the ground station. Do not change this manually.</source>
      <translation>地面站已加载的任务任务物品数量。不要手动更改此设置。</translation>
    </message>
    <message>
      <source>The orientation of a third external compass relative to the vehicle frame. This value will be ignored unless this compass is set as an external compass. When set correctly in the northern hemisphere, pointing the nose and right side down should increase the MagX and MagY values respectively. Rolling the vehicle upside down should decrease the MagZ value. For southern hemisphere, switch increase and decrease. NOTE: For internal compasses, AHRS_ORIENT is used.</source>
      <translation>第三外部罗盘相对于车架的方向。除非将此罗盘设置为外部罗盘，否则该值将被忽略。在北半球正确设置时，将鼻子和右侧朝下应分别增加 MagX 和 MagY 值。将车辆倒置会降低 MagZ 值。对于南半球，切换增加和减少。 NOTE：对于内部罗盘，使用 AHRS_ORIENT。</translation>
    </message>
    <message>
      <source>The orientation of the device (Radians) with respect to the vehicle frame around the y axis.
Requires MS_EXT_MAG_EN to be enabled to be used.</source>
      <translation>设备相对于车架绕 y 轴的方向（弧度）。需要启用 MS_EXT_MAG_EN 才能使用。</translation>
    </message>
    <message>
      <source>The orientation of the device (Radians) with respect to the vehicle frame around the z axis. Requires MS_EXT_MAG_EN to be enabled to be used.</source>
      <translation>设备相对于车架绕 z 轴的方向（弧度）。需要启用 MS_EXT_MAG_EN 才能使用。</translation>
    </message>
    <message>
      <source>The output frequency of the main SBF blocks needed for PVT information.</source>
      <translation>PVT 信息所需的主要 SBF 块的输出频率。</translation>
    </message>
    <message>
      <source>The pin number that the Parachute pyrotechnics are connected to. (start at 1)</source>
      <translation>降落伞烟火所连接的引脚号。 （从1开始）</translation>
    </message>
    <message>
      <source>The pitch rate at which tailsitter aircraft will pitch down in the transition from VTOL to forward flight</source>
      <translation>尾座飞机从 VTOL 过渡到前飞时俯仰的俯仰速率</translation>
    </message>
    <message>
      <source>The range finder will be used as the primary height source when below a specified percentage of the sensor maximum as set by the RNGFND_MAX_CM parameter. Set to -1 to prevent range finder use.</source>
      <translation>当低于 RNGFND_MAX_CM 参数设置的传感器最大值的指定百分比时，测距仪将用作主要高度源。设置为 -1 以防止使用测距仪。</translation>
    </message>
    <message>
      <source>The rate that additional generator telemetry is sent</source>
      <translation>发送附加发电机遥测数据的速率</translation>
    </message>
    <message>
      <source>The sealevel bank angle limit for a continuous loiter. (Used to calculate airframe loading limits at higher altitudes). Setting to 0, will instead just scale the loiter radius directly</source>
      <translation>连续盘旋的海平面倾斜角限制。 （用于计算较高高度下的机身负载限制）。设置为 0，将直接缩放盘旋半径</translation>
    </message>
    <message>
      <source>The stall airspeed (calibrated airspeed) of the vehicle.
It is used for airspeed sensor failure detection and for the control
surface scaling airspeed limits.</source>
      <translation>飞行器的失速空速（校准空速）。它用于空速传感器故障检测和控制面缩放空速限制。</translation>
    </message>
    <message>
      <source>The supported sub-modes are:
Direct velocity:
Sticks directly map to velocity setpoints without smoothing.
Also applies to vertical direction and Altitude mode.
Useful for velocity control tuning.
Acceleration based:
Sticks map to acceleration and there's a virtual brake drag</source>
      <translation>支持的子模式有： 直接速度：摇杆直接映射到速度设定点，无需平滑。也适用于垂直方向和海拔模式。对于速度控制调整很有用。基于加速度：将地图贴在加速度上，并且存在虚拟制动阻力</translation>
    </message>
    <message>
      <source>The throttle failsafe allows you to configure a software failsafe activated by a setting on the throttle input channel to a low value. This can be used to detect the RC transmitter going out of range. Failsafe will be triggered when the throttle channel reaches the FS_THR_VALUE, or goes below it, for FS_TIMEOUT seconds.</source>
      <translation>油门故障保护允许您将油门输入通道上的设置激活的软件故障保护配置为低值。这可用于检测 RC 发射器超出范围。当油门通道达到 FS_THR_VALUE 或低于 FS_TIMEOUT 秒时，将触发故障保护。</translation>
    </message>
    <message>
      <source>The time in seconds that a failsafe condition has to persist before a long failsafe event will occur. This defaults to 5 seconds.</source>
      <translation>在发生长故障安全事件之前，故障安全条件必须持续的时间（以秒为单位）。默认为 5 秒。</translation>
    </message>
    <message>
      <source>The timeout for holding the left stick to the lower left and the right stick to the lower right at the same time until the gesture kills the actuators one-way. A negative value disables the feature.</source>
      <translation>同时按住左摇杆到左下角和右摇杆到右下角直到手势单向终止执行器的超时时间。负值会禁用该功能。</translation>
    </message>
    <message>
      <source>The typical closest angle to the wind the vehicle will sail at. the vehicle will sail at this angle when going upwind</source>
      <translation>车辆航行时与风的典型最接近角度。逆风时车辆将以这个角度航行</translation>
    </message>
    <message>
      <source>The vehicle aborts the current operation and returns to launch when
the time since takeoff is above this value. It is not possible to resume the
mission or switch to any auto mode other than RTL or Land. Taking over in any manual
mode is still possible.
Starting from 90% of the maximum flight time, a warning message will be sent
every 1 minute with the remaining time until automatic RTL.
Set to -1 to disable.</source>
      <translation>当起飞后的时间高于此值时，飞行器将中止当前操作并返回发射。无法恢复任务或切换到 RTL 或陆地以外的任何自动模式。在任何手动模式下接管仍然是可能的。从最大飞行时间90%开始，每1分钟发送一次警告消息，直到自动RTL为止剩余时间。设置为 -1 以禁用。</translation>
    </message>
    <message>
      <source>The vertical acceleration used when pilot is controlling the altitude</source>
      <translation>飞行员控制高度时使用的垂直加速度</translation>
    </message>
    <message>
      <source>Thermal Range Enabled</source>
      <translation>热范围启用</translation>
    </message>
    <message>
      <source>Thermal control off</source>
      <translation>热控制关闭</translation>
    </message>
    <message>
      <source>Third Gyro bias (Z-axis)</source>
      <translation>第三陀螺仪偏置（Z 轴）</translation>
    </message>
    <message>
      <source>Third TOFSENSE-M sensor ID. This cannot be 0. You can change ID of sensor from NAssistant Software</source>
      <translation>第三个 TOFSENSE-M 传感器 ID。这不可能是0。您可以从 NAssistant 软件更改传感器的 ID</translation>
    </message>
    <message>
      <source>Third order term</source>
      <translation>三阶项</translation>
    </message>
    <message>
      <source>This allows a ground control station to automatically find the drone on the local network.</source>
      <translation>这使得地面控制站能够自动在本地网络上找到无人机。</translation>
    </message>
    <message>
      <source>This allows selection of position servos or on/off servos for pitch</source>
      <translation>这允许选择位置舵机系统或俯仰开/关舵机系统</translation>
    </message>
    <message>
      <source>This allows to specify which collective pitch command results in the least amount of rotor drag.
This is used to increase the accuracy of the yaw drag torque compensation based on collective pitch
by aligning the lowest rotor drag with zero compensation.
For symmetric profile blades this is the command that results in exactly 0° collective blade angle.
For lift profile blades this is typically a command resulting in slightly negative collective blade angle.
tail_output += CA_HELI_YAW_CP_S * abs(collective_pitch - CA_HELI_YAW_CP_O)</source>
      <translation>这允许指定哪个总距桨距命令导致最小的转子阻力。这用于通过将最低转子阻力与零补偿对齐来提高基于总距的偏航阻力扭矩补偿的精度。对于对称轮廓叶片，该命令可精确得出 0° 总距叶片角度。对于升力轮廓叶片，这通常是导致稍微负的总距叶片角度的命令。 tail_output += CA_HELI_YAW_CP_S * abs(collective_pitch - CA_HELI_YAW_CP_O)</translation>
    </message>
    <message>
      <source>This channel immediately sets all outputs to their disarmed values, parachutes are NOT deployed.
Unlike termination this can be undone. Quickly flipping the switch back restores control.
System auto-disarms after COM_KILL_DISARM seconds, preflight checks and re-arming are then required.</source>
      <translation>该通道立即将所有输出设置为其解除值，降落伞已部署 NOT。与终止不同，这是可以撤消的。快速翻转开关即可恢复控制。系统在 COM_KILL_DISARM 秒后自动上锁，然后需要进行飞行前检查并重新解锁。</translation>
    </message>
    <message>
      <source>This check detects if the Open Drone ID system is missing.
Depending on the value of the parameter, the check can be
disabled, warn only or deny arming.</source>
      <translation>此检查可检测 Open Drone ID 系统是否丢失。根据参数的值，可以禁用检查、仅警告或拒绝解锁。</translation>
    </message>
    <message>
      <source>This compensates for voltage drop of the battery over time by attempting to normalize performance across the operating range of the battery.</source>
      <translation>通过尝试使电池工作范围内的性能正常化，可以补偿电池随时间的电压下降。</translation>
    </message>
    <message>
      <source>This control disturbance noise controls the growth of estimated error due to gyro measurement errors excluding bias. Increasing it makes the flter trust the gyro measurements less and other measurements more.</source>
      <translation>该控制干扰噪声控制由于陀螺仪测量误差（不包括偏差）引起的估计误差的增长。增加它会使滤波器对陀螺仪测量的信任度降低，而对其他测量的信任度增加。</translation>
    </message>
    <message>
      <source>This controls how much to use the GPS to correct the attitude. This should never be set to zero for a plane as it would result in the plane losing control in turns. For a plane please use the default value of 1.0.</source>
      <translation>该参数控制使用 GPS 校正姿态的程度。对于飞机来说，绝对不应该将其设置为零，因为这会导致飞机依次失去控制。对于平面请使用1.0的默认值。</translation>
    </message>
    <message>
      <source>This controls if waypoint missions use rangefinder for terrain following</source>
      <translation>这控制航路点任务是否使用测距仪进行地形跟踪</translation>
    </message>
    <message>
      <source>This controls the amount of extra pitch given to the vectored control when at high pitch errors</source>
      <translation>这控制在高俯仰误差时给予矢量控制的额外俯仰量</translation>
    </message>
    <message>
      <source>This controls the mavlink type given in HEARTBEAT messages. For some GCS types a particular setting will be needed for correct operation.</source>
      <translation>这控制 HEARTBEAT 消息中给出的 mavlink 类型。对于某些 GCS 类型，需要特定的设置才能正确操作。</translation>
    </message>
    <message>
      <source>This controls the maximum rate at which the throttle output (HeliRSC servo) can change, as a percentage per second. A value of 100 means the throttle can change over its full range in one second. A value of zero gives unlimited slew rate.</source>
      <translation>这控制油门输出（HeliRSC 舵机）可以改变的最大速率，以每秒的百分比表示。 100 的值意味着油门可以在一秒内完成整个范围的变化。值为零给出无限的转换速率。</translation>
    </message>
    <message>
      <source>This controls the time constant for the cross-over frequency used to fuse AHRS (airspeed and heading) and GPS data to estimate ground velocity. Time constant is 0.1/beta. A larger time constant will use GPS data less and a small time constant will use air data less.</source>
      <translation>这控制用于融合 AHRS（空速和航向）和 GPS 数据以估计地面速度的交叉频率的时间常数。时间常数为0.1/beta。较大的时间常数将较少使用 GPS 数据，较小的时间常数将较少使用空气数据。</translation>
    </message>
    <message>
      <source>This controls when to use reverse thrust. If set to zero then reverse thrust is never used. If set to a non-zero value then the bits correspond to flight stages where reverse thrust may be used. Note that reverse thrust is only ever auto-enabled in auto-throttle modes. In modes where throttle control is pilot controlled the ability to do reverse thrust is controlled by throttle stick input. The most commonly used value for USE_REV_THRUST is 2, which means AUTO_LAND only. That enables reverse thrust in the landing stage of AUTO mode. Another common choice is 1, which means to use reverse thrust in all auto flight stages.</source>
      <translation>这控制何时使用反向推力。如果设置为零，则永远不会使用反向推力。如果设置为非零值，则这些位对应于可以使用反向推力的飞行级。请注意，反向推力仅在自动油门模式下自动启用。在油门控制由先导控制的模式下，反向推力的能力由油门杆输入控制。 USE_REV_THRUST 最常用的值是 2，仅表示 AUTO_LAND。这可以在 AUTO 模式的着陆阶段实现反推力。另一个常见的选择是1，这意味着在所有自动飞行阶段都使用反推力。</translation>
    </message>
    <message>
      <source>This controls whether to use dead-reckoning or GPS based navigation. If set to 0 then the GPS won't be used for navigation, and only dead reckoning will be used. A value of zero should never be used for normal flight.</source>
      <translation>这控制是否使用航位推算或基于 GPS 的导航。如果设置为 0，则 GPS 将不会用于导航，仅使用航位推算。正常飞行绝不应使用零值。</translation>
    </message>
    <message>
      <source>This defines the latency between a pitch step input and the achieved setpoint
(inverse to a P gain). Smaller systems may require smaller values.</source>
      <translation>这定义了俯仰步进输入和达到的设定点之间的延迟（与 P 增益相反）。较小的系统可能需要较小的值。</translation>
    </message>
    <message>
      <source>This determines when the filter will use the 3-axis magnetometer fusion model that estimates both earth and body fixed magnetic field states and when it will use a simpler magnetic heading fusion model that does not use magnetic field states. The 3-axis magnetometer fusion is only suitable for use when the external magnetic field environment is stable. EK3_MAG_CAL = 0 uses heading fusion on ground, 3-axis fusion in-flight, and is the default setting for Plane users. EK3_MAG_CAL = 1 uses 3-axis fusion only when manoeuvring. EK3_MAG_CAL = 2 uses heading fusion at all times, is recommended if the external magnetic field is varying and is the default for rovers. EK3_MAG_CAL = 3 uses heading fusion on the ground and 3-axis fusion after the first in-air field and yaw reset has completed, and is the default for copters. EK3_MAG_CAL = 4 uses 3-axis fusion at all times. EK3_MAG_CAL = 5 uses an external yaw sensor with simple heading fusion. NOTE : Use of simple heading magnetometer fusion makes vehicle compass calibration and alignment errors harder for the EKF to detect which reduces the sensitivity of the Copter EKF failsafe algorithm. NOTE: The fusion mode can be forced to 2 for specific EKF cores using the EK3_MAG_MASK parameter. EK3_MAG_CAL = 6 uses an external yaw sensor with fallback to compass when the external sensor is not available if we are flying. NOTE: The fusion mode can be forced to 2 for specific EKF cores using the EK3_MAG_MASK parameter. NOTE: limited operation without a magnetometer or any other yaw sensor is possible by setting all COMPASS_USE, COMPASS_USE2, COMPASS_USE3, etc parameters to 0 and setting COMPASS_ENABLE to 0. If this is done, the EK3_GSF_RUN and EK3_GSF_USE masks must be set to the same as EK3_IMU_MASK. A yaw angle derived from IMU and GPS velocity data using a Gaussian Sum Filter (GSF) will then be used to align the yaw when flight commences and there is sufficient movement.</source>
      <translation>这决定了滤波器何时使用 3 轴磁力计融合模型来估计地球和人体固定磁场状态，以及何时使用不使用磁场状态的更简单的磁航向融合模型。 3轴磁力计融合器仅适合在外部磁场环境稳定时使用。 EK3_MAG_CAL = 0 在地面上使用航向融合，3 在飞行中使用轴融合，并且是 Plane 用户的默认设置。 EK3_MAG_CAL = 1 仅在机动时使用 3 轴融合。 EK3_MAG_CAL = 2 始终使用航向融合，如果外部磁场变化，建议使用航向融合，并且是流动站的默认设置。 EK3_MAG_CAL = 3 在地面上使用航向融合，在第一个空中场和偏航重置完成后使用 3 轴融合，并且是直升机的默认设置。 EK3_MAG_CAL = 4 始终使用 3 轴融合。 EK3_MAG_CAL = 5 使用外部偏航传感器，具有简单的航向融合功能。 NOTE：使用简单的航向磁力计融合使得 EKF 更难检测车辆罗盘校准和对准错误，从而降低了 Copter EKF 故障安全算法的灵敏度。 NOTE：对于特定的 EKF 内核，可以使用 EK3_MAG_MASK 参数将融合模式强制为 2。 EK3_MAG_CAL = 6 使用外部偏航传感器，当我们飞行时外部传感器不可用时，可以回退到罗盘。 NOTE：对于特定的 EKF 内核，可以使用 EK3_MAG_MASK 参数将融合模式强制为 2。 NOTE：通过将所有 COMPASS_USE、COMPASS_USE2、COMPASS_USE3 等参数设置为 0 并将 COMPASS_ENABLE 设置为 0，可以在没有磁力计或任何其他偏航传感器的情况下进行有限操作。如果这样做，EK3_GSF_RUN 和 EK3_GSF_USE 掩码必须设置为与 EK3_IMU_MASK 相同。然后，使用高斯求和滤波器 (GSF) 从 IMU 和 GPS 速度数据导出的偏航角将用于在飞行开始且有足够运动时对齐偏航。</translation>
    </message>
    <message>
      <source>This enables QuadPlane functionality, assuming multicopter motors start on output 5. If this is set to 2 then when starting AUTO mode it will initially be in VTOL AUTO mode.</source>
      <translation>假设多旋翼电机在输出 5 上启动，这将启用 QuadPlane 功能。如果设置为 2，则启动 AUTO 模式时，最初将处于 VTOL AUTO 模式。</translation>
    </message>
    <message>
      <source>This enables automatic crash checking. When enabled the motors will disarm if a crash is detected.</source>
      <translation>这可以实现自动崩溃检查。启用后，如果检测到碰撞，电机将上锁。</translation>
    </message>
    <message>
      <source>This enables optional temperature calibration features. Setting PersistParams will save the accelerometer and temperature calibration parameters in the bootloader sector on the next update of the bootloader.</source>
      <translation>这使得可选的温度校准功能成为可能。设置 PersistParams 将在下次更新引导加载程序时将加速度计和温度校准参数保存在引导加载程序扇区中。</translation>
    </message>
    <message>
      <source>This enables the advanced failsafe system. If this is set to zero (disable) then all the other AFS options have no effect</source>
      <translation>这使得先进的故障安全系统成为可能。如果此设置为零（禁用），则所有其他 AFS 选项均无效</translation>
    </message>
    <message>
      <source>This enables use of VTOL in guided mode. When enabled the aircraft will switch to VTOL flight when the guided destination is reached and hover at the destination.</source>
      <translation>这使得可以在引导模式下使用 VTOL。启用后，当到达引导目的地时，飞机将切换到 VTOL 飞行并悬停在目的地。</translation>
    </message>
    <message>
      <source>This gain can be used to counteract the "adverse yaw" effect for fixed wings. When the plane enters a roll it will tend to yaw the nose out of the turn. This gain enables the use of a yaw actuator to counteract this effect.</source>
      <translation>该增益可用于抵消固定翼的“不利偏航”效应。当飞机进入横滚状态时，机头往往会偏离转弯。该增益使得能够使用偏航致动器来抵消这种影响。</translation>
    </message>
    <message>
      <source>This increment is added to TRIM_YAW when airspeed is FW_AIRSPD_MIN.</source>
      <translation>当空速为 FW_AIRSPD_MIN 时，该增量被添加到 TRIM_YAW。</translation>
    </message>
    <message>
      <source>This is a bitmask of enabled NMEA messages. All messages will be sent consecutively at the same rate interval</source>
      <translation>这是启用的 NMEA 消息的位掩码。所有消息将以相同的速率间隔连续发送</translation>
    </message>
    <message>
      <source>This is added to the target altitude in automatic flight. It can be used to add a global altitude offset to a mission. The offset resets to zero on reboot</source>
      <translation>这会添加到自动飞行中的目标高度中。它可用于为任务添加全球高度偏移。重新启动时偏移量重置为零</translation>
    </message>
    <message>
      <source>This is deceleration rate that will be used in calculating the stopping distance when transitioning from fixed wing flight to multicopter flight.</source>
      <translation>这是用于计算从固定翼飞行过渡到多旋翼飞行时的停止距离的减速度。</translation>
    </message>
    <message>
      <source>This is slew rate in %/s that will be used in manual mode to avoid large step outputs on the motors. Set to zero to disable.</source>
      <translation>这是以 %/s 为单位的转换速率，将在手动模式下使用，以避免电机上出现大的步进输出。设置为零以禁用。</translation>
    </message>
    <message>
      <source>This is the RMS value of noise and errors in optical flow measurements. Increasing it reduces the weighting on these measurements.</source>
      <translation>这是光流测量中噪声和误差的 RMS 值。增加它会减少这些测量的权重。</translation>
    </message>
    <message>
      <source>This is the RMS value of noise in the range beacon measurement. Increasing it reduces the weighting on this measurement.</source>
      <translation>这是距离信标测量中噪声的 RMS 值。增加它会减少该测量的权重。</translation>
    </message>
    <message>
      <source>This is the action taken when the power button is pressed while the left (Mode) button is held down</source>
      <translation>这是在按住左侧（模式）按钮的同时按下电源按钮时所采取的操作</translation>
    </message>
    <message>
      <source>This is the altitude below which the wings are held level for TAKEOFF and AUTO modes. Below this altitude, roll demand is restricted to LEVEL_ROLL_LIMIT. Normal-flight roll restriction resumes above TKOFF_LVL_ALT*2 or TKOFF_ALT, whichever is lower. Roll limits are scaled while between those altitudes for a smooth transition.</source>
      <translation>这是 TAKEOFF 和 AUTO 模式下机翼保持水平的高度。低于此高度，横滚需求仅限于 LEVEL_ROLL_LIMIT。正常飞行横滚限制在 TKOFF_LVL_ALT*2 或 TKOFF_ALT（以较低者为准）之上恢复。在这些高度之间，横滚限制会按比例缩放，以实现平稳过渡。</translation>
    </message>
    <message>
      <source>This is the angle of the tilt servos when in VTOL mode and at minimum output (fully back). This needs to be set in addition to Q_TILT_TYPE=2, to enable vectored control for yaw in tilt quadplanes. This is also used to limit the forward travel of bicopter tilts(Q_TILT_TYPE=3) when in VTOL modes.</source>
      <translation>这是处于 VTOL 模式和最小输出（完全向后）时倾斜舵机系统的角度。除了 Q_TILT_TYPE=2 之外，还需要设置此值，以启用倾斜四平面中偏航的矢量控制。这也用于限制在 VTOL 模式下双旋翼倾斜的向前移动（Q_TILT_TYPE=3）。</translation>
    </message>
    <message>
      <source>This is the applied simulated acceleration to the 1st accelerometer (Z-axis)</source>
      <translation>这是对第一个加速度计（Z 轴）应用的模拟加速度</translation>
    </message>
    <message>
      <source>This is the applied simulated angular rate to the 1st gyroscope (Y-axis)</source>
      <translation>这是对第一个陀螺仪（Y 轴）应用的模拟角速率</translation>
    </message>
    <message>
      <source>This is the arm length generating the pitching moment
This value can be measured with a ruler.
This corresponds to half the distance between the front and rear motors.</source>
      <translation>这是产生俯仰力矩的臂长。这个值可以用尺子测量。这相当于前后电机之间距离的一半。</translation>
    </message>
    <message>
      <source>This is the damping gain for the pitch demand loop during landing. Increase to add damping  to correct for oscillations in speed and height. If set to 0 then TECS_PTCH_DAMP will be used instead.</source>
      <translation>这是着陆期间俯仰需求回路的阻尼增益。增加阻尼以纠正速度和高度的振荡。如果设置为 0，则将使用 TECS_PTCH_DAMP。</translation>
    </message>
    <message>
      <source>This is the divider from battery 2 voltage to ADC voltage.
If using e.g. Mauch power modules the value from the datasheet
can be applied straight here. A value of -1 means to use
the board default.</source>
      <translation>这是电池 2 电压到 ADC 电压的分压器。如果使用例如Mauch 电源模块数据表中的值可以直接应用于此处。 -1 值表示使用板默认值。</translation>
    </message>
    <message>
      <source>This is the horizontal velocity error (EVH) threshold that will trigger a failsafe.
The default is appropriate for a multicopter. Can be increased for a fixed-wing.
If the previous velocity error was below this threshold, there is an additional
factor of 2.5 applied (threshold for invalidation 2.5 times the one for validation).</source>
      <translation>这是将触发故障保护的水平速度误差 (EVH) 阈值。默认值适用于多旋翼飞行器。可以增加固定翼。如果先前的速度误差低于此阈值，则应用 2.5 的附加因子（无效阈值 2.5 乘以验证阈值）。</translation>
    </message>
    <message>
      <source>This is the main flight mode selector.
The channel index (starting from 1 for channel 1) indicates
which channel should be used for deciding about the main mode.
A value of zero indicates the switch is not assigned.</source>
      <translation>这是主要的飞行模式选择器。通道索引（通道 1 从 1 开始）指示应使用哪个通道来决定主模式。值为零表示未分配开关。</translation>
    </message>
    <message>
      <source>This is the maximum calibrated climb rate that the aircraft can achieve with the throttle set to FW_THR_MAX and the airspeed set to the trim value. For electric aircraft make sure this number can be achieved towards the end of flight when the battery voltage has reduced.</source>
      <translation>这是油门设置为 FW_THR_MAX、空速设置为配平值时飞机可以达到的最大校准爬升率。对于电动飞机，请确保在飞行结束时电池电压降低时可以达到此数字。</translation>
    </message>
    <message>
      <source>This is the maximum torque delivered by one propeller
when the motor is running at full speed.
This value is usually about few percent of the maximum thrust force.</source>
      <translation>这是电机全速运转时，一个螺旋桨所能输出的最大扭矩。该值通常约为最大推力的百分之几。</translation>
    </message>
    <message>
      <source>This is the minimum altitude above Home the system will always obey in Loiter (Hold) mode if switched into this
mode without specifying an altitude (e.g. through Loiter switch on RC).
Doesn't affect Loiters that are part of Missions or that are entered through a reposition setpoint ("Go to").
Set to a negative value to disable.</source>
      <translation>这是高于 Home 的最小高度，如果在未指定高度的情况下切换到此模式（例如通过 RC 上的盘旋开关），系统将始终遵循盘旋（保持）模式。不影响属于任务一部分或通过重新定位设定点（“前往”）输入的盘旋者。设置为负值以禁用。</translation>
    </message>
    <message>
      <source>This is the minimum percentage throttle output while running, this includes being disarmed, but not safe</source>
      <translation>这是运行时油门输出的最小百分比，包括上锁，但不安全</translation>
    </message>
    <message>
      <source>This is the number of msec that the external navigation system measurements lag behind the inertial measurements.</source>
      <translation>这是外部导航系统测量滞后于惯性测量的毫秒数。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PCA9685_FUNC12).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC12）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PCA9685_FUNC5).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC5）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PWM_AUX_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_AUX_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PWM_MAIN_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_MAIN_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_EC_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_SV_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_WH_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_WH_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC6).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_EC_FUNC6）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC6).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_SV_FUNC6）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC14).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC14）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC13).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC13）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PCA9685_FUNC14).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC14）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PCA9685_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PWM_AUX_FUNC6).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_AUX_FUNC6）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PWM_MAIN_FUNC6).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_MAIN_FUNC6）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_EC_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_SV_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_WH_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_WH_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC8).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_EC_FUNC8）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC8).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_SV_FUNC8）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC16).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC16）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC9).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC9）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC15).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC15）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC9).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC9）。</translation>
    </message>
    <message>
      <source>This is the protocol used between the ground station and the autopilot.
Recommended is Auto, RC only or MAVLink gimbal protocol v2.
The rest will be deprecated.</source>
      <translation>这是地面站和自动驾驶仪之间使用的协议。推荐使用自动、仅 RC 或 MAVLink 万向节协议 v2。其余的将被弃用。</translation>
    </message>
    <message>
      <source>This is the ratio of static pressure error to dynamic pressure generated by a positive wind relative velocity along the X body axis. If the baro height estimate rises during forward flight, then this will be a negative number.</source>
      <translation>这是静压误差与沿 X 体轴正风相对速度产生的动压之比。如果在向前飞行期间气压高度估计值上升，则这将是负数。</translation>
    </message>
    <message>
      <source>This is the ratio of static pressure error to dynamic pressure generated by a wind relative velocity along the positive Y (RH) body axis. If the baro height estimate rises during sideways flight to the right, then this will be a negative number.</source>
      <translation>这是静压误差与沿正 Y (RH) 主体轴的风相对速度产生的动压之比。如果在向右侧向飞行期间巴罗高度估计值上升，则这将是负数。</translation>
    </message>
    <message>
      <source>This is the secondary mode. This mode is assumed to require GPS</source>
      <translation>这是次要模式。假设该模式需要 GPS</translation>
    </message>
    <message>
      <source>This is the target pitch during the takeoff.</source>
      <translation>这是起飞期间的目标俯仰角。</translation>
    </message>
    <message>
      <source>This is the time in seconds in manual mode that failsafe termination will activate if RC input is lost. For the OBC rules this should be (1.5). Use 0 to disable.</source>
      <translation>这是在手动模式下，如果 RC 输入丢失，故障安全终止将激活的时间（以秒为单位）。对于 OBC 规则，这应该是 (1.5)。使用0禁用。</translation>
    </message>
    <message>
      <source>This is the value sent to the starter channel when off</source>
      <translation>这是关闭时发送到启动器通道的值</translation>
    </message>
    <message>
      <source>This is the weight of the vehicle at which it's performance limits were derived. A zero or negative value disables trim throttle and minimum airspeed compensation based on weight.</source>
      <translation>这是得出其性能极限的车辆重量。零或负值将禁用调整油门和基于重量的最小空速补偿。</translation>
    </message>
    <message>
      <source>This is used to constrain a minimum altitude below which we keep wings level to avoid wing tip strike. It's safer
to give a slight margin here (&gt; 0m)</source>
      <translation>这用于限制最低高度，低于该高度我们保持机翼水平以避免翼尖撞击。这里稍微留点余量比较安全（&gt; 0m）</translation>
    </message>
    <message>
      <source>This is used to specify how to handle motor failures
reported by failure detector.</source>
      <translation>用于指定如何处理故障检测器报告的电机故障。</translation>
    </message>
    <message>
      <source>This limits the number of degrees of steering in centi-degrees over which the integrator will operate. At the default setting of 1500 centi-degrees, the integrator will be limited to +- 15 degrees of servo travel. The maximum servo deflection is +- 45 centi-degrees, so the default value represents a 1/3rd of the total control throw which is adequate unless the vehicle is severely out of trim.</source>
      <translation>这限制了积分器运行的转向度数（以厘度为单位）。在 1500 厘度的默认设置下，积分器将被限制为 +- 15 度的舵机行程。最大舵机偏转为 +- 45 厘度，因此默认值表示总控制行程的 1/3，这已经足够了，除非车辆严重偏离平衡。</translation>
    </message>
    <message>
      <source>This noise controls the growth of estimated error due to accelerometer measurement errors excluding bias. Increasing it makes the flter trust the accelerometer measurements less and other measurements more.</source>
      <translation>该噪声控制了由于加速度计测量误差（不包括偏差）导致的估计误差的增长。增加它会使滤波器更少信任加速度计测量结果，而更多信任其他测量结果。</translation>
    </message>
    <message>
      <source>This number is incremented automatically after every flight on disarming in order to remember the next flight UUID. The first flight is 0.</source>
      <translation>每次上锁飞行后，该数字都会自动递增，以便记住下一个飞行 UUID。首个飞行是0。</translation>
    </message>
    <message>
      <source>This parameter allows you to control whether the order in which the tubes are attached to your pitot tube matters. If you set this to 0 then the first (often the top) connector on the sensor needs to be the stagnation pressure (the pressure at the tip of the pitot tube). If set to 1 then the second (often the bottom) connector needs to be the stagnation pressure. If set to 2 (the default) then the airspeed driver will accept either order. The reason you may wish to specify the order is it will allow your airspeed sensor to detect if the aircraft is receiving excessive pressure on the static port compared to the stagnation port such as during a stall, which would otherwise be seen as a positive airspeed.</source>
      <translation>此参数允许您控制管连接到皮托管的顺序是否重要。如果将其设置为 0，则传感器上的第一个（通常是顶部）连接器需要是停滞压力（皮托管尖端的压力）。如果设置为 1，则第二个（通常是底部）连接器需要是停滞压力。如果设置为 2（默认值），则空速驱动程序将接受任一命令。您可能希望指定顺序的原因是，它将允许您的空速传感器检测飞机在静态端口上是否受到与停滞端口相比过大的压力，例如在失速期间，否则这将被视为正空速。</translation>
    </message>
    <message>
      <source>This parameter can be used to force the servos to their trim value for a time on startup. This can help with some servo types</source>
      <translation>该参数可用于在启动时强制舵机系统在一段时间内达到其调整值。这对某些舵机类型有帮助</translation>
    </message>
    <message>
      <source>This parameter controls the primary height sensor used by the EKF. If the selected option cannot be used, it will default to Baro as the primary height source. Setting 0 will use the baro altitude at all times. Setting 1 uses the range finder and is only available in combination with optical flow navigation (EK2_GPS_TYPE = 3). Setting 2 uses GPS. Setting 3 uses the range beacon data. NOTE - the EK2_RNG_USE_HGT parameter can be used to switch to range-finder when close to the ground.</source>
      <translation>该参数控制 EKF 使用的主高度传感器。如果无法使用所选选项，它将默认使用 Baro 作为主要高度源。设置 0 将始终使用气压高度。设置 1 使用测距仪，并且仅与光流导航结合使用 (EK2_GPS_TYPE = 3)。设置 2 使用 GPS。设置 3 使用距离信标数据。 NOTE - EK2_RNG_USE_HGT 参数可用于在接近地面时切换到测距仪。</translation>
    </message>
    <message>
      <source>This parameter defines measurement rate of the AFBR Rangefinder in long range mode.</source>
      <translation>该参数定义了 AFBR 测距仪在远距离模式下的测量速率。</translation>
    </message>
    <message>
      <source>This parameter defines the minimum condition under which the system will command
the LIGHT_ID_LANDING lights on
0 - Always off
1 - When autopilot is armed
2 - When autopilot is prearmed
3 - Always on</source>
      <translation>该参数定义了系统命令 LIGHT_ID_LANDING 灯亮起的最低条件 0 - 始终关闭 1 - 当自动驾驶仪解锁时 2 - 当自动驾驶仪预解锁时 3 - 始终打开</translation>
    </message>
    <message>
      <source>This parameter defines the minimum distance from ground at which the optical flow sensor operates reliably. The sensor may be usable below this height, but accuracy will progressively reduce to loss of focus.</source>
      <translation>该参数定义了光流传感器可靠运行时距地面的最小距离。传感器可能在低于此高度时可用，但精度将逐渐降低直至失去焦点。</translation>
    </message>
    <message>
      <source>This parameter defines the threshold for switching between short and long range mode.
The mode will switch from short to long range when the distance is greater than the threshold plus the hysteresis.
The mode will switch from long to short range when the distance is less than the threshold minus the hysteresis.</source>
      <translation>该参数定义短距离和长距离模式之间切换的阈值。当距离大于阈值加上迟滞时，模式将从短距离切换到长距离。当距离小于阈值减去迟滞时，模式将从长距离切换到短距离。</translation>
    </message>
    <message>
      <source>This parameter enables bidirectional DShot which provides RPM feedback. Note that this requires ESCs that support bidirectional DSHot, e.g. BlHeli32. This is not the same as DShot telemetry which requires an additional serial connection.</source>
      <translation>该参数启用提供 RPM 反馈的双向 DShot。请注意，这需要支持双向 DSHot 的 ESC，例如BlHeli32。这与需要额外串行连接的 DShot 遥测不同。</translation>
    </message>
    <message>
      <source>This parameter is used by Ground Station software to save the number
of channels which were used during RC calibration. It is only meant
for ground station use.</source>
      <translation>地面站软件使用此参数来保存 RC 校准期间使用的通道数。它仅供地面站使用。</translation>
    </message>
    <message>
      <source>This parameter sets the RMS terrain gradient percentage assumed by the terrain height estimation. Terrain height can be estimated using optical flow and/or range finder sensor data if fitted. Smaller values cause the terrain height estimate to be slower to respond to changes in measurement. Larger values cause the terrain height estimate to be faster to respond, but also more noisy. Generally this value can be reduced if operating over very flat terrain and increased if operating over uneven terrain.</source>
      <translation>该参数设置地形高度估计所假设的RMS地形坡度百分比。可以使用光流和/或测距仪传感器数据（如果安装）来估计地形高度。较小的值会导致地形高度估计对测量变化的响应速度较慢。较大的值会导致地形高度估计响应速度更快，但噪音也更大。一般来说，如果在非常平坦的地形上运行，则可以减小该值；如果在不平坦的地形上运行，则可以增大该值。</translation>
    </message>
    <message>
      <source>This parameter sets the delay between image integration start and strobe firing</source>
      <translation>该参数设置图像积分开始和闪光灯发射之间的延迟</translation>
    </message>
    <message>
      <source>This parameter sets the gain from forward accel/tilt to forward throttle in certain Q modes. The Q modes this feature operates in is controlled by the Q_FWD_THR_USE parameter. Vehicles using separate forward thrust motors, eg quadplanes, should set this parameter to (all up weight) / (maximum combined thrust of forward motors) with a value of 2 being typical. Vehicles that tilt lifting rotors to provide forward thrust should set this parameter to (all up weight) / (weight lifted by tilting rotors) which for most aircraft can be approximated as (total number of lifting rotors) / (number of lifting rotors that tilt). When using this method of forward throttle control, the forward tilt angle limit is controlled by the Q_FWD_PIT_LIM parameter.</source>
      <translation>该参数设置某些 Q 模式下从前向加速/倾斜到前向油门的增益。该功能运行的 Q 模式由 Q_FWD_THR_USE 参数控制。使用单独的前向推力电机的车辆，例如四翼飞机，应将此参数设置为（全部重量）/（前向电机的最大组合推力），典型值为 2。倾斜升力旋翼以提供向前推力的车辆应将此参数设置为（全部向上重量）/（倾斜旋翼升起的重量），对于大多数飞机而言，该参数可近似为（升力旋翼总数）/（倾斜升力旋翼数量）。使用这种向前油门控制方法时，前倾角度限制由 Q_FWD_PIT_LIM 参数控制。</translation>
    </message>
    <message>
      <source>This parameter sets the number of standard deviations applied to the magnetometer measurement innovation consistency check. Decreasing it makes it more likely that good measurements will be rejected. Increasing it makes it more likely that bad measurements will be accepted.</source>
      <translation>该参数设置应用于磁力计测量创新一致性检查的标准偏差数。降低它会使好的测量结果更有可能被拒绝。增加它使得错误的测量更有可能被接受。</translation>
    </message>
    <message>
      <source>This parameter sets the time between two consecutive trigger events</source>
      <translation>该参数设置两个连续触发事件之间的时间</translation>
    </message>
    <message>
      <source>This provides a set of additional control options for quadplanes. LevelTransition means that the wings should be held level to within LEVEL_ROLL_LIMIT degrees during transition to fixed wing flight, and the vehicle will not use the vertical lift motors to climb during the transition. If AllowFWTakeoff bit is not set then fixed wing takeoff on quadplanes will instead perform a VTOL takeoff. If AllowFWLand bit is not set then fixed wing land on quadplanes will instead perform a VTOL land. If respect takeoff frame is not set the vehicle will interpret all takeoff waypoints as an altitude above the corrent position. When Use QRTL is set it will replace QLAND with QRTL for failsafe actions when in VTOL modes.</source>
      <translation>这为四平面提供了一组附加控制选项。 LevelTransition是指在过渡到固定翼飞行时机翼应保持水平在LEVEL_ROLL_LIMIT度以内，并且飞行器在过渡期间不会使用垂直升力电机爬升。如果未设置AllowFWTakeoff位，则四平面上的固定翼起飞将改为执行VTOL起飞。如果未设置AllowFWLand 位，则四平面上的固定翼着陆将改为执行VTOL 着陆。如果未设置尊重起飞坐标系，车辆会将所有起飞航路点解释为高于当前位置的高度。当设置使用 QRTL 时，在 VTOL 模式下，它将用 QRTL 替换 QLAND，以实现故障安全操作。</translation>
    </message>
    <message>
      <source>This selects between different stick input modes. The default is mode2, which has throttle on the left stick and pitch on the right stick. You can instead set mode1, which has throttle on the right stick and pitch on the left stick.</source>
      <translation>这可以在不同的摇杆输入模式之间进行选择。默认为2模式，左摇杆油门，右摇杆俯仰。您可以改为设置模式1，油门在右摇杆上，俯仰在左摇杆上。</translation>
    </message>
    <message>
      <source>This selects the output PWM type as regular PWM, OneShot, Brushed motor support using PWM (duty cycle) with separated direction signal, Brushed motor support with separate throttle and direction PWM (duty cyle)</source>
      <translation>这将选择输出 PWM 类型作为常规 PWM、OneShot、使用具有独立方向信号的 PWM（占空比）的有刷电机支持、具有独立油门和方向的有刷电机支持 PWM（占空比）</translation>
    </message>
    <message>
      <source>This sets a bitmask of outputs which will be available as GPIOs. Any auxillary output with either the function set to -1 or with the corresponding bit set in this mask will be available for use as a GPIO pin</source>
      <translation>这设置了可用作 GPIO 的输出位掩码。任何功能设置为 -1 或在此掩码中设置相应位的辅助输出都可用作 GPIO 引脚</translation>
    </message>
    <message>
      <source>This sets a lower limit on the speed accuracy reported by the GPS receiver that is used to set horizontal velocity observation noise. If the model of receiver used does not provide a speed accurcy estimate, then the parameter value will be used. Increasing it reduces the weighting of the GPS horizontal velocity measurements.</source>
      <translation>这对用于设置水平速度观测噪声的 GPS 接收器报告的速度精度设置了下限。如果所使用的接收器模型不提供速度精度估计，则将使用该参数值。增加它会减少 GPS 水平速度测量的权重。</translation>
    </message>
    <message>
      <source>This sets options to change the behaviour of the battery monitor</source>
      <translation>这设置了更改电池监视器行为的选项</translation>
    </message>
    <message>
      <source>This sets the PWM lower limit for the tuning channel</source>
      <translation>这设置了调谐通道的 PWM 下限</translation>
    </message>
    <message>
      <source>This sets the amount of noise used when fusing X and Y acceleration as an observation that enables esitmation of wind velocity for multi-rotor vehicles. This feature is enabled by the EK3_BCOEF_X and EK3_BCOEF_Y parameters</source>
      <translation>这设置了融合 X 和 Y 加速度作为观测值时使用的噪声量，从而可以估计多旋翼飞行器的风速。该功能由 EK3_BCOEF_X 和 EK3_BCOEF_Y 参数启用</translation>
    </message>
    <message>
      <source>This sets the bus address of the sensor, where applicable. Used for the I2C and UAVCAN sensors to allow for multiple sensors on different addresses.</source>
      <translation>这将设置传感器的总线地址（如果适用）。用于 I2C 和 UAVCAN 传感器，以允许不同地址上的多个传感器。</translation>
    </message>
    <message>
      <source>This sets the controller error threshold above which an alarm will sound and a message will be sent to the GCS to warn of controller instability</source>
      <translation>这设置了控制器错误阈值，超过该阈值将发出警报，并向 GCS 发送一条消息以警告控制器不稳定</translation>
    </message>
    <message>
      <source>This sets the max PWM value in microseconds that will ever be output to the motors</source>
      <translation>这设置了将输出到电机的最大 PWM 值（以微秒为单位）</translation>
    </message>
    <message>
      <source>This sets the maximum nose down pitch rate that the attitude controller will demand (degrees/sec) in angle stabilized modes. Setting it to zero disables the limit.</source>
      <translation>这设置了姿态控制器在角度稳定模式下要求的最大机头向下俯仰速率（度/秒）。将其设置为零会禁用该限制。</translation>
    </message>
    <message>
      <source>This sets the maximum rate that streaming log messages will be logged to the file backend. A value of zero means that rate limiting is disabled.</source>
      <translation>这设置流日志消息记录到文件后端的最大速率。值为零表示禁用速率限制。</translation>
    </message>
    <message>
      <source>This sets the minimum RSSI of an auto-bind packet for it to be accepted. This should be set so that auto-bind will only happen at short range to minimise the change of an auto-bind happening accidentially</source>
      <translation>这设置了自动绑定数据包被接受的最小 RSSI。应设置此项，以便自动绑定仅在短距离内发生，以最大程度地减少意外发生的自动绑定的变化</translation>
    </message>
    <message>
      <source>This sets the percentage number of standard deviations applied to the GPS velocity measurement innovation consistency check. Decreasing it makes it more likely that good measurements will be rejected. Increasing it makes it more likely that bad measurements will be accepted. If EK3_GLITCH_RAD set to 0 the velocity innovations will be clipped instead of rejected if they exceed the gate size and a smaller value of EK3_VEL_I_GATE not exceeding 300 is recommended to limit the effect of GPS transient errors.</source>
      <translation>这设置了应用于 GPS 速度测量创新一致性检查的标准偏差百分比。降低它会使好的测量结果更有可能被拒绝。增加它使得错误的测量更有可能被接受。如果 EK3_GLITCH_RAD 设置为 0，则如果速度创新超过门尺寸，则速度创新将被微调而不是被拒绝，并且建议使用不超过 300 的较小值 EK3_VEL_I_GATE 以限制 GPS 瞬态误差的影响。</translation>
    </message>
    <message>
      <source>This sets the percentage number of standard deviations applied to the range beacon measurement innovation consistency check. Decreasing it makes it more likely that good measurements will be rejected. Increasing it makes it more likely that bad measurements will be accepted.</source>
      <translation>这设置了应用于范围信标测量创新一致性检查的标准偏差的百分比数。降低它会使好的测量结果更有可能被拒绝。增加它使得错误的测量更有可能被接受。</translation>
    </message>
    <message>
      <source>This sets the rate we send data for time sync between aircraft</source>
      <translation>这设置了我们发送飞机之间时间同步数据的速率</translation>
    </message>
    <message>
      <source>This sets the telemetry port to use for blheli pass-thru</source>
      <translation>这设置用于 blheli pass-thru 的遥测端口</translation>
    </message>
    <message>
      <source>This sets the time that maximum throttle will be forced during a fixed wing takeoff.</source>
      <translation>这设置了固定翼起飞期间强制最大油门的时间。</translation>
    </message>
    <message>
      <source>This sets what MAVLink source system IDs are accepted for GCS failsafe handling, RC overrides and manual control. When MAV_GCS_SYSID_HI is less than MAV_GCS_SYSID then only this value is considered to be a GCS. When MAV_GCS_SYSID_HI is greater than or equal to MAV_GCS_SYSID then the range of values between MAV_GCS_SYSID and MAV_GCS_SYSID_HI (inclusive) are all treated as valid GCS MAVLink system IDs.</source>
      <translation>这设置了 GCS 故障安全处理、RC 覆盖和手动控制接受的 MAVLink 源系统 ID。当 MAV_GCS_SYSID_HI 小于 MAV_GCS_SYSID 时，仅该值被视为 GCS。当 MAV_GCS_SYSID_HI 大于或等于 MAV_GCS_SYSID 时，MAV_GCS_SYSID 和 MAV_GCS_SYSID_HI（含）之间的值范围均被视为有效的 GCS MAVLink 系统 ID。</translation>
    </message>
    <message>
      <source>This setting helps avoid signal problems like overshoot or undershoot.</source>
      <translation>此设置有助于避免信号问题，例如过冲或下冲。</translation>
    </message>
    <message>
      <source>This threshold is used for the state machine to switch from driving to turning based on the error between the desired and actual yaw. It is also used as the threshold whether the rover should come to a smooth stop at the next waypoint. This slow down effect is active if the angle between the line segments from prevWP-currWP and currWP-nextWP is smaller then 180 - RD_TRANS_DRV_TRN.</source>
      <translation>该阈值用于状态机根据期望偏航与实际偏航之间的误差从驾驶切换到转弯。它还用作流动站是否应在下一个航路点平滑停止的阈值。如果 prevWP-currWP 和 currWP-nextWP 之间的线段之间的角度小于 180 - RD_TRANS_DRV_TRN，则此减速效果有效。</translation>
    </message>
    <message>
      <source>This value is used in slow mode if
no aux channel is mapped and
no limit is commanded through MAVLink.</source>
      <translation>如果没有映射辅助通道并且通过 MAVLink 没有命令限制，则在慢速模式下使用该值。</translation>
    </message>
    <message>
      <source>This value represents the North-South location on Earth where the simulation begins.
LAT0, LON0, H0, MU_X, MU_Y, and MU_Z should ideally be consistent among each others
to represent a physical ground location on Earth.</source>
      <translation>该值表示模拟开始时地球上的南北位置。理想情况下，LAT0、LON0、H0、MU_X、MU_Y 和 MU_Z 彼此之间应保持一致，以表示地球上的物理地面位置。</translation>
    </message>
    <message>
      <source>Three side calibration</source>
      <translation>三边校准</translation>
    </message>
    <message>
      <source>Threshold for selecting offboard mode</source>
      <translation>选择离线模式的阈值</translation>
    </message>
    <message>
      <source>Threshold to tolerate vibration to determine if vehicle is motionless. This depends on the frame type and if there is a constant vibration due to motors before launch or after landing. Total motionless is about 0.05. Suggested values: Planes/rover use 0.1, multirotors use 1, tradHeli uses 5</source>
      <translation>容忍振动的阈值以确定车辆是否静止。这取决于机架类型以及发射前或着陆后电机是否持续振动。总不动大约是0.05。建议值：飞机/流动站使用 0.1，多旋翼使用 1，tradHeli 使用 5</translation>
    </message>
    <message>
      <source>Throttle Curve at 0% Coll</source>
      <translation>0% Coll 的油门曲线</translation>
    </message>
    <message>
      <source>Throttle Mix Maximum</source>
      <translation>油门混合最大</translation>
    </message>
    <message>
      <source>Throttle acceleration controller D gain.  Compensates for short-term change in desired vertical acceleration vs actual acceleration</source>
      <translation>油门加速控制器 D 增益。  补偿所需垂直加速度与实际加速度的短期变化</translation>
    </message>
    <message>
      <source>Throttle and RC Failsafe Enable</source>
      <translation>油门和 RC 故障安全启用</translation>
    </message>
    <message>
      <source>Throttle channel number. This is useful when you have a RC transmitter that can't change the channel order easily. Throttle is normally on channel 3, but you can move it to any channel with this parameter. Warning APM 2.X: Changing the throttle channel could produce unexpected fail-safe results if connection between receiver and on-board PPM Encoder is lost. Disabling on-board PPM Encoder is recommended.  Reboot is required for changes to take effect.</source>
      <translation>油门通道数。当您的 RC 发射机无法轻松更改通道顺序时，这非常有用。油门通常在通道 3 上，但您可以使用此参数将其移动到任何通道。警告 APM 2.X：如果接收器和板载 PPM 编码器之间的连接丢失，更改油门通道可能会产生意外的故障安全结果。建议禁用板载 PPM 编码器。  需要重新启动才能使更改生效。</translation>
    </message>
    <message>
      <source>Throttle curve at position 4</source>
      <translation>4 位置油门曲线</translation>
    </message>
    <message>
      <source>Throttle gain for manual inputs in attitude control mode</source>
      <translation>姿态控制模式下手动输入油门增益</translation>
    </message>
    <message>
      <source>Throttle linearisation - Second Order</source>
      <translation>油门线性化 - 二阶</translation>
    </message>
    <message>
      <source>Throttle notch min freqency ratio</source>
      <translation>油门档次最小频率比</translation>
    </message>
    <message>
      <source>Throttle slew rate</source>
      <translation>油门转换速率</translation>
    </message>
    <message>
      <source>Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)</source>
      <translation>手动飞行期间使用的油门与姿态控制优先级（较高的值意味着我们优先考虑姿态控制而不是油门）</translation>
    </message>
    <message>
      <source>ThrottleLeft</source>
      <translation>油门左</translation>
    </message>
    <message>
      <source>Throw mode target altitude to descend</source>
      <translation>投掷模式目标高度下降</translation>
    </message>
    <message>
      <source>Thrust coefficient of rotor 1</source>
      <translation>转子推力系数1</translation>
    </message>
    <message>
      <source>Thrust coefficient of rotor 7</source>
      <translation>转子推力系数7</translation>
    </message>
    <message>
      <source>Thrust from Motors in SIM_ENGINE_FAIL will be multiplied by this factor</source>
      <translation>SIM_ENGINE_FAIL 中电机的推力将乘以该系数</translation>
    </message>
    <message>
      <source>Tiller</source>
      <translation>耕耘者</translation>
    </message>
    <message>
      <source>Tilt 4</source>
      <translation>倾斜4</translation>
    </message>
    <message>
      <source>Tilt Servo 2 Tilt Angle at Minimum</source>
      <translation>倾斜舵机 2 最小倾斜角度</translation>
    </message>
    <message>
      <source>TiltMotorFrontLeft</source>
      <translation>倾斜电机左前</translation>
    </message>
    <message>
      <source>Tiltrotor downwards tilt rate</source>
      <translation>倾转旋翼机向下倾斜率</translation>
    </message>
    <message>
      <source>Time Horizon Fail</source>
      <translation>时间范围失败</translation>
    </message>
    <message>
      <source>Time constant for deepstall L1 control</source>
      <translation>深失速 L1 控制的时间常数</translation>
    </message>
    <message>
      <source>Time constant used to limit the smooth power throttling.</source>
      <translation>用于限制平滑功率节流的时间常数。</translation>
    </message>
    <message>
      <source>Time in seconds for throttle output (HeliRSC servo) to ramp from ground idle (RSC_IDLE) to flight idle throttle setting when motor interlock is enabled (throttle hold off).</source>
      <translation>当启用电机互锁（油门延迟关闭）时，油门输出（HeliRSC 舵机）从地面怠速 (RSC_IDLE) 斜坡到飞行怠速油门设置的时间（以秒为单位）。</translation>
    </message>
    <message>
      <source>Time in seconds to slew output from full to zero.  For medium size copter such as a Solo, a value of 0.275 is a good starting point.  This is used to limit the rate at which output can change.  Range is constrained between 0 and 0.5.</source>
      <translation>将输出从满转为零的时间（以秒为单位）。  对于中型直升机（例如 Solo），0.275 的值是一个很好的起点。  这用于限制输出变化的速率。  范围限制在 0 和 0.5 之间。</translation>
    </message>
    <message>
      <source>Time it takes for the RunCam to become fully ready in ms. If this is too short then commands can get out of sync.</source>
      <translation>RunCam 完全准备就绪所需的时间（以毫秒为单位）。如果这太短，那么命令可能会不同步。</translation>
    </message>
    <message>
      <source>Time the Iridium driver will wait for additional mavlink messages to combine them into one SBD message</source>
      <translation>Iridium 驱动程序等待其他 mavlink 消息以将它们组合成一个 SBD 消息的时间</translation>
    </message>
    <message>
      <source>Time to reach zero amplitude at the end of the sweep</source>
      <translation>扫描结束时达到零幅度的时间</translation>
    </message>
    <message>
      <source>Time-out to wait when onboard computer connection is lost before warning about loss connection</source>
      <translation>机载计算机连接丢失时等待超时，然后发出有关丢失连接的警告</translation>
    </message>
    <message>
      <source>Timeout for detecting a failure after takeoff</source>
      <translation>起飞后检测故障超时</translation>
    </message>
    <message>
      <source>Timesync I gain</source>
      <translation>时间同步我增益</translation>
    </message>
    <message>
      <source>Tmode action 4</source>
      <translation>T模式动作 4</translation>
    </message>
    <message>
      <source>Tmode second mode</source>
      <translation>Tmode 第二模式</translation>
    </message>
    <message>
      <source>Toggles body frame velocity as an aiding measurement.
The driver uses the body frame velocity from the optical flow sensor as the aiding measurements.</source>
      <translation>切换身体框架速度作为辅助测量。驾驶员使用光流传感器的机体速度作为辅助测量。</translation>
    </message>
    <message>
      <source>Toggles the Video Transmitter on and off</source>
      <translation>打开和关闭视频发射器</translation>
    </message>
    <message>
      <source>Topotek 480p</source>
      <translation>拓泰480p</translation>
    </message>
    <message>
      <source>Torqeedo Options Bitmask</source>
      <translation>Torqeedo 选项位掩码</translation>
    </message>
    <message>
      <source>Torqeedo slew rate specified as the minimum number of seconds required to increase the throttle from 0 to 100%.  Higher values cause a slower response, lower values cause a faster response.  A value of zero disables the limit</source>
      <translation>Torqeedo 转换速率指定为将油门从 0 增加到 100% 所需的最小秒数。  较高的值会导致较慢的响应，较低的值会导致较快的响应。  零值禁用限制</translation>
    </message>
    <message>
      <source>Total flight time of this autopilot. Higher 32 bits of the value.
Flight time in microseconds = (LND_FLIGHT_T_HI &lt;&lt; 32) | LND_FLIGHT_T_LO.</source>
      <translation>该自动驾驶仪的总飞行时间。该值的较高 32 位。飞行时间（微秒）= (LND_FLIGHT_T_HI &lt;&lt; 32) | LND_FLIGHT_T_LO。</translation>
    </message>
    <message>
      <source>Total number of Tilt Servos</source>
      <translation>倾斜舵机总数</translation>
    </message>
    <message>
      <source>Total time it takes to go through all three land detection stages: ground contact, maybe landed, landed when all necessary conditions are constantly met.</source>
      <translation>经过所有三个着陆检测阶段所需的总时间：接触地面、可能着陆、不断满足所有必要条件时着陆。</translation>
    </message>
    <message>
      <source>Tracks horizontal attitude (allows yaw change)</source>
      <translation>跟踪水平姿态（允许偏航变化）</translation>
    </message>
    <message>
      <source>Transceiver RF selection for Rx enable and/or Tx enable. This only effects devices that can Tx and Rx. Rx-only devices override this to always be Rx-only.</source>
      <translation>用于 Rx 启用和/或 Tx 启用的收发器 RF 选择。这仅影响可以 Tx 和 Rx 的设备。仅 Rx 设备会覆盖此设置，使其始终仅 Rx。</translation>
    </message>
    <message>
      <source>Transition time in milliseconds after minimum airspeed is reached</source>
      <translation>达到最小空速后的过渡时间（以毫秒为单位）</translation>
    </message>
    <message>
      <source>Transmitter switch screen minimum pwm</source>
      <translation>发射机开关屏最小pwm</translation>
    </message>
    <message>
      <source>Transverse</source>
      <translation>横向</translation>
    </message>
    <message>
      <source>Trigger time</source>
      <translation>触发时间</translation>
    </message>
    <message>
      <source>Trim PWM</source>
      <translation>微调 PWM</translation>
    </message>
    <message>
      <source>Trimble</source>
      <translation>天宝</translation>
    </message>
    <message>
      <source>Trust DCM more</source>
      <translation>更加信任DCM</translation>
    </message>
    <message>
      <source>Tuning minimum</source>
      <translation>调整最小值</translation>
    </message>
    <message>
      <source>Turn rate during pivot turns</source>
      <translation>枢轴转弯时的转弯速率</translation>
    </message>
    <message>
      <source>Two Paddles Input</source>
      <translation>两个拨片输入</translation>
    </message>
    <message>
      <source>Type of BendyRuler</source>
      <translation>BendyRuler 类型</translation>
    </message>
    <message>
      <source>Type of magnetometer fusion</source>
      <translation>磁力计融合类型</translation>
    </message>
    <message>
      <source>Typically the desired landing slope angle when landing configuration (flaps, airspeed) is enabled. Set this value within the vehicle's performance limits.</source>
      <translation>通常是启用着陆配置（襟翼、空速）时所需的着陆坡度。将此值设置在车辆的性能限制内。</translation>
    </message>
    <message>
      <source>UART ESC Over-Temperature Threshold (Degrees C)</source>
      <translation>UART ESC 过温阈值（摄氏度）</translation>
    </message>
    <message>
      <source>UART ESC Turtle Mode Crash Flip Motor expo</source>
      <translation>UART ESC 海龟模式碰撞翻转电机博览会</translation>
    </message>
    <message>
      <source>UAT_in</source>
      <translation>UAT_输入</translation>
    </message>
    <message>
      <source>UAVCAN ESC 1 Maximum Value</source>
      <translation>UAVCAN ESC 1 最大值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 3</source>
      <translation>UAVCAN ESC 3</translation>
    </message>
    <message>
      <source>UAVCAN ESC 4 Minimum Value</source>
      <translation>UAVCAN ESC 4 最小值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 6 Failsafe Value</source>
      <translation>UAVCAN ESC 6 故障安全值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 7 Output Function</source>
      <translation>UAVCAN ESC 7 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN RIGHT_OF_WAY light operating mode</source>
      <translation>UAVCAN RIGHT_OF_WAY 轻型工作模式</translation>
    </message>
    <message>
      <source>UAVCAN Servo 2</source>
      <translation>UAVCAN 舵机 2</translation>
    </message>
    <message>
      <source>UAVCAN Servo 3 Failsafe Value</source>
      <translation>UAVCAN 舵机 3 故障安全值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 4 Minimum Value</source>
      <translation>UAVCAN 舵机 4 最小值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 6</source>
      <translation>UAVCAN 舵机 6</translation>
    </message>
    <message>
      <source>UAVCAN Servo 7 Failsafe Value</source>
      <translation>UAVCAN 舵机 7 故障安全值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 8 Minimum Value</source>
      <translation>UAVCAN 舵机 8 最小值</translation>
    </message>
    <message>
      <source>UAVCAN options</source>
      <translation>UAVCAN 选项</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 1 Minimum Value</source>
      <translation>UAVCANv1 ESC 1 最小值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 11 Failsafe Value</source>
      <translation>UAVCANv1 ESC 11 故障保护值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 12 Output Function</source>
      <translation>UAVCANv1 ESC 12 输出功能</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 14 Maximum Value</source>
      <translation>UAVCANv1 ESC 14 最大值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 16</source>
      <translation>UAVCANv1 ESC 16</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 2 Minimum Value</source>
      <translation>UAVCANv1 ESC 2 最小值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 4 Failsafe Value</source>
      <translation>UAVCANv1 ESC 4 故障安全值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 5 Output Function</source>
      <translation>UAVCANv1 ESC 5 输出功能</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 7 Maximum Value</source>
      <translation>UAVCANv1 ESC 7 最大值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 9</source>
      <translation>UAVCANv1 ESC 9</translation>
    </message>
    <message>
      <source>UDP echo test</source>
      <translation>UDP回波测试</translation>
    </message>
    <message>
      <source>USD1_CAN</source>
      <translation>USD1_CAN</translation>
    </message>
    <message>
      <source>UWB sensor Z offset in body frame</source>
      <translation>UWB 传感器在机体框架中的 Z 偏移</translation>
    </message>
    <message>
      <source>Uncontrolled</source>
      <translation>不受控制</translation>
    </message>
    <message>
      <source>UnknownMaxSpeed</source>
      <translation>未知最大速度</translation>
    </message>
    <message>
      <source>Update rate in Hz</source>
      <translation>更新率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Upside down</source>
      <translation>倒置</translation>
    </message>
    <message>
      <source>Use FBWA steering in AUTO</source>
      <translation>在AUTO中使用FBWA转向</translation>
    </message>
    <message>
      <source>Use Link Quality for RSSI with CRSF</source>
      <translation>使用 RSSI 和 CRSF 的链路质量</translation>
    </message>
    <message>
      <source>Use Range Beacon</source>
      <translation>使用范围信标</translation>
    </message>
    <message>
      <source>Use baudrate 115200</source>
      <translation>使用波特率115200</translation>
    </message>
    <message>
      <source>Use ellipsoid height instead of AMSL for uBlox driver</source>
      <translation>对于 uBlox 驱动程序，使用椭球体高度而不是 AMSL</translation>
    </message>
    <message>
      <source>Use in RTL and SURFACE</source>
      <translation>用于RTL和SURFACE</translation>
    </message>
    <message>
      <source>Use output driver settings to calibrate (e.g. PWM_CENT/_MIN/_MAX). Note that only symmetric angular ranges are supported.</source>
      <translation>使用输出驱动器设置进行校准（例如 PWM_CENT/_MIN/_MAX）。请注意，仅支持对称角度范围。</translation>
    </message>
    <message>
      <source>Use safety pins as profiled</source>
      <translation>按照规定使用安全别针</translation>
    </message>
    <message>
      <source>Use when no angular position feedback is available.
With MNT_MODE_OUT set to AUX, the mount operates in open-loop and directly commands the servo output.
Parameters must be tuned for the specific servo to approximate its speed and response.</source>
      <translation>当没有可用的角度位置反馈时使用。当 MNT_MODE_OUT 设置为 AUX 时，安装座以开环方式运行并直接命令舵机输出。必须针对特定舵机器调整参数以接近其速度和响应。</translation>
    </message>
    <message>
      <source>UseFence</source>
      <translation>使用围栏</translation>
    </message>
    <message>
      <source>UseSecond</source>
      <translation>使用第二</translation>
    </message>
    <message>
      <source>Used by Throw mode. Specifies whether Copter is thrown upward or dropped.</source>
      <translation>由投掷模式使用。指定直升机是向上抛出还是掉落。</translation>
    </message>
    <message>
      <source>Used to calculate back transition distance in an auto mode.
For standard vtol and tiltrotors a controller is used to track this value during the transition.</source>
      <translation>用于计算自动模式下的向后过渡距离。对于标准垂直起降和倾转旋翼机，控制器用于在过渡期间跟踪该值。</translation>
    </message>
    <message>
      <source>Used to cap how quickly the magnitude of yaw rate setpoints can decrease.
Set to -1 to disable.</source>
      <translation>用于限制偏航率设定值下降的速度。设置为 -1 以禁用。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT2_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATT2_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT5_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT). For the 3DR Power brick with a Pixhawk, this should be set to 10.1. For the Pixhawk with the 3DR 4in1 ESC this should be 12.02. For the PX using the PX4IO power supply this should be set to 1.</source>
      <translation>用于将电压检测引脚（BATT5_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。对于带有 Pixhawk 的 3DR 电源块，应设置为 10.1。对于配备 3DR 4in1 ESC 的 Pixhawk，这应该是 12.02。对于使用 PX4IO 电源的 PX，应设置为 1。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT9_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT). For the 3DR Power brick with a Pixhawk, this should be set to 10.1. For the Pixhawk with the 3DR 4in1 ESC this should be 12.02. For the PX using the PX4IO power supply this should be set to 1.</source>
      <translation>用于将电压检测引脚（BATT9_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。对于带有 Pixhawk 的 3DR 电源块，应设置为 10.1。对于配备 3DR 4in1 ESC 的 Pixhawk，这应该是 12.02。对于使用 PX4IO 电源的 PX，应设置为 1。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATTD_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT). For the 3DR Power brick with a Pixhawk, this should be set to 10.1. For the Pixhawk with the 3DR 4in1 ESC this should be 12.02. For the PX using the PX4IO power supply this should be set to 1.</source>
      <translation>用于将电压检测引脚（BATTD_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。对于带有 Pixhawk 的 3DR 电源块，应设置为 10.1。对于配备 3DR 4in1 ESC 的 Pixhawk，这应该是 12.02。对于使用 PX4IO 电源的 PX，应设置为 1。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT). For the 3DR Power brick on APM2 or Pixhawk, this should be set to 10.1. For the Pixhawk with the 3DR 4in1 ESC this should be 12.02. For the PX4 using the PX4IO power supply this should be set to 1.</source>
      <translation>用于将电压检测引脚（BATT_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。对于 APM2 或 Pixhawk 上的 3DR 电源块，应将其设置为 10.1。对于配备 3DR 4in1 ESC 的 Pixhawk，这应该是 12.02。对于使用 PX4IO 电源的 PX4，应设置为 1。</translation>
    </message>
    <message>
      <source>User Flight Profile</source>
      <translation>用户飞行资料</translation>
    </message>
    <message>
      <source>User-defined serial number</source>
      <translation>用户自定义序列号</translation>
    </message>
    <message>
      <source>V5 Nano</source>
      <translation>V5纳米</translation>
    </message>
    <message>
      <source>VL53L0X or VL53L1X</source>
      <translation>VL53L0X 或 VL53L1X</translation>
    </message>
    <message>
      <source>VOXL ESC Output ESC 3 Output Function</source>
      <translation>VOXL ESC 输出 ESC 3 输出功能</translation>
    </message>
    <message>
      <source>VOXL2 IO Output PWM Channel 2</source>
      <translation>VOXL2 IO 输出 PWM 通道 2</translation>
    </message>
    <message>
      <source>VOXL2 IO Output PWM Channel 6</source>
      <translation>VOXL2 IO 输出 PWM 通道 6</translation>
    </message>
    <message>
      <source>VOXL2_IO Disabled PWM</source>
      <translation>VOXL2_IO 禁用 PWM</translation>
    </message>
    <message>
      <source>VRBrain 5.4</source>
      <translation>VRBrain 5.4</translation>
    </message>
    <message>
      <source>VRX UBRAIN52</source>
      <translation>VRX UBRAIN52</translation>
    </message>
    <message>
      <source>VTOL Land</source>
      <translation>VTOL着陆</translation>
    </message>
    <message>
      <source>VTOL Rate Roll</source>
      <translation>VTOL 速率横滚</translation>
    </message>
    <message>
      <source>VTOL Takeoff relative loiter altitude</source>
      <translation>VTOL 起飞相对盘旋高度</translation>
    </message>
    <message>
      <source>VTOL transition switch channel mapping</source>
      <translation>VTOL 转换开关通道映射</translation>
    </message>
    <message>
      <source>VTailLeft</source>
      <translation>V尾左</translation>
    </message>
    <message>
      <source>Variance of acceleration measurement used for landing target position prediction.
Higher values results in tighter following of the measurements and more lenient outlier rejection</source>
      <translation>用于着陆目标位置预测的加速度测量的方差。较高的值会导致更严格的测量跟踪和更宽松的异常值拒绝</translation>
    </message>
    <message>
      <source>VectorNav</source>
      <translation>矢量导航</translation>
    </message>
    <message>
      <source>Vehicle cross term inertia xy</source>
      <translation>车辆交叉项惯量 xy</translation>
    </message>
    <message>
      <source>Vehicle should not follow itself</source>
      <translation>车辆不应跟随自己</translation>
    </message>
    <message>
      <source>Vehicle will continue landing vertically until this height if target is not found. Below this height if landing target is not found, landing retry/failsafe might be attempted. This needs a rangefinder to work. Set to zero to disable this.</source>
      <translation>如果没有找到目标，飞行器将继续垂直着陆直到此高度。低于此高度，如果未找到着陆目标，可能会尝试着陆重试/故障保护。这需要测距仪才能工作。设置为零以禁用此功能。</translation>
    </message>
    <message>
      <source>Velocity</source>
      <translation>速度</translation>
    </message>
    <message>
      <source>Velocity (horizontal) P gain</source>
      <translation>速度（水平）P 增益</translation>
    </message>
    <message>
      <source>Velocity (horizontal) feed forward gain.  Converts the difference between desired velocity to a target acceleration</source>
      <translation>速度（水平）前馈增益。  将所需速度之间的差值转换为目标加速度</translation>
    </message>
    <message>
      <source>Velocity (horizontal) input filter. This filter (in Hz) is applied to the input for P and I terms. Previously _VELXY_FLTE.</source>
      <translation>速度（水平）输入滤波器。该滤波器（以 Hz 为单位）应用于 P 和 I 项的输入。以前是_VELXY_FLTE。</translation>
    </message>
    <message>
      <source>Velocity (vertical) controller D gain. Corrects short-term changes in velocity. Previously _VELZ_D.</source>
      <translation>速度（垂直）控制器 D 增益。纠正速度的短期变化。以前_VELZ_D。</translation>
    </message>
    <message>
      <source>Velocity (vertical) controller I gain.  Corrects long-term difference in desired velocity to a target acceleration</source>
      <translation>速度（垂直）控制器 I 增益。  将所需速度的长期差异校正为目标加速度</translation>
    </message>
    <message>
      <source>Velocity (vertical) input filter for D term</source>
      <translation>D 项的速度（垂直）输入滤波器</translation>
    </message>
    <message>
      <source>Velocity Vertical Source</source>
      <translation>速度垂直源</translation>
    </message>
    <message>
      <source>Velocity low pass cutoff frequency</source>
      <translation>速度低通截止频率</translation>
    </message>
    <message>
      <source>VerboseSignalInfoGCS</source>
      <translation>详细信号信息GCS</translation>
    </message>
    <message>
      <source>Vertical offsets can be compensated for by adjusting the Pitch offset. Note that this can be interpreted as the "roll" angle in case the antennas are aligned along the perpendicular axis. This occurs in situations where the two antenna ARPs may not be exactly at the same height in the vehicle reference frame. Since pitch is defined as the right-handed rotation about the vehicle Y axis, a situation where the main antenna is mounted lower than the aux antenna (assuming the default antenna setup) will result in a positive pitch.</source>
      <translation>垂直偏移可以通过调整俯仰偏移来补偿。请注意，如果天线沿垂直轴对齐，这可以解释为“横滚”角。这种情况发生在两个天线 ARP 在车辆参考系中可能不完全相同的高度的情况下。由于俯仰被定义为绕车辆 Y 轴的右手旋转，因此主天线安装得低于辅助天线的情况（假设默认天线设置）将导致正俯仰。</translation>
    </message>
    <message>
      <source>Vertical thrust required to hover</source>
      <translation>悬停所需的垂直推力</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 0</source>
      <translation>Vertiq IO CVI 0</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 1 Failsafe Value</source>
      <translation>Vertiq IO CVI 1 故障安全值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 10 Minimum Value</source>
      <translation>Vertiq IO CVI 10 最小值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 12</source>
      <translation>Vertiq IO CVI 12</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 13 Failsafe Value</source>
      <translation>Vertiq IO CVI 13 故障安全值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 14 Minimum Value</source>
      <translation>Vertiq IO CVI 14 最小值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 2</source>
      <translation>Vertiq IO CVI 2</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 3 Failsafe Value</source>
      <translation>Vertiq IO CVI 3 故障安全值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 4 Minimum Value</source>
      <translation>Vertiq IO CVI 4 最小值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 6</source>
      <translation>Vertiq IO CVI 6</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 7 Failsafe Value</source>
      <translation>Vertiq IO CVI 7 故障安全值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 8 Minimum Value</source>
      <translation>Vertiq IO CVI 8 最小值</translation>
    </message>
    <message>
      <source>Very Crisp</source>
      <translation>非常脆</translation>
    </message>
    <message>
      <source>Vibration frequency</source>
      <translation>振动频率</translation>
    </message>
    <message>
      <source>Video Transmitter Frequency</source>
      <translation>视频发射频率</translation>
    </message>
    <message>
      <source>Video Transmitter Power Level. Different VTXs support different power levels, the power level chosen will be rounded down to the nearest supported power level</source>
      <translation>视频发射器功率级别。不同的VTX支持不同的功率级别，选择的功率级别将向下舍入到最接近的支持功率级别</translation>
    </message>
    <message>
      <source>Video stream encoding</source>
      <translation>视频流编码</translation>
    </message>
    <message>
      <source>VideoStabilization</source>
      <translation>视频稳定</translation>
    </message>
    <message>
      <source>ViewPro debug</source>
      <translation>ViewPro调试</translation>
    </message>
    <message>
      <source>Viso Align</source>
      <translation>视觉对齐</translation>
    </message>
    <message>
      <source>Visual odometry minimum quality</source>
      <translation>视觉里程计最低质量</translation>
    </message>
    <message>
      <source>Visual odometry sensor delay relative to inertial measurements</source>
      <translation>视觉里程计传感器相对于惯性测量的延迟</translation>
    </message>
    <message>
      <source>Voltage</source>
      <translation>电压</translation>
    </message>
    <message>
      <source>Voltage offset on voltage pin. This allows for an offset due to a diode. This voltage is subtracted before the scaling is applied</source>
      <translation>电压引脚上的电压偏移。这允许由于二极管而产生偏移。在应用缩放之前减去该电压</translation>
    </message>
    <message>
      <source>Vtol Takeoff Frame</source>
      <translation>垂直起降起飞架</translation>
    </message>
    <message>
      <source>WARNING: this will inject steps to the rate controller
and can be dangerous. Only activate if you know what you
are doing, and in a safe environment.
Any motion of the remote stick will abort the signal
injection and reset this parameter
Best is to perform the identification in position or
hold mode.</source>
      <translation>WARNING：这会将步骤注入速率控制器，并且可能很危险。仅当您知道自己在做什么并且在安全的环境中时才激活。遥控杆的任何运动都会中止信号注入并重置此参数。最好是在位置或保持模式下执行识别。</translation>
    </message>
    <message>
      <source>WIND_EN</source>
      <translation>WIND_EN</translation>
    </message>
    <message>
      <source>WaitForPilotInput</source>
      <translation>等待引导输入</translation>
    </message>
    <message>
      <source>Warn only</source>
      <translation>仅警告</translation>
    </message>
    <message>
      <source>Wave length</source>
      <translation>波长</translation>
    </message>
    <message>
      <source>Waypoint Descent Speed Target</source>
      <translation>航路点下降速度目标</translation>
    </message>
    <message>
      <source>Waypoint Terrain following altitude margin</source>
      <translation>航点地形遵循高度裕度</translation>
    </message>
    <message>
      <source>Waypoint navigation altitude minimum</source>
      <translation>航路点最低导航高度</translation>
    </message>
    <message>
      <source>Waypoint speed minimum</source>
      <translation>航路点速度最小值</translation>
    </message>
    <message>
      <source>Weathervaning min roll</source>
      <translation>风标最小横滚</translation>
    </message>
    <message>
      <source>What action the vehicle should perform if it hits a critical battery failsafe</source>
      <translation>如果车辆遇到严重的电池故障保护，车辆应该采取什么行动</translation>
    </message>
    <message>
      <source>What type of RPM sensor is connected</source>
      <translation>连接什么类型的 RPM 传感器</translation>
    </message>
    <message>
      <source>Wheel odometry velocity error</source>
      <translation>车轮里程计速度误差</translation>
    </message>
    <message>
      <source>Wheel rate control D gain.  Compensates for short-term change in desired rate vs actual</source>
      <translation>轮速控制 D 增益。  补偿期望利率与实际利率的短期变化</translation>
    </message>
    <message>
      <source>Wheel rate control PD sum maximum.  The maximum/minimum value that the sum of the P and D term can output</source>
      <translation>轮速控制 PD 总和最大值。  P和D项之和可以输出的最大/最小值</translation>
    </message>
    <message>
      <source>Wheel rate control target frequency in Hz</source>
      <translation>轮速率控制目标频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Wheel's Y position offset</source>
      <translation>车轮 Y 位置偏移</translation>
    </message>
    <message>
      <source>WheelEncoder 2 counts per revolution</source>
      <translation>WheelEncoder 2 每转计数</translation>
    </message>
    <message>
      <source>When bit 0 is set disable mode change following fence action until fence breach is cleared. When bit 1 is set the allowable flight areas is the union of all polygon and circle fence areas instead of the intersection, which means a fence breach occurs only if you are outside all of the fence areas.</source>
      <translation>当设置位 0 时，将禁用栅栏操作后的模式更改，直到清除栅栏违规为止。当位 1 被设置时，允许的飞行区域是所有多边形和圆形围栏区域的并集而不是交集，这意味着仅当您位于所有围栏区域之外时才会发生围栏突破。</translation>
    </message>
    <message>
      <source>When enabled, ROS 2 topic and service names include a v&lt;MAV_SYSID&gt; segment</source>
      <translation>启用后，ROS 2 主题和服务名称包含 v&lt;MAV_SYSID&gt; 段</translation>
    </message>
    <message>
      <source>When enabled, this adds steering user stick input in auto modes, allowing the user to have some degree of control without changing modes.</source>
      <translation>启用后，这会在自动模式下添加转向用户摇杆输入，允许用户在不更改模式的情况下进行一定程度的控制。</translation>
    </message>
    <message>
      <source>When enabled, uxrce_dds_client will synchronize the timestamps of the incoming and outgoing messages measuring the offset between the Agent OS time and the PX4 time.</source>
      <translation>启用后，uxrce_dds_client 将同步传入和传出消息的时间戳，测量代理 OS 时间和 PX4 时间之间的偏移量。</translation>
    </message>
    <message>
      <source>When in auto sail trim modes the heel will be limited to this value using PID control</source>
      <translation>当处于自动航行配平模式时，后跟将使用 PID 控制限制为该值</translation>
    </message>
    <message>
      <source>When non zero, this altitude value (in FOLL_ALT_TYPE frame) overrides the value sent by the target vehicle</source>
      <translation>当非零时，该高度值（在 FOLL_ALT_TYPE 帧中）将覆盖目标车辆发送的值</translation>
    </message>
    <message>
      <source>When performing an autonomous landing, this value is used as the goal airspeed during approach.  Max airspeed allowed is AIRSPEED_CRUISE or AIRSPEED_MAX as defined by LAND_OPTIONS bitmask.  Note that this parameter is not useful if your platform does not have an airspeed sensor (use TECS_LAND_THR instead).  If negative then this value is halfway between AIRSPEED_MIN and AIRSPEED_CRUISE speed for fixed wing autolandings.</source>
      <translation>执行自主着陆时，该值用作进场时的目标空速。  允许的最大空速为 AIRSPEED_CRUISE 或 AIRSPEED_MAX，由 LAND_OPTIONS 位掩码定义。  请注意，如果您的平台没有空速传感器（请改用 TECS_LAND_THR），则此参数没有用处。  如果为负值，则该值是固定翼自动着陆的 AIRSPEED_MIN 和 AIRSPEED_CRUISE 速度之间的中间值。</translation>
    </message>
    <message>
      <source>When set to 0 (default), default DGNSS timeout set by u-blox will be used.</source>
      <translation>当设置为 0（默认）时，将使用 u-blox 设置的默认 DGNSS 超时。</translation>
    </message>
    <message>
      <source>When set to a non-zero value, this is the assumed direction of the mission. Otherwise the waypoint angle is used</source>
      <translation>当设置为非零值时，这是任务的假定方向。否则使用航路点角度</translation>
    </message>
    <message>
      <source>When the throw launch is enabled, the drone will only allow motors to spin after this speed
is exceeded before detecting the freefall. This is a safety feature to ensure the drone does
not turn on after accidental drop or a rapid movement before the throw.
Set to 0 to disable.</source>
      <translation>启用投掷发射后，无人机将仅在超过此速度后才允许电机旋转，然后才会检测到自由落体。这是一项安全功能，可确保无人机在意外跌落或投掷前快速移动后不会启动。设置为 0 以禁用。</translation>
    </message>
    <message>
      <source>When unset the maximum allowed throttle is always used (THR_MAX or TKOFF_THR_MAX) during takeoff. When set TECS is allowed to operate between a minimum (THR_MIN or TKOFF_THR_MIN) and a maximum (THR_MAX or TKOFF_THR_MAX) limit. Applicable only when using an airspeed sensor.</source>
      <translation>未设置时，起飞期间始终使用最大允许油门（THR_MAX 或 TKOFF_THR_MAX）。设置后，允许 TECS 在最小（THR_MIN 或 TKOFF_THR_MIN）和最大（THR_MAX 或 TKOFF_THR_MAX）限制之间运行。仅在使用空速传感器时适用。</translation>
    </message>
    <message>
      <source>Which battery monitor should be used for doing compensation</source>
      <translation>应该用哪款电池监测仪来做补偿</translation>
    </message>
    <message>
      <source>WhiteHot</source>
      <translation>白热化</translation>
    </message>
    <message>
      <source>Winch Rate Control RC function</source>
      <translation>绞车速率控制 RC 功能</translation>
    </message>
    <message>
      <source>Winch control filter frequency</source>
      <translation>绞车控制滤波器频率</translation>
    </message>
    <message>
      <source>Winch options</source>
      <translation>绞车选项</translation>
    </message>
    <message>
      <source>Wind coefficient down</source>
      <translation>风系数下降</translation>
    </message>
    <message>
      <source>Wind estimator true airspeed scale process noise spectral density</source>
      <translation>风估算器真实空速标度过程噪声谱密度</translation>
    </message>
    <message>
      <source>Wind speed sensor Type</source>
      <translation>风速传感器类型</translation>
    </message>
    <message>
      <source>Wind vane calibration start</source>
      <translation>风向标校准开始</translation>
    </message>
    <message>
      <source>Wind vane speed sensor analog pin</source>
      <translation>风向标速度传感器模拟引脚</translation>
    </message>
    <message>
      <source>Wind-based airspeed scaling factor</source>
      <translation>基于风的空速比例因子</translation>
    </message>
    <message>
      <source>Write data to a file</source>
      <translation>将数据写入文件</translation>
    </message>
    <message>
      <source>X position of GPS antenna in body frame</source>
      <translation>GPS天线在机体框架中的X位置</translation>
    </message>
    <message>
      <source>X position of the camera in body frame. Positive X is forward of the origin.</source>
      <translation>相机在机身框架中的 X 位置。正 X 位于原点前方。</translation>
    </message>
    <message>
      <source>X position of the optical flow sensor focal point in body frame. Positive X is forward of the origin.</source>
      <translation>光流传感器焦点在机身框架中的 X 位置。正 X 位于原点前方。</translation>
    </message>
    <message>
      <source>X position offset</source>
      <translation>X 位置偏移</translation>
    </message>
    <message>
      <source>XTRACK_Y</source>
      <translation>XTRACK_Y</translation>
    </message>
    <message>
      <source>XYZ position of the sonar relative to the body frame origin (X-axis)</source>
      <translation>XYZ 声纳相对于机体框架原点（X轴）的位置</translation>
    </message>
    <message>
      <source>Y position of IMU in body frame</source>
      <translation>IMU在机体框架中的Y位置</translation>
    </message>
    <message>
      <source>Y position of the center of the second wheel in body frame. Positive Y is to the right of the origin.</source>
      <translation>机体框架中第二轮中心的 Y 位置。正 Y 位于原点右侧。</translation>
    </message>
    <message>
      <source>Y position of the rangefinder in body frame. Positive Y is to the right of the origin. Use the zero range datum point if supplied.</source>
      <translation>测距仪在机身框架中的 Y 位置。正 Y 位于原点右侧。使用零范围基准点（如果提供）。</translation>
    </message>
    <message>
      <source>Y-axis ballistic coefficient used for multi-rotor wind estimation</source>
      <translation>用于多旋翼风估计的Y轴弹道系数</translation>
    </message>
    <message>
      <source>Yaw 225°</source>
      <translation>偏航225°</translation>
    </message>
    <message>
      <source>Yaw Error notch filter index, zero disables</source>
      <translation>偏航误差陷波滤波器索引，零禁用</translation>
    </message>
    <message>
      <source>Yaw Target notch filter index</source>
      <translation>偏航目标陷波滤波器指数</translation>
    </message>
    <message>
      <source>Yaw axis controller D gain</source>
      <translation>偏航轴控制器D增益</translation>
    </message>
    <message>
      <source>Yaw axis controller PD sum maximum</source>
      <translation>偏航轴控制器PD和最大</translation>
    </message>
    <message>
      <source>Yaw axis rate controller D gain.  Compensates for short-term change in desired yaw rate vs actual yaw rate</source>
      <translation>偏航轴速率控制器 D 增益。  补偿所需偏航率与实际偏航率的短期变化</translation>
    </message>
    <message>
      <source>Yaw axis rate controller P gain.  Converts the difference between desired yaw rate and actual yaw rate into a motor speed output</source>
      <translation>偏航轴速率控制器 P 增益。  将期望偏航率与实际偏航率之间的差值转换为电机速度输出</translation>
    </message>
    <message>
      <source>Yaw axis rate controller target frequency in Hz</source>
      <translation>偏航轴速率控制器目标频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Yaw control is given at least this pwm range</source>
      <translation>偏航控制至少给出这个 pwm 范围</translation>
    </message>
    <message>
      <source>Yaw gain for manual inputs in attitude control mode</source>
      <translation>姿态控制模式下手动输入的偏航增益</translation>
    </message>
    <message>
      <source>Yaw offset applied to SIM AHRS type. For testing stepless handover between AHRS estimators.</source>
      <translation>偏航偏移适用于 SIM AHRS 型。用于测试 AHRS 估计器之间的无级切换。</translation>
    </message>
    <message>
      <source>Yaw rate differential gain. Small values help reduce fast oscillations. If value is too big oscillations will appear again.</source>
      <translation>偏航率微分增益。小值有助于减少快速振荡。如果值太大，将会再次出现振荡。</translation>
    </message>
    <message>
      <source>Yaw rate integrator limit</source>
      <translation>偏航率积分器限制</translation>
    </message>
    <message>
      <source>Yaw reverser expo</source>
      <translation>偏航反向器博览会</translation>
    </message>
    <message>
      <source>Yaw trim increment at maximum airspeed</source>
      <translation>最大空速下的偏航配平增量</translation>
    </message>
    <message>
      <source>Yaw225</source>
      <translation>偏航225</translation>
    </message>
    <message>
      <source>Yaw315</source>
      <translation>偏航315</translation>
    </message>
    <message>
      <source>Yaw90Pitch180Roll90</source>
      <translation>偏航90俯仰180滚动90</translation>
    </message>
    <message>
      <source>Yes(minimum PWM when disarmed)</source>
      <translation>是（上锁时最小 PWM）</translation>
    </message>
    <message>
      <source>Z position of the base (primary) GPS antenna in body frame from the position of the 2nd antenna. Positive Z is down from the 2nd antenna. Use antenna phase centroid location if provided by the manufacturer.This is renamed in 4.6 and later to GPSx_MB_OFS_Z.</source>
      <translation>机身框架中底座（主）GPS 天线的 Z 位置（距第二根天线的位置）。正 Z 轴位于第二根天线下方。如果制造商提供，请使用天线相位质心位置。这在 4.6 中重命名，后来更名为 GPSx_MB_OFS_Z。</translation>
    </message>
    <message>
      <source>Z position of the first rangefinder in body frame. Positive Z is down from the origin. Use the zero range datum point if supplied.</source>
      <translation>机身框架中第一个测距仪的 Z 位置。正 Z 轴从原点向下。使用零范围基准点（如果提供）。</translation>
    </message>
    <message>
      <source>Z position of the third IMU accelerometer in body frame. Positive Z is down from the origin. Attention: The IMU should be located as close to the vehicle c.g. as practical so that the value of this parameter is minimised. Failure to do so can result in noisy navigation velocity measurements due to vibration and IMU gyro noise. If the IMU cannot be moved and velocity noise is a problem, a location closer to the IMU can be used as the body frame origin.</source>
      <translation>机体框架中第三个 IMU 加速度计的 Z 位置。正 Z 轴从原点向下。注意：IMU 应尽可能靠近车辆 c.g.。根据实际情况，使该参数的值最小化。否则，可能会因振动和 IMU 陀螺仪噪声而导致导航速度测量产生噪声。如果 IMU 无法移动并且速度噪声成为问题，则可以使用更靠近 IMU 的位置作为机体框架原点。</translation>
    </message>
    <message>
      <source>ZigZag Auto</source>
      <translation>自动之字折线</translation>
    </message>
    <message>
      <source>a1 in polynomial of form temperature in deg = a0 + a1*voltage + a2*voltage^2 + a3*voltage^3 + a4*voltage^4 + a5*voltage^5</source>
      <translation>a1 以度为单位的温度多项式 = a0 + a1*电压 + a2*电压^2 + a3*电压^3 + a4*电压^4 + a5*电压^5</translation>
    </message>
    <message>
      <source>absolute</source>
      <translation>绝对的</translation>
    </message>
    <message>
      <source>actuator_1_min_momentary</source>
      <translation>Actuator_1_min_momentary</translation>
    </message>
    <message>
      <source>actuator_2_min</source>
      <translation>执行器_2_min</translation>
    </message>
    <message>
      <source>actuator_3_max_toggle</source>
      <translation>Actuator_3_max_toggle</translation>
    </message>
    <message>
      <source>actuator_4_max_momentary</source>
      <translation>Actuator_4_max_momentary</translation>
    </message>
    <message>
      <source>actuator_5_max</source>
      <translation>执行器_5_max</translation>
    </message>
    <message>
      <source>actuator_6_inc</source>
      <translation>执行器_6_inc</translation>
    </message>
    <message>
      <source>airborne with &lt;1g acceleration</source>
      <translation>空中加速度&lt;1g</translation>
    </message>
    <message>
      <source>altitude target</source>
      <translation>高度目标</translation>
    </message>
    <message>
      <source>arm length</source>
      <translation>臂长</translation>
    </message>
    <message>
      <source>balloon burst height</source>
      <translation>气球爆破高度</translation>
    </message>
    <message>
      <source>battery voltage in the drag reference test</source>
      <translation>拖动参考测试中的电池电压</translation>
    </message>
    <message>
      <source>calibrated ground temperature in degrees Celsius</source>
      <translation>校准地面温度（摄氏度）</translation>
    </message>
    <message>
      <source>circuit_id of the CircuitStatus message to use for this instance. Set to -1 to disable.</source>
      <translation>用于此实例的 CircuitStatus 消息的 Circuit_id。设置为 -1 以禁用。</translation>
    </message>
    <message>
      <source>custom_4</source>
      <translation>定制_4</translation>
    </message>
    <message>
      <source>desired landing position of the camera right of the target in vehicle body frame</source>
      <translation>摄像机期望的着陆位置在机体框架中目标的右侧</translation>
    </message>
    <message>
      <source>distance sensor maximum range</source>
      <translation>距离传感器最大范围</translation>
    </message>
    <message>
      <source>drag on Y axis</source>
      <translation>在 Y 轴上拖动</translation>
    </message>
    <message>
      <source>enable roll stabilisation relative to Earth</source>
      <translation>启用相对于地球的横滚稳定性</translation>
    </message>
    <message>
      <source>enter_mode_5</source>
      <translation>输入模式_5</translation>
    </message>
    <message>
      <source>field elevation</source>
      <translation>场高程</translation>
    </message>
    <message>
      <source>from boot until shutdown</source>
      <translation>从开机到关机</translation>
    </message>
    <message>
      <source>fuse vision position</source>
      <translation>熔断器视觉位置</translation>
    </message>
    <message>
      <source>helium mass</source>
      <translation>氦质量</translation>
    </message>
    <message>
      <source>if &gt;= 0 the distance sensor measures will be overridden by this value</source>
      <translation>如果 &gt;= 0，距离传感器测量值将被该值覆盖</translation>
    </message>
    <message>
      <source>left</source>
      <translation>左</translation>
    </message>
    <message>
      <source>log Dijkstra points</source>
      <translation>对数 Dijkstra 点</translation>
    </message>
    <message>
      <source>loop-rate</source>
      <translation>循环率</translation>
    </message>
    <message>
      <source>mass of lifting gas</source>
      <translation>提升气体质量</translation>
    </message>
    <message>
      <source>maximum PWM pulse width. Typically 1000 is lower limit, 1500 is neutral and 2000 is upper limit.</source>
      <translation>最大 PWM 脉冲宽度。通常，1000 为下限，1500 为中性，2000 为上限。</translation>
    </message>
    <message>
      <source>maximum motor tilt angle</source>
      <translation>电机最大倾斜角度</translation>
    </message>
    <message>
      <source>minimum PWM when disarmed</source>
      <translation>上锁时最小 PWM</translation>
    </message>
    <message>
      <source>mode_depth_hold</source>
      <translation>模式深度保持</translation>
    </message>
    <message>
      <source>model mass</source>
      <translation>模型质量</translation>
    </message>
    <message>
      <source>monitor 3</source>
      <translation>监控3</translation>
    </message>
    <message>
      <source>motor slew rate</source>
      <translation>电机转换速率</translation>
    </message>
    <message>
      <source>mount2_roll</source>
      <translation>挂载2_roll</translation>
    </message>
    <message>
      <source>mount_tilt</source>
      <translation>倾斜安装</translation>
    </message>
    <message>
      <source>one arm</source>
      <translation>一只手臂</translation>
    </message>
    <message>
      <source>payload's position estimator's time constant used to compensate for GPS errors and wind.  Higher values result in smoother estimate but slower response</source>
      <translation>有效负载位置估计器的时间常数用于补偿 GPS 误差和风。  值越高，估计越平滑，但响应速度越慢</translation>
    </message>
    <message>
      <source>pitch</source>
      <translation>俯仰</translation>
    </message>
    <message>
      <source>publish moving baseline data RTCM stream</source>
      <translation>发布移动基线数据RTCM流</translation>
    </message>
    <message>
      <source>rate at which roll angle returns to level in acro and sport mode.  A higher value causes the vehicle to return to level faster. For helicopter sets the decay rate of the virtual flybar in the roll axis. A higher value causes faster decay of desired to actual attitude.</source>
      <translation>在特技和运动模式下横滚角恢复到水平的速率。  值越高，车辆返回水平的速度越快。对于直升机，设置虚拟飞行杆在横滚轴上的衰减率。较高的值会导致期望态度到实际态度的更快衰减。</translation>
    </message>
    <message>
      <source>relative</source>
      <translation>亲戚</translation>
    </message>
    <message>
      <source>relay_2_toggle</source>
      <translation>继电器_2_切换</translation>
    </message>
    <message>
      <source>relay_4_toggle</source>
      <translation>继电器_4_切换</translation>
    </message>
    <message>
      <source>roll_pitch_toggle</source>
      <translation>滚动俯仰切换</translation>
    </message>
    <message>
      <source>sbgECom driver INS configuration enable</source>
      <translation>sbgECom 驱动程序 INS 配置使能</translation>
    </message>
    <message>
      <source>scaling factors applied to simulated gyroscope (Y-axis)</source>
      <translation>应用于模拟陀螺仪的缩放因子（Y 轴）</translation>
    </message>
    <message>
      <source>send to 2nd GPS</source>
      <translation>发送至第二个GPS</translation>
    </message>
    <message>
      <source>servo speed</source>
      <translation>舵机速度</translation>
    </message>
    <message>
      <source>servo_1_min</source>
      <translation>舵机_1_min</translation>
    </message>
    <message>
      <source>servo_2_max_toggle</source>
      <translation>舵机_2_max_toggle</translation>
    </message>
    <message>
      <source>servo_3_max_momentary</source>
      <translation>舵机_3_max_momentary</translation>
    </message>
    <message>
      <source>sh1106</source>
      <translation>s1106</translation>
    </message>
    <message>
      <source>simulated magnetometer Z offset</source>
      <translation>模拟磁力计 Z 偏移</translation>
    </message>
    <message>
      <source>standalone attitude estimator enable (unsupported)</source>
      <translation>独立姿态估计器启用（不支持）</translation>
    </message>
    <message>
      <source>subscription battery</source>
      <translation>订阅电池</translation>
    </message>
    <message>
      <source>sum speed of error for integrational gain</source>
      <translation>积分增益的误差总和速度</translation>
    </message>
    <message>
      <source>terrain</source>
      <translation>地形</translation>
    </message>
    <message>
      <source>the difference in hours and minutes from Coordinated Universal Time (UTC) for a your place and date. for example, In case of South Korea(UTC+09:00), UTC offset is 540 min (9*60) refer to https://en.wikipedia.org/wiki/List_of_UTC_time_offsets</source>
      <translation>您的地点和日期与协调世界时 (UTC) 的小时和分钟差。例如，对于韩国（UTC+09:00），UTC 偏移量为 540 分钟（9*60），请参阅 https://en.wikipedia.org/wiki/List_of_UTC_time_offsets</translation>
    </message>
    <message>
      <source>threshold = FD_ACT_MOT_C2T * thrust - FD_ACT_LOW_OFF</source>
      <translation>阈值 = FD_ACT_MOT_C2T * 推力 - FD_ACT_LOW_OFF</translation>
    </message>
    <message>
      <source>thrust at max throttle for one motor</source>
      <translation>一台电机最大油门推力</translation>
    </message>
    <message>
      <source>tmode (or "toy" mode) gives a simplified user interface designed for mass market drones. Version1 is for the SkyViper V2450GPS. Version2 is for the F412 based boards</source>
      <translation>tmode（或“玩具”模式）提供了专为大众市场无人机设计的简化用户界面。版本1适用于SkyViper V2450GPS。版本2适用于基于F412的板</translation>
    </message>
    <message>
      <source>transport type for second CAN interface</source>
      <translation>第二个 CAN 接口的传输类型</translation>
    </message>
    <message>
      <source>u-blox</source>
      <translation>乌布洛克斯</translation>
    </message>
    <message>
      <source>u-blox GPS output rate</source>
      <translation>u-blox GPS 输出速率</translation>
    </message>
    <message>
      <source>uBlox-MovingBaseline-Base</source>
      <translation>uBlox-MovingBaseline-底座</translation>
    </message>
    <message>
      <source>uXRCE-DDS participant configuration</source>
      <translation>uXRCE-DDS参与者配置</translation>
    </message>
    <message>
      <source>use geo_lookup declination</source>
      <translation>使用 geo_lookup 磁偏角</translation>
    </message>
    <message>
      <source>web server block size</source>
      <translation>Web 服务器块大小</translation>
    </message>
    <message>
      <source>yaw error</source>
      <translation>偏航误差</translation>
    </message>
  </context>
</TS>
