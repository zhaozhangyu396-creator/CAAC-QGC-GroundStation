<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en">
  <context>
    <name>VehicleParameters</name>
    <message>
      <source> Enable use of CAN buses</source>
      <translation> 启用 CAN 总线</translation>
    </message>
    <message>
      <source>"Superexponential" factor for refining the input curve shape tuned using MC_ACRO_EXPO.
0 Pure Expo function
0.7 reasonable shape enhancement for intuitive stick feel
0.95 very strong bent input curve only near maxima have effect</source>
      <translation>用于细化使用 MC_ACRO_EXPO 调整的输入曲线形状的“超指数”因子。 0 Pure Expo功能 0.7 合理的形状增强，带来直观的棒感 0.95 非常强的弯曲输入曲线，仅在最大值附近才有效果</translation>
    </message>
    <message>
      <source>(Optional) Maximum drift distance to allow when thermalling.</source>
      <translation>（可选）热作用时允许的最大漂移距离。</translation>
    </message>
    <message>
      <source>- Default</source>
      <translation>- 默认</translation>
    </message>
    <message>
      <source>- VOXL ESC</source>
      <translation>- VOXL ESC</translation>
    </message>
    <message>
      <source>0 : Set to 0 to do nothing 1 : Set to 1 to start a calibration at next boot This parameter is reset to zero when the temperature calibration starts. default (0, no calibration)</source>
      <translation>0：设置为 0 不执行任何操作 1：设置为 1 以在下次启动时开始校准 当温度校准开始时，此参数重置为零。默认（0，无校准）</translation>
    </message>
    <message>
      <source>0 for position control, small for low speeds, 100 for max speed. A good general value is 10 which gives a movement speed of 3 degrees per second.</source>
      <translation>0 用于位置控制，小型用于低速，100 用于最大速度。一个好的通用值是 10，它给出每秒 3 度的移动速度。</translation>
    </message>
    <message>
      <source>0: Arming/disarming triggers on switch transition. 1: Arming/disarming triggers when holding the momentary button down for COM_RC_ARM_HYST like the stick gesture.</source>
      <translation>0：开关转换时解锁/上锁触发。 1：当按住 COM_RC_ARM_HYST 的瞬时按钮（如摇杆手势）时，解锁/上锁触发。</translation>
    </message>
    <message>
      <source>0: do not read RSSI from input channel 1-18: read RSSI from specified input channel Specify the range for RSSI input with RC_RSSI_PWM_MIN and RC_RSSI_PWM_MAX parameters.</source>
      <translation>0：不从输入通道读取 RSSI 1-18：从指定输入通道读取 RSSI 用 RC_RSSI_PWM_MIN 和 RC_RSSI_PWM_MAX 参数指定 RSSI 输入的范围。</translation>
    </message>
    <message>
      <source>0:zero pwm, 1:trim pwm</source>
      <translation>0：零 pwm，1：调整 pwm</translation>
    </message>
    <message>
      <source>0x87</source>
      <translation>0x87</translation>
    </message>
    <message>
      <source>1 byte bitmap of which EKF2 instances will use the output from the EKF-GSF yaw estimator that has been turned on by the EK2_GSF_RUN parameter. If the inertial navigation calculation stops following the GPS, then the vehicle code can request EKF2 to attempt to resolve the issue, either by performing a yaw reset if enabled by this parameter by switching to another EKF2 instance. Additionally the EKF2 will  initiate a reset internally if navigation is lost for more than EK2_GSF_DELAY milli seconds.</source>
      <translation>1 字节位图，其中 EKF2 实例将使用已由 EK2_GSF_RUN 参数打开的 EKF-GSF 偏航估计器的输出。如果惯性导航计算在 GPS 之后停止，则车辆代码可以请求 EKF2 尝试解决问题，方法是通过切换到另一个 EKF2 实例来执行偏航重置（如果此参数启用）。此外，如果导航丢失时间超过 EK2_GSF_DELAY 毫秒，EKF2 将在内部启动重置。</translation>
    </message>
    <message>
      <source>1-sigma IMU gyro switch-on bias</source>
      <translation>1-sigma IMU 陀螺仪开启偏置</translation>
    </message>
    <message>
      <source>1000000 8N1</source>
      <translation>1000000 8N1</translation>
    </message>
    <message>
      <source>10th harmonic</source>
      <translation>10次谐波</translation>
    </message>
    <message>
      <source>1200Kts</source>
      <translation>1200Kts</translation>
    </message>
    <message>
      <source>1475 Hz</source>
      <translation>1475 Hz</translation>
    </message>
    <message>
      <source>15S Battery</source>
      <translation>15S电池</translation>
    </message>
    <message>
      <source>1G3 Band B</source>
      <translation>1G3 频段 B</translation>
    </message>
    <message>
      <source>2 Hz</source>
      <translation>2 Hz</translation>
    </message>
    <message>
      <source>2000 Hz</source>
      <translation>2000 Hz</translation>
    </message>
    <message>
      <source>25 Hz</source>
      <translation>25 Hz</translation>
    </message>
    <message>
      <source>2D + Terrain: Maintain constant altitude relative to terrain below and track XY position</source>
      <translation>2D + 地形：相对于下方地形保持恒定高度并跟踪 XY 位置</translation>
    </message>
    <message>
      <source>2S Battery</source>
      <translation>2S电池</translation>
    </message>
    <message>
      <source>2ndSensor</source>
      <translation>第二传感器</translation>
    </message>
    <message>
      <source>300Kts</source>
      <translation>300Kts</translation>
    </message>
    <message>
      <source>3D Fix</source>
      <translation>3D修复</translation>
    </message>
    <message>
      <source>3G3 Band B</source>
      <translation>3G3 频段 B</translation>
    </message>
    <message>
      <source>400Hz</source>
      <translation>400Hz</translation>
    </message>
    <message>
      <source>4th Sensor</source>
      <translation>第四个传感器</translation>
    </message>
    <message>
      <source>500000 8N1</source>
      <translation>500000 8N1</translation>
    </message>
    <message>
      <source>5900 Hz</source>
      <translation>5900 Hz</translation>
    </message>
    <message>
      <source>600 8N1</source>
      <translation>600 8N1</translation>
    </message>
    <message>
      <source>6th Sensor</source>
      <translation>第 6 个传感器</translation>
    </message>
    <message>
      <source>8 sample averaging</source>
      <translation>8 样本平均</translation>
    </message>
    <message>
      <source>8x8 mode</source>
      <translation>8x8 模式</translation>
    </message>
    <message>
      <source>A bitmask to set some FRSky Telemetry specific options</source>
      <translation>用于设置一些 FRsky 遥测特定选项的位掩码</translation>
    </message>
    <message>
      <source>A fraction [0,1] deprioritizing yaw compared to roll and pitch in non-linear attitude control.
Deprioritizing yaw is necessary because multicopters have much less control authority
in yaw compared to the other axes and it makes sense because yaw is not critical for
stable hovering or 3D navigation.
For yaw control tuning use SC_YAW_P. This ratio has no impact on the yaw gain.</source>
      <translation>在非线性姿态控制中，与横滚和俯仰相比，偏航的分数 [0,1] 降低了优先级。取消偏航优先级是必要的，因为与其他轴相比，多旋翼飞行器对偏航的控制权限要小得多，而且这是有道理的，因为偏航对于稳定悬停或 3D 导航并不重要。对于偏航控制调整，请使用 SC_YAW_P。该比率对偏航增益没有影响。</translation>
    </message>
    <message>
      <source>A non-zero, positive value specifies the timeframe in seconds within failure detector is allowed to disarm the vehicle
if attitude exceeds the limits defined in FD_FAIL_P and FD_FAIL_R.
The check is not executed for flight modes that do support acrobatic maneuvers, e.g: Acro (MC/FW) and Manual (FW).
A zero or negative value means that the check is disabled.</source>
      <translation>非零正值指定如果姿态超出 FD_FAIL_P 和 FD_FAIL_R 中定义的限制，故障检测器允许解除车辆的时间范围（以秒为单位）。对于支持特技动作的飞行模式，例如：Acro (MC/FW) 和手动 (FW)，不会执行该检查。零或负值意味着检查被禁用。</translation>
    </message>
    <message>
      <source>A taildragger with steerable wheel might need to pitch up a little to keep its wheel on the ground before airspeed to takeoff is reached.</source>
      <translation>在达到起飞空速之前，带有可转向轮的尾部拖曳机可能需要稍微向上倾斜以将其轮子保持在地面上。</translation>
    </message>
    <message>
      <source>A1</source>
      <translation>A1</translation>
    </message>
    <message>
      <source>A5</source>
      <translation>A5</translation>
    </message>
    <message>
      <source>ACRO Mode</source>
      <translation>ACRO模式</translation>
    </message>
    <message>
      <source>AD7091R5</source>
      <translation>AD7091R5</translation>
    </message>
    <message>
      <source>ADSB altitude of another aircraft</source>
      <translation>另一架飞机的ADSB高度</translation>
    </message>
    <message>
      <source>ADSB vehicle list altitude filter</source>
      <translation>ADSB 车辆列表高度过滤器</translation>
    </message>
    <message>
      <source>ADSB-In Vehicle List Size</source>
      <translation>ADSB-In 车辆列表尺寸</translation>
    </message>
    <message>
      <source>ADSB-Out Vehicle Size Configuration</source>
      <translation>ADSB-Out车辆尺寸配置</translation>
    </message>
    <message>
      <source>AFBR Rangefinder Short/Long Range Threshold Hysteresis</source>
      <translation>AFBR 测距仪短/长距离阈值迟滞</translation>
    </message>
    <message>
      <source>AHRS RP_P</source>
      <translation>AHRS RP_P</translation>
    </message>
    <message>
      <source>AHRS last origin latitude</source>
      <translation>AHRS 最后原点纬度</translation>
    </message>
    <message>
      <source>AHRS/EKF Origin Altitude</source>
      <translation>AHRS/EKF 原点海拔高度</translation>
    </message>
    <message>
      <source>AIS</source>
      <translation>AIS</translation>
    </message>
    <message>
      <source>AK0991x</source>
      <translation>AK0991x</translation>
    </message>
    <message>
      <source>AMP offset</source>
      <translation>AMP 偏移量</translation>
    </message>
    <message>
      <source>APM TriggerPin3</source>
      <translation>APM 触发引脚3</translation>
    </message>
    <message>
      <source>APM1-airspeed port</source>
      <translation>APM1-空速口</translation>
    </message>
    <message>
      <source>APM2-A6</source>
      <translation>APM2-A6</translation>
    </message>
    <message>
      <source>ARMING_EN</source>
      <translation>ARMING_EN</translation>
    </message>
    <message>
      <source>ASPD1_EN</source>
      <translation>ASPD1_EN</translation>
    </message>
    <message>
      <source>ASPEED_Y</source>
      <translation>ASPEED_Y</translation>
    </message>
    <message>
      <source>ATT_THR</source>
      <translation>ATT_THR</translation>
    </message>
    <message>
      <source>AUAV-10in</source>
      <translation>AUAV-10in</translation>
    </message>
    <message>
      <source>AUTOLAND if possible else RTL</source>
      <translation>如果可能的话 AUTOLAND 否则 RTL</translation>
    </message>
    <message>
      <source>AUTO_LOITER_ALL</source>
      <translation>AUTO_LOITER_ALL</translation>
    </message>
    <message>
      <source>AUX2</source>
      <translation>AUX2</translation>
    </message>
    <message>
      <source>AUX6</source>
      <translation>AUX6</translation>
    </message>
    <message>
      <source>AUXPIN</source>
      <translation>AUXPIN</translation>
    </message>
    <message>
      <source>Abort if terrain times out (after a first successful measurement)</source>
      <translation>如果地形超时则中止（第一次成功测量后）</translation>
    </message>
    <message>
      <source>Above this speed, the measurement noise is linearly increased
to reduce the sensitivity of the estimator from biased measurement.
Set to a low value on vehicles with large lifting surfaces.</source>
      <translation>超过该速度，测量噪声会线性增加，从而降低估计器对有偏差测量的灵敏度。对于具有大升力面的车辆设置为较低值。</translation>
    </message>
    <message>
      <source>Absolute maximum for all velocity or acceleration controlled modes.
Any higher value is truncated.</source>
      <translation>所有速度或加速度控制模式的绝对最大值。任何较高的值都会被截断。</translation>
    </message>
    <message>
      <source>Accel (vertical) Derivative FeedForward Gain</source>
      <translation>加速（垂直）微分前馈增益</translation>
    </message>
    <message>
      <source>Accel 1 bias</source>
      <translation>加速 1 偏置</translation>
    </message>
    <message>
      <source>Accel 3 scaling factor</source>
      <translation>Accel 3 比例因子</translation>
    </message>
    <message>
      <source>Accel bias propagation noise density</source>
      <translation>加速偏置传播噪声密度</translation>
    </message>
    <message>
      <source>Acceleration</source>
      <translation>加速</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller I gain.  Corrects long-term difference in desired vertical acceleration and actual acceleration</source>
      <translation>加速度（垂直）控制器 I 增益。  纠正所需垂直加速度和实际加速度的长期差异</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller derivative frequency in Hz</source>
      <translation>加速度（垂直）控制器导数频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller target frequency in Hz. Previously _ACCZ_FLTT.</source>
      <translation>加速度（垂直）控制器目标频率（以 Hz 为单位）。以前是_ACCZ_FLTT。</translation>
    </message>
    <message>
      <source>Acceleration limit for the horizontal kinematic input shaping</source>
      <translation>水平运动输入整形的加速度限制</translation>
    </message>
    <message>
      <source>Acceleration of shove y</source>
      <translation>推力 y 的加速度</translation>
    </message>
    <message>
      <source>Accelerometer 0 Z-axis offset</source>
      <translation>加速度计 0 Z 轴偏移</translation>
    </message>
    <message>
      <source>Accelerometer 1 Y-axis scaling factor</source>
      <translation>加速度计 1 Y 轴比例因子</translation>
    </message>
    <message>
      <source>Accelerometer 1st order temperature coefficient Z axis</source>
      <translation>加速度计一阶温度系数 Z 轴</translation>
    </message>
    <message>
      <source>Accelerometer 2 priority</source>
      <translation>加速度计2优先级</translation>
    </message>
    <message>
      <source>Accelerometer 3 Y-axis scaling factor</source>
      <translation>加速度计 3 Y 轴比例因子</translation>
    </message>
    <message>
      <source>Accelerometer 3rd order temperature coefficient Z axis</source>
      <translation>加速度计三阶温度系数 Z 轴</translation>
    </message>
    <message>
      <source>Accelerometer bias stability (m/s^3)</source>
      <translation>加速度计偏置稳定性 (m/s^3)</translation>
    </message>
    <message>
      <source>Accelerometer noise (m/s^2)</source>
      <translation>加速度计噪声 (m/s^2)</translation>
    </message>
    <message>
      <source>Accelerometer offset temperature ^2 polynomial coefficient - X axis</source>
      <translation>加速度计偏移温度^2多项式系数-X轴</translation>
    </message>
    <message>
      <source>Accelerometer offsets of Y axis</source>
      <translation>Y 轴加速度计偏移</translation>
    </message>
    <message>
      <source>Accelerometer scaling of Z axis</source>
      <translation>Z 轴加速度计缩放</translation>
    </message>
    <message>
      <source>Accelerometer2 offsets of X axis. This is setup using the acceleration calibration or level operations</source>
      <translation>加速度计2 X 轴偏移。这是使用加速度校准或水平操作进行设置的</translation>
    </message>
    <message>
      <source>Accelerometer2 scaling of Y axis  Calculated during acceleration calibration routine</source>
      <translation>Accelerometer2 Y 轴缩放 在加速度校准例程期间计算</translation>
    </message>
    <message>
      <source>Accelerometer3 offsets of Y axis. This is setup using the acceleration calibration or level operations</source>
      <translation>加速度计3 Y 轴偏移。这是使用加速度校准或水平操作进行设置的</translation>
    </message>
    <message>
      <source>Accelerometer3 scaling of Z axis  Calculated during acceleration calibration routine</source>
      <translation>加速度计3 Z 轴缩放 在加速度校准例程期间计算</translation>
    </message>
    <message>
      <source>Acceptance radius for multicopter altitude.</source>
      <translation>多旋翼高度的接受半径。</translation>
    </message>
    <message>
      <source>Acro Roll and Pitch Rate</source>
      <translation>特技横滚和俯仰速率</translation>
    </message>
    <message>
      <source>Acro Yaw P gain</source>
      <translation>特技偏航 P 增益</translation>
    </message>
    <message>
      <source>Acro mode Expo factor for Yaw</source>
      <translation>Acro 模式偏航的 Expo 系数</translation>
    </message>
    <message>
      <source>Acro mode options</source>
      <translation>特技模式选项</translation>
    </message>
    <message>
      <source>Acro roll/pitch rate control input time constant</source>
      <translation>特技横滚/俯仰速率控制输入时间常数</translation>
    </message>
    <message>
      <source>Action the system takes when a wind speed above the specified threshold is detected.
See COM_WIND_MAX to set the failsafe threshold.
If enabled, it is not possible to resume the mission or switch to any auto mode other than
RTL or Land if this threshold is exceeded. Taking over in any manual
mode is still possible.</source>
      <translation>当检测到风速高于指定阈值时，系统将采取行动。请参见 COM_WIND_MAX 设置故障安全阈值。如果启用，如果超过此阈值，则无法恢复任务或切换到除 RTL 或着陆之外的任何自动模式。在任何手动模式下接管仍然是可能的。</translation>
    </message>
    <message>
      <source>Activate wind speed estimation using specific-force measurements and a drag model defined by EKF2_BCOEF_[XY] and EKF2_MCOEF. Only use on vehicles that have their thrust aligned with the Z axis and no thrust in the XY plane.</source>
      <translation>使用比力测量和 EKF2_BCOEF_[XY] 和 EKF2_MCOEF 定义的阻力模型激活风速估计。仅适用于推力与 Z 轴对齐且 XY 平面无推力的车辆。</translation>
    </message>
    <message>
      <source>Actual time in seconds for the main rotor to reach full speed after motor interlock is enabled (throttle hold off). Must be at least one second longer than the Throttle Ramp Time that is set with RSC_RAMP_TIME. WARNING: For AutoThrottle users with piston and turbine engines it is VERY important to know how long it takes to warm up your engine and reach full rotor speed when throttle switch is turned ON. This timer should be set for at least the amount of time it takes to get your helicopter to full flight power, ready for takeoff. Failure to heed this warning could result in the auto-takeoff mode attempting to lift up into hover before the engine has reached full power, and subsequent loss of control</source>
      <translation>启用电机联锁（关闭油门保持）后，主旋翼达到全速所需的实际时间，单位为秒。此值必须至少比 RSC_RAMP_TIME 设置的油门爬升时间长 1 秒。警告：使用活塞或涡轮发动机并启用自动油门时，必须准确了解打开油门开关后发动机预热并使旋翼达到全速所需的时间。此计时器应至少覆盖直升机达到完整飞行功率、准备好起飞所需的时间。忽视此要求可能导致发动机尚未达到全功率时，自动起飞模式就尝试悬停，从而失去控制。</translation>
    </message>
    <message>
      <source>Add leading zero byte to requests</source>
      <translation>向请求添加前导零字节</translation>
    </message>
    <message>
      <source>Adjusts the amount of weighting that the pitch control
applies to speed vs height errors.
0 -&gt; control height only
2 -&gt; control speed only (gliders)</source>
      <translation>调整俯仰控制应用于速度与高度误差的权重量。 0 -&gt; 仅控制高度 2 -&gt; 仅控制速度（滑翔机）</translation>
    </message>
    <message>
      <source>After a data link loss: after this number of seconds with a healthy datalink the 'datalink loss' flag is set back to false</source>
      <translation>数据链路丢失后：在数据链路正常运行的秒数之后，“数据链路丢失”标志将设置回 false</translation>
    </message>
    <message>
      <source>After this amount of seconds without datalink, the GCS connection lost mode triggers</source>
      <translation>在没有数据链路的秒数后，GCS 连接丢失模式触发</translation>
    </message>
    <message>
      <source>AirSimSITL</source>
      <translation>航空模拟器SITL</translation>
    </message>
    <message>
      <source>Aircraft length and width</source>
      <translation>飞机长度和宽度</translation>
    </message>
    <message>
      <source>Airspeed Enable</source>
      <translation>空速启用</translation>
    </message>
    <message>
      <source>Airspeed at which we can switch to fw mode</source>
      <translation>我们可以切换到 fw 模式的空速</translation>
    </message>
    <message>
      <source>Airspeed failure innovation threshold</source>
      <translation>空速故障创新阈值</translation>
    </message>
    <message>
      <source>Airspeed offset</source>
      <translation>空速偏移</translation>
    </message>
    <message>
      <source>Airspeed ratio for 2nd airspeed sensor</source>
      <translation>第二空速传感器的空速比</translation>
    </message>
    <message>
      <source>Airspeed sensor failure pressure</source>
      <translation>空速传感器故障压力</translation>
    </message>
    <message>
      <source>Airspeed-less front transition time (open loop)</source>
      <translation>无空速前过渡时间（开环）</translation>
    </message>
    <message>
      <source>Alexmos Serial</source>
      <translation>亚历克斯莫斯系列</translation>
    </message>
    <message>
      <source>All sensors</source>
      <translation>所有传感器</translation>
    </message>
    <message>
      <source>AllIMUs</source>
      <translation>所有IMU</translation>
    </message>
    <message>
      <source>Allow FW Land-if bit is not set then NAV_LAND command on quadplanes will instead perform a NAV_VTOL_LAND</source>
      <translation>允许 FW Land-如果未设置位，则四平面上的 NAV_LAND 命令将改为执行 NAV_VTOL_LAND</translation>
    </message>
    <message>
      <source>Allow arm/disarm by rudder input. When enabled arming can be done with right rudder, disarming with left rudder. Rudder arming only works in manual throttle modes with throttle at zero +- deadzone (RCx_DZ)</source>
      <translation>允许通过方向舵输入解锁/上锁。启用后，可以使用右舵进行解锁，使用左舵进行上锁。方向舵解锁仅适用于手动油门模式，油门位于零 +- 死区 (RCx_DZ)</translation>
    </message>
    <message>
      <source>Allow final approach (without horizontal positioning) if losing landing target closer than this to the ground.</source>
      <translation>如果失去距离地面比此更近的着陆目标，则允许最后进场（无需水平定位）。</translation>
    </message>
    <message>
      <source>AllowFWTakeoff</source>
      <translation>允许FW起飞</translation>
    </message>
    <message>
      <source>Allows setting an individual MAVLink system id for this vehicle to distinguish it from others on the same network</source>
      <translation>允许为此车辆设置单独的 MAVLink 系统 ID，以将其与同一网络上的其他车辆区分开来</translation>
    </message>
    <message>
      <source>Allows setting the maximum acceptable compass, velocity, position and height variances. Used in arming check and EKF failsafe.</source>
      <translation>允许设置最大可接受的罗盘、速度、位置和高度变化。用于解锁检查和EKF故障保护。</translation>
    </message>
    <message>
      <source>Allows you to emulate wind in sim</source>
      <translation>允许您在 sim 中模拟风</translation>
    </message>
    <message>
      <source>Allows you to enable (1) or disable (0) the Volz simulator</source>
      <translation>允许您启用 (1) 或禁用 (0) Volz 模拟器</translation>
    </message>
    <message>
      <source>Allows you to enable (1) or disable (0) the simulated tonealarm</source>
      <translation>允许您启用 (1) 或禁用 (0) 模拟音调警报</translation>
    </message>
    <message>
      <source>AltHold</source>
      <translation>按住Alt键</translation>
    </message>
    <message>
      <source>Altitude Hold kP</source>
      <translation>高度保持 kP</translation>
    </message>
    <message>
      <source>Altitude at which wind reaches full strength, decaying from full strength as altitude lowers to ground level</source>
      <translation>风力达到最大强度的高度，随着高度降低到地面，风力逐渐减弱</translation>
    </message>
    <message>
      <source>Altitude error threshold for quad-chute triggering during fixed-wing flight. The check is only active if altitude is controlled and the vehicle is below the current altitude reference. The altitude error is relative to the highest altitude the vehicle has achieved since it has flown below the current altitude reference. Set to 0 do disable.</source>
      <translation>固定翼飞行期间四降落伞触发的高度误差阈值。仅当高度受到控制且车辆低于当前高度参考值时，该检查才会激活。高度误差是相对于飞行器自低于当前高度参考飞行以来所达到的最高高度而言的。设置为 0 禁用。</translation>
    </message>
    <message>
      <source>Altitude in meters above which navigation will begin during auto takeoff</source>
      <translation>高度（以米为单位），高于该高度，自动起飞期间将开始导航</translation>
    </message>
    <message>
      <source>Altitude mode</source>
      <translation>海拔模式</translation>
    </message>
    <message>
      <source>Altitude the aircraft will transit to when a fence breach occurs.  If FENCE_RETALT is &lt;= 0 then the midpoint between FENCE_MAXALT and FENCE_MINALT is used, unless FENCE_MAXALT &lt; FENCE_MINALT.  If FENCE_MAXALT &lt; FENCE_MINALT AND FENCE_RETALT is &lt;= 0 then ALT_HOLD_RTL is the altitude used on a fence breach.</source>
      <translation>发生围栏突破时飞机将飞越的高度。  如果 FENCE_RETALT &lt;= 0，则使用 FENCE_MAXALT 和 FENCE_MINALT 之间的中点，除非 FENCE_MAXALT &lt; FENCE_MINALT。  如果 FENCE_MAXALT &lt; FENCE_MINALT AND FENCE_RETALT &lt;= 0 则 ALT_HOLD_RTL 是围栏突破处使用的高度。</translation>
    </message>
    <message>
      <source>Always Land</source>
      <translation>永远着陆</translation>
    </message>
    <message>
      <source>Always use FW spiral approach</source>
      <translation>始终使用 FW 螺旋方法</translation>
    </message>
    <message>
      <source>Amount of memory available for scripting</source>
      <translation>可用于脚本编写的内存量</translation>
    </message>
    <message>
      <source>Amplitude of the GPS1 altitude error</source>
      <translation>GPS1高度误差的幅度</translation>
    </message>
    <message>
      <source>An angle to compensate between the true north and magnetic north</source>
      <translation>补偿真北和磁北之间的角度</translation>
    </message>
    <message>
      <source>An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero. Set to "Custom Euler Angle" to define the rotation using CAL_MAG2_ROLL, CAL_MAG2_PITCH and CAL_MAG2_YAW.</source>
      <translation>内部传感器将强制设置 -1 的值，因此 GCS 仅应在该值大于或等于零时尝试配置旋转。设置为“自定义欧拉角”以使用 CAL_MAG2_ROLL、CAL_MAG2_PITCH 和 CAL_MAG2_YAW 定义旋转。</translation>
    </message>
    <message>
      <source>Analog Voltage and Current</source>
      <translation>模拟电压和电流</translation>
    </message>
    <message>
      <source>Analog pin that rangefinder is connected to. Set this to 0..9 for the APM2 analog pins. Set to 64 on an APM1 for the dedicated 'airspeed' port on the end of the board. Set to 11 on PX4 for the analog 'airspeed' port. Set to 15 on the Pixhawk for the analog 'airspeed' port.</source>
      <translation>测距仪连接的模拟引脚。对于 APM2 模拟引脚，将其设置为 0..9。对于板末端的专用“空速”端口，在 APM1 上设置为 64。对于模拟“空速”端口，在 PX4 上设置为 11。在 Pixhawk 上将模拟“空速”端口设置为 15。</translation>
    </message>
    <message>
      <source>Angle for swash plate servo 0</source>
      <translation>斜盘舵机角度 0</translation>
    </message>
    <message>
      <source>Angle to follow the target from. 0.0 Equals straight in front of the target's course (direction of motion) and the angle increases in clockwise direction, meaning Right-side would be 90.0 degrees while Left-side is -90.0 degrees Note: When the user force sets the angle out of the min/max range, it will be wrapped (e.g. 480 -&gt; 120) in the range to gracefully handle the out of range.</source>
      <translation>跟随目标的角度。 0.0 等于目标航向（运动方向）正前方，角度按顺时针方向增加，这意味着右侧为 90.0 度，左侧为 -90.0 度 注意：当用户用力将角度设置在最小/最大范围之外时，它将在该范围内包裹（例如 480 -&gt; 120），以优雅地处理超出范围的情况。</translation>
    </message>
    <message>
      <source>Angular acceleration limit for the heading kinematic input shaping</source>
      <translation>航向运动输入整形的角加速度限制</translation>
    </message>
    <message>
      <source>Antenna X position offset</source>
      <translation>天线 X 位置偏移</translation>
    </message>
    <message>
      <source>Applied simulated angular rate to gyroscope</source>
      <translation>将模拟角速率应用于陀螺仪</translation>
    </message>
    <message>
      <source>Applies to both directions in all manual modes with attitude stabilization</source>
      <translation>适用于所有具有姿态稳定的手动模式下的两个方向</translation>
    </message>
    <message>
      <source>Approach angle nudging: shifts the touchdown point laterally while keeping the approach entrance point constant
Approach path nudging: shifts the touchdown point laterally along with the entire approach path
This is useful for manually adjusting the landing point in real time when map or GNSS errors cause an offset from the
desired landing vector. Nudging is done with yaw stick, constrained to FW_LND_TD_OFF (in meters) and the direction is
relative to the vehicle heading (stick deflection to the right = land point moves to the right as seen by the vehicle).</source>
      <translation>进近角度微移：横向移动着陆点，同时保持进近入口点恒定。 进场路径微移：沿着整个进近路径横向移动接地点。当地图或 GNSS 错误导致偏离所需着陆矢量时，这对于实时手动调整着陆点非常有用。微移是用偏航杆完成的，限制为 FW_LND_TD_OFF（以米为单位），方向相对于车辆航向（杆向右偏转 = 着陆点向右移动，如车辆所见）。</translation>
    </message>
    <message>
      <source>Arm authorization method</source>
      <translation>ARM授权方式</translation>
    </message>
    <message>
      <source>Arm switch channel</source>
      <translation>解锁开关通道</translation>
    </message>
    <message>
      <source>ArmDisarm with AirMode  (4.2 and higher)</source>
      <translation>使用 AirMode 进行 ArmDisarm（4.2 及更高版本）</translation>
    </message>
    <message>
      <source>Arming disabled until some requirements are met. If 0, there are no requirements (arm immediately).  If 1, all checks not skipped by ARMING_SKIPCHK must pass before the vehicle can be armed (for example, via rudder stick or GCS command).  If 3, Arm immediately once pre-arm/arm checks are satisfied, but only one time per boot up.  Note that a reboot is NOT required when setting to 0 but IS require when setting to 3.</source>
      <translation>在满足某些要求之前，禁用解锁。如果是0，则没有要求（立即解锁）。  如果是 1，则必须通过 ARMING_SKIPCHK 未跳过的所有检查，然后才能对车辆进行解锁（例如，通过舵杆或 GCS 命令）。  如果是 3，一旦预解锁/解锁检查得到满足，就会立即解锁，但每次启动仅一次。  请注意，当设置为 0 时，NOT 需要重新启动，而当设置为 3 时，IS 则需要重新启动。</translation>
    </message>
    <message>
      <source>ArmingOnly</source>
      <translation>仅解锁</translation>
    </message>
    <message>
      <source>AttCon Accel Limits</source>
      <translation>AttCon 加速限制</translation>
    </message>
    <message>
      <source>Aurora</source>
      <translation>极光</translation>
    </message>
    <message>
      <source>Auto RTL</source>
      <translation>自动RTL</translation>
    </message>
    <message>
      <source>Auto modes</source>
      <translation>自动模式</translation>
    </message>
    <message>
      <source>Auto-enable of fence</source>
      <translation>自动启用围栏</translation>
    </message>
    <message>
      <source>AutoEnable</source>
      <translation>自动启用</translation>
    </message>
    <message>
      <source>AutoThrottle governor droop response under load, normal settings of 0-50%. Higher value is quicker response to large speed changes due to load but may cause surging. Adjust this to be as aggressive as possible without getting surging or RPM over-run when the governor responds to large load changes on the rotor system</source>
      <translation>AutoThrottle调速器负载下响应下降，正常设置为0-50%。值越高，对负载引起的大速度变化的响应越快，但可能会导致喘振。当调速器响应转子系统上的大负载变化时，将其调整为尽可能积极，而不会出现喘振或 RPM 超限</translation>
    </message>
    <message>
      <source>AutoTune minimum sweep frequency</source>
      <translation>自动调谐最小扫描频率</translation>
    </message>
    <message>
      <source>Automatic GPS configuration</source>
      <translation>自动GPS配置</translation>
    </message>
    <message>
      <source>Automatic: reset on fusion timeout if no other source of position is available Dead-reckoning: reset on fusion timeout if no source of velocity is available</source>
      <translation>自动：如果没有其他可用的位置源，则在融合超时时重置航位推测：如果没有可用的速度源，则在融合超时时重置</translation>
    </message>
    <message>
      <source>Automatically detect a crash during AUTO flight and perform the bitmask selected action(s). Disarm will turn off motor for safety and to help against burning out ESC and motor. Set to 0 to disable crash detection.</source>
      <translation>在 AUTO 飞行期间自动检测碰撞并执行位掩码选择的操作。上锁将关闭电机以确保安全并有助于防止烧坏 ESC 和电机。设置为 0 以禁用崩溃检测。</translation>
    </message>
    <message>
      <source>Automotive</source>
      <translation>汽车</translation>
    </message>
    <message>
      <source>Autotune aggressiveness. Defines the bounce back used to detect size of the D term.</source>
      <translation>自动调节攻击性。定义用于检测 D 项大小的反弹。</translation>
    </message>
    <message>
      <source>Aux2</source>
      <translation>辅助2</translation>
    </message>
    <message>
      <source>Auxiliary channel to control pitch (in AUX input or manual mode)</source>
      <translation>控制俯仰的辅助通道（AUX输入或手动模式）</translation>
    </message>
    <message>
      <source>Auxiliary global position sensor 1 delay (to IMU)</source>
      <translation>辅助全球位置传感器 1 延迟（至 IMU）</translation>
    </message>
    <message>
      <source>Auxiliary switch channel</source>
      <translation>辅助开关通道</translation>
    </message>
    <message>
      <source>Avoidance control enable/disable</source>
      <translation>回避控制启用/禁用</translation>
    </message>
    <message>
      <source>Avoidance maximum vertical backup speed</source>
      <translation>回避最大垂直后退速度</translation>
    </message>
    <message>
      <source>Axis of rotor 0 thrust vector, Z body axis component</source>
      <translation>转子轴0推力矢量，Z体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 11 thrust vector, Y body axis component</source>
      <translation>转子轴11推力矢量，Y本体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 4 thrust vector, X body axis component</source>
      <translation>转子轴4推力矢量，X体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 6 thrust vector, Z body axis component</source>
      <translation>转子轴6推力矢量，Z体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 9 thrust vector, Y body axis component</source>
      <translation>转子轴9推力矢量，Y本体轴分量</translation>
    </message>
    <message>
      <source>BAT2USED_X</source>
      <translation>BAT2USED_X</translation>
    </message>
    <message>
      <source>BATUSED_EN</source>
      <translation>BATUSED_EN</translation>
    </message>
    <message>
      <source>BB Blue GP0 pin 4</source>
      <translation>BB 蓝色 GP0 引脚 4</translation>
    </message>
    <message>
      <source>BBBMini Pin P8.19</source>
      <translation>BBBMini 引脚 P8.19</translation>
    </message>
    <message>
      <source>BLHRPM_Y</source>
      <translation>BLHRPM_Y</translation>
    </message>
    <message>
      <source>BLHeli bitmask of 3D channels</source>
      <translation>3D 通道的 BLHeli 位掩码</translation>
    </message>
    <message>
      <source>BLHeli protocol timeout</source>
      <translation>BLHeli 协议超时</translation>
    </message>
    <message>
      <source>BLHeli_S+EDT</source>
      <translation>BLHeli_S+EDT</translation>
    </message>
    <message>
      <source>BMMCC</source>
      <translation>BMMCC</translation>
    </message>
    <message>
      <source>BTEMP_EN</source>
      <translation>BTEMP_EN</translation>
    </message>
    <message>
      <source>Backtransition deceleration setpoint to tilt I gain</source>
      <translation>向后过渡减速度设定点至倾斜 I 增益</translation>
    </message>
    <message>
      <source>BalanceBot Maximum Pitch</source>
      <translation>BalanceBot 最大俯仰</translation>
    </message>
    <message>
      <source>Bandwidth per notch filter when using dynamic notch filtering with ESC RPM.</source>
      <translation>使用 ESC RPM 进行动态陷波滤波时的每陷波滤波器带宽。</translation>
    </message>
    <message>
      <source>Baro count</source>
      <translation>巴罗伯爵</translation>
    </message>
    <message>
      <source>Baro2 Noise</source>
      <translation>Baro2 噪声</translation>
    </message>
    <message>
      <source>Barometer 1 offset</source>
      <translation>气压计 1 偏移</translation>
    </message>
    <message>
      <source>Barometer Vertical Accuracy when installed in the vehicle. Note this is dependent upon installation conditions and thus disabled by default</source>
      <translation>安装在车辆上时气压计的垂直精度。请注意，这取决于安装条件，因此默认情况下禁用</translation>
    </message>
    <message>
      <source>Barometer data rate (Hz).
Valid rates: 0 or any factor of 1000.</source>
      <translation>气压计数据速率 (Hz)。有效率：0 或 1000 的任何因子。</translation>
    </message>
    <message>
      <source>Barometer ground effect altitude error</source>
      <translation>气压计地面效应高度误差</translation>
    </message>
    <message>
      <source>Barometer offset temperature ^4 polynomial coefficient</source>
      <translation>气压计偏移温度^4多项式系数</translation>
    </message>
    <message>
      <source>Barometer wind coefficient direction forward in SITL</source>
      <translation>SITL 中的气压计风系数方向</translation>
    </message>
    <message>
      <source>Barometric presssure altitude z standard deviation</source>
      <translation>气压高度 z 标准差</translation>
    </message>
    <message>
      <source>Base diving pressure. This is the ambient air pressure at launch site, and is persistent between boots.</source>
      <translation>基础潜水压力。这是发射场的环境气压，并且在靴子之间持续存在。</translation>
    </message>
    <message>
      <source>Battery 1 capacity</source>
      <translation>电池1容量</translation>
    </message>
    <message>
      <source>Battery 2 current per volt (A/V)</source>
      <translation>电池 2 每伏电流 (A/V)</translation>
    </message>
    <message>
      <source>Battery Failsafe Enable</source>
      <translation>电池故障保护启用</translation>
    </message>
    <message>
      <source>Battery Voltage sensing pin on the AD7091R5 Ic</source>
      <translation>AD7091R5 Ic 上的电池电压检测引脚</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT5_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT5_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT9_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT9_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTD_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTD_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT8_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT8_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTG_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTG_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT4_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT4_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT8_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT8_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATTC_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATTC_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATTG_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATTG_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery estimator cell count</source>
      <translation>电池估算器电池数量</translation>
    </message>
    <message>
      <source>Battery monitor I2C address</source>
      <translation>电池监视器I2C地址</translation>
    </message>
    <message>
      <source>Battery power level scaler</source>
      <translation>电池电量定标器</translation>
    </message>
    <message>
      <source>Battery voltage compensation minimum voltage</source>
      <translation>电池电压补偿最低电压</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATT6_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATT6_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATT6_LOW_TIMER 参数指定的时间，则车辆将执行 BATT6_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATTE_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATTE_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATTE_LOW_TIMER 参数指定的时间，则车辆将执行 BATTE_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATT6_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATT6_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATT6_LOW_TIMER 参数指定的时间，则车辆将执行 BATT6_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATTE_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATTE_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATTE_LOW_TIMER 参数指定的时间，则车辆将执行 BATTE_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Baud rate for the serial port to the LTE modem when connected. Initial power on baudrate is in LTE_IBAUD</source>
      <translation>连接 LTE 调制解调器时串行端口的波特率。初始上电波特率位于 LTE_IBAUD 中</translation>
    </message>
    <message>
      <source>Baudrate for the TELEM 2 Serial Port</source>
      <translation>TELEM 2 串口的波特率</translation>
    </message>
    <message>
      <source>Beacon</source>
      <translation>灯塔</translation>
    </message>
    <message>
      <source>Bebop</source>
      <translation>贝博普</translation>
    </message>
    <message>
      <source>Below this altitude descending velocity gets limited to a value
between "MPC_Z_VEL_MAX_DN" (or "MPC_Z_V_AUTO_DN") and "MPC_LAND_SPEED"
Value needs to be higher than "MPC_LAND_ALT2"</source>
      <translation>低于此高度，下降速度被限制为“MPC_Z_VEL_MAX_DN”（或“MPC_Z_V_AUTO_DN”）和“MPC_LAND_SPEED”之间的值 值需要高于“MPC_LAND_ALT2”</translation>
    </message>
    <message>
      <source>BendyRuler's bearing change resistance threshold angle   </source>
      <translation>BendyRuler 的方位变化阻力阈值角度   </translation>
    </message>
    <message>
      <source>BenewakeTFmini-Serial</source>
      <translation>北醒TF迷你系列</translation>
    </message>
    <message>
      <source>Binary</source>
      <translation>二进制</translation>
    </message>
    <message>
      <source>Bitmap of what on-board log types to enable. This value is made up of the sum of each of the log types you want to be saved. It is usually best just to enable all basic log types by setting this to 65535.</source>
      <translation>要启用的板载日志类型的位图。该值由您要保存的每种日志类型的总和组成。通常最好通过将其设置为 65535 来启用所有基本日志类型。</translation>
    </message>
    <message>
      <source>Bitmask controlling sidelip angle fusion</source>
      <translation>控制侧唇角度融合的位掩码</translation>
    </message>
    <message>
      <source>Bitmask for what GNSS system to use on the first GPS (all unchecked or zero to leave GPS as configured)</source>
      <translation>用于第一个 GPS 上使用的 GNSS 系统的位掩码（全部未选中或为零以使 GPS 保持配置状态）</translation>
    </message>
    <message>
      <source>Bitmask of FlightAxis options</source>
      <translation>FlightAxis 选项的位掩码</translation>
    </message>
    <message>
      <source>Bitmask of enabled RC protocols. Allows narrowing the protocol detection to only specific types of RC receivers which can avoid issues with incorrect detection. Set to 1 to enable all protocols.</source>
      <translation>启用的 RC 协议的位掩码。允许将协议检测范围缩小到仅特定类型的 RC 接收器，从而避免错误检测的问题。设置为 1 以启用所有协议。</translation>
    </message>
    <message>
      <source>Bitmask of heli options.  Bit 0 changes how the pitch, roll, and yaw axis integrator term is managed for low speed and takeoff/landing. In AC 4.0 and earlier, scheme uses a leaky integrator for ground speeds less than 5 m/s and won't let the steady state integrator build above ILMI. The integrator is allowed to build to the ILMI value when it is landed.  The other integrator management scheme bases integrator limiting on takeoff and landing.  Whenever the aircraft is landed the integrator is set to zero.  When the aicraft is airborne, the integrator is only limited by IMAX. </source>
      <translation>直升机选项的位掩码。  位 0 更改低速和起飞/着陆时俯仰、横滚和偏航轴积分器项的管理方式。在 AC、4.0 及更早版本中，方案针对低于 5 m/s 的地面速度使用泄漏积分器，并且不会让稳态积分器构建在 ILMI 之上。当积分器着陆时，允许积分器建立到 ILMI 值。  另一种积分器管理方案基于起飞和着陆的积分器限制。  每当飞机着陆时，积分器都会设置为零。  当飞行器在空中时，积分器仅受IMAX的限制。 </translation>
    </message>
    <message>
      <source>Bitmask of options to use with airspeed. 0:Disable use based on airspeed/groundspeed mismatch (see ARSPD_WIND_MAX), 1:Automatically reenable use based on airspeed/groundspeed mismatch recovery (see ARSPD_WIND_MAX) 2:Disable voltage correction, 3:Check that the airspeed is statistically consistent with the navigation EKF vehicle and wind velocity estimates using EKF3 (requires AHRS_EKF_TYPE = 3), 4:Report cal offset to GCS</source>
      <translation>与空速一起使用的选项位掩码。 0：基于空速/地速不匹配禁用使用（参见 ARSPD_WIND_MAX），1：基于空速/地速不匹配恢复自动重新启用使用（参见 ARSPD_WIND_MAX） 2：禁用电压校正，3：检查空速与导航 EKF 车辆和使用 EKF3 估计的风速在统计上一致（需要 AHRS_EKF_TYPE = 3)、4：向 GCS 报告校准偏移</translation>
    </message>
    <message>
      <source>Bitmask of which EKF-GSF yaw estimators are used</source>
      <translation>使用 EKF-GSF 偏航估计器的位掩码</translation>
    </message>
    <message>
      <source>Bitmask which holds which flight modes use simple heading mode (eg bit 0 = 1 means Flight Mode 0 uses simple mode). The bitmask is for flightmode switch positions.</source>
      <translation>位掩码保存哪些飞行模式使用简单航向模式（例如位 0 = 1 表示飞行模式 0 使用简单模式）。位掩码用于飞行模式开关位置。</translation>
    </message>
    <message>
      <source>Bitmask with one set for channel to be transmitted as a servo command over UAVCAN</source>
      <translation>具有一组位掩码，用于通过 UAVCAN 作为舵机命令传输通道</translation>
    </message>
    <message>
      <source>Blending time constant</source>
      <translation>混合时间常数</translation>
    </message>
    <message>
      <source>Board Heater integrator gain</source>
      <translation>板加热器积分器增益</translation>
    </message>
    <message>
      <source>Board options</source>
      <translation>主板选项</translation>
    </message>
    <message>
      <source>Board specific option flags</source>
      <translation>板特定选项标志</translation>
    </message>
    <message>
      <source>BodyFrameRoll</source>
      <translation>机体框架滚动</translation>
    </message>
    <message>
      <source>Boot delay</source>
      <translation>启动延迟</translation>
    </message>
    <message>
      <source>Broadcast heartbeats on local network for MAVLink instance 0</source>
      <translation>在 MAVLink 实例 0 的本地网络上广播心跳</translation>
    </message>
    <message>
      <source>Build in LED</source>
      <translation>内置LED</translation>
    </message>
    <message>
      <source>Bus number of the I2C bus where the airspeed sensor is connected</source>
      <translation>空速传感器所连接的I2C总线的总线号</translation>
    </message>
    <message>
      <source>Bus2(auxillary)</source>
      <translation>总线2（辅助）</translation>
    </message>
    <message>
      <source>Button Pin 1 RC Channel function</source>
      <translation>按钮引脚 1 RC 通道功能</translation>
    </message>
    <message>
      <source>Button flight mode selection</source>
      <translation>按键飞行模式选择</translation>
    </message>
    <message>
      <source>By default disabled for safety reasons</source>
      <translation>出于安全原因默认禁用</translation>
    </message>
    <message>
      <source>CALLSIGN_Y</source>
      <translation>CALLSIGN_Y</translation>
    </message>
    <message>
      <source>CAN per-interface options</source>
      <translation>CAN 每个接口选项</translation>
    </message>
    <message>
      <source>CAN5</source>
      <translation>CAN5</translation>
    </message>
    <message>
      <source>CAN_D1_UC_S3</source>
      <translation>CAN_D1_UC_S3</translation>
    </message>
    <message>
      <source>CHANGING THIS VALUE REQUIRES A RESTART. Defines the auto-start script used to bootstrap the system.</source>
      <translation>CHANGING THIS VALUE REQUIRES RESTART。定义用于引导系统的自动启动脚本。</translation>
    </message>
    <message>
      <source>CLK_EN</source>
      <translation>CLK_EN</translation>
    </message>
    <message>
      <source>COM0</source>
      <translation>COM0</translation>
    </message>
    <message>
      <source>CORTEX</source>
      <translation>CORTEX</translation>
    </message>
    <message>
      <source>CRUISE</source>
      <translation>CRUISE</translation>
    </message>
    <message>
      <source>CURRENT_DRAW</source>
      <translation>CURRENT_DRAW</translation>
    </message>
    <message>
      <source>Calibrate Compasses</source>
      <translation>校准罗盘</translation>
    </message>
    <message>
      <source>Calibration temperature for 3rd accelerometer</source>
      <translation>第三加速度计的校准温度</translation>
    </message>
    <message>
      <source>Camera ISO</source>
      <translation>相机ISO</translation>
    </message>
    <message>
      <source>Camera Shutter Speed</source>
      <translation>相机快门速度</translation>
    </message>
    <message>
      <source>Camera capture timestamping mode</source>
      <translation>相机捕获时间戳模式</translation>
    </message>
    <message>
      <source>Camera options bitmask</source>
      <translation>相机选项位掩码</translation>
    </message>
    <message>
      <source>Camera shutter duration held open</source>
      <translation>相机快门持续时间保持打开</translation>
    </message>
    <message>
      <source>Camera trigger mode</source>
      <translation>相机触发方式</translation>
    </message>
    <message>
      <source>Camera1 Video Stream Bitrate</source>
      <translation>Camera1 视频流比特率</translation>
    </message>
    <message>
      <source>Camera1 Video Stream IP Address 1</source>
      <translation>摄像头1 视频流 IP 地址 1</translation>
    </message>
    <message>
      <source>CameraFocus</source>
      <translation>相机对焦</translation>
    </message>
    <message>
      <source>Can be used for parameter tuning with the RC. This one is further referenced as the 1st parameter channel.
Set to 0 to deactivate *</source>
      <translation>可用于 RC 的参数调整。该通道进一步被称为第一个参数通道。设置为 0 以停用*</translation>
    </message>
    <message>
      <source>Can be used to alter the battery level during SITL- or HITL-simulation on the fly.
Particularly useful for testing different low-battery behaviour.</source>
      <translation>可用于在 SITL 或 HITL 仿真过程中即时更改电池电量。对于测试不同的低电量行为特别有用。</translation>
    </message>
    <message>
      <source>Causes the servo angle output to lead the current angle of the vehicle by some amount of time based on current angular rate. Increase until the servo is responsive but doesn't overshoot. Does nothing with pan stabilization enabled.</source>
      <translation>使舵机角度输出根据当前角速率领先车辆当前角度一段时间。增加直到舵机有响应但不会超调。启用平移稳定功能时不执行任何操作。</translation>
    </message>
    <message>
      <source>Ch12</source>
      <translation>Ch12</translation>
    </message>
    <message>
      <source>Ch7</source>
      <translation>Ch7</translation>
    </message>
    <message>
      <source>Chan14</source>
      <translation>陈14</translation>
    </message>
    <message>
      <source>Chan7</source>
      <translation>陈7</translation>
    </message>
    <message>
      <source>Change the uplink sensor id (SPort only)</source>
      <translation>更改上行链路传感器 ID（仅限 SPort）</translation>
    </message>
    <message>
      <source>Channel 11</source>
      <translation>通道11</translation>
    </message>
    <message>
      <source>Channel 17</source>
      <translation>频道 17</translation>
    </message>
    <message>
      <source>Channel 24</source>
      <translation>频道 24</translation>
    </message>
    <message>
      <source>Channel 31</source>
      <translation>频道 31</translation>
    </message>
    <message>
      <source>Channel 8</source>
      <translation>通道8</translation>
    </message>
    <message>
      <source>Channel to show telemetry packets-per-second value, as received at TX</source>
      <translation>显示在 TX 接收的每秒遥测数据包值的通道</translation>
    </message>
    <message>
      <source>Channel16</source>
      <translation>通道16</translation>
    </message>
    <message>
      <source>Channel23</source>
      <translation>通道23</translation>
    </message>
    <message>
      <source>Channel31</source>
      <translation>通道31</translation>
    </message>
    <message>
      <source>Channels to which ignore the safety switch state</source>
      <translation>忽略安全开关状态的通道</translation>
    </message>
    <message>
      <source>Check if the estimator detects a strong magnetic disturbance (check enabled by EKF2_MAG_CHECK)</source>
      <translation>检查估计器是否检测到强磁干扰（检查由 EKF2_MAG_CHECK 启用）</translation>
    </message>
    <message>
      <source>Choice of which constellations the receiver should use for PVT computation.
When this is 0, the constellation usage isn't changed.</source>
      <translation>选择接收器应使用哪些星座进行 PVT 计算。当这是 0 时，星座使用不会改变。</translation>
    </message>
    <message>
      <source>Circle Radius</source>
      <translation>圆半径</translation>
    </message>
    <message>
      <source>Circle mode's turn rate in deg/sec.  Positive to turn clockwise, negative for counter clockwise</source>
      <translation>圆形模式的转弯速率（以度/秒为单位）。  正为顺时针旋转，负为逆时针旋转</translation>
    </message>
    <message>
      <source>Circuit breaker for flight termination</source>
      <translation>飞行终止断路器</translation>
    </message>
    <message>
      <source>Clear Mission on reboot</source>
      <translation>重启后清除任务</translation>
    </message>
    <message>
      <source>Clockwise</source>
      <translation>顺时针</translation>
    </message>
    <message>
      <source>Coast Motors</source>
      <translation>海岸汽车</translation>
    </message>
    <message>
      <source>Coefficient describing linear relationship between Z component of magnetometer in body frame axis and either current or throttle depending on value of CAL_MAG_COMP_TYP. Unit for throttle-based compensation is [G] and for current-based compensation [G/kA]</source>
      <translation>描述机体框架轴上磁力计的 Z 分量与电流或油门之间线性关系的系数，具体取决于 CAL_MAG_COMP_TYP 的值。基于油门的补偿的单位是[G]，基于电流的补偿的单位是[G/kA]</translation>
    </message>
    <message>
      <source>Collective needed to hover expressed as a number from 0 to 1 where 0 is H_COL_MIN and 1 is H_COL_MAX</source>
      <translation>需要悬停的总距表示为从 0 到 1 的数字，其中 0 是 H_COL_MIN，1 是 H_COL_MAX</translation>
    </message>
    <message>
      <source>Collision Avoidance Behavior - Warn</source>
      <translation>避免碰撞行为 - 警告</translation>
    </message>
    <message>
      <source>Compass declination</source>
      <translation>罗盘赤纬</translation>
    </message>
    <message>
      <source>Compass device id with 3rd order priority, set automatically if 0. Reboot required after change.</source>
      <translation>罗盘设备 ID 具有第三顺序优先级，如果 0，则自动设置。更改后需要重新启动。</translation>
    </message>
    <message>
      <source>Compass maximum offset</source>
      <translation>罗盘最大偏移</translation>
    </message>
    <message>
      <source>Compass orientation</source>
      <translation>罗盘方向</translation>
    </message>
    <message>
      <source>Compass per-motor3 Y</source>
      <translation>每电机罗盘3 Y</translation>
    </message>
    <message>
      <source>Compass soft-iron off-diagonal X component</source>
      <translation>罗盘软铁非对角 X 分量</translation>
    </message>
    <message>
      <source>Compass2 offsets in milligauss on the Y axis</source>
      <translation>Compass2 Y 轴上的毫高斯偏移</translation>
    </message>
    <message>
      <source>Compass2 soft-iron off-diagonal Y component</source>
      <translation>罗盘2软铁非对角Y分量</translation>
    </message>
    <message>
      <source>Compass3 offsets in milligauss on the Z axis</source>
      <translation>Compass3 Z 轴上的毫高斯偏移</translation>
    </message>
    <message>
      <source>Compass3 soft-iron off-diagonal Z component</source>
      <translation>Compass3软铁非对角Z分量</translation>
    </message>
    <message>
      <source>Compensates for the roll angle difference between the control board and the frame. Positive values make the vehicle roll right.</source>
      <translation>补偿控制板和框架之间的侧倾角差异。正值使车辆向右横滚。</translation>
    </message>
    <message>
      <source>Compensation for Y axis of motor4</source>
      <translation>电机Y轴补偿4</translation>
    </message>
    <message>
      <source>Complimentary filter external heading weight</source>
      <translation>免费过滤器外部标题重量</translation>
    </message>
    <message>
      <source>Condition to enter the prearmed state, an intermediate state between disarmed and armed in which non-throttling actuators are active.</source>
      <translation>进入预设防状态的条件，这是介于上锁和设防之间的中间状态，其中非节流执行器处于活动状态。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run CRSF RC Input Driver.
Crossfire RC (CRSF) driver.</source>
      <translation>配置在哪个串行端口上运行 CRSF RC 输入驱动程序。 Crossfire RC (CRSF) 驱动程序。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run GHST RC Input Driver.
Ghost (GHST) RC driver.</source>
      <translation>配置在哪个串行端口上运行 GHST RC 输入驱动程序。 Ghost (GHST) RC 驱动程序。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Lightware Laser Rangefinder (serial).</source>
      <translation>配置在哪个串行端口上运行 Lightware 激光测距仪（串行）。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Roboclaw Driver.</source>
      <translation>配置在哪个串行端口上运行 Roboclaw 驱动程序。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Ultrawideband position sensor driver.</source>
      <translation>配置在哪个串行端口上运行超宽带位置传感器驱动程序。</translation>
    </message>
    <message>
      <source>Configure the ATXXXX OSD Chip (mounted on the OmnibusF4SD board) and select the transmission standard.</source>
      <translation>配置 ATXXXX OSD 芯片（安装在 OmnibusF4SD 板上）并选择传输标准。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the GPS 3 Serial Port. Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 GPS 3 串口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the TELEM 3 Serial Port. Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 TELEM 3 串口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the maximum sending rate for the MAVLink streams in Bytes/sec.
If the configured streams exceed the maximum rate, the sending rate of
each stream is automatically decreased.
If this is set to 0 a value of half of the theoretical maximum bandwidth is used.
This corresponds to baudrate/20 Bytes/s (baudrate/10 = maximum data rate on
8N1-configured links).</source>
      <translation>配置 MAVLink 流的最大发送速率（以字节/秒为单位）。如果配置的码流超过最大速率，则各码流的发送速率会自动降低。如果设置为 0，则使用理论最大带宽一半的值。这对应于波特率/20 字节/秒（波特率/10 = 8N1 配置链路上的最大数据速率）。</translation>
    </message>
    <message>
      <source>Configured fence types held as bitmask. Max altitide, Circle and Polygon fences will be immediately enabled if configured. Min altitude fence will only be enabled once the minimum altitude is reached.</source>
      <translation>配置的栅栏类型保存为位掩码。如果配置，最大高度、圆形和多边形围栏将立即启用。只有达到最低高度后，最低高度围栏才会启用。</translation>
    </message>
    <message>
      <source>Configures whether the driver outputs only raw sensor output (the default), or additionally supplies INS data such as position and velocity estimates.</source>
      <translation>配置驱动程序是仅输出原始传感器输出（默认），还是另外提供 INS 数据，例如位置和速度估计。</translation>
    </message>
    <message>
      <source>ConstantThrust</source>
      <translation>恒定推力</translation>
    </message>
    <message>
      <source>Continue if in Auto on RC failsafe only</source>
      <translation>如果仅在 RC 故障安全上处于自动状态，则继续</translation>
    </message>
    <message>
      <source>ContinuousRotation</source>
      <translation>连续旋转</translation>
    </message>
    <message>
      <source>Control Surface 0 type</source>
      <translation>控制面0型</translation>
    </message>
    <message>
      <source>Control Surface 1 type</source>
      <translation>控制面1型</translation>
    </message>
    <message>
      <source>Control Surface 2 type</source>
      <translation>控制面2型</translation>
    </message>
    <message>
      <source>Control Surface 3 type</source>
      <translation>控制面3型</translation>
    </message>
    <message>
      <source>Control Surface 4 type</source>
      <translation>控制面4型</translation>
    </message>
    <message>
      <source>Control Surface 5 type</source>
      <translation>控制面5型</translation>
    </message>
    <message>
      <source>Control Surface 6 type</source>
      <translation>控制面6型</translation>
    </message>
    <message>
      <source>Control Surface 7 type</source>
      <translation>控制面7型</translation>
    </message>
    <message>
      <source>Control height 0: relative earth frame origin which may drift due to sensors 1: relative to ground (requires distance sensor) which changes with terrain variation. It will revert to relative earth frame if the distance to ground estimate becomes invalid. 2: relative to ground (requires distance sensor) when stationary and relative to earth frame when moving horizontally. The speed threshold is MPC_HOLD_MAX_XY</source>
      <translation>控制高度 0：相对地球坐标系原点，可能因传感器而漂移 1：相对于地面（需要距离传感器），随地形变化而变化。如果到地面的距离估计无效，它将恢复到相对地球坐标系。 2：静止时相对于地面（需要距离传感器），水平移动时相对于地球框架。速度阈值为MPC_HOLD_MAX_XY</translation>
    </message>
    <message>
      <source>Control over what function is used to calculate distance. For a linear function, the distance is (voltage-offset)*scaling. For a inverted function the distance is (offset-voltage)*scaling. For a hyperbolic function the distance is scaling/(voltage-offset). The functions return the distance in meters.</source>
      <translation>控制使用什么函数来计算距离。对于线性函数，距离为（电压-偏移）*缩放比例。对于反函数，距离为（偏移电压）*缩放比例。对于双曲函数，距离为缩放比例/（电压偏移）。这些函数返回以米为单位的距离。</translation>
    </message>
    <message>
      <source>Control what protocol Serial4 port should be used for. Note that the Frsky options require external converter hardware. See the wiki for details.</source>
      <translation>控制 Serial4 端口应使用什么协议。请注意，Frsky 选项需要外部转换器硬件。详细信息请参阅维基百科。</translation>
    </message>
    <message>
      <source>Control what protocol to use on the console.</source>
      <translation>控制在控制台上使用什么协议。</translation>
    </message>
    <message>
      <source>Controller throttle damping</source>
      <translation>控制器油门阻尼</translation>
    </message>
    <message>
      <source>Controls how often the GPS should provide a position update. Lowering below 5Hz(default) is not allowed. Raising the rate above 5Hz usually provides little benefit and for some GPS (eg Ublox M9N) can severely impact performance.Renamed in 4.6 and later to GPS1_RATE_MS</source>
      <translation>控制 GPS 提供位置更新的频率。不允许降低到 5Hz（默认）以下。将速率提高到 5Hz 以上通常没有什么好处，并且对于某些 GPS（例如 Ublox M9N）可能会严重影响性能。在 4.6 中重命名，后来更名为 GPS1_RATE_MS</translation>
    </message>
    <message>
      <source>Controls maximum apparent flow rate in flowhold</source>
      <translation>控制流量中的最大视在流量</translation>
    </message>
    <message>
      <source>Controls the action that will be taken when an EKF failsafe is invoked</source>
      <translation>控制调用 EKF 故障保护时将采取的操作</translation>
    </message>
    <message>
      <source>Controls the type of moving base used if using moving base.This is renamed in 4.6 and later to GPSx_MB_TYPE.</source>
      <translation>如果使用移动底座，则控制所使用的移动底座的类型。这在 4.6 中重命名，后来更名为 GPSx_MB_TYPE。</translation>
    </message>
    <message>
      <source>Controls what action to take if internal temperature exceeds FS_TEMP_MAX parameter.</source>
      <translation>控制内部温度超过 FS_TEMP_MAX 参数时采取的操作。</translation>
    </message>
    <message>
      <source>Controls whether ArduCopter will enter ESC calibration on the next restart.  Do not adjust this parameter manually.</source>
      <translation>控制ArduCopter是否在下次重启时进入ESC校准。  请勿手动调整该参数。</translation>
    </message>
    <message>
      <source>Controls which parameters (normally PID gains) are being tuned with transmitter's channel 6 knob</source>
      <translation>控制使用发射机通道 6 旋钮调节哪些参数（通常是 PID 增益）</translation>
    </message>
    <message>
      <source>Copter switch to this mode after GPS recovers or DR_FLY_TIMEOUT has elapsed.  Default is 6/RTL.  Set to -1 to return to mode used before deadreckoning was triggered</source>
      <translation>当 GPS 恢复或 DR_FLY_TIMEOUT 超时后，飞行器切换到该模式。  默认为 6/RTL。  设置为 -1 以返回触发航位推算之前使用的模式</translation>
    </message>
    <message>
      <source>Counter-Clockwise</source>
      <translation>逆时针</translation>
    </message>
    <message>
      <source>Crash check enable</source>
      <translation>启用碰撞检查</translation>
    </message>
    <message>
      <source>Critical battery failsafe action</source>
      <translation>关键电池故障安全措施</translation>
    </message>
    <message>
      <source>Crossfire telemetry enable</source>
      <translation>交叉火力遥测启用</translation>
    </message>
    <message>
      <source>CubeOrange</source>
      <translation>立方橙</translation>
    </message>
    <message>
      <source>Custom 2</source>
      <translation>定制2</translation>
    </message>
    <message>
      <source>Custom control type to be used</source>
      <translation>要使用的自定义控件类型</translation>
    </message>
    <message>
      <source>Custom pitch</source>
      <translation>定制间距</translation>
    </message>
    <message>
      <source>Cut off throttle at this alt.</source>
      <translation>在此高度处切断油门。</translation>
    </message>
    <message>
      <source>CygbotD1</source>
      <translation>CYGbotD1</translation>
    </message>
    <message>
      <source>Cyphal ESC 5 zubax feedback port ID</source>
      <translation>Cyphal ESC 5 zubax 反馈端口 ID</translation>
    </message>
    <message>
      <source>Cyphal legacy battery port ID</source>
      <translation>Cyphal 传统电池端口 ID</translation>
    </message>
    <message>
      <source>DDFP Tail Rotor Battery compensation index</source>
      <translation>DDFP尾桨电池补偿指标</translation>
    </message>
    <message>
      <source>DDS namespace</source>
      <translation>DDS命名空间</translation>
    </message>
    <message>
      <source>DIA_X in the compass3 soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>罗盘 3 软铁校准矩阵中的 DIA_X 元素：[[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</translation>
    </message>
    <message>
      <source>DIA_Z in the compass3 soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>罗盘 3 软铁校准矩阵中的 DIA_Z 元素：[[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</translation>
    </message>
    <message>
      <source>DJIRS2 debug</source>
      <translation>DJIRS2调试</translation>
    </message>
    <message>
      <source>DSM</source>
      <translation>DSM</translation>
    </message>
    <message>
      <source>DShot300</source>
      <translation>DShot300</translation>
    </message>
    <message>
      <source>Damping gain for the throttle demand loop during an auto-landing. Same as TECS_THR_DAMP but only in effect during an auto-land. Increase to add throttle activity to dampen oscillations in speed and height. When set to 0 landing throttle damping is controlled by TECS_THR_DAMP.</source>
      <translation>自动着陆期间油门需求回路的阻尼增益。与 TECS_THR_DAMP 相同，但仅在自动着陆期间有效。增加油门活动以抑制速度和高度的波动。设置为 0 时，着陆油门阻尼由 TECS_THR_DAMP 控制。</translation>
    </message>
    <message>
      <source>Data sheet noise density = 150ug/sqrt(Hz) = 0.0015 m/s^2/sqrt(Hz)
Larger than data sheet to account for tilt error.</source>
      <translation>数据表噪声密度 = 150ug/sqrt(Hz) = 0.0015 m/s^2/sqrt(Hz) 大于数据表以考虑倾斜误差。</translation>
    </message>
    <message>
      <source>DeadReckon Failsafe Action</source>
      <translation>DeadReckon 故障安全操作</translation>
    </message>
    <message>
      <source>Deadreckoning GPS check trigger seconds</source>
      <translation>航位推算 GPS 检查触发秒数</translation>
    </message>
    <message>
      <source>Debug</source>
      <translation>调试</translation>
    </message>
    <message>
      <source>Deepstall L1 I gain</source>
      <translation>深失速 L1 I 增益</translation>
    </message>
    <message>
      <source>Deepstall forward velocity</source>
      <translation>深失速前进速度</translation>
    </message>
    <message>
      <source>Deepstall yaw rate limit</source>
      <translation>深失速偏航率限制</translation>
    </message>
    <message>
      <source>Default downlink sensor id</source>
      <translation>默认下行传感器 ID</translation>
    </message>
    <message>
      <source>Default orientation of Yaw 90°</source>
      <translation>偏航默认方向 90°</translation>
    </message>
    <message>
      <source>Default take-off altitude</source>
      <translation>默认起飞高度</translation>
    </message>
    <message>
      <source>Default+IMU</source>
      <translation>默认+IMU</translation>
    </message>
    <message>
      <source>Defined as corrective acceleration in m/s^2 per m velocity integral</source>
      <translation>定义为每米速度积分的修正加速度（以 m/s^2 为单位）</translation>
    </message>
    <message>
      <source>Defines an index-based namespace for DDS messages, e.g, uav_0, uav_1, up to uav_9999 A value less than zero leaves the namespace empty</source>
      <translation>为 DDS 消息定义基于索引的命名空间，例如 uav_0、uav_1，直到 uav_9999
A 值小于零，使命名空间为空</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL13_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL13_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL20_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL20_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL28_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL28_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL35_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL35_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL42_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL42_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL4_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL4_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL57_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL57_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL6_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL6_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL12_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL12_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL1_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL1_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL27_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL27_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL34_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL34_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL41_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL41_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL49_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL49_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL56_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL56_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL63_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL63_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL11_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL11_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL19_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL19_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL26_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL26_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL33_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL33_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL40_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL40_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL48_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL48_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL55_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL55_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL62_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL62_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL10_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL10_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL18_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL18_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL25_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL25_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL32_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL32_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL3_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL3_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL47_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL47_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL54_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL54_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL61_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL61_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the capacity of battery 1 in mAh.</source>
      <translation>定义电池 1 的容量（以 mAh 为单位）。</translation>
    </message>
    <message>
      <source>Defines the collective pitch at the interval position 2 for a given thrust setpoint. Use negative values if the swash plate needs to move down to provide upwards thrust.</source>
      <translation>定义给定推力设定值的间隔位置 2 处的总距桨距。如果斜盘需要向下移动以提供向上的推力，请使用负值。</translation>
    </message>
    <message>
      <source>Defines the distance from a waypoint, that when crossed indicates the wp has been hit.</source>
      <translation>定义距航路点的距离，穿过该距离时表示 wp 已被击中。</translation>
    </message>
    <message>
      <source>Defines the maximum cornering acceleration in cm/s/s used during missions, zero uses max lean angle.</source>
      <translation>定义任务期间使用的最大转弯加速度（以厘米/秒/秒为单位），零使用最大倾斜角度。</translation>
    </message>
    <message>
      <source>Defines the offset of the ship's landing pad w.r.t. the ship's origin, i.e. where the beacon is placed on the ship (Y-axis)</source>
      <translation>定义船舶着陆场相对于地面的偏移量。船舶的原点，即信标放置在船舶上的位置（Y 轴）</translation>
    </message>
    <message>
      <source>Defines the radius of the loiter circle when in RTL mode. If this is zero then WP_LOITER_RAD is used. If the radius is negative then a counter-clockwise is used. If positive then a clockwise loiter is used.</source>
      <translation>定义处于 RTL 模式时盘旋圆的半径。如果该值为零，则使用 WP_LOITER_RAD。如果半径为负，则使用逆时针方向。如果为正，则使用顺时针盘旋。</translation>
    </message>
    <message>
      <source>Defines the speed in cm/s which the aircraft will attempt to maintain horizontally while flying home. If this is set to zero, WPNAV_SPEED will be used instead.</source>
      <translation>定义飞机在返航时尝试保持水平的速度（以厘米/秒为单位）。如果设置为零，则将使用 WPNAV_SPEED。</translation>
    </message>
    <message>
      <source>Defines the vertical acceleration in cm/s/s used during missions</source>
      <translation>定义任务期间使用的垂直加速度（以 cm/s/s 为单位）</translation>
    </message>
    <message>
      <source>Defines which axes will be tuned during the auto-tuning sequence
Set bits in the following positions to enable:
0 : Roll
1 : Pitch
2 : Yaw</source>
      <translation>定义在自动调谐序列期间将调谐哪些轴 将以下位置的位设置为启用： 0：横滚 1：俯仰 2：偏航</translation>
    </message>
    <message>
      <source>Degrees of steering wheel to derate at each additional m/s of speed above "Derating speed". Should be set so that at higher speeds the plane does not roll to the wing in turns.</source>
      <translation>高于“降额速度”的速度每增加 m/s 时方向盘降额的度数。应进行设置，以便在较高速度下飞机不会轮流向机翼侧滚。</translation>
    </message>
    <message>
      <source>Delay before automatic disarm in seconds. A value of zero disables auto disarm.</source>
      <translation>自动上锁前的延迟（以秒为单位）。值为零将禁用自动上锁。</translation>
    </message>
    <message>
      <source>Delay between start attempts</source>
      <translation>启动尝试之间的延迟</translation>
    </message>
    <message>
      <source>Deploy Parachute</source>
      <translation>展开降落伞</translation>
    </message>
    <message>
      <source>Descend to this altitude (above destination position) after return, and wait for time defined in RTL_LAND_DELAY.
Land (i.e. slowly descend) from this altitude if autolanding allowed.
VTOLs do transition to hover in this altitdue above the landing point.</source>
      <translation>返回后下降到该高度（目的地位置以上），并等待RTL_LAND_DELAY中定义的时间。如果允许自动着陆，则从该高度着陆（即缓慢下降）。垂直起降飞机确实会在着陆点上方的这个高度上转变为悬停。</translation>
    </message>
    <message>
      <source>Desired baud rate</source>
      <translation>所需波特率</translation>
    </message>
    <message>
      <source>Determines how agressively LOITER tries to correct for drift from loiter point. Higher is faster but default should be acceptable.</source>
      <translation>确定 LOITER 尝试纠正盘旋点漂移的积极程度。越高速度越快，但默认值应该可以接受。</translation>
    </message>
    <message>
      <source>Determines the slope between expected steady state current and linearized, normalized thrust command.
E.g. FD_ACT_MOT_C2T A represents the expected steady state current at 100%.
FD_ACT_LOW_OFF and FD_ACT_HIGH_OFF offset the threshold from that slope.</source>
      <translation>确定预期稳态电流与线性化、归一化推力命令之间的斜率。例如。 FD_ACT_MOT_C2T A 代表 100% 的预期稳态电流。 FD_ACT_LOW_OFF 和 FD_ACT_HIGH_OFF 从该斜率偏移阈值。</translation>
    </message>
    <message>
      <source>Developer</source>
      <translation>开发商</translation>
    </message>
    <message>
      <source>Device ID of simulated compass 7</source>
      <translation>模拟罗盘7的装置ID</translation>
    </message>
    <message>
      <source>Diameter of the circle the ship is traveling on</source>
      <translation>船舶行驶圆周的直径</translation>
    </message>
    <message>
      <source>Differential spoiler and crow flaps options</source>
      <translation>差速扰流板和乌鸦襟翼选项</translation>
    </message>
    <message>
      <source>Differential-Collective-Pitch Yaw Mixing</source>
      <translation>差分总距变桨偏航混合</translation>
    </message>
    <message>
      <source>Digital pin number for 2nd relay control.</source>
      <translation>第二个继电器控制的数字引脚号。</translation>
    </message>
    <message>
      <source>Digital pin number for 6th relay control.</source>
      <translation>第 6 个继电器控制的数字引脚号。</translation>
    </message>
    <message>
      <source>Digital pin number for relay control. Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.</source>
      <translation>用于继电器控制的数字引脚号。给出了一些常见值，但请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。</translation>
    </message>
    <message>
      <source>Dijkstra</source>
      <translation>迪克斯特拉</translation>
    </message>
    <message>
      <source>DirectDrive FixedPitch CCW</source>
      <translation>直接驱动固定俯仰 CCW</translation>
    </message>
    <message>
      <source>Direction unchanged when backing up</source>
      <translation>备份时方向不变</translation>
    </message>
    <message>
      <source>Disable FLTD update by Autotune</source>
      <translation>通过自动调谐禁用 FLTD 更新</translation>
    </message>
    <message>
      <source>Disable barometer in SITL</source>
      <translation>禁用 SITL 中的气压计</translation>
    </message>
    <message>
      <source>Disable on sensor failure</source>
      <translation>传感器故障时禁用</translation>
    </message>
    <message>
      <source>Disable speed based Qassist when using synthetic airspeed estimates</source>
      <translation>使用合成空速估计时禁用基于速度的 Qassist</translation>
    </message>
    <message>
      <source>Disable thrust loss detection in transtions and fixed wing modes. Thrust loss detection will only run in VTOL modes.</source>
      <translation>禁用转换和固定翼模式下的推力损失检测。推力损失检测仅在 VTOL 模式下运行。</translation>
    </message>
    <message>
      <source>DisableCrosshair</source>
      <translation>禁用十字线</translation>
    </message>
    <message>
      <source>DisablePPPEchoLimit</source>
      <translation>禁用PPPEchoLimit</translation>
    </message>
    <message>
      <source>Disabled/NoAction</source>
      <translation>禁用／无动作</translation>
    </message>
    <message>
      <source>Disallow arming</source>
      <translation>禁止解锁</translation>
    </message>
    <message>
      <source>Discard log on reboot if never armed</source>
      <translation>如果从未装备，则在重新启动时丢弃日志</translation>
    </message>
    <message>
      <source>Displays GPS latitude</source>
      <translation>显示 GPS 纬度</translation>
    </message>
    <message>
      <source>Displays Receiver link quality</source>
      <translation>显示接收器链路质量</translation>
    </message>
    <message>
      <source>Displays arming status (MSP OSD only)</source>
      <translation>显示解锁状态（仅限 MSP OSD）</translation>
    </message>
    <message>
      <source>Displays battery2 voltage</source>
      <translation>显示电池2电压</translation>
    </message>
    <message>
      <source>Displays distance and relative direction to HOME</source>
      <translation>显示到 HOME 的距离和相对方向</translation>
    </message>
    <message>
      <source>Displays heading</source>
      <translation>显示标题</translation>
    </message>
    <message>
      <source>Displays number of acquired satellites</source>
      <translation>显示捕获的卫星数量</translation>
    </message>
    <message>
      <source>Displays temperature reported by primary airspeed sensor</source>
      <translation>显示主空速传感器报告的温度</translation>
    </message>
    <message>
      <source>Displays total flight time</source>
      <translation>显示总飞行时间</translation>
    </message>
    <message>
      <source>Distance Fail Z</source>
      <translation>距离失败 Z</translation>
    </message>
    <message>
      <source>Distance between terrain grid points in meters. This controls the horizontal resolution of the terrain data that is stored on te SD card and requested from the ground station. If your GCS is using the worldwide SRTM database then a resolution of 100 meters is appropriate. Some parts of the world may have higher resolution data available, such as 30 meter data available in the SRTM database in the USA. The grid spacing also controls how much data is kept in memory during flight. A larger grid spacing will allow for a larger amount of data in memory. A grid spacing of 100 meters results in the vehicle keeping 12 grid squares in memory with each grid square having a size of 2.7 kilometers by 3.2 kilometers. Any additional grid squares are stored on the SD once they are fetched from the GCS and will be demand loaded as needed.</source>
      <translation>地形网格点之间的距离（以米为单位）。它控制存储在 SD 卡上并从地面站请求的地形数据的水平分辨率。如果您的 GCS 使用全球 SRTM 数据库，则 100 仪表的分辨率是合适的。世界上的某些地区可能有更高分辨率的数据可用，例如 USA 中的 SRTM 数据库中可用的 30 仪表数据。网格间距还控制飞行期间内存中保存的数据量。较大的网格间距将允许内存中存储更多的数据。 100 米的网格间距导致车辆在内存中保留 12 网格方块，每个网格方块的大小为 2.7 公里 x 3.2 公里。任何额外的网格方块一旦从 GCS 获取，就会存储在 SD 上，并将根据需要按需加载。</translation>
    </message>
    <message>
      <source>Distance from the front to the rear axle.</source>
      <translation>前轴到后轴的距离。</translation>
    </message>
    <message>
      <source>Distance to thermal center</source>
      <translation>距热中心距离</translation>
    </message>
    <message>
      <source>Do not land (just Hover) (Very Strict)</source>
      <translation>不要着陆（仅悬停）（非常严格）</translation>
    </message>
    <message>
      <source>DoNotIncludeHome</source>
      <translation>不包括家</translation>
    </message>
    <message>
      <source>Don't Use</source>
      <translation>不要使用</translation>
    </message>
    <message>
      <source>DontDisableAirspeedUsingEKF</source>
      <translation>不要使用 EKF 禁用空速</translation>
    </message>
    <message>
      <source>Driver1</source>
      <translation>驱动1</translation>
    </message>
    <message>
      <source>DroneCAN</source>
      <translation>无人机CAN</translation>
    </message>
    <message>
      <source>DroneCAN Hardpoint ID 14</source>
      <translation>DroneCAN 硬点 ID 14</translation>
    </message>
    <message>
      <source>DroneCAN Hardpoint ID 8</source>
      <translation>DroneCAN 硬点 ID 8</translation>
    </message>
    <message>
      <source>DroneCAN node ID used by the driver itself on this network</source>
      <translation>该网络上驱动程序本身使用的 DroneCAN 节点 ID</translation>
    </message>
    <message>
      <source>DroneCAN-MovingBaseline-Rover</source>
      <translation>DroneCAN 移动基线移动端</translation>
    </message>
    <message>
      <source>Durandal</source>
      <translation>杜兰达尔</translation>
    </message>
    <message>
      <source>Duration of inactivity after which SLCAN is switched back to original protocol in seconds.</source>
      <translation>SLCAN 切换回原始协议之前的不活动持续时间（以秒为单位）。</translation>
    </message>
    <message>
      <source>Dynamic Scripting Matrix</source>
      <translation>动态脚本矩阵</translation>
    </message>
    <message>
      <source>EFF_Y</source>
      <translation>EFF_Y</translation>
    </message>
    <message>
      <source>EFI INF driver options</source>
      <translation>EFI INF 驱动程序选项</translation>
    </message>
    <message>
      <source>EFI communication type</source>
      <translation>EFI通讯型</translation>
    </message>
    <message>
      <source>EKF Source ExternalNav Quality Threshold</source>
      <translation>EKF 源ExternalNav 质量阈值</translation>
    </message>
    <message>
      <source>EKF lane switch attempt</source>
      <translation>EKF 车道切换尝试</translation>
    </message>
    <message>
      <source>EKF2 enable</source>
      <translation>EKF2 使能</translation>
    </message>
    <message>
      <source>EKF_MAG_CAL = 0 enables calibration based on flying speed and altitude and is the default setting for Plane users. EKF_MAG_CAL = 1 enables calibration based on manoeuvre level and is the default setting for Copter and Rover users. EKF_MAG_CAL = 2 prevents magnetometer calibration regardless of flight condition and is recommended if in-flight magnetometer calibration is unreliable.</source>
      <translation>EKF_MAG_CAL = 0：根据飞行速度和高度进行校准，这是固定翼的默认设置。EKF_MAG_CAL = 1：根据机动程度进行校准，这是多旋翼和无人车的默认设置。EKF_MAG_CAL = 2：在任何飞行状态下均禁止磁力计校准；飞行中磁力计校准不可靠时，建议使用此设置。</translation>
    </message>
    <message>
      <source>EPV (EKF2_REQ_EPV)</source>
      <translation>垂直位置误差（EKF2_REQ_EPV）</translation>
    </message>
    <message>
      <source>ESC 12</source>
      <translation>ESC 12</translation>
    </message>
    <message>
      <source>ESC 2</source>
      <translation>ESC 2</translation>
    </message>
    <message>
      <source>ESC 27</source>
      <translation>ESC 27</translation>
    </message>
    <message>
      <source>ESC 5</source>
      <translation>ESC 5</translation>
    </message>
    <message>
      <source>ESC RPM number of harmonics (multiples of RPM) for ESC RPM dynamic notch filtering.</source>
      <translation>ESC RPM ESC RPM 动态陷波滤波的谐波数量（RPM 的倍数）。</translation>
    </message>
    <message>
      <source>ESC Telemetry mavlink offset</source>
      <translation>ESC 遥测 mavlink 偏移量</translation>
    </message>
    <message>
      <source>ESCRPM_EN</source>
      <translation>ESCRPM_EN</translation>
    </message>
    <message>
      <source>EV noise parameters</source>
      <translation>EV噪声参数</translation>
    </message>
    <message>
      <source>Eeprom format version number</source>
      <translation>Eeprom 格式版本号</translation>
    </message>
    <message>
      <source>Emergency Kill switch channel</source>
      <translation>紧急停机开关通道</translation>
    </message>
    <message>
      <source>Emitter type</source>
      <translation>发射器类型</translation>
    </message>
    <message>
      <source>Enable ADS-B</source>
      <translation>启用ADS-B</translation>
    </message>
    <message>
      <source>Enable Avoidance using ADSB</source>
      <translation>使用 ADSB 启用回避</translation>
    </message>
    <message>
      <source>Enable DDS subsystem</source>
      <translation>启用 DDS 子系统</translation>
    </message>
    <message>
      <source>Enable EFI throttle linearisation</source>
      <translation>启用 EFI 油门线性化</translation>
    </message>
    <message>
      <source>Enable FMU SD card hardfault streaming</source>
      <translation>启用 FMU SD 卡硬故障流</translation>
    </message>
    <message>
      <source>Enable HITL/SIH mode on next boot</source>
      <translation>下次启动时启用 HITL/SIH 模式</translation>
    </message>
    <message>
      <source>Enable I2C output protocol UBX</source>
      <translation>使能 I2C 输出协议 UBX</translation>
    </message>
    <message>
      <source>Enable MAVLink Message forwarding for instance 0</source>
      <translation>为实例 0 启用 MAVLink 消息转发</translation>
    </message>
    <message>
      <source>Enable MicroStrain external heading aiding</source>
      <translation>启用 MicroStrain 外部航向辅助</translation>
    </message>
    <message>
      <source>Enable ODID subsystem</source>
      <translation>启用 ODID 子系统</translation>
    </message>
    <message>
      <source>Enable RawIMU pub</source>
      <translation>启用 RawIMU 酒吧</translation>
    </message>
    <message>
      <source>Enable Tiltrotor functionality</source>
      <translation>启用倾斜旋转器功能</translation>
    </message>
    <message>
      <source>Enable UAVCAN IMU subscription.
uavcan::equipment::ahrs::RawIMU</source>
      <translation>启用 UAVCAN IMU 订阅。无人机::设备::ahrs::RawIMU</translation>
    </message>
    <message>
      <source>Enable UAVCAN barometer subscription.
uavcan::equipment::air_data::StaticPressure
uavcan::equipment::air_data::StaticTemperature</source>
      <translation>启用UAVCAN气压计订阅。 uavcan::设备::air_data::StaticPressure uavcan::equipment::air_data::StaticTemperature</translation>
    </message>
    <message>
      <source>Enable UAVCAN fuel tank status subscription.</source>
      <translation>启用UAVCAN油箱状态订阅。</translation>
    </message>
    <message>
      <source>Enable UAVCAN range finder subscription.
uavcan::equipment::range_sensor::Measurement</source>
      <translation>启用 UAVCAN 测距仪订阅。 uavcan::设备::range_sensor::测量</translation>
    </message>
    <message>
      <source>Enable airspeed sensor support</source>
      <translation>启用空速传感器支持</translation>
    </message>
    <message>
      <source>Enable auto start of rate gyro thermal calibration at the next power up</source>
      <translation>在下次通电时启用速率陀螺仪热校准的自动启动</translation>
    </message>
    <message>
      <source>Enable bank of dynamically updating notch filters.
Requires ESC RPM feedback or onboard FFT (IMU_GYRO_FFT_EN).</source>
      <translation>启用动态更新陷波滤波器组。需要 ESC RPM 反馈或板载 FFT (IMU_GYRO_FFT_EN)。</translation>
    </message>
    <message>
      <source>Enable constant position fusion during engine warmup</source>
      <translation>在发动机预热期间启用恒定位置融合</translation>
    </message>
    <message>
      <source>Enable flow control on serial 1 (telemetry 1). You must have the RTS and CTS pins connected to your radio. The standard DF13 6 pin connector for a 3DR radio does have those pins connected. If this is set to 2 then flow control will be auto-detected by checking for the output buffer filling on startup. Note that the PX4v1 does not have hardware flow control pins on this port, so you should leave this disabled.</source>
      <translation>启用串行 1（遥测 1）上的流量控制。您必须将 RTS 和 CTS 引脚连接到您的无线电。用于 3DR 无线电的标准 DF13 6 引脚连接器确实已连接这些引脚。如果设置为 2，则将通过在启动时检查输出缓冲区填充来自动检测流量控制。请注意，PX4v1 在该端口上没有硬件流控制引脚，因此您应该将其禁用。</translation>
    </message>
    <message>
      <source>Enable flow control on serial 7. You must have the RTS and CTS pins connected to your radio. The standard DF13 6 pin connector for a 3DR radio does have those pins connected. If this is set to 2 then flow control will be auto-detected by checking for the output buffer filling on startup.</source>
      <translation>启用串行 7 上的流量控制。您必须将 RTS 和 CTS 引脚连接到您的无线电。用于 3DR 无线电的标准 DF13 6 引脚连接器确实已连接这些引脚。如果设置为 2，则将通过在启动时检查输出缓冲区填充来自动检测流量控制。</translation>
    </message>
    <message>
      <source>Enable ground control station telemetry failsafe. When enabled the Rover will execute the FS_ACTION when it fails to receive MAVLink heartbeat packets for FS_TIMEOUT seconds.</source>
      <translation>启用地面控制站遥测故障保护。启用后，当 Rover 在 FS_TIMEOUT 秒内未能接收到 MAVLink 心跳数据包时，它将执行 FS_ACTION。</translation>
    </message>
    <message>
      <source>Enable logging of information needed for Replay</source>
      <translation>启用重播所需信息的记录</translation>
    </message>
    <message>
      <source>Enable of BLHeli pass-thru servo protocol support to specific channels. This mask is in addition to motors enabled using SERVO_BLH_AUTO (if any)</source>
      <translation>启用对特定通道的 BLHeli 直通舵机协议支持。该掩码是对使用 SERVO_BLH_AUTO 启用的电机的补充（如果有）</translation>
    </message>
    <message>
      <source>Enable or disable the automatic calculation of the declination based on gps location</source>
      <translation>启用或禁用根据 GPS 位置自动计算磁偏角</translation>
    </message>
    <message>
      <source>Enable or disable the second compass for determining heading.</source>
      <translation>启用或禁用第二个罗盘以确定航向。</translation>
    </message>
    <message>
      <source>Enable outputs 17 to 31</source>
      <translation>使能输出 17 至 31</translation>
    </message>
    <message>
      <source>Enable publication of satellite info (ORB_ID(satellite_info)) if possible. Not available on MTK.</source>
      <translation>如果可能，启用卫星信息 (ORB_ID(satellite_info)) 的发布。不适用于 MTK。</translation>
    </message>
    <message>
      <source>Enable sat info (if available)</source>
      <translation>启用卫星信息（如果可用）</translation>
    </message>
    <message>
      <source>Enable setting</source>
      <translation>启用设置</translation>
    </message>
    <message>
      <source>Enable simulated airspeed sensor instance</source>
      <translation>启用模拟空速传感器实例</translation>
    </message>
    <message>
      <source>Enable stall prevention</source>
      <translation>启用失速预防</translation>
    </message>
    <message>
      <source>Enable temperature calibration. Set to 0 to disable. Set to 1 to use learned values. Set to 2 to learn new values and use the values</source>
      <translation>启用温度校准。设置为 0 以禁用。设置为 1 以使用学习值。设置为 2 以学习新值并使用这些值</translation>
    </message>
    <message>
      <source>Enable the use of synthetic airspeed</source>
      <translation>启用合成空速的使用</translation>
    </message>
    <message>
      <source>Enable uXRCE-DDS timestamp synchronization</source>
      <translation>启用uXRCE-DDS时间戳同步</translation>
    </message>
    <message>
      <source>Enable weather vaning.  When active, the aircraft will automatically yaw into wind when in a VTOL position controlled mode. Pilot yaw commands override the weathervaning action.</source>
      <translation>启用天气风向。  激活后，飞机将在 VTOL 位置控制模式下自动迎风偏航。飞行员偏航命令优先于风向标动作。</translation>
    </message>
    <message>
      <source>Enable/Disable Solo Oreo LED driver, 0 to disable, 1 for Aircraft theme, 2 for Rover theme</source>
      <translation>启用/禁用 Solo Oreo LED 驱动程序、禁用 0、用于飞机主题的 1、用于流动站主题的 2</translation>
    </message>
    <message>
      <source>Enable/disable event task for displaying the vehicle status using arm-mounted
LEDs. When enabled and if the vehicle supports it, LEDs will flash
indicating various vehicle status changes. Currently PX4 has not implemented
any specific status events.
-</source>
      <translation>启用/禁用使用安装在手臂上的 LED 显示车辆状态的事件任务。启用后，如果车辆支持，LED 将闪烁，指示各种车辆状态变化。目前PX4还没有实现任何特定的状态事件。 -</translation>
    </message>
    <message>
      <source>EnableCanfd</source>
      <translation>启用Canfd</translation>
    </message>
    <message>
      <source>EnableLandResponsition</source>
      <translation>启用土地响应</translation>
    </message>
    <message>
      <source>EnableVersion1</source>
      <translation>启用版本1</translation>
    </message>
    <message>
      <source>Enabled Auto DO_LAND_START/DO_RETURN_PATH_START or RTL</source>
      <translation>任务降落／返航路径或返航</translation>
    </message>
    <message>
      <source>Enabled always SmartRTL or RTL</source>
      <translation>始终智能返航或返航</translation>
    </message>
    <message>
      <source>Enabled if above MPC_LAND_ALT2 (except LANDING)</source>
      <translation>如果高于 MPC_LAND_ALT2，则启用（LANDING 除外）</translation>
    </message>
    <message>
      <source>Enabled-Fixed</source>
      <translation>启用-固定</translation>
    </message>
    <message>
      <source>Enables Tricks on Switch. TRIK params hidden until enabled</source>
      <translation>在 Switch 上启用技巧。 TRIK 参数在启用之前隐藏</translation>
    </message>
    <message>
      <source>Enables display of altitude AGL</source>
      <translation>启用高度显示 AGL</translation>
    </message>
    <message>
      <source>Enables or disables the high sensitivity mode for the u-blox jamming detection (CFG-SEC-JAMDET_SENSITIVITY_HI). When enabled, the receiver uses a more sensitive algorithm to detect jamming. Disabling this may reduce false positives in electrically noisy environments.</source>
      <translation>启用或禁用 u-blox 干扰检测 (CFG-SEC-JAMDET_SENSITIVITY_HI) 的高灵敏度模式。启用后，接收器使用更灵敏的算法来检测干扰。禁用此功能可能会减少电噪声环境中的误报。</translation>
    </message>
    <message>
      <source>Enables the RPM capture module to estimate RPM from pulses detected on a PWM pin configured as "RPM Input".</source>
      <translation>使 RPM 捕获模块能够根据在配置为“RPM 输入”的 PWM 引脚上检测到的脉冲来估计 RPM。</translation>
    </message>
    <message>
      <source>Enabling this option starts UAVCAN protocol.</source>
      <translation>启用此选项将启动 UAVCAN 协议。</translation>
    </message>
    <message>
      <source>Enforce Open Drone ID system presence</source>
      <translation>强制开放无人机 ID 系统存在</translation>
    </message>
    <message>
      <source>Engine Fail Mask</source>
      <translation>发动机故障面罩</translation>
    </message>
    <message>
      <source>Enter surface mode</source>
      <translation>进入上浮模式</translation>
    </message>
    <message>
      <source>Estimator replay (EKF2)</source>
      <translation>估计器重播 (EKF2)</translation>
    </message>
    <message>
      <source>Explicitly set error code</source>
      <translation>明确设置错误代码</translation>
    </message>
    <message>
      <source>Extends the spacing between the sidebar elements by this amount of columns. Positive values increases the width to the right of the screen.</source>
      <translation>将侧边栏元素之间的间距延长此列数。正值会增加屏幕右侧的宽度。</translation>
    </message>
    <message>
      <source>External Gov SetPoint</source>
      <translation>外部政府设定点</translation>
    </message>
    <message>
      <source>External Mode 4</source>
      <translation>外部模式 4</translation>
    </message>
    <message>
      <source>External Vision</source>
      <translation>外部视觉</translation>
    </message>
    <message>
      <source>External mode identifier 4</source>
      <translation>外部模式标识符 4</translation>
    </message>
    <message>
      <source>External vision (EV) sensor aiding</source>
      <translation>外部视觉 (EV) 传感器辅助</translation>
    </message>
    <message>
      <source>Extra 4th compass's device id.  Automatically detected, do not set manually</source>
      <translation>额外的第四个罗盘的设备 ID。  自动检测，无需手动设置</translation>
    </message>
    <message>
      <source>Extra data type 2 stream rate</source>
      <translation>额外数据类型 2 流速率</translation>
    </message>
    <message>
      <source>FBW style</source>
      <translation>FBW款式</translation>
    </message>
    <message>
      <source>FBWMixing</source>
      <translation>FBW混合</translation>
    </message>
    <message>
      <source>FF D Gain which produces an output that is proportional to the rate of change of the target. If upgrading from 4.6 this is _ACCZ_P * 0.1.</source>
      <translation>FF D 增益，产生与目标变化率成比例的输出。如果从 4.6 升级，则为 _ACCZ_P * 0.1。</translation>
    </message>
    <message>
      <source>FFT attenuation level in dB for bandwidth calculation and peak detection. The bandwidth is calculated by comparing peak power output with the attenuated version. The default of 15 has shown to be a good compromise in both simulations and real flight.</source>
      <translation>FFT 用于带宽计算和峰值检测的衰减级别（以 dB 为单位）。通过将峰值功率输出与衰减版本进行比较来计算带宽。 15 的默认设置在模拟和实际飞行中都表现出良好的折衷。</translation>
    </message>
    <message>
      <source>FFT learned thrust reference</source>
      <translation>FFT学习推力参考</translation>
    </message>
    <message>
      <source>FLIP Mode</source>
      <translation>FLIP模式</translation>
    </message>
    <message>
      <source>FOLLOW Mode</source>
      <translation>FOLLOW模式</translation>
    </message>
    <message>
      <source>FW Altitude Acceptance Radius</source>
      <translation>FW 高度接受半径</translation>
    </message>
    <message>
      <source>Face direction of travel</source>
      <translation>面对行驶方向</translation>
    </message>
    <message>
      <source>Failsafe battery voltage</source>
      <translation>故障保护电池电压</translation>
    </message>
    <message>
      <source>Failure detection per motor only triggers above this thrust value.
Set to 1 to disable the detection.</source>
      <translation>每个电机的故障检测仅在高于该推力值时触发。设置为 1 以禁用检测。</translation>
    </message>
    <message>
      <source>Fallback to thrust based airspeed estimation</source>
      <translation>回退到基于推力的空速估计</translation>
    </message>
    <message>
      <source>Fast rate thread divisor used to control the maximum fast rate update rate. The actual rate is the gyro rate in Hz divided by this value. This value is scaled depending on the configuration of FSTRATE_ENABLE.</source>
      <translation>快速线程除数用于控制最大快速更新速率。实际速率是陀螺仪速率（以 Hz 为单位）除以该值。该值根据 FSTRATE_ENABLE 的配置进行调整。</translation>
    </message>
    <message>
      <source>Feedback from mid stick</source>
      <translation>中杆反馈</translation>
    </message>
    <message>
      <source>Fence Horizontal Margin</source>
      <translation>栅栏水平边距</translation>
    </message>
    <message>
      <source>Fence Total</source>
      <translation>围栏总数</translation>
    </message>
    <message>
      <source>FenceReturnPoint</source>
      <translation>栅栏返回点</translation>
    </message>
    <message>
      <source>Fifth Gyro bias on Z axis</source>
      <translation>Z 轴上的第五陀螺仪偏置</translation>
    </message>
    <message>
      <source>Filter cutoff frequency for accelerometers. This can be set to a lower value to try to cope with very high vibration levels in aircraft. A value of zero means no filtering (not recommended!)</source>
      <translation>加速度计的滤波器截止频率。可以将其设置为较低的值，以尝试应对飞机中非常高的振动水平。值为零表示不过滤（不推荐！）</translation>
    </message>
    <message>
      <source>Final approach WP distance</source>
      <translation>最终进近WP距离</translation>
    </message>
    <message>
      <source>First Gyro bias (Y-axis)</source>
      <translation>第一陀螺仪偏置（Y 轴）</translation>
    </message>
    <message>
      <source>First Relay Pin</source>
      <translation>第一个继电器引脚</translation>
    </message>
    <message>
      <source>First driver</source>
      <translation>第一司机</translation>
    </message>
    <message>
      <source>First principle check (airspeed change vs. throttle and pitch)</source>
      <translation>第一原理检查（空速变化与油门和桨距）</translation>
    </message>
    <message>
      <source>FirstIMU</source>
      <translation>第一IMU</translation>
    </message>
    <message>
      <source>Fixed Wing Autotune specific options. Useful on QuadPlanes with higher INS_GYRO_FILTER settings to prevent these filter values from being set too agressively during Fixed Wing Autotune.</source>
      <translation>固定翼自动调谐特定选项。在具有较高 INS_GYRO_FILTER 设置的四平面上很有用，可防止在固定翼自动调谐期间将这些滤波器值设置得过于激进。</translation>
    </message>
    <message>
      <source>Fixed-wing actuation thrust scale in hover</source>
      <translation>悬停时固定翼驱动推力刻度</translation>
    </message>
    <message>
      <source>FixedWing</source>
      <translation>固定翼</translation>
    </message>
    <message>
      <source>FixedWingRollI</source>
      <translation>固定翼滚转I型</translation>
    </message>
    <message>
      <source>Flap input channel</source>
      <translation>襟翼输入通道</translation>
    </message>
    <message>
      <source>FlaperonRight</source>
      <translation>襟副翼右</translation>
    </message>
    <message>
      <source>Flight Mode 1</source>
      <translation>飞行模式1</translation>
    </message>
    <message>
      <source>Flight mode for switch position 1 (910 to 1230 and above 2049)</source>
      <translation>开关位置 1 的飞行模式（910 至 1230 及以上 2049）</translation>
    </message>
    <message>
      <source>Flight mode when Channel 5 pwm is &lt;= 1230</source>
      <translation>通道 5 pwm &lt;= 1230 时的飞行模式</translation>
    </message>
    <message>
      <source>Flight mode when pwm of Flightmode channel(FLTMODE_CH) is &gt;1360, &lt;= 1490</source>
      <translation>Flightmode通道(FLTMODE_CH)的pwm &gt;1360, &lt;= 1490时的飞行模式</translation>
    </message>
    <message>
      <source>FlightMode2</source>
      <translation>飞行模式2</translation>
    </message>
    <message>
      <source>FloatRTK</source>
      <translation>浮点RTK</translation>
    </message>
    <message>
      <source>FlowHold Braking rate</source>
      <translation>FlowHold 制动率</translation>
    </message>
    <message>
      <source>Fly By Wire B altitude change rate</source>
      <translation>电传飞行 B 高度变化率</translation>
    </message>
    <message>
      <source>Follow Angle setting in degrees</source>
      <translation>跟随角度设置（以度为单位）</translation>
    </message>
    <message>
      <source>Follow distance maximum.  targets further than this will be ignored</source>
      <translation>跟随距离最大。  超出此范围的目标将被忽略</translation>
    </message>
    <message>
      <source>Follow offsets in meters east/right.  If positive, this vehicle will fly to the right or east of lead vehicle.  Depends on FOLL_OFS_TYPE</source>
      <translation>遵循东/右偏移（以米为单位）。  如果为正，该车辆将飞至领头车辆的右侧或东部。  取决于 FOLL_OFS_TYPE</translation>
    </message>
    <message>
      <source>Follow position error P gain</source>
      <translation>跟随位置误差P增益</translation>
    </message>
    <message>
      <source>Follow yaw behaviour</source>
      <translation>跟随偏航行为</translation>
    </message>
    <message>
      <source>For reliable wind estimation both sideslip and airspeed fusion (see EKF2_ARSP_THR) should be enabled. Only applies to vehicles in fixed-wing mode or with airspeed fusion active. Note: side slip fusion is currently not supported for tailsitters.</source>
      <translation>为了进行可靠的风估计，应启用侧滑和空速融合（参见 EKF2_ARSP_THR）。仅适用于固定翼模式或空速融合处于活动状态的车辆。注意：tailsitter 目前不支持侧滑融合。</translation>
    </message>
    <message>
      <source>For use on tilt wings, the wing will tilt up to this angle for flap, transistion will be complete when the wing reaches this angle from the forward fight position, 0 disables</source>
      <translation>对于倾斜机翼使用，机翼将倾斜至此角度进行襟翼，当机翼从前战斗位置到达此角度时，过渡将完成，0 禁用</translation>
    </message>
    <message>
      <source>Force RTL mode on VTOL failsafes overriding bit 5(USE QRTL)</source>
      <translation>在 VTOL 故障保护覆盖位 5(USE QRTL) 上强制使用 RTL 模式</translation>
    </message>
    <message>
      <source>Force on</source>
      <translation>强制开启</translation>
    </message>
    <message>
      <source>Forces the motor output signal to take at least the configured time (in seconds)
to traverse its full range (normally [0, 1], or if reversible [-1, 1]).
Zero means that slew rate limiting is disabled.</source>
      <translation>强制电机输出信号至少花费配置的时间（以秒为单位）来遍历其整个范围（通常为 [0、1]，或者如果可逆则为 [-1、1]）。零表示转换速率限制被禁用。</translation>
    </message>
    <message>
      <source>Forward Speed Controller D Gain</source>
      <translation>前进速度控制器 D 增益</translation>
    </message>
    <message>
      <source>Forward axis with origin relative to vehicle centre of gravity</source>
      <translation>原点相对于车辆重心的前轴</translation>
    </message>
    <message>
      <source>Forward throttle battery voltage compensation maximum voltage (voltage above this will have no additional scaling effect on thrust). Recommend 4.2 * cell count, 0 = Disabled. Recommend THR_MAX is set to no more than 100 x FWD_BAT_VOLT_MIN / FWD_BAT_VOLT_MAX, THR_MIN is set to no less than -100 x FWD_BAT_VOLT_MIN / FWD_BAT_VOLT_MAX and climb descent rate limits are set accordingly.</source>
      <translation>前进油门电池电压补偿最大电压（高于此电压不会对推力产生额外的缩放效果）。推荐 4.2 * 细胞计数，0 = 禁用。建议THR_MAX设置不大于100 x FWD_BAT_VOLT_MIN / FWD_BAT_VOLT_MAX，THR_MIN设置不小于-100 x FWD_BAT_VOLT_MIN / FWD_BAT_VOLT_MAX，并相应设置爬升下降率限制。</translation>
    </message>
    <message>
      <source>Forward-Left</source>
      <translation>左前</translation>
    </message>
    <message>
      <source>Fourth Gyro bias on X axis</source>
      <translation>X 轴上的第四个陀螺仪偏置</translation>
    </message>
    <message>
      <source>FourthEKF</source>
      <translation>第四次EKF</translation>
    </message>
    <message>
      <source>Frame configuration</source>
      <translation>框架配置</translation>
    </message>
    <message>
      <source>Frequency of vibration applied to IMU readings in SITL (Z-axis)</source>
      <translation>SITL（Z 轴）中应用于 IMU 读数的振动频率</translation>
    </message>
    <message>
      <source>Front transition timeout</source>
      <translation>前过渡超时</translation>
    </message>
    <message>
      <source>FuelLevelPWM</source>
      <translation>燃油位脉宽调制</translation>
    </message>
    <message>
      <source>FullInput</source>
      <translation>全输入</translation>
    </message>
    <message>
      <source>FuseAllVelocities</source>
      <translation>融合所有速度</translation>
    </message>
    <message>
      <source>GCS connection loss time threshold</source>
      <translation>GCS 连接丢失时间阈值</translation>
    </message>
    <message>
      <source>GLONASS+SBAS</source>
      <translation>GLONASS+SBAS</translation>
    </message>
    <message>
      <source>GNSS receiver 1 only</source>
      <translation>仅 GNSS 接收器 1</translation>
    </message>
    <message>
      <source>GPIO emulation</source>
      <translation>GPIO仿真</translation>
    </message>
    <message>
      <source>GPS 1 Altitude Drift</source>
      <translation>GPS 1 海拔漂移</translation>
    </message>
    <message>
      <source>GPS 1 Noise</source>
      <translation>GPS 1 噪声</translation>
    </message>
    <message>
      <source>GPS 1 altitude drift error</source>
      <translation>GPS 1 高度漂移误差</translation>
    </message>
    <message>
      <source>GPS 2</source>
      <translation>GPS 2</translation>
    </message>
    <message>
      <source>GPS 2 Hz</source>
      <translation>GPS 2 Hz</translation>
    </message>
    <message>
      <source>GPS 2 Velocity Error Offsets in NED (X-axis)</source>
      <translation>GPS 2 NED 中的速度误差偏移（X 轴）</translation>
    </message>
    <message>
      <source>GPS 2 disable</source>
      <translation>GPS 2 禁用</translation>
    </message>
    <message>
      <source>GPS Accuracy</source>
      <translation>GPS 精度</translation>
    </message>
    <message>
      <source>GPS Disabled</source>
      <translation>GPS 禁用</translation>
    </message>
    <message>
      <source>GPS Lock</source>
      <translation>GPS 锁</translation>
    </message>
    <message>
      <source>GPS Node id for discovered first.</source>
      <translation>GPS 首先发现的节点 ID。</translation>
    </message>
    <message>
      <source>GPS Node id for second-discovered GPS.</source>
      <translation>GPS 第二个发现的 GPS 的节点 ID。</translation>
    </message>
    <message>
      <source>GPS Update rate</source>
      <translation>GPS 更新率</translation>
    </message>
    <message>
      <source>GPS antenna lateral offset. This describes the physical location offest from center of the GPS antenna on the aircraft.</source>
      <translation>GPS 天线横向偏移。这描述了飞机上 GPS 天线中心的物理位置。</translation>
    </message>
    <message>
      <source>GPS configuration</source>
      <translation>GPS配置</translation>
    </message>
    <message>
      <source>GPS glitch radius gate size (m)</source>
      <translation>GPS毛刺半径闸门尺寸（m）</translation>
    </message>
    <message>
      <source>GPS initial lat offset from origin</source>
      <translation>GPS 距原点的初始纬度偏移</translation>
    </message>
    <message>
      <source>GPS measurement delay relative to IMU measurements</source>
      <translation>相对于 IMU 测量的 GPS 测量延迟</translation>
    </message>
    <message>
      <source>GPS type</source>
      <translation>GPS型</translation>
    </message>
    <message>
      <source>GPS velocity measurement delay (msec)</source>
      <translation>GPS 速度测量延迟（毫秒）</translation>
    </message>
    <message>
      <source>GPS z velocity standard deviation</source>
      <translation>GPS z 速度标准差</translation>
    </message>
    <message>
      <source>GPSLONG_EN</source>
      <translation>GPSLONG_EN</translation>
    </message>
    <message>
      <source>GSF</source>
      <translation>GSF</translation>
    </message>
    <message>
      <source>Gain added to pitch to keep aircraft from descending or ascending in turns. Increase in increments of 0.05 to reduce altitude loss. Decrease for altitude gain.</source>
      <translation>添加到俯仰角的增益可防止飞机轮流下降或上升。增加 0.05 的增量以减少高度损失。减少高度增益。</translation>
    </message>
    <message>
      <source>Gain from yaw rate to rudder. Most effective at yaw damping and should be tuned after KFF_RDDRMIX. Also disables YAW2SRV_INT if set to 0.</source>
      <translation>从偏航率到方向舵的增益。对偏航阻尼最有效，应在 KFF_RDDRMIX 之后进行调整。如果设置为 0，也会禁用 YAW2SRV_INT。</translation>
    </message>
    <message>
      <source>Gain to the yaw rate required to keep it consistent with the turn rate in a coordinated turn. Corrects for yaw tendencies after the turn is established. Increase yaw into the turn by raising. Increase yaw out of the turn by decreasing. Values outside of 0.9-1.1 range indicate airspeed calibration problems.</source>
      <translation>获得与协调转弯中的转弯速率保持一致所需的偏航速率。转弯建立后纠正偏航倾向。通过抬高来增加转弯时的偏航。通过减少来增加转弯时的偏航。超出 0.9-1.1 范围的值表示空速校准问题。</translation>
    </message>
    <message>
      <source>Gate size for acceleration fusion</source>
      <translation>加速融合的门尺寸</translation>
    </message>
    <message>
      <source>Gate size for magnetometer XYZ component fusion</source>
      <translation>磁力计 XYZ 组件融合的门尺寸</translation>
    </message>
    <message>
      <source>Gate size used for range finder kinematic consistency check</source>
      <translation>用于测距仪运动一致性检查的门尺寸</translation>
    </message>
    <message>
      <source>Generator type</source>
      <translation>发电机类型</translation>
    </message>
    <message>
      <source>Get absolute timestamp</source>
      <translation>获取绝对时间戳</translation>
    </message>
    <message>
      <source>Gimbal Roll</source>
      <translation>万向节横滚</translation>
    </message>
    <message>
      <source>Glitch for 1st baro</source>
      <translation>第一气压故障</translation>
    </message>
    <message>
      <source>Glitch offsets of simulated GPS sensor (X-axis)</source>
      <translation>模拟 GPS 传感器的毛刺偏移（X 轴）</translation>
    </message>
    <message>
      <source>Global gain of the controller.
This gain scales the P, I and D terms of the controller:
output = MC_YAWRATE_K * (MC_YAWRATE_P * error
+ MC_YAWRATE_I * error_integral
+ MC_YAWRATE_D * error_derivative)
Set MC_YAWRATE_P=1 to implement a PID in the ideal form.
Set MC_YAWRATE_K=1 to implement a PID in the parallel form.</source>
      <translation>控制器的全局增益。该增益可缩放控制器的 P、I 和 D 项：输出 = MC_YAWRATE_K * (MC_YAWRATE_P * 误差 + MC_YAWRATE_I * error_integral + MC_YAWRATE_D * error_derivative) 设置 MC_YAWRATE_P=1 以实现理想形式的 PID。设置MC_YAWRATE_K=1，以并行形式实现PID。</translation>
    </message>
    <message>
      <source>Global gain of the controller. This gain scales the P, I and D terms of the controller: output = SC_ROLLRATE_K * (SC_ROLLRATE_P * error + SC_ROLLRATE_I * error_integral + SC_ROLLRATE_D * error_derivative) Set SC_ROLLRATE_P=1 to implement a PID in the ideal form. Set SC_ROLLRATE_K=1 to implement a PID in the parallel form.</source>
      <translation>控制器的全局增益。该增益可缩放控制器的 P、I 和 D 项：输出 = SC_ROLLRATE_K * (SC_ROLLRATE_P * 误差 + SC_ROLLRATE_I * error_integral + SC_ROLLRATE_D * error_derivative) 设置 SC_ROLLRATE_P=1 以实现理想形式的 PID。设置SC_ROLLRATE_K=1，以并行形式实现PID。</translation>
    </message>
    <message>
      <source>GoPro in Solo Gimbal</source>
      <translation>独奏万向节中的 GoPro</translation>
    </message>
    <message>
      <source>Governor Operational Range</source>
      <translation>调速器工作范围</translation>
    </message>
    <message>
      <source>Gripper Autoclose time</source>
      <translation>夹具自动关闭时间</translation>
    </message>
    <message>
      <source>Gripper emp pin</source>
      <translation>夹具 Emp 销</translation>
    </message>
    <message>
      <source>Ground Effect Takeoff Timeout</source>
      <translation>地面效应起飞超时</translation>
    </message>
    <message>
      <source>Ground effect compensation timeout after takeoff. Compensation is turned off this many seconds after takeoff AND the vehicle has climbed at least GNDEFF_ALT. Compensation is also disabled after 5sec regardless of this timeout or the vehicle's altitude. Zero disables this timeout and only the altitude check is applied. Vehicles with strong baro disturbance from propwash should use values of 2 to 5 sec. This does not affect the compensation during touchdown.</source>
      <translation>起飞后地效补偿超时。起飞 AND 后多少秒关闭补偿，车辆至少爬升了 GNDEFF_ALT。无论超时时间或车辆高度如何，5 秒后补偿也会被禁用。零会禁用此超时，并且仅应用高度检查。受到螺旋桨洗流强烈气压干扰的车辆应使用 2 至 5 秒的值。这不会影响接地期间的补偿。</translation>
    </message>
    <message>
      <source>Group of the parameter to be displayed and modified</source>
      <translation>显示和修改参数组</translation>
    </message>
    <message>
      <source>GuidedModeThrPass</source>
      <translation>引导模式ThrPass</translation>
    </message>
    <message>
      <source>Gyro 3 scaling factor</source>
      <translation>陀螺仪 3 比例因子</translation>
    </message>
    <message>
      <source>Gyro ID</source>
      <translation>陀螺仪ID</translation>
    </message>
    <message>
      <source>Gyro data to/from files</source>
      <translation>陀螺仪数据传入/传出文件</translation>
    </message>
    <message>
      <source>Gyro offsets of Y axis</source>
      <translation>Y轴陀螺仪偏移</translation>
    </message>
    <message>
      <source>Gyro rate offset temperature ^1 polynomial coefficient - Y axis</source>
      <translation>陀螺仪速率偏移温度^1多项式系数-Y轴</translation>
    </message>
    <message>
      <source>Gyro sensor ID, taking into account its type, bus and instance</source>
      <translation>陀螺仪传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>Gyro2 sensor ID, taking into account its type, bus and instance</source>
      <translation>Gyro2 传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>Gyro3 sensor ID, taking into account its type, bus and instance</source>
      <translation>Gyro3 传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>Gyroscope 0 priority</source>
      <translation>陀螺仪0优先级</translation>
    </message>
    <message>
      <source>Gyroscope 1st order temperature coefficient X axis</source>
      <translation>陀螺仪一阶温度系数X轴</translation>
    </message>
    <message>
      <source>Gyroscope 2 rotation relative to airframe</source>
      <translation>陀螺仪 2 相对机身旋转</translation>
    </message>
    <message>
      <source>Gyroscope 3 priority</source>
      <translation>陀螺仪3优先级</translation>
    </message>
    <message>
      <source>H265</source>
      <translation>H265</translation>
    </message>
    <message>
      <source>HC-SR04</source>
      <translation>HC-SR04</translation>
    </message>
    <message>
      <source>HELICOPTER</source>
      <translation>HELICOPTER</translation>
    </message>
    <message>
      <source>HFI EFI relay index</source>
      <translation>HFI EFI 继电器索引</translation>
    </message>
    <message>
      <source>HOMEDIR_EN</source>
      <translation>HOMEDIR_EN</translation>
    </message>
    <message>
      <source>HOME_EN</source>
      <translation>HOME_EN</translation>
    </message>
    <message>
      <source>Half-angle of the return mode altitude cone</source>
      <translation>返回模式高度锥的半角</translation>
    </message>
    <message>
      <source>Handles logging raw data; on uBlox chips that support raw data this will log RXM messages into logger; on Septentrio this will log on the equipment's SD card and when set to 2, the autopilot will try to stop logging after disarming and restart after arming</source>
      <translation>处理记录原始数据；在支持原始数据的 uBlox 芯片上，这会将 RXM 消息记录到记录器中；在Septentrio上，这将登录设备的SD卡，当设置为2时，飞控将在上锁后尝试停止记录并在解锁后重新启动</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter bandwidth</source>
      <translation>谐波陷波滤波器带宽</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter base frequency</source>
      <translation>谐波陷波滤波器基频</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter options</source>
      <translation>谐波陷波滤波器选项</translation>
    </message>
    <message>
      <source>Heading offset angle for dual antenna GPS setups that support heading estimation.
Set this to 0 if the antennas are parallel to the forward-facing direction
of the vehicle and the rover (or Unicore primary) antenna is in front.
The offset angle increases clockwise.
Set this to 90 if the rover (or Unicore primary, or Septentrio Mosaic Aux)
antenna is placed on the right side of the vehicle and the moving base
antenna is on the left side.
(Note: the Unicore primary antenna is the one connected on the right as seen
from the top).</source>
      <translation>支持航向估计的双天线 GPS 设置的航向偏移角。如果天线与车辆的前向平行并且流动站（或 Unicore 主）天线位于前面，则将此设置为 0。偏移角顺时针增大。如果流动站（或 Unicore 主天线或 Septentrio Mosaic Aux）天线放置在车辆的右侧并且移动基座天线位于左侧​​，则将此设置为 90。 （注：Unicore 主天线是从顶部看连接在右侧的天线）。</translation>
    </message>
    <message>
      <source>Heartbeat Pin</source>
      <translation>心跳别针</translation>
    </message>
    <message>
      <source>Height above ground threshold below which tighter altitude
tracking gets enabled (see FW_LND_THRTC_SC). Below this height, TECS smoothly
(1 sec / sec) transitions the altitude tracking time constant from FW_T_ALT_TC
to FW_LND_THRTC_SC*FW_T_ALT_TC.
-1 to disable.</source>
      <translation>高于地面的高度阈值，低于该阈值将启用更严格的高度跟踪（请参阅 FW_LND_THRTC_SC）。低于该高度，TECS 平滑地（1 秒/秒）将高度跟踪时间常数从 FW_T_ALT_TC 过渡到 FW_LND_THRTC_SC*FW_T_ALT_TC。 -1 禁用。</translation>
    </message>
    <message>
      <source>Height measurement delay (msec)</source>
      <translation>高度测量延迟（毫秒）</translation>
    </message>
    <message>
      <source>Height rc-button down</source>
      <translation>rc-按钮向下高度</translation>
    </message>
    <message>
      <source>Heli_Dual</source>
      <translation>直升机_双</translation>
    </message>
    <message>
      <source>Helicopter's minimum collective pitch setting at zero collective stick input in Stabilize mode.  Set this as a percent of collective range given by H_COL_MAX minus H_COL_MIN.</source>
      <translation>在稳定模式下，直升机在零总距操纵杆输入时的最小总距桨距设置。  将其设置为 H_COL_MAX 减去 H_COL_MIN 给出的总距范围的百分比。</translation>
    </message>
    <message>
      <source>High</source>
      <translation>高</translation>
    </message>
    <message>
      <source>High rate sensors</source>
      <translation>高速率传感器</translation>
    </message>
    <message>
      <source>Highest possible servo position in PWM microseconds for swashplate 2</source>
      <translation>斜盘 2 的最高可能舵机位置（以 PWM 微秒为单位）</translation>
    </message>
    <message>
      <source>Hobbywing ESC motor poles</source>
      <translation>好盈ESC电机极数</translation>
    </message>
    <message>
      <source>Home Location</source>
      <translation>家庭位置</translation>
    </message>
    <message>
      <source>Horizontal position drift (EKF2_REQ_HDRIFT)</source>
      <translation>水平位置漂移（EKF2_REQ_HDRIFT）</translation>
    </message>
    <message>
      <source>Horizontal thrust margin</source>
      <translation>水平推力裕度</translation>
    </message>
    <message>
      <source>Hover Value Learning</source>
      <translation>悬停价值学习</translation>
    </message>
    <message>
      <source>I gain</source>
      <translation>我收获</translation>
    </message>
    <message>
      <source>I2C address for BatMon battery 1</source>
      <translation>BatMon 电池 1 的 I2C 地址</translation>
    </message>
    <message>
      <source>I2C-MS5525 (0x76)</source>
      <translation>I2C-MS5525 (0x76)</translation>
    </message>
    <message>
      <source>ICAO_ID of special vehicle</source>
      <translation>特种车ICAO_ID</translation>
    </message>
    <message>
      <source>ICM20948</source>
      <translation>ICM20948</translation>
    </message>
    <message>
      <source>IE 650w 800w Fuel Cell</source>
      <translation>IE 650w 800w 燃料电池</translation>
    </message>
    <message>
      <source>IMU Heater I gain</source>
      <translation>IMU 加热器 I 增益</translation>
    </message>
    <message>
      <source>IMU accelerometer Y position</source>
      <translation>IMU加速度计Y位置</translation>
    </message>
    <message>
      <source>IMU gyro ESC notch filter bandwidth</source>
      <translation>IMU 陀螺仪 ESC 陷波滤波器带宽</translation>
    </message>
    <message>
      <source>IMU gyro dynamic notch filter minimum frequency</source>
      <translation>IMU陀螺仪动态陷波滤波器最小频率</translation>
    </message>
    <message>
      <source>IMU temperature end</source>
      <translation>IMU 温度结束</translation>
    </message>
    <message>
      <source>IMU_FAST</source>
      <translation>IMU_FAST</translation>
    </message>
    <message>
      <source>INA226 Power Monitor Config</source>
      <translation>INA226 功率监视器配置</translation>
    </message>
    <message>
      <source>INA239_SPI</source>
      <translation>INA239_SPI</translation>
    </message>
    <message>
      <source>INS (default)</source>
      <translation>INS（默认）</translation>
    </message>
    <message>
      <source>IP Subnet mask</source>
      <translation>IP 子网掩码</translation>
    </message>
    <message>
      <source>IPv4 address. Example: xxx.xxx.13.xxx</source>
      <translation>IPv4 地址。示例：xxx.xxx.13.xxx</translation>
    </message>
    <message>
      <source>IRC Tramp</source>
      <translation>IRC 流浪者</translation>
    </message>
    <message>
      <source>Ice shedding cycle period</source>
      <translation>脱冰周期</translation>
    </message>
    <message>
      <source>Idle throttle used for all RSC modes.  For external governors, this would be set to signal it to enable fast spool-up, when bailing out of an autorotation.  Set 0 to disable. If also using a tail rotor of type DDVP with external governor then this value must lie within the autorotation window of both governors.</source>
      <translation>用于所有 RSC 模式的怠速油门。  对于外部调速器，这将被设置为在脱离自动旋转时发出信号以启用快速卷起。  将 0 设置为禁用。如果还使用带外部调速器的 DDVP 型尾桨，则该值必须位于两个调速器的自转窗口内。</translation>
    </message>
    <message>
      <source>If FOLL_ENABLE = 1, check that FOLL_SYSID is different to MAV_SYSID. 3 or less to prevent arming. -1 to disable.</source>
      <translation>如果 FOLL_ENABLE = 1，请检查 FOLL_SYSID 是否与 MAV_SYSID 不同。 3以下防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>If RK9_FORCEHL=2, this is the number of seconds of GCS timeout until High Latency mode is auto-enabled</source>
      <translation>如果 RK9_FORCEHL=2，则这是自动启用高延迟模式之前 GCS 超时的秒数</translation>
    </message>
    <message>
      <source>If enabled, MAVLink messages will be throttled according to
`txbuf` field reported by radio_status.
Requires a radio to send the mavlink message RADIO_STATUS.</source>
      <translation>如果启用，MAVLink 消息将根据 radio_status 报告的“txbuf”字段进行限制。需要无线电发送 mavlink 消息 RADIO_STATUS。</translation>
    </message>
    <message>
      <source>If enabled, forward incoming MAVLink messages to other MAVLink ports if the message is either broadcast or the target is not the autopilot. This allows for example a GCS to talk to a camera that is connected to the autopilot via MAVLink (on a different link than the GCS).</source>
      <translation>如果启用，如果消息是广播或目标不是自动驾驶仪，则将传入的 MAVLink 消息转发到其他 MAVLink 端口。例如，这允许 GCS 与通过 MAVLink（与 GCS 不同的链路）连接到自动驾驶仪的摄像机进行通信。</translation>
    </message>
    <message>
      <source>If ethernet enabled and selected as configuration for MAVLink instance 0,
selected remote port will be set and used in MAVLink instance 0.</source>
      <translation>如果启用以太网并选择作为 MAVLink 实例 0 的配置，则选定的远程端口将在 MAVLink 实例 0 中设置和使用。</translation>
    </message>
    <message>
      <source>If ethernet enabled and selected as configuration for MAVLink instance 2,
selected remote port will be set and used in MAVLink instance 2.</source>
      <translation>如果启用以太网并选择作为 MAVLink 实例 2 的配置，则选定的远程端口将在 MAVLink 实例 2 中设置和使用。</translation>
    </message>
    <message>
      <source>If external guided command was not received by this timeout, the vehicle will revert to regular GUIDED mode.</source>
      <translation>如果在此超时时间内未收到外部引导命令，车辆将恢复到常规 GUIDED 模式。</translation>
    </message>
    <message>
      <source>If non-zero this airspeed will be used when cruising between thermals in AUTO. If set to -1, airspeed will be selected based on speed-to-fly theory. If set to 0, then TRIM_ARSPD_CM will be used while cruising between thermals.</source>
      <translation>如果非零，则在 AUTO 中的热气流之间巡航时将使用该空速。如果设置为 -1，将根据飞行速度理论选择空速。如果设置为 0，则在热气流之间巡航时将使用 TRIM_ARSPD_CM。</translation>
    </message>
    <message>
      <source>If set &gt; 0 it will ignore the target heading for normal waypoint acceptance. If the waypoint forces the heading the timeout will matter. For example on VTOL forwards transition. Mainly useful for VTOLs that have less yaw authority and might not reach target yaw in wind. Disabled by default.</source>
      <translation>如果设置&gt; 0，它将忽略正常航路点接受的目标航向。如果航路点强制航向，超时就会很重要。例如，在 VTOL 上向前过渡。主要适用于偏航权限较小且可能无法在风中达到目标偏航的 VTOL。默认禁用。</translation>
    </message>
    <message>
      <source>If set to 1, the airspeed measurement data, if valid, is used in the following controllers:
- Rate controller: output scaling
- Attitude controller: coordinated turn controller
- Position controller: airspeed setpoint tracking, takeoff logic
- VTOL: transition logic</source>
      <translation>如果设置为 1，则空速测量数据（如果有效）将用于以下控制器： - 速率控制器：输出缩放 - 姿态控制器：协调转弯控制器 - 位置控制器：空速设定点跟踪、起飞逻辑 - VTOL：转换逻辑</translation>
    </message>
    <message>
      <source>If set to a value greater than zero, other parameters are automatically set (such as
MPC_XY_VEL_MAX or MPC_VEL_MANUAL).
If set to a negative value, the existing individual parameters are used.</source>
      <translation>如果设置为大于零的值，则自动设置其他参数（例如 MPC_XY_VEL_MAX 或 MPC_VEL_MANUAL）。如果设置为负值，则使用现有的各个参数。</translation>
    </message>
    <message>
      <source>If set to two side calibration, only the offsets are estimated, the scale calibration is left unchanged. Thus an initial six side calibration is recommended. Bits: ORIENTATION_TAIL_DOWN = 1 ORIENTATION_NOSE_DOWN = 2 ORIENTATION_LEFT = 4 ORIENTATION_RIGHT = 8 ORIENTATION_UPSIDE_DOWN = 16 ORIENTATION_RIGHTSIDE_UP = 32</source>
      <translation>如果设置为两侧校准，则仅估计偏移，比例校准保持不变。因此，建议进行初始六面校准。位： ORIENTATION_TAIL_DOWN = 1 ORIENTATION_NOSE_DOWN = 2 ORIENTATION_LEFT = 4 ORIENTATION_RIGHT = 8 ORIENTATION_UPSIDE_DOWN = 16 ORIENTATION_RIGHTSIDE_UP = 32</translation>
    </message>
    <message>
      <source>If the connected radio stops reporting RADIO_STATUS for a certain time,
a warning is triggered and, if MAV_X_RADIO_CTL is enabled, the software-flow
control is reset.</source>
      <translation>如果所连接的无线电在一定时间内停止报告 RADIO_STATUS，则会触发警告；如果启用了 MAV_X_RADIO_CTL，则会重置软件流控制。</translation>
    </message>
    <message>
      <source>If the farwardward distance is less than this value then start Pitching up to gain altitude.</source>
      <translation>如果远距离小于该值，则开始向上俯仰以获得高度。</translation>
    </message>
    <message>
      <source>If the sensor has a different orientation with respect to the vehicle. This will enable a transform to correct itself. The transform is described by MS_SENSOR_ROLL, MS_SENSOR_PITCH, MS_SENSOR_YAW.</source>
      <translation>如果传感器相对于车辆具有不同的方向。这将使转换能够自我纠正。该变换由 MS_SENSOR_ROLL、MS_SENSOR_PITCH、MS_SENSOR_YAW 描述。</translation>
    </message>
    <message>
      <source>If this is enabled then the radio will continuously transmit as required for FCC testing. The transmit channel is set by the value of the parameter. The radio will not work for RC input while this is enabled</source>
      <translation>如果启用此功能，则无线电将根据 FCC 测试的要求连续传输。传输通道由参数值设置。启用此功能后，无线电将无法用于 RC 输入</translation>
    </message>
    <message>
      <source>If this parameter is enabled then the estimator will make use of the barometric height measurements to estimate its height in addition to other height sources (if activated).</source>
      <translation>如果启用此参数，则估算器将利用气压高度测量值以及其他高度源（如果已激活）来估算其高度。</translation>
    </message>
    <message>
      <source>If true the neural network control is automatically started on boot</source>
      <translation>如果为 true，神经网络控制将在启动时自动启动</translation>
    </message>
    <message>
      <source>Ignore MAVLink Overrides</source>
      <translation>忽略 MAVLink 覆盖</translation>
    </message>
    <message>
      <source>Ignore forward flight angle limits-in Qmodes and use Q_ANGLE_MAX exclusively</source>
      <translation>忽略 Q 模式中的前向飞行角度限制并仅使用 Q_ANGLE_MAX</translation>
    </message>
    <message>
      <source>Imbalanced propeller failsafe mode</source>
      <translation>不平衡螺旋桨故障安全模式</translation>
    </message>
    <message>
      <source>In auto modes: default sink rate output by controller to achieve altitude setpoints. In manual modes: maximum sink rate setpoint.</source>
      <translation>在自动模式下：控制器输出默认下沉速率以达到高度设定点。在手动模式下：最大下降率设定值。</translation>
    </message>
    <message>
      <source>Increase Target landing airspeed constraint From Trim Airspeed to ARSPD_FBW_MAX</source>
      <translation>增加目标着陆空速限制从微调空速到 ARSPD_FBW_MAX</translation>
    </message>
    <message>
      <source>Increase value to increase sensitivity of head speed controller during autonomous autorotation.</source>
      <translation>增加值可提高自主自动旋转期间头部速度控制器的灵敏度。</translation>
    </message>
    <message>
      <source>Increment step for actuator 4</source>
      <translation>执行器 4 的增量步长</translation>
    </message>
    <message>
      <source>Indicate takeoff waiting for neutral rudder with flight control surfaces</source>
      <translation>用飞行控制面指示起飞等待中立舵</translation>
    </message>
    <message>
      <source>Initial Altitude Offset</source>
      <translation>初始高度偏移</translation>
    </message>
    <message>
      <source>Initial geodetic longitude</source>
      <translation>初始大地经度</translation>
    </message>
    <message>
      <source>InitialHeading</source>
      <translation>初始标题</translation>
    </message>
    <message>
      <source>Input Yaw Angle</source>
      <translation>输入偏航角</translation>
    </message>
    <message>
      <source>Integer bitmask controlling handling of magnetic declination</source>
      <translation>控制磁偏角处理的整数位掩码</translation>
    </message>
    <message>
      <source>Integral gain for velocity error</source>
      <translation>速度误差积分增益</translation>
    </message>
    <message>
      <source>Integrator gain pitch</source>
      <translation>积分器增益俯仰</translation>
    </message>
    <message>
      <source>Intermeshing</source>
      <translation>相互啮合</translation>
    </message>
    <message>
      <source>Internal device counter is reset to 0 when overrun this value,
counter is able to store up to 6 digits
reset of counter takes some time - measurement with reset has worse accuracy.
0 means reset counter after every measurement.</source>
      <translation>内部设备计数器超过此值时重置为 0，计数器最多可保存 6 位数字。重置需要一定时间，伴随重置的测量精度会降低。设为 0 时，每次测量后都重置计数器。</translation>
    </message>
    <message>
      <source>Inverted</source>
      <translation>反转</translation>
    </message>
    <message>
      <source>Is the Video Transmitter enabled or not</source>
      <translation>视频传输器是否启用</translation>
    </message>
    <message>
      <source>Jerk limit</source>
      <translation>加加速度限制</translation>
    </message>
    <message>
      <source>Jungle</source>
      <translation>丛林</translation>
    </message>
    <message>
      <source>Keep parameters</source>
      <translation>保留参数</translation>
    </message>
    <message>
      <source>L1 control crosstrack integrator gain</source>
      <translation>L1 控制交叉跟踪积分器增益</translation>
    </message>
    <message>
      <source>L35W38</source>
      <translation>L35W38</translation>
    </message>
    <message>
      <source>L75W80</source>
      <translation>L75W80</translation>
    </message>
    <message>
      <source>LED layout config value</source>
      <translation>LED布局配置值</translation>
    </message>
    <message>
      <source>LP5562 External</source>
      <translation>LP5562 外部</translation>
    </message>
    <message>
      <source>LTE initial baudrate</source>
      <translation>LTE初始波特率</translation>
    </message>
    <message>
      <source>Land (4.0+ Only)</source>
      <translation>降落</translation>
    </message>
    <message>
      <source>Land even in Stabilize</source>
      <translation>即使在稳定状态下也能着陆</translation>
    </message>
    <message>
      <source>Land speed</source>
      <translation>陆地速度</translation>
    </message>
    <message>
      <source>Landing Gear Startup behaviour control</source>
      <translation>起落架启动行为控制</translation>
    </message>
    <message>
      <source>Landing auto-abort slope threshold</source>
      <translation>着陆自动中止坡度阈值</translation>
    </message>
    <message>
      <source>Landing flare time</source>
      <translation>着陆耀斑时间</translation>
    </message>
    <message>
      <source>Landing pre-flare altitude</source>
      <translation>着陆预耀斑高度</translation>
    </message>
    <message>
      <source>Landing touchdown nudging option</source>
      <translation>着陆触地推进选项</translation>
    </message>
    <message>
      <source>LatLeft6M</source>
      <translation>左纬6M</translation>
    </message>
    <message>
      <source>Leak Failsafe Enable</source>
      <translation>泄漏故障保护启用</translation>
    </message>
    <message>
      <source>LearnCalibration</source>
      <translation>学习校准</translation>
    </message>
    <message>
      <source>LeddarVu8-Serial</source>
      <translation>LeddarVu8-串行</translation>
    </message>
    <message>
      <source>Left Wheel</source>
      <translation>左轮</translation>
    </message>
    <message>
      <source>Len25_Wid34</source>
      <translation>Len25_Wid34</translation>
    </message>
    <message>
      <source>Len65_Wid67</source>
      <translation>Len65_Wid67</translation>
    </message>
    <message>
      <source>Level Transition-keep wings within LEVEL_ROLL_LIMIT and only use forward motor(s) for climb during transition</source>
      <translation>水平转换 - 将机翼保持在 LEVEL_ROLL_LIMIT 内，并且在转换过程中仅使用正向电机进行爬升</translation>
    </message>
    <message>
      <source>LevelTransition</source>
      <translation>电平转换</translation>
    </message>
    <message>
      <source>Lever arm offset (m) in the X direction for the external GNSS receiver. In the case of a dual antenna setup, this is antenna 1.</source>
      <translation>外部 GNSS 接收器 X 方向的杠杆臂偏移 (m)。如果是双天线设置，则为 1 天线。</translation>
    </message>
    <message>
      <source>Lidar z offset from center of vehicle +down</source>
      <translation>激光雷达 z 距车辆中心的偏移 +down</translation>
    </message>
    <message>
      <source>Light</source>
      <translation>光</translation>
    </message>
    <message>
      <source>LightWareSF40C</source>
      <translation>LightWareSF40C</translation>
    </message>
    <message>
      <source>Lightware SF1xx/SF20/LW20 Operation Mode</source>
      <translation>Lightware SF1xx/SF20/LW20 操作模式</translation>
    </message>
    <message>
      <source>Limit for pitch rate in manual and auto modes (except acro). Has effect for large rotations in autonomous mode, to avoid large control output and mixer saturation. This is not only limited by the vehicle's properties, but also by the maximum measurement rate of the gyro.</source>
      <translation>手动和自动模式下的俯仰速率限制（特技除外）。对自主模式下的大旋转有效，以避免大的控制输出和混合器饱和。这不仅受到车辆性能的限制，还受到陀螺仪最大测量速率的限制。</translation>
    </message>
    <message>
      <source>Limit the maximum jerk (acceleration change) of the vehicle. A lower value leads to smoother motions but limits agility. Setting this to the maximum value essentially disables the limit. Only used with MPC_POS_MODE Acceleration based.</source>
      <translation>限制车辆的最大加加速度（加速度变化）。较低的值会导致更平滑的运动，但会限制敏捷性。将此设置为最大值实际上会禁用该限制。仅与基于 MPC_POS_MODE 加速的设备一起使用。</translation>
    </message>
    <message>
      <source>Limits the rate of change of the yaw setpoint to avoid large control output and mixer saturation.</source>
      <translation>限制偏航设定点的变化率，以避免大的控制输出和混合器饱和。</translation>
    </message>
    <message>
      <source>Lite</source>
      <translation>精简版</translation>
    </message>
    <message>
      <source>Local origin longitude for nav w/o GPS</source>
      <translation>不带 GPS 的导航的本地原点经度</translation>
    </message>
    <message>
      <source>Log Error</source>
      <translation>日志错误</translation>
    </message>
    <message>
      <source>Log all AIVDM messages</source>
      <translation>记录所有 AIVDM 消息</translation>
    </message>
    <message>
      <source>Log primary gyro only</source>
      <translation>仅记录主陀螺仪</translation>
    </message>
    <message>
      <source>Logarithmic sine sweep</source>
      <translation>对数正弦扫描</translation>
    </message>
    <message>
      <source>Logging File and Block Backend buffer size max (in kilobytes)</source>
      <translation>日志文件和块后端缓冲区最大大小（以千字节为单位）</translation>
    </message>
    <message>
      <source>Logging topic profile (integer bitmask)</source>
      <translation>记录主题配置文件（整数位掩码）</translation>
    </message>
    <message>
      <source>Loiter Radius Bank Angle Limit</source>
      <translation>盘旋半径银行角度限制</translation>
    </message>
    <message>
      <source>Loiter braking jerk in cm/s/s/s. Higher values will remove braking faster if the pilot moves the sticks during a braking maneuver.</source>
      <translation>盘旋制动急度，单位为 cm/s/s/s。如果飞行员在制动操作期间移动操纵杆，则较高的值将更快地消除制动。</translation>
    </message>
    <message>
      <source>Loiter maximum jerk</source>
      <translation>盘旋最大加加速度</translation>
    </message>
    <message>
      <source>Loiter minimum acceleration in cm/s/s. Higher values stop the copter more quickly when the stick is centered, but cause a larger jerk when the copter stops.</source>
      <translation>盘旋最小加速度（以 cm/s/s 为单位）。当摇杆居中时，较高的值会更快地停止飞行器，但在飞行器停止时会导致更大的急动。</translation>
    </message>
    <message>
      <source>Loiter speed gain</source>
      <translation>盘旋速度增益</translation>
    </message>
    <message>
      <source>Longitudinal</source>
      <translation>纵向</translation>
    </message>
    <message>
      <source>Low Bandwidth</source>
      <translation>低带宽</translation>
    </message>
    <message>
      <source>Low pass filter cutoff frequency for yaw torque setpoint</source>
      <translation>偏航扭矩设定值的低通滤波器截止频率</translation>
    </message>
    <message>
      <source>Low-pass filter frequency applied to the derivative (Hz)</source>
      <translation>应用于导数的低通滤波器频率 (Hz)</translation>
    </message>
    <message>
      <source>Loweheiser throttle</source>
      <translation>洛海塞尔油门</translation>
    </message>
    <message>
      <source>Lua Scripts</source>
      <translation>Lua脚本</translation>
    </message>
    <message>
      <source>MAC Address 6th byte</source>
      <translation>MAC 地址第 6 个字节</translation>
    </message>
    <message>
      <source>MAG1 Failure</source>
      <translation>MAG1故障</translation>
    </message>
    <message>
      <source>MAG2 external compass orientation</source>
      <translation>MAG2 外置罗盘定向</translation>
    </message>
    <message>
      <source>MAG6 Device ID</source>
      <translation>MAG6 设备 ID</translation>
    </message>
    <message>
      <source>MAIN 1 Minimum Value</source>
      <translation>MAIN 1 最小值</translation>
    </message>
    <message>
      <source>MAIN 3</source>
      <translation>MAIN 3</translation>
    </message>
    <message>
      <source>MAIN 4 Failsafe Value</source>
      <translation>MAIN 4 故障安全值</translation>
    </message>
    <message>
      <source>MAIN 5 Minimum Value</source>
      <translation>MAIN 5 最小值</translation>
    </message>
    <message>
      <source>MAIN 7</source>
      <translation>MAIN 7</translation>
    </message>
    <message>
      <source>MAIN 8 Failsafe Value</source>
      <translation>MAIN 8 故障安全值</translation>
    </message>
    <message>
      <source>MAVLINK_DO_MOUNT (protocol v1, to be deprecated)</source>
      <translation>MAVLINK_DO_MOUNT（协议 v1，已弃用）</translation>
    </message>
    <message>
      <source>MAVLink High Latency</source>
      <translation>MAVLink 高延迟</translation>
    </message>
    <message>
      <source>MAVLink Options</source>
      <translation>MAVLink 选项</translation>
    </message>
    <message>
      <source>MAVLink Stream rate of AHRS, SYSTEM_TIME, WIND, RANGEFINDER, DISTANCE_SENSOR, TERRAIN_REQUEST, BATTERY_STATUS, GIMBAL_DEVICE_ATTITUDE_STATUS, OPTICAL_FLOW, MAG_CAL_REPORT, MAG_CAL_PROGRESS, EKF_STATUS_REPORT, VIBRATION, RPM, ESC TELEMETRY,GENERATOR_STATUS, and WINCH_STATUS</source>
      <translation>AHRS、SYSTEM_TIME、WIND、RANGEFINDER、DISTANCE_SENSOR、TERRAIN_REQUEST、BATTERY_STATUS、GIMBAL_DEVICE_ATTITUDE_STATUS、OPTICAL_FLOW、MAG_CAL_REPORT、MAG_CAL_PROGRESS、EKF_STATUS_REPORT、VIBRATION、RPM、ESC、TELEMETRY、GENERATOR_STATUS 的 MAVLink 流速率WINCH_STATUS</translation>
    </message>
    <message>
      <source>MAVLink Stream rate of RAW_IMU, SCALED_IMU2, SCALED_IMU3, SCALED_PRESSURE, SCALED_PRESSURE2, SCALED_PRESSURE3 and AIRSPEED</source>
      <translation>RAW_IMU、SCALED_IMU2、SCALED_IMU3、SCALED_PRESSURE、SCALED_PRESSURE2、SCALED_PRESSURE3 和 AIRSPEED 的 MAVLink 流速率</translation>
    </message>
    <message>
      <source>MAVLink component ID</source>
      <translation>MAVLink组件ID</translation>
    </message>
    <message>
      <source>MAVLink serial port</source>
      <translation>MAVLink串口</translation>
    </message>
    <message>
      <source>MAVLinkCamV2</source>
      <translation>MAVLinkCamV2</translation>
    </message>
    <message>
      <source>MAX31865 3 wire</source>
      <translation>MAX31865 3 电线</translation>
    </message>
    <message>
      <source>MLX90614</source>
      <translation>MLX90614</translation>
    </message>
    <message>
      <source>MPC_POS_MODE
1 just deceleration
4 not used, use MPC_ACC_HOR instead</source>
      <translation>MPC_POS_MODE 1 只是减速 4 没有使用，使用 MPC_ACC_HOR 代替</translation>
    </message>
    <message>
      <source>MPPT reports input voltage and current</source>
      <translation>MPPT 报告输入电压和电流</translation>
    </message>
    <message>
      <source>MSP_DISPLAYPORT</source>
      <translation>MSP_DISPLAYPORT</translation>
    </message>
    <message>
      <source>Magnetic anomaly height</source>
      <translation>磁异常高度</translation>
    </message>
    <message>
      <source>Magnetometer 0 Custom Euler Pitch Angle</source>
      <translation>磁力计 0 定制欧拉俯仰角</translation>
    </message>
    <message>
      <source>Magnetometer 0 Y-axis off diagonal scale factor</source>
      <translation>磁力计 0 Y 轴偏离对角线比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 0 priority</source>
      <translation>磁力计0优先</translation>
    </message>
    <message>
      <source>Magnetometer 1 X-axis scaling factor</source>
      <translation>磁力计 1 X 轴比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 1 Z-axis scaling factor</source>
      <translation>磁力计 1 Z 轴比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 2 X-axis off diagonal scale factor</source>
      <translation>磁力计 2 X 轴偏离对角线比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 2 Z-axis off diagonal scale factor</source>
      <translation>磁力计 2 Z 轴偏离对角线比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 3 Custom Euler Yaw Angle</source>
      <translation>磁力计 3 定制欧拉偏航角</translation>
    </message>
    <message>
      <source>Magnetometer 3 Y-axis scaling factor</source>
      <translation>磁力计 3 Y 轴比例因子</translation>
    </message>
    <message>
      <source>Magnetometer auto calibration</source>
      <translation>磁力计自动校准</translation>
    </message>
    <message>
      <source>Magnetometer data rate (Hz). Valid rates: 0 or any factor of 1000.</source>
      <translation>磁力计数据速率（Hz）。有效率：0 或 1000 的任何因子。</translation>
    </message>
    <message>
      <source>Magnetometer offset injected into the simulation (X-axis)</source>
      <translation>注入模拟的磁力计偏移（X 轴）</translation>
    </message>
    <message>
      <source>Magnetometer offset temperature ^1 polynomial coefficient - Z axis</source>
      <translation>磁力计偏移温度^1多项式系数-Z轴</translation>
    </message>
    <message>
      <source>Magnetometer soft-iron diagonal Y component</source>
      <translation>磁力计软铁对角Y分量</translation>
    </message>
    <message>
      <source>Main Priority</source>
      <translation>主要优先事项</translation>
    </message>
    <message>
      <source>MainOut1</source>
      <translation>主输出1</translation>
    </message>
    <message>
      <source>MainSail</source>
      <translation>主帆</translation>
    </message>
    <message>
      <source>Make Mount ROI the center of the circle</source>
      <translation>将安装ROI作为圆心</translation>
    </message>
    <message>
      <source>Manual control expo for roll</source>
      <translation>手动控制辊展</translation>
    </message>
    <message>
      <source>Manual input mapped to scale vertical velocity in position slow mode</source>
      <translation>手动输入映射到位置慢速模式下的刻度垂直速度</translation>
    </message>
    <message>
      <source>Manual servo override for swash set-up. Do not set this manually!</source>
      <translation>用于斜盘设置的手动舵机超控。不要手动设置此项！</translation>
    </message>
    <message>
      <source>Mapped to center throttle stick in Stabilized mode (see MPC_THR_CURVE).
Used for initialization of the hover thrust estimator (see MPC_USE_HTE).
The estimated hover thrust is used as base for zero vertical acceleration in altitude control.
The hover thrust is important for land detection to work correctly.</source>
      <translation>在稳定模式下映射到中心油门杆（参见 MPC_THR_CURVE）。用于初始化悬停推力估计器（参见 MPC_USE_HTE）。估计的悬停推力用作高度控制中零垂直加速度的基础。悬停推力对于陆地探测的正常工作非常重要。</translation>
    </message>
    <message>
      <source>MappyDot Sensor 2 Rotation</source>
      <translation>MappyDot 传感器 2 旋转</translation>
    </message>
    <message>
      <source>Margin in centi-meters to accept terrain data from the GCS. This can be used to allow older terrain data generated with less accurate latitude/longitude scaling to be used</source>
      <translation>接受来自 GCS 的地形数据的余量（以厘米为单位）。这可用于允许使用以不太准确的纬度/经度缩放生成的较旧地形数据</translation>
    </message>
    <message>
      <source>Mask Channel 11 as a mode button</source>
      <translation>屏蔽通道 11 作为模式按钮</translation>
    </message>
    <message>
      <source>Mask Channel 2 as a mode button</source>
      <translation>屏蔽通道 2 作为模式按钮</translation>
    </message>
    <message>
      <source>Mask of CAN servos/ESCs</source>
      <translation>CAN舵机/电调掩模</translation>
    </message>
    <message>
      <source>Mask of channels which are reversed. This is used to configure ESCs to reverse motor direction for unidirectional rotation. Do not use for channels selected with SERVO_BLH_3DMASK.</source>
      <translation>反转通道的掩码。这用于配置 ESC 以反转电机方向以实现单向旋转。请勿用于通过 SERVO_BLH_3DMASK 选择的通道。</translation>
    </message>
    <message>
      <source>Matrix Yaw Min</source>
      <translation>矩阵偏航最小值</translation>
    </message>
    <message>
      <source>Mavlink Serial Channel5</source>
      <translation>Mavlink串行通道5</translation>
    </message>
    <message>
      <source>Max Gain Only</source>
      <translation>仅最大增益</translation>
    </message>
    <message>
      <source>Max frequency to use as baseline for adding motor noise for the gyros and accels</source>
      <translation>用作增加陀螺仪和加速器电机噪声基线的最大频率</translation>
    </message>
    <message>
      <source>Max positive angle of pitch setpoint (only in MNT_MODE_OUT=AUX)</source>
      <translation>最大正桨距角设定值（仅适用于 MNT_MODE_OUT=AUX）</translation>
    </message>
    <message>
      <source>Max yaw error in degrees needed for waypoint heading acceptance</source>
      <translation>航路点航向接受所需的最大偏航误差（以度为单位）</translation>
    </message>
    <message>
      <source>MaxBotix MB12XX Sensor 4 Rotation</source>
      <translation>MaxBotix MB12XX 传感器 4 旋转</translation>
    </message>
    <message>
      <source>Maxbotix Sonar (mb12xx)</source>
      <translation>Maxbotix 声纳 (mb12xx)</translation>
    </message>
    <message>
      <source>Maximum AP_Logger MAVLink Backend buffer size</source>
      <translation>最大 AP_Logger MAVLink 后端缓冲区大小</translation>
    </message>
    <message>
      <source>Maximum Collective Pitch</source>
      <translation>最大总距间距</translation>
    </message>
    <message>
      <source>Maximum MAVLink sending rate for instance 0</source>
      <translation>实例 0 的最大 MAVLink 发送速率</translation>
    </message>
    <message>
      <source>Maximum RPM for ESC</source>
      <translation>ESC 的最大 RPM</translation>
    </message>
    <message>
      <source>Maximum Throttle for takeoff</source>
      <translation>起飞最大油门</translation>
    </message>
    <message>
      <source>Maximum airspeed allowed in the landed state</source>
      <translation>着陆状态允许的最大空速</translation>
    </message>
    <message>
      <source>Maximum allowed RAM usage to pass checks</source>
      <translation>通过检查允许的最大 RAM 使用量</translation>
    </message>
    <message>
      <source>Maximum allowed norm of the angular velocity in the landed state. Only used if neither airspeed nor groundspeed can be used for landing detection.</source>
      <translation>着陆状态下角速度的最大允许范数。仅当空速和地速都不能用于着陆检测时才使用。</translation>
    </message>
    <message>
      <source>Maximum angular velocity in roll axis</source>
      <translation>横滚轴最大角速度</translation>
    </message>
    <message>
      <source>Maximum change in throttle percentage per second. Lower limit  based on 1 microsend of servo increase per loop. Divide SCHED_LOOP_RATE by approximately 10 to determine minimum achievable value.</source>
      <translation>每秒油门百分比的最大变化。下限基于 1 微发送每个循环的舵机增量。将 SCHED_LOOP_RATE 除以大约 10 以确定可实现的最小值。</translation>
    </message>
    <message>
      <source>Maximum cyclic blade pitch angle in deg that corresponds to the setting for maximum cyclic pitch (H_CYC_MAX). Used for logging only. Zero disables swashplate logging.</source>
      <translation>最大循环叶片桨距角（以度为单位），对应于最大循环桨距 (H_CYC_MAX) 的设置。仅用于记录。零禁用旋转斜盘记录。</translation>
    </message>
    <message>
      <source>Maximum depth allowed before geofence triggers</source>
      <translation>地理围栏触发前允许的最大深度</translation>
    </message>
    <message>
      <source>Maximum downwards acceleration in climb rate controlled modes</source>
      <translation>爬升率控制模式下的最大向下加速度</translation>
    </message>
    <message>
      <source>Maximum height above ground when reliant on optical flow</source>
      <translation>依赖光流时距地面的最大高度</translation>
    </message>
    <message>
      <source>Maximum horizontal drift speed to use GPS</source>
      <translation>使用GPS的最大水平漂移速度</translation>
    </message>
    <message>
      <source>Maximum horizontal velocity setpoint in Position mode</source>
      <translation>位置模式下的最大水平速度设定值</translation>
    </message>
    <message>
      <source>Maximum lean angle autopilot can request.  Set to zero to use ANGLE_MAX parameter value</source>
      <translation>自动驾驶仪可以请求的最大倾斜角。  设置为零以使用 ANGLE_MAX 参数值</translation>
    </message>
    <message>
      <source>Maximum logging rate for mavlink backend</source>
      <translation>mavlink 后端的最大日志记录速率</translation>
    </message>
    <message>
      <source>Maximum normal load factor during pullup</source>
      <translation>上拉期间的最大正常负载系数</translation>
    </message>
    <message>
      <source>Maximum number of log files</source>
      <translation>最大日志文件数</translation>
    </message>
    <message>
      <source>Maximum offset cal speed error</source>
      <translation>最大偏移校准速度误差</translation>
    </message>
    <message>
      <source>Maximum pitch angle before FailureDetector triggers the attitude_failure flag. The flag triggers flight termination (if @CBRK_FLIGHTTERM = 0), which sets outputs to their failsafe values. On takeoff the flag triggers lockdown (irrespective of @CBRK_FLIGHTTERM), which disarms motors but does not set outputs to failsafe values. Setting this parameter to 0 disables the check</source>
      <translation>FailureDetector 触发 Attitude_failure 标志之前的最大俯仰角。该标志触发飞行终止（如果 @CBRK_FLIGHTTERM = 0），这会将输出设置为其故障安全值。起飞时，该标志会触发锁定（无论@CBRK_FLIGHTTERM如何），这会解除电机但不会将输出设置为故障安全值。将此参数设置为 0 将禁用检查</translation>
    </message>
    <message>
      <source>Maximum pitch up rate that the pitch controller demands (degrees/sec) in ACRO mode.</source>
      <translation>ACRO 模式下桨距控制器要求的最大桨距上升速率（度/秒）。</translation>
    </message>
    <message>
      <source>Maximum rate the yaw target can be updated in Loiter, RTL, Auto flight modes</source>
      <translation>Loiter、RTL、自动飞行模式下偏航目标更新的最大速率</translation>
    </message>
    <message>
      <source>Maximum roll rate setpoint</source>
      <translation>最大横滚速率设定值</translation>
    </message>
    <message>
      <source>Maximum speed that will be used to back away from obstacles vertically in height control modes (m/s). Set zero to disable vertical backup.</source>
      <translation>在高度控制模式下垂直向后远离障碍物的最大速度 (m/s)。设置为零以禁用垂直备份。</translation>
    </message>
    <message>
      <source>Maximum throttle percentage used in automatic throttle modes.</source>
      <translation>自动油门模式中使用的最大油门百分比。</translation>
    </message>
    <message>
      <source>Maximum time allowed for forward transitions, exceeding this time will cancel the transition and the aircraft will immediately change to QLAND. 0 for no limit.</source>
      <translation>允许向前过渡的最大时间，超过此时间将取消过渡，飞机将立即更改为 QLAND。 0无限制。</translation>
    </message>
    <message>
      <source>Maximum value for manual forward throttle; used with RC option FWD_THR (209)</source>
      <translation>手动前进油门最大值；与 RC 选件 FWD_THR (209) 一起使用</translation>
    </message>
    <message>
      <source>Maximum velocity setpoint in Position mode</source>
      <translation>位置模式下的最大速度设定值</translation>
    </message>
    <message>
      <source>Maximum voltage of battery. Provides scaling of current versus voltage</source>
      <translation>电池最大电压。提供电流与电压的缩放</translation>
    </message>
    <message>
      <source>Maximum yaw rate setpoint</source>
      <translation>最大偏航率设定值</translation>
    </message>
    <message>
      <source>Measurement noise</source>
      <translation>测量噪声</translation>
    </message>
    <message>
      <source>Measurement noise for auxiliary global position sensor 3</source>
      <translation>辅助全球位置传感器 3 的测量噪声</translation>
    </message>
    <message>
      <source>Measurement noise for vision angle measurements</source>
      <translation>视角测量的测量噪声</translation>
    </message>
    <message>
      <source>MegaSquirt EFI</source>
      <translation>巨型喷射 EFI</translation>
    </message>
    <message>
      <source>MicroStrain EKF data rate</source>
      <translation>MicroStrain EKF 数据速率</translation>
    </message>
    <message>
      <source>MicroStrain GNSS lever arm offset 1 (Y)</source>
      <translation>MicroStrain GNSS 杠杆臂偏移 1 (Y)</translation>
    </message>
    <message>
      <source>MicroStrain Sensor to vehicle transform (Roll)</source>
      <translation>MicroStrain 传感器到车辆变换（横滚）</translation>
    </message>
    <message>
      <source>MicroStrain optical flow offset (X)</source>
      <translation>MicroStrain 光流偏移 (X)</translation>
    </message>
    <message>
      <source>MidChannel</source>
      <translation>中频道</translation>
    </message>
    <message>
      <source>Min voltage for output limiting</source>
      <translation>输出限制最小电压</translation>
    </message>
    <message>
      <source>Minimum DShot Motor Output</source>
      <translation>最小 DShot 电机输出</translation>
    </message>
    <message>
      <source>Minimum Loiter altitude</source>
      <translation>最低盘旋高度</translation>
    </message>
    <message>
      <source>Minimum RPM for ESC</source>
      <translation>ESC 的最低 RPM</translation>
    </message>
    <message>
      <source>Minimum acceptable time until collsion.
Assumes constant speed over 3d distance.</source>
      <translation>碰撞前可接受的最短时间。假设 3d 距离内速度恒定。</translation>
    </message>
    <message>
      <source>Minimum altitude allowed before geofence triggers</source>
      <translation>地理围栏触发前允许的最低高度</translation>
    </message>
    <message>
      <source>Minimum altitude for fixed-wing flight. When the vehicle is in fixed-wing mode
and the altitude drops below this altitude (relative altitude above local origin),
it will instantly switch back to MC mode and execute behavior defined in COM_QC_ACT.</source>
      <translation>固定翼飞行的最低高度。当飞行器处于固定翼模式并且高度低于该高度（高于本地原点的相对高度）时，它将立即切换回 MC 模式并执行 COM_QC_ACT 中定义的行为。</translation>
    </message>
    <message>
      <source>Minimum camera trigger interval</source>
      <translation>最小相机触发间隔</translation>
    </message>
    <message>
      <source>Minimum depth to engage SURFTRAK mode</source>
      <translation>启用 SURFTRAK 模式的最小深度</translation>
    </message>
    <message>
      <source>Minimum duration during which the reported range finder signal quality needs to be non-zero in order to be declared valid (s)</source>
      <translation>报告的测距仪信号质量需要非零才能声明有效的最短持续时间（秒）</translation>
    </message>
    <message>
      <source>Minimum ground speed in cm/s when under airspeed control</source>
      <translation>空速控制下的最小地面速度（以厘米/秒为单位）</translation>
    </message>
    <message>
      <source>Minimum landing target standard covariance, uses reported covariance if greater</source>
      <translation>最小着陆目标标准协方差，如果更大则使用报告的协方差</translation>
    </message>
    <message>
      <source>Minimum output value (when not disarmed).</source>
      <translation>最小输出值（未上锁时）。</translation>
    </message>
    <message>
      <source>Minimum pitch during landing flare.</source>
      <translation>着陆拉平期间的最小俯仰角。</translation>
    </message>
    <message>
      <source>Minimum security of log level to display on the OSD.</source>
      <translation>OSD 上显示的日志级别的最低安全性。</translation>
    </message>
    <message>
      <source>Minimum speed for the throw start</source>
      <translation>投掷开始的最低速度</translation>
    </message>
    <message>
      <source>Minimum time between photos</source>
      <translation>照片之间的最短时间间隔</translation>
    </message>
    <message>
      <source>Minimum voltage on the autopilot power rail to allow the aircraft to arm. 0 to disable the check.</source>
      <translation>自动驾驶仪电源轨上允许飞机解锁的最小电压。 0 禁用该检查。</translation>
    </message>
    <message>
      <source>Mission Log</source>
      <translation>任务日志</translation>
    </message>
    <message>
      <source>Mixer Roll</source>
      <translation>搅拌辊</translation>
    </message>
    <message>
      <source>Mode slot 1</source>
      <translation>模式插槽 1</translation>
    </message>
    <message>
      <source>Mode to switch to when follow mode is exited normally</source>
      <translation>正常退出跟随模式时切换到的模式</translation>
    </message>
    <message>
      <source>Mode1+2+3+4+6</source>
      <translation>型号1+2+3+4+6</translation>
    </message>
    <message>
      <source>Mode1+2+5</source>
      <translation>模式1+2+5</translation>
    </message>
    <message>
      <source>Mode1+3+5</source>
      <translation>模式1+3+5</translation>
    </message>
    <message>
      <source>Mode1+5+6</source>
      <translation>模式1+5+6</translation>
    </message>
    <message>
      <source>Mode2+3+5</source>
      <translation>模式2+3+5</translation>
    </message>
    <message>
      <source>Mode2+5+6</source>
      <translation>模式2+5+6</translation>
    </message>
    <message>
      <source>Mode3+5+6</source>
      <translation>模式3+5+6</translation>
    </message>
    <message>
      <source>Mode6</source>
      <translation>模式6</translation>
    </message>
    <message>
      <source>ModeDrift</source>
      <translation>模式漂移</translation>
    </message>
    <message>
      <source>ModeRTL</source>
      <translation>模式RTL</translation>
    </message>
    <message>
      <source>Model with Pitot CAL_AIR_TUBED_MM: Not used, 1.5 mm tubes assumed. CAL_AIR_TUBELEN: Length of the tubes connecting the pitot to the sensor. Model without Pitot (1.5 mm tubes) CAL_AIR_TUBED_MM: Not used, 1.5 mm tubes assumed. CAL_AIR_TUBELEN: Length of the tubes connecting the pitot to the sensor. Tube Pressure Drop CAL_AIR_TUBED_MM: Diameter in mm of the pitot and tubes, must have the same diameter. CAL_AIR_TUBELEN: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.</source>
      <translation>带皮托管 CAL_AIR_TUBED_MM 的型号：未使用，假定为 1.5 mm 管。 CAL_AIR_TUBELEN：连接皮托管和传感器的管道长度。不带皮托管的型号（1.5 mm 管） CAL_AIR_TUBED_MM：未使用，假定为 1.5 mm 管。 CAL_AIR_TUBELEN：连接皮托管和传感器的管道长度。管压降 CAL_AIR_TUBED_MM：皮托管和管的直径（以毫米为单位）必须具有相同的直径。 CAL_AIR_TUBELEN：连接皮托管和传感器的管道长度以及皮托管的静态+动态端口长度。</translation>
    </message>
    <message>
      <source>Module ID 10</source>
      <translation>模块 ID 10</translation>
    </message>
    <message>
      <source>Module ID 18</source>
      <translation>模块 ID 18</translation>
    </message>
    <message>
      <source>Module ID 25</source>
      <translation>模块 ID 25</translation>
    </message>
    <message>
      <source>Module ID 32</source>
      <translation>模块 ID 32</translation>
    </message>
    <message>
      <source>Module ID 4</source>
      <translation>模块 ID 4</translation>
    </message>
    <message>
      <source>Module ID 47</source>
      <translation>模块 ID 47</translation>
    </message>
    <message>
      <source>Module ID 54</source>
      <translation>模块 ID 54</translation>
    </message>
    <message>
      <source>Module ID 61</source>
      <translation>模块 ID 61</translation>
    </message>
    <message>
      <source>Module Param - CVI for the X rectangular coordinate</source>
      <translation>模块参数 - CVI 用于 X 直角坐标</translation>
    </message>
    <message>
      <source>Module Param - The encoder angle at which theta is zero</source>
      <translation>Module Param - theta 为零时的编码器角度</translation>
    </message>
    <message>
      <source>Moment coefficient of rotor 2</source>
      <translation>转子力矩系数2</translation>
    </message>
    <message>
      <source>Momentum coefficient for propeller drag</source>
      <translation>螺旋桨阻力动量系数</translation>
    </message>
    <message>
      <source>Motor 10 slew rate limit</source>
      <translation>电机 10 转换速率限制</translation>
    </message>
    <message>
      <source>Motor 4</source>
      <translation>电机4</translation>
    </message>
    <message>
      <source>Motor 8</source>
      <translation>电机8</translation>
    </message>
    <message>
      <source>Motor Failure Current/Throttle Scale</source>
      <translation>电机故障电流/油门刻度</translation>
    </message>
    <message>
      <source>Motor Output PWM type</source>
      <translation>电机输出PWM型</translation>
    </message>
    <message>
      <source>Motor delay</source>
      <translation>电机延迟</translation>
    </message>
    <message>
      <source>Motor harmonics generated in SITL</source>
      <translation>SITL 中产生的电机谐波</translation>
    </message>
    <message>
      <source>Motor interference compensation to compass3 for body frame Y axis</source>
      <translation>机身Y轴对罗盘3电机干扰补偿</translation>
    </message>
    <message>
      <source>Motor options</source>
      <translation>电机选项</translation>
    </message>
    <message>
      <source>Motor1</source>
      <translation>电机1</translation>
    </message>
    <message>
      <source>Motor17</source>
      <translation>电机17</translation>
    </message>
    <message>
      <source>Motor24</source>
      <translation>电机24</translation>
    </message>
    <message>
      <source>Motor31</source>
      <translation>电机31</translation>
    </message>
    <message>
      <source>Motor9</source>
      <translation>电机9</translation>
    </message>
    <message>
      <source>Mount Device ID</source>
      <translation>安装设备ID</translation>
    </message>
    <message>
      <source>Mount Pitch stabilization lead time</source>
      <translation>安装间距稳定提前时间</translation>
    </message>
    <message>
      <source>Mount Type (None, Servo or MAVLink)</source>
      <translation>安装类型（无、舵机或 MAVLink）</translation>
    </message>
    <message>
      <source>Mount input mode</source>
      <translation>安装输入模式</translation>
    </message>
    <message>
      <source>Mount pitch angle when in retracted position</source>
      <translation>处于缩回位置时安装俯仰角</translation>
    </message>
    <message>
      <source>Mount1 Pitch</source>
      <translation>安装1间距</translation>
    </message>
    <message>
      <source>Mount2 Roll</source>
      <translation>云台2横滚</translation>
    </message>
    <message>
      <source>Mount2 yaw/pan angle when in retracted position</source>
      <translation>Mount2 处于缩回位置时的偏航/平移角度</translation>
    </message>
    <message>
      <source>Mount2's minimum pan angle</source>
      <translation>Mount2的最小航向角</translation>
    </message>
    <message>
      <source>Mount2's tilt (pitch) RC input channel</source>
      <translation>安装2的倾斜（俯仰）RC输入通道</translation>
    </message>
    <message>
      <source>MountOpen</source>
      <translation>挂载打开</translation>
    </message>
    <message>
      <source>Moving</source>
      <translation>搬家</translation>
    </message>
    <message>
      <source>Mtrs_Only_Qassist</source>
      <translation>Mtrs_Only_Qassist</translation>
    </message>
    <message>
      <source>Multi-Source</source>
      <translation>多源</translation>
    </message>
    <message>
      <source>Multicopter max rotational speed</source>
      <translation>多旋翼最大转速</translation>
    </message>
    <message>
      <source>Multiplied by the current throttle and added to compass2's y-axis values to compensate for motor interference</source>
      <translation>乘以当前油门并添加到compass2的y轴值以补偿电机干扰</translation>
    </message>
    <message>
      <source>Multiplied by the current throttle and added to compass3's z-axis values to compensate for motor interference</source>
      <translation>乘以当前油门并添加到compass3的z轴值以补偿电机干扰</translation>
    </message>
    <message>
      <source>Multiplied by the descent rate to calculate a dynamic altitude at which
to trigger the flare.
NOTE: max(FW_LND_FLALT, FW_LND_FL_TIME * descent rate) is taken as the flare altitude</source>
      <translation>乘以下降率即可计算触发耀斑的动态高度。 NOTE: max(FW_LND_FLALT, FW_LND_FL_TIME * 下降率)作为耀斑高度</translation>
    </message>
    <message>
      <source>Multirotor wind estimation selection</source>
      <translation>多旋翼测风选择</translation>
    </message>
    <message>
      <source>NCP5623 External</source>
      <translation>NCP5623 外部</translation>
    </message>
    <message>
      <source>NMEA (generic)</source>
      <translation>NMEA（通用）</translation>
    </message>
    <message>
      <source>NMEA Output rate. This controls the interval at which all the enabled NMEA messages are sent. Most NMEA systems expect 100ms (10Hz) or slower.</source>
      <translation>NMEA 输出速率。这控制发送所有启用的 NMEA 消息的时间间隔。大多数 NMEA 系统期望 100ms (10Hz) 或更慢。</translation>
    </message>
    <message>
      <source>NRA24_CAN</source>
      <translation>NRA24_CAN</translation>
    </message>
    <message>
      <source>NanoRadar_NRA24</source>
      <translation>纳米雷达_NRA24</translation>
    </message>
    <message>
      <source>Navigator</source>
      <translation>导航器</translation>
    </message>
    <message>
      <source>NeoPixel</source>
      <translation>新像素</translation>
    </message>
    <message>
      <source>Networking Enable</source>
      <translation>联网启用</translation>
    </message>
    <message>
      <source>Next flight UUID</source>
      <translation>下一飞行 UUID</translation>
    </message>
    <message>
      <source>No PWMs</source>
      <translation>不输出 PWM</translation>
    </message>
    <message>
      <source>No override</source>
      <translation>无覆盖</translation>
    </message>
    <message>
      <source>No(AutoArmOnce after checks are passed)</source>
      <translation>否（检查通过后自动布防一次）</translation>
    </message>
    <message>
      <source>Node ID to send ECU throttle messages to. Set to zero to disable ECU throttle messages. Set to 255 to broadcast to all ECUs.</source>
      <translation>将 ECU 节流消息发送到的节点 ID。设置为零以禁用 ECU 限制消息。设置为 255 以广播到所有 ECU。</translation>
    </message>
    <message>
      <source>NoopLoop_TOFSense</source>
      <translation>NoopLoop_TOFSense</translation>
    </message>
    <message>
      <source>Normalized tilt in Hover</source>
      <translation>悬停中的标准化倾斜</translation>
    </message>
    <message>
      <source>NotDisabled</source>
      <translation>未禁用</translation>
    </message>
    <message>
      <source>Notch Filter quality factor given by the notch centre frequency divided by its bandwidth.</source>
      <translation>陷波滤波器品质因数由陷波中心频率除以其带宽得出。</translation>
    </message>
    <message>
      <source>Note: Setting this value to 4 enables flight termination,
which will kill the vehicle on violation of the fence.</source>
      <translation>注意：将此值设置为 4 会启用飞行终止，这将在违反围栏时杀死车辆。</translation>
    </message>
    <message>
      <source>Notify the user if the IMU is clipping</source>
      <translation>如果 IMU 正在削波，则通知用户</translation>
    </message>
    <message>
      <source>Nudging enabled</source>
      <translation>启用轻推</translation>
    </message>
    <message>
      <source>Number of TOFSENSE-M CAN sensors connected</source>
      <translation>连接的 TOFSENSE-M CAN 传感器数量</translation>
    </message>
    <message>
      <source>Number of cells for battery 2</source>
      <translation>电池 2 节数</translation>
    </message>
    <message>
      <source>Number of motor electrical poles</source>
      <translation>电机极数</translation>
    </message>
    <message>
      <source>Number of samples to take when logging streams of IMU sensor readings.  Will be rounded down to a multiple of 32.</source>
      <translation>记录 IMU 传感器读数流时要采集的样本数。  将向下舍入为 32 的倍数。</translation>
    </message>
    <message>
      <source>Number of simulated baros to create in SITL</source>
      <translation>在 SITL 中创建的模拟巴罗数</translation>
    </message>
    <message>
      <source>Numerical velocity derivative low pass cutoff frequency</source>
      <translation>数值速度导数低通截止频率</translation>
    </message>
    <message>
      <source>OADatabase minimum altitude above home before storing obstacles</source>
      <translation>OADatabase 存储障碍物之前高于家的最小高度</translation>
    </message>
    <message>
      <source>ODI_X in the compass2 soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>罗盘 2 软铁校准矩阵中的 ODI_X 元素：[[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</translation>
    </message>
    <message>
      <source>ODI_Z in the compass2 soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>罗盘 2 软铁校准矩阵中的 ODI_Z 元素：[[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</translation>
    </message>
    <message>
      <source>OSD</source>
      <translation>OSD</translation>
    </message>
    <message>
      <source>OSD Warning Level</source>
      <translation>OSD 警告级别</translation>
    </message>
    <message>
      <source>Object Avoidance Path Planning algorithm to use</source>
      <translation>使用的物体回避路径规划算法</translation>
    </message>
    <message>
      <source>Octa</source>
      <translation>奥克塔</translation>
    </message>
    <message>
      <source>Offset (m) in the X direction if an Optical Flow sensor is connected. Requires MS_OPT_FLOW_EN to be enabled to be used.</source>
      <translation>如果连接了光流传感器，则 X 方向的偏移量 (m)。需要启用 MS_OPT_FLOW_EN 才能使用。</translation>
    </message>
    <message>
      <source>Offset for ESC numbering in DroneCAN ESC RawCommand messages. This allows for more efficient packing of ESC command messages. If your ESCs are on servo outputs 5 to 8 and you set this parameter to 4 then the ESC RawCommand will be sent with the first 4 slots filled. This can be used for more efficient usage of CAN bandwidth</source>
      <translation>DroneCAN ESC RawCommand 消息中 ESC 编号的偏移量。这可以更有效地打包 ESC 命令消息。如果您的电调位于舵机输出 5 至 8 上，并且您将此参数设置为 4，则 ESC RawCommand 将在第一个 4 插槽已填充的情况下发送。这可用于更有效地使用 CAN 带宽</translation>
    </message>
    <message>
      <source>Offset in volt as seen by the ADC input of the current sensor</source>
      <translation>电流传感器 ADC 输入所见的偏移（以伏特为单位）</translation>
    </message>
    <message>
      <source>Offset to be added to compass2's y-axis values to compensate for metal in the frame</source>
      <translation>添加到 compass2 的 y 轴值的偏移量以补偿框架中的金属</translation>
    </message>
    <message>
      <source>Old logs on the SD card will be deleted to maintain this amount of free space</source>
      <translation>SD 卡上的旧日志将被删除，以维持此可用空间量</translation>
    </message>
    <message>
      <source>On ground not moving test scale factor</source>
      <translation>地面不动测试比例因子</translation>
    </message>
    <message>
      <source>OneShot</source>
      <translation>一次射击</translation>
    </message>
    <message>
      <source>Only enabled during automatic runway takeoff and landing.
In all manual modes the wheel is directly controlled with yaw stick.</source>
      <translation>仅在自动跑道起飞和着陆期间启用。在所有手动模式下，车轮均由偏航杆直接控制。</translation>
    </message>
    <message>
      <source>Only use during takeoffs or landing see weathervane takeoff and land override parameters</source>
      <translation>仅在起飞或着陆期间使用，请参阅风向标起飞和着陆覆盖参数</translation>
    </message>
    <message>
      <source>OpenDroneID options</source>
      <translation>OpenDroneID 选项</translation>
    </message>
    <message>
      <source>Opportunistic precision landing</source>
      <translation>伺机精确着陆</translation>
    </message>
    <message>
      <source>Optical Flow sensor measurement noise</source>
      <translation>光流传感器测量噪声</translation>
    </message>
    <message>
      <source>Optical flow data will not fused by the estimators if the magnitude of the flow rate exceeds this value and control loops will be instructed to limit ground speed such that the flow rate produced by movement over ground is less than 50% of this value.</source>
      <translation>如果流量的大小超过该值，光流数据将不会被估计器融合，并且控制回路将被指示限制地面速度，使得地面上的移动产生的流量小于该值的 50%。</translation>
    </message>
    <message>
      <source>Optical flow nav gain full-scale height</source>
      <translation>光流导航增益全尺寸高度</translation>
    </message>
    <message>
      <source>Optical flow sensor type</source>
      <translation>光流传感器类型</translation>
    </message>
    <message>
      <source>Option to not jump to AFS_WP_COMMS if already in the return path</source>
      <translation>如果已经在返回路径中，则可以选择不跳转到 AFS_WP_COMMS</translation>
    </message>
    <message>
      <source>Options for OpenDroneID subsystem</source>
      <translation>OpenDroneID 子系统的选项</translation>
    </message>
    <message>
      <source>Options for Pin 4. PWM input detects PWM above or below 1800/1200us instead of logic level. If PWM is not detected or is less than 800us or above 2200us the button will interpreted as low. Invert changes HIGH state to be logic low voltage on pin, or below 1200us, if PWM input.</source>
      <translation>引脚 4 的选项。 PWM 输入检测 PWM 高于或低于 1800/1200us，而不是逻辑电平。如果未检测到 PWM 或小于 800us 或高于 2200us，则按钮将被解释为低电平。反转将 HIGH 状态更改为引脚上的逻辑低电压，或低于 1200us（如果 PWM 输入）。</translation>
    </message>
    <message>
      <source>Options for the BatchSampler. Post-filter and sensor-rate logging cannot be used at the same time.</source>
      <translation>BatchSampler 的选项。后置滤波器和传感器速率记录不能同时使用。</translation>
    </message>
    <message>
      <source>Options to control the LTE modem driver. If VerboseSignalInfoGCS is set then additional NAMED_VALUE_FLOAT values are sent with verbose signal information</source>
      <translation>控制 LTE 调制解调器驱动程序的选项。如果设置了 VerboseSignalInfoGCS，则附加 NAMED_VALUE_FLOAT 值将与详细信号信息一起发送</translation>
    </message>
    <message>
      <source>Orientation of 4th range finder</source>
      <translation>第四测距仪的方向</translation>
    </message>
    <message>
      <source>Oscillation is suppressed when vehicle and payload are no more than this distance horizontally.  Set to 0 to always suppress</source>
      <translation>当车辆和有效负载的水平距离不超过此距离时，振动就会受到抑制。  设置为 0 以始终抑制</translation>
    </message>
    <message>
      <source>Output 15</source>
      <translation>输出15</translation>
    </message>
    <message>
      <source>Output 22</source>
      <translation>输出22</translation>
    </message>
    <message>
      <source>Output 3</source>
      <translation>输出3</translation>
    </message>
    <message>
      <source>Output 8</source>
      <translation>输出8</translation>
    </message>
    <message>
      <source>Output Protocol Configuration for PWM Aux 7-8</source>
      <translation>PWM Aux 7-8 的输出协议配置</translation>
    </message>
    <message>
      <source>Output predictor position time constant</source>
      <translation>输出预测器位置时间常数</translation>
    </message>
    <message>
      <source>Output11</source>
      <translation>输出11</translation>
    </message>
    <message>
      <source>Output19</source>
      <translation>输出19</translation>
    </message>
    <message>
      <source>Output26</source>
      <translation>输出26</translation>
    </message>
    <message>
      <source>Output4</source>
      <translation>输出4</translation>
    </message>
    <message>
      <source>Overall Horizontal Velocity Limit</source>
      <translation>总水平速度限制</translation>
    </message>
    <message>
      <source>Override the weather vaning behaviour when in landing</source>
      <translation>覆盖着陆时的天气风向行为</translation>
    </message>
    <message>
      <source>Own node ID</source>
      <translation>自有节点ID</translation>
    </message>
    <message>
      <source>P gain for the speed PID controller velocity component</source>
      <translation>速度 PID 控制器速度分量的 P 增益</translation>
    </message>
    <message>
      <source>PAW3902/PAW3903 Optical Flow</source>
      <translation>PAW3902/PAW3903光流</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 10 Disarmed Value</source>
      <translation>PCA9685 输出通道 10 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 11 Maximum Value</source>
      <translation>PCA9685 输出通道 11 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 12 Output Function</source>
      <translation>PCA9685 输出通道 12 输出功能</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 14 Disarmed Value</source>
      <translation>PCA9685 输出通道 14 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 15 Maximum Value</source>
      <translation>PCA9685 输出通道 15 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 16 Output Function</source>
      <translation>PCA9685 输出通道 16 输出功能</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 3 Disarmed Value</source>
      <translation>PCA9685 输出通道 3 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 4 Maximum Value</source>
      <translation>PCA9685 输出通道 4 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 5 Output Function</source>
      <translation>PCA9685 输出通道 5 输出功能</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 7 Disarmed Value</source>
      <translation>PCA9685 输出通道 7 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 8 Maximum Value</source>
      <translation>PCA9685 输出通道 8 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 9 Output Function</source>
      <translation>PCA9685 输出通道 9 输出功能</translation>
    </message>
    <message>
      <source>PGA460 Ultrasonic driver (PGA460)</source>
      <translation>PGA460 超声波驱动器 (PGA460)</translation>
    </message>
    <message>
      <source>PID Integral Gain</source>
      <translation>PID 积分增益</translation>
    </message>
    <message>
      <source>PITCH_Y</source>
      <translation>PITCH_Y</translation>
    </message>
    <message>
      <source>POWER</source>
      <translation>POWER</translation>
    </message>
    <message>
      <source>PSC Angle Max</source>
      <translation>PSC 最大角度</translation>
    </message>
    <message>
      <source>PWM Aux 1 Disarmed Value</source>
      <translation>PWM 辅助 1 上锁值</translation>
    </message>
    <message>
      <source>PWM Aux 2 Failsafe Value</source>
      <translation>PWM 辅助 2 故障安全值</translation>
    </message>
    <message>
      <source>PWM Aux 3 Maximum Value</source>
      <translation>PWM 辅助 3 最大值</translation>
    </message>
    <message>
      <source>PWM Aux 4 Minimum Value</source>
      <translation>PWM 辅助 4 最小值</translation>
    </message>
    <message>
      <source>PWM Aux 5 Output Function</source>
      <translation>PWM 辅助 5 输出功能</translation>
    </message>
    <message>
      <source>PWM Aux 7</source>
      <translation>PWM 辅助 7</translation>
    </message>
    <message>
      <source>PWM Aux 8 Center Value</source>
      <translation>PWM 辅助 8 中心值</translation>
    </message>
    <message>
      <source>PWM Capture 1 Disarmed Value</source>
      <translation>PWM 捕获 1 上锁值</translation>
    </message>
    <message>
      <source>PWM Capture 2 Failsafe Value</source>
      <translation>PWM 捕获 2 故障安全值</translation>
    </message>
    <message>
      <source>PWM Capture 3 Maximum Value</source>
      <translation>PWM 捕捉 3 最大值</translation>
    </message>
    <message>
      <source>PWM disabled while disarmed</source>
      <translation>PWM 上锁时禁用</translation>
    </message>
    <message>
      <source>PWM on kill active</source>
      <translation>PWM 处于终止状态</translation>
    </message>
    <message>
      <source>PWM value for ignition off</source>
      <translation>PWM 点火关闭值</translation>
    </message>
    <message>
      <source>PWM value in microseconds to move servo to when shutter is deactivated</source>
      <translation>PWM 快门关闭时将舵机移动到的值（以微秒为单位）</translation>
    </message>
    <message>
      <source>PWMAngle</source>
      <translation>脉宽调制角度</translation>
    </message>
    <message>
      <source>PX4 AUX5</source>
      <translation>PX4 AUX5</translation>
    </message>
    <message>
      <source>PX4-PWM</source>
      <translation>PX4-PWM</translation>
    </message>
    <message>
      <source>PX4IO Relay2</source>
      <translation>PX4IO 继电器2</translation>
    </message>
    <message>
      <source>Parachute Servo ON PWM value</source>
      <translation>降落伞舵机 ON PWM 值</translation>
    </message>
    <message>
      <source>Parachute release</source>
      <translation>降落伞释放</translation>
    </message>
    <message>
      <source>Parameter group</source>
      <translation>参数组</translation>
    </message>
    <message>
      <source>Parameter stream rate to ground station</source>
      <translation>到地面站的参数流速率</translation>
    </message>
    <message>
      <source>Parameters+Rangefinder</source>
      <translation>参数+测距仪</translation>
    </message>
    <message>
      <source>Payload Place decent speed</source>
      <translation>有效负载放置速度不错</translation>
    </message>
    <message>
      <source>Percent of bytes lost from GPS 2</source>
      <translation>GPS 2 丢失的字节百分比</translation>
    </message>
    <message>
      <source>Percentage of throttle where the governor will disengage to allow return to flight idle power. Typically should be set to the same value as flight idle throttle (the very lowest throttle setting on the throttle curve). The governor disengage can be disabled by setting this value to zero and using the pull-down from the governor TCGAIN to reduce power to flight idle with the collective at it's lowest throttle setting on the throttle curve.</source>
      <translation>调速器将脱离以允许恢复飞行怠速功率的油门百分比。通常应设置为与飞行怠速油门相同的值（油门曲线上最低的油门设置）。可以通过将该值设置为零并使用调速器 TCGAIN 的下拉功能来禁用调速器脱离，以在油门曲线上总距处于最低油门设置时减少飞行怠速功率。</translation>
    </message>
    <message>
      <source>Peripheral via Actuator Set 1</source>
      <translation>通过执行器套件 1 的外围设备</translation>
    </message>
    <message>
      <source>PersistTemps</source>
      <translation>持续时间</translation>
    </message>
    <message>
      <source>Pick the appropriate scaling from the datasheet. this number defines the (linear) conversion from voltage to Pascal (pa). For the MPXV7002DP this is 1000. NOTE: If the sensor always registers zero, try switching the static and dynamic tubes.</source>
      <translation>从数据表中选择适当的缩放比例。该数字定义了从电压到帕斯卡 (pa) 的（线性）转换。对于 MPXV7002DP，这是 1000。 NOTE：如果传感器始终显示为零，请尝试切换静态管和动态管。</translation>
    </message>
    <message>
      <source>Pilot input steering type</source>
      <translation>先导输入转向类型</translation>
    </message>
    <message>
      <source>Pilot maximum vertical speed up</source>
      <translation>飞行员最大垂直加速</translation>
    </message>
    <message>
      <source>Pin number connected to RS485 to Serial converter's DE pin. -1 to use serial port's CTS pin if available</source>
      <translation>连接到 RS485 到串行转换器的 DE 引脚的引脚号。 -1 使用串行端口的 CTS 引脚（如果可用）</translation>
    </message>
    <message>
      <source>Pin that the leak detector is connected to</source>
      <translation>检漏仪连接的引脚</translation>
    </message>
    <message>
      <source>Pitch 180°, Yaw 270°</source>
      <translation>俯仰180°，偏航270°</translation>
    </message>
    <message>
      <source>Pitch Only</source>
      <translation>仅推介</translation>
    </message>
    <message>
      <source>Pitch angle P gain. If zero a gain of (1 / PTCH2SRV_TCONST) will be used.</source>
      <translation>俯仰角 P 增益。如果为零，则将使用 (1 / PTCH2SRV_TCONST) 增益。</translation>
    </message>
    <message>
      <source>Pitch axis controller D gain</source>
      <translation>俯仰轴控制器D增益</translation>
    </message>
    <message>
      <source>Pitch axis controller PD sum maximum</source>
      <translation>俯仰轴控制器PD和最大</translation>
    </message>
    <message>
      <source>Pitch axis rate controller D gain.  Compensates for short-term change in desired pitch rate vs actual pitch rate</source>
      <translation>俯仰轴速率控制器 D 增益。  补偿所需俯仰速率与实际俯仰速率的短期变化</translation>
    </message>
    <message>
      <source>Pitch axis rate controller I-term leak minimum</source>
      <translation>俯仰轴速率控制器 I 项泄漏最小</translation>
    </message>
    <message>
      <source>Pitch axis rate controller PD sum maximum.  The maximum/minimum value that the sum of the P and D term can output</source>
      <translation>俯仰轴速率控制器 PD 总和最大值。  P和D项之和可以输出的最大/最小值</translation>
    </message>
    <message>
      <source>Pitch control D gain</source>
      <translation>俯仰控制 D 增益</translation>
    </message>
    <message>
      <source>Pitch control I gain</source>
      <translation>我获得了俯仰控制</translation>
    </message>
    <message>
      <source>Pitch control Target filter frequency in Hz</source>
      <translation>变桨控制 目标滤波器频率（Hz）</translation>
    </message>
    <message>
      <source>Pitch control input filter.  Lower values reduce noise but add delay.</source>
      <translation>变桨控制输入滤波器。  较低的值会减少噪音，但会增加延迟。</translation>
    </message>
    <message>
      <source>Pitch damping gain when landing</source>
      <translation>着陆时俯仰阻尼增益</translation>
    </message>
    <message>
      <source>Pitch gain multiplier active when landed. A factor of 1.0 means no reduction in gain while landed. Reduce this factor to reduce ground oscitation in the pitch axis.</source>
      <translation>落地时俯仰增益倍增器处于活动状态。 1.0 系数意味着着陆时增益不会减少。减小该系数可减少俯仰轴上的地面振荡。</translation>
    </message>
    <message>
      <source>Pitch offset for dual antenna GPS</source>
      <translation>双天线GPS的俯仰偏移</translation>
    </message>
    <message>
      <source>Pitch rate differential gain.</source>
      <translation>俯仰率微分增益。</translation>
    </message>
    <message>
      <source>Pitch rate integrator limit. Can be set to increase the amount of integrator available to counteract disturbances or reduced to improve settling time after large pitch moment trim changes.</source>
      <translation>俯仰速率积分器限制。可以设置为增加可用于抵消干扰的积分器数量，或减少积分器数量以改善大俯仰力矩调整变化后的稳定时间。</translation>
    </message>
    <message>
      <source>Pitch trim</source>
      <translation>节距微调</translation>
    </message>
    <message>
      <source>Pitch180Roll270</source>
      <translation>俯仰180滚动270</translation>
    </message>
    <message>
      <source>Pitch315</source>
      <translation>节距315</translation>
    </message>
    <message>
      <source>Pitmode when disarmed</source>
      <translation>上锁时的坑模式</translation>
    </message>
    <message>
      <source>Pivot when the difference bewteen the vehicle's heading and it's target heading is more than this many degrees.  Set to zero to disable pivot turns</source>
      <translation>当车辆航向与其目标航向之间的差异超过这么多度数时旋转。  设置为零以禁用枢轴转动</translation>
    </message>
    <message>
      <source>Pixhawk ADC6/Pixhawk2 ADC</source>
      <translation>Pixhawk ADC6/Pixhawk2 ADC</translation>
    </message>
    <message>
      <source>Pixhawk Aux2</source>
      <translation>Pixhawk Aux2</translation>
    </message>
    <message>
      <source>Pixhawk TriggerPin51</source>
      <translation>Pixhawk TriggerPin51</translation>
    </message>
    <message>
      <source>Pixhawk2</source>
      <translation>像素鹰2</translation>
    </message>
    <message>
      <source>PixhawkAUX4</source>
      <translation>PixhawkAUX4</translation>
    </message>
    <message>
      <source>Plane Follow Scripting Altitude Override</source>
      <translation>飞机跟随脚本高度覆盖</translation>
    </message>
    <message>
      <source>Plane Follow Scripting speed D gain</source>
      <translation>平面跟随脚本速度 D 增益</translation>
    </message>
    <message>
      <source>Plane Follow crosstrack PID controller maximum correction</source>
      <translation>平面跟随交叉轨迹 PID 控制器最大修正</translation>
    </message>
    <message>
      <source>Point at which the motors start to spin expressed as a number from 0 to 1 in the entire output range.  Should be lower than MOT_SPIN_MIN.</source>
      <translation>电机开始旋转的点，在整个输出范围内用从 0 到 1 的数字表示。  应低于 MOT_SPIN_MIN。</translation>
    </message>
    <message>
      <source>Polarity for feedback pin. If this is 1 then the pin should be high when there is weight on wheels. If set to 0 then then weight on wheels level is low.</source>
      <translation>反馈引脚的极性。如果这是 1，那么当车轮上有重量时，该引脚应该处于高电平。如果设置为 0，则车轮重量较低。</translation>
    </message>
    <message>
      <source>PosHold braking rate</source>
      <translation>PosHold 制动率</translation>
    </message>
    <message>
      <source>Position (horizontal) controller P gain</source>
      <translation>位置（水平）控制器P增益</translation>
    </message>
    <message>
      <source>Position Horizontal Source (Tertiary)</source>
      <translation>位置水平源（第三）</translation>
    </message>
    <message>
      <source>Position controller P gain. Converts the distance (in the latitude direction) to the target location into a desired speed which is then passed to the loiter latitude rate controller. Previously _POSXY_P.</source>
      <translation>位置控制器 P 增益。将到目标位置的距离（纬度方向）转换为所需速度，然后将其传递给盘旋纬度速率控制器。以前的_POSXY_P。</translation>
    </message>
    <message>
      <source>Position of rotor 1 along Y body axis relative to center of gravity</source>
      <translation>转子 1 沿 Y 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 2 along X body axis relative to center of gravity</source>
      <translation>转子 2 沿 X 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 4 along Z body axis relative to center of gravity</source>
      <translation>转子 4 沿 Z 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 7 along Y body axis relative to center of gravity</source>
      <translation>转子 7 沿 Y 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position propagation noise density</source>
      <translation>位置传播噪声密度</translation>
    </message>
    <message>
      <source>Positive real value that configures the transmission frequency of the HIGH_LATENCY2 stream for instance 1, configured in iridium mode. This parameter has no effect if the instance mode is different from iridium.</source>
      <translation>配置 HIGH_LATENCY2 流的传输频率的正实数值，例如 1，在铱星模式下配置。如果实例模式与iridium不同，则该参数无效。</translation>
    </message>
    <message>
      <source>Power Module / Analog</source>
      <translation>电源模块/模拟</translation>
    </message>
    <message>
      <source>PrecLand Maximum number of retires for a failed landing. Set to zero to disable landing retry.</source>
      <translation>PrecLand 因着陆失败而退出的最大次数。设置为零以禁用着陆重试。</translation>
    </message>
    <message>
      <source>Precision Land</source>
      <translation>精准降落</translation>
    </message>
    <message>
      <source>Precision Landing sensor lag, to cope with variable landing_target latency</source>
      <translation>精确着陆传感器滞后，以应对可变的landing_target延迟</translation>
    </message>
    <message>
      <source>Precland device center's longitude</source>
      <translation>Precland装置中心的经度</translation>
    </message>
    <message>
      <source>Precland device origin's longitude</source>
      <translation>Precland 设备原点的经度</translation>
    </message>
    <message>
      <source>Prefix LQ with RF Mode</source>
      <translation>前缀为 LQ 的 RF 模式</translation>
    </message>
    <message>
      <source>Pressure position error coefficient for the negative Y axis</source>
      <translation>Y轴负方向压力位置误差系数</translation>
    </message>
    <message>
      <source>Primary core number</source>
      <translation>主芯数</translation>
    </message>
    <message>
      <source>Prio: RC &gt; MAVL 2 &gt; MAVL 1</source>
      <translation>优先级：RC &gt; MAVL 2 &gt; MAVL 1</translation>
    </message>
    <message>
      <source>Process noise for earth magnetic field prediction</source>
      <translation>地磁场预测的过程噪声</translation>
    </message>
    <message>
      <source>ProfiLED Clock</source>
      <translation>轮廓LED时钟</translation>
    </message>
    <message>
      <source>Progressive crow flaps</source>
      <translation>渐进式乌鸦皮瓣</translation>
    </message>
    <message>
      <source>Proportional gain for horizontal trajectory position error</source>
      <translation>水平轨迹位置误差比例增益</translation>
    </message>
    <message>
      <source>Proportional gain from roll angle demands to ailerons. Higher values allow more servo response but can cause oscillations. Automatically set and adjusted by AUTOTUNE mode.</source>
      <translation>副翼横滚角需求的比例增益。较高的值允许更多的舵机响应，但可能会导致振荡。通过AUTOTUNE模式自动设置和调整。</translation>
    </message>
    <message>
      <source>Proximity lowest altitude.</source>
      <translation>接近最低海拔。</translation>
    </message>
    <message>
      <source>Proximity sensor ignore angle 5</source>
      <translation>接近传感器忽略角度 5</translation>
    </message>
    <message>
      <source>Proximity sensor land detection</source>
      <translation>接近传感器陆地检测</translation>
    </message>
    <message>
      <source>Publish primary magnetometer</source>
      <translation>发布初级磁力计</translation>
    </message>
    <message>
      <source>Put CH1 to Duty-Cycle mode</source>
      <translation>将 CH1 置于占空比模式</translation>
    </message>
    <message>
      <source>Put CH2 to Duty-Cycle mode</source>
      <translation>将 CH2 置于占空比模式</translation>
    </message>
    <message>
      <source>Put radio into FCC test mode</source>
      <translation>将无线电置于 FCC 测试模式</translation>
    </message>
    <message>
      <source>QACRO</source>
      <translation>QACRO</translation>
    </message>
    <message>
      <source>QLOITER</source>
      <translation>QLOITER</translation>
    </message>
    <message>
      <source>QRTL Mode</source>
      <translation>QRTL模式</translation>
    </message>
    <message>
      <source>Q_RTL_ALT should be &lt; 120m (400ft). 3 or less to prevent arming. -1 to disable.</source>
      <translation>Q_RTL_ALT 应 &lt; 120m (400ft)。 3以下防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>Quad-chute transition altitude loss threshold</source>
      <translation>四滑道过渡高度损失阈值</translation>
    </message>
    <message>
      <source>Quadplane assistance options</source>
      <translation>四平面辅助选项</translation>
    </message>
    <message>
      <source>QuickTune</source>
      <translation>快速调音</translation>
    </message>
    <message>
      <source>Quicktune doubling time</source>
      <translation>Quicktune 时间加倍</translation>
    </message>
    <message>
      <source>RAM storage</source>
      <translation>RAM存储</translation>
    </message>
    <message>
      <source>RC AUX 6</source>
      <translation>RC AUX 6</translation>
    </message>
    <message>
      <source>RC Channels</source>
      <translation>RC 通道</translation>
    </message>
    <message>
      <source>RC Out channels to be transmitted as ESC over CAN bus</source>
      <translation>RC 输出通道作为 ESC 通过 CAN 总线传输</translation>
    </message>
    <message>
      <source>RC Targeting</source>
      <translation>RC 定位</translation>
    </message>
    <message>
      <source>RC channel 10 minimum</source>
      <translation>RC 通道 10 最小值</translation>
    </message>
    <message>
      <source>RC channel 12 minimum</source>
      <translation>RC 通道 12 最小值</translation>
    </message>
    <message>
      <source>RC channel 14 minimum</source>
      <translation>RC 通道 14 最小值</translation>
    </message>
    <message>
      <source>RC channel 16 minimum</source>
      <translation>RC 通道 16 最小值</translation>
    </message>
    <message>
      <source>RC channel 18 minimum</source>
      <translation>RC 通道 18 最小值</translation>
    </message>
    <message>
      <source>RC channel 3 minimum</source>
      <translation>RC 通道 3 最小值</translation>
    </message>
    <message>
      <source>RC channel 5 minimum</source>
      <translation>RC 通道 5 最小值</translation>
    </message>
    <message>
      <source>RC channel 7 minimum</source>
      <translation>RC 通道 7 最小值</translation>
    </message>
    <message>
      <source>RC channel 9 minimum</source>
      <translation>RC 通道 9 最小值</translation>
    </message>
    <message>
      <source>RC failsafe will trigger this many seconds after loss of RC</source>
      <translation>RC 故障保护将在 RC 丢失后这么多秒内触发</translation>
    </message>
    <message>
      <source>RC link SNR warn level (in %)</source>
      <translation>RC 链接 SNR 警告级别（以 % 为单位）</translation>
    </message>
    <message>
      <source>RC minimum PWM pulse width. Typically 1000 is lower limit, 1500 is neutral and 2000 is upper limit.</source>
      <translation>RC 最小 PWM 脉冲宽度。通常，1000 为下限，1500 为中性，2000 为上限。</translation>
    </message>
    <message>
      <source>RC pitch and roll control radius and rate</source>
      <translation>RC 俯仰和横滚控制半径和速率</translation>
    </message>
    <message>
      <source>RC11</source>
      <translation>RC11</translation>
    </message>
    <message>
      <source>RCIN</source>
      <translation>RCIN</translation>
    </message>
    <message>
      <source>RCIN12Scaled</source>
      <translation>RCIN12缩放</translation>
    </message>
    <message>
      <source>RCIN16Scaled</source>
      <translation>RCIN16缩放</translation>
    </message>
    <message>
      <source>RCIN4</source>
      <translation>RCIN4</translation>
    </message>
    <message>
      <source>RCIN6Scaled</source>
      <translation>RCIN6缩放</translation>
    </message>
    <message>
      <source>RCIN9Scaled</source>
      <translation>RCIN9缩放</translation>
    </message>
    <message>
      <source>RC_LQ_Y</source>
      <translation>RC_LQ_Y</translation>
    </message>
    <message>
      <source>RCn_OPTION number to use to control tuning stop/start/save</source>
      <translation>RCn_OPTION 用于控制调谐停止/启动/保存的编号</translation>
    </message>
    <message>
      <source>RESTVOLT_X</source>
      <translation>RESTVOLT_X</translation>
    </message>
    <message>
      <source>ROCKET</source>
      <translation>ROCKET</translation>
    </message>
    <message>
      <source>ROTATION_FORWARD_FACING</source>
      <translation>ROTATION_FORWARD_FACING</translation>
    </message>
    <message>
      <source>ROTATION_PITCH_90</source>
      <translation>ROTATION_PITCH_90</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_180_YAW_315</source>
      <translation>ROTATION_ROLL_180_YAW_315</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_270_YAW_45</source>
      <translation>ROTATION_ROLL_270_YAW_45</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_90_PITCH_90</source>
      <translation>ROTATION_ROLL_90_PITCH_90</translation>
    </message>
    <message>
      <source>ROTATION_YAW_225</source>
      <translation>ROTATION_YAW_225</translation>
    </message>
    <message>
      <source>RPM Setpoint for Idle Governor</source>
      <translation>RPM 怠速调速器设定点</translation>
    </message>
    <message>
      <source>RPM type</source>
      <translation>RPM型</translation>
    </message>
    <message>
      <source>RSSI PWM low value</source>
      <translation>RSSI PWM 低值</translation>
    </message>
    <message>
      <source>RSSI signal strength</source>
      <translation>RSSI信号强度</translation>
    </message>
    <message>
      <source>RSSI_X</source>
      <translation>RSSI_X</translation>
    </message>
    <message>
      <source>RTL Altitude</source>
      <translation>返航高度</translation>
    </message>
    <message>
      <source>RTL loiter radius</source>
      <translation>RTL盘旋半径</translation>
    </message>
    <message>
      <source>RTL or Land</source>
      <translation>劳教或土地</translation>
    </message>
    <message>
      <source>RTL_ALT should be a valid value</source>
      <translation>RTL_ALT 应该是一个有效值</translation>
    </message>
    <message>
      <source>RTPUDP</source>
      <translation>RTPUDP</translation>
    </message>
    <message>
      <source>Radio Controller</source>
      <translation>无线电控制器</translation>
    </message>
    <message>
      <source>Rally Total</source>
      <translation>拉力赛总计</translation>
    </message>
    <message>
      <source>Range finder can be used as the primary height source when below this percentage of its maximum range (see RNGFND*_MAX). This will not work unless Baro or GPS height is selected as the primary height source vis EK2_ALT_SOURCE = 0 or 2 respectively.  This feature should not be used for terrain following as it is designed  for vertical takeoff and landing with climb above  the range finder use height before commencing the mission, and with horizontal position changes below that height being limited to a flat region around the takeoff and landing point.</source>
      <translation>当测距仪低于其最大范围的这个百分比时，可以用作主要高度源（参见 RNGFND*_MAX）。除非分别选择 Baro 或 GPS 高度作为主要高度源（相对于 EK2_ALT_SOURCE = 0 或 2），否则此功能将不起作用。  此功能不应用于地形跟踪，因为它设计用于垂直起飞和着陆，在开始任务之前爬升至测距仪使用高度之上，并且低于该高度的水平位置变化仅限于起飞和着陆点周围的平坦区域。</translation>
    </message>
    <message>
      <source>Range finder measurement gate size</source>
      <translation>测距仪测量门尺寸</translation>
    </message>
    <message>
      <source>Range of travel</source>
      <translation>行程范围</translation>
    </message>
    <message>
      <source>RangeFinder CAN receive ID</source>
      <translation>测距仪 CAN 接收 ID</translation>
    </message>
    <message>
      <source>Rangefinder 4 orientation</source>
      <translation>测距仪4方向</translation>
    </message>
    <message>
      <source>Rangefinder orientation</source>
      <translation>测距仪方向</translation>
    </message>
    <message>
      <source>Rate Feedforward Enable</source>
      <translation>速率前馈启用</translation>
    </message>
    <message>
      <source>Rate Roll FF</source>
      <translation>速率横滚 FF</translation>
    </message>
    <message>
      <source>Rate Yaw FF</source>
      <translation>速率偏航 FF</translation>
    </message>
    <message>
      <source>Rate gyro noise for covariance prediction</source>
      <translation>用于协方差预测的速率陀螺仪噪声</translation>
    </message>
    <message>
      <source>RatePitchI</source>
      <translation>速率间距I</translation>
    </message>
    <message>
      <source>RateRollPI</source>
      <translation>速率滚动PI</translation>
    </message>
    <message>
      <source>Ratio between P and I gains for roll and pitch. Raise this to get a lower I gain</source>
      <translation>横滚和俯仰的 P 增益与 I 增益之比。提高此值可降低 I 增益</translation>
    </message>
    <message>
      <source>Ratio between steering FF and I gains. Raise this to get a higher I gain, 0 to leave I unchanged</source>
      <translation>转向 FF 和 I 增益之间的比率。提高此值以获得更高的 I 增益，0 使 I 保持不变</translation>
    </message>
    <message>
      <source>Ratio of vertical thrust during decent below which payload touchdown will trigger.</source>
      <translation>下降过程中的垂直推力比，低于该比将触发有效载荷着陆。</translation>
    </message>
    <message>
      <source>Raw data</source>
      <translation>原始数据</translation>
    </message>
    <message>
      <source>Re-enable on sensor recovery</source>
      <translation>传感器恢复后重新启用</translation>
    </message>
    <message>
      <source>Readings stopped</source>
      <translation>读数停止</translation>
    </message>
    <message>
      <source>Recovery Roll Angle</source>
      <translation>恢复侧倾角</translation>
    </message>
    <message>
      <source>Reduces vibrations by lowering high frequency torque caused by rotor acceleration.
0 disables the filter</source>
      <translation>通过降低转子加速引起的高频扭矩来减少振动。 0 禁用滤波器</translation>
    </message>
    <message>
      <source>Relative to Home</source>
      <translation>相对于家</translation>
    </message>
    <message>
      <source>Relay default state</source>
      <translation>继电器默认状态</translation>
    </message>
    <message>
      <source>Relay5</source>
      <translation>继电器5</translation>
    </message>
    <message>
      <source>Remaining flight time low failsafe</source>
      <translation>剩余飞行时间低故障保护</translation>
    </message>
    <message>
      <source>Report Only</source>
      <translation>仅报告</translation>
    </message>
    <message>
      <source>Require Arming Motors</source>
      <translation>需要解锁电机</translation>
    </message>
    <message>
      <source>Require both a takeoff and a landing, or neither</source>
      <translation>需要起飞和着陆，或者两者都不需要</translation>
    </message>
    <message>
      <source>Required XOR of CRC32 checksum of loaded scripts, vehicle will not arm with incorrect scripts loaded, -1 disables</source>
      <translation>加载脚本所需的 CRC32 校验和需要 XOR，加载的脚本不正确时车辆不会装备，-1 禁用</translation>
    </message>
    <message>
      <source>Required number of redundant power modules</source>
      <translation>所需冗余电源模块数量</translation>
    </message>
    <message>
      <source>Required throttle (at sea level, standard atmosphere) for level flight at minimum airspeed FW_AIRSPD_MIN
Set to 0 to disable mapping of airspeed to trim throttle below FW_AIRSPD_TRIM.</source>
      <translation>以最小空速 FW_AIRSPD_MIN 进行水平飞行所需的油门（在海平面、标准大气下） 设置为 0 以禁用空速映射以将油门调整到低于 FW_AIRSPD_TRIM。</translation>
    </message>
    <message>
      <source>Reset ALT_OFFSET on flight mode or AUTO waypoint changes</source>
      <translation>在飞行模式或 AUTO 航路点更改时重置 ALT_OFFSET</translation>
    </message>
    <message>
      <source>Respect takeoff frame types</source>
      <translation>尊重起飞机架类型</translation>
    </message>
    <message>
      <source>Retract Mount</source>
      <translation>缩回安装</translation>
    </message>
    <message>
      <source>Return</source>
      <translation>返回</translation>
    </message>
    <message>
      <source>Return switch channel</source>
      <translation>返航开关通道</translation>
    </message>
    <message>
      <source>Return via direct path to closest destination: home, start of mission landing pattern or safe point. If the destination is a mission landing pattern, follow the pattern to land.</source>
      <translation>通过直接路径返回最近的目的地：家、任务起始着陆点或安全点。如果目的地是任务着陆航线，则按照该航线着陆。</translation>
    </message>
    <message>
      <source>Reverse Output Range for Roboclaw Driver</source>
      <translation>机械爪驱动器的反向输出范围</translation>
    </message>
    <message>
      <source>Reverse Output Range for Vertiq IO</source>
      <translation>Vertiq IO 的反向输出范围</translation>
    </message>
    <message>
      <source>Reverse the closing direction.</source>
      <translation>反转关闭方向。</translation>
    </message>
    <message>
      <source>Right (pitch) axis with origin relative to vehicle centre of gravity</source>
      <translation>右（俯仰）轴，其原点相对于车辆重心</translation>
    </message>
    <message>
      <source>Right side up</source>
      <translation>右侧朝上</translation>
    </message>
    <message>
      <source>Roboclaw Driver Channel 1</source>
      <translation>机械爪驱动程序通道 1</translation>
    </message>
    <message>
      <source>Roboclaw Driver Channel 2 Failsafe Value</source>
      <translation>Roboclaw 驱动程序通道 2 故障安全值</translation>
    </message>
    <message>
      <source>Rockblock Scripting Serial Port</source>
      <translation>Rockblock 脚本串行端口</translation>
    </message>
    <message>
      <source>Roll 180°, Yaw 135°</source>
      <translation>横滚 180°、偏航 135°</translation>
    </message>
    <message>
      <source>Roll 270°, Pitch 270°</source>
      <translation>横滚 270°、俯仰 270°</translation>
    </message>
    <message>
      <source>Roll 90°, Pitch 270°</source>
      <translation>横滚 90°、俯仰 270°</translation>
    </message>
    <message>
      <source>Roll Derivative FeedForward Gain</source>
      <translation>横滚导数前馈增益</translation>
    </message>
    <message>
      <source>Roll acceleration limit. Setting to zero disables input shaping.</source>
      <translation>横滚加速度限制。设置为零会禁用输入整形。</translation>
    </message>
    <message>
      <source>Roll axis angle controller P gain.  Converts the error between the desired roll angle and actual angle to a desired roll rate</source>
      <translation>横滚轴角度控制器 P 增益。  将所需横滚角度与实际角度之间的误差转换为所需横滚速率</translation>
    </message>
    <message>
      <source>Roll axis rate controller I gain.  Corrects long-term difference in desired roll rate vs actual roll rate</source>
      <translation>横滚轴速率控制器 I 增益。  纠正所需横滚速率与实际横滚速率的长期差异</translation>
    </message>
    <message>
      <source>Roll axis rate controller derivative frequency in Hz</source>
      <translation>横滚轴速率控制器导数频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Roll control channel mapping</source>
      <translation>横滚控制通道映射</translation>
    </message>
    <message>
      <source>Roll gain for manual inputs in manual control mode</source>
      <translation>手动控制模式下手动输入的横滚增益</translation>
    </message>
    <message>
      <source>Roll proportional gain, i.e. desired angular speed in rad/s for error 1 rad.</source>
      <translation>横滚比例增益，即误差 1 rad 所需的角速度（以 rad/s 为单位）。</translation>
    </message>
    <message>
      <source>Roll rate feedforward</source>
      <translation>横滚率前馈</translation>
    </message>
    <message>
      <source>Roll stabilization lead time</source>
      <translation>横摇稳定前置时间</translation>
    </message>
    <message>
      <source>Roll/Pitch/Yaw</source>
      <translation>横滚/俯仰/偏航</translation>
    </message>
    <message>
      <source>Roll180Yaw45</source>
      <translation>横滚180偏航45</translation>
    </message>
    <message>
      <source>Roll270Yaw45</source>
      <translation>横滚270偏航45</translation>
    </message>
    <message>
      <source>Roll90Pitch315</source>
      <translation>滚动90俯仰315</translation>
    </message>
    <message>
      <source>Rotation forward</source>
      <translation>向前旋转</translation>
    </message>
    <message>
      <source>Rotation upward</source>
      <translation>向上旋转</translation>
    </message>
    <message>
      <source>Rotor 10 tilt assignment</source>
      <translation>转子 10 倾斜分配</translation>
    </message>
    <message>
      <source>Rotor 8 tilt assignment</source>
      <translation>转子 8 倾斜分配</translation>
    </message>
    <message>
      <source>Rover</source>
      <translation>罗孚</translation>
    </message>
    <message>
      <source>Rover Quicktune Speed FF to P ratio</source>
      <translation>流动站 Quicktune 速度 FF 与 P 的比率</translation>
    </message>
    <message>
      <source>Rover Quicktune enable</source>
      <translation>流动站 Quicktune 启用</translation>
    </message>
    <message>
      <source>Run PCF8583 driver automatically</source>
      <translation>自动运行PCF8583驱动程序</translation>
    </message>
    <message>
      <source>RunCam Split Micro/RunCam with UART</source>
      <translation>RunCam 分体式 Micro/RunCam 与 UART</translation>
    </message>
    <message>
      <source>RunCam features available</source>
      <translation>RunCam 可用功能</translation>
    </message>
    <message>
      <source>Rx</source>
      <translation>接收</translation>
    </message>
    <message>
      <source>SATS_X</source>
      <translation>SATS_X</translation>
    </message>
    <message>
      <source>SBG</source>
      <translation>SBG</translation>
    </message>
    <message>
      <source>SBus servo out</source>
      <translation>SBus舵机输出</translation>
    </message>
    <message>
      <source>SDCard Mission size</source>
      <translation>SD卡任务大小</translation>
    </message>
    <message>
      <source>SERIAL7</source>
      <translation>SERIAL7</translation>
    </message>
    <message>
      <source>SERVO3</source>
      <translation>SERVO3</translation>
    </message>
    <message>
      <source>SERVOn_MAX</source>
      <translation>SERVOn_MAX</translation>
    </message>
    <message>
      <source>SF10/b</source>
      <translation>SF10/b</translation>
    </message>
    <message>
      <source>SIDEBARS_EN</source>
      <translation>SIDEBARS_EN</translation>
    </message>
    <message>
      <source>SIM Channel 11</source>
      <translation>SIM 通道 11</translation>
    </message>
    <message>
      <source>SIM Channel 15</source>
      <translation>SIM 通道 15</translation>
    </message>
    <message>
      <source>SIM Channel 4</source>
      <translation>SIM 通道 4</translation>
    </message>
    <message>
      <source>SIM Channel 8</source>
      <translation>SIM 通道 8</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 1 Disarmed Value</source>
      <translation>SIM_GZ ESC 1 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 2 Maximum Value</source>
      <translation>SIM_GZ ESC 2 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 3 Output Function</source>
      <translation>SIM_GZ ESC 3 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 5 Disarmed Value</source>
      <translation>SIM_GZ ESC 5 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 6 Maximum Value</source>
      <translation>SIM_GZ ESC 6 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 7 Output Function</source>
      <translation>SIM_GZ ESC 7 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 1 Disarmed Value</source>
      <translation>SIM_GZ 舵机 1 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 2 Maximum Value</source>
      <translation>SIM_GZ 舵机 2 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 3 Output Function</source>
      <translation>SIM_GZ 舵机 3 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 5 Disarmed Value</source>
      <translation>SIM_GZ 舵机 5 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 6 Maximum Value</source>
      <translation>SIM_GZ 舵机 6 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 7 Output Function</source>
      <translation>SIM_GZ 舵机 7 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 1 Disarmed Value</source>
      <translation>SIM_GZ 轮子 1 解除值</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 2 Maximum Value</source>
      <translation>SIM_GZ 轮子 2 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 3 Output Function</source>
      <translation>SIM_GZ 轮子 3 输出功能</translation>
    </message>
    <message>
      <source>SIM_Precland follow ship</source>
      <translation>SIM_Precland 跟随船</translation>
    </message>
    <message>
      <source>SITL enable bandwidth limitting on telemetry ports with non-zero values</source>
      <translation>SITL 对具有非零值的遥测端口启用带宽限制</translation>
    </message>
    <message>
      <source>SITL vicon position glitch Down</source>
      <translation>SITL vicon 位置毛刺 向下</translation>
    </message>
    <message>
      <source>SITL vicon type mask</source>
      <translation>SITL vicon型面罩</translation>
    </message>
    <message>
      <source>SITL_Gazebo</source>
      <translation>SITL_凉亭</translation>
    </message>
    <message>
      <source>SMBUS Smart battery driver BQ40Z50 and BQ40Z80</source>
      <translation>SMBUS 智能电池驱动器 BQ40Z50 和 BQ40Z80</translation>
    </message>
    <message>
      <source>SPC_VEHICLE_RESP</source>
      <translation>SPC_VEHICLE_RESP</translation>
    </message>
    <message>
      <source>ST24</source>
      <translation>ST24</translation>
    </message>
    <message>
      <source>STEERING Mode</source>
      <translation>STEERING模式</translation>
    </message>
    <message>
      <source>Safety button</source>
      <translation>安全按钮</translation>
    </message>
    <message>
      <source>Sagetech MXS Participant Configuration</source>
      <translation>Sagetech MXS 参与者配置</translation>
    </message>
    <message>
      <source>Sail Heel Target filter frequency in Hz</source>
      <translation>帆跟目标滤波器频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Sail Heel control I gain maximum.  Constrains the maximum I term contribution to the main sail output (range -1 to +1)</source>
      <translation>帆跟控制我获得最大。  限制主帆输出的最大 I 项贡献（范围 -1 至 +1）</translation>
    </message>
    <message>
      <source>Sail heel slew rate limit</source>
      <translation>帆跟回转速率限制</translation>
    </message>
    <message>
      <source>Sailboat minimum wind speed to sail in</source>
      <translation>帆船最低航行风速</translation>
    </message>
    <message>
      <source>Same as Lead vehicle</source>
      <translation>与主车相同</translation>
    </message>
    <message>
      <source>Sat count (EKF2_REQ_NSATS)</source>
      <translation>周六计数 (EKF2_REQ_NSATS)</translation>
    </message>
    <message>
      <source>Save config</source>
      <translation>保存配置</translation>
    </message>
    <message>
      <source>Scale applied to the demanded pitch (below VT_PITCH_MIN) to get the fixed-wing forward actuation in hover mode.
Enabled via VT_FWD_THRUST_EN.</source>
      <translation>将比例应用于所需的俯仰角（低于 VT_PITCH_MIN），以获得悬停模式下的固定翼向前驱动。通过 VT_FWD_THRUST_EN 启用。</translation>
    </message>
    <message>
      <source>Scale factor for Path/Box size. 0.5 would half the distances in maneuvers. Radii are unaffected.</source>
      <translation>路径/框大小的比例因子。 0.5 将使机动距离减半。半径不受影响。</translation>
    </message>
    <message>
      <source>Scale of airspeed sensor 3</source>
      <translation>空速传感器3刻度</translation>
    </message>
    <message>
      <source>Scaling factor between rangefinder reading and distance. For the linear and inverted functions this is in meters per volt. For the hyperbolic function the units are meterVolts.</source>
      <translation>测距仪读数和距离之间的比例因子。对于线性和反函数，单位为米每伏。对于双曲函数，单位是米伏特。</translation>
    </message>
    <message>
      <source>Scan for unknown target</source>
      <translation>扫描未知目标</translation>
    </message>
    <message>
      <source>Screen to be shown on Arm event. Zero to disable the feature.</source>
      <translation>Arm 事件上显示的屏幕。为零则禁用该功能。</translation>
    </message>
    <message>
      <source>Script 14</source>
      <translation>脚本14</translation>
    </message>
    <message>
      <source>Script 7</source>
      <translation>脚本7</translation>
    </message>
    <message>
      <source>Script14</source>
      <translation>脚本14</translation>
    </message>
    <message>
      <source>Script7</source>
      <translation>脚本7</translation>
    </message>
    <message>
      <source>Scripting Serial port number to which the Rockblock is connected for HL2 messages This is the index of the SCR_SDEV ports that are set to 2 for "MavlinkHL"</source>
      <translation>脚本 Rockblock 连接用于 HL2 消息的串行端口号 这是为“MavlinkHL”设置为 2 的 SCR_SDEV 端口的索引</translation>
    </message>
    <message>
      <source>Scripting Virtual Machine Instruction Count</source>
      <translation>编写虚拟机指令计数脚本</translation>
    </message>
    <message>
      <source>Scripting13</source>
      <translation>脚本13</translation>
    </message>
    <message>
      <source>Scripting6</source>
      <translation>脚本6</translation>
    </message>
    <message>
      <source>Search altitude</source>
      <translation>搜索海拔高度</translation>
    </message>
    <message>
      <source>Second Gyro bias (X-axis)</source>
      <translation>第二陀螺仪偏置（X 轴）</translation>
    </message>
    <message>
      <source>Second Proximity sensor yaw correction</source>
      <translation>第二个接近传感器偏航校正</translation>
    </message>
    <message>
      <source>Second TOFSENSE-M sensors backend Instance. Setting this to 2 will pick the second backend from PRX_ or RNG_ Parameters (Depending on TOFSENSE_PRX)</source>
      <translation>第二个 TOFSENSE-M 传感器后端实例。设置为 2 将从 PRX_ 或 RNG_ 参数中选择第二个后端（取决于 TOFSENSE_PRX）</translation>
    </message>
    <message>
      <source>Second driver</source>
      <translation>第二司机</translation>
    </message>
    <message>
      <source>SecondGPS</source>
      <translation>第二GPS</translation>
    </message>
    <message>
      <source>Seconds until maintenance required</source>
      <translation>距离需要维护还有几秒</translation>
    </message>
    <message>
      <source>Select the GPS protocol over serial.
Auto-detection will probe all protocols, and thus is a bit slower.</source>
      <translation>选择 GPS 协议而不是串行协议。自动检测将探测所有协议，因此速度会慢一些。</translation>
    </message>
    <message>
      <source>Select the u-blox configuration setup. Most setups will use the default, including RTK and dual GPS without heading. If rover has RTCM corrections from a static base (or other static correction source) coming in on UART2, then select Mode 5. The Heading mode requires 2 F9P devices to be attached. The main GPS will act as rover and output heading information, whereas the secondary will act as moving base. Modes 1 and 2 require each F9P UART1 to be connected to the Autopilot. In addition, UART2 on the F9P units are connected to each other. Modes 3 and 4 only require UART1 on each F9P connected to the Autopilot or Can Node. UART RX DMA is required. RTK is still possible with this setup. Mode 6 is intended for use with a ground control station (not necessarily an RTK correction base).</source>
      <translation>选择 u-blox 配置。大多数安装方式可使用默认值，包括 RTK 和不提供航向的双 GPS。
如果移动端通过 UART2 接收静态基站或其他静态差分源提供的 RTCM 数据，应选择模式 5。
航向模式需要连接 2 台 F9P。主 GPS 作为移动端输出航向，次 GPS 作为移动基站。
模式 1 和 2 要求每台 F9P 的 UART1 均连接到飞控，并将两台 F9P 的 UART2 相互连接。
模式 3 和 4 只需将每台 F9P 的 UART1 连接至飞控或 CAN 节点，并且需要 UART 接收 DMA；这种接法仍可使用 RTK。
模式 6 用于地面站，地面站不一定是 RTK 差分基站。</translation>
    </message>
    <message>
      <source>Select what should be output on MAIN 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 MAIN 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on MAIN 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 MAIN 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 12. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 12 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 16. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 16 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 5. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 5 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 9. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 9 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Aux 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PWM Aux 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Aux 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PWM Aux 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Roboclaw Driver Channel 1. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Roboclaw 驱动程序通道 1 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 11. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 11 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 15. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 15 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ ESC 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ ESC 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ ESC 7. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ ESC 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Servo 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 舵机 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Servo 7. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 舵机 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Wheels 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 轮子 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on TAP ESC Output ESC 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 TAP ESC 输出 ESC 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on TAP ESC Output ESC 7. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 TAP ESC 输出 ESC 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN ESC 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN ESC 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN ESC 7. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN ESC 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN Servo 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN 舵机 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN Servo 7. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN 舵机 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 11. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 11 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 15. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 15 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL ESC Output ESC 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 VOXL ESC 输出 ESC 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL2 IO Output PWM Channel 3. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 VOXL2 IO 输出 PWM 通道 3 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL2 IO Output PWM Channel 7. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 VOXL2 IO 输出 PWM 通道 7 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 10. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 10 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 14. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 14 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select which Output Protocol to use for outputs MAIN 5-8. Custom PWM rates can be used by directly setting any value &gt;0.</source>
      <translation>选择用于输出 MAIN 5-8 的输出协议。可以通过直接设置&gt;0的任何值来使用自定义PWM速率。</translation>
    </message>
    <message>
      <source>Select which Output Protocol to use for outputs PWM Capture 1-3. Custom PWM rates can be used by directly setting any value &gt;0.</source>
      <translation>选择用于输出 PWM 捕获 1-3 的输出协议。可以通过直接设置&gt;0的任何值来使用自定义PWM速率。</translation>
    </message>
    <message>
      <source>Select which function is performed when CH11 is above 1800 pwm</source>
      <translation>选择当 CH11 高于 1800 pwm 时执行哪个功能</translation>
    </message>
    <message>
      <source>Selector acceleration threshold</source>
      <translation>选择器加速度阈值</translation>
    </message>
    <message>
      <source>Selects stick input selection behavior: either a traditional remote control receiver (RC) or a MAVLink joystick (MANUAL_CONTROL message) Priority sources are immediately switched to whenever they get valid. 0 RC only. Requires valid RC calibration. 1 MAVLink only. RC and related checks are disabled. 2 Switches only if current source becomes invalid. 3 Locks to the first valid source until reboot. 4 Ignores all sources. 5 RC priority, then MAVLink (lower instance before higher) 6 MAVLink priority (lower instance before higher), then RC 7 RC priority, then MAVLink (higher instance before lower) 8 MAVLink priority (higher instance before lower), then RC</source>
      <translation>选择摇杆输入选择行为：传统遥控接收器 (RC) 或 MAVLink 操纵杆（MANUAL_CONTROL 消息） 优先源在有效时立即切换到。仅限 0 RC。需要有效的 RC 校准。仅限 1 MAVLink。 RC 和相关检查被禁用。 2 仅当电流源无效时才进行切换。 3 锁定第一个有效源直至重新启动。 4 忽略所有来源。 5 RC 优先级，然后 MAVLink（实例先低后高） 6 MAVLink 优先级（实例先低后高），然后 RC 7 RC 优先级，然后 MAVLink（实例先高后低） 8 MAVLink 优先级（实例先高后低），然后 RC</translation>
    </message>
    <message>
      <source>Selects the type of rotor speed control used to determine throttle output to the HeliRSC servo channel when motor interlock is enabled (throttle hold off). RC Passthrough sends the input from the RC Motor Interlock channel as throttle output.  External Gov SetPoint sends the RSC SetPoint parameter value as throttle output.  Throttle Curve uses the 5 point throttle curve to determine throttle output based on the collective output.  AutoThrottle requires a rotor speed sensor, contains an advanced autothrottle governor and is primarily for piston and turbine engines. WARNING: Throttle ramp time and throttle curve MUST be tuned properly using Throttle Curve mode before using AutoThrottle</source>
      <translation>选择转子速度控制类型，用于在启用电机互锁（油门延迟关闭）时确定 HeliRSC 舵机通道的油门输出。 RC 直通将来自 RC 电机互锁通道的输入作为油门输出发送。  外部 Gov SetPoint 将 RSC SetPoint 参数值作为油门输出发送。  油门曲线使用5点油门曲线，根据总距输出确定油门输出。  AutoThrottle 需要转子速度传感器，包含先进的自动油门调速器，主要用于活塞和涡轮发动机。 WARNING：使用 AutoThrottle 之前，使用油门曲线模式正确调整油门斜坡时间和油门曲线 MUST</translation>
    </message>
    <message>
      <source>Send all items</source>
      <translation>发送所有项目</translation>
    </message>
    <message>
      <source>SensAItion</source>
      <translation>感觉</translation>
    </message>
    <message>
      <source>Sensor ID for slot 2. Set to 0 to disable this slot.</source>
      <translation>用于插槽 2 的传感器 ID。设置为 0 以禁用此插槽。</translation>
    </message>
    <message>
      <source>Sensor disabled, when explicitly started treated as AUAV L05D</source>
      <translation>传感器禁用，明确启动时视为 AUAV L05D</translation>
    </message>
    <message>
      <source>Sensors Manual Config</source>
      <translation>传感器手动配置</translation>
    </message>
    <message>
      <source>Septentrio-DualAntenna(SBF)</source>
      <translation>Septentrio-双天线(SBF)</translation>
    </message>
    <message>
      <source>Serial 5 Baud Rate</source>
      <translation>串口 5 波特率</translation>
    </message>
    <message>
      <source>Serial 9 Baud Rate</source>
      <translation>串口 9 波特率</translation>
    </message>
    <message>
      <source>Serial Configuration for FT Technologies Digital Wind Sensor (serial)</source>
      <translation>FT Technologies 数字风传感器的串行配置（串行）</translation>
    </message>
    <message>
      <source>Serial Configuration for LeddarOne Rangefinder</source>
      <translation>LeddarOne 测距仪的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for Main GPS</source>
      <translation>主 GPS 的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for UXRCE-DDS Client</source>
      <translation>UXRCE-DDS 客户端的串行配置</translation>
    </message>
    <message>
      <source>Serial Port ID to be used for temporary SLCAN iface, -1 means no temporary serial. This parameter is automatically reset on reboot or on timeout. See CAN_SLCAN_TIMOUT for timeout details</source>
      <translation>用于临时 SLCAN 接口的串口 ID。-1 表示不使用临时串口。此参数将在重启或超时后自动重置；超时设置详见 CAN_SLCAN_TIMOUT。</translation>
    </message>
    <message>
      <source>Serial port number to which the Rockblock is connected.This is the index of the SERIALn_ ports that are set to 28 for "scripting"</source>
      <translation>Rockblock 连接的串行端口号。这是设置为 28 用于“脚本编写”的 SERIALn_ 端口的索引</translation>
    </message>
    <message>
      <source>Serial1</source>
      <translation>串口1</translation>
    </message>
    <message>
      <source>Serial5 options</source>
      <translation>串行5选项</translation>
    </message>
    <message>
      <source>Serial8 protocol selection</source>
      <translation>Serial8协议选择</translation>
    </message>
    <message>
      <source>Service ceiling</source>
      <translation>服务上限</translation>
    </message>
    <message>
      <source>Servo 1 slew rate limit</source>
      <translation>舵机 1 转换速率限制</translation>
    </message>
    <message>
      <source>Servo 17</source>
      <translation>舵机17</translation>
    </message>
    <message>
      <source>Servo 21</source>
      <translation>舵机21</translation>
    </message>
    <message>
      <source>Servo 29</source>
      <translation>舵机29</translation>
    </message>
    <message>
      <source>Servo 4</source>
      <translation>舵机4</translation>
    </message>
    <message>
      <source>Servo 6</source>
      <translation>舵机6</translation>
    </message>
    <message>
      <source>Servo 8</source>
      <translation>舵机8</translation>
    </message>
    <message>
      <source>Servo OFF PWM value</source>
      <translation>舵机 OFF PWM 值</translation>
    </message>
    <message>
      <source>Servo channel reverse rotation bitmask</source>
      <translation>舵机通道反向旋转位掩码</translation>
    </message>
    <message>
      <source>Servo out function</source>
      <translation>舵机输出功能</translation>
    </message>
    <message>
      <source>Servos to Zero PWM</source>
      <translation>舵机归零 PWM</translation>
    </message>
    <message>
      <source>Set NAV TRAFFIC AVOID vertical distance</source>
      <translation>设置 NAV TRAFFIC AVOID 垂直距离</translation>
    </message>
    <message>
      <source>Set bits in the following positions to enable:
0 : Set to true to fuse GPS data if available, also requires GPS for altitude init
1 : Set to true to fuse optical flow data if available
2 : Set to true to fuse vision position
3 : Set to true to enable landing target
4 : Set to true to fuse land detector
5 : Set to true to publish AGL as local position down component
6 : Set to true to enable flow gyro compensation
7 : Set to true to enable baro fusion
default (145 - GPS, baro, land detector)</source>
      <translation>将以下位置中的位设置为启用： 0：设置为 true 以融合 GPS 数据（如果可用），还需要 GPS 进行高度初始化 1：设置为 true 以融合光流数据（如果可用） 2：设置为 true 以融合视觉位置 3：设置为 true 以启用着陆目标 4：设置为 true 以融合着陆探测器 5：设置为 true 以发布 AGL作为本地位置向下组件 6：设置为 true 以启用流量陀螺仪补偿 7：设置为 true 以启用气压融合默认值（145 - GPS、气压、陆地探测器）</translation>
    </message>
    <message>
      <source>Set home position automatically if possible. During missions, the latitude/longitude of the home position is locked and will not reset during intermediate landings. It will only update once the mission is complete or landed outside of a mission. However, the altitude is still being adjusted to correct for GNSS vertical drift in the first 2 minutes after takeoff.</source>
      <translation>如果可能，自动设置起始位置。执行任务时，起始位置的纬度/经度被锁定，中间着陆时不会重置。仅当任务完成或降落在任务之外时才会更新。然而，仍在调整高度以纠正起飞后第一个 2 分钟内的 GNSS 垂直漂移。</translation>
    </message>
    <message>
      <source>Set level at which RSSI item will flash</source>
      <translation>设置 RSSI 项目闪烁的级别</translation>
    </message>
    <message>
      <source>Set servos to neutral after landing</source>
      <translation>着陆后将舵机设置为空档</translation>
    </message>
    <message>
      <source>Set the radius for loitering to a safe altitude for VTOL transition.</source>
      <translation>将徘徊半径设置为 VTOL 过渡的安全高度。</translation>
    </message>
    <message>
      <source>Set this to 1 for skid steering input rovers (tank track style in RC controller). When enabled, servo1 is used for the left track control, servo3 is used for right track control</source>
      <translation>对于滑移转向输入流动站，将此设置为 1（RC 控制器中的坦克轨道样式）。使能时，servo1 用于左磁道控制，servo3 用于右磁道控制</translation>
    </message>
    <message>
      <source>Set to -1 to disable. Note that if it is disabled the rover will not slow down when approaching waypoints in auto modes. For mecanum rovers this limit is used for longitudinal and lateral jerk.</source>
      <translation>设置为 -1 以禁用。请注意，如果禁用该功能，流动站在自动模式下接近航路点时不会减速。对于麦克纳姆漫游车，此限制​​用于纵向和横向加加速度。</translation>
    </message>
    <message>
      <source>Set to 1 to enable S.BUS version 1 output instead of RSSI.</source>
      <translation>设置为 1 以启用 S.BUS 版本 1 输出而不是 RSSI。</translation>
    </message>
    <message>
      <source>Set to true for servo gimbal, false for passthrough.
This is required for a gimbal which is not capable of stabilizing itself
and relies on the IMU's attitude estimation.</source>
      <translation>对于舵机万向节设置为 true，对于直通设置为 false。对于无法自行稳定并依赖于 IMU 姿态估计的云台来说，这是必需的。</translation>
    </message>
    <message>
      <source>Set type of direct attached radio</source>
      <translation>设置直连无线电的类型</translation>
    </message>
    <message>
      <source>Set_RateRollPitch</source>
      <translation>Set_RateRollPitch</translation>
    </message>
    <message>
      <source>Sets a fraction of full flaps during landing. Also applies to flaperons if enabled in the mixer/allocation.</source>
      <translation>在着陆期间设置全襟翼的一小部分。如果在混合器/分配中启用，也适用于襟副翼。</translation>
    </message>
    <message>
      <source>Sets message duration seconds</source>
      <translation>设置消息持续时间秒</translation>
    </message>
    <message>
      <source>Sets the analog input pin that should be used for temprature monitoring. Values for some autopilots are given as examples. Search wiki for "Analog pins".</source>
      <translation>设置应用于温度监控的模拟输入引脚。一些自动驾驶仪的值作为示例给出。在 wiki 中搜索“模拟引脚”。</translation>
    </message>
    <message>
      <source>Sets the maximum distance to a waypoint for the waypoint to be considered complete. This overrides the "cross the finish line" logic that is normally used to consider a waypoint complete. For normal AUTO behaviour this parameter should be set to zero. Using a non-zero value is only recommended when it is critical that the aircraft does approach within the given radius, and should loop around until it has done so. This can cause the aircraft to loop forever if its turn radius is greater than the maximum radius set.</source>
      <translation>设置到航路点的最大距离，以便该航路点被视为完整。这会覆盖通常用于认为航路点已完成的“越过终点线”逻辑。对于正常的 AUTO 行为，该参数应设置为零。仅当飞机必须在给定半径内接近时才建议使用非零值，并且应循环飞行直至完成。如果转弯半径大于设置的最大半径，这可能会导致飞机永远循环。</translation>
    </message>
    <message>
      <source>Sets the minimum type of differential GPS corrections required before allowing to switch into DGPS mode.</source>
      <translation>设置允许切换到 DGPS 模式之前所需的差分 GPS 校正的最小类型。</translation>
    </message>
    <message>
      <source>Sets the overlay text resolution for this screen to either SD 30x16 or HD 50x18/60x22 (MSP DisplayPort only)</source>
      <translation>将此屏幕的覆盖文本分辨率设置为 SD 30x16 或 HD 50x18/60x22（仅限 MSP DisplayPort）</translation>
    </message>
    <message>
      <source>Sets the threshold when the battery will be reported as low.
This has to be higher than the critical threshold.</source>
      <translation>设置报告电池电量低的阈值。这必须高于临界阈值。</translation>
    </message>
    <message>
      <source>Sets the type of simulation used for GPS</source>
      <translation>设置用于 GPS 的模拟类型</translation>
    </message>
    <message>
      <source>Sets vertical offset of the osd inside image</source>
      <translation>设置 osd 内部图像的垂直偏移</translation>
    </message>
    <message>
      <source>Setting this parameter changes CAL_MAG2_ROT to "Custom Euler Angle"</source>
      <translation>设置此参数将 CAL_MAG2_ROT 更改为“自定义欧拉角”</translation>
    </message>
    <message>
      <source>Setting this parameter to 197848 will disable the USB connected checks in the commander, setting it to 0 keeps them enabled (recommended). We are generally recommending to not fly with the USB link connected and production vehicles should set this parameter to zero to prevent users from flying USB powered. However, for R&amp;D purposes it has proven over the years to work just fine.</source>
      <translation>将此参数设置为 197848 将禁用命令器中的 USB 连接检查，将其设置为 0 将使它们保持启用状态（推荐）。我们一般建议不要在连接 USB 链路的情况下飞行，并且量产车辆应将此参数设置为零，以防止用户使用 USB 供电进行飞行。然而，出于研发目的，多年来已经证明它工作得很好。</translation>
    </message>
    <message>
      <source>Setting this to 0 ~ 13 will enable battery current sensing on pins A0 ~ A13. For the 3DR power brick on APM2.5 it should be set to 12. On the PX4 it should be set to 101. On the Pixhawk powered from the PM connector it should be set to 3.</source>
      <translation>设置为 0 ~ 13 将启用引脚 A0 ~ A13 上的电池电流检测。对于 APM2.5 上的 3DR 电源块，应设置为 12。在 PX4 上，应设置为 101。在由 PM 连接器供电的 Pixhawk 上，应将其设置为 3。</translation>
    </message>
    <message>
      <source>Setting this value to true will reinitialize PX4's IQUART connected parameters to the value stored on the currently targeted motor. This is especially useful if your flight controller powered on before your connected modules</source>
      <translation>将此值设置为 true 会将 PX4 的 IQUART 连接参数重新初始化为当前目标电机上存储的值。如果您的飞行控制器在连接的模块之前打开，这尤其有用</translation>
    </message>
    <message>
      <source>SeventhIMU</source>
      <translation>第七IMU</translation>
    </message>
    <message>
      <source>Ship landing pad offset</source>
      <translation>船舶着陆场偏移</translation>
    </message>
    <message>
      <source>ShowOverruns</source>
      <translation>显示超限</translation>
    </message>
    <message>
      <source>Sideslip measurement noise of the internal wind estimator(s) of the airspeed selector.</source>
      <translation>空速选择器内部风估计器的侧滑测量噪声。</translation>
    </message>
    <message>
      <source>Sim AHRS offset yaw</source>
      <translation>Sim AHRS 偏航偏移</translation>
    </message>
    <message>
      <source>SimpleROV-5</source>
      <translation>简单ROV-5</translation>
    </message>
    <message>
      <source>Simulated ESC Telemetry</source>
      <translation>模拟 ESC 遥测</translation>
    </message>
    <message>
      <source>Simulated airspeed sensor failure pressure</source>
      <translation>模拟空速传感器故障压力</translation>
    </message>
    <message>
      <source>Simulated battery internal resistance</source>
      <translation>模拟电池内阻</translation>
    </message>
    <message>
      <source>Simulated failure of ACCEL3</source>
      <translation>ACCEL3的模拟故障</translation>
    </message>
    <message>
      <source>Simulated standard deviation of radius in AIS of a vessel</source>
      <translation>船舶 AIS 中半径的模拟标准偏差</translation>
    </message>
    <message>
      <source>Simulates localized magnetic field distortions at ground level that decays with altitude. (X-axis)</source>
      <translation>模拟地面上随高度衰减的局部磁场扭曲。 （X轴）</translation>
    </message>
    <message>
      <source>Since ESC messages are high priority and sent at a high rate, it is recommended to only enable the interfaces that are actually used. Otherwise, the ESC messages will arbitrate lower priority messages and starve other nodes on the bus.</source>
      <translation>由于ESC消息优先级高且发送速率高，因此建议仅启用实际使用的接口。否则，ESC 消息将仲裁较低优先级的消息并使总线上的其他节点处于饥饿状态。</translation>
    </message>
    <message>
      <source>Six side calibration</source>
      <translation>六面校准</translation>
    </message>
    <message>
      <source>Siyi ZT30 IR</source>
      <translation>四艺 ZT30 IR</translation>
    </message>
    <message>
      <source>Size of window to be used in FFT calculations. Takes effect on reboot. Must be a power of 2 and between 32 and 512. Larger windows give greater frequency resolution but poorer time resolution, consume more CPU time and may not be appropriate for all vehicles. Time and frequency resolution are given by the sample-rate / window-size. Windows of 256 are only really recommended for F7 class boards, windows of 512 or more H7 class.</source>
      <translation>FFT 计算中使用的窗口大小。重启后生效。必须是 2 的幂且介于 32 和 512 之间。较大的窗口提供更高的频率分辨率，但时间分辨率较差，消耗更多的 CPU 时间，并且可能并不适合所有车辆。时间和频率分辨率由采样率/窗口大小给出。 256 的窗口仅真正推荐用于 F7 级板，512 或更高 H7 级的窗口。</translation>
    </message>
    <message>
      <source>Skip RC</source>
      <translation>跳过RC</translation>
    </message>
    <message>
      <source>Skip the arming check for neutral Roll/Pitch/Yay sticks</source>
      <translation>跳过中性横滚/俯仰/耶摇杆的解锁检查</translation>
    </message>
    <message>
      <source>SkyPower EFI log rate. This is the rate at which extra logging of the SkyPower EFI is performed</source>
      <translation>SkyPower EFI 日志速率。这是 SkyPower EFI 执行额外记录的速率</translation>
    </message>
    <message>
      <source>SkyPower EFI telemetry rate. This is the rate at which extra telemetry values are sent to the GCS</source>
      <translation>SkyPower EFI 遥测速率。这是将额外遥测值发送到 GCS 的速率</translation>
    </message>
    <message>
      <source>Slew time of throttle during take-off</source>
      <translation>起飞时油门回转时间</translation>
    </message>
    <message>
      <source>Slung Payload debug output</source>
      <translation>悬挂有效负载调试输出</translation>
    </message>
    <message>
      <source>Slung Payload mavlink system id.  0 to use any/all system ids</source>
      <translation>悬挂有效负载 mavlink 系统 ID。  0 使用任何/所有系统 ID</translation>
    </message>
    <message>
      <source>SmartRTL</source>
      <translation>智能RTL</translation>
    </message>
    <message>
      <source>SmartRTL or RTL</source>
      <translation>SmartRTL 或 RTL</translation>
    </message>
    <message>
      <source>Software Type</source>
      <translation>软件类型</translation>
    </message>
    <message>
      <source>Sonar measurement offset, in meters. Can be used for error injection.</source>
      <translation>声纳测量偏移，以米为单位。可用于错误注入。</translation>
    </message>
    <message>
      <source>Space</source>
      <translation>空间</translation>
    </message>
    <message>
      <source>Specific setting for airspeed when thermalling.</source>
      <translation>热飞行时空速的特定设置。</translation>
    </message>
    <message>
      <source>Specifies aircraft behaviour when a collision may occur</source>
      <translation>指定可能发生碰撞时飞机的行为</translation>
    </message>
    <message>
      <source>Specifies the auto-landing type to use</source>
      <translation>指定要使用的自动着陆类型</translation>
    </message>
    <message>
      <source>Specifies the source for the colours and brightness for the LED.  OutbackChallenge conforms to the MedicalExpress (https://uavchallenge.org/medical-express/) rules, essentially "Green" is disarmed (safe-to-approach), "Red" is armed (not safe-to-approach).</source>
      <translation>指定 LED 的颜色和亮度源。  OutbackChallenge 符合 MedicalExpress（https://uavchallenge.org/medical-express/) 规则，本质上“绿色”为上锁（可安全接近），“红色”为解锁（不可安全接近）。</translation>
    </message>
    <message>
      <source>Specify modes in which datalink loss is ignored and the failsafe action not triggered.</source>
      <translation>指定忽略数据链路丢失且不触发故障安全操作的模式。</translation>
    </message>
    <message>
      <source>Specify the number of magnetic poles of the motors. It is required to compute the RPM value from the eRPM returned with the ESC telemetry. Either get the number from the motor spec sheet or count the magnets on the bell of the motor (not the stator magnets). Typical motors for 5 inch props have 14 poles.</source>
      <translation>指定电机的磁极数。需要根据 ESC 遥测返回的 eRPM 计算 RPM 值。从电机规格表中获取数字或计算电机罩上的磁铁（不是定子磁铁）。 5 英寸螺旋桨的典型电机具有 14 极。</translation>
    </message>
    <message>
      <source>Speed at which the zoom output moves for continuous (rate) zoom commands. Only used by the Servo camera type. The default of 5%/s moves across the full range in 20 seconds</source>
      <translation>对于连续（速率）变焦命令，变焦输出移动的速度。仅由舵机相机类型使用。默认值 5%/s 在 20 秒内移动整个范围</translation>
    </message>
    <message>
      <source>Speed control Error filter frequency in Hz</source>
      <translation>速度控制 误差滤波器频率（Hz）</translation>
    </message>
    <message>
      <source>Speed control P gain</source>
      <translation>速度控制P增益</translation>
    </message>
    <message>
      <source>Speed control acceleration (and deceleration) maximum in m/s/s.  0 to disable acceleration limiting</source>
      <translation>速度控制加速度（和减速度）最大值，以 m/s/s 为单位。  0 禁用加速度限制</translation>
    </message>
    <message>
      <source>Speed control slew rate limit</source>
      <translation>速度控制转换速率限制</translation>
    </message>
    <message>
      <source>Speed measurement threshold</source>
      <translation>测速阈值</translation>
    </message>
    <message>
      <source>Spinner's rotation speed in PWM (a higher rate will disperse the spray over a wider area horizontally)</source>
      <translation>PWM 中旋转器的旋转速度（较高的速度会将喷雾水平分散到更广泛的区域）</translation>
    </message>
    <message>
      <source>Sprayer</source>
      <translation>喷雾器</translation>
    </message>
    <message>
      <source>Spring constant for the tether to simulate elastic forces when stretched beyond its maximum length</source>
      <translation>系绳的弹簧常数，用于模拟拉伸超过其最大长度时的弹力</translation>
    </message>
    <message>
      <source>Stabilize Collective High</source>
      <translation>稳定总距高位</translation>
    </message>
    <message>
      <source>Stabilize mount's pitch/tilt angle</source>
      <translation>稳定安装的俯仰/倾斜角度</translation>
    </message>
    <message>
      <source>Stall airspeed</source>
      <translation>失速空速</translation>
    </message>
    <message>
      <source>Standard deviation of noise in process for strength</source>
      <translation>强度过程中噪声的标准偏差</translation>
    </message>
    <message>
      <source>Start motors before throwing is detected</source>
      <translation>在检测到投掷之前启动电机</translation>
    </message>
    <message>
      <source>Start-up in ESC Calibration mode regardless of throttle</source>
      <translation>无论油门如何，都以 ESC 校准模式启动</translation>
    </message>
    <message>
      <source>Statistics Reset Time</source>
      <translation>统计重置时间</translation>
    </message>
    <message>
      <source>Steering control D gain.  Compensates for short-term change in desired turn rate vs actual</source>
      <translation>转向控制 D 增益。  补偿所需转弯率与实际转弯率的短期变化</translation>
    </message>
    <message>
      <source>Steering control I gain maximum.  Constraings the steering output (range -1 to +1) that the I term will generate</source>
      <translation>转向控制我获得最大。  限制 I 项将生成的转向输出（范围 -1 至 +1）</translation>
    </message>
    <message>
      <source>Steering control angle P gain</source>
      <translation>转向控制角P增益</translation>
    </message>
    <message>
      <source>Steering control input filter.  Lower values reduce noise but add delay.</source>
      <translation>转向控制输入滤波器。  较低的值会减少噪音，但会增加延迟。</translation>
    </message>
    <message>
      <source>Steering turning gain</source>
      <translation>转向增益</translation>
    </message>
    <message>
      <source>Stick roll right</source>
      <translation>向右横滚</translation>
    </message>
    <message>
      <source>Stop-Restart Scripting</source>
      <translation>停止-重新启动脚本</translation>
    </message>
    <message>
      <source>Stream rate of AHRS, HWSTATUS, SYSTEM_TIME, RANGEFINDER, DISTANCE_SENSOR, TERRAIN_REQUEST, BATTERY2, MOUNT_STATUS, OPTICAL_FLOW, GIMBAL_REPORT, MAG_CAL_REPORT, MAG_CAL_PROGRESS, EKF_STATUS_REPORT, VIBRATION and RPM to ground station</source>
      <translation>AHRS、HWSTATUS、SYSTEM_TIME、RANGEFINDER、DISTANCE_SENSOR、TERRAIN_REQUEST、BATTERY2、MOUNT_STATUS、OPTICAL_FLOW、GIMBAL_REPORT、MAG_CAL_REPORT、MAG_CAL_PROGRESS、EKF_STATUS_REPORT、VIBRATION、RPM到地面站的码流速率</translation>
    </message>
    <message>
      <source>Stream rate of RAW_IMU, SCALED_IMU2, SCALED_IMU3, SCALED_PRESSURE, SCALED_PRESSURE2, SCALED_PRESSURE3 and SENSOR_OFFSETS to ground station</source>
      <translation>RAW_IMU、SCALED_IMU2、SCALED_IMU3、SCALED_PRESSURE、SCALED_PRESSURE2、SCALED_PRESSURE3、SENSOR_OFFSETS 到地面站的码流速率</translation>
    </message>
    <message>
      <source>Stream rate of VFR_HUD to ground station</source>
      <translation>VFR_HUD到地面站的码流速率</translation>
    </message>
    <message>
      <source>Sum monitor measures minimum voltage instead of average</source>
      <translation>总和监视器测量最小电压而不是平均值</translation>
    </message>
    <message>
      <source>Supply Voltage Mode means that the maximum voltage applied to pulsing is the supplied voltage. Voltage Limit Mode
indicates that PULSE_VOLT_LIM is the maximum allowed voltage to apply towards pulsing.</source>
      <translation>供电电压模式意味着施加到脉冲的最大电压是供电电压。电压限制模式表示 PULSE_VOLT_LIM 是施加于脉冲的最大允许电压。</translation>
    </message>
    <message>
      <source>SurfTrak</source>
      <translation>冲浪追踪</translation>
    </message>
    <message>
      <source>Surface mode throttle output when no barometer is available</source>
      <translation>无气压计时的表面模式油门输出</translation>
    </message>
    <message>
      <source>SwapTXRX</source>
      <translation>交换TXRX</translation>
    </message>
    <message>
      <source>Swash 1 Phase Angle Compensation</source>
      <translation>斜盘 1 相位角补偿</translation>
    </message>
    <message>
      <source>Swash 2 Maximum Collective Pitch</source>
      <translation>斜盘 2 最大总距桨距</translation>
    </message>
    <message>
      <source>SwiftNav</source>
      <translation>迅捷导航</translation>
    </message>
    <message>
      <source>SwitchPos4</source>
      <translation>开关Pos4</translation>
    </message>
    <message>
      <source>System ID Chirp</source>
      <translation>系统 ID 线性调频</translation>
    </message>
    <message>
      <source>System identification Start Frequency</source>
      <translation>系统识别起始频率</translation>
    </message>
    <message>
      <source>TAP ESC Output ESC 1</source>
      <translation>TAP ESC 输出 ESC 1</translation>
    </message>
    <message>
      <source>TAP ESC Output ESC 5</source>
      <translation>TAP ESC 输出 ESC 5</translation>
    </message>
    <message>
      <source>TCP client</source>
      <translation>TCP客户端</translation>
    </message>
    <message>
      <source>TE MS5525DSO differential pressure sensor (external I2C)</source>
      <translation>TE MS5525DSO 差压传感器（外接I2C）</translation>
    </message>
    <message>
      <source>TEMP_EN</source>
      <translation>TEMP_EN</translation>
    </message>
    <message>
      <source>TEST_3</source>
      <translation>TEST_3</translation>
    </message>
    <message>
      <source>TEST_KDE_CAN</source>
      <translation>TEST_KDE_CAN</translation>
    </message>
    <message>
      <source>TEST_PARAMS</source>
      <translation>TEST_PARAMS</translation>
    </message>
    <message>
      <source>TF02 Pro Distance Sensor (i2c)</source>
      <translation>TF02 Pro 距离传感器 (i2c)</translation>
    </message>
    <message>
      <source>THR_MIN PWM when disarmed</source>
      <translation>上锁时 THR_MIN PWM</translation>
    </message>
    <message>
      <source>TOFSENSE-M Thir ID</source>
      <translation>TOFSENSE-M 第三代 ID</translation>
    </message>
    <message>
      <source>TRAINING</source>
      <translation>TRAINING</translation>
    </message>
    <message>
      <source>TURTLE Mode</source>
      <translation>TURTLE模式</translation>
    </message>
    <message>
      <source>Tail Type</source>
      <translation>尾型</translation>
    </message>
    <message>
      <source>Tailsitter VTOL</source>
      <translation>尾座VTOL</translation>
    </message>
    <message>
      <source>Tailsitter forward flight to VTOL transition rate</source>
      <translation>尾座向前飞行至 VTOL 的过渡率</translation>
    </message>
    <message>
      <source>Tailsitter transition angle</source>
      <translation>尾座过渡角度</translation>
    </message>
    <message>
      <source>Takeoff Check RPM minimum</source>
      <translation>起飞检查 RPM 最小值</translation>
    </message>
    <message>
      <source>Takeoff maximum throttle during take-off ramp up</source>
      <translation>起飞加速期间的起飞最大油门</translation>
    </message>
    <message>
      <source>Takeoff pitch limit reduction</source>
      <translation>起飞俯仰极限减小</translation>
    </message>
    <message>
      <source>Takeoff throttle delay</source>
      <translation>起飞油门延迟</translation>
    </message>
    <message>
      <source>Target Glide Body Frame Forward Speed</source>
      <translation>目标滑翔身体框架前进速度</translation>
    </message>
    <message>
      <source>Target altitude above home for RTL mode. Maintains current altitude if set to -1. Rally point altitudes are used if plane does not return to home.</source>
      <translation>RTL 模式的目标高度。如果设置为 -1，则保持当前高度。如果飞机没有返航，则使用集结点高度。</translation>
    </message>
    <message>
      <source>Target percentage of throttle to apply for flight in automatic throttle modes and throttle percentage that maintains AIRSPEED_CRUISE. Caution: low battery voltages at the end of flights may require higher throttle to maintain airspeed.</source>
      <translation>自动油门模式下飞行时申请的目标油门百分比以及维持 AIRSPEED_CRUISE 的油门百分比。注意：飞行结束时电池电压低可能需要更高的油门以维持空速。</translation>
    </message>
    <message>
      <source>Telem1 options</source>
      <translation>Telem1 选项</translation>
    </message>
    <message>
      <source>Telemetry 2 protocol selection</source>
      <translation>遥测2协议选择</translation>
    </message>
    <message>
      <source>Temperature calibration enable</source>
      <translation>温度校准使能</translation>
    </message>
    <message>
      <source>Temperature sensor address</source>
      <translation>温度传感器地址</translation>
    </message>
    <message>
      <source>Temperature sensor analog voltage sensing pin</source>
      <translation>温度传感器模拟电压传感引脚</translation>
    </message>
    <message>
      <source>Temperature that the 3rd gyroscope was calibrated at</source>
      <translation>第三陀螺仪校准温度</translation>
    </message>
    <message>
      <source>Terminate</source>
      <translation>终止</translation>
    </message>
    <message>
      <source>Terrain Gradient % RMS</source>
      <translation>地形梯度% RMS</translation>
    </message>
    <message>
      <source>Terrain grid spacing</source>
      <translation>地形网格间距</translation>
    </message>
    <message>
      <source>Test autotuned gains after tune is complete</source>
      <translation>调谐完成后测试自动调谐增益</translation>
    </message>
    <message>
      <source>TestChan7</source>
      <translation>测试Chan7</translation>
    </message>
    <message>
      <source>TestMotor7</source>
      <translation>测试电机7</translation>
    </message>
    <message>
      <source>Tether Wire Density</source>
      <translation>系绳密度</translation>
    </message>
    <message>
      <source>The FC and the ESC must agree upon the meaning of the signal coming out of the ESC. When FCs are in 3D mode
they re-map negative signals. This parameter keeps the FC and ESC in agreement.</source>
      <translation>FC 和 ESC 必须就 ESC 发出的信号的含义达成一致。当 FC 处于 3D 模式时，它们会重新映射负信号。该参数使 FC 和 ESC 保持一致。</translation>
    </message>
    <message>
      <source>The IQUART driver's baud rate</source>
      <translation>IQUART驱动器的波特率</translation>
    </message>
    <message>
      <source>The PWM level in microseconds on channel 3 below which throttle failsafe triggers</source>
      <translation>通道 3 上的 PWM 级别（以微秒为单位），低于该级别时油门故障安全触发</translation>
    </message>
    <message>
      <source>The RC auxilliary function number for start/stop of the generator. Zero to disable start function</source>
      <translation>RC 发电机启停辅助功能号。归零以禁用启动功能</translation>
    </message>
    <message>
      <source>The action to take on a long (FS_LONG_TIMEOUT seconds) failsafe event. If the aircraft was in a stabilization or manual mode when failsafe started and a long failsafe occurs then it will change to RTL mode if FS_LONG_ACTN is 0 or 1, and will change to FBWA if FS_LONG_ACTN is set to 2. If the aircraft was in an auto mode (such as AUTO or GUIDED) when the failsafe started then it will continue in the auto mode if FS_LONG_ACTN is set to 0, will change to RTL mode if FS_LONG_ACTN is set to 1 and will change to FBWA mode if FS_LONG_ACTN is set to 2. If FS_LONG_ACTN is set to 3, the parachute will be deployed (make sure the chute is configured and enabled). If FS_LONG_ACTN is set to 4 the aircraft will switch to mode AUTO with the current waypoint if it is not already in mode AUTO, unless it is in the middle of a landing sequence. This parameter only applies to failsafes during fixed wing modes. Quadplane modes will switch to QLAND unless Q_OPTIONS bit 5 (QRTL) or 20(RTL) are set.</source>
      <translation>对长（FS_LONG_TIMEOUT 秒）故障安全事件采取的操作。如果飞机在失控启动时处于增稳或手动模式，且发生长时失控，则当 FS_LONG_ACTN 设置为 0 或 1 时，将切换至 RTL 模式；当 FS_LONG_ACTN 设置为 2 时，将切换至 FBWA 模式。如果故障保护启动时飞行器处于自动模式（例如 AUTO 或 GUIDED），则如果 FS_LONG_ACTN 设置为 0，则飞行器将继续处于自动模式；如果 FS_LONG_ACTN 设置为 1，则将切换到 RTL 模式；如果 FS_LONG_ACTN 设置为 2，则将切换为 FBWA 模式。如果FS_LONG_ACTN设置为3，降落伞将展开（确保降落伞已配置并启用）。如果 FS_LONG_ACTN 设置为 4，如果飞机尚未处于 AUTO 模式，则飞机将以当前航路点切换到 AUTO 模式，除非飞机处于着陆序列的中间。该参数仅适用于固定翼模式下的故障保护。除非设置了 Q_OPTIONS 位 5 (QRTL) 或 20(RTL)，否则四平面模式将切换到 QLAND。</translation>
    </message>
    <message>
      <source>The aircraft's wing span (length from tip to tip)</source>
      <translation>飞机的翼展（翼尖到翼尖的长度）</translation>
    </message>
    <message>
      <source>The altitude which QRTL mode heads to initially</source>
      <translation>QRTL模式最初到达的高度</translation>
    </message>
    <message>
      <source>The base throttle percentage to use in auto mode. The CRUISE_SPEED parameter controls the target speed, but the rover starts with the CRUISE_THROTTLE setting as the initial estimate for how much throttle is needed to achieve that speed. It then adjusts the throttle based on how fast the rover is actually going.</source>
      <translation>自动模式下使用的基本油门百分比。 CRUISE_SPEED 参数控制目标速度，但流动站以 CRUISE_THROTTLE 设置开始，作为达到该速度所需油门的初始估计。然后它根据流动站的实际行驶速度调整油门。</translation>
    </message>
    <message>
      <source>The baud rate used for Serial6. Most stm32-based boards can support rates of up to 1500. If you setup a rate you cannot support and then can't connect to your board you should load a firmware from a different vehicle type. That will reset all your parameters to defaults.</source>
      <translation>Serial6 使用的波特率。大多数基于 stm32 的板卡可支持高达 1500 的速率。如果您设置了无法支持的速率，然后无法连接到您的主板，您应该从不同的车辆类型加载固件。这会将您的所有参数重置为默认值。</translation>
    </message>
    <message>
      <source>The baud rate used on the USB console. The APM2 can support all baudrates up to 115, and also can support 500. The PX4 can support rates of up to 1500. If you setup a rate you cannot support on APM2 and then can't connect to your board you should load a firmware from a different vehicle type. That will reset all your parameters to defaults.</source>
      <translation>USB 控制台上使用的波特率。 APM2 可以支持高达 115 的所有波特率，也可以支持 500。 PX4 可支持高达 1500 的速率。如果您设置了 APM2 不支持的速率，然后无法连接到您的主板，您应该从不同的车辆类型加载固件。这会将您的所有参数重置为默认值。</translation>
    </message>
    <message>
      <source>The calibrated airspeed setpoint during landing. If set &lt;= 0, landing airspeed = FW_AIRSPD_MIN by default.</source>
      <translation>着陆期间校准的空速设定值。如果设置 &lt;= 0，默认着陆空速 = FW_AIRSPD_MIN。</translation>
    </message>
    <message>
      <source>The center frequency for the 2nd order notch filter on the primary gyro.
This filter can be enabled to avoid feedback amplification of structural resonances at a specific frequency.
This only affects the signal sent to the controllers, not the estimators.
Applies to both angular velocity and angular acceleration sent to the controllers.
See "IMU_GYRO_NF1_BW" to set the bandwidth of the filter.
A value of 0 disables the filter.</source>
      <translation>主陀螺仪上二阶陷波滤波器的中心频率。可以启用该滤波器以避免特定频率下结构共振的反馈放大。这仅影响发送到控制器的信号，而不影响估计器。适用于发送到控制器的角速度和角加速度。请参阅“IMU_GYRO_NF1_BW”来设置滤波器的带宽。 0 的值会禁用滤波器。</translation>
    </message>
    <message>
      <source>The channel index (starting from 1 for channel 1) indicates
which channel should be used for reading yaw inputs from.
A value of zero indicates the switch is not assigned.</source>
      <translation>通道索引（通道 1 从 1 开始）指示应使用哪个通道读取偏航输入。值为零表示未分配开关。</translation>
    </message>
    <message>
      <source>The check fails if the RAM usage is above this threshold.
A negative value disables the check.</source>
      <translation>如果 RAM 使用率高于此阈值，则检查失败。负值会禁用检查。</translation>
    </message>
    <message>
      <source>The cutoff frequency for the 2nd order butterworth filter on the primary accelerometer. This only affects the signal sent to the controllers, not the estimators. 0 disables the filter.</source>
      <translation>主加速度计上的二阶巴特沃斯滤波器的截止频率。这仅影响发送到控制器的信号，而不影响估计器。 0 禁用滤波器。</translation>
    </message>
    <message>
      <source>The default value of 1000 requires the stick to be held in the arm or disarm position for 1 second.</source>
      <translation>1000 的默认值要求摇杆保持在解锁或上锁位置 1 秒。</translation>
    </message>
    <message>
      <source>The desired gain to convert roll sp into yaw rate sp.</source>
      <translation>将横滚 sp 转换为偏航率 sp 所需的增益。</translation>
    </message>
    <message>
      <source>The driver will output standard pulse-width encoded signal without this bit set. To make PCA9685 output in duty-cycle fashion, please enable the corresponding channel bit here and adjusting standard params to suit your need. The driver will have 12bits resolution for duty-cycle output. That means to achieve 0% to 100% output range on one channel, the corresponding params MIN and MAX for the channel should be set to 0 and 4096. Other standard params follows the same rule.</source>
      <translation>如果没有设置该位，驱动器将输出标准脉冲宽度编码信号。要使 PCA9685 以占空比方式输出，请在此处启用相应的通道位并调整标准参数以满足您的需要。该驱动器的占空比输出分辨率为 12 位。也就是说，要在一个通道上实现 0% 到 100% 的输出范围，该通道对应的参数 MIN 和 MAX 应设置为 0 和 4096。其他标准参数遵循相同的规则。</translation>
    </message>
    <message>
      <source>The expected value of COMPASS_DEV_ID, used by arming checks. Setting this to -1 means "don't care."</source>
      <translation>COMPASS_DEV_ID 的预期值，用于解锁检查。将其设置为 -1 意味着“不关心”。</translation>
    </message>
    <message>
      <source>The frequency width of the stop band for the 2nd order notch filter on the primary gyro. See "IMU_GYRO_NF0_FRQ" to activate the filter and to set the notch frequency. Applies to both angular velocity and angular acceleration sent to the controllers.</source>
      <translation>主陀螺仪上二阶陷波滤波器的阻带频率宽度。请参阅“IMU_GYRO_NF0_FRQ”来激活滤波器并设置陷波频率。适用于发送到控制器的角速度和角加速度。</translation>
    </message>
    <message>
      <source>The height above ground below which ground effect creates barometric altitude errors. A negative value indicates no ground effect.</source>
      <translation>距地面的高度，低于地面效应会产生气压高度误差。负值表示没有地面效应。</translation>
    </message>
    <message>
      <source>The idle throttle to hold after arming and before a takeoff. Applicable in TAKEOFF and AUTO modes.</source>
      <translation>解锁后和起飞前保持怠速油门。适用于TAKEOFF和AUTO模式。</translation>
    </message>
    <message>
      <source>The lookahead for throttle control</source>
      <translation>油门控制的前瞻</translation>
    </message>
    <message>
      <source>The maximal airspeed (calibrated airspeed) the user is able to command.</source>
      <translation>用户能够命令的最大空速（校准空速）。</translation>
    </message>
    <message>
      <source>The maximum horizontal velocity the pilot may request in cm/s</source>
      <translation>飞行员可能请求的最大水平速度（以厘米/秒为单位）</translation>
    </message>
    <message>
      <source>The maximum pitch rate at full stick deflection in ACRO mode</source>
      <translation>ACRO 模式下全摇杆偏转时的最大俯仰速率</translation>
    </message>
    <message>
      <source>The maximum throttle setting during automatic takeoff. If this is zero then THR_MAX is used for takeoff as well.</source>
      <translation>自动起飞时的最大油门设置。如果为零，则 THR_MAX 也用于起飞。</translation>
    </message>
    <message>
      <source>The maximum vertical descending velocity the pilot may request in cm/s.  If 0 PILOT_SPEED_UP value is used.</source>
      <translation>飞行员可能请求的最大垂直下降速度（以厘米/秒为单位）。  如果使用 0 PILOT_SPEED_UP 值。</translation>
    </message>
    <message>
      <source>The maximum/minimum value that the sum of the P and D term can output</source>
      <translation>P和D项之和可以输出的最大/最小值</translation>
    </message>
    <message>
      <source>The minimum alt above home the vehicle will climb to before returning.  If the vehicle is flying higher than this value it will return at its current altitude.</source>
      <translation>车辆返回前将爬升到的最低海拔高度。  如果飞行器飞行高度高于此值，它将返回当前高度。</translation>
    </message>
    <message>
      <source>The minimum threshold for the yaw rate measurement not to be interpreted as zero.</source>
      <translation>偏航率测量的最小阈值不应被解释为零。</translation>
    </message>
    <message>
      <source>The moment coefficient if defined as Torque = KM * Thrust. Use a positive value for a rotor with CCW rotation. Use a negative value for a rotor with CW rotation.</source>
      <translation>力矩系数定义为扭矩 = KM * 推力。对于 CCW 旋转的转子使用正值。对于 CW 旋转的转子使用负值。</translation>
    </message>
    <message>
      <source>The offboard loss failsafe will only be entered after a timeout, set by COM_OF_LOSS_T in seconds.</source>
      <translation>只有在超时（由 COM_OF_LOSS_T 以秒为单位设置）后才会进入板外丢失故障保护。</translation>
    </message>
    <message>
      <source>The orientation of the compass relative to the autopilot board. This will default to the right value for each board type, but can be changed if you have an external compass. See the documentation for your external compass for the right value. The correct orientation should give the X axis forward, the Y axis to the right and the Z axis down. So if your aircraft is pointing west it should show a positive value for the Y axis, and a value close to zero for the X axis. On a PX4 or Pixhawk with an external compass the correct value is zero if the compass is correctly oriented. NOTE: This orientation is combined with any AHRS_ORIENTATION setting.</source>
      <translation>罗盘相对于自动驾驶仪板的方向。对于每种板类型，这将默认为正确的值，但如果您有外部罗盘，则可以更改。请参阅外部罗盘的文档以了解正确的值。正确的方向应该是 X 轴向前，Y 轴向右，Z 轴向下。因此，如果您的飞机指向西，则 Y 轴应显示正值，X 轴应显示接近零的值。在带有外部罗盘的 PX4 或 Pixhawk 上，如果罗盘方向正确，则正确值为零。 NOTE：该方向与任何 AHRS_ORIENTATION 设置相结合。</translation>
    </message>
    <message>
      <source>The orientation of the device (Radians) with respect to the vehicle frame around the y axis. Requires MS_SVT_EN to be enabled to be used.</source>
      <translation>设备相对于车架绕 y 轴的方向（弧度）。需要启用 MS_SVT_EN 才能使用。</translation>
    </message>
    <message>
      <source>The orientation of the first external compass relative to the vehicle frame. This value will be ignored unless this compass is set as an external compass. When set correctly in the northern hemisphere, pointing the nose and right side down should increase the MagX and MagY values respectively. Rolling the vehicle upside down should decrease the MagZ value. For southern hemisphere, switch increase and decrease. NOTE: For internal compasses, AHRS_ORIENT is used. The label for each option is specified in the order of rotations for that orientation.</source>
      <translation>第一外部罗盘相对于车架的方向。除非将此罗盘设置为外部罗盘，否则该值将被忽略。在北半球正确设置时，将鼻子和右侧朝下应分别增加 MagX 和 MagY 值。将车辆倒置会降低 MagZ 值。对于南半球，切换增加和减少。 NOTE：对于内部罗盘，使用 AHRS_ORIENT。每个选项的标签按照该方向的旋转顺序指定。</translation>
    </message>
    <message>
      <source>The physical COM port on the connected device, currently only applies to SBF GPS</source>
      <translation>连接设备上的物理COM端口，目前仅适用于SBF GPS</translation>
    </message>
    <message>
      <source>The pin number that the Sprayer spinner servo is connected to. (start at 1)</source>
      <translation>喷涂机旋转器舵机系统连接的引脚号。 （从1开始）</translation>
    </message>
    <message>
      <source>The preflight check will fail if fewer than this number of distance sensors with valid data is present. Disable the check with 0.</source>
      <translation>如果具有有效数据的距离传感器数量少于此数量，则预检检查将会失败。禁用 0 的检查。</translation>
    </message>
    <message>
      <source>The rate at which Mavlink updates position and baro data</source>
      <translation>Mavlink 更新位置和气压数据的速率</translation>
    </message>
    <message>
      <source>The return altitude will be calculated based on RTL_CONE_ANG parameter.
The yaw setpoint will switch to the one defined by corresponding waypoint.</source>
      <translation>返回高度将根据RTL_CONE_ANG参数计算。偏航设定点将切换到相应航路点定义的设定点。</translation>
    </message>
    <message>
      <source>The sink rate max for the landing approach stage of landing. This will need to be large for steep landing approaches especially when using reverse thrust. If 0, then use TECS_SINK_MAX.</source>
      <translation>着陆进场阶段的最大下沉率。对于陡峭着陆进场，该值需要很大，尤其是在使用反向推力时。如果是 0，则使用 TECS_SINK_MAX。</translation>
    </message>
    <message>
      <source>The state of the relay on boot. </source>
      <translation>启动时继电器的状态。 </translation>
    </message>
    <message>
      <source>The target head speed in RPM during autorotation. Start by setting to desired hover speed and tune from there as necessary.</source>
      <translation>自动旋转期间 RPM 中的目标磁头速度。首先设置所需的悬停速度，然后根据需要进行调整。</translation>
    </message>
    <message>
      <source>The thrust coefficient if defined as Thrust = CT * u^2, where u (with value between actuator minimum and maximum) is the output signal sent to the motor controller.</source>
      <translation>推力系数定义为 Thrust = CT * u^2，其中 u（执行器最小值和最大值之间的值）是发送到电机控制器的输出信号。</translation>
    </message>
    <message>
      <source>The time in seconds without a new setpoint from RC or Joystick, after which the connection is considered lost.
This must be kept short as the vehicle will use the last supplied setpoint until the timeout triggers.
Ensure the value is not set lower than the update interval of the RC or Joystick.</source>
      <translation>RC 或操纵杆没有新设定点的时间（以秒为单位），之后连接被视为丢失。该时间必须保持较短，因为车辆将使用最后提供的设定点，直到超时触发。确保该值设置不低于 RC 或操纵杆的更新间隔。</translation>
    </message>
    <message>
      <source>The triggered behavior sent to the motors on PX4 disarm</source>
      <translation>在 PX4 上锁时发送到电机的触发行为</translation>
    </message>
    <message>
      <source>The use of this parameter is no longer recommended and has been superseded by a method that works in all VTOL modes with the exception of QAUTOTUNE which is controlled by the Q_FWD_THR_USE parameter. This Q_VFD_GAIN parameter controls use of the forward motor in VTOL modes that use the velocity controller. Set to 0 to disable this function. A value of 0.05 is a good place to start if you want to use the forward motor for position control. No forward motor will be used in QSTABILIZE or QHOVER modes. Use with QLOITER for position hold with the forward motor.</source>
      <translation>已不再建议使用此参数，新的控制方法适用于除 QAUTOTUNE 外的所有垂直起降模式；QAUTOTUNE 由 Q_FWD_THR_USE 控制。Q_VFD_GAIN 控制使用速度控制器的垂直起降模式中，前向推进电机的使用方式。设为 0 可禁用；若希望使用前向推进电机控制位置，可从 0.05 开始调试。QSTABILIZE 和 QHOVER 模式不使用前向推进电机；可配合 QLOITER 使用前向推进电机保持位置。</translation>
    </message>
    <message>
      <source>The vehicle will attempt achieve this pitch angle during the pull-up maneouvre.</source>
      <translation>车辆将尝试在上拉动作期间达到该俯仰角。</translation>
    </message>
    <message>
      <source>The voltage seen on the analog pin when the fuel tank is empty. Note: For this type of battery monitor, the voltage seen by the analog pin is displayed as battery voltage on a GCS.</source>
      <translation>当油箱空时模拟引脚上看到的电压。注：对于这种类型的电池监视器，模拟引脚看到的电压在 GCS 上显示为电池电压。</translation>
    </message>
    <message>
      <source>Thermal compensation for barometric pressure sensors</source>
      <translation>气压传感器的热补偿</translation>
    </message>
    <message>
      <source>Thermalling bank angle</source>
      <translation>热熔坡度角</translation>
    </message>
    <message>
      <source>Third Gyro bias on Z axis</source>
      <translation>Z 轴上的第三个陀螺仪偏置</translation>
    </message>
    <message>
      <source>Third button Pin</source>
      <translation>第三个按钮别针</translation>
    </message>
    <message>
      <source>ThirdIMU</source>
      <translation>第三IMU</translation>
    </message>
    <message>
      <source>This allows for the IO co-processor on boards with an IOMCU to be disabled. Setting to 2 will enable the IOMCU but not attempt to update firmware on startup</source>
      <translation>这允许禁用具有 IOMCU 的板上的 IO 协处理器。设置为 2 将启用 IOMCU，但不会在启动时尝试更新固件</translation>
    </message>
    <message>
      <source>This allows the enabling of special features in the speed/height controller.</source>
      <translation>这允许启用速度/高度控制器中的特殊功能。</translation>
    </message>
    <message>
      <source>This bitmask allows to specify multiple channels for changing flight modes using momentary buttons. Each channel is assigned to a mode slot ((lowest channel = slot 1). The resulting modes for each slot X is defined by the COM_FLTMODEX parameters. The functionality can be used only if RC_MAP_FLTMODE is disabled. The maximum number of available slots and hence bits set in the mask is 6.</source>
      <translation>该位掩码允许使用瞬时按钮指定多个通道来更改飞行模式。每个通道都分配给一个模式槽（（最低通道 = 槽 1）。每个槽 X 的结果模式由 COM_FLTMODEX 参数定义。仅当禁用 RC_MAP_FLTMODE 时才能使用该功能。可用槽的最大数量以及掩码中设置的位为 6。</translation>
    </message>
    <message>
      <source>This channel triggers irreversible flight termination: All outputs are disabled and set to their failsafe values (disarmed by default) and MAVLink parachutes are triggered. Unlike a kill switch, this cannot be undone until system reboot. Use with caution.</source>
      <translation>该通道触发不可逆的飞行终止：所有输出均被禁用并设置为其故障安全值（默认情况下处于解除状态），并触发 MAVLink 降落伞。与终止开关不同，此操作在系统重新启动之前无法撤消。谨慎使用。</translation>
    </message>
    <message>
      <source>This check detects if there are hardfault / watchdog files present on the SD card. If so, and the parameter is enabled, arming is prevented.</source>
      <translation>此检查检测 SD 卡上是否存在硬故障/看门狗文件。如果是这样，并且该参数已启用，则会阻止解锁。</translation>
    </message>
    <message>
      <source>This configures a check to verify the expected number of 5V rail power supplies are present. By default only one is expected. Note: CBRK_SUPPLY_CHK disables all power checks including this one.</source>
      <translation>这将配置一项检查以验证是否存在预期数量的 5V 导轨电源。默认情况下只需要一个。注意：CBRK_SUPPLY_CHK 禁用包括此项在内的所有电源检查。</translation>
    </message>
    <message>
      <source>This controls how fast the accelerometers correct the attitude</source>
      <translation>这控制加速度计纠正姿态的速度</translation>
    </message>
    <message>
      <source>This controls how rapidly the tracker will change the servo output for yaw. It is set as the number of seconds to do a full rotation. You can use this parameter to slow the trackers movements, which may help with some types of trackers. A value of zero will allow for unlimited servo movement per update.</source>
      <translation>这控制跟踪器改变偏航舵机输出的速度。它被设置为完成一次完整旋转的秒数。您可以使用此参数来减慢跟踪器的移动速度，这可能对某些类型的跟踪器有所帮助。值为零将允许每次更新无限的舵机运动。</translation>
    </message>
    <message>
      <source>This controls optional AHRS behaviour. Setting DisableDCMFallbackFW will change the AHRS behaviour for fixed wing aircraft in fly-forward flight to not fall back to DCM when the EKF stops navigating. Setting DisableDCMFallbackVTOL will change the AHRS behaviour for fixed wing aircraft in non fly-forward (VTOL) flight to not fall back to DCM when the EKF stops navigating. Setting DontDisableAirspeedUsingEKF disables the EKF based innovation check for airspeed consistency. Setting AutoRecordOrigin will auto-save the EKF origin to parameters when it becomes valid.</source>
      <translation>这控制可选的 AHRS 行为。设置DisableDCMFallbackFW 将更改固定翼飞机在前飞飞行中的AHRS 行为，以便在EKF 停止导航时不会后退至DCM。设置DisableDCMFallbackVTOL 将更改固定翼飞机在非前飞(VTOL) 飞行中的AHRS 行为，以便在EKF 停止导航时不会后退至DCM。设置 DontDisableAirspeedUsingEKF 将禁用基于 EKF 的空速一致性创新检查。设置 AutoRecordOrigin 生效后，将自动将 EKF 原点保存到参数中。</translation>
    </message>
    <message>
      <source>This controls the amount of vectored thrust control used in hover for a vectored tailsitter</source>
      <translation>这控制矢量尾翼悬停时使用的矢量推力控制量</translation>
    </message>
    <message>
      <source>This controls the maximum current the INA239 sensor will work with.</source>
      <translation>这控制 INA239 传感器工作的最大电流。</translation>
    </message>
    <message>
      <source>This controls the minimum altitude change for a waypoint before an altitude slope will be used instead of an immediate altitude change. The default value is 15 meters, which helps to smooth out waypoint missions where small altitude changes happen near waypoints. If you don't want altitude slopes to be used in missions then you can set this to zero, which will disable altitude slope calculations. Otherwise you can set it to a minimum number of meters of altitude error to the destination waypoint before an altitude slope will be used to change altitude. This also gates the gradual altitude ramp used during the approach leg of QRTL.</source>
      <translation>这控制在使用高度斜率而不是立即高度变化之前航路点的最小高度变化。默认值为 15 米，这有助于平滑航路点附近发生微小高度变化的航路点任务。如果您不希望在任务中使用高度斜率，则可以将其设置为零，这将禁用高度斜率计算。否则，您可以将其设置为在使用高度斜率更改高度之前到目的地航路点的最小高度误差米数。这也控制了 QRTL 进近航段期间使用的逐渐高度坡道。</translation>
    </message>
    <message>
      <source>This controls the weight the compass or GPS has on the heading. A higher value means the heading will track the yaw source (GPS or compass) more rapidly.</source>
      <translation>这控制罗盘或 GPS 在航向上的权重。值越高意味着航向将更快地跟踪偏航源（GPS 或罗盘）。</translation>
    </message>
    <message>
      <source>This controls whether real servo controls are used in HIL mode. If you enable this then the APM will control the real servos in HIL mode. If disabled it will report servo values, but will not output to the real servos. Be careful that your motor and propeller are not connected if you enable this option.</source>
      <translation>该参数控制 HIL 模式下是否使用真正的舵机控制。如果启用此功能，则 APM 将在 HIL 模式下控制实际舵机。如果禁用，它将报告舵机值，但不会输出到真实舵机。如果启用此选项，请注意您的电机和螺旋桨未连接。</translation>
    </message>
    <message>
      <source>This controls whether tuning values will revert on a flight mode change.</source>
      <translation>这控制调整值是否会在飞行模式更改时恢复。</translation>
    </message>
    <message>
      <source>This defines the latency between a roll step input and the achieved setpoint (inverse to a P gain). Smaller systems may require smaller values.</source>
      <translation>这定义了横滚步进输入和达到的设定点之间的延迟（与 P 增益相反）。较小的系统可能需要较小的值。</translation>
    </message>
    <message>
      <source>This determines when the filter will use the 3-axis magnetometer fusion model that estimates both earth and body fixed magnetic field states, when it will use a simpler magnetic heading fusion model that does not use magnetic field states and when it will use an alternative method of yaw determination to the magnetometer. The 3-axis magnetometer fusion is only suitable for use when the external magnetic field environment is stable. EK2_MAG_CAL = 0 uses heading fusion on ground, 3-axis fusion in-flight, and is the default setting for Plane users. EK2_MAG_CAL = 1 uses 3-axis fusion only when manoeuvring. EK2_MAG_CAL = 2 uses heading fusion at all times, is recommended if the external magnetic field is varying and is the default for rovers. EK2_MAG_CAL = 3 uses heading fusion on the ground and 3-axis fusion after the first in-air field and yaw reset has completed, and is the default for copters. EK2_MAG_CAL = 4 uses 3-axis fusion at all times. NOTE: The fusion mode can be forced to 2 for specific EKF cores using the EK2_MAG_MASK parameter. NOTE: limited operation without a magnetometer or any other yaw sensor is possible by setting all COMPASS_USE, COMPASS_USE2, COMPASS_USE3, etc parameters to 0 with COMPASS_ENABLE set to 1. If this is done, the EK2_GSF_RUN and EK2_GSF_USE masks must be set to the same as EK2_IMU_MASK.</source>
      <translation>这决定了滤波器何时使用估计地球和人体固定磁场状态的 3 轴磁力计融合模型、何时使用不使用磁场状态的更简单的磁航向融合模型以及何时使用磁力计的替代偏航确定方法。 3轴磁力计融合器仅适合在外部磁场环境稳定时使用。 EK2_MAG_CAL = 0 在地面上使用航向融合，3 在飞行中使用轴融合，并且是 Plane 用户的默认设置。 EK2_MAG_CAL = 1 仅在机动时使用 3 轴融合。 EK2_MAG_CAL = 2 始终使用航向融合，如果外部磁场变化，建议使用航向融合，并且是流动站的默认设置。 EK2_MAG_CAL = 3 在地面上使用航向融合，在第一个空中场和偏航重置完成后使用 3 轴融合，并且是直升机的默认设置。 EK2_MAG_CAL = 4 始终使用 3 轴融合。 NOTE：对于特定的 EKF 内核，可以使用 EK2_MAG_MASK 参数将融合模式强制为 2。 NOTE：通过将所有 COMPASS_USE、COMPASS_USE2、COMPASS_USE3 等参数设置为 0，并将 COMPASS_ENABLE 设置为 1，可以在没有磁力计或任何其他偏航传感器的情况下进行有限操作。如果这样做，EK2_GSF_RUN 和 EK2_GSF_USE 掩码必须设置为与 EK2_IMU_MASK 相同。</translation>
    </message>
    <message>
      <source>This enables Tailsitter functionality. A value of 2 forces Qassist active and always stabilize in forward flight with airmode for stabilisation at 0 throttle, for use on vehicles with no control surfaces, vehicle will not arm in forward flight modes, see also Q_OPTIONS "Mtrs_Only_Qassist"</source>
      <translation>这将启用 Tailsitter 功能。 2 的值会强制 Qassist 处于活动状态，并始终在空气模式下稳定向前飞行，以便在 0 油门处稳定，用于没有控制面的车辆，车辆不会在向前飞行模式下解锁，另请参阅 Q_OPTIONS“Mtrs_Only_Qassist”</translation>
    </message>
    <message>
      <source>This enables continuous calibration of the magnetometers
before takeoff using gyro data.</source>
      <translation>这样可以在起飞前使用陀螺仪数据连续校准磁力计。</translation>
    </message>
    <message>
      <source>This enables support for direct attached radio receivers</source>
      <translation>这可以支持直接连接的无线电接收器</translation>
    </message>
    <message>
      <source>This enables the geofence part of the AFS. Will only be in effect if AFS_ENABLE is also 1</source>
      <translation>这将启用 AFS 的地理围栏部分。仅当 AFS_ENABLE 也是 1 时才有效</translation>
    </message>
    <message>
      <source>This filter low pass filter is applied to the input for D terms.</source>
      <translation>该滤波器低通滤波器应用于 D 项的输入。</translation>
    </message>
    <message>
      <source>This increment is added to TRIM_PITCH when airspeed is FW_AIRSPD_MAX.</source>
      <translation>当空速为 FW_AIRSPD_MAX 时，该增量被添加到 TRIM_PITCH。</translation>
    </message>
    <message>
      <source>This integer bitmask controls the set of GNSS systems used by the receiver. Check your receiver's documentation on how many systems are supported to be used in parallel. Currently this functionality is just implemented for u-blox receivers. When no bits are set, the receiver's default configuration should be used. Set bits true to enable: 0 : Use GPS (with QZSS) 1 : Use SBAS (multiple GPS augmentation systems) 2 : Use Galileo 3 : Use BeiDou 4 : Use GLONASS 5 : Use NAVIC</source>
      <translation>该整数位掩码控制接收器使用的 GNSS 系统集。检查接收器的文档，了解支持并行使用的系统数量。目前，此功能仅针对 u-blox 接收器实现。当未设置任何位时，应使用接收器的默认配置。设置位为真以启用： 0：使用 GPS（带 QZSS） 1：使用 SBAS（多个 GPS 增强系统） 2：使用伽利略 3：使用北斗 4：使用 GLONASS 5：使用 NAVIC</translation>
    </message>
    <message>
      <source>This is a limit on how high the terrain avoidane will take the vehicle. It acts a failsafe to prevent vertical flyaways.</source>
      <translation>这是对地形回避将车辆带到多高的限制。它起到故障保护作用，防止垂直飞走。</translation>
    </message>
    <message>
      <source>This is an RC input channel for requesting engine start. Engine will try to start when channel is at or above 1700. Engine will stop when channel is at or below 1300. Between 1301 and 1699 the engine will not change state unless a MAVLink command or mission item commands a state change, or the vehicle is disamed.</source>
      <translation>这是用于请求发动机启动的 RC 输入通道。当通道等于或高于 1700 时，发动机将尝试启动。当通道等于或低于 1300 时，发动机将停止。在 1301 和 1699 之间，发动机不会改变状态，除非 MAVLink 命令或任务项命令状态改变，或者车辆被解除。</translation>
    </message>
    <message>
      <source>This is delay between the assistance thresholds being met and the assistance starting.</source>
      <translation>这是达到援助阈值和援助开始之间的延迟。</translation>
    </message>
    <message>
      <source>This is the 1-STD odometry velocity observation error that will be assumed when wheel encoder data is being fused.</source>
      <translation>这是融合车轮编码器数据时假设的 1-STD 里程计速度观测误差。</translation>
    </message>
    <message>
      <source>This is the RMS value of noise in magnetometer measurements. Increasing it reduces the weighting on these measurements.</source>
      <translation>这是磁力计测量中的噪声 RMS 值。增加它会减少这些测量的权重。</translation>
    </message>
    <message>
      <source>This is the action taken when the left (Mode) button is long-pressed</source>
      <translation>这是长按左（模式）按钮时执行的操作</translation>
    </message>
    <message>
      <source>This is the action taken when the right action is pressed while the left (Mode) button is held down</source>
      <translation>这是在按住左侧（模式）按钮的同时按下右侧操作时所采取的操作</translation>
    </message>
    <message>
      <source>This is the altitude in meters above which for navigation can begin. This applies in auto takeoff and auto landing.</source>
      <translation>这是可以开始导航的高度（以米为单位）。这适用于自动起飞和自动着陆。</translation>
    </message>
    <message>
      <source>This is the angular error in attitude beyond which the quadplane VTOL motors will provide stability assistance. This will only be used if Q_ASSIST_SPEED is also non-zero. Assistance will be given if the attitude is outside the normal attitude limits by at least 5 degrees and the angular error in roll or pitch is greater than this angle for at least 1 second. Set to zero to disable angle assistance.</source>
      <translation>这是姿态角度误差，超过该角度误差四平面 VTOL 电机将提供稳定性辅助。仅当 Q_ASSIST_SPEED 也不为零时才使用此值。如果姿态超出正常姿态限制至少 5 度，并且横摇或俯仰角度误差大于该角度至少 1 秒，则将提供帮助。设置为零以禁用角度辅助。</translation>
    </message>
    <message>
      <source>This is the applied simulated acceleration to the 2nd accelerometer (Z-axis)</source>
      <translation>这是应用到第二个加速度计（Z 轴）的模拟加速度</translation>
    </message>
    <message>
      <source>This is the applied simulated angular rate to the 2nd gyroscope (Y-axis)</source>
      <translation>这是对第二个陀螺仪（Y 轴）应用的模拟角速率</translation>
    </message>
    <message>
      <source>This is the arm length generating the rolling moment This value can be measured with a ruler. This corresponds to half the distance between the left and right motors.</source>
      <translation>这是产生滚动力矩的臂长。这个值可以用尺子测量。这相当于左右电机之间距离的一半。</translation>
    </message>
    <message>
      <source>This is the distance from Home that the final approach waypoint is set. The waypoint point will be in the opposite direction of takeoff (the direction the plane is facing when the plane sets its takeoff heading)</source>
      <translation>这是设置的最终进近航路点距起飞点的距离。航路点将位于起飞的相反方向（飞机设定起飞航向时飞机所面对的方向）</translation>
    </message>
    <message>
      <source>This is the extra throttle added in schedule elements marked as needing a throttle boost</source>
      <translation>这是在标记为需要油门升压的计划元素中添加的额外油门</translation>
    </message>
    <message>
      <source>This is the initial mode when the vehicle is first turned on. This mode is assumed to not require GPS</source>
      <translation>这是车辆首次启动时的初始模式。假设该模式不需要 GPS</translation>
    </message>
    <message>
      <source>This is the maximally added yaw rate setpoint from the yaw stick in any attitude controlled flight mode. It is added to the yaw rate setpoint generated by the controller for turn coordination.</source>
      <translation>这是任何姿态控制飞行模式下偏航杆添加的最大偏航率设定值。它被添加到控制器生成的偏航率设定值中以进行转弯协调。</translation>
    </message>
    <message>
      <source>This is the maximum range of the vehicle in kilometers from first arming. If the vehicle goes beyond this range then the TERM_ACTION is performed. A value of zero disables this feature.</source>
      <translation>这是车辆从第一次解锁开始的最大射程（以公里为单位）。如果车辆超出此范围，则执行 TERM_ACTION。值为零会禁用此功能。</translation>
    </message>
    <message>
      <source>This is the maximum vertical acceleration either up or down that the controller will use to correct speed or height errors.</source>
      <translation>这是控制器用来校正速度或高度误差的最大向上或向下垂直加速度。</translation>
    </message>
    <message>
      <source>This is the minimum altitude in centimeters that FBWB and CRUISE modes will allow. If you attempt to descend below this altitude then the plane will level off. A value of zero means no limit.</source>
      <translation>这是 FBWB 和 CRUISE 模式允许的最小高度（以厘米为单位）。如果您尝试下降到该高度以下，那么飞机将会保持平稳。值为零表示没有限制。</translation>
    </message>
    <message>
      <source>This is the number of acceleration events to require for arming with TKOFF_THR_MINACC. The default is 1, which means a single forward acceleration above TKOFF_THR_MINACC will arm. By setting this higher than 1 you can require more forward/backward movements to arm.</source>
      <translation>这是配备 TKOFF_THR_MINACC 所需的加速事件数。默认值为 1，这意味着高于 TKOFF_THR_MINACC 的单一向前加速度将启动。通过将其设置为高于 1，您可能需要更多的向前/向后移动来进行解锁。</translation>
    </message>
    <message>
      <source>This is the number of msec that the range beacon measurements lag behind the inertial measurements. It is the time from the end of the optical flow averaging period and does not include the time delay due to the 100msec of averaging within the flow sensor.</source>
      <translation>这是距离信标测量滞后于惯性测量的毫秒数。它是从光流平均周期结束开始的时间，不包括由于流量传感器内平均 100msec 造成的时间延迟。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PCA9685_FUNC15).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC15）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PCA9685_FUNC8).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC8）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PWM_AUX_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_AUX_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PWM_MAIN_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_MAIN_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC5).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_EC_FUNC5）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC5).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_SV_FUNC5）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_EC_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_SV_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC0).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC0）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PCA9685_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PCA9685_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PWM_AUX_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_AUX_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PWM_MAIN_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_MAIN_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see RBCLW_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 RBCLW_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_EC_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_SV_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC3).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_EC_FUNC3）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC3).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_SV_FUNC3）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC11).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC11）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC10).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC10）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the percentage throttle output for engine start</source>
      <translation>这是发动机启动时的油门输出百分比</translation>
    </message>
    <message>
      <source>This is the ratio of static pressure error to dynamic pressure generated by a negative wind relative velocity along the X body axis. If the baro height estimate rises during backwards flight, then this will be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_BCOEF_X and EK3_BCOEF_Y parameters have been tuned.</source>
      <translation>这是沿 X 体轴负风相对速度产生的静压误差与动压之比。如果在向后飞行期间气压高度估计值上升，则这将是负数。仅当使用 EKF3 并且已调整 EK3_BCOEF_X 和 EK3_BCOEF_Y 参数时，多旋翼才能使用此功能。</translation>
    </message>
    <message>
      <source>This is the ratio of static pressure error to dynamic pressure generated by a positive wind relative velocity along the Y body axis. If the baro height estimate rises during sideways flight to the right, then this should be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_BCOEF_X and EK3_BCOEF_Y parameters have been tuned.</source>
      <translation>这是静压误差与沿 Y 体轴正风相对速度产生的动压之比。如果在向右侧向飞行期间气压高度估计值上升，则该值应该是负数。仅当使用 EKF3 且已调整 EK3_BCOEF_X 和 EK3_BCOEF_Y 参数时，多旋翼飞行器才能使用此功能。</translation>
    </message>
    <message>
      <source>This is the scale IAS --&gt; CAS of the first airspeed sensor instance</source>
      <translation>这是第一个空速传感器实例的比例 IAS --&gt; CAS</translation>
    </message>
    <message>
      <source>This is the speed below which the quad motors will provide stability and lift assistance in fixed wing modes. Zero means no assistance except during transition</source>
      <translation>低于该速度，四电机将在固定翼模式下提供稳定性和升力辅助。零意味着除了过渡期间之外没有任何帮助</translation>
    </message>
    <message>
      <source>This is the time after the start of flaring that we expect the vehicle to touch the runway. At this time, a 0.5s clamp down ramp will engage, constraining the pitch setpoint to RWTO_PSP. If enabled, ensure that RWTO_PSP is configured appropriately for full gear contact on ground roll. Set to -1.0 to disable touchdown clamping. E.g. it may not be desirable to clamp on belly landings. The touchdown time will be constrained to be greater than or equal to the flare time (FW_LND_FL_TIME).</source>
      <translation>这是燃烧开始后我们预计飞行器接触跑道的时间。此时，0.5s 钳位斜坡将接合，将桨距设定点限制为 RWTO_PSP。如果启用，请确保 RWTO_PSP 已正确配置，以便在地面横滚时实现全齿轮接触。设置为 -1.0 以禁用触地钳位。例如。夹住机腹着陆可能并不理想。着陆时间将被限制为大于或等于耀斑时间 (FW_LND_FL_TIME)。</translation>
    </message>
    <message>
      <source>This is the timeout in seconds before a low voltage event will be triggered. For aircraft with low C batteries it may be necessary to raise this in order to cope with low voltage on long takeoffs. A value of zero disables low voltage errors.</source>
      <translation>这是触发低电压事件之前的超时时间（以秒为单位）。对于配备低 C 电池的飞机，可能需要提高此值以应对长途起飞时的低电压。值为零会禁用低电压错误。</translation>
    </message>
    <message>
      <source>This is the value used as the target module ID of all configuration parameters (not operational parameters). The Vertiq module with the module ID matching this value will react to all get and set requests from PX4. Any Vertiq client made with dynamic object IDs should use this value to instantiate itself.</source>
      <translation>这是用作目标模块 ID 的所有配置参数（非操作参数）的值。具有与该值匹配的模块 ID 的 Vertiq 模块将对来自 PX4 的所有获取和设置请求做出反应。任何使用动态对象 ID 制作的 Vertiq 客户端都应使用此值来实例化自身。</translation>
    </message>
    <message>
      <source>This is used in rover vehicles, where the sensor is a fixed height above the ground</source>
      <translation>这用于流动车辆，其中传感器距地面固定高度</translation>
    </message>
    <message>
      <source>This is used to enable flow control for the serial uXRCE instance. Used for reliable high bandwidth communication.</source>
      <translation>这用于启用串行 uXRCE 实例的流量控制。用于可靠的高带宽通信。</translation>
    </message>
    <message>
      <source>This is useful when map, GNSS, or yaw errors on ground are misaligned with what the operator intends for takeoff course. Particularly useful for skinny runways or if the wheel servo is a bit off trim.</source>
      <translation>当地面上的地图、GNSS 或偏航误差与操作员想要的起飞航向不一致时，这非常有用。对于狭窄的跑道或车轮舵机系统有点偏离的情况特别有用。</translation>
    </message>
    <message>
      <source>This linearizes the swashplate 2 servo's mechanical output to account for nonlinear output due to arm rotation.  This requires a specific setup procedure to work properly.  The servo arm must be centered on the mechanical throw at the servo trim position and the servo trim position kept as close to 1500 as possible. Leveling the swashplate can only be done through the pitch links.  See the ardupilot wiki for more details on setup.</source>
      <translation>这使得斜盘 2 舵机系统的机械输出线性化，以解决由于手臂旋转而导致的非线性输出。  这需要特定的设置过程才能正常工作。  舵机臂必须在舵机微调位置处的机械投掷上居中，并且舵机微调位置尽可能靠近 1500。只能通过俯仰连杆调平斜盘。  有关设置的更多详细信息，请参阅 ardupilot wiki。</translation>
    </message>
    <message>
      <source>This noise controls the growth of the vertical accelerometer delta velocity bias state error estimate. Increasing it makes accelerometer bias estimation faster and noisier.</source>
      <translation>该噪声控制垂直加速度计增量速度偏差状态误差估计的增长。增加它会使加速度计偏差估计更快、噪音更大。</translation>
    </message>
    <message>
      <source>This offsets where the pulse starts around the motor to allow for propeller mechanical properties.</source>
      <translation>这会偏移电机周围脉冲开始的位置，以考虑螺旋桨的机械特性。</translation>
    </message>
    <message>
      <source>This parameter allows you to skip airspeed offset calibration on startup, instead using the offset from the last calibration or requiring a manual calibration. This may be desirable if the offset variance between flights for your sensor is low and you want to avoid having to cover the pitot tube on each boot.</source>
      <translation>此参数允许您在启动时跳过空速偏移校准，而是使用上次校准的偏移或需要手动校准。如果您的传感器飞行之间的偏移差异较小并且您希望避免覆盖每个引导件上的皮托管，则这可能是理想的。</translation>
    </message>
    <message>
      <source>This parameter controls the maximum amount of difference in horizontal acceleration between the value predicted by the filter and the value measured by the GPS before the GPS position data is rejected. If this value is set too low, then valid GPS data will be regularly discarded, and the position accuracy will degrade. If this parameter is set too high, then large GPS glitches will cause large rapid changes in position.</source>
      <translation>该参数控制在拒绝 GPS 位置数据之前滤波器预测值与 GPS 测量值之间水平加速度的最大差异量。如果该值设置得太低，则有效的GPS数据将被定期丢弃，并且位置精度将下降。如果该参数设置得太高，那么较大的 GPS 毛刺将导致位置发生较大的快速变化。</translation>
    </message>
    <message>
      <source>This parameter controls the source of battery data. The value 'Power Module / Analog' means that measurements are expected to come from either analog (ADC) inputs or an I2C power monitor (e.g. INA226). Analog inputs are voltage and current measurements read from the board's ADC channels, typically from an onboard voltage divider and current shunt, or an external analog power module. I2C power monitors are digital sensors on the I2C bus. If the value is set to 'External' then the system expects to receive MAVLink or CAN battery status messages, or the battery data is published by an external driver. If the value is set to 'ESCs', the battery information are taken from the esc_status message. This requires the ESC to provide both voltage as well as current (via ESC telemetry).</source>
      <translation>该参数控制电池数据的来源。 “电源模块/模拟”值表示测量预计来自模拟 (ADC) 输入或 I2C 电源监视器（例如 INA226）。模拟输入是从电路板的 ADC 通道读取的电压和电流测量值，通常来自板载分压器和分流器，或外部模拟电源模块。 I2C 功率监视器是 I2C 总线上的数字传感器。如果该值设置为“外部”，则系统期望接收 MAVLink 或 CAN 电池状态消息，或者电池数据由外部驱动程序发布。如果该值设置为“ESCs”，则从 esc_status 消息中获取电池信息。这需要 ESC 提供电压和电流（通过 ESC 遥测）。</translation>
    </message>
    <message>
      <source>This parameter defines the maximum distance from ground at which the optical flow sensor operates reliably.
The height setpoint will be limited to be no greater than this value when the navigation system
is completely reliant on optical flow data and the height above ground estimate is valid.
The sensor may be usable above this height, but accuracy will progressively degrade.</source>
      <translation>该参数定义了光流传感器可靠运行时距地面的最大距离。当导航系统完全依赖于光流数据并且离地高度估计有效时，高度设定点将被限制为不大于该值。高于此高度传感器可能仍可用，但精度会逐渐降低。</translation>
    </message>
    <message>
      <source>This parameter defines the minimum condition under which the system will command the ANTI_COLLISION lights on 0 - Always off 1 - When autopilot is armed 2 - When autopilot is prearmed 3 - Always on</source>
      <translation>该参数定义了系统命令 ANTI_COLLISION 灯亮起的最低条件 0 - 始终关闭 1 - 当自动驾驶仪解锁时 2 - 当自动驾驶仪预解锁时 3 - 始终打开</translation>
    </message>
    <message>
      <source>This parameter defines the operating mode of the MXS</source>
      <translation>该参数定义了MXS的工作模式</translation>
    </message>
    <message>
      <source>This parameter defines the type of fuel used in the vehicle's fuel tank. 0: Unknown 1: Liquid (e.g., gasoline, diesel) 2: Gas (e.g., hydrogen, methane, propane)</source>
      <translation>该参数定义了车辆油箱中使用的燃油类型。 0：未知 1：液体（例如汽油、柴油） 2：气体（例如氢气、甲烷、丙烷）</translation>
    </message>
    <message>
      <source>This parameter is automatically set to identify external modes. It ensures that modes get assigned to the same index independent from their startup order, which is required when mapping an external mode to an RC switch.</source>
      <translation>该参数自动设置以识别外部模式。它确保模式被分配到与其启动顺序无关的相同索引，这在将外部模式映射到 RC 开关时是必需的。</translation>
    </message>
    <message>
      <source>This parameter is used to predict the drag produced by the rotors when flying a multi-copter, enabling estimation of wind drift. The drag produced by this effect scales with speed not speed squared and is produced because some of the air velocity normal to the rotors axis of rotation is lost when passing through the rotor disc which changes the momentum of the airflow causing drag. For unducted rotors the effect is roughly proportional to the area of the propeller blades when viewed side on and changes with different propellers. It is higher for ducted rotors. For example if flying at 15 m/s at sea level conditions produces a rotor induced drag acceleration of 1.5 m/s/s, then EK3_DRAG_MCOEF would be set to 0.1 = (1.5/15.0). Set EK3_MCOEF to a postive value to enable wind estimation using this drag effect. To account for the drag produced by the body which scales with speed squared, see documentation for the EK3_DRAG_BCOEF_X and EK3_DRAG_BCOEF_Y parameters.</source>
      <translation>该参数用于预测多旋翼飞行时转子产生的阻力，从而能够估计风漂移。这种效应产生的阻力与速度成比例，而不是速度的平方，并且是由于垂直于转子旋转轴的一些空气速度在穿过转子盘时损失而产生的，这改变了气流的动量，从而导致阻力。对于无涵道转子，从侧面观察时，效果大致与螺旋桨叶片的面积成正比，并且随不同的螺旋桨而变化。对于涵道式转子来说该值更高。例如，如果在海平面条件下以 15 m/s 飞行产生 1.5 m/s/s 的旋翼诱导阻力加速度，则 EK3_DRAG_MCOEF 将设置为 0.1 = (1.5/15.0)。将 EK3_MCOEF 设置为正值以启用使用此阻力效果的风估计。要考虑主体产生的阻力（与速度平方成比例），请参阅 EK3_DRAG_BCOEF_X 和 EK3_DRAG_BCOEF_Y 参数的文档。</translation>
    </message>
    <message>
      <source>This parameter sets the airspeed at which the aircraft will "rotate", setting climb pitch specified in the mission. If TKOFF_ROTATE_SPD is zero then the climb pitch will be used as soon as takeoff is started. For hand launch and catapult launches a TKOFF_ROTATE_SPD of zero should be set. For all ground launches TKOFF_ROTATE_SPD should be set above the stall speed, usually by about 10 to 30 percent. During the run, use TKOFF_GND_PITCH to keep the aircraft on the runway while below this airspeed.</source>
      <translation>该参数设置飞机“旋转”的空速，设置任务中指定的爬升俯仰。如果 TKOFF_ROTATE_SPD 为零，则起飞开始后将立即使用爬升俯仰。对于手动发射和弹射器发射，TKOFF_ROTATE_SPD 应设置为零。对于所有地面发射，TKOFF_ROTATE_SPD 应设置为高于失速速度，通常约为 10 至 30 百分比。在运行过程中，使用 TKOFF_GND_PITCH 使飞机在低于此空速时保持在跑道上。</translation>
    </message>
    <message>
      <source>This parameter sets the expected range measurement(in cm) that the fourth range finder should return when the vehicle is on the ground.</source>
      <translation>该参数设置车辆在地面时第四个测距仪应返回的预期距离测量值（以厘米为单位）。</translation>
    </message>
    <message>
      <source>This parameter sets the minimum time between two consecutive trigger events the specific camera setup is supporting.</source>
      <translation>此参数设置特定相机设置支持的两个连续触发事件之间的最短时间。</translation>
    </message>
    <message>
      <source>This parameter sets the polarity of the trigger (0 = active low, 1 = active high )</source>
      <translation>该参数设置触发的极性（0 = 低电平有效，1 = 高电平有效）</translation>
    </message>
    <message>
      <source>This parameter sets whether an analog rangefinder is ratiometric. Most analog rangefinders are ratiometric, meaning that their output voltage is influenced by the supply voltage. Some analog rangefinders (such as the SF/02) have their own internal voltage regulators so they are not ratiometric.</source>
      <translation>该参数设置模拟测距仪是否为比例式。大多数模拟测距仪都是比例式的，这意味着它们的输出电压受电源电压的影响。一些模拟测距仪（例如 SF/02）有自己的内部稳压器，因此它们不是比例式的。</translation>
    </message>
    <message>
      <source>This represents the Control Value Index where the targeted module will look for the Y rectangular coordinate.</source>
      <translation>这表示目标模块将在其中查找 Y 直角坐标的控制值索引。</translation>
    </message>
    <message>
      <source>This selects the bus number for looking for an I2C barometer. When set to -1 it will probe all external i2c buses based on the GND_PROBE_EXT parameter.</source>
      <translation>这将选择用于查找 I2C 气压计的总线编号。当设置为 -1 时，它将根据 GND_PROBE_EXT 参数探测所有外部 i2c 总线。</translation>
    </message>
    <message>
      <source>This selects the output PWM type, allowing for normal PWM continuous output, OneShot, brushed or DShot motor output.PWMRange and PWMAngle are PWM special/rare cases for ESCs that dont calibrate normally (some Sub motors) or where each ESC must have its PWM range set individually using the Servo params instead of PWM_MIN/MAX parameters.</source>
      <translation>选择输出 PWM 类型，允许正常 PWM 连续输出、OneShot、有刷或 DShot 电机输出。 PWMRange 和 PWMAngle 是 PWM 特殊/罕见情况，适用于无法正常校准的电调（某些子电机）或每个 ESC 必须使用舵机参数而不是 PWM_MIN/MAX 参数单独设置其 PWM 范围。</translation>
    </message>
    <message>
      <source>This sets a digital output pin to set high on flight termination. Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.</source>
      <translation>这会将数字输出引脚设置为在飞行终止时设置为高电平。给出了一些常见值，但请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。</translation>
    </message>
    <message>
      <source>This sets a timeout for serial pass-through in seconds. When the pass-through is enabled by setting the SERIAL_PASS1 and SERIAL_PASS2 parameters then it remains in effect until no data comes from the first port for SERIAL_PASSTIMO seconds. This allows the port to revent to its normal usage (such as MAVLink connection to a GCS) when it is no longer needed. A value of 0 means no timeout. A value of -1 means no timeout and the SERIAL_PASS2 parameter is not reset on reboot.</source>
      <translation>这设置了串行传递的超时（以秒为单位）。当通过设置 SERIAL_PASS1 和 SERIAL_PASS2 参数启用直通时，它将一直有效，直到 SERIAL_PASSTIMO 秒内没有来自第一个端口的数据。这允许端口在不再需要时恢复正常使用（例如 MAVLink 连接到 GCS）。 0 的值表示没有超时。 -1 值表示没有超时，并且 SERIAL_PASS2 参数在重新启动时不会重置。</translation>
    </message>
    <message>
      <source>This sets the DShot ESC type for all outputs. The ESC type affects the range of DShot commands available. None means that no dshot commands will be executed.</source>
      <translation>这将为所有输出设置 DShot ESC 类型。 ESC 类型影响可用 DShot 命令的范围。 None 表示不会执行 dshot 命令。</translation>
    </message>
    <message>
      <source>This sets the PWM upper limit for this screen</source>
      <translation>这设置了该屏幕的 PWM 上限</translation>
    </message>
    <message>
      <source>This sets the amount of position variation that the EKF allows for when operating without external measurements (eg GPS or optical flow). Increasing this parameter makes the EKF attitude estimate less sensitive to vehicle manoeuvres but more sensitive to IMU errors.</source>
      <translation>这设置了 EKF 在没有外部测量（例如 GPS 或光流）的情况下运行时允许的位置变化量。增加该参数会使 EKF 姿态估计对车辆操纵不太敏感，但对 IMU 误差更敏感。</translation>
    </message>
    <message>
      <source>This sets the channel for transmitter tuning. This should be connected to a knob or slider on your transmitter. It needs to be setup to use the PWM range given by TUNE_CHAN_MIN to TUNE_CHAN_MAX</source>
      <translation>这设置了发射机调谐的通道。它应该连接到发射器上的旋钮或滑块。需要设置为使用 TUNE_CHAN_MIN 至 TUNE_CHAN_MAX 给出的 PWM 范围</translation>
    </message>
    <message>
      <source>This sets the duration during which the MC motors ramp up to the commanded thrust during the back transition stage.</source>
      <translation>这设置了 MC 电机在后过渡阶段加速到指令推力的持续时间。</translation>
    </message>
    <message>
      <source>This sets the max pulsing voltage limit when in Voltage Limit Mode.</source>
      <translation>这设置了处于电压限制模式时的最大脉冲电压限制。</translation>
    </message>
    <message>
      <source>This sets the maximum number of degrees of back or pitch up in Q modes when the airspeed is at AIRSPEED_MIN, and is used to prevent excessive sutructural loads when pitching up decelerate. If airspeed is above or below AIRSPEED_MIN, the pitch up/back will be adjusted according to the formula pitch_limit = Q_BCK_PIT_LIM * (AIRSPEED_MIN / IAS)^2. The backwards/up pitch limit controlled by this parameter is in addition to limiting applied by PTCH_LIM_MAX_DEG and Q_ANGLE_MAX. The BCK_PIT_LIM limit is only applied when Q_FWD_THR_USE is set to 1 or 2 and the vehicle is flying in a mode that uses forward throttle instead of forward tilt to generate forward speed. Set to a non positive value 0 to deactivate this limit.</source>
      <translation>这设置了当空速为 AIRSPEED_MIN 时 Q 模式下后退或俯仰的最大度数，并用于防止在俯仰减速时出现过大的结构负载。如果空速高于或低于AIRSPEED_MIN，则将根据公式pitch_limit = Q_BCK_PIT_LIM * (AIRSPEED_MIN / IAS)^2 调整俯仰/后仰。除 PTCH_LIM_MAX_DEG 和 Q_ANGLE_MAX 所施加的限制外，该参数控制的向后/向上俯仰限制。仅当 Q_FWD_THR_USE 设置为 1 或 2 并且飞行器以使用前向油门而不是前倾来产生前向速度的模式飞行时，BCK_PIT_LIM 限制才适用。设置为非正值 0 可禁用此限制。</translation>
    </message>
    <message>
      <source>This sets the maximum speed adjustment for time sync between aircraft</source>
      <translation>这设置了飞机之间时间同步的最大速度调整</translation>
    </message>
    <message>
      <source>This sets the parts per thousand scale factor correction applied to the flow sensor X axis optical rate. It can be used to correct for variations in effective focal length. Each positive increment of 1 increases the scale factor applied to the X axis optical flow reading by 0.1%. Negative values reduce the scale factor.</source>
      <translation>这设置了应用于流量传感器 X 轴光学速率的千分之比例因子校正。它可用于校正有效焦距的变化。 1 的每个正增量都会增加 0.1% 应用于 X 轴光流读数的比例因子。负值会降低比例因子。</translation>
    </message>
    <message>
      <source>This sets the percentage number of standard deviations applied to the height measurement innovation consistency check. Decreasing it makes it more likely that good measurements will be rejected. Increasing it makes it more likely that bad measurements will be accepted.</source>
      <translation>这设置了应用于高度测量创新一致性检查的标准偏差的百分比数。降低它会使好的测量结果更有可能被拒绝。增加它使得错误的测量更有可能被接受。</translation>
    </message>
    <message>
      <source>This sets the radio to a fixed test channel for factory testing. Using a fixed channel avoids the need for binding in factory testing.</source>
      <translation>这会将无线电设置为固定测试通道以进行工厂测试。使用固定通道可以避免在工厂测试中进行绑定。</translation>
    </message>
    <message>
      <source>This sets the specific gravity of the fluid when flying an underwater ROV.</source>
      <translation>这设置了 ROV 水下飞行时流体的比重。</translation>
    </message>
    <message>
      <source>This sets the time constant of the low pass filter that is applied to the height demand input when bit 0 of TECS_OPTIONS is not selected.</source>
      <translation>当未选择 TECS_OPTIONS 的位 0 时，设置应用于高度需求输入的低通滤波器的时间常数。</translation>
    </message>
    <message>
      <source>This sets the voltage max for PWM output pulses. 0 for 3.3V and 1 for 5V output. On boards with an IOMCU that support this parameter this option only affects the 8 main outputs, not the 6 auxiliary outputs. Using 5V output can help to reduce the impact of ESC noise interference corrupting signals to the ESCs.</source>
      <translation>这设置了 PWM 输出脉冲的最大电压。 0 用于 3.3V，1 用于 5V 输出。在带有支持此参数的 IOMCU 的板上，此选项仅影响 8 主输出，而不影响 6 辅助输出。使用 5V 输出有助于减少 ESC 噪声干扰破坏信号对 ESC 的影响。</translation>
    </message>
    <message>
      <source>This sets which OSD font to use. It is an integer from 0 to the number of fonts available</source>
      <translation>这设置了使用哪种 OSD 字体。它是一个从 0 到可用字体数量的整数</translation>
    </message>
    <message>
      <source>This state  process noise controls growth of the gyro delta angle bias state error estimate. Increasing it makes rate gyro bias estimation faster and noisier.</source>
      <translation>该状态过程噪声控制陀螺仪增量角偏置状态误差估计的增长。增加它会使速率陀螺仪偏差估计更快、噪声更大。</translation>
    </message>
    <message>
      <source>This triggers the action specified in COM_POS_LOW_ACT if the estimated position accuracy is below this threshold.
Local position has to be still declared valid, which requires some kind of velocity aiding or large dead-reckoning time (EKF2_NOAID_TOUT),
and a high failsafe threshold (COM_POS_FS_EPH).
Set to -1 to disable.</source>
      <translation>如果估计的位置精度低于该阈值，则会触发 COM_POS_LOW_ACT 中指定的操作。本地位置仍必须被声明为有效，这需要某种速度辅助或较大的航位推算时间 (EKF2_NOAID_TOUT) 以及较高的故障安全阈值 (COM_POS_FS_EPH)。设置为 -1 以禁用。</translation>
    </message>
    <message>
      <source>This value is used to initialize the in-flight average current estimation, which in turn is used for estimating remaining flight time and RTL triggering.</source>
      <translation>该值用于初始化飞行中平均电流估计，进而用于估计剩余飞行时间和 RTL 触发。</translation>
    </message>
    <message>
      <source>This will stop scripts being loaded from the given locations</source>
      <translation>这将停止从给定位置加载脚本</translation>
    </message>
    <message>
      <source>Threshold for mid position of payload power switch</source>
      <translation>负载电源开关中间位置阈值</translation>
    </message>
    <message>
      <source>Threshold for the angle between the active cruise direction and the cruise direction given
by the stick inputs.
This can be understood as a deadzone for the combined stick inputs for forward/backwards
and lateral speed which defines a course direction.</source>
      <translation>活动巡航方向与摇杆输入给定的巡航方向之间的角度阈值。这可以理解为用于定义路线方向的向前/向后和横向速度的组合操纵杆输入的死区。</translation>
    </message>
    <message>
      <source>Throttle (HeliRSC Servo) output in percent to the external motor governor when motor interlock enabled (throttle hold off).</source>
      <translation>当电机互锁启用（油门延迟关闭）时，油门（HeliRSC 舵机）以百分比形式输出到外部电机调速器。</translation>
    </message>
    <message>
      <source>Throttle Curve at 50% Coll</source>
      <translation>50% Coll 的油门曲线</translation>
    </message>
    <message>
      <source>Throttle Ramp Time</source>
      <translation>油门斜坡时间</translation>
    </message>
    <message>
      <source>Throttle acceleration controller I gain maximum.  Constrains the maximum pwm that the I term will generate</source>
      <translation>油门加速控制器 I 增益最大。  限制 I 项将生成的最大 pwm</translation>
    </message>
    <message>
      <source>Throttle at min airspeed</source>
      <translation>油门最小空速</translation>
    </message>
    <message>
      <source>Throttle cruise percentage</source>
      <translation>油门巡航百分比</translation>
    </message>
    <message>
      <source>Throttle during runway takeoff</source>
      <translation>跑道起飞时油门</translation>
    </message>
    <message>
      <source>Throttle gain scalar</source>
      <translation>油门增益标量</translation>
    </message>
    <message>
      <source>Throttle maximum</source>
      <translation>油门最大</translation>
    </message>
    <message>
      <source>Throttle output (HeliRSC Servo) in percent while armed but motor interlock is disabled (throttle hold on). FOR COMBUSTION ENGINES. Sets the engine ground idle throttle percentage with clutch disengaged. This must be set to zero for electric helicopters under most situations. If the ESC has an autorotation window this can be set to keep the autorotation window open in the ESC. Consult the operating manual for your ESC to set it properly for this purpose</source>
      <translation>解锁时油门输出（HeliRSC 舵机）以百分比表示，但电机联锁被禁用（油门保持打开）。 FOR COMBUSTION ENGINES。设置离合器分离时发动机地面怠速油门百分比。在大多数情况下，电动直升机必须将其设置为零。如果 ESC 有自动旋转窗口，则可以进行设置以在 ESC 中保持自动旋转窗口打开。请查阅 ESC 的操作手册，以便为此目的正确设置</translation>
    </message>
    <message>
      <source>Throttle suppress manual passthru</source>
      <translation>油门抑制手动直通</translation>
    </message>
    <message>
      <source>Throttle within a dead zone of RC trim value</source>
      <translation>油门在 RC 调整值的死区内</translation>
    </message>
    <message>
      <source>Throw</source>
      <translation>投掷</translation>
    </message>
    <message>
      <source>Thrust Asymmetry. Used for skid-steering. 2.0 means your motors move twice as fast forward than they do backwards.</source>
      <translation>推力不对称。用于滑移转向。 2.0 意味着您的电机向前移动的速度是向后移动速度的两倍。</translation>
    </message>
    <message>
      <source>Thrust coefficient of rotor 2</source>
      <translation>转子推力系数2</translation>
    </message>
    <message>
      <source>Thrust coefficient of the motors. Used to normalize the output of the neural network. Divided by 100 000</source>
      <translation>电机的推力系数。用于标准化神经网络的输出。除以 100 000</translation>
    </message>
    <message>
      <source>Tide direction</source>
      <translation>潮汐方向</translation>
    </message>
    <message>
      <source>Tilt 1 is used for control</source>
      <translation>倾斜1用于控制</translation>
    </message>
    <message>
      <source>Tilt Servo 0 Tilt Direction</source>
      <translation>倾斜舵机 0 倾斜方向</translation>
    </message>
    <message>
      <source>Tilt Servo 3 Tilt Angle at Minimum</source>
      <translation>倾斜舵机 3 最小倾斜角度</translation>
    </message>
    <message>
      <source>TiltMotorRearRight</source>
      <translation>倾斜电机后右</translation>
    </message>
    <message>
      <source>Tiltrotor tilt angle that will be used as flap</source>
      <translation>将用作襟翼的倾转旋翼倾斜角</translation>
    </message>
    <message>
      <source>Time allowed for in-flight power re-engagement</source>
      <translation>飞行中重新接通电源所允许的时间</translation>
    </message>
    <message>
      <source>Time constant of roll controller command / response, modeled as first order delay.
Used to determine lower period bound. Setting zero disables automatic period bounding.</source>
      <translation>横滚控制器命令/响应的时间常数，建模为一阶延迟。用于确定周期下限。设置为零会禁用自动周期限制。</translation>
    </message>
    <message>
      <source>Time for pitch to slew through its full range</source>
      <translation>俯仰在整个范围内发挥作用的时间</translation>
    </message>
    <message>
      <source>Time in seconds it takes to tilt form VT_TILT_FW to VT_TILT_MC.</source>
      <translation>从 VT_TILT_FW 倾斜到 VT_TILT_MC 所需的时间（以秒为单位）。</translation>
    </message>
    <message>
      <source>Time in seconds to slew output from zero to full. This is used to limit the rate at which output can change. Range is constrained between 0 and 0.5.</source>
      <translation>将输出从零转换到满的时间（以秒为单位）。这用于限制输出变化的速率。范围限制在 0 和 0.5 之间。</translation>
    </message>
    <message>
      <source>Time length for shove</source>
      <translation>推力时间长度</translation>
    </message>
    <message>
      <source>Time to achieve 63.2% of the surface altitude measurement change.  If 0 filtering is disabled</source>
      <translation>实现 63.2% 表面高度测量变化的时间。  如果 0 过滤被禁用</translation>
    </message>
    <message>
      <source>Time to slew the throttle from minimum to maximum while checking for a succsessful takeoff.</source>
      <translation>是时候将油门从最小转到最大，同时检查是否成功起飞。</translation>
    </message>
    <message>
      <source>Timeout before giving up on file writes</source>
      <translation>放弃文件写入之前超时</translation>
    </message>
    <message>
      <source>Timeout in seconds for the LTE connection. If no data is received for this time, the connection will be reset. A value of zero disables the timeout</source>
      <translation>LTE 连接超时（以秒为单位）。如果这次没有收到数据，连接将被重置。值为零会禁用超时</translation>
    </message>
    <message>
      <source>Timesync speed max</source>
      <translation>最大时间同步速度</translation>
    </message>
    <message>
      <source>Tmode first mode</source>
      <translation>Tmode 第一模式</translation>
    </message>
    <message>
      <source>Toggle automatic receiver configuration</source>
      <translation>切换自动接收器配置</translation>
    </message>
    <message>
      <source>Toggles external heading as an aiding measurement. If enabled, the filter will be configured to accept external heading as an aiding meaurement.</source>
      <translation>切换外部航向作为辅助测量。如果启用，过滤器将配置为接受外部航向作为辅助测量。</translation>
    </message>
    <message>
      <source>ToneAlarm enable/disable</source>
      <translation>音调警报启用/禁用</translation>
    </message>
    <message>
      <source>Torqeedo DE pin</source>
      <translation>Torqeedo DE 引脚</translation>
    </message>
    <message>
      <source>Torqeedo TorqLink Debug Level</source>
      <translation>Torqeedo TorqLink 调试级别</translation>
    </message>
    <message>
      <source>Total Flight Count</source>
      <translation>飞行总数</translation>
    </message>
    <message>
      <source>Total flight time of this autopilot. Lower 32 bits of the value. Flight time in microseconds = (LND_FLIGHT_T_HI &lt;&lt; 32) | LND_FLIGHT_T_LO.</source>
      <translation>该自动驾驶仪的总飞行时间。该值的低 32 位。飞行时间（微秒）= (LND_FLIGHT_T_HI &lt;&lt; 32) | LND_FLIGHT_T_LO。</translation>
    </message>
    <message>
      <source>Total number of lines for ZigZag auto if 1 or more. -1: Infinity, 0: Just moving to sideways</source>
      <translation>如果 1 或更多，则 ZigZag 自动的总行数。 -1：无穷大，0：只是向侧面移动</translation>
    </message>
    <message>
      <source>Towards Right</source>
      <translation>向右</translation>
    </message>
    <message>
      <source>Training</source>
      <translation>训练</translation>
    </message>
    <message>
      <source>Transition blending airspeed</source>
      <translation>过渡混合空速</translation>
    </message>
    <message>
      <source>Transmitter Tuning</source>
      <translation>发射机调谐</translation>
    </message>
    <message>
      <source>Transmitter tuning channel maximum pwm</source>
      <translation>发射机调谐通道最大pwm</translation>
    </message>
    <message>
      <source>Tricks on Switch Enable</source>
      <translation>开关启用技巧</translation>
    </message>
    <message>
      <source>TriggerLow</source>
      <translation>低触发</translation>
    </message>
    <message>
      <source>Trim applied to simulated accelerometer (X-axis)</source>
      <translation>应用于模拟加速度计的微调（X 轴）</translation>
    </message>
    <message>
      <source>Triple notch</source>
      <translation>三重刻痕</translation>
    </message>
    <message>
      <source>Tune Check</source>
      <translation>调音检查</translation>
    </message>
    <message>
      <source>Tuning parameter for the speed reduction based on the course error</source>
      <translation>根据航向误差调整减速参数</translation>
    </message>
    <message>
      <source>Twist time</source>
      <translation>扭转时间</translation>
    </message>
    <message>
      <source>Tx-Only</source>
      <translation>仅发送</translation>
    </message>
    <message>
      <source>Type of Type</source>
      <translation>类型 类型</translation>
    </message>
    <message>
      <source>Type of servo system being used for yaw</source>
      <translation>用于偏航的舵机系统类型</translation>
    </message>
    <message>
      <source>UART ESC ID 1 Spin Direction Flag</source>
      <translation>UART ESC ID 1 自旋方向标志</translation>
    </message>
    <message>
      <source>UART ESC Temperature Warning Threshold (Degrees C)</source>
      <translation>UART ESC 温度警告阈值（摄氏度）</translation>
    </message>
    <message>
      <source>UART ESC verbose logging</source>
      <translation>UART ESC 详细日志记录</translation>
    </message>
    <message>
      <source>UAVCAN</source>
      <translation>UAVCAN</translation>
    </message>
    <message>
      <source>UAVCAN ESC 2</source>
      <translation>UAVCAN ESC 2</translation>
    </message>
    <message>
      <source>UAVCAN ESC 3 Minimum Value</source>
      <translation>UAVCAN ESC 3 最小值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 5 Failsafe Value</source>
      <translation>UAVCAN ESC 5 故障安全值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 6 Output Function</source>
      <translation>UAVCAN ESC 6 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN ESC 8 Maximum Value</source>
      <translation>UAVCAN ESC 8 最大值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 1 Disarmed Value</source>
      <translation>UAVCAN 舵机 1 上锁值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 2 Maximum Value</source>
      <translation>UAVCAN 舵机 2 最大值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 3 Output Function</source>
      <translation>UAVCAN 舵机 3 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN Servo 5 Disarmed Value</source>
      <translation>UAVCAN 舵机 5 上锁值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 6 Maximum Value</source>
      <translation>UAVCAN 舵机 6 最大值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 7 Output Function</source>
      <translation>UAVCAN 舵机 7 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN fuel tank maximum capacity</source>
      <translation>UAVCAN油箱最大容量</translation>
    </message>
    <message>
      <source>UAVCAN-BatteryInfo</source>
      <translation>UAVCAN-电池信息</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 10 Failsafe Value</source>
      <translation>UAVCANv1 ESC 10 故障保护值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 11 Output Function</source>
      <translation>UAVCANv1 ESC 11 输出功能</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 13 Maximum Value</source>
      <translation>UAVCANv1 ESC 13 最大值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 15</source>
      <translation>UAVCANv1 ESC 15</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 16 Minimum Value</source>
      <translation>UAVCANv1 ESC 16 最小值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 3 Failsafe Value</source>
      <translation>UAVCANv1 ESC 3 故障安全值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 4 Output Function</source>
      <translation>UAVCANv1 ESC 4 输出功能</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 6 Maximum Value</source>
      <translation>UAVCANv1 ESC 6 最大值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 8</source>
      <translation>UAVCANv1 ESC 8</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 9 Minimum Value</source>
      <translation>UAVCANv1 ESC 9 最小值</translation>
    </message>
    <message>
      <source>UDRAL battery energy source subscription port ID</source>
      <translation>UDRAL 电池能源订阅端口 ID</translation>
    </message>
    <message>
      <source>UTC offset (unit: min)</source>
      <translation>UTC偏移量（单位：分钟）</translation>
    </message>
    <message>
      <source>UWB sensor positioning in relation to Drone in NED. Y offset.</source>
      <translation>UWB 传感器相对于 NED 中无人机的定位。 Y 轴偏移。</translation>
    </message>
    <message>
      <source>Unicast</source>
      <translation>单播</translation>
    </message>
    <message>
      <source>Unused</source>
      <translation>未使用</translation>
    </message>
    <message>
      <source>Upper bound of FFT frequency detection in Hz. On smaller vehicles the maximum motor frequency is likely to be significantly higher than for larger vehicles.</source>
      <translation>FFT 频率检测的上限（以 Hz 为单位）。在小型车辆上，最大电机频率可能明显高于大型车辆。</translation>
    </message>
    <message>
      <source>Use</source>
      <translation>使用</translation>
    </message>
    <message>
      <source>Use GPS for DCM position</source>
      <translation>使用 GPS 作为 DCM 位置</translation>
    </message>
    <message>
      <source>Use QRTL</source>
      <translation>使用QRTL</translation>
    </message>
    <message>
      <source>Use Throttle</source>
      <translation>使用油门</translation>
    </message>
    <message>
      <source>Use centered throttle in Cruise or FBWB to indicate trim airspeed</source>
      <translation>在巡航或 FBWB 中使用中心油门来指示配平空速</translation>
    </message>
    <message>
      <source>Use first IMU for attitude, velocity and position estimates</source>
      <translation>首先使用 IMU 进行姿态、速度和位置估计</translation>
    </message>
    <message>
      <source>Use in conjunction with disk therory gain scaling and Q_TAILSIT_DSKLD to specify minumum airspeed over control surfaces, this will be used to boost throttle, when decending for example, 0 disables</source>
      <translation>与磁盘理论增益缩放和 Q_TAILSIT_DSKLD 结合使用来指定控制面上的最小空速，这将用于提高油门，例如在下降时，0 禁用</translation>
    </message>
    <message>
      <source>Use precision landing when doing an RTL landing phase.
This setting does not apply for RTL destinations planned as part of a mission.</source>
      <translation>进行 RTL 着陆阶段时使用精确着陆。此设置不适用于计划作为任务一部分的 RTL 目的地。</translation>
    </message>
    <message>
      <source>Use terrain following</source>
      <translation>使用地形跟随</translation>
    </message>
    <message>
      <source>Use/Accept HIL GPS message even if not in HIL mode</source>
      <translation>即使不在 HIL 模式下，也使用/接受 HIL GPS 消息</translation>
    </message>
    <message>
      <source>UseHobbyWingESC</source>
      <translation>使用HobbyWingESC</translation>
    </message>
    <message>
      <source>UseWhenZeroThrottle</source>
      <translation>使用零油门</translation>
    </message>
    <message>
      <source>Used for average cell voltage display. -1 disables, 0 uses cell count autodetection for well charged LIPO/LIION batteries at connection, other values manually select cell count used.</source>
      <translation>用于平均电池电压显示。 -1 禁用，0 在连接时使用电池计数自动检测充电良好的 LIPO/LIION 电池，其他值手动选择使用的电池计数。</translation>
    </message>
    <message>
      <source>Used to calculate increased terrain random walk nosie due to movement.</source>
      <translation>用于计算由于运动而增加的地形随机游走噪声。</translation>
    </message>
    <message>
      <source>Used to cap how quickly the magnitude of yaw rate setpoints can increase. Set to -1 to disable.</source>
      <translation>用于限制偏航率设定值的增加速度。设置为 -1 以禁用。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT3_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATT3_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT7_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATT7_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATTB_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATTB_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATTF_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATTF_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to linearize mechanical output of swashplate servos to avoid axis coupling and binding with 4 servo redundancy.
This requires a symmetric setup where the servo horn is exactly centered with a 0 command.
Setting to zero disables feature.</source>
      <translation>用于线性化斜盘舵机系统的机械输出，以避免轴耦合和与 4 舵机冗余的绑定。这需要对称设置，其中舵机喇叭通过 0 命令精确居中。设置为零会禁用该功能。</translation>
    </message>
    <message>
      <source>User Function 3</source>
      <translation>用户功能 3</translation>
    </message>
    <message>
      <source>Using stick input the vehicle can be moved horizontally and yawed. The descend speed is amended: stick full up - 0 stick centered - MPC_LAND_SPEED stick full down - 2 * MPC_LAND_SPEED Manual override during auto modes has to be disabled to use this feature (see COM_RC_OVERRIDE).</source>
      <translation>使用操纵杆输入，车辆可以水平移动和偏航。下降速度已修改： 摇杆完全向上 - 0 摇杆居中 - MPC_LAND_SPEED 摇杆完全向下 - 2 * MPC_LAND_SPEED 在自动模式期间必须禁用手动超控才能使用此功能（请参阅 COM_RC_OVERRIDE）。</translation>
    </message>
    <message>
      <source>VICON_POSITION_ESTIMATE</source>
      <translation>VICON_POSITION_ESTIMATE</translation>
    </message>
    <message>
      <source>VOXL ESC Output ESC 1</source>
      <translation>VOXL ESC 输出 ESC 1</translation>
    </message>
    <message>
      <source>VOXL Power Monitor Shunt, Battery</source>
      <translation>VOXL 功率监视器分流器，电池</translation>
    </message>
    <message>
      <source>VOXL2 IO Output PWM Channel 3 Output Function</source>
      <translation>VOXL2 IO 输出 PWM 通道 3 输出功能</translation>
    </message>
    <message>
      <source>VOXL2 IO Output PWM Channel 7 Output Function</source>
      <translation>VOXL2 IO 输出 PWM 通道 7 输出功能</translation>
    </message>
    <message>
      <source>VOXL2_IO UART baud rate</source>
      <translation>VOXL2_IO UART 波特率</translation>
    </message>
    <message>
      <source>VRX BRAIN52</source>
      <translation>VRX BRAIN52</translation>
    </message>
    <message>
      <source>VSPEED_Y</source>
      <translation>VSPEED_Y</translation>
    </message>
    <message>
      <source>VTOL Mixer Thrust</source>
      <translation>VTOL 混合器推力</translation>
    </message>
    <message>
      <source>VTOL Recovery Roll Angle</source>
      <translation>VTOL 恢复侧倾角</translation>
    </message>
    <message>
      <source>VTOL Type (Tailsitter=0, Tiltrotor=1, Standard=2)</source>
      <translation>VTOL型（尾翼=0，倾转旋翼=1，标准=2）</translation>
    </message>
    <message>
      <source>VTOL_TILTROTOR</source>
      <translation>VTOL_TILTROTOR</translation>
    </message>
    <message>
      <source>Validate that AIRSPEED_STALL(if set) &lt; MIN &lt; CRUISE &lt; MAX d. 3 or less to prevent arming. -1 to disable.</source>
      <translation>验证 AIRSPEED_STALL（如果设置）&lt; MIN &lt; CRUISE &lt; MAX d． 3以下防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>Variance of the landing target measurement from the driver. Higher values result in less aggressive following of the measurement and a smoother output as well as fewer rejected measurements.</source>
      <translation>驾驶员着陆目标测量值的方差。值越高，测量的跟随性越小，输出越平滑，并且被拒绝的测量也越少。</translation>
    </message>
    <message>
      <source>VectoredYaw</source>
      <translation>矢量偏航</translation>
    </message>
    <message>
      <source>Vehicle gross weight</source>
      <translation>车辆总重</translation>
    </message>
    <message>
      <source>Vehicle waits this many seconds after completing a pivot turn before proceeding</source>
      <translation>车辆在完成枢轴转弯后等待这么多秒才继续行驶</translation>
    </message>
    <message>
      <source>Vehicle will not slow below this speed for corners.  Should be set to boat's plane speed.  Does not apply to pivot turns.</source>
      <translation>车辆在转弯时不会低于此速度。  应设置为船的平面速度。  不适用于枢轴转弯。</translation>
    </message>
    <message>
      <source>Velocity (horizontal) D gain. Corrects short-term changes in velocity. Previously _VELXY_D.</source>
      <translation>速度（水平）D 增益。纠正速度的短期变化。以前_VELXY_D。</translation>
    </message>
    <message>
      <source>Velocity (horizontal) P gain.  Determines the proportion of the target acceleration based on the velocity error.</source>
      <translation>速度（水平）P 增益。  根据速度误差确定目标加速度的比例。</translation>
    </message>
    <message>
      <source>Velocity (horizontal) input filter.  Corrects the target acceleration proportionally to the desired velocity.</source>
      <translation>速度（水平）输入滤波器。  将目标加速度与所需速度成比例地校正。</translation>
    </message>
    <message>
      <source>Velocity (horizontal) integrator maximum. Constrains the target acceleration that the I gain will output. If upgrading from 4.6 this is _VELXY_IMAX * 0.01.</source>
      <translation>速度（水平）积分器最大值。约束 I 增益将输出的目标加速度。如果从 4.6 升级，则为 _VELXY_IMAX * 0.01。</translation>
    </message>
    <message>
      <source>Velocity (vertical) controller Feed Forward gain. Produces an output that is proportional to the magnitude of the target. Previously _VELZ_FF.</source>
      <translation>速度（垂直）控制器前馈增益。产生与目标大小成比例的输出。以前是_VELZ_FF。</translation>
    </message>
    <message>
      <source>Velocity (vertical) controller P gain.  Converts the difference between desired vertical speed and actual speed into a desired acceleration that is passed to the throttle acceleration controller</source>
      <translation>速度（垂直）控制器 P 增益。  将所需垂直速度与实际速度之间的差值转换为传递至油门加速度控制器的所需加速度</translation>
    </message>
    <message>
      <source>Velocity East</source>
      <translation>速度东</translation>
    </message>
    <message>
      <source>Velocity XY kI</source>
      <translation>速度 XY kI</translation>
    </message>
    <message>
      <source>Velocity propagation noise density</source>
      <translation>速度传播噪声密度</translation>
    </message>
    <message>
      <source>Vert Pos</source>
      <translation>垂直位置</translation>
    </message>
    <message>
      <source>Vertical position of Save button on screen</source>
      <translation>屏幕上“保存”按钮的垂直位置</translation>
    </message>
    <message>
      <source>Vertical v-speed</source>
      <translation>垂直速度</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 0 Maximum Value</source>
      <translation>Vertiq IO CVI 0 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 1 Output Function</source>
      <translation>Vertiq IO CVI 1 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 11 Disarmed Value</source>
      <translation>Vertiq IO CVI 11 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 12 Maximum Value</source>
      <translation>Vertiq IO CVI 12 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 13 Output Function</source>
      <translation>Vertiq IO CVI 13 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 15 Disarmed Value</source>
      <translation>Vertiq IO CVI 15 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 2 Maximum Value</source>
      <translation>Vertiq IO CVI 2 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 3 Output Function</source>
      <translation>Vertiq IO CVI 3 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 5 Disarmed Value</source>
      <translation>Vertiq IO CVI 5 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 6 Maximum Value</source>
      <translation>Vertiq IO CVI 6 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 7 Output Function</source>
      <translation>Vertiq IO CVI 7 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 9 Disarmed Value</source>
      <translation>Vertiq IO CVI 9 上锁值</translation>
    </message>
    <message>
      <source>Very Relaxed</source>
      <translation>非常放松</translation>
    </message>
    <message>
      <source>Vicon Healthy</source>
      <translation>维康健康</translation>
    </message>
    <message>
      <source>Video Transmitter Maximum Power Level. Different VTXs support different power levels, this prevents the power aux switch from requesting too high a power level. The switch supports 6 power levels and the selected power will be a subdivision between 0 and this setting.</source>
      <translation>视频发射器最大功率级别。不同的 VTX 支持不同的功率级别，这可以防止电源辅助开关请求过高的功率级别。该开关支持 6 功率级别，所选功率将是 0 和此设置之间的细分。</translation>
    </message>
    <message>
      <source>Video stream IP Address fourth octet</source>
      <translation>视频流 IP 地址第四个八位字节</translation>
    </message>
    <message>
      <source>Video stream horizontal FOV</source>
      <translation>视频流水平 FOV</translation>
    </message>
    <message>
      <source>ViewPro Camera For Switch Low</source>
      <translation>适用于 Switch Low 的 ViewPro 摄像头</translation>
    </message>
    <message>
      <source>Vision</source>
      <translation>愿景</translation>
    </message>
    <message>
      <source>Visual odometry camera X position offset</source>
      <translation>视觉里程计相机X位置偏移</translation>
    </message>
    <message>
      <source>Visual odometry position measurement noise </source>
      <translation>视觉里程计位置测量噪声 </translation>
    </message>
    <message>
      <source>Visual odometry will only be sent to EKF if over this value. -1 to always send (even bad values), 0 to send if good or unknown</source>
      <translation>如果超过此值，视觉里程计只会发送到 EKF。 -1 始终发送（即使是错误的值），0 发送良好或未知的值</translation>
    </message>
    <message>
      <source>Voltage compensation uses raw voltage</source>
      <translation>电压补偿使用原始电压</translation>
    </message>
    <message>
      <source>Voltage type used for detection of low voltage event</source>
      <translation>用于检测低电压事件的电压类型</translation>
    </message>
    <message>
      <source>WARNING: ESC must be configured for 3D mode, and DSHOT_MIN set to 0.
This splits the throttle ranges in two.
Direction 1) 48 is the slowest, 1047 is the fastest.
Direction 2) 1049 is the slowest, 2047 is the fastest.
When mixer outputs 1000 or value inside DSHOT 3D deadband, DShot 0 is sent.</source>
      <translation>WARNING：ESC 必须配置为 3D 模式，DSHOT_MIN 设置为 0。这将油门范围一分为二。方向1）48最慢，1047最快。方向2）1049最慢，2047最快。当混频器输出 1000 或 DSHOT 3D 死区内的值时，发送 DShot 0。</translation>
    </message>
    <message>
      <source>WARNING: this will inject steps to the rate controller and can be dangerous. Only activate if you know what you are doing, and in a safe environment. Any motion of the remote stick will abort the signal injection and reset this parameter Best is to perform the identification in position or hold mode. Increase the amplitude of the injected signal using MC_AT_SYSID_AMP for more signal/noise ratio</source>
      <translation>WARNING：这会将步骤注入速率控制器，并且可能很危险。仅当您知道自己在做什么并且在安全的环境中时才激活。遥控杆的任何运动都会中止信号注入并重置此参数。最好是在位置或保持模式下执行识别。使用 MC_AT_SYSID_AMP 增加注入信号的幅度以获得更高的信噪比</translation>
    </message>
    <message>
      <source>WP Speed</source>
      <translation>WP 速度</translation>
    </message>
    <message>
      <source>Walking Height</source>
      <translation>行走高度</translation>
    </message>
    <message>
      <source>Wave amplitude</source>
      <translation>波幅</translation>
    </message>
    <message>
      <source>Wave speed in SITL</source>
      <translation>SITL 中的波速</translation>
    </message>
    <message>
      <source>Waypoint Loiter Radius</source>
      <translation>航路点盘旋半径</translation>
    </message>
    <message>
      <source>Waypoint acceleration</source>
      <translation>航点加速</translation>
    </message>
    <message>
      <source>Waypoint number to navigate to on comms loss</source>
      <translation>通讯丢失时导航至的航路点编号</translation>
    </message>
    <message>
      <source>Weathervaning gain</source>
      <translation>风向增益</translation>
    </message>
    <message>
      <source>Weight on wheels feedback pin</source>
      <translation>车轮重量反馈销</translation>
    </message>
    <message>
      <source>What provides altitude information for vehicle. Vehicle only assumes tracker has same altitude as vehicle's home</source>
      <translation>什么为车辆提供高度信息。车辆仅假设跟踪器与车辆所在的高度相同</translation>
    </message>
    <message>
      <source>What type of beacon based position estimation device is connected</source>
      <translation>连接什么类型的基于信标的位置估计设备</translation>
    </message>
    <message>
      <source>Wheel rate Error notch filter index</source>
      <translation>轮速误差陷波滤波器指数</translation>
    </message>
    <message>
      <source>Wheel rate control I gain maximum.  Constrains the output (range -1 to +1) that the I term will generate</source>
      <translation>轮速控制我获得最大。  约束 I 项将生成的输出（范围 -1 到 +1）</translation>
    </message>
    <message>
      <source>Wheel rate control error frequency in Hz</source>
      <translation>车轮速率控制误差频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Wheel steering rate integrator gain</source>
      <translation>车轮转向率积分器增益</translation>
    </message>
    <message>
      <source>Wheel2's Y position offset</source>
      <translation>Wheel2的Y位置偏移</translation>
    </message>
    <message>
      <source>WheelEncoder type</source>
      <translation>轮编码器类型</translation>
    </message>
    <message>
      <source>When closer than this distance assume we track by heading</source>
      <translation>当比这个距离更近时，假设我们按航向跟踪</translation>
    </message>
    <message>
      <source>When enabled, logging will not start from boot if battery power is not detected (e.g. powered via USB on a test bench). This prevents extraneous flight logs from being created during bench testing. Note that this only applies to log-from-boot modes. This has no effect on arm-based modes.</source>
      <translation>启用后，如果未检测到电池电源（例如通过测试台上的 USB 供电），记录将不会从启动开始。这可以防止在台架测试期间创建无关的飞行日志。请注意，这仅适用于启动日志模式。这对基于 Arm 的模式没有影响。</translation>
    </message>
    <message>
      <source>When enabled, this adds user stick input to the control surfaces in auto modes, allowing the user to have some degree of flight control without changing modes.  There are two types of stick mixing available. If you set STICK_MIXING to 1 then it will use "fly by wire" mixing, which controls the roll and pitch in the same way that the FBWA mode does. This is the safest option if you usually fly ArduPlane in FBWA or FBWB mode. If you set STICK_MIXING to 3 then it will apply to the yaw while in quadplane modes only, such as while doing an automatic VTOL takeoff or landing.</source>
      <translation>启用后，这会在自动模式下将用户摇杆输入添加到控制表面，从而允许用户在不更改模式的情况下进行一定程度的飞行控制。  有两种类型的棒式混合可供选择。如果将 STICK_MIXING 设置为 1，那么它将使用“线控飞行”混合，其控制横滚和俯仰的方式与 FBWA 模式相同。如果您通常在 FBWA 或 FBWB 模式下飞行 ArduPlane，这是最安全的选择。如果将 STICK_MIXING 设置为 3，则它将仅适用于四平面模式下的偏航，例如在进行自动 VTOL 起飞或着陆时。</translation>
    </message>
    <message>
      <source>When flying</source>
      <translation>飞行时</translation>
    </message>
    <message>
      <source>When multiple height sources are enabled at the same time, the height estimate will always converge towards the reference height source selected by this parameter. The range sensor and vision options should only be used when for operation over a flat surface as the local NED origin will move up and down with ground level. If GPS is set as reference but altitude fusion is disabled in EKF2_GPS_CTRL, the GPS altitude is still used to initiaize the bias of the other height sensors.</source>
      <translation>当同时启用多个高度源时，高度估计将始终向该参数选择的参考高度源收敛。范围传感器和视觉选项仅应在平坦表面上操作时使用，因为本地 NED 原点将随地面上下移动。如果将 GPS 设置为参考，但在 EKF2_GPS_CTRL 中禁用高度融合，则 GPS 高度仍用于初始化其他高度传感器的偏置。</translation>
    </message>
    <message>
      <source>When non-zero this sets the time with no transmitter packets before we start looking for auto-bind packets.</source>
      <translation>当非零时，这会设置在我们开始寻找自动绑定数据包之前没有发送器数据包的时间。</translation>
    </message>
    <message>
      <source>When performing an autonomus landing, this value is used as the goal airspeed during approach.  Max airspeed allowed is Trim Airspeed or ARSPD_FBW_MAX as defined by LAND_OPTIONS bitmask.  Note that this parameter is not useful if your platform does not have an airspeed sensor (use TECS_LAND_THR instead).  If negative then this value is not used during landing.</source>
      <translation>执行自主着陆时，该值用作进场时的目标空速。  允许的最大空速是调整空速或由 LAND_OPTIONS 位掩码定义的 ARSPD_FBW_MAX。  请注意，如果您的平台没有空速传感器（请改用 TECS_LAND_THR），则此参数没有用处。  如果为负值，则着陆期间不使用该值。</translation>
    </message>
    <message>
      <source>When set to 0 this script will not run. When set to 1 this script will run.</source>
      <translation>当设置为 0 时，该脚本将不会运行。当设置为 1 时，该脚本将运行。</translation>
    </message>
    <message>
      <source>When the actuator_output is between DSHOT_3D_DEAD_L and DSHOT_3D_DEAD_H, motor will not spin.
This value is with respect to the mixer_module range (0-1999), not the DSHOT values.</source>
      <translation>当actuator_output在DSHOT_3D_DEAD_L和DSHOT_3D_DEAD_H之间时，电机不会旋转。该值是相对于 Mixer_module 范围 (0-1999) 的，而不是 DSHOT 值。</translation>
    </message>
    <message>
      <source>When this is enabled all the hardfaults on the SD card are streamed
over MAVLink. This is useful for cases where the FMU does reset in-flight due
to a hardfault and the SD card may not survive a crash.</source>
      <translation>启用此功能后，SD 卡上的所有硬故障都会通过 MAVLink 进行流式传输。这对于 FMU 由于硬故障而在飞行中重置并且 SD 卡可能无法在崩溃中幸存的情况非常有用。</translation>
    </message>
    <message>
      <source>WhenNoYawSensor</source>
      <translation>当没有偏航传感器时</translation>
    </message>
    <message>
      <source>Which navigation controller to enable. Currently the only navigation controller available is L1. From time to time other experimental controllers will be added which are selected using this parameter.</source>
      <translation>要启用哪个导航控制器。目前唯一可用的导航控制器是 L1。有时会添加使用此参数选择的其他实验控制器。</translation>
    </message>
    <message>
      <source>Will provide a fast idle for engine cooldown by raising the Ground Idle speed setting by 50% for the number of seconds the timer is set for. A setting of zero disables the fast idle. This feature will only apply after the runup complete has been declared. This will not extend the time before ground idle is declared, which triggers engine shutdown for autonomous landings.</source>
      <translation>通过提高 50% 的地面怠速设置定时器设置的秒数，可以为发动机冷却提供快速怠速。设置为零会禁用快速空闲。此功能仅在声明启动完成后才适用。这不会延长宣布地面闲置之前的时间，地面闲置会触发发动机关闭以进行自主着陆。</translation>
    </message>
    <message>
      <source>Winch control D gain.  Compensates for short-term change in desired rate vs actual</source>
      <translation>绞车控制 D 增益。  补偿期望利率与实际利率的短期变化</translation>
    </message>
    <message>
      <source>Winch control rate P gain</source>
      <translation>绞车控制率 P 增益</translation>
    </message>
    <message>
      <source>WinchControl Rate Up</source>
      <translation>绞车控制速率提高</translation>
    </message>
    <message>
      <source>Wind coefficient left</source>
      <translation>左风系数</translation>
    </message>
    <message>
      <source>Wind process noise of the internal wind estimator(s) of the airspeed selector. When unaided, the wind estimate uncertainty (1-sigma, in m/s) increases by this amount every second.</source>
      <translation>空速选择器内部风估计器的风过程噪声。在没有辅助的情况下，风估计不确定性（1-sigma，以 m/s 为单位）每秒都会增加此量。</translation>
    </message>
    <message>
      <source>Wind speed sensor type</source>
      <translation>风速传感器类型</translation>
    </message>
    <message>
      <source>Wind vane direction low pass filter frequency</source>
      <translation>风向标方向低通滤波器频率</translation>
    </message>
    <message>
      <source>Wind vane voltage minimum</source>
      <translation>风向标电压最小值</translation>
    </message>
    <message>
      <source>Window for comparing airspeed change to throttle and pitch change. Triggers when the airspeed change within this window is negative while throttle increases and the vehicle pitches down. Is meant to catch degrading airspeed blockages as can happen when flying through icing conditions. Relies on  FW_THR_TRIM being set accurately.</source>
      <translation>用于将空速变化与油门和俯仰变化进行比较的窗口。当此窗口内的空速变化为负且油门增加且飞行器向下俯仰时触发。旨在捕捉在结冰条件下飞行时可能发生的降低空速阻塞的情况。依赖于 FW_THR_TRIM 的准确设置。</translation>
    </message>
    <message>
      <source>X</source>
      <translation>X</translation>
    </message>
    <message>
      <source>X position of optical flow focal point in body frame</source>
      <translation>光流焦点在机身框架中的X位置</translation>
    </message>
    <message>
      <source>X position of the first GPS antenna in body frame. Positive X is forward of the origin. Use antenna phase centroid location if provided by the manufacturer.</source>
      <translation>机身框架中第一根 GPS 天线的 X 位置。正 X 位于原点前方。如果制造商提供，请使用天线相位质心位置。</translation>
    </message>
    <message>
      <source>X position of the second GPS antenna in body frame. Positive X is forward of the origin. Use antenna phase centroid location if provided by the manufacturer.Renamed in 4.6 and later to GPS2_POS_X.</source>
      <translation>机身框架中第二个 GPS 天线的 X 位置。正 X 位于原点前方。如果制造商提供，请使用天线相位质心位置。在 4.6 中重命名，后来更名为 GPS2_POS_X。</translation>
    </message>
    <message>
      <source>X-axis ballistic coefficient used for multi-rotor wind estimation</source>
      <translation>用于多旋翼风估计的X轴弹道系数</translation>
    </message>
    <message>
      <source>XYZ position of the IMU accelerometer relative to the body frame origin (Y-axis)</source>
      <translation>XYZ IMU 加速度计相对于机体框架原点（Y 轴）的位置</translation>
    </message>
    <message>
      <source>Xacti</source>
      <translation>克克斯蒂</translation>
    </message>
    <message>
      <source>Y position of range finder origin in body frame</source>
      <translation>测距仪原点在机身框架中的 Y 位置</translation>
    </message>
    <message>
      <source>Y position of the first GPS antenna in body frame. Positive Y is to the right of the origin. Use antenna phase centroid location if provided by the manufacturer.Renamed in 4.6 and later to GPS1_POS_Y.</source>
      <translation>机身框架中第一个 GPS 天线的 Y 位置。正 Y 位于原点右侧。如果制造商提供，请使用天线相位质心位置。在 4.6 中重命名，后来更名为 GPS1_POS_Y。</translation>
    </message>
    <message>
      <source>Y position of the second IMU accelerometer in body frame. Positive Y is to the right of the origin. Attention: The IMU should be located as close to the vehicle c.g. as practical so that the value of this parameter is minimised. Failure to do so can result in noisy navigation velocity measurements due to vibration and IMU gyro noise. If the IMU cannot be moved and velocity noise is a problem, a location closer to the IMU can be used as the body frame origin.</source>
      <translation>机体框架中第二个 IMU 加速度计的 Y 位置。正 Y 位于原点右侧。注意：IMU 应尽可能靠近车辆 c.g.。根据实际情况，使该参数的值最小化。否则，可能会因振动和 IMU 陀螺仪噪声而导致导航速度测量产生噪声。如果 IMU 无法移动并且速度噪声成为问题，则可以使用更靠近 IMU 的位置作为机体框架原点。</translation>
    </message>
    <message>
      <source>Y6B</source>
      <translation>Y6B</translation>
    </message>
    <message>
      <source>Yaw 45°</source>
      <translation>偏航45°</translation>
    </message>
    <message>
      <source>Yaw P gain</source>
      <translation>偏航 P 增益</translation>
    </message>
    <message>
      <source>Yaw acceleration limit</source>
      <translation>偏航加速度限制</translation>
    </message>
    <message>
      <source>Yaw axis controller I gain maximum</source>
      <translation>偏航轴控制器 I 增益最大</translation>
    </message>
    <message>
      <source>Yaw axis controller error frequency in Hz</source>
      <translation>偏航轴控制器错误频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Yaw axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output</source>
      <translation>偏航轴速率控制器 I 增益最大。  限制 I 增益将输出的最大电机输出</translation>
    </message>
    <message>
      <source>Yaw axis rate controller PD sum maximum.  The maximum/minimum value that the sum of the P and D term can output</source>
      <translation>偏航轴速率控制器 PD 总和最大值。  P和D项之和可以输出的最大/最小值</translation>
    </message>
    <message>
      <source>Yaw behaviour during orbit flight</source>
      <translation>轨道飞行期间的偏航行为</translation>
    </message>
    <message>
      <source>Yaw deceleration limit</source>
      <translation>偏航减速度限制</translation>
    </message>
    <message>
      <source>Yaw gain multiplier active when landed. A factor of 1.0 means no reduction in gain while landed. Reduce this factor to reduce ground oscitation in the yaw axis.</source>
      <translation>偏航增益乘数在着陆时激活。 1.0 系数意味着着陆时增益不会减少。减小该系数可减少偏航轴上的地面振荡。</translation>
    </message>
    <message>
      <source>Yaw rate I gain</source>
      <translation>我获得的偏航率</translation>
    </message>
    <message>
      <source>Yaw rate feed forward</source>
      <translation>偏航率前馈</translation>
    </message>
    <message>
      <source>Yaw rate lower limit</source>
      <translation>横摆率下限</translation>
    </message>
    <message>
      <source>Yaw slew rate limit</source>
      <translation>偏航摆率限制</translation>
    </message>
    <message>
      <source>Yaw135</source>
      <translation>偏航135</translation>
    </message>
    <message>
      <source>Yaw270Pitch180</source>
      <translation>偏航270俯仰180</translation>
    </message>
    <message>
      <source>Yaw45Roll180</source>
      <translation>偏航45横滚180</translation>
    </message>
    <message>
      <source>Yaw90Roll90</source>
      <translation>偏航90横滚90</translation>
    </message>
    <message>
      <source>Z position of IMU in body frame</source>
      <translation>IMU在机体框架中的Z位置</translation>
    </message>
    <message>
      <source>Z position of the center of the second wheel in body frame. Positive Z is down from the origin.</source>
      <translation>机体框架中第二轮中心的 Z 位置。正 Z 轴从原点向下。</translation>
    </message>
    <message>
      <source>Z position of the rangefinder in body frame. Positive Z is down from the origin. Use the zero range datum point if supplied.</source>
      <translation>测距仪在机身框架中的 Z 位置。正 Z 轴从原点向下。使用零范围基准点（如果提供）。</translation>
    </message>
    <message>
      <source>Zero lift drag coef.</source>
      <translation>零升力阻力系数。</translation>
    </message>
    <message>
      <source>ZigZag auto enable/disable</source>
      <translation>ZigZag 自动启用/禁用</translation>
    </message>
    <message>
      <source>a3 in polynomial of form temperature in deg = a0 + a1*voltage + a2*voltage^2 + a3*voltage^3 + a4*voltage^4</source>
      <translation>a3 以度为单位的温度多项式 = a0 + a1*电压 + a2*电压^2 + a3*电压^3 + a4*电压^4</translation>
    </message>
    <message>
      <source>actuator_1_inc</source>
      <translation>执行器_1_inc</translation>
    </message>
    <message>
      <source>actuator_2_dec</source>
      <translation>执行器_2_dec</translation>
    </message>
    <message>
      <source>actuator_3_center</source>
      <translation>执行器_3_center</translation>
    </message>
    <message>
      <source>actuator_3_min_toggle</source>
      <translation>Actuator_3_min_toggle</translation>
    </message>
    <message>
      <source>actuator_4_min_momentary</source>
      <translation>Actuator_4_min_momentary</translation>
    </message>
    <message>
      <source>actuator_5_min</source>
      <translation>执行器_5_min</translation>
    </message>
    <message>
      <source>actuator_6_max_toggle</source>
      <translation>Actuator_6_max_toggle</translation>
    </message>
    <message>
      <source>airspeed in the drag reference test, at a fixed lean angle</source>
      <translation>阻力参考测试中的空速，固定倾斜角</translation>
    </message>
    <message>
      <source>apparent Wind speed low pass filter frequency, a value of -1 disables filter</source>
      <translation>表观风速低通滤波器频率，-1 的值禁用滤波器</translation>
    </message>
    <message>
      <source>auxilliary value to kill throttle</source>
      <translation>辅助值杀死油门</translation>
    </message>
    <message>
      <source>battery capacity</source>
      <translation>电池容量</translation>
    </message>
    <message>
      <source>bias of simulated accelerometer sensor (Z-axis)</source>
      <translation>模拟加速度传感器的偏置（Z轴）</translation>
    </message>
    <message>
      <source>ceiling for pitching/quading</source>
      <translation>俯仰/四边形上限</translation>
    </message>
    <message>
      <source>current battery cycles</source>
      <translation>当前电池循环次数</translation>
    </message>
    <message>
      <source>cylinder</source>
      <translation>气缸</translation>
    </message>
    <message>
      <source>disable receive CRC</source>
      <translation>禁止接收CRC</translation>
    </message>
    <message>
      <source>down distance minimum for Pitching</source>
      <translation>投球的最小向下距离</translation>
    </message>
    <message>
      <source>e.g. in Missions, RTL, Goto if the waypoint does not specify differently</source>
      <translation>例如在任务中，RTL，如果航路点没有指定不同，则转到</translation>
    </message>
    <message>
      <source>enable web server</source>
      <translation>启用网络服务器</translation>
    </message>
    <message>
      <source>external navigation system measurement delay (msec)</source>
      <translation>外部导航系统测量延迟（毫秒）</translation>
    </message>
    <message>
      <source>forward distance minimum for Pitching</source>
      <translation>投球前向最小距离</translation>
    </message>
    <message>
      <source>fuse GPS, requires GPS for alt. init</source>
      <translation>保险丝 GPS，备用需要 GPS。初始化</translation>
    </message>
    <message>
      <source>gain_inc</source>
      <translation>增益公司</translation>
    </message>
    <message>
      <source>horiz pos error</source>
      <translation>水平位置错误</translation>
    </message>
    <message>
      <source>input_hold_toggle</source>
      <translation>输入保持切换</translation>
    </message>
    <message>
      <source>lights1_dimmer</source>
      <translation>灯1_调光器</translation>
    </message>
    <message>
      <source>log every sample</source>
      <translation>记录每个样本</translation>
    </message>
    <message>
      <source>manual control</source>
      <translation>手动控制</translation>
    </message>
    <message>
      <source>max battery cycles for arming</source>
      <translation>解锁的最大电池周期</translation>
    </message>
    <message>
      <source>maximum angular rate in deg/s allowed during autotune maneuvers</source>
      <translation>自动调谐操作期间允许的最大角速率（以度/秒为单位）</translation>
    </message>
    <message>
      <source>microSD slowdown</source>
      <translation>microSD 速度减慢</translation>
    </message>
    <message>
      <source>minimum motor PWM in the reference test, matching MOT_PWM_MIN</source>
      <translation>参考测试中最小电机PWM，匹配MOT_PWM_MIN</translation>
    </message>
    <message>
      <source>mode_poshold</source>
      <translation>模式位置</translation>
    </message>
    <message>
      <source>moment of inertia in yaw</source>
      <translation>偏航惯性矩</translation>
    </message>
    <message>
      <source>monitor 6</source>
      <translation>监控6</translation>
    </message>
    <message>
      <source>motor thrust</source>
      <translation>电机推力</translation>
    </message>
    <message>
      <source>mount_open</source>
      <translation>挂载打开</translation>
    </message>
    <message>
      <source>no logging</source>
      <translation>没有记录</translation>
    </message>
    <message>
      <source>output channel to change on throttle kill</source>
      <translation>油门关闭时改变输出通道</translation>
    </message>
    <message>
      <source>pin number to use for save accurate camera feedback messages. If set to -1 then don't use a pin flag for this, otherwise this is a pin number which if held high after a picture trigger order, will save camera messages when camera really takes a picture. A universal camera hot shoe is needed. The pin should be held high for at least 2 milliseconds for reliable trigger detection.  Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot. See also the CAM_FEEDBACK_POL option.</source>
      <translation>用于保存准确的相机反馈消息的引脚号。如果设置为 -1，则不要为此使用引脚标志，否则这是一个引脚编号，如果在图片触发命令后保持高电平，将在相机真正拍照时保存相机消息。需要一个通用相机热靴。该引脚应保持高电平至少 2 毫秒，以实现可靠的触发检测。  给出了一些常见值，但请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。另请参见 CAM_FEEDBACK_POL 选项。</translation>
    </message>
    <message>
      <source>position error</source>
      <translation>位置误差</translation>
    </message>
    <message>
      <source>radio debug level</source>
      <translation>无线电调试级别</translation>
    </message>
    <message>
      <source>reference angle</source>
      <translation>参考角</translation>
    </message>
    <message>
      <source>relay_1_on</source>
      <translation>继电器_1_on</translation>
    </message>
    <message>
      <source>relay_3_on</source>
      <translation>继电器_3_on</translation>
    </message>
    <message>
      <source>right</source>
      <translation>对</translation>
    </message>
    <message>
      <source>rudder differential thrust gain</source>
      <translation>舵差推力增益</translation>
    </message>
    <message>
      <source>scaling factor for simulated vibration from motors</source>
      <translation>电机模拟振动的比例因子</translation>
    </message>
    <message>
      <source>script_1</source>
      <translation>脚本_1</translation>
    </message>
    <message>
      <source>sendfile is an offloading mechanism for faster file download. If this is non-zero and the file is larger than this size then sendfile will be used for file download</source>
      <translation>sendfile 是一种用于加快文件下载速度的卸载机制。如果该值非零且文件大于此大小，则将使用 sendfile 进行文件下载</translation>
    </message>
    <message>
      <source>servo_1_dec</source>
      <translation>舵机_1_dec</translation>
    </message>
    <message>
      <source>servo_2_center</source>
      <translation>舵机_2_center</translation>
    </message>
    <message>
      <source>servo_2_min_toggle</source>
      <translation>servo_2_min_toggle</translation>
    </message>
    <message>
      <source>servo_3_min_momentary</source>
      <translation>舵机_3_min_momentary</translation>
    </message>
    <message>
      <source>simulated GPS number of satellites used</source>
      <translation>模拟GPS使用的卫星数量</translation>
    </message>
    <message>
      <source>speed error</source>
      <translation>速度误差</translation>
    </message>
    <message>
      <source>subscription GPS Relative</source>
      <translation>认购 GPS 相对</translation>
    </message>
    <message>
      <source>subscription flow</source>
      <translation>订阅流程</translation>
    </message>
    <message>
      <source>tail into wind</source>
      <translation>尾随风</translation>
    </message>
    <message>
      <source>terrain brake enable</source>
      <translation>地形制动启用</translation>
    </message>
    <message>
      <source>thermal image colour palette</source>
      <translation>热图像调色板</translation>
    </message>
    <message>
      <source>throttle kill default value. The default auxilliary function position on boot</source>
      <translation>节流终止默认值。开机默认辅助功能位置</translation>
    </message>
    <message>
      <source>tilt (pitch) RC input channel</source>
      <translation>倾斜（俯仰）RC输入通道</translation>
    </message>
    <message>
      <source>total rotor disc area. Coaxial rotors count as one rotor only</source>
      <translation>转子盘总面积。同轴转子仅算作一个转子</translation>
    </message>
    <message>
      <source>trim_pitch_inc</source>
      <translation>微调间距增量</translation>
    </message>
    <message>
      <source>u-blox GPS Mode</source>
      <translation>u-blox GPS 模式</translation>
    </message>
    <message>
      <source>u-blox receivers support different dynamic platform models to adjust the navigation engine to the expected application environment.</source>
      <translation>u-blox 接收器支持不同的动态平台模型，以将导航引擎调整到预期的应用环境。</translation>
    </message>
    <message>
      <source>uXRCE-DDS Agent IP address</source>
      <translation>uXRCE-DDS代理IP地址</translation>
    </message>
    <message>
      <source>unused</source>
      <translation>未使用</translation>
    </message>
    <message>
      <source>use speed accuracy</source>
      <translation>使用速度精度</translation>
    </message>
    <message>
      <source>web server minimum file size for sendfile</source>
      <translation>Web 服务器发送文件的最小文件大小</translation>
    </message>
  </context>
</TS>
