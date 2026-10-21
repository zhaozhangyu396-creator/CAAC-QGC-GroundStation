<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en">
  <context>
    <name>VehicleParameters</name>
    <message>
      <source> Backup EKF origin altitude (MSL) used when not using a positioning system.</source>
      <translation>不使用定位系统时使用的备份EKF原点高度（MSL）。</translation>
    </message>
    <message>
      <source> Initial increment step for changing the actuator's PWM</source>
      <translation> 更改执行器 PWM 的初始增量步长</translation>
    </message>
    <message>
      <source>"Superexponential" factor for refining the input curve shape tuned using MC_ACRO_EXPO_Y.
0 Pure Expo function
0.7 reasonable shape enhancement for intuitive stick feel
0.95 very strong bent input curve only near maxima have effect</source>
      <translation>用于细化使用 MC_ACRO_EXPO_Y 调整的输入曲线形状的“超指数”因子。 0 Pure Expo功能 0.7 合理的形状增强，带来直观的棒感 0.95 非常强的弯曲输入曲线，仅在最大值附近才有效果</translation>
    </message>
    <message>
      <source>(Vectored frame only) Used to decouple pitch from forward/vertical motion. 0 to disable, 1.2 normal</source>
      <translation>（仅限矢量帧）用于将俯仰与向前/垂直运动解耦。 0禁用，1.2正常</translation>
    </message>
    <message>
      <source>- Enabled</source>
      <translation>- 启用</translation>
    </message>
    <message>
      <source>-1 = Will not be configured, and will use the device default range. Ranges vary by device and map to integer codes. Check the device's [User Manual](https://www.hbkworld.com/en/products/transducers/inertial-sensors#!ref_microstrain.com) for supported ranges and set the corresponding integer.</source>
      <translation>-1 = 不会配置，并将使用器件默认范围。范围因设备而异，并映射到整数代码。检查设备的[用户手册](https://www.hbkworld.com/en/products/transducers/inertial-sensors#!ref_microstrain.com))支持的范围并设置相应的整数。</translation>
    </message>
    <message>
      <source>0 disables the failsafe. 1 enables failsafe on loss of RC input. This is detected either by the throttle channel reaching THR_FS_VALUE (at or below it, or at or above it for a reversed throttle channel), loss of receiver valid pulses/data, or by the FS bit in receivers that provide it, like SBUS. A programmable failsafe action will occur and RC inputs, if present, will be ignored. A value of 2 means that the RC inputs won't be used when RC failsafe is detected by any of the above methods, but it won't trigger an RC failsafe action.</source>
      <translation>0 禁用故障保护。 1 在 RC 输入丢失时启用故障保护。这可以通过油门通道达到 THR_FS_VALUE（在其处或之下，或者在反向油门通道处在其处或之上）、接收器有效脉冲/数据丢失或通过提供该信号的接收器（如 SBUS）中的 FS 位来检测。将发生可编程故障安全操作，并且 RC 输入（如果存在）将被忽略。 2 值意味着当通过上述任何方法检测到 RC 故障安全时，将不会使用 RC 输入，但不会触发 RC 故障安全操作。</translation>
    </message>
    <message>
      <source>0-1 indicate where in the full channel range the threshold sits 0 : min 1 : max sign indicates polarity of comparison positive : true when channel&gt;th negative : true when channel&lt;th</source>
      <translation>0-1 指示阈值在整个通道范围内的位置 0 : 最小值 1 : 最大符号指示比较的极性 正：当通道&gt;第一个时为真 负：当通道&lt;第一个时为真</translation>
    </message>
    <message>
      <source>0: Disallow disarming when not landed
1: Allow disarming in multicopter flight in modes where
the thrust is directly controlled by thr throttle stick
e.g. Stabilized, Acro</source>
      <translation>0：未着陆时不允许上锁 1：允许在多旋翼飞行中以油门杆直接控制推力的模式下上锁，例如稳定，Acro</translation>
    </message>
    <message>
      <source>0: no logging, 1: log only special ID, 2:log all</source>
      <translation>0：不记录，1：仅记录特殊 ID，2：记录全部</translation>
    </message>
    <message>
      <source>0x81</source>
      <translation>0x81</translation>
    </message>
    <message>
      <source>1 byte bitmap controlling use of sideslip angle fusion for estimation of non wind states during operation of 'fly forward' vehicle types such as fixed wing planes. By assuming that the angle of sideslip is small, the wind velocity state estimates are corrected  whenever the EKF is not dead reckoning (e.g. has an independent velocity or position sensor such as GPS). This behaviour is on by default and cannot be disabled. When the EKF is dead reckoning, the wind states are used as a reference, enabling use of the small angle of sideslip assumption to correct non wind velocity states (eg attitude, velocity, position, etc) and improve navigation accuracy. This behaviour is on by default and cannot be disabled. The behaviour controlled by this parameter is the use of the small angle of sideslip assumption to correct non wind velocity states when the EKF is NOT dead reckoning. This is primarily of benefit to reduce the buildup of yaw angle errors during straight and level flight without a yaw sensor (e.g. magnetometer or dual antenna GPS yaw) provided aerobatic flight maneuvers with large sideslip angles are not performed. The 'always' option might be used where the yaw sensor is intentionally not fitted or disabled. The 'WhenNoYawSensor' option might be used if a yaw sensor is fitted, but protection against in-flight failure and continual rejection by the EKF is desired. For vehicles operated within visual range of the operator performing frequent turning maneuvers, setting this parameter is unnecessary.</source>
      <translation>1 字节位图控制使用侧滑角融合来估计“向前飞行”飞行器类型（例如固定翼飞机）操作期间的非风状态。通过假设侧滑角很小，只要 EKF 没有航位推算（例如具有独立的速度或位置传感器，如 GPS），就会对风速状态估计进行校正。此行为默认启用且无法禁用。 EKF航位推算时，以风态为参考，可以利用小角侧滑假设来修正非风速状态（如姿态、速度、位置等），提高导航精度。此行为默认启用且无法禁用。该参数控制的行为是当EKF为NOT航位推算时，利用小侧滑角假设来修正非风速状态。这主要有利于减少在没有偏航传感器（例如磁力计或双天线 GPS 偏航）的直线和水平飞行期间偏航角误差的累积，前提是不执行具有大侧滑角的特技飞行机动。当故意不安装或禁用偏航传感器时，可以使用“始终”选项。如果安装了偏航传感器，则可以使用“WhenNoYawSensor”选项，但需要防止飞行中故障和 EKF 的持续拒绝。对于在操作员可视范围内进行频繁转弯操作的车辆，无需设置此参数。</translation>
    </message>
    <message>
      <source>1 byte bitmap of which EKF3 instances run an independant EKF-GSF yaw estimator to provide a backup yaw estimate that doesn't rely on magnetometer data. This estimator uses IMU, GPS and, if available, airspeed data. EKF-GSF yaw estimator data for the primary EKF3 instance will be logged as GSF0 and GSF1 messages. Use of the yaw estimate generated by this algorithm is controlled by the EK3_GSF_USE, EK3_GSF_DELAY and EK3_GSF_MAXCOUNT parameters. To run the EKF-GSF yaw estimator in ride-along and logging only, set EK3_GSF_USE to 0. </source>
      <translation>1 字节位图，其中 EKF3 实例运行独立的 EKF-GSF 偏航估计器，以提供不依赖于磁力计数据的备份偏航估计。该估算器使用 IMU、GPS 以及空速数据（如果可用）。主 EKF3 实例的 EKF-GSF 偏航估计器数据将记录为 GSF0 和 GSF1 消息。该算法生成的偏航估计的使用由 EK3_GSF_USE、EK3_GSF_DELAY 和 EK3_GSF_MAXCOUNT 参数控制。要仅在行驶和记录中运行 EKF-GSF 偏航估算器，请将 EK3_GSF_USE 设置为 0。 </translation>
    </message>
    <message>
      <source>1-sigma tilt angle uncertainty after gravity vector alignment</source>
      <translation>重力矢量对准后的 1-sigma 倾斜角不确定度</translation>
    </message>
    <message>
      <source>100Hz</source>
      <translation>100赫兹</translation>
    </message>
    <message>
      <source>115200 8N1</source>
      <translation>115200 8N1</translation>
    </message>
    <message>
      <source>12S Battery</source>
      <translation>12S电池</translation>
    </message>
    <message>
      <source>14th harmonic</source>
      <translation>14次谐波</translation>
    </message>
    <message>
      <source>16S Battery</source>
      <translation>16S电池</translation>
    </message>
    <message>
      <source>1M</source>
      <translation>1M</translation>
    </message>
    <message>
      <source>2-byte bitmask to select what tuning should be performed.  Max gain automatically performed if Rate D is selected. Values: 7:All,1:VFF Only,2:Rate D/Rate P Only(incl max gain),4:Angle P Only,8:Max Gain Only,16:Tune Check,3:VFF and Rate D/Rate P(incl max gain),5:VFF and Angle P,6:Rate D/Rate P(incl max gain) and angle P</source>
      <translation>2 字节位掩码，用于选择应执行的调整。  如果选择速率 D，则自动执行最大增益。值：7：全部、1：仅 VFF、2：仅速率 D/速率 P（包括最大增益）、4：仅角度 P、8：仅最大增益、16：调谐检查、3：VFF 和速率 D/速率 P（包括最大增益）、5：VFF 和角度P，6：速率D/速率P（包括最大增益）和角度P</translation>
    </message>
    <message>
      <source>2000hz</source>
      <translation>2000hz</translation>
    </message>
    <message>
      <source>250 Hz</source>
      <translation>250 Hz</translation>
    </message>
    <message>
      <source>2D Counter Clockwise</source>
      <translation>2D 逆时针</translation>
    </message>
    <message>
      <source>2nd GPS type</source>
      <translation>第二GPS型</translation>
    </message>
    <message>
      <source>3.125 Hz</source>
      <translation>3.125 Hz</translation>
    </message>
    <message>
      <source>370 Hz</source>
      <translation>370 Hz</translation>
    </message>
    <message>
      <source>3D RTK Float</source>
      <translation>3D RTK 浮点</translation>
    </message>
    <message>
      <source>3rd Sensor</source>
      <translation>第三传感器</translation>
    </message>
    <message>
      <source>45 degrees half cone angle.</source>
      <translation>45度半锥角。</translation>
    </message>
    <message>
      <source>4th harmonic</source>
      <translation>第四次谐波</translation>
    </message>
    <message>
      <source>500hz</source>
      <translation>500hz</translation>
    </message>
    <message>
      <source>5M</source>
      <translation>5M</translation>
    </message>
    <message>
      <source>60x22</source>
      <translation>60x22</translation>
    </message>
    <message>
      <source>738 Hz</source>
      <translation>738 Hz</translation>
    </message>
    <message>
      <source>800 Hz</source>
      <translation>800 Hz</translation>
    </message>
    <message>
      <source>9600 8N1</source>
      <translation>9600 8N1</translation>
    </message>
    <message>
      <source>A bitmask to set some MSP specific options: EnableTelemetryMode-allows "push" mode telemetry when only rx line of OSD ic connected to autopilot,  EnableBTFLFonts-uses indexes corresponding to Betaflight fonts if OSD uses those instead of ArduPilot fonts.</source>
      <translation>用于设置一些 MSP 特定选项的位掩码：EnableTelemetryMode - 当仅 OSD ic 的 rx 线连接到自动驾驶仪时允许“推送”模式遥测，EnableBTFLFonts - 如果 OSD 使用 Betaflight 字体而不是 ArduPilot 字体，则使用与 Betaflight 字体相对应的索引。</translation>
    </message>
    <message>
      <source>A fraction [0,1] deprioritizing yaw compared to roll and pitch in non-linear attitude control. Deprioritizing yaw is necessary because multicopters have much less control authority in yaw compared to the other axes and it makes sense because yaw is not critical for stable hovering or 3D navigation. For yaw control tuning use SC_YAW_P. This ratio has no impact on the yaw gain.</source>
      <translation>在非线性姿态控制中，与横滚和俯仰相比，偏航的分数 [0,1] 降低了优先级。取消偏航优先级是必要的，因为与其他轴相比，多旋翼飞行器对偏航的控制权限要小得多，而且这是有道理的，因为偏航对于稳定悬停或 3D 导航并不重要。对于偏航控制调整，请使用 SC_YAW_P。该比率对偏航增益没有影响。</translation>
    </message>
    <message>
      <source>A pressure change with temperature that closely matches what has been observed with a ICM-20789</source>
      <translation>压力随温度的变化与 ICM-20789 观察到的情况非常吻合</translation>
    </message>
    <message>
      <source>A temperature increase greater than this value is required during calibration. Calibration will complete for each sensor when the temperature increase above the starting temperature exceeds the value set by SYS_CAL_TDEL. If the temperature rise is insufficient, the calibration will continue indefinitely and the board will need to be repowered to exit.</source>
      <translation>校准期间需要增加大于该值的温度。当起始温度以上的温度升高超过 SYS_CAL_TDEL 设置的值时，每个传感器的校准将完成。如果温升不足，校准将无限期地继续，并且板需要重新上电才能退出。</translation>
    </message>
    <message>
      <source>A11</source>
      <translation>A11</translation>
    </message>
    <message>
      <source>ACCEL2 Failure</source>
      <translation>ACCEL2故障</translation>
    </message>
    <message>
      <source>ACRO mode pitch rate</source>
      <translation>ACRO模式俯仰率</translation>
    </message>
    <message>
      <source>ADS-B avoidance minimum altitude</source>
      <translation>ADS-B回避最低高度</translation>
    </message>
    <message>
      <source>ADSB list size of nearest vehicles. Longer lists take longer to refresh with lower SRx_ADSB values.</source>
      <translation>ADSB 列出最近车辆的大小。较长的列表需要较长的时间才能刷新，且 SRx_ADSB 值较低。</translation>
    </message>
    <message>
      <source>ADSB vehicle list altitude filter. Vehicles detected more than this altitude above our own altitude will be completely ignored. They will not show up in the SRx_ADSB stream to the GCS and will not be considered in any avoidance calculations. A value of 0 will disable this filter.</source>
      <translation>ADSB 车辆列表高度过滤器。检测到高于我们自己的高度超过此高度的车辆将被完全忽略。它们不会出现在 GCS 的 SRx_ADSB 流中，并且不会在任何回避计算中考虑。 0 的值将禁用此过滤器。</translation>
    </message>
    <message>
      <source>ADSB-Out GPS Offset lat</source>
      <translation>ADSB-输出 GPS 偏移纬度</translation>
    </message>
    <message>
      <source>ADSBMavlinkProcessing</source>
      <translation>ADSBMavlink处理</translation>
    </message>
    <message>
      <source>AGL KF for optflow scaling</source>
      <translation>AGL KF 用于 optflow 缩放</translation>
    </message>
    <message>
      <source>AHRS Trim Roll</source>
      <translation>AHRS 切边辊</translation>
    </message>
    <message>
      <source>AHRS last origin longitude</source>
      <translation>AHRS 最后原点经度</translation>
    </message>
    <message>
      <source>AHRS/EKF Origin Longitude</source>
      <translation>AHRS/EKF 原点经度</translation>
    </message>
    <message>
      <source>AIS logging options</source>
      <translation>AIS 日志记录选项</translation>
    </message>
    <message>
      <source>AK8963</source>
      <translation>AK8963</translation>
    </message>
    <message>
      <source>ANX CAN battery index</source>
      <translation>ANX CAN 电池索引</translation>
    </message>
    <message>
      <source>APM TriggerPin5</source>
      <translation>APM 触发引脚5</translation>
    </message>
    <message>
      <source>APM2-A0</source>
      <translation>APM2-A0</translation>
    </message>
    <message>
      <source>APM2-A8</source>
      <translation>APM2-A8</translation>
    </message>
    <message>
      <source>ARMING_Y</source>
      <translation>ARMING_Y</translation>
    </message>
    <message>
      <source>ASPD1_Y</source>
      <translation>ASPD1_Y</translation>
    </message>
    <message>
      <source>ATEMP_X</source>
      <translation>ATEMP_X</translation>
    </message>
    <message>
      <source>AU-Telstra</source>
      <translation>AU-澳洲电信</translation>
    </message>
    <message>
      <source>AUAV-5in</source>
      <translation>AUAV-5英寸</translation>
    </message>
    <message>
      <source>AUTOLAND or RTL</source>
      <translation>AUTOLAND 或 RTL</translation>
    </message>
    <message>
      <source>AUTO_WAYPOINTS</source>
      <translation>AUTO_WAYPOINTS</translation>
    </message>
    <message>
      <source>AUX3</source>
      <translation>AUX3</translation>
    </message>
    <message>
      <source>AUXOUT1</source>
      <translation>AUXOUT1</translation>
    </message>
    <message>
      <source>AVGCELLV_EN</source>
      <translation>AVGCELLV_EN</translation>
    </message>
    <message>
      <source>Above Origin</source>
      <translation>高于原点</translation>
    </message>
    <message>
      <source>Above this speed, the measurement noise is linearly increased to reduce the sensitivity of the estimator from biased measurement. Set to a low value on vehicles with large lifting surfaces.</source>
      <translation>超过该速度，测量噪声会线性增加，从而降低估计器对有偏差测量的灵敏度。对于具有大升力面的车辆设置为较低值。</translation>
    </message>
    <message>
      <source>Absolute pitch threshold for quad-chute triggering in FW mode.
Above this the vehicle will transition back to MC mode and execute behavior defined in COM_QC_ACT.
Set to 0 do disable this threshold.</source>
      <translation>FW 模式下四槽触发的绝对俯仰阈值。在此之上，车辆将转换回 MC 模式并执行 COM_QC_ACT 中定义的行为。设置为 0 会禁用此阈值。</translation>
    </message>
    <message>
      <source>Accel (vertical) Error notch filter index, zero disables. If upgrading from 4.6 this is Previously _ACCZ_NEF.</source>
      <translation>加速（垂直） 错误陷波滤波器索引，零禁用。如果从 4.6 升级，则为以前的 _ACCZ_NEF。</translation>
    </message>
    <message>
      <source>Accel 1 scaling factor</source>
      <translation>Accel 1 比例因子</translation>
    </message>
    <message>
      <source>Accel 4 motor noise factor</source>
      <translation>Accel 4 电机噪声系数</translation>
    </message>
    <message>
      <source>Accel filter cutoff frequency</source>
      <translation>加速滤波器截止频率</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller D gain.  Compensates for short-term change in desired vertical acceleration vs actual acceleration</source>
      <translation>加速度（垂直）控制器 D 增益。  补偿所需垂直加速度与实际加速度的短期变化</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller P gain</source>
      <translation>加速度（垂直）控制器 P 增益</translation>
    </message>
    <message>
      <source>Acceleration (vertical) controller error frequency in Hz</source>
      <translation>加速度（垂直）控制器错误频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Acceleration Max for Roll</source>
      <translation>横滚最大加速度</translation>
    </message>
    <message>
      <source>Acceleration limit of the horizontal kinematic path generation used to determine how quickly the estimate varies in velocity</source>
      <translation>水平运动路径生成的加速度限制，用于确定估计速度变化的速度</translation>
    </message>
    <message>
      <source>Acceleration to tilt coupling</source>
      <translation>加速倾斜耦合</translation>
    </message>
    <message>
      <source>Accelerometer 0 calibration device ID</source>
      <translation>加速度计0校准装置ID</translation>
    </message>
    <message>
      <source>Accelerometer 1 Z-axis scaling factor</source>
      <translation>加速度计 1 Z 轴比例因子</translation>
    </message>
    <message>
      <source>Accelerometer 2 X-axis scaling factor</source>
      <translation>加速度计 2 X 轴比例因子</translation>
    </message>
    <message>
      <source>Accelerometer 2nd order temperature coefficient X axis</source>
      <translation>加速度计二阶温度系数 X 轴</translation>
    </message>
    <message>
      <source>Accelerometer 3 Z-axis scaling factor</source>
      <translation>加速度计 3 Z 轴比例因子</translation>
    </message>
    <message>
      <source>Accelerometer ID</source>
      <translation>加速度计ID</translation>
    </message>
    <message>
      <source>Accelerometer calibration minimum temperature</source>
      <translation>加速度计校准最低温度</translation>
    </message>
    <message>
      <source>Accelerometer offset temperature ^0 polynomial coefficient - X axis</source>
      <translation>加速度计偏移温度^0多项式系数-X轴</translation>
    </message>
    <message>
      <source>Accelerometer offset temperature ^2 polynomial coefficient - Z axis</source>
      <translation>加速度计偏移温度^2多项式系数-Z轴</translation>
    </message>
    <message>
      <source>Accelerometer offsets of Z axis</source>
      <translation>Z 轴加速度计偏移</translation>
    </message>
    <message>
      <source>Accelerometer sensor ID, taking into account its type, bus and instance</source>
      <translation>加速度计传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>Accelerometer2 offsets of Y axis. This is setup using the acceleration calibration or level operations</source>
      <translation>加速度计2 Y 轴偏移。这是使用加速度校准或水平操作进行设置的</translation>
    </message>
    <message>
      <source>Accelerometer2 scaling of Z axis  Calculated during acceleration calibration routine</source>
      <translation>加速度计2 Z 轴缩放 在加速度校准例程期间计算</translation>
    </message>
    <message>
      <source>Accelerometer3 offsets of Z axis. This is setup using the acceleration calibration or level operations</source>
      <translation>加速度计3 Z 轴偏移。这是使用加速度校准或水平操作进行设置的</translation>
    </message>
    <message>
      <source>Accept MAVLink only from system IDs given by MAV_SYSID_GCS and MAV_SYSID_GCS_HI</source>
      <translation>仅接受来自 MAV_SYSID_GCS 和 MAV_SYSID_GCS_HI 给出的系统 ID 的 MAVLink</translation>
    </message>
    <message>
      <source>Acro Balance Pitch</source>
      <translation>特技平衡俯仰</translation>
    </message>
    <message>
      <source>Acro Roll/Pitch deg/s</source>
      <translation>特技横滚/俯仰度/秒</translation>
    </message>
    <message>
      <source>Acro Yaw deg/s</source>
      <translation>俯仰偏航度/秒</translation>
    </message>
    <message>
      <source>Acro mode SuperExpo factor for Yaw</source>
      <translation>Acro 模式 SuperExpo 偏航系数</translation>
    </message>
    <message>
      <source>Acro mode roll, pitch super expo factor</source>
      <translation>特技模式横滚、俯仰超级展因子</translation>
    </message>
    <message>
      <source>Acro yaw rate control input time constant</source>
      <translation>Acro偏航角速度控制输入时间常数</translation>
    </message>
    <message>
      <source>Action the system takes when an imbalanced propeller is detected by the failure detector.
See also FD_IMB_PROP_THR to set the failure threshold.</source>
      <translation>当故障检测器检测到不平衡螺旋桨时，系统将采取行动。另请参阅 FD_IMB_PROP_THR 以设置故障阈值。</translation>
    </message>
    <message>
      <source>Active high</source>
      <translation>高电平有效</translation>
    </message>
    <message>
      <source>Actuator2</source>
      <translation>执行器2</translation>
    </message>
    <message>
      <source>Additional backend specific options</source>
      <translation>其他后端特定选项</translation>
    </message>
    <message>
      <source>Adjusts the autothrottle governor torque compensator that determines how fast the governor will adjust the base torque reference to compensate for changes in density altitude. If RPM is low or high by more than 2-5 RPM, increase this setting by 1% at a time until the governor speed matches your RPM setting. Setting the compensator too high can result in surging and throttle "hunting". Do not make large adjustments at one time</source>
      <translation>调整自动油门调速器扭矩补偿器，该补偿器确定调速器调整基本扭矩参考以补偿密度高度变化的速度。如果 RPM 的低值或高值超过 2-5 RPM，请一次将该设置增加 1%，直到调速器速度与 RPM 设置匹配。将补偿器设置得太高会导致喘振和油门“振荡”。不要一次性进行大幅调整</translation>
    </message>
    <message>
      <source>After first climb yaw reset</source>
      <translation>首次爬升偏航重置后</translation>
    </message>
    <message>
      <source>Aileron landing gain scalaing</source>
      <translation>副翼着陆增益缩放</translation>
    </message>
    <message>
      <source>Airborne1G</source>
      <translation>空降1G</translation>
    </message>
    <message>
      <source>Aircraft velocity vectors are multiplied by this time to determine closest approach.  If this results in an approach closer than F_DIST_XY or F_DIST_Z then F_ACTION is undertaken</source>
      <translation>飞机速度矢量乘以该时间以确定最近的接近。  如果这导致比 F_DIST_XY 或 F_DIST_Z 更接近，则采用 F_ACTION</translation>
    </message>
    <message>
      <source>Airspeed I2C bus for 2nd sensor</source>
      <translation>用于第二个传感器的 Airspeed I2C 总线</translation>
    </message>
    <message>
      <source>Airspeed calibration ratio</source>
      <translation>空速校准比</translation>
    </message>
    <message>
      <source>Airspeed in m/s to use when calculating surface speed scaling. Note that changing this value will affect all PID values</source>
      <translation>计算表面速度缩放时使用的空速（以米/秒为单位）。请注意，更改该值将影响所有 PID 值</translation>
    </message>
    <message>
      <source>Airspeed options bitmask</source>
      <translation>空速选项位掩码</translation>
    </message>
    <message>
      <source>Airspeed scale process noise of the internal wind estimator(s) of the airspeed selector.
When unaided, the scale uncertainty (1-sigma, unitless) increases by this amount every second.</source>
      <translation>空速选择器内部风估计器的空速刻度过程噪声。在没有辅助的情况下，尺度不确定性（1-sigma，无单位）每秒都会增加这个量。</translation>
    </message>
    <message>
      <source>Airspeed sensor offset</source>
      <translation>空速传感器偏移</translation>
    </message>
    <message>
      <source>Airspeed2 sensor ID, taking into account its type, bus and instance</source>
      <translation>Airspeed2 传感器 ID，考虑其类型、总线和实例</translation>
    </message>
    <message>
      <source>All</source>
      <translation>全部</translation>
    </message>
    <message>
      <source>All+FastATT</source>
      <translation>全部+FastATT</translation>
    </message>
    <message>
      <source>Allocate the RPM sensor instance to use for measuring head speed. RPM1 = 0.  RPM2 = 1.</source>
      <translation>分配 RPM 传感器实例用于测量磁头速度。 RPM1 = 0。  RPM2 = 1。</translation>
    </message>
    <message>
      <source>Allow FW Takeoff-if bit is not set then NAV_TAKEOFF command on quadplanes will instead perform a NAV_VTOL takeoff</source>
      <translation>允许 FW 起飞 - 如果未设置位，则四平面上的 NAV_TAKEOFF 命令将执行 NAV_VTOL 起飞</translation>
    </message>
    <message>
      <source>Allow arming</source>
      <translation>允许解锁</translation>
    </message>
    <message>
      <source>Allow relay output operation when running SIM-on-hardware</source>
      <translation>在硬件上运行 SIM 时允许继电器输出操作</translation>
    </message>
    <message>
      <source>AllowSpeedMismatchRecovery</source>
      <translation>允许速度不匹配恢复</translation>
    </message>
    <message>
      <source>Allows setting an individual MAVLink system id for this vehicle to distinguish it from others on the same network. IDs above 255 require MAVLink2 with 32-bit system ID support on every communicating device. MAVLink1 ports cannot transmit with these IDs.</source>
      <translation>允许为此车辆设置单独的 MAVLink 系统 ID，以将其与同一网络上的其他车辆区分开来。 255 以上的 ID 需要每个通信设备上具有 32 位系统 ID 支持的 MAVLink2。 MAVLink1 端口无法使用这些 ID 进行传输。</translation>
    </message>
    <message>
      <source>Allows to deploy the landing configuration (flaps, landing airspeed, etc.) already in the loiter-down waypoint before the final approach. Otherwise is enabled only in the final approach.</source>
      <translation>允许在最终进近之前部署已在悬停航路点的着陆配置（襟翼、着陆空速等）。否则仅在最终方法中启用。</translation>
    </message>
    <message>
      <source>Allows you to enable (1) or disable (0) the FETtecOneWireESC simulator</source>
      <translation>允许您启用 (1) 或禁用 (0) FETtecOneWireESC 模拟器</translation>
    </message>
    <message>
      <source>Allows you to enable (1) or disable (0) the autorotation functionality within the Rotor Speed Controller.</source>
      <translation>允许您启用 (1) 或禁用 (0) 转子速度控制器内的自动旋转功能。</translation>
    </message>
    <message>
      <source>Allows you to set vertical wind direction (true deg) in sim. 0 means pure horizontal wind. 90 means pure updraft.</source>
      <translation>允许您在 sim 中设置垂直风向（真实角度）。 0表示纯水平风。 90 表示纯上升气流。</translation>
    </message>
    <message>
      <source>Alternative HW config</source>
      <translation>替代 HW 配置</translation>
    </message>
    <message>
      <source>Altitude above home to which to climb when searching for the landing target.</source>
      <translation>搜索着陆目标时要爬升的海拔高度。</translation>
    </message>
    <message>
      <source>Altitude control mode</source>
      <translation>高度控制方式</translation>
    </message>
    <message>
      <source>Altitude following</source>
      <translation>高度跟随</translation>
    </message>
    <message>
      <source>Altitude in standard atmosphere at which the vehicle in normal configuration (WEIGHT_BASE) is still able to achieve a maximum climb rate of 0.5m/s at maximum throttle (FW_THR_MAX). Used to compensate for air density in FW_T_CLMB_MAX. Set negative to disable.</source>
      <translation>标准大气中正常配置（WEIGHT_BASE）的车辆在最大油门（FW_THR_MAX）下仍能达到0.5m/s最大爬升率的高度。用于补偿FW_T_CLMB_MAX中的空气密度。设置为负数以禁用。</translation>
    </message>
    <message>
      <source>Altitude offset</source>
      <translation>海拔高度偏移</translation>
    </message>
    <message>
      <source>Altitude the vehicle will transit to when a fence breach occurs</source>
      <translation>发生围栏突破时车辆将行驶到的高度</translation>
    </message>
    <message>
      <source>Always face bow towards target point</source>
      <translation>始终将弓面向目标点</translation>
    </message>
    <message>
      <source>Amount of (evenly-distributed) noise injected into the 1st baro</source>
      <translation>注入第一个气压计的（均匀分布的）噪声量</translation>
    </message>
    <message>
      <source>Amount of rudder to add during aileron movement. Increase if nose initially yaws away from roll. Reduces adverse yaw.</source>
      <translation>副翼运动期间添加的方向舵量。如果机头最初因侧倾而偏航，则增加。减少不利的偏航。</translation>
    </message>
    <message>
      <source>Amplitude of the injected signal</source>
      <translation>注入信号的幅度</translation>
    </message>
    <message>
      <source>An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
Set to "Custom Euler Angle" to define the rotation using CAL_MAG0_ROLL, CAL_MAG0_PITCH and CAL_MAG0_YAW.</source>
      <translation>内部传感器将强制设置 -1 的值，因此 GCS 仅应在该值大于或等于零时尝试配置旋转。设置为“自定义欧拉角”以使用 CAL_MAG0_ROLL、CAL_MAG0_PITCH 和 CAL_MAG0_YAW 定义旋转。</translation>
    </message>
    <message>
      <source>Analog</source>
      <translation>模拟</translation>
    </message>
    <message>
      <source>Analog input pin that fuel level sensor is connected to.Analog Airspeed or RSSI ports can be used for Analog input( some autopilots provide others also). Values for some autopilots are given as examples. Search wiki for "Analog pins".</source>
      <translation>燃油位传感器连接的模拟输入引脚。模拟空速或 RSSI 端口可用于模拟输入（某些自动驾驶仪也提供其他端口）。一些自动驾驶仪的值作为示例给出。在 wiki 中搜索“模拟引脚”。</translation>
    </message>
    <message>
      <source>Angle</source>
      <translation>角度</translation>
    </message>
    <message>
      <source>Angle for swash plate servo 2</source>
      <translation>斜盘舵机角度 2</translation>
    </message>
    <message>
      <source>AngleRollP</source>
      <translation>角度滚动</translation>
    </message>
    <message>
      <source>Angular jerk limit for the heading kinematic input shaping</source>
      <translation>航向运动输入整形的角度加加速度限制</translation>
    </message>
    <message>
      <source>Antenna Z position offset</source>
      <translation>天线 Z 位置偏移</translation>
    </message>
    <message>
      <source>Applies in any altitude controlled flight mode.</source>
      <translation>适用于任何高度控制飞行模式。</translation>
    </message>
    <message>
      <source>Apply choke when stopping engine</source>
      <translation>停止发动机时使用阻风门</translation>
    </message>
    <message>
      <source>Approximate deceleration during back transition</source>
      <translation>向后过渡期间的近似减速度</translation>
    </message>
    <message>
      <source>Arm authorizer system id</source>
      <translation>Arm授权者系统ID</translation>
    </message>
    <message>
      <source>Arm-Land-RTL</source>
      <translation>解锁－降落－返航</translation>
    </message>
    <message>
      <source>ArmDisarm with Quadplane AirMode (4.2 and higher)</source>
      <translation>使用 Quadplane AirMode 的 ArmDisarm（4.2 及更高版本）</translation>
    </message>
    <message>
      <source>Arming disabled until some requirements are met. If 0, there are no requirements (arm immediately).  If 1, require rudder stick or GCS arming before arming motors and sends the minimum throttle PWM value to the throttle channel when disarmed.  If 2, require rudder stick or GCS arming and send 0 PWM to throttle channel when disarmed. See the ARMING_CHECK_* parameters to see what checks are done before arming. Note, if setting this parameter to 0 a reboot is required to arm the plane.  Also note, even with this parameter at 0, if ARMING_CHECK parameter is not also zero the plane may fail to arm throttle at boot due to a pre-arm check failure. On planes with ICE enabled and the throttle while disarmed option set in ICE_OPTIONS the motor will get THR_MIN when disarmed.</source>
      <translation>在满足某些要求之前，禁用解锁。如果是0，则没有要求（立即解锁）。  如果是 1，则需要在解锁电机之前先解锁舵杆或 GCS，并在解锁时将最小油门 PWM 值发送到油门通道。  如果是 2，则要求舵杆或 GCS 解锁，并在上锁时将 0 PWM 发送至油门通道。请参阅 ARMING_CHECK_* 参数以了解解锁前进行的检查。注意，如果将此参数设置为 0，则需要重新启动才能解锁飞机。  另请注意，即使在 0 处使用此参数，如果 ARMING_CHECK 参数也不为零，则飞机可能会因预解锁检查失败而无法在启动时解锁油门。在启用 ICE 且在 ICE_OPTIONS 中设置上锁选项时油门的飞机上，上锁时电机将获得 THR_MIN。</translation>
    </message>
    <message>
      <source>Ashtech / Trimble</source>
      <translation>阿什泰克 / 天宝</translation>
    </message>
    <message>
      <source>Attenuation</source>
      <translation>衰减</translation>
    </message>
    <message>
      <source>Auto (RC and MAVLink gimbal protocol v2)</source>
      <translation>自动（RC 和 MAVLink 万向节协议 v2）</translation>
    </message>
    <message>
      <source>Auto Takeoff Check RPM minimum</source>
      <translation>自动起飞检查 RPM 最小值</translation>
    </message>
    <message>
      <source>Auto takeoff (in Auto or Guided) is not permitted until motors report at least this RPM. Set to zero to disable check (which also disables Q_TKOFF_RPM_MAX).</source>
      <translation>在电机至少报告此 RPM 之前，不允许自动起飞（自动或引导）。设置为零以禁用检查（这也会禁用 Q_TKOFF_RPM_MAX）。</translation>
    </message>
    <message>
      <source>Auto-enable of fences. AutoEnableOnTakeoff enables all configured fences, except the minimum altitude fence (which is enabled when the minimum altitude is reached), after autotakeoffs reach altitude. During autolandings the fences will be disabled.  AutoEnableDisableFloorOnLanding enables all configured fences, except the minimum altitude fence (which is enabled when the minimum altitude is reached), after autotakeoffs reach altitude. During autolandings only the Minimum Altitude fence will be disabled. AutoEnableOnlyWhenArmed enables all configured fences on arming, except the minimum altitude fence (which is enabled when the minimum altitude is reached), but no fences are disabled during autolandings. However, fence breaches are ignored while executing prior breach recovery actions which may include autolandings.</source>
      <translation>自动启用围栏。自动起飞达到高度后，AutoEnableOnTakeoff 启用所有已配置的围栏，除了最低高度围栏（达到最小高度时启用）。自动着陆期间，围栏将被禁用。  自动起飞到达高度后，AutoEnableDisableFloorOnLanding 启用所有已配置的围栏，最低高度围栏（达到最低高度时启用）除外。在自动着陆期间，只有最低高度栅栏将被禁用。 AutoEnableOnlyWhenArmed 在解锁时启用所有已配置的围栏，除了最低高度围栏（达到最小高度时启用），但在自动着陆期间不会禁用任何围栏。然而，在执行先前的违规恢复操作（可能包括自动着陆）时，围栏违规行为将被忽略。</translation>
    </message>
    <message>
      <source>AutoEnableDisableFloorOnly</source>
      <translation>仅自动启用禁用楼层</translation>
    </message>
    <message>
      <source>AutoTune Gain Margin Backoff</source>
      <translation>自动调谐增益裕度退避</translation>
    </message>
    <message>
      <source>Autodetect</source>
      <translation>自动检测</translation>
    </message>
    <message>
      <source>Automatic airspeed ratio calibration</source>
      <translation>自动空速比校准</translation>
    </message>
    <message>
      <source>Automatically begin landing sequence after arriving at RTL location. This requires the addition of a DO_LAND_START mission item, which acts as a marker for the start of a landing sequence. The closest landing sequence will be chosen to the current location For a value of 1 a rally point will be used instead of HOME if in range (see rally point documentation).If this is set to 0 and there is a DO_LAND_START or DO_RETURN_PATH_START mission item then you will get an arming check failure. You can set to a value of 3 to avoid the arming check failure and use the DO_LAND_START for go-around (see wiki for aborting autolandings) without it changing RTL behaviour.</source>
      <translation>到达 RTL 位置后自动开始着陆序列。这需要添加 DO_LAND_START 任务项目，作为着陆序列开始的标记。将选择距离当前位置最近的着陆序列。对于 1 的值，如果在范围内，将使用集结点而不是 HOME（请参阅集结点文档）。如果将其设置为 0 并且存在 DO_LAND_START 或 DO_RETURN_PATH_START 任务项目，那么您将收到解锁检查失败。您可以设置为 3 的值以避免解锁检查失败，并使用 DO_LAND_START 进行复飞（请参阅 wiki 以中止自动着陆），而不会改变 RTL 的行为。</translation>
    </message>
    <message>
      <source>Automatically enables High Latency mode if not already enabled</source>
      <translation>如果尚未启用，则自动启用高延迟模式</translation>
    </message>
    <message>
      <source>Autopilot mounting position pitch offset. Positive values = pitch up, negative values = pitch down. This parameter is only used when AHRS_ORIENTATION is set to CUSTOM.</source>
      <translation>自动驾驶仪安装位置俯仰偏移。正值 = 向上倾斜，负值 = 向下倾斜。该参数仅在 AHRS_ORIENTATION 设置为 CUSTOM 时使用。</translation>
    </message>
    <message>
      <source>Autotune level</source>
      <translation>自动调谐级别</translation>
    </message>
    <message>
      <source>Aux4</source>
      <translation>辅助 4</translation>
    </message>
    <message>
      <source>Auxiliary channel to control yaw (in AUX input or manual mode)</source>
      <translation>控制偏航的辅助通道（在AUX输入或手动模式下）</translation>
    </message>
    <message>
      <source>Auxiliary global position sensor 2 aiding</source>
      <translation>辅助全球位置传感器 2 提供帮助</translation>
    </message>
    <message>
      <source>Avg Cell Resting Volt warn level</source>
      <translation>平均电池静息电压警告水平</translation>
    </message>
    <message>
      <source>Avoidance distance margin in GPS modes</source>
      <translation>GPS 模式下的回避距离余量</translation>
    </message>
    <message>
      <source>Avoidance processing rate</source>
      <translation>回避处理率</translation>
    </message>
    <message>
      <source>Axis of rotor 1 thrust vector, Y body axis component</source>
      <translation>转子轴1推力矢量，Y本体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 2 thrust vector, X body axis component</source>
      <translation>转子轴2推力矢量，X体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 4 thrust vector, Z body axis component</source>
      <translation>转子轴4推力矢量，Z体轴分量</translation>
    </message>
    <message>
      <source>Axis of rotor 7 thrust vector, Y body axis component</source>
      <translation>转子轴7推力矢量，Y本体轴分量</translation>
    </message>
    <message>
      <source>Azimuth position on swashplate 1 for servo 2 with the front of the heli being 0 deg</source>
      <translation>舵机 2 斜盘 1 上的方位位置，直升机前部为 0 deg</translation>
    </message>
    <message>
      <source>BAT2VLT_EN</source>
      <translation>BAT2VLT_EN</translation>
    </message>
    <message>
      <source>BATUSED_Y</source>
      <translation>BATUSED_Y</translation>
    </message>
    <message>
      <source>BB Blue GP0 pin 6</source>
      <translation>BB 蓝色 GP0 引脚 6</translation>
    </message>
    <message>
      <source>BK2425</source>
      <translation>BK2425</translation>
    </message>
    <message>
      <source>BLHTEMP_X</source>
      <translation>BLHTEMP_X</translation>
    </message>
    <message>
      <source>BLHeli bitmask of reversed channels</source>
      <translation>反向通道的 BLHeli 位掩码</translation>
    </message>
    <message>
      <source>BLHeli32/AM32/Kiss+EDT</source>
      <translation>BLHeli32/AM32/吻+EDT</translation>
    </message>
    <message>
      <source>BLHeli_S/BlueJay+EDT</source>
      <translation>BLHeli_S/蓝鸟+EDT</translation>
    </message>
    <message>
      <source>BMP280</source>
      <translation>BMP280</translation>
    </message>
    <message>
      <source>BTEMP_Y</source>
      <translation>BTEMP_Y</translation>
    </message>
    <message>
      <source>Backup latitude for EKF origin</source>
      <translation>EKF原点的备份纬度</translation>
    </message>
    <message>
      <source>Ballistic coefficient for Y axis drag</source>
      <translation>Y 轴阻力的弹道系数</translation>
    </message>
    <message>
      <source>Baro</source>
      <translation>气压计</translation>
    </message>
    <message>
      <source>Baro height ground effect dead zone</source>
      <translation>巴罗高度地面效应盲区</translation>
    </message>
    <message>
      <source>Baro3 Noise</source>
      <translation>Baro3 噪声</translation>
    </message>
    <message>
      <source>Barometer 2 calibration device ID</source>
      <translation>气压计2校准装置ID</translation>
    </message>
    <message>
      <source>Barometer altitude drifts at this rate</source>
      <translation>气压计高度以此速率漂移</translation>
    </message>
    <message>
      <source>Barometer data time delay</source>
      <translation>气压计数据时间延迟</translation>
    </message>
    <message>
      <source>Barometer noise</source>
      <translation>气压计噪音</translation>
    </message>
    <message>
      <source>Barometer options</source>
      <translation>气压计选项</translation>
    </message>
    <message>
      <source>Barometer wind coefficient direction right in SITL</source>
      <translation>气压计风系数方向在 SITL 右侧</translation>
    </message>
    <message>
      <source>Barometric sensor height aiding</source>
      <translation>气压传感器高度辅助</translation>
    </message>
    <message>
      <source>Baseline airspeed for pitch feed-forward.</source>
      <translation>俯仰前馈的基线空速。</translation>
    </message>
    <message>
      <source>Battery 1 idle current overwrite</source>
      <translation>电池 1 空闲电流覆盖</translation>
    </message>
    <message>
      <source>Battery 2 monitoring source</source>
      <translation>电池2监控源</translation>
    </message>
    <message>
      <source>Battery Index</source>
      <translation>电池指数</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT2_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT2_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT6_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT6_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTA_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTA_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTE_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTE_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATT2_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATT2_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the BATTA_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池容量。设置为 0 以禁用电池保持故障保护。如果电池容量低于此水平，车辆将执行 BATTA_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity remaining to trigger failsafe. Set to 0 to disable battery remaining failsafe.</source>
      <translation>用于触发故障保护的剩余电池容量。设置为 0 以禁用电池保持故障保护。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT5_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT5_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT9_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT9_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATTD_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATTD_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery capacity remaining which is required to arm the aircraft. Set to 0 to allow arming at any capacity. Note that execept for smart batteries rebooting the vehicle will always reset the remaining capacity estimate, which can lead to this check not providing sufficent protection, it is recommended to always use this in conjunction with the BATT_ARM_VOLT parameter.</source>
      <translation>解锁飞机所需的剩余电池容量。设置为 0 以允许以任何容量解锁。请注意，除了智能电池重新启动车辆将始终重置剩余电量估计值（这可能导致此检查无法提供足够的保护）之外，建议始终将其与 BATT_ARM_VOLT 参数结合使用。</translation>
    </message>
    <message>
      <source>Battery estimator coefficient2</source>
      <translation>电池估算器系数2</translation>
    </message>
    <message>
      <source>Battery monitor I2C bus number</source>
      <translation>电池监视器I2C总线号</translation>
    </message>
    <message>
      <source>Battery serial number</source>
      <translation>电池序列号</translation>
    </message>
    <message>
      <source>Battery voltage compensation minimum voltage (voltage below this will have no additional scaling effect on thrust).  Recommend 3.5 * cell count, 0 = Disabled</source>
      <translation>电池电压补偿最小电压（低于此电压不会对推力产生额外的缩放效果）。  推荐 3.5 * 细胞计数，0 = 禁用</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATT8_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATT8_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATT8_LOW_TIMER 参数指定的时间，则车辆将执行 BATT8_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATTG_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATTG_FS_CRT_ACT parameter.</source>
      <translation>触发关键电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATTG_LOW_TIMER 参数指定的时间，则车辆将执行 BATTG_FS_CRT_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATT8_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATT8_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATT8_LOW_TIMER 参数指定的时间，则车辆将执行 BATT8_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the BATTG_LOW_TIMER parameter then the vehicle will perform the failsafe specified by the BATTG_FS_LOW_ACT parameter.</source>
      <translation>触发低电池故障保护的电池电压。设置为 0 以禁用。如果电池电压持续低于该电压的时间超过 BATTG_LOW_TIMER 参数指定的时间，则车辆将执行 BATTG_FS_LOW_ACT 参数指定的故障保护。</translation>
    </message>
    <message>
      <source>Baudrate for the GPS 1 Serial Port</source>
      <translation>GPS 1 串口的波特率</translation>
    </message>
    <message>
      <source>Baudrate for the TELEM/SERIAL 4 Serial Port</source>
      <translation>TELEM/SERIAL 4 串口波特率</translation>
    </message>
    <message>
      <source>Beacon origin's altitude above sealevel in meters</source>
      <translation>信标原点的海拔高度（以米为单位）</translation>
    </message>
    <message>
      <source>Before entering failsafe (RTL, Land, Hold), wait COM_FAIL_ACT_T seconds in Hold mode for the user to realize. During that time the user can switch modes, but cannot take over control via the stick override feature (see COM_RC_OVERRIDE). Afterwards the configured failsafe action is triggered and the user may use stick override. A zero value disables the delay.</source>
      <translation>在进入故障保护（RTL、着陆、保持）之前，请在保持模式下等待 COM_FAIL_ACT_T 秒，以便用户意识到。在此期间，用户可以切换模式，但无法通过摇杆超控功能接管控制（参见 COM_RC_OVERRIDE）。之后，配置的故障安全操作被触发，用户可以使用操纵杆覆盖。零值会禁用延迟。</translation>
    </message>
    <message>
      <source>Below this ground speed weathervaning is permitted. Set to 0 to ignore this condition when checking if vehicle should weathervane.</source>
      <translation>低于这个地速是允许风向变化的。设置为 0 可在检查车辆是否应随风向标时忽略此情况。</translation>
    </message>
    <message>
      <source>Benewake-Serial</source>
      <translation>北醒系列</translation>
    </message>
    <message>
      <source>Benewake_CAN</source>
      <translation>北醒_CAN</translation>
    </message>
    <message>
      <source>Bit rate can be set up to from 10000 to 1000000</source>
      <translation>比特率可设置为 10000 至 1000000</translation>
    </message>
    <message>
      <source>Bitmap of what on-board log types to enable. This value is made up of the sum of each of the log types you want to be saved. It is usually best just to enable all basiclog types by setting this to 65535. Note that if you want to reduce log sizes you should consider using LOG_FILE_RATEMAX instead of disabling logging items with this parameter.</source>
      <translation>要启用的板载日志类型的位图。该值由您要保存的每种日志类型的总和组成。通常最好通过将其设置为 65535 来启用所有基本日志类型。请注意，如果您想减少日志大小，您应该考虑使用 LOG_FILE_RATEMAX，而不是使用此参数禁用日志记录项。</translation>
    </message>
    <message>
      <source>Bitmask defining which servo channels are to be transmitted over Piccolo CAN</source>
      <translation>定义哪些舵机通道要通过 Piccolo CAN 传输的位掩码</translation>
    </message>
    <message>
      <source>Bitmask for what GNSS system to use on the second GPS (all unchecked or zero to leave GPS as configured)</source>
      <translation>用于第二个 GPS 上使用的 GNSS 系统的位掩码（全部未选中或为零以使 GPS 保持配置状态）</translation>
    </message>
    <message>
      <source>Bitmask of SmartRTL options.</source>
      <translation>SmartRTL 选项的位掩码。</translation>
    </message>
    <message>
      <source>Bitmask of flight modes to disable for GCS selection. Mode can still be accessed via RC or failsafe.</source>
      <translation>用于禁用 GCS 选择的飞行模式位掩码。仍然可以通过 RC 或故障安全访问模式。</translation>
    </message>
    <message>
      <source>Bitmask of motors to remain active in forward flight for a 'Copter' tailsitter. Non-zero indicates airframe is a Copter tailsitter and uses copter style motor layouts determined by Q_FRAME_CLASS and Q_FRAME_TYPE. This should be zero for non-Copter tailsitters.</source>
      <translation>电机位掩码在“直升机”尾翼向前飞行时保持活动状态。非零表示机身是直升机尾座，并使用由 Q_FRAME_CLASS 和 Q_FRAME_TYPE 确定的直升机式电机布局。对于非直升机尾座来说，该值应该为零。</translation>
    </message>
    <message>
      <source>Bitmask of options to use with landing.</source>
      <translation>用于登陆的选项的位掩码。</translation>
    </message>
    <message>
      <source>Bitmask to enable Rover failsafe options</source>
      <translation>用于启用 Rover 故障安全选项的位掩码</translation>
    </message>
    <message>
      <source>Bitmask which will govern vehicles behaviour while recovering from Obstacle Avoidance (i.e Avoidance is turned off after the path ahead is clear).   </source>
      <translation>位掩码将控制车辆在从避障状态恢复时的行为（即前方道路畅通后关闭避障）。   </translation>
    </message>
    <message>
      <source>Bitrate of CAN interface</source>
      <translation>CAN接口比特率</translation>
    </message>
    <message>
      <source>Block and MAVLink</source>
      <translation>块和 MAVLink</translation>
    </message>
    <message>
      <source>Board Orientation</source>
      <translation>板方向</translation>
    </message>
    <message>
      <source>Board orientation roll offset</source>
      <translation>板定向辊偏移</translation>
    </message>
    <message>
      <source>Board voltage</source>
      <translation>板电压</translation>
    </message>
    <message>
      <source>BodyFrameRoll_P</source>
      <translation>BodyFrameRoll_P</translation>
    </message>
    <message>
      <source>Bootloader update</source>
      <translation>引导加载程序更新</translation>
    </message>
    <message>
      <source>Broadcast heartbeats on local network for MAVLink instance 2</source>
      <translation>在 MAVLink 实例 2 的本地网络上广播心跳</translation>
    </message>
    <message>
      <source>Built-in buzzer</source>
      <translation>内置蜂鸣器</translation>
    </message>
    <message>
      <source>Bus0</source>
      <translation>总线0</translation>
    </message>
    <message>
      <source>Bus6</source>
      <translation>总线6</translation>
    </message>
    <message>
      <source>Button Pin 2 RC Channel function</source>
      <translation>按钮引脚 2 RC 通道功能</translation>
    </message>
    <message>
      <source>Buzzer Driver Types</source>
      <translation>蜂鸣器驱动器类型</translation>
    </message>
    <message>
      <source>By default, the receiver is automatically configured. Sometimes it may be used for multiple purposes.
If the offered parameters aren't sufficient, this parameter can be disabled to have full control of the receiver configuration.
A good way to use this is to enable automatic configuration, let the receiver be configured, and then disable it to make manual adjustments.</source>
      <translation>默认情况下，接收器是自动配置的。有时它可以用于多种目的。如果提供的参数不够，可以禁用此参数以完全控制接收器配置。使用此功能的一个好方法是启用自动配置，配置接收器，然后禁用它以进行手动调整。</translation>
    </message>
    <message>
      <source>CAN Driver ID to be routed to SLCAN, 0 means no routing</source>
      <translation>CAN 驱动ID要路由到SLCAN，0表示不路由</translation>
    </message>
    <message>
      <source>CAN receive ID</source>
      <translation>CAN 接收 ID</translation>
    </message>
    <message>
      <source>CAN7</source>
      <translation>CAN7</translation>
    </message>
    <message>
      <source>CAN_D2_UC_S2</source>
      <translation>CAN_D2_UC_S2</translation>
    </message>
    <message>
      <source>CIRCLE Mode</source>
      <translation>CIRCLE模式</translation>
    </message>
    <message>
      <source>CLK_Y</source>
      <translation>CLK_Y</translation>
    </message>
    <message>
      <source>COM1(RS232) on GSOF</source>
      <translation>GSOF 上的 COM1(RS232)</translation>
    </message>
    <message>
      <source>CROSSHAIRS</source>
      <translation>CROSSHAIRS</translation>
    </message>
    <message>
      <source>CTUN</source>
      <translation>CTUN</translation>
    </message>
    <message>
      <source>CURRENT_X</source>
      <translation>CURRENT_X</translation>
    </message>
    <message>
      <source>Calibrate speed</source>
      <translation>校准速度</translation>
    </message>
    <message>
      <source>Calibration temperature for accelerometer</source>
      <translation>加速度计的校准温度</translation>
    </message>
    <message>
      <source>Camera Lens</source>
      <translation>相机镜头</translation>
    </message>
    <message>
      <source>Camera X position offset</source>
      <translation>相机 X 位置偏移</translation>
    </message>
    <message>
      <source>Camera feedback pin polarity</source>
      <translation>相机反馈引脚极性</translation>
    </message>
    <message>
      <source>Camera selection when switch is in high position</source>
      <translation>开关处于高位时选择摄像头</translation>
    </message>
    <message>
      <source>Camera tilt mount center</source>
      <translation>相机倾斜安装中心</translation>
    </message>
    <message>
      <source>Camera vertical field of view</source>
      <translation>相机垂直视野</translation>
    </message>
    <message>
      <source>Camera1 Video Stream Encoding</source>
      <translation>Camera1视频流编码</translation>
    </message>
    <message>
      <source>Camera1 Video Stream IP Address 3</source>
      <translation>摄像头1 视频流 IP 地址 3</translation>
    </message>
    <message>
      <source>CameraShutterSpeed</source>
      <translation>相机快门速度</translation>
    </message>
    <message>
      <source>Can be used for parameter tuning with the RC. This one is further referenced as the 2nd parameter channel.
Set to 0 to deactivate *</source>
      <translation>可用于 RC 的参数调整。该通道进一步被称为第二个参数通道。设置为 0 以停用*</translation>
    </message>
    <message>
      <source>Capacity of the battery in mAh when full</source>
      <translation>充满时电池的容量（mAh）</translation>
    </message>
    <message>
      <source>Cell count override</source>
      <translation>细胞计数覆盖</translation>
    </message>
    <message>
      <source>Ch14</source>
      <translation>Ch14</translation>
    </message>
    <message>
      <source>Ch9</source>
      <translation>Ch9</translation>
    </message>
    <message>
      <source>Chan16</source>
      <translation>陈16</translation>
    </message>
    <message>
      <source>Chan9</source>
      <translation>陈9</translation>
    </message>
    <message>
      <source>Changes the overall responsiveness of the vehicle.
The higher the value, the faster the vehicle will react.
If set to a value greater than zero, other parameters are automatically set (such as
the acceleration or jerk limits).
If set to a negative value, the existing individual parameters are used.</source>
      <translation>改变车辆的整体响应能力。值越高，车辆反应越快。如果设置为大于零的值，则会自动设置其他参数（例如加速度或急动度限制）。如果设置为负值，则使用现有的各个参数。</translation>
    </message>
    <message>
      <source>Channel 12</source>
      <translation>通道12</translation>
    </message>
    <message>
      <source>Channel 19</source>
      <translation>频道 19</translation>
    </message>
    <message>
      <source>Channel 26</source>
      <translation>频道 26</translation>
    </message>
    <message>
      <source>Channel 4</source>
      <translation>通道4</translation>
    </message>
    <message>
      <source>Channel 9</source>
      <translation>通道9</translation>
    </message>
    <message>
      <source>Channel10</source>
      <translation>通道10</translation>
    </message>
    <message>
      <source>Channel18</source>
      <translation>频道18</translation>
    </message>
    <message>
      <source>Channel25</source>
      <translation>通道25</translation>
    </message>
    <message>
      <source>Channel4</source>
      <translation>频道4</translation>
    </message>
    <message>
      <source>Chassis deployment feedback pin</source>
      <translation>底盘部署反馈引脚</translation>
    </message>
    <message>
      <source>Check that MAV_SYSID (or SYDID_THISMAV) has been set. 3 or less to prevent arming. -1 to disable.</source>
      <translation>检查 MAV_SYSID（或 SYDID_THISMAV）是否已设置。 3 或以下以防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>Choke</source>
      <translation>窒息</translation>
    </message>
    <message>
      <source>Circle Speed</source>
      <translation>圆周速度</translation>
    </message>
    <message>
      <source>Circle options</source>
      <translation>圆圈选项</translation>
    </message>
    <message>
      <source>CircuitStatus battery index</source>
      <translation>CircuitStatus电池指数</translation>
    </message>
    <message>
      <source>Clear all configurations not set by ardupilot (UBlox only)</source>
      <translation>清除 ardupilot 未设置的所有配置（仅限 UBlox）</translation>
    </message>
    <message>
      <source>Closest allowed projected distance before BEHAVIOUR_F is undertaken</source>
      <translation>进行 BEHAVIOUR_F 之前允许的最近投影距离</translation>
    </message>
    <message>
      <source>Coaxial helicopter</source>
      <translation>同轴直升机</translation>
    </message>
    <message>
      <source>Collective Blade Pitch Angle Maximum</source>
      <translation>总距叶片最大桨距角</translation>
    </message>
    <message>
      <source>Collective pitch curve at position 1</source>
      <translation>1 位置处的总距桨距曲线</translation>
    </message>
    <message>
      <source>Combined with START_LONGITUDE this parameter allows for an initial position of the tracker to be set. This position will be used until the GPS gets lock. It can also be used to run a stationary tracker with no GPS attached.</source>
      <translation>与 START_LONGITUDE 结合使用该参数可以设置跟踪器的初始位置。该位置将一直使用到 GPS 锁定为止。它还可用于运行未连接 GPS 的固定跟踪器。</translation>
    </message>
    <message>
      <source>Compass device id expected</source>
      <translation>需要罗盘设备 ID</translation>
    </message>
    <message>
      <source>Compass direction in which vehicle should approach towards dock. -1 value represents unset parameter</source>
      <translation>车辆应接近码头的罗盘方向。 -1值代表未设置的参数</translation>
    </message>
    <message>
      <source>Compass mounting position roll offset. Positive values = roll right, negative values = roll left. This parameter is only used when COMPASS_ORIENT/2/3 is set to CUSTOM.</source>
      <translation>罗盘安装位置横滚偏移。正值=向右横滚，负值=向左横滚。该参数仅在 COMPASS_ORIENT/2/3 设置为 CUSTOM 时使用。</translation>
    </message>
    <message>
      <source>Compass per-motor1 Y</source>
      <translation>每电机罗盘1 Y</translation>
    </message>
    <message>
      <source>Compass per-motor4 X</source>
      <translation>每电机罗盘4 X</translation>
    </message>
    <message>
      <source>Compass soft-iron off-diagonal Z component</source>
      <translation>罗盘软铁非对角 Z 分量</translation>
    </message>
    <message>
      <source>Compass2 orientation</source>
      <translation>罗盘2方向</translation>
    </message>
    <message>
      <source>Compass2 used for yaw</source>
      <translation>Compass2 用于偏航</translation>
    </message>
    <message>
      <source>Compass3 scale factor</source>
      <translation>罗盘3比例因子</translation>
    </message>
    <message>
      <source>Compass4 device id</source>
      <translation>罗盘4设备ID</translation>
    </message>
    <message>
      <source>Compensation for X axis of motor2</source>
      <translation>电机X轴补偿2</translation>
    </message>
    <message>
      <source>Compensation for Z axis of motor2</source>
      <translation>电机Z轴补偿2</translation>
    </message>
    <message>
      <source>Complimentary filter magnetometer weight</source>
      <translation>免费过滤磁力计重量</translation>
    </message>
    <message>
      <source>Configuration for thrust loss detection detection, this results in "Potential VTOL Thrust Loss" warnings. This detection also allows the motor mixer to ignore the failed motor allowing better use of the remaining motors.</source>
      <translation>配置推力损失检测，这会导致“潜在 VTOL 推力损失”警告。这种检测还允许电机混合器忽略故障电机，从而更好地使用剩余电机。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run DSM RC Input Driver.
DSM RC (Spektrum) driver.</source>
      <translation>配置在哪个串行端口上运行 DSM RC 输入驱动程序。 DSM RC (Spektrum) 驱动程序。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run GPS Port.</source>
      <translation>配置GPS端口在哪个串口上运行。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run MAVLink.</source>
      <translation>配置在哪个串行端口上运行 MAVLink。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run SBUS RC Input Driver. SBUS RC driver.</source>
      <translation>配置在哪个串行端口上运行 SBUS RC 输入驱动程序。 SBUS RC 驱动程序。</translation>
    </message>
    <message>
      <source>Configure on which serial port to run Vertiq IO.</source>
      <translation>配置在哪个串行端口上运行 Vertiq IO。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the EXT2 Serial Port. Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 EXT2 串口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the Radio Controller Serial Port. Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置无线电控制器串行端口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the Baudrate for the TELEM/SERIAL 4 Serial Port. Note: certain drivers such as the GPS can determine the Baudrate automatically.</source>
      <translation>配置 TELEM/SERIAL 4 串口的波特率。注意：某些驱动程序（例如 GPS）可以自动确定波特率。</translation>
    </message>
    <message>
      <source>Configure the mode of the landing target. Depending on the mode, the landing target observations are used differently to aid position estimation.
Mode Moving:     The landing target may be moving around while in the field of view of the vehicle. Landing target measurements are not used to aid positioning.
Mode Stationary: The landing target is stationary. Measured velocity w.r.t. the landing target is used to aid velocity estimation.</source>
      <translation>配置着陆目标的模式。根据模式的不同，着陆目标观测结果会以不同的方式用于帮助位置估计。移动模式：着陆目标在飞行器视野范围内可能会移动。着陆目标测量不用于辅助定位。静止模式：着陆目标静止。测量速度 w.r.t.着陆目标用于帮助速度估计。</translation>
    </message>
    <message>
      <source>Configures the frequency of HIGH_LATENCY2 stream for instance 0</source>
      <translation>配置 HIGH_LATENCY2 流的频率，例如 0</translation>
    </message>
    <message>
      <source>Configures which RC channel is used by the receiver to indicate the signal was lost (on receivers that use output a fixed signal value to report lost signal). If set to 0, the channel mapped to throttle is used. Use RC_FAILS_THR to set the threshold indicating lost signal. By default it's below the expected range and hence disabled.</source>
      <translation>配置接收器使用哪个 RC 通道来指示信号丢失（在使用输出固定信号值来报告丢失信号的接收器上）。如果设置为 0，则使用映射到油门的通道。使用 RC_FAILS_THR 设置指示丢失信号的阈值。默认情况下，它低于预期范围，因此被禁用。</translation>
    </message>
    <message>
      <source>Continue</source>
      <translation>继续</translation>
    </message>
    <message>
      <source>Continue if in Guided on RC failsafe only</source>
      <translation>如果仅在 RC 故障安全上处于引导状态，则继续</translation>
    </message>
    <message>
      <source>Control Frame used in stabilized modes</source>
      <translation>稳定模式下使用的控制框架</translation>
    </message>
    <message>
      <source>Control Surface 1</source>
      <translation>控制面 1</translation>
    </message>
    <message>
      <source>Control Surface 2</source>
      <translation>控制面 2</translation>
    </message>
    <message>
      <source>Control Surface 3</source>
      <translation>控制面 3</translation>
    </message>
    <message>
      <source>Control Surface 4</source>
      <translation>控制面 4</translation>
    </message>
    <message>
      <source>Control Surface 5</source>
      <translation>控制面 5</translation>
    </message>
    <message>
      <source>Control Surface 6</source>
      <translation>控制面 6</translation>
    </message>
    <message>
      <source>Control Surface 7</source>
      <translation>控制面 7</translation>
    </message>
    <message>
      <source>Control Surface 8</source>
      <translation>控制面 8</translation>
    </message>
    <message>
      <source>Control if the vehicle has a GPS</source>
      <translation>控制车辆是否有 GPS</translation>
    </message>
    <message>
      <source>Control pitot tube order of 2nd airspeed sensor</source>
      <translation>控制第二空速传感器的皮托管顺序</translation>
    </message>
    <message>
      <source>Control what protocol Serial6 port should be used for. Note that the Frsky options require external converter hardware. See the wiki for details.</source>
      <translation>控制 Serial6 端口应使用什么协议。请注意，Frsky 选项需要外部转换器硬件。详细信息请参阅维基百科。</translation>
    </message>
    <message>
      <source>Controller error threshold</source>
      <translation>控制器错误阈值</translation>
    </message>
    <message>
      <source>Controller time constant (sec)</source>
      <translation>控制器时间常数（秒）</translation>
    </message>
    <message>
      <source>Controls how tightly the output track the EKF states</source>
      <translation>控制输出跟踪 EKF 状态的紧密程度</translation>
    </message>
    <message>
      <source>Controls motor mixing for multicopter component</source>
      <translation>控制多旋翼组件的电机混合</translation>
    </message>
    <message>
      <source>Controls the amount of GPS  measurement delay that the autopilot compensates for. Set to zero to use the default delay for the detected GPS type.Renamed in 4.6 and later to GPS1_DELAY_MS.</source>
      <translation>控制自动驾驶仪补偿的 GPS 测量延迟量。设置为零以使用检测到的 GPS 类型的默认延迟。在 4.6 和更高版本中重命名为 GPS1_DELAY_MS。</translation>
    </message>
    <message>
      <source>Controls the update rate of PWM output. Flight Controller will inform those numbers of update events in a second, to PCA9685. Higher update rate will consume more I2C bandwidth, which may even lead to worse output latency, or completely block I2C bus.</source>
      <translation>控制PWM输出的更新率。飞控会在一秒钟内将更新事件的数量通知给 PCA9685。较高的更新速率将消耗更多的 I2C 带宽，甚至可能导致更差的输出延迟，或完全阻塞 I2C 总线。</translation>
    </message>
    <message>
      <source>Controls what action to take if terrain information is lost during AUTO mode</source>
      <translation>控制在 AUTO 模式期间丢失地形信息时采取的操作</translation>
    </message>
    <message>
      <source>Controls whether body-frame rate feedfoward is enabled or disabled</source>
      <translation>控制是否启用或禁用主体帧速率前馈</translation>
    </message>
    <message>
      <source>Converts pilot roll and pitch into a desired rate of rotation in ACRO and SPORT mode.  Higher values mean faster rate of rotation.</source>
      <translation>在 ACRO 和 SPORT 模式下将引导横滚和俯仰转换为所需的旋转速率。  值越高意味着旋转速度越快。</translation>
    </message>
    <message>
      <source>Copter will fly at at least this altitude (in meters) above home during deadreckoning</source>
      <translation>在航位推算过程中，直升机将至少在离家上方的这个高度（以米为单位）飞行</translation>
    </message>
    <message>
      <source>Crash Angle</source>
      <translation>碰撞角度</translation>
    </message>
    <message>
      <source>Crash throttle minimum</source>
      <translation>碰撞油门最小</translation>
    </message>
    <message>
      <source>Critical sink speed rate in m/s to trigger emergency parachute</source>
      <translation>触发紧急降落伞的临界下降速度（米/秒）</translation>
    </message>
    <message>
      <source>Crosstrack error integrator gain. This gain is applied to the crosstrack error to ensure it converges to zero. Set to zero to disable. Smaller values converge slower, higher values will cause crosstrack error oscillation.</source>
      <translation>交叉跟踪误差积分器增益。该增益应用于交叉跟踪误差，以确保其收敛到零。设置为零以禁用。较小的值收敛速度较慢，较高的值会导致交叉跟踪误差振荡。</translation>
    </message>
    <message>
      <source>CubeOrange_PM2/Navigator</source>
      <translation>CubeOrange_PM2/导航器</translation>
    </message>
    <message>
      <source>Custom Controller bitmask</source>
      <translation>自定义控制器位掩码</translation>
    </message>
    <message>
      <source>Custom euler roll, euler 321 (yaw, pitch, roll) ordering</source>
      <translation>定制欧拉横滚、欧拉321（偏航、俯仰、横滚）订购</translation>
    </message>
    <message>
      <source>Custom stick configuration</source>
      <translation>自定义棒配置</translation>
    </message>
    <message>
      <source>Cut-off frequency for collective low pass filter.  For the glide phase.  Acts as a following trim.  Must be lower than AROT_COL_FILT_E.</source>
      <translation>总距低通滤波器的截止频率。  对于滑行阶段。  充当跟随装饰。  必须低于 AROT_COL_FILT_E。</translation>
    </message>
    <message>
      <source>Cyphal</source>
      <translation>赛法尔</translation>
    </message>
    <message>
      <source>Cyphal ESC 7 zubax feedback port ID</source>
      <translation>Cyphal ESC 7 zubax 反馈端口 ID</translation>
    </message>
    <message>
      <source>D gain</source>
      <translation>D增益</translation>
    </message>
    <message>
      <source>DDFP Tail Rotor Motor Spin minimum</source>
      <translation>DDFP 尾桨电机旋转最小</translation>
    </message>
    <message>
      <source>DDS ping timeout</source>
      <translation>DDS ping 超时</translation>
    </message>
    <message>
      <source>DIA_Y in the compass soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>罗盘软铁校准矩阵中的DIA_Y：[[DIA_X、ODI_X、ODI_Y]、[ODI_X、DIA_Y、ODI_Z]、[ODI_Y、ODI_Z、DIA_Z]]</translation>
    </message>
    <message>
      <source>DISABLED</source>
      <translation>禁用</translation>
    </message>
    <message>
      <source>DJIX</source>
      <translation>DJIX</translation>
    </message>
    <message>
      <source>DSMX</source>
      <translation>DSMX</translation>
    </message>
    <message>
      <source>DTS6012M</source>
      <translation>DTS6012M</translation>
    </message>
    <message>
      <source>Damping gain for throttle demand loop. Slows the throttle response to correct for speed and height oscillations.</source>
      <translation>油门需求回路的阻尼增益。减慢油门响应以纠正速度和高度振荡。</translation>
    </message>
    <message>
      <source>Data stuck (triggers if data is exactly constant for 2s in FW mode)</source>
      <translation>数据卡住（如果在 FW 模式下 2s 的数据完全恒定则触发）</translation>
    </message>
    <message>
      <source>DeadReckoning is available for this many seconds after losing position and/or velocity source.  After this timeout elapses the EKF failsafe will trigger in modes requiring a position estimate</source>
      <translation>在失去位置和/或速度源后，航位推算可以持续这么多秒。  超时过后，EKF 故障保护将在需要位置估计的模式下触发</translation>
    </message>
    <message>
      <source>Deadreckoning GPS speed accuracy maximum threshold</source>
      <translation>航位推算GPS速度精度最大阈值</translation>
    </message>
    <message>
      <source>Debugging options</source>
      <translation>调试选项</translation>
    </message>
    <message>
      <source>Deepstall L1 navigational controller period</source>
      <translation>深失速L1导航控制器周期</translation>
    </message>
    <message>
      <source>Deepstall minimum abort altitude</source>
      <translation>深失速最小中止高度</translation>
    </message>
    <message>
      <source>Default Target sysID for the mount to point to</source>
      <translation>挂载点的默认目标 sysID</translation>
    </message>
    <message>
      <source>Default gain at boot, must be in range [JS_GAIN_MIN , JS_GAIN_MAX]</source>
      <translation>启动时的默认增益，必须在 [JS_GAIN_MIN , JS_GAIN_MAX] 范围内</translation>
    </message>
    <message>
      <source>Default rate is 250Kbps, which is used in off-the-shelf MoadalAI ESC products.</source>
      <translation>默认速率为250Kbps，用于现成的MoadalAI ESC产品。</translation>
    </message>
    <message>
      <source>Default target sinkrate</source>
      <translation>默认目标沉降率</translation>
    </message>
    <message>
      <source>Default+RCIN</source>
      <translation>默认+RCIN</translation>
    </message>
    <message>
      <source>Defined as corrective acceleration in m/s^2 per m/s^2 velocity derivative</source>
      <translation>定义为每 m/s^2 速度导数的修正加速度（以 m/s^2 为单位）</translation>
    </message>
    <message>
      <source>Defines how the throttle stick is mapped to collective thrust in Stabilized mode. Rescale to hover thrust estimate: Stick input is linearly rescaled, such that a centered throttle stick corresponds to the hover thrust estimator's output. No rescale: Directly map the stick 1:1 to the output. Can be useful with very low hover thrust which leads to much distortion and the upper half getting sensitive. Rescale to hover thrust parameter: Similar to rescaling to the hover thrust estimate, but it uses the hover thrust parameter value (see MPC_THR_HOVER) instead of estimated value. With MPC_THR_HOVER 0.5 it's equivalent to No rescale.</source>
      <translation>定义在稳定模式下油门杆如何映射到总距推力。重新调整为悬停推力估计：摇杆输入线性重新调整，使得居中的油门摇杆对应于悬停推力估计器的输出。不重新缩放：直接将棒 1:1 映射到输出。对于非常低的悬停推力非常有用，这会导致很大的失真并且上半部分变得敏感。重新调整为悬停推力参数：与重新调整为悬停推力估计类似，但它使用悬停推力参数值（请参阅 MPC_THR_HOVER）而不是估计值。对于 MPC_THR_HOVER 0.5，它相当于不重新缩放。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL15_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL15_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL22_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL22_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL2_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL2_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL37_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL37_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL44_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL44_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL51_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL51_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL59_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL59_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL8_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL8_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL14_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL14_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL21_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL21_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL29_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL29_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL36_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL36_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL43_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL43_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL50_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL50_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL58_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL58_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the maximum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL7_MAXA.</source>
      <translation>定义舵机最大时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL7_MAXA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL13_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL13_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL20_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL20_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL28_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL28_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL35_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL35_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL42_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL42_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL4_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL4_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL57_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL57_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum.
Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL6_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL6_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL12_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL12_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL1_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL1_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL27_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL27_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL34_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL34_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL41_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL41_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL49_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL49_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL56_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL56_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the angle when the servo is at the minimum. Currently only supported in gz simulation and must be coherent with .sdf file and CA_SV_TL63_MINA.</source>
      <translation>定义舵机最小时的角度。目前仅支持 gz 模拟，并且必须与 .sdf 文件和 CA_SV_TL63_MINA 一致。</translation>
    </message>
    <message>
      <source>Defines the capacity of battery 3 in mAh.</source>
      <translation>定义电池 3 的容量（以 mAh 为单位）。</translation>
    </message>
    <message>
      <source>Defines the collective pitch at the interval position 3 for a given thrust setpoint. Use negative values if the swash plate needs to move down to provide upwards thrust.</source>
      <translation>定义给定推力设定值的间隔位置 3 处的总距桨距。如果斜盘需要向下移动以提供向上的推力，请使用负值。</translation>
    </message>
    <message>
      <source>Defines the distance from the waypoint center, the plane will maintain during a loiter. If you set this value to a negative number then the default loiter direction will be counter-clockwise instead of clockwise. If this value is too close to zero, the achieved loiter radius will be determined by ROLL_LIMIT_DEG.</source>
      <translation>定义飞机在盘旋期间与航路点中心的距离。如果将此值设置为负数，则默认盘旋方向将为逆时针方向而不是顺时针方向。如果该值太接近零，则达到的盘旋半径将由 ROLL_LIMIT_DEG 确定。</translation>
    </message>
    <message>
      <source>Defines the maximum distance from a waypoint that when crossed indicates the waypoint may be complete. To avoid the aircraft looping around the waypoint in case it misses by more than the WP_RADIUS an additional check is made to see if the aircraft has crossed a "finish line" passing through the waypoint and perpendicular to the flight path from the previous waypoint. If that finish line is crossed then the waypoint is considered complete. Note that the navigation controller may decide to turn later than WP_RADIUS before a waypoint, based on how sharp the turn is and the speed of the aircraft. It is safe to set WP_RADIUS much larger than the usual turn radius of your aircraft and the navigation controller will work out when to turn. If you set WP_RADIUS too small then you will tend to overshoot the turns.</source>
      <translation>定义距航路点的最大距离，跨越该距离时表明该航路点可能已完成。为了避免飞机在错过超过 WP_RADIUS 的情况下绕航路点循环，需要进行额外检查，看看飞机是否越过穿过该航路点并垂直于前一个航路点的飞行路径的“终点线”。如果越过终点线，则该航路点被视为完成。请注意，导航控制器可能会根据转弯的急速和飞机的速度决定在航路点之前比 WP_RADIUS 更晚转弯。将 WP_RADIUS 设置为比飞机通常的转弯半径大得多是安全的，导航控制器将计算出何时转弯。如果您将 WP_RADIUS 设置得太小，那么您将倾向于超调转弯。</translation>
    </message>
    <message>
      <source>Defines the output throttle at the interval position 0.</source>
      <translation>定义间隔位置 0 处的输出油门。</translation>
    </message>
    <message>
      <source>Defines the range of the hover thrust estimate around MPC_THR_HOVER.
A value of 0.2 with MPC_THR_HOVER at 0.5 results in a range of [0.3, 0.7].
Set to a large value if the vehicle operates in varying physical conditions that
affect the required hover thrust strongly (e.g. differently sized payloads).</source>
      <translation>定义 MPC_THR_HOVER 周围的悬停推力估计范围。 0.2 和 MPC_THR_HOVER 位于 0.5 的值会产生 [0.3, 0.7] 范围。如果飞行器在强烈影响所需悬停推力的不同物理条件下运行（例如不同大小的有效负载），则设置为较大值。</translation>
    </message>
    <message>
      <source>Defines the speed in cm/s which the aircraft will attempt to maintain while descending during a WP mission</source>
      <translation>定义飞机在 WP 任务期间下降时尝试保持的速度（以厘米/秒为单位）</translation>
    </message>
    <message>
      <source>Defines the voltage where a single cell of the battery is considered empty. The voltage should be chosen above the steep dropoff at 3.5V. A typical lithium battery can only be discharged under high load down to 10% before it drops off to a voltage level damaging the cells.</source>
      <translation>定义电池的单个电池被视为空的电压。应选择高于 3.5V 急剧下降的电压。典型的锂电池只能在高负载下放电至 10%，然后才会下降到损坏电池的电压水平。</translation>
    </message>
    <message>
      <source>Defines which mixer implementation to use.
Some are generic, while others are specifically fit to a certain vehicle with a fixed set of actuators.
'Custom' should only be used if noting else can be used.</source>
      <translation>定义要使用的混合器实现。有些是通用的，而另一些则专门适合具有一组固定执行器的特定车辆。仅当可以使用其他说明时才应使用“自定义”。</translation>
    </message>
    <message>
      <source>Delay Spoolup-delay VTOL spoolup for 2 seconds after arming</source>
      <translation>延迟电机升速：解锁后将 VTOL 电机升速延迟 2 秒</translation>
    </message>
    <message>
      <source>Delay before landing (after initial descent) in Return mode.
If set to -1 the system will not land but loiter at RTL_DESCEND_ALT.</source>
      <translation>返回模式下着陆前（初始下降后）延迟。如果设置为 -1，系统将不会着陆，而是在 RTL_DESCEND_ALT 处盘旋。</translation>
    </message>
    <message>
      <source>Delay in seconds before GPS acquires lock</source>
      <translation>GPS 获取锁之前的延迟（以秒为单位）</translation>
    </message>
    <message>
      <source>Depth reading at surface</source>
      <translation>表面深度读数</translation>
    </message>
    <message>
      <source>Descent velocity in autonomous modes</source>
      <translation>自主模式下的下降速度</translation>
    </message>
    <message>
      <source>Desired pivot turn rate in deg/s.</source>
      <translation>所需的枢轴转动速率（以度/秒为单位）。</translation>
    </message>
    <message>
      <source>Determines how the autopilot controls the yaw during missions and RTL</source>
      <translation>确定自动驾驶仪在执行任务和 RTL 期间如何控制偏航</translation>
    </message>
    <message>
      <source>Determines what the aircraft will do after a fail event is resolved</source>
      <translation>确定故障事件解决后飞机将做什么</translation>
    </message>
    <message>
      <source>Device ID of simulated compass 1</source>
      <translation>模拟罗盘1的装置ID</translation>
    </message>
    <message>
      <source>Device ID of the accelerometer this calibration applies to.</source>
      <translation>此校准适用的加速度计的设备 ID。</translation>
    </message>
    <message>
      <source>Differential Collective Pitch Trim</source>
      <translation>差动总距桨距调整</translation>
    </message>
    <message>
      <source>Differential spoiler crow flaps inner weight</source>
      <translation>差速扰流乌鸦襟翼内部重量</translation>
    </message>
    <message>
      <source>DifferentialSpoiler2</source>
      <translation>差速扰流器2</translation>
    </message>
    <message>
      <source>Digital pin number for 3rd relay control.</source>
      <translation>第三继电器控制的数字引脚号。</translation>
    </message>
    <message>
      <source>Digital pin number for first button input. </source>
      <translation>第一个按钮输入的数字引脚编号。 </translation>
    </message>
    <message>
      <source>Digital pin number for second button input.  Some common values are given, but see the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.</source>
      <translation>第二个按钮输入的数字引脚编号。  给出了一些常见值，但请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。</translation>
    </message>
    <message>
      <source>Direct Drive Fixed Pitch (Tail RSC only)</source>
      <translation>直接驱动固定俯仰（仅限尾部 RSC）</translation>
    </message>
    <message>
      <source>DirectDrive VarPitch</source>
      <translation>直接驱动可变俯仰</translation>
    </message>
    <message>
      <source>Directory disable</source>
      <translation>目录禁用</translation>
    </message>
    <message>
      <source>Disable Ground Effect Compensation</source>
      <translation>禁用地面效应补偿</translation>
    </message>
    <message>
      <source>Disable force fixed wing controller recovery</source>
      <translation>禁用强制固定翼控制器恢复</translation>
    </message>
    <message>
      <source>Disable prearm display</source>
      <translation>禁用预报警显示</translation>
    </message>
    <message>
      <source>Disable the terrain estimate</source>
      <translation>禁用地形估计</translation>
    </message>
    <message>
      <source>Disable to use the fixed parameter MPC_THR_HOVER instead of the hover thrust estimate in the position controller.
This parameter does not influence Stabilized mode throttle curve (see MPC_THR_CURVE).</source>
      <translation>禁止使用固定参数 MPC_THR_HOVER 代替位置控制器中的悬停推力估计。该参数不影响稳定模式油门曲线（参见MPC_THR_CURVE）。</translation>
    </message>
    <message>
      <source>DisableDCMFallbackVTOL</source>
      <translation>禁用DCMFallbackVTOL</translation>
    </message>
    <message>
      <source>DisableRedineGovernor</source>
      <translation>禁用RedineGovernor</translation>
    </message>
    <message>
      <source>Disables GPS 2</source>
      <translation>禁用 GPS 2</translation>
    </message>
    <message>
      <source>Disarm delay</source>
      <translation>上锁延迟</translation>
    </message>
    <message>
      <source>Disk Theory</source>
      <translation>圆盘理论</translation>
    </message>
    <message>
      <source>Displays Height above terrain</source>
      <translation>显示高于地形的高度</translation>
    </message>
    <message>
      <source>Displays a clock panel based on AP_RTC local time</source>
      <translation>显示基于 AP_RTC 本地时间的时钟面板</translation>
    </message>
    <message>
      <source>Displays artificial horizon crosshair (MSP OSD only)</source>
      <translation>显示人工地平线十字准线（仅限 MSP OSD）</translation>
    </message>
    <message>
      <source>Displays callsign from callsign.txt on microSD card</source>
      <translation>显示 microSD 卡上 callsign.txt 中的呼号</translation>
    </message>
    <message>
      <source>Displays first esc's current</source>
      <translation>显示第一个 esc 的当前</translation>
    </message>
    <message>
      <source>Displays highest temp of all active ESCs, or of a specific ECS if OSDx_ESC_IDX is set</source>
      <translation>显示所有活动 ESC 的最高温度，如果设置了 OSDx_ESC_IDX，则显示特定 ECS 的最高温度</translation>
    </message>
    <message>
      <source>Displays pluscode (OLC) element</source>
      <translation>显示 pluscode (OLC) 元素</translation>
    </message>
    <message>
      <source>Displays temperature reported by secondary barometer</source>
      <translation>显示辅助气压计报告的温度</translation>
    </message>
    <message>
      <source>Displays wind speed and relative direction, on Rover this is the apparent wind speed and direction from the windvane, if fitted</source>
      <translation>显示风速和相对方向，在流动站上，这是来自风向标（如果安装）的表观风速和方向</translation>
    </message>
    <message>
      <source>Distance Sensor Rotation as MAV_SENSOR_ORIENTATION enum</source>
      <translation>距离传感器旋转为 MAV_SENSOR_ORIENTATION 枚举</translation>
    </message>
    <message>
      <source>Distance from a waypoint, that when crossed indicates the wp has been reached.</source>
      <translation>距航路点的距离，交叉时表示已到达 wp。</translation>
    </message>
    <message>
      <source>Distance in meters between camera triggers. If this value is non-zero then the camera will trigger whenever the GPS position changes by this number of meters regardless of what mode the APM is in. Note that this parameter can also be set in an auto mission using the DO_SET_CAM_TRIGG_DIST command, allowing you to enable/disable the triggering of the camera during the flight.</source>
      <translation>相机触发器之间的距离（以米为单位）。如果该值非零，则无论 APM 处于何种模式，每当 GPS 位置改变此米数时，相机都会触发。请注意，该参数也可以在自动任务中使用 DO_SET_CAM_TRIGG_DIST 命令设置，允许您在飞行过程中启用/禁用相机的触发。</translation>
    </message>
    <message>
      <source>Do Not Use in RTL and Land</source>
      <translation>请勿在 RTL 和陆地上使用</translation>
    </message>
    <message>
      <source>Do not require offset calibration before flight. Manual calibration should be performed during initial setup.</source>
      <translation>飞行前不需要偏移校准。应在初始设置期间执行手动校准。</translation>
    </message>
    <message>
      <source>Dock</source>
      <translation>码头</translation>
    </message>
    <message>
      <source>Don't forward mavlink to/from</source>
      <translation>不要将 mavlink 转发到/从</translation>
    </message>
    <message>
      <source>Double notch</source>
      <translation>双缺口</translation>
    </message>
    <message>
      <source>Driving mode for switch position 1 (910 to 1230 and above 2049)</source>
      <translation>开关位置驱动方式 1（910 至 1230 及以上 2049）</translation>
    </message>
    <message>
      <source>DroneCAN Hardpoint ID 0</source>
      <translation>DroneCAN 硬点 ID 0</translation>
    </message>
    <message>
      <source>DroneCAN Hardpoint ID 2</source>
      <translation>DroneCAN 硬点 ID 2</translation>
    </message>
    <message>
      <source>DroneCAN Sensor ID</source>
      <translation>DroneCAN 传感器 ID</translation>
    </message>
    <message>
      <source>DroneCAN node that is used for this network</source>
      <translation>用于该网络的 DroneCAN 节点</translation>
    </message>
    <message>
      <source>Drop</source>
      <translation>掉落</translation>
    </message>
    <message>
      <source>Duration in seconds that the camera shutter is held open</source>
      <translation>相机快门保持打开的持续时间（以秒为单位）</translation>
    </message>
    <message>
      <source>Duration of the input signal sent on each axis during system identification</source>
      <translation>系统识别期间每个轴上发送的输入信号的持续时间</translation>
    </message>
    <message>
      <source>Dynamically simulate failure of airspeed sensor instance</source>
      <translation>动态模拟空速传感器故障实例</translation>
    </message>
    <message>
      <source>EFI Calibration Coefficient 1</source>
      <translation>EFI 校准系数 1</translation>
    </message>
    <message>
      <source>EFI INF throttle output rate</source>
      <translation>EFI INF 油门输出率</translation>
    </message>
    <message>
      <source>EKF Failsafe Action</source>
      <translation>EKF 故障保护动作</translation>
    </message>
    <message>
      <source>EKF Source OpticalFlow Quality Threshold</source>
      <translation>EKF 源光流质量阈值</translation>
    </message>
    <message>
      <source>EKF optional behaviour. Bit 0 (JammingExpected): Setting JammingExpected will change the EKF behaviour such that if dead reckoning navigation is possible it will require the preflight alignment GPS quality checks controlled by EK3_GPS_CHECK and EK3_CHECK_SCALE to pass before resuming GPS use if GPS lock is lost for more than 2 seconds to prevent bad position estimate. Bit 1 (Manual lane switching): DANGEROUS – If enabled, this disables automatic lane switching. If the active lane becomes unhealthy, no automatic switching will occur. Users must manually set EK3_PRIMARY to change lanes. No health checks will be performed on the selected lane. Use with extreme caution.  Bit 2 (Optflow may use terrain alt): Terrain SRTM data will be used if the vehicle climbs above the rangefinder's range allowing optical flow to be used at higher altitudes. Bit 3 (AGL KF for optflow scaling): Use a 2-state IMU-aided AGL Kalman filter (height + vertical velocity, fused with rangefinder) to compute the height-above-ground used for optical flow velocity scaling, instead of terrainState-pd. This decouples optical flow scaling from errors in the main filter's vertical position state.</source>
      <translation>EKF 可选行为。位 0（JammingExpected）：设置 JammingExpected 将改变 EKF 的行为，这样，如果可以进行航位推测导航，则如果 GPS 锁定丢失超过 2 秒，则需要在恢复 GPS 使用之前通过由 EK3_GPS_CHECK 和 EK3_CHECK_SCALE 控制的飞行前对准 GPS 质量检查，以防止错误的位置估计。位 1（手动通道切换）：DANGEROUS – 如果启用，则会禁用自动通道切换。如果活动车道变得不健康，则不会发生自动切换。用户必须手动设置EK3_PRIMARY才能改变车道。不会对所选车道执行健康检查。请务必谨慎使用。  位 2（Optflow 可能使用地形 alt）：如果车辆爬升到测距仪范围以上，则将使用地形 SRTM 数据，从而允许在更高高度使用光流。位 3（用于 optflow 缩放的 AGL KF）：使用 2 状态 IMU 辅助 AGL 卡尔曼滤波器（高度 + 垂直速度，与测距仪融合）来计算用于光流速度缩放的地面高度，而不是地形状态 pd。这将光流缩放与主滤波器垂直位置状态的误差解耦。</translation>
    </message>
    <message>
      <source>EKF2 selector acceleration error threshold for comparing accelerometers. Acceleration vector differences larger than this will result in accumulated velocity error.</source>
      <translation>EKF2 选择器加速度误差阈值，用于比较加速度计。大于此的加速度矢量差将导致累积速度误差。</translation>
    </message>
    <message>
      <source>EO1</source>
      <translation>EO1</translation>
    </message>
    <message>
      <source>ERB</source>
      <translation>ERB</translation>
    </message>
    <message>
      <source>ESC 14</source>
      <translation>ESC 14</translation>
    </message>
    <message>
      <source>ESC 21</source>
      <translation>ESC 21</translation>
    </message>
    <message>
      <source>ESC 29</source>
      <translation>ESC 29</translation>
    </message>
    <message>
      <source>ESC 7</source>
      <translation>ESC 7</translation>
    </message>
    <message>
      <source>ESC Telemetry</source>
      <translation>ESC遥测</translation>
    </message>
    <message>
      <source>ESC channels</source>
      <translation>ESC 通道</translation>
    </message>
    <message>
      <source>ESCRPM_Y</source>
      <translation>ESCRPM_Y</translation>
    </message>
    <message>
      <source>EXT2</source>
      <translation>EXT2</translation>
    </message>
    <message>
      <source>Elevator</source>
      <translation>电梯</translation>
    </message>
    <message>
      <source>Emergency threshold</source>
      <translation>紧急阈值</translation>
    </message>
    <message>
      <source>Empty</source>
      <translation>空</translation>
    </message>
    <message>
      <source>Enable AFS for all autonomous modes (not just AUTO)</source>
      <translation>为所有自主模式启用 AFS（不仅仅是 AUTO）</translation>
    </message>
    <message>
      <source>Enable CAN1 multicast endpoint</source>
      <translation>启用 CAN1 多播端点</translation>
    </message>
    <message>
      <source>Enable Debug Pins</source>
      <translation>启用调试引脚</translation>
    </message>
    <message>
      <source>Enable EKF2</source>
      <translation>启用EKF2</translation>
    </message>
    <message>
      <source>Enable GPS output of NMEA heading HDT sentence or UBLOX_RELPOSNED</source>
      <translation>使能 GPS 输出 NMEA 标题 HDT 句子或 UBLOX_RELPOSNED</translation>
    </message>
    <message>
      <source>Enable Hobbywing ESC telemetry</source>
      <translation>启用好盈 ESC 遥测</translation>
    </message>
    <message>
      <source>Enable INA220 Power Monitor</source>
      <translation>启用 INA220 功率监视器</translation>
    </message>
    <message>
      <source>Enable MAVLink Message forwarding for instance 2</source>
      <translation>为实例 2 启用 MAVLink 消息转发</translation>
    </message>
    <message>
      <source>Enable MicroStrain internal heading aiding</source>
      <translation>启用 MicroStrain 内部航向辅助</translation>
    </message>
    <message>
      <source>Enable QuadPlane</source>
      <translation>启用四平面</translation>
    </message>
    <message>
      <source>Enable SBG Systems INS configuration through sbgECom driver on start.</source>
      <translation>启动时通过 sbgECom 驱动程序启用 SBG 系统 INS 配置。</translation>
    </message>
    <message>
      <source>Enable UAVCAN Arming Status stream publication
uavcan::equipment::safety::ArmingStatus</source>
      <translation>启用 UAVCAN 解锁状态流发布 uavcan::equipment::safety::ArmingStatus</translation>
    </message>
    <message>
      <source>Enable UAVCAN RTCM stream publication
ardupilot::gnss::MovingBaselineData</source>
      <translation>启用 UAVCAN RTCM 流发布 ardupilot::gnss::MovingBaselineData</translation>
    </message>
    <message>
      <source>Enable UAVCAN battery subscription.
uavcan::equipment::power::BatteryInfo
ardupilot::equipment::power::BatteryInfoAux
cuav::equipment::power::CBAT
0 - Disable
1 - Use raw data. Recommended for Smart battery
2 - Filter the data with internal battery library (unsupported with CBAT)</source>
      <translation>启用 UAVCAN 电池订阅。 uavcan::equipment::power::BatteryInfo ardupilot::equipment::power::BatteryInfoAux cuav::equipment::power::CBAT 0 - 禁用 1 - 使用原始数据。推荐用于智能电池 2 - 使用内部电池库过滤数据（CBAT 不支持）</translation>
    </message>
    <message>
      <source>Enable UAVCAN hygrometer subscriptions. dronecan::sensors::hygrometer::Hygrometer</source>
      <translation>启用 UAVCAN 湿度计订阅。无人机::传感器::湿度计::湿度计</translation>
    </message>
    <message>
      <source>Enable USB autostart</source>
      <translation>启用 USB 自动启动</translation>
    </message>
    <message>
      <source>Enable all modes</source>
      <translation>启用所有模式</translation>
    </message>
    <message>
      <source>Enable automatic configuration</source>
      <translation>启用自动配置</translation>
    </message>
    <message>
      <source>Enable barometer/air pressure sensor in Gazebo bridge</source>
      <translation>在 Gazebo 桥中启用气压计/气压传感器</translation>
    </message>
    <message>
      <source>Enable differential thrust seperately for roll, pitch, yaw in forward (fixed-wing) mode. The effectiveness of differential thrust around the corresponding axis can be tuned by setting VT_FW_DIFTHR_S_R / VT_FW_DIFTHR_S_P / VT_FW_DIFTHR_S_Y.</source>
      <translation>在前向（固定翼）模式下，分别为横滚、俯仰、偏航启用差动推力。绕相应轴的差推力的有效性可以通过设置VT_FW_DIFTHR_S_R / VT_FW_DIFTHR_S_P / VT_FW_DIFTHR_S_Y 来调整。</translation>
    </message>
    <message>
      <source>Enable flow control on serial 2 (telemetry 2) on Pixhawk and STATE. You must have the RTS and CTS pins connected to your radio. The standard DF13 6 pin connector for a 3DR radio does have those pins connected. If this is set to 2 then flow control will be auto-detected by checking for the output buffer filling on startup.</source>
      <translation>在 Pixhawk 和 STATE 上启用串行 2（遥测 2）的流量控制。您必须将 RTS 和 CTS 引脚连接到您的无线电。用于 3DR 无线电的标准 DF13 6 引脚连接器确实已连接这些引脚。如果设置为 2，则将通过在启动时检查输出缓冲区填充来自动检测流量控制。</translation>
    </message>
    <message>
      <source>Enable force safety</source>
      <translation>启用强制安全</translation>
    </message>
    <message>
      <source>Enable internal barometers</source>
      <translation>启用内部气压计</translation>
    </message>
    <message>
      <source>Enable mag strength preflight check</source>
      <translation>启用磁力强度预检检查</translation>
    </message>
    <message>
      <source>Enable online mag bias calibration</source>
      <translation>启用在线磁偏置校准</translation>
    </message>
    <message>
      <source>Enable or disable the automatic learning of compass offsets. You can enable learning either using a compass-only method that is suitable only for fixed wing aircraft or using the offsets learnt by the active EKF state estimator. If this option is enabled then the learnt offsets are saved when you disarm the vehicle.</source>
      <translation>启用或禁用罗盘偏移的自动学习。您可以使用仅适用于固定翼飞机的仅罗盘方法或使用主动 EKF 状态估计器学习的偏移来启用学习。如果启用此选项，那么当您上锁车辆时，将保存学习到的偏移量。</translation>
    </message>
    <message>
      <source>Enable or disable the tertiary compass for determining heading.</source>
      <translation>启用或禁用第三罗盘以确定航向。</translation>
    </message>
    <message>
      <source>Enable override during offboard mode</source>
      <translation>在离机模式下启用覆盖</translation>
    </message>
    <message>
      <source>Enable pullup after altitude wait</source>
      <translation>高度等待后启用上拉</translation>
    </message>
    <message>
      <source>Enable scripting serial devices</source>
      <translation>启用串行设备脚本</translation>
    </message>
    <message>
      <source>Enable ship landing simulation</source>
      <translation>启用船舶着陆模拟</translation>
    </message>
    <message>
      <source>Enable simulated magnetometer sensor instance</source>
      <translation>启用模拟磁力计传感器实例</translation>
    </message>
    <message>
      <source>Enable steering speed scaling</source>
      <translation>启用转向速度缩放</translation>
    </message>
    <message>
      <source>Enable the PCA9685 output driver.
The integer refers to the I2C bus number where PCA9685 is connected.</source>
      <translation>使能 PCA9685 输出驱动器。整数是指连接 PCA9685 的 I2C 总线编号。</translation>
    </message>
    <message>
      <source>Enable this screen</source>
      <translation>启用此屏幕</translation>
    </message>
    <message>
      <source>Enable use of rangefinder</source>
      <translation>启用测距仪的使用</translation>
    </message>
    <message>
      <source>Enable wheel steering controller</source>
      <translation>启用车轮转向控制器</translation>
    </message>
    <message>
      <source>Enable/Disable automatic learning of hover throttle</source>
      <translation>启用/禁用悬停油门自动学习</translation>
    </message>
    <message>
      <source>EnableAirspeedAffinity</source>
      <translation>启用AirspeedAffinity</translation>
    </message>
    <message>
      <source>EnableDefaultAirspeed for takeoff</source>
      <translation>启用起飞默认空速</translation>
    </message>
    <message>
      <source>EnableLogging</source>
      <translation>启用日志记录</translation>
    </message>
    <message>
      <source>EnableWhenArmed</source>
      <translation>解锁时启用</translation>
    </message>
    <message>
      <source>Enabled Continue with Mission in Auto Mode (Deprecated in 4.0+)</source>
      <translation>自动模式继续任务</translation>
    </message>
    <message>
      <source>Enabled constantly</source>
      <translation>持续启用</translation>
    </message>
    <message>
      <source>Enabled including attitude reporting</source>
      <translation>已启用，包括态度报告</translation>
    </message>
    <message>
      <source>Enabled/disable avoidance input sources</source>
      <translation>启用/禁用回避输入源</translation>
    </message>
    <message>
      <source>Enables airspeed use for automatic throttle modes and replaces control from THR_TRIM. Continues to display and log airspeed if set to 0. Uses airspeed for control if set to 1. Only uses airspeed when throttle = 0 if set to 2 (useful for gliders with airspeed sensors behind propellers).</source>
      <translation>启用自动油门模式的空速并取代 THR_TRIM 的控制。如果设置为 0，则继续显示和记录空速。如果设置为 1，则使用空速进行控制。如果设置为 2，则仅在油门 = 0 时使用空速（对于螺旋桨后面带有空速传感器的滑翔机有用）。</translation>
    </message>
    <message>
      <source>Enables leak detector 1. Use this parameter to indicate the signal type (0:analog, 1:digital) of an appropriately configured input pin, then specify its pin number using the LEAK1_PIN parameter. NOT FOR USE by default with Pixhawk, Pixhawk 4 or Navigator flight controllers.</source>
      <translation>启用检漏仪 1。使用此参数指示适当配置的输入引脚的信号类型（0：模拟，1：数字），然后使用 LEAK1_PIN 参数指定其引脚编号。 NOT FOR USE 默认搭配 Pixhawk、Pixhawk 4 或 Navigator 飞行控制器。</translation>
    </message>
    <message>
      <source>Enables restart if a fault is detected during the running state. Otherwise commands continues in running state until given an user request off.</source>
      <translation>如果在运行状态下检测到故障，则可以重新启动。否则，命令将继续处于运行状态，直到用户请求关闭为止。</translation>
    </message>
    <message>
      <source>Enables the setting SERVOn_TRIM values to current levels when switching out of MANUAL mode. Should not be left on as mode switches while the plane is rolling or pitching can cause invalid trim values and subsequent unstable behavior. See newer and safer SERVO_AUTO_TRIM parameter for automated results.</source>
      <translation>当退出 MANUAL 模式时，启用将 SERVOn_TRIM 值设置为当前级别。不应保持打开状态，因为飞机滚动或俯仰时模式开关可能会导致无效的配平值和随后的不稳定行为。查看更新、更安全的 SERVO_AUTO_TRIM 参数以获取自动化结果。</translation>
    </message>
    <message>
      <source>Enabling this option will provide debug messages</source>
      <translation>启用此选项将提供调试消息</translation>
    </message>
    <message>
      <source>Enforce for all modes</source>
      <translation>对所有模式强制执行</translation>
    </message>
    <message>
      <source>Engine start/stop input source</source>
      <translation>发动机启动/停止输入源</translation>
    </message>
    <message>
      <source>Equivalent airspeed measurement noise (m/s)</source>
      <translation>等效空速测量噪声（m/s）</translation>
    </message>
    <message>
      <source>Expect and require a healthy MAVLink parachute system</source>
      <translation>期望并需要一个健康的 MAVLink 降落伞系统</translation>
    </message>
    <message>
      <source>Explicitly specify a state for the generator to be in</source>
      <translation>显式指定生成器所处的状态</translation>
    </message>
    <message>
      <source>External AHRS options</source>
      <translation>外部 AHRS 选项</translation>
    </message>
    <message>
      <source>External Heading (first valid external heading will be used to initialize the filter)</source>
      <translation>外部标题（第一个有效的外部标题将用于初始化过滤器）</translation>
    </message>
    <message>
      <source>External Mode 6</source>
      <translation>外部模式 6</translation>
    </message>
    <message>
      <source>External barometers to probe</source>
      <translation>外部气压计进行探测</translation>
    </message>
    <message>
      <source>External mode identifier 6</source>
      <translation>外部模式标识符 6</translation>
    </message>
    <message>
      <source>External yaw sensor with compass fallback (Deprecated in 4.1+ see EK3_SRCn_YAW)</source>
      <translation>带罗盘回退功能的外部偏航传感器（在 4.1+ 中已弃用，请参阅 EK3_SRCn_YAW）</translation>
    </message>
    <message>
      <source>Extra 6th compass's device id.  Automatically detected, do not set manually</source>
      <translation>额外的第 6 个罗盘的设备 ID。  自动检测，无需手动设置</translation>
    </message>
    <message>
      <source>Extra data type 3 stream rate</source>
      <translation>额外数据类型 3 流速率</translation>
    </message>
    <message>
      <source>FBWA</source>
      <translation>FBWA</translation>
    </message>
    <message>
      <source>FENCE_X</source>
      <translation>FENCE_X</translation>
    </message>
    <message>
      <source>FF Gain which produces an output value that is proportional to the demanded input</source>
      <translation>FF 增益，产生与所需输入成比例的输出值</translation>
    </message>
    <message>
      <source>FFT harmonic fit frequency threshold</source>
      <translation>FFT 谐波拟合频率阈值</translation>
    </message>
    <message>
      <source>FFT options</source>
      <translation>FFT 选项</translation>
    </message>
    <message>
      <source>FLTIME_EN</source>
      <translation>FLTIME_EN</translation>
    </message>
    <message>
      <source>FOLL_SYSID must be set </source>
      <translation>必须设置FOLL_SYSID </translation>
    </message>
    <message>
      <source>FW Input Pitch Angle</source>
      <translation>FW 输入俯仰角</translation>
    </message>
    <message>
      <source>Face next waypoint except RTL</source>
      <translation>面向除 RTL 之外的下一个航路点</translation>
    </message>
    <message>
      <source>Failsafe channel mapping</source>
      <translation>故障安全通道映射</translation>
    </message>
    <message>
      <source>FailureDetector Max Pitch</source>
      <translation>故障检测器最大间距</translation>
    </message>
    <message>
      <source>Fast</source>
      <translation>快</translation>
    </message>
    <message>
      <source>FastSBUS</source>
      <translation>快速SBUS</translation>
    </message>
    <message>
      <source>Femtomes</source>
      <translation>母体</translation>
    </message>
    <message>
      <source>Fence Maximum Altitude</source>
      <translation>围栏最大高度</translation>
    </message>
    <message>
      <source>Fence automatic enable</source>
      <translation>围栏自动启用</translation>
    </message>
    <message>
      <source>Fifth Gyro bias</source>
      <translation>第五陀螺仪偏置</translation>
    </message>
    <message>
      <source>FifthEKF</source>
      <translation>第五次EKF</translation>
    </message>
    <message>
      <source>Filter cutoff frequency for gyroscopes. This can be set to a lower value to try to cope with very high vibration levels in aircraft. A value of zero means no filtering (not recommended!)</source>
      <translation>陀螺仪的滤波器截止频率。可以将其设置为较低的值，以尝试应对飞机中非常高的振动水平。值为零表示不过滤（不推荐！）</translation>
    </message>
    <message>
      <source>Final approach waypoint alt</source>
      <translation>最终进近航路点替代</translation>
    </message>
    <message>
      <source>First Gyro bias on X axis</source>
      <translation>X 轴上的第一个陀螺仪偏置</translation>
    </message>
    <message>
      <source>First TOFSENSE-M sensors backend Instance. Setting this to 1 will pick the first backend from PRX_ or RNG_ Parameters (Depending on TOFSENSE_PRX)</source>
      <translation>第一个 TOFSENSE-M 传感器后端实例。设置为 1 将从 PRX_ 或 RNG_ 参数中选择第一个后端（取决于 TOFSENSE_PRX）</translation>
    </message>
    <message>
      <source>First octet of the server IP address to connect to</source>
      <translation>要连接的服务器 IP 地址的第一个八位字节</translation>
    </message>
    <message>
      <source>FirstBaro</source>
      <translation>第一巴罗</translation>
    </message>
    <message>
      <source>FirstRaw</source>
      <translation>第一原料</translation>
    </message>
    <message>
      <source>Fixed offset applied to yaw output to minimize yaw I-term contribution needed to counter rotor drag. Currently only works of DDFP tails (H_TAIL_TYPE = 3 or H_TAIL_TYPE = 4). If using the H_COL2YAW compensation this trim is used to compensate for the main rotor profile drag. If H_COL2YAW is not used, this value can be set to reduce the yaw I contribution to zero when in a steady hover.</source>
      <translation>应用于偏航输出的固定偏移量可最大限度地减少抵消转子阻力所需的偏航 I 项贡献。目前仅适用于 DDFP 尾部（H_TAIL_TYPE = 3 或 H_TAIL_TYPE = 4）。如果使用 H_COL2YAW 补偿，则该微调用于补偿主旋翼轮廓阻力。如果不使用 H_COL2YAW，可以设置该值以将稳定悬停时的偏航 I 贡献减小到零。</translation>
    </message>
    <message>
      <source>Fixed-wing land detector: Max airspeed</source>
      <translation>固定翼陆地探测器：最大空速</translation>
    </message>
    <message>
      <source>FixedWingPitchD</source>
      <translation>固定翼桨距D</translation>
    </message>
    <message>
      <source>Flag to allow arming</source>
      <translation>允许解锁的标志</translation>
    </message>
    <message>
      <source>Flap slew rate</source>
      <translation>襟翼回转率</translation>
    </message>
    <message>
      <source>Flaps setting during landing</source>
      <translation>着陆时襟翼设置</translation>
    </message>
    <message>
      <source>Flight Mode 3</source>
      <translation>飞行模式3</translation>
    </message>
    <message>
      <source>Flight mode for switch position 3 (1361 to 1490)</source>
      <translation>开关位置3（1361至1490）的飞行模式</translation>
    </message>
    <message>
      <source>Flight mode when Channel 5 pwm is &gt;1360, &lt;= 1490</source>
      <translation>通道 5 pwm &gt;1360，&lt;= 1490 时的飞行模式</translation>
    </message>
    <message>
      <source>Flight mode when pwm of Flightmode channel(FLTMODE_CH) is &gt;1620, &lt;= 1749</source>
      <translation>Flightmode通道(FLTMODE_CH)的pwm &gt;1620, &lt;= 1749时的飞行模式</translation>
    </message>
    <message>
      <source>FlightMode4</source>
      <translation>飞行模式4</translation>
    </message>
    <message>
      <source>Flow sensor yaw alignment</source>
      <translation>流量传感器偏航对准</translation>
    </message>
    <message>
      <source>FlowHold Flow Rate Max</source>
      <translation>FlowHold 最大流量</translation>
    </message>
    <message>
      <source>Fly HOME then land</source>
      <translation>飞行 HOME 然后着陆</translation>
    </message>
    <message>
      <source>Follow Offsets defaulted</source>
      <translation>默认跟随偏移量</translation>
    </message>
    <message>
      <source>Follow enable/disable</source>
      <translation>跟随启用/禁用</translation>
    </message>
    <message>
      <source>Follow offsets in meters north/forward</source>
      <translation>跟随向北/向前偏移（以米为单位）</translation>
    </message>
    <message>
      <source>Follow position error P gain. Converts the difference between desired vertical speed and actual speed into a desired acceleration that is passed to the throttle acceleration controller</source>
      <translation>跟随位置误差P增益。将所需垂直速度与实际速度之间的差值转换为传递至油门加速度控制器的所需加速度</translation>
    </message>
    <message>
      <source>For intermeshing mode only. Yaw revereser smoothing exponent, smoothen transition near zero collective region. Increase this parameter to shink smoothing range. Set to -1 to disable reverser.</source>
      <translation>仅适用于啮合模式。偏航反向器平滑指数，平滑接近零总距区域的过渡。增加此参数可缩小平滑范围。设置为 -1 以禁用反向器。</translation>
    </message>
    <message>
      <source>For systems a INA220 Power Monitor, this should be set to true</source>
      <translation>对于 INA220 功率监视器系统，应设置为 true</translation>
    </message>
    <message>
      <source>Force FPV (bf) lock on roll and pitch</source>
      <translation>强制 FPV (bf) 锁定横滚和俯仰</translation>
    </message>
    <message>
      <source>Force Terminate</source>
      <translation>强制终止</translation>
    </message>
    <message>
      <source>Force safety when the vehicle disarms</source>
      <translation>车辆上锁时强制安全</translation>
    </message>
    <message>
      <source>Forces the servo output signal to take at least the configured time (in seconds)
to traverse its full range [-100%, 100%].
Zero means that slew rate limiting is disabled.</source>
      <translation>强制舵机输出信号至少花费配置的时间（以秒为单位）来遍历其整个范围 [-100%、100%]。零表示转换速率限制被禁用。</translation>
    </message>
    <message>
      <source>Forward Speed Controller Feed Forward Gain</source>
      <translation>正向速度控制器前馈增益</translation>
    </message>
    <message>
      <source>Forward channel number. This is useful when you have a RC transmitter that can't change the channel order easily. Forward is normally on channel 5, but you can move it to any channel with this parameter. Reboot is required for changes to take effect.</source>
      <translation>前向通道数。当您的 RC 发射机无法轻松更改通道顺序时，这非常有用。转发通常在通道 5 上，但您可以使用此参数将其移动到任何通道。需要重新启动才能使更改生效。</translation>
    </message>
    <message>
      <source>Forward throttle battery voltage compensation minimum voltage (voltage below this will have no additional scaling effect on thrust).  Recommend 3.5 * cell count, 0 = Disabled</source>
      <translation>前进油门电池电压补偿最小电压（低于此电压不会对推力产生额外的缩放效果）。  推荐 3.5 * 细胞计数，0 = 禁用</translation>
    </message>
    <message>
      <source>Forward/vertical to pitch decoupling factor</source>
      <translation>前向/垂直至俯仰解耦系数</translation>
    </message>
    <message>
      <source>Fourth Gyro bias on Z axis</source>
      <translation>Z 轴上的第四个陀螺仪偏置</translation>
    </message>
    <message>
      <source>FrSky SPort Passthrough (OpenTX)</source>
      <translation>FrSky SPort 直通 (OpenTX)</translation>
    </message>
    <message>
      <source>Freeze barometer to last recorded altitude</source>
      <translation>将气压计冻结至最后记录的高度</translation>
    </message>
    <message>
      <source>Fresh Water</source>
      <translation>淡水</translation>
    </message>
    <message>
      <source>Frsky SPort</source>
      <translation>Frsky运动</translation>
    </message>
    <message>
      <source>Full Wind Altitude</source>
      <translation>全风高度</translation>
    </message>
    <message>
      <source>Fullrate Notch</source>
      <translation>全速率陷波</translation>
    </message>
    <message>
      <source>Fusion reset mode</source>
      <translation>融合重置模式</translation>
    </message>
    <message>
      <source>GCS failsafe timeout</source>
      <translation>GCS 故障保护超时</translation>
    </message>
    <message>
      <source>GNSS</source>
      <translation>全球导航卫星系统</translation>
    </message>
    <message>
      <source>GNSS sensor aiding</source>
      <translation>GNSS 传感器辅助</translation>
    </message>
    <message>
      <source>GPRMC</source>
      <translation>GPRMC</translation>
    </message>
    <message>
      <source>GPS 1 Altitude Offset</source>
      <translation>GPS 1 高度偏移</translation>
    </message>
    <message>
      <source>GPS 1 Position</source>
      <translation>GPS 1 位置</translation>
    </message>
    <message>
      <source>GPS 1 antenna phase center position relative to the body frame origin (Y-axis)</source>
      <translation>GPS 1 天线相位中心相对于机身框架原点（Y轴）的位置</translation>
    </message>
    <message>
      <source>GPS 2 Altitude Drift</source>
      <translation>GPS 2 海拔漂移</translation>
    </message>
    <message>
      <source>GPS 2 Lock Time</source>
      <translation>GPS 2 锁定时间</translation>
    </message>
    <message>
      <source>GPS 2 Velocity Error Offsets in NED (Z-axis)</source>
      <translation>GPS 2 NED 中的速度误差偏移（Z 轴）</translation>
    </message>
    <message>
      <source>GPS 2 type</source>
      <translation>GPS 2型</translation>
    </message>
    <message>
      <source>GPS Altitude Error</source>
      <translation>GPS 高度错误</translation>
    </message>
    <message>
      <source>GPS Glitch</source>
      <translation>GPS 故障</translation>
    </message>
    <message>
      <source>GPS Log Number</source>
      <translation>GPS 日志编号</translation>
    </message>
    <message>
      <source>GPS Node id for first GPS. If 0 the gps will be automatically selected on a first-come-first-GPS basis.</source>
      <translation>GPS 第一个 GPS 的节点 ID。如果是 0，将按照先到先得的 GPS 原则自动选择 GPS。</translation>
    </message>
    <message>
      <source>GPS Noise</source>
      <translation>GPS 噪声</translation>
    </message>
    <message>
      <source>GPS Velocity Error Offsets in NED (X-axis)</source>
      <translation>GPS NED 中的速度误差偏移（X 轴）</translation>
    </message>
    <message>
      <source>GPS antenna longitudinal offset</source>
      <translation>GPS天线纵向偏移</translation>
    </message>
    <message>
      <source>GPS delay in milliseconds</source>
      <translation>GPS 延迟（以毫秒为单位）</translation>
    </message>
    <message>
      <source>GPS heading offset in degrees. how off the simulated GPS heading is from the actual heading</source>
      <translation>GPS 航向偏移（以度为单位）。模拟的 GPS 航向与实际航向的偏差如何</translation>
    </message>
    <message>
      <source>GPS jamming enable</source>
      <translation>GPS 干扰使能</translation>
    </message>
    <message>
      <source>GPS physical COM port</source>
      <translation>GPS 物理 COM 端口</translation>
    </message>
    <message>
      <source>GPS type of 1st GPS.Renamed in 4.6 and later to GPS1_TYPE</source>
      <translation>GPS 第一个 GPS 的类型。在 4.6 中重命名，后来更名为 GPS1_TYPE</translation>
    </message>
    <message>
      <source>GPS vertical accuracy threshold</source>
      <translation>GPS垂直精度阈值</translation>
    </message>
    <message>
      <source>GPS+IMES+QZSS+SBAS (Japan Only)</source>
      <translation>GPS+IMES+QZSS+SBAS（仅限日本）</translation>
    </message>
    <message>
      <source>GPSLONG_Y</source>
      <translation>GPSLONG_Y</translation>
    </message>
    <message>
      <source>GSPEED_EN</source>
      <translation>GSPEED_EN</translation>
    </message>
    <message>
      <source>Gain for position control velocity setpoint update</source>
      <translation>位置控制速度设定值更新增益</translation>
    </message>
    <message>
      <source>Gain of D controller Y</source>
      <translation>D控制器Y的增益</translation>
    </message>
    <message>
      <source>Galileo+SBAS</source>
      <translation>伽利略+SBAS</translation>
    </message>
    <message>
      <source>Gate size for auxiliary global position sensor 0 fusion</source>
      <translation>辅助全球位置传感器 0 fusion 的门尺寸</translation>
    </message>
    <message>
      <source>Gate size for range finder fusion</source>
      <translation>测距仪融合的门尺寸</translation>
    </message>
    <message>
      <source>General purpose user variable input for scripts</source>
      <translation>脚本的通用用户变量输入</translation>
    </message>
    <message>
      <source>Generator-Fuel</source>
      <translation>发电机-燃料</translation>
    </message>
    <message>
      <source>Get timestamp of mid exposure (active low)</source>
      <translation>获取中间曝光的时间戳（低电平有效）</translation>
    </message>
    <message>
      <source>Glide</source>
      <translation>滑翔</translation>
    </message>
    <message>
      <source>Glitch offsets of simulated GPS 1 sensor (X-axis)</source>
      <translation>模拟 GPS 1 传感器的毛刺偏移（X 轴）</translation>
    </message>
    <message>
      <source>Glitch offsets of simulated GPS sensor (Z-axis)</source>
      <translation>模拟 GPS 传感器的毛刺偏移（Z 轴）</translation>
    </message>
    <message>
      <source>Global gain of the controller.
This gain scales the P, I and D terms of the controller:
output = SC_ROLLRATE_K * (SC_ROLLRATE_P * error
+ SC_ROLLRATE_I * error_integral
+ SC_ROLLRATE_D * error_derivative)
Set SC_ROLLRATE_P=1 to implement a PID in the ideal form.
Set SC_ROLLRATE_K=1 to implement a PID in the parallel form.</source>
      <translation>控制器的全局增益。该增益可缩放控制器的 P、I 和 D 项：输出 = SC_ROLLRATE_K * (SC_ROLLRATE_P * 误差 + SC_ROLLRATE_I * error_integral + SC_ROLLRATE_D * error_derivative) 设置 SC_ROLLRATE_P=1 以实现理想形式的 PID。设置SC_ROLLRATE_K=1，以并行形式实现PID。</translation>
    </message>
    <message>
      <source>GloryHot</source>
      <translation>荣耀热</translation>
    </message>
    <message>
      <source>Goertek SPL06 Barometer (external I2C)</source>
      <translation>歌尔SPL06气压计（外置I2C）</translation>
    </message>
    <message>
      <source>Governor Torque Compensator</source>
      <translation>调速器扭矩补偿器</translation>
    </message>
    <message>
      <source>Gripper Grab PWM</source>
      <translation>抓具 PWM</translation>
    </message>
    <message>
      <source>Gripper servo Sim enable/disable</source>
      <translation>夹具舵机 Sim 启用/禁用</translation>
    </message>
    <message>
      <source>Ground Station Failsafe Enable</source>
      <translation>地面站故障保护启用</translation>
    </message>
    <message>
      <source>Ground station MAVLink system ID</source>
      <translation>地面站MAVLink系统ID</translation>
    </message>
    <message>
      <source>Guided mode options</source>
      <translation>引导模式选项</translation>
    </message>
    <message>
      <source>Guided_NoGPS</source>
      <translation>引导_无GPS</translation>
    </message>
    <message>
      <source>Gyro 4 scaling factor</source>
      <translation>陀螺仪 4 比例因子</translation>
    </message>
    <message>
      <source>Gyro bias learning limit</source>
      <translation>陀螺仪偏置学习限制</translation>
    </message>
    <message>
      <source>Gyro drift rate of change in degrees/second/minute</source>
      <translation>陀螺仪漂移变化率（度/秒/分钟）</translation>
    </message>
    <message>
      <source>Gyro rate for IMUs with Fast Sampling enabled</source>
      <translation>启用快速采样的 IMU 陀螺仪速率</translation>
    </message>
    <message>
      <source>Gyro rate offset temperature ^2 polynomial coefficient - X axis</source>
      <translation>陀螺仪速率偏移温度^2多项式系数-X轴</translation>
    </message>
    <message>
      <source>Gyro sensor offsets of Y axis. This is setup on each boot during gyro calibrations</source>
      <translation>Y 轴陀螺仪传感器偏移。这是在陀螺仪校准期间每次启动时设置的</translation>
    </message>
    <message>
      <source>Gyro2 sensor offsets of Y axis. This is setup on each boot during gyro calibrations</source>
      <translation>Gyro2 传感器 Y 轴偏移。这是在陀螺仪校准期间每次启动时设置的</translation>
    </message>
    <message>
      <source>Gyro3 sensor offsets of Y axis. This is setup on each boot during gyro calibrations</source>
      <translation>Gyro3 传感器 Y 轴偏移。这是在陀螺仪校准期间每次启动时设置的</translation>
    </message>
    <message>
      <source>Gyroscope 1 X-axis offset</source>
      <translation>陀螺仪1 X轴偏移</translation>
    </message>
    <message>
      <source>Gyroscope 1st order temperature coefficient Z axis</source>
      <translation>陀螺仪一阶温度系数Z轴</translation>
    </message>
    <message>
      <source>Gyroscope 2nd order temperature coefficient Y axis</source>
      <translation>陀螺仪二阶温度系数Y轴</translation>
    </message>
    <message>
      <source>Gyroscope 3rd order temperature coefficient X axis</source>
      <translation>陀螺仪三阶温度系数X轴</translation>
    </message>
    <message>
      <source>H3 is generic, three-servo only. H3_120/H3_140 plates have Motor1 left side, Motor2 right side, Motor3 elevator in rear. HR3_120/HR3_140 have Motor1 right side, Motor2 left side, Motor3 elevator in front - use H3_120/H3_140 and reverse servo and collective directions as necessary. For all H3_90 swashplates use H4_90 and don't use servo output for the missing servo. For H4-90 Motors1&amp;2 are left/right respectively, Motors3&amp;4 are rear/front respectively. For H4-45 Motors1&amp;2 are LF/RF, Motors3&amp;4 are LR/RR</source>
      <translation>H3 是通用的，仅限三舵机。 H3_120/H3_140 板左侧有 Motor1，右侧有 Motor2，后部有 Motor3 升降舵。 HR3_120/HR3_140 右侧有 Motor1，左侧有 Motor2，前面有 Motor3 电梯 - 使用 H3_120/H3_140 并根据需要反转舵机和总距方向。对于所有 H3_90 斜盘均使用 H4_90，并且对于缺失的舵机不使用舵机输出。对于H4-90电机，1和2分别为左/右，电机3和4分别为后/前。对于H4-45电机1&amp;2为LF/RF，电机3&amp;4为LR/RR</translation>
    </message>
    <message>
      <source>HDOP_X</source>
      <translation>HDOP_X</translation>
    </message>
    <message>
      <source>HFI EFI ECU index</source>
      <translation>HFI EFI ECU 索引</translation>
    </message>
    <message>
      <source>HIL Servos enable</source>
      <translation>HIL 舵机使能</translation>
    </message>
    <message>
      <source>HOMEDIR_Y</source>
      <translation>HOMEDIR_Y</translation>
    </message>
    <message>
      <source>HOME_Y</source>
      <translation>HOME_Y</translation>
    </message>
    <message>
      <source>Halo6000 CAN driver</source>
      <translation>Halo6000 CAN 驱动程序</translation>
    </message>
    <message>
      <source>Hardware safety switch</source>
      <translation>硬件安全开关</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter bandwidth in Hz. This is typically set to half the base frequency. The ratio of base frequency to bandwidth determines the notch quality factor and is fixed across harmonics.</source>
      <translation>谐波陷波滤波器带宽（以 Hz 为单位）。这通常设置为基频的一半。基频与带宽之比决定了陷波品质因数，并且在谐波上是固定的。</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter dynamic frequency tracking mode. Dynamic updates can be throttle, RPM sensor or ESC telemetry based. Throttle-based updates should only be used with multicopters.</source>
      <translation>谐波陷波滤波器动态频率跟踪模式。动态更新可以基于油门、RPM 传感器或 ESC 遥测。基于油门的更新只能用于多旋翼飞行器。</translation>
    </message>
    <message>
      <source>Harmonic Notch Filter options. Triple and double-notches can provide deeper attenuation across a wider bandwidth with reduced latency than single notches and are suitable for larger aircraft. Dynamic harmonics attaches a harmonic notch to each detected noise frequency instead of simply being multiples of the base frequency, in the case of FFT it will attach notches to each of three detected noise peaks, in the case of ESC it will attach notches to each of four motor RPM values. Loop rate update changes the notch center frequency at the scheduler loop rate rather than at the default of 200Hz. If both double and triple notches are specified only double notches will take effect.</source>
      <translation>谐波陷波滤波器选项。三陷波和双陷波可以在更宽的带宽上提供更深的衰减，并且比单陷波减少延迟，并且适合较大的飞机。动态谐波将谐波陷波附加到每个检测到的噪声频率，而不是简单地为基频的倍数，在 FFT 的情况下，它将为三个检测到的噪声峰值中的每一个附加陷波，在 ESC 的情况下，它将为四个电机 RPM 值中的每一个附加陷波。循环速率更新以调度程序循环速率而不是默认的 200Hz 更改陷波中心频率。如果同时指定了双槽口和三槽口，则仅双槽口生效。</translation>
    </message>
    <message>
      <source>Heading offset angle for dual antenna GPS setups that support heading estimation. Set this to 0 if the antennas are parallel to the forward-facing direction of the vehicle and the rover (or Unicore primary) antenna is in front. The offset angle increases clockwise. Set this to 90 if the rover (or Unicore primary, or Septentrio Mosaic Aux) antenna is placed on the right side of the vehicle and the moving base antenna is on the left side. (Note: the Unicore primary antenna is the one connected on the right as seen from the top).</source>
      <translation>支持航向估计的双天线 GPS 设置的航向偏移角。如果天线与车辆的前向平行并且流动站（或 Unicore 主）天线位于前面，则将此设置为 0。偏移角顺时针增大。如果流动站（或 Unicore 主天线或 Septentrio Mosaic Aux）天线放置在车辆的右侧并且移动基座天线位于左侧​​，则将此设置为 90。 （注：Unicore 主天线是从顶部看连接在右侧的天线）。</translation>
    </message>
    <message>
      <source>HeartbeatAndAUTO</source>
      <translation>仅自动模式下心跳超时触发</translation>
    </message>
    <message>
      <source>Height above ground where anomally strength has decayed to 1/8 of the ground level value</source>
      <translation>异常强度衰减至地面值 1/8 时距地面的高度</translation>
    </message>
    <message>
      <source>Height override of sensor above ground</source>
      <translation>传感器距地面高度超控</translation>
    </message>
    <message>
      <source>Heli</source>
      <translation>直升机</translation>
    </message>
    <message>
      <source>Helicopter (Coaxial)</source>
      <translation>直升机（同轴）</translation>
    </message>
    <message>
      <source>Herelink USB-tethering</source>
      <translation>这里链接 USB-tethering</translation>
    </message>
    <message>
      <source>High Latency Datalink loss time threshold</source>
      <translation>高延迟数据链路丢失时间阈值</translation>
    </message>
    <message>
      <source>High wind failsafe mode</source>
      <translation>大风故障安全模式</translation>
    </message>
    <message>
      <source>HighlyManuv</source>
      <translation>高度机动</translation>
    </message>
    <message>
      <source>Hold Front Tangent to Circle</source>
      <translation>保持前部与圆相切</translation>
    </message>
    <message>
      <source>Home reset altitude threshold</source>
      <translation>回家重置高度阈值</translation>
    </message>
    <message>
      <source>Horizontal position of Save button on screen</source>
      <translation>屏幕上“保存”按钮的水平位置</translation>
    </message>
    <message>
      <source>Horizontal velocity lower limit</source>
      <translation>水平速度下限</translation>
    </message>
    <message>
      <source>How long the shutter will be held open in 10ths of a second (i.e. enter 10 for 1second, 50 for 5seconds)</source>
      <translation>快门在 10 秒内保持打开状态的时间（即输入 10 1 秒，50 5 秒）</translation>
    </message>
    <message>
      <source>I gain for the speed PID  distance component</source>
      <translation>I 速度增益 PID 距离分量</translation>
    </message>
    <message>
      <source>I2C-DLVR-20in</source>
      <translation>I2C-DLVR-20in</translation>
    </message>
    <message>
      <source>I2C-SDP3X</source>
      <translation>I2C-SDP3X</translation>
    </message>
    <message>
      <source>ICAO_ID unique vehicle identification number of this aircraft. This is a integer limited to 24bits. If set to 0 then one will be randomly generated. If set to -1 then static information is not sent, transceiver is assumed pre-programmed.</source>
      <translation>ICAO_ID 该飞机的唯一车辆识别号。这是一个限制为 24 位的整数。如果设置为 0，则将随机生成一个。如果设置为 -1，则不发送静态信息，假设收发器已预先编程。</translation>
    </message>
    <message>
      <source>ID of Barometer that the calibration is for</source>
      <translation>校准用气压计ID</translation>
    </message>
    <message>
      <source>IIS2MDC or LIS2MDL</source>
      <translation>IIS2MDC 或 LIS2MDL</translation>
    </message>
    <message>
      <source>IMU Heater P gain</source>
      <translation>IMU 加热器 P 增益</translation>
    </message>
    <message>
      <source>IMU auto calibration</source>
      <translation>IMU自动校准</translation>
    </message>
    <message>
      <source>IMU gyro FFT enable</source>
      <translation>IMU 陀螺仪 FFT 使能</translation>
    </message>
    <message>
      <source>IMU heater controller feedforward value</source>
      <translation>IMU加热器控制器前馈值</translation>
    </message>
    <message>
      <source>IMU temperature time constant</source>
      <translation>IMU温度时间常数</translation>
    </message>
    <message>
      <source>IMax</source>
      <translation>最大电流</translation>
    </message>
    <message>
      <source>INA226 Power Monitor Shunt</source>
      <translation>INA226 功率监控分流器</translation>
    </message>
    <message>
      <source>INA2XX (INA226 INA228 INA238 INA231 INA260)</source>
      <translation>INA2XX (INA226 INA228 INA238 INA231 INA260)</translation>
    </message>
    <message>
      <source>INS or sensors</source>
      <translation>INS或传感器</translation>
    </message>
    <message>
      <source>IPv4 Address 2nd byte</source>
      <translation>IPv4 地址第二个字节</translation>
    </message>
    <message>
      <source>IPv4 address. Example: xxx.xxx.xxx.14</source>
      <translation>IPv4 地址。示例：xxx.xxx.xxx.14</translation>
    </message>
    <message>
      <source>IS31FL3195 External</source>
      <translation>IS31FL3195 外部</translation>
    </message>
    <message>
      <source>Ice shedding prevents ice buildup in VTOL aircraft motors by periodically spinning inactive rotors. When enabled (period &gt; 0), every cycle lasts for the defined period and includes a 2-second spin at 0.01 motor output. If period &lt;= 0, the feature is disabled.</source>
      <translation>冰脱落通过定期旋转不活动的转子来防止 VTOL 飞机发动机中积冰。启用后（周期 &gt; 0），每个周期都会持续定义的周期，并在 0.01 电机输出处包含 2 秒旋转。如果 period &lt;= 0，则该功能被禁用。</translation>
    </message>
    <message>
      <source>Idle time</source>
      <translation>空闲时间</translation>
    </message>
    <message>
      <source>If LOG_DISARMED is set to 1 then logging will be enabled while disarmed. This can make for very large logfiles but can help a lot when tracking down startup issues</source>
      <translation>如果 LOG_DISARMED 设置为 1，则上锁时将启用日志记录。这可能会产生非常大的日志文件，但在跟踪启动问题时有很大帮助</translation>
    </message>
    <message>
      <source>If a valid distance sensor measurement to the ground is available,
limit descending velocity to "MPC_LAND_CRWL" below this altitude.</source>
      <translation>如果可以使用到地面的有效距离传感器测量值，则将下降速度限制为低于该高度的“MPC_LAND_CRWL”。</translation>
    </message>
    <message>
      <source>If enabled, a small additional "mission" log file will be written to the SD card. The log contains just those messages that are useful for tasks like generating flight statistics and geotagging. The different modes can be used to further reduce the logged data (and thus the log file size). For example, choose geotagging mode to only log data required for geotagging. Note that the normal/full log is still created, and contains all the data in the mission log (and more).</source>
      <translation>如果启用，一个小的附加“任务”日志文件将被写入 SD 卡。该日志仅包含对生成飞行统计数据和地理标记等任务有用的消息。不同的模式可用于进一步减少记录的数据（从而减少日志文件的大小）。例如，选择地理标记模式以仅记录地理标记所需的数据。请注意，仍会创建正常/完整日志，并包含任务日志中的所有数据（以及更多数据）。</translation>
    </message>
    <message>
      <source>If enabled, future sensor calibrations will be stored to /fs/mtd_caldata. Note: this is only supported on boards with a separate calibration storage /fs/mtd_caldata.</source>
      <translation>如果启用，将来的传感器校准将存储到 /fs/mtd_caldata。注意：这仅在具有单独校准存储 /fs/mtd_caldata 的主板上受支持。</translation>
    </message>
    <message>
      <source>If ethernet enabled and selected as configuration for MAVLink instance 0, selected remote port will be set and used in MAVLink instance 0.</source>
      <translation>如果启用以太网并选择作为 MAVLink 实例 0 的配置，则选定的远程端口将在 MAVLink 实例 0 中设置和使用。</translation>
    </message>
    <message>
      <source>If ethernet enabled and selected as configuration for MAVLink instance 2, selected remote port will be set and used in MAVLink instance 2.</source>
      <translation>如果启用以太网并选择作为 MAVLink 实例 2 的配置，则选定的远程端口将在 MAVLink 实例 2 中设置和使用。</translation>
    </message>
    <message>
      <source>If more than one compass is available, this selects which compass is the primary. When external compasses are connected, they will be ordered first. NOTE: If no external compass is attached, this parameter is ignored.</source>
      <translation>如果有多个罗盘可用，则选择哪个罗盘作为主要罗盘。当连接外部罗盘时，将首先订购它们。 NOTE：如果没有连接外部罗盘，则忽略该参数。</translation>
    </message>
    <message>
      <source>If non-zero this airspeed will be used when thermalling.</source>
      <translation>如果不为零，则在热飞行时将使用该空速。</translation>
    </message>
    <message>
      <source>If set to 1 incoming HIL GPS messages are parsed.</source>
      <translation>如果设置为 1，则解析传入的 HIL GPS 消息。</translation>
    </message>
    <message>
      <source>If set to a negative value or larger than
MPC_VEL_MANUAL then MPC_VEL_MANUAL is used.</source>
      <translation>如果设置为负值或大于 MPC_VEL_MANUAL，则使用 MPC_VEL_MANUAL。</translation>
    </message>
    <message>
      <source>If set to a value greater than zero, other parameters are automatically set (such as MPC_VEL_MAX or MPC_VEL_MANUAL). If set to a negative value, the existing individual parameters are used.</source>
      <translation>如果设置为大于零的值，则自动设置其他参数（例如 MPC_VEL_MAX 或 MPC_VEL_MANUAL）。如果设置为负值，则使用现有的各个参数。</translation>
    </message>
    <message>
      <source>If set, enables the actuator test interface via MAVLink (ACTUATOR_TEST), that allows spinning the motors and moving the servos for testing purposes.</source>
      <translation>如果设置，则通过 MAVLink (ACTUATOR_TEST) 启用执行器测试接口，允许旋转电机并移动舵机系统以进行测试。</translation>
    </message>
    <message>
      <source>If the difference between airspeed and GPS speed is greater than this value the sensor will issue a warning. If 0 ARSPD_WIND_MAX is used.</source>
      <translation>如果空速和 GPS 速度之间的差异大于该值，传感器将发出警告。如果使用 0 ARSPD_WIND_MAX。</translation>
    </message>
    <message>
      <source>If the logfile is encrypted using a symmetric key algorithm, the used encryption key is generated at logging start and stored on the sdcard RSA2048 encrypted using this key.</source>
      <translation>如果日志文件使用对称密钥算法加密，则在日志记录开始时生成使用的加密密钥，并存储在使用该密钥加密的 sdcard RSA2048 上。</translation>
    </message>
    <message>
      <source>If the target is greater than this many degrees left or right, assume it's turning</source>
      <translation>如果目标向左或向右大于这么多度，则假设它正在转动</translation>
    </message>
    <message>
      <source>If this is set then when in STABILIZE, FBWA or ACRO modes the throttle is a direct passthru from the transmitter. This means the THR_MIN and THR_MAX settings are not used in these modes. This is useful for petrol engines where you setup a throttle cut switch that suppresses the throttle below the normal minimum.</source>
      <translation>如果设置了此项，则在 STABILIZE、FBWA 或 ACRO 模式下，油门将直接来自发射器。这意味着在这些模式下不使用 THR_MIN 和 THR_MAX 设置。这对于汽油发动机很有用，您可以设置油门切断开关，将油门抑制到正常最小值以下。</translation>
    </message>
    <message>
      <source>If this parameter is non-zero, flight modes are only selected by this channel and are assigned to six slots.</source>
      <translation>如果该参数非零，则飞行模式仅由该通道选择并分配给六个插槽。</translation>
    </message>
    <message>
      <source>If while tuning the angle error goes over this limit then the tune will aborts to prevent a bad oscillation in the case of the tuning algorithm failing. If you get an error "Quicktune: attitude error ABORTING" and you think it is a false positive then you can either raise this parameter or you can try increasing the QWIK_DOUBLE_TIME to do the tune more slowly.</source>
      <translation>如果在调谐时角度误差超过此限制，则调谐将中止，以防止在调谐算法失败的情况下出现不良振荡。如果您收到错误“Quicktune：姿态错误 ABORTING”并且您认为这是误报，那么您可以提高此参数，也可以尝试增加 QWIK_DOUBLE_TIME 以更慢地进行调整。</translation>
    </message>
    <message>
      <source>Ignore Receiver Failsafe</source>
      <translation>忽略接收器故障保护</translation>
    </message>
    <message>
      <source>Ignore pilot yaw</source>
      <translation>忽略飞行员偏航</translation>
    </message>
    <message>
      <source>Improves tracking performance.</source>
      <translation>提高跟踪性能。</translation>
    </message>
    <message>
      <source>InFlight-Learning</source>
      <translation>飞行学习</translation>
    </message>
    <message>
      <source>Increase it to trim out speed and height offsets faster, with the downside of possible overshoots and oscillations.</source>
      <translation>增加它可以更快地消除速度和高度偏移，但缺点是可能出现超调和振荡。</translation>
    </message>
    <message>
      <source>Increasing this value will make climb rate controlled takeoff slower.
If it's too slow the drone might scratch the ground and tip over.
A time constant of 0 disables the ramp</source>
      <translation>增加该值将使爬升率控制起飞变慢。如果速度太慢，无人机可能会刮伤地面并翻倒。 0 的时间常数禁用斜坡</translation>
    </message>
    <message>
      <source>Increment step for actuator 6</source>
      <translation>执行器 6 的增量步长</translation>
    </message>
    <message>
      <source>InertialLabs</source>
      <translation>惯性实验室</translation>
    </message>
    <message>
      <source>Initial Latitude before GPS lock</source>
      <translation>GPS锁定前的初始纬度</translation>
    </message>
    <message>
      <source>Initial increment step for changing the actuator's PWM</source>
      <translation>更改执行器 PWM 的初始增量步长</translation>
    </message>
    <message>
      <source>Input Lateral Velocity</source>
      <translation>输入横向速度</translation>
    </message>
    <message>
      <source>Input channel for engine start minimum PWM</source>
      <translation>发动机启动最小值 PWM 的输入通道</translation>
    </message>
    <message>
      <source>IntegerRTK</source>
      <translation>整数RTK</translation>
    </message>
    <message>
      <source>Integral gain from lateral acceleration error. Effectively trims rudder to eliminate long-term sideslip.</source>
      <translation>横向加速度误差的积分增益。有效修整舵，消除长期侧滑。</translation>
    </message>
    <message>
      <source>Integrator gain to trim out long-term speed and height errors.</source>
      <translation>用于消除长期速度和高度误差的积分增益。</translation>
    </message>
    <message>
      <source>Internal</source>
      <translation>内部</translation>
    </message>
    <message>
      <source>Internal-Learning</source>
      <translation>内部学习</translation>
    </message>
    <message>
      <source>InvertedFlight</source>
      <translation>倒飞</translation>
    </message>
    <message>
      <source>Is used to compensate for the additional drag created by turning.
Increase this gain if the aircraft initially loses energy in turns
and reduce if the aircraft initially gains energy in turns.</source>
      <translation>用于补偿转弯产生的额外阻力。如果无人机进入转弯时能量降低，应增大此增益；如果能量增加，应减小此增益。</translation>
    </message>
    <message>
      <source>Jerk limit for the vertical kinematic input shaping</source>
      <translation>垂直运动输入整形的加加速度限制</translation>
    </message>
    <message>
      <source>KITE</source>
      <translation>KITE</translation>
    </message>
    <message>
      <source>Key of the parameter to be displayed and modified</source>
      <translation>要显示和修改的参数键名</translation>
    </message>
    <message>
      <source>L1 control period</source>
      <translation>L1控制周期</translation>
    </message>
    <message>
      <source>L45W45</source>
      <translation>L45W45</translation>
    </message>
    <message>
      <source>L85W90</source>
      <translation>L85W90</translation>
    </message>
    <message>
      <source>LINK_Q_X</source>
      <translation>LINK_Q_X</translation>
    </message>
    <message>
      <source>LPS25H</source>
      <translation>LPS25H</translation>
    </message>
    <message>
      <source>LTE options</source>
      <translation>LTE 选项</translation>
    </message>
    <message>
      <source>Land alt low</source>
      <translation>土地海拔低</translation>
    </message>
    <message>
      <source>Land final speed</source>
      <translation>着陆最终速度</translation>
    </message>
    <message>
      <source>Landed pitch gain multiplier</source>
      <translation>落地俯仰增益乘数</translation>
    </message>
    <message>
      <source>Landing Gear Wheel</source>
      <translation>起落架轮</translation>
    </message>
    <message>
      <source>Landing direction offset from takeoff</source>
      <translation>着陆方向偏离起飞</translation>
    </message>
    <message>
      <source>Landing gear deployment altitude</source>
      <translation>起落架展开高度</translation>
    </message>
    <message>
      <source>Landing slope re-calc threshold</source>
      <translation>着陆坡度重新计算阈值</translation>
    </message>
    <message>
      <source>LandingGear</source>
      <translation>起落架</translation>
    </message>
    <message>
      <source>LatRight2M</source>
      <translation>右纬2M</translation>
    </message>
    <message>
      <source>Leaky I(4.0 and earlier)</source>
      <translation>泄漏 I（4.0 及更早版本）</translation>
    </message>
    <message>
      <source>LearnCruise Speed</source>
      <translation>学习巡航速度</translation>
    </message>
    <message>
      <source>Left A-tail</source>
      <translation>左A型尾</translation>
    </message>
    <message>
      <source>Left4m</source>
      <translation>左4m</translation>
    </message>
    <message>
      <source>Len35_Wid38</source>
      <translation>Len35_Wid38</translation>
    </message>
    <message>
      <source>Len75_Wid80</source>
      <translation>Len75_Wid80</translation>
    </message>
    <message>
      <source>Level Transition:Keep wings within LEVEL_ROLL_LIMIT and only use forward motor(s) for climb during transition, Allow FW Takeoff: If bit is not set then NAV_TAKEOFF command on quadplanes will instead perform a NAV_VTOL takeoff, Allow FW Land:If bit is not set then NAV_LAND command on quadplanes will instead perform a NAV_VTOL_LAND, Vtol Takeoff Frame: command NAV_VTOL_TAKEOFF altitude is as set by the command's reference frame rather than a delta above current location, Use FW Approach:Use a fixed wing approach for VTOL landings, USE QRTL:instead of QLAND for rc failsafe when in VTOL modes, Use Governor:Use ICE Idle Governor in MANUAL for forward motor, Force Qassist: on always,Mtrs_Only_Qassist: in tailsitters only, uses VTOL motors and not flying surfaces for QASSIST, Airmode_On_Arm:Airmode enabled when arming by aux switch, Disarmed Yaw Tilt:Enable motor tilt for yaw when disarmed, Delay Spoolup:Delay VTOL spoolup for 2 seconds after arming, ThrLandControl: enable throttle stick control of landing rate, DisableApproach: Disable use of approach and airbrake stages in VTOL landing, EnableLandResponsition: enable pilot controlled repositioning in AUTO land. Descent will pause while repositioning.</source>
      <translation>水平转换：将机翼横滚角保持在 LEVEL_ROLL_LIMIT 范围内，转换期间仅使用前向推进电机爬升。
允许固定翼起飞：未启用时，复合翼执行 NAV_TAKEOFF 会改为 NAV_VTOL 起飞。
允许固定翼降落：未启用时，复合翼执行 NAV_LAND 会改为 NAV_VTOL_LAND。
垂直起飞高度坐标系：NAV_VTOL_TAKEOFF 的高度使用指令指定的参考坐标系，不再使用相对当前位置的高度差。
使用固定翼进近：垂直降落前执行固定翼进近。
使用 QRTL：垂直起降模式下遥控失联时，用 QRTL 代替 QLAND。
使用调速器：MANUAL 模式下，前向推进电机使用内燃机（ICE）怠速调速器。
强制辅助：始终启用 QASSIST。
仅电机辅助（Mtrs_Only_Qassist）：尾座式机型只使用垂直起降电机进行 QASSIST，不使用气动舵面。
解锁时启用空中模式（Airmode_On_Arm）：使用辅助开关解锁时启用空中模式。
上锁时允许偏航倾转：上锁状态下允许电机倾转控制偏航。
延迟电机升速：解锁后延迟 2 秒再使垂直起降电机升速。
油门控制降落速率（ThrLandControl）：允许油门摇杆控制降落速率。
禁用进近（DisableApproach）：垂直降落时禁用进近和气动制动阶段。
允许降落位置调整（EnableLandResponsition）：AUTO 降落时允许驾驶员调整位置，调整期间暂停下降。</translation>
    </message>
    <message>
      <source>Leveling and Limited</source>
      <translation>练级与限制</translation>
    </message>
    <message>
      <source>Lever arm offset (m) in the Y direction for antenna 2. This will only be used if the device supports a dual antenna setup.</source>
      <translation>天线 2 的 Y 方向杠杆臂偏移 (m)。仅当设备支持双天线设置时才会使用此功能。</translation>
    </message>
    <message>
      <source>Lidar-Lite (LL40LS)</source>
      <translation>激光雷达精简版 (LL40LS)</translation>
    </message>
    <message>
      <source>LightWare GRF Distance Return Type</source>
      <translation>LightWare GRF 距离返回型</translation>
    </message>
    <message>
      <source>LightWareSerial</source>
      <translation>LightWare系列</translation>
    </message>
    <message>
      <source>LightwareSF40c</source>
      <translation>灯件SF40c</translation>
    </message>
    <message>
      <source>Limit for roll rate in manual and auto modes (except acro). Has effect for large rotations in autonomous mode, to avoid large control output and mixer saturation. This is not only limited by the vehicle's properties, but also by the maximum measurement rate of the gyro.</source>
      <translation>手动和自动模式下的横滚速率限制（除了特技模式）。对自主模式下的大旋转有效，以避免大的控制输出和混合器饱和。这不仅受到车辆性能的限制，还受到陀螺仪最大测量速率的限制。</translation>
    </message>
    <message>
      <source>Limit the maximum jerk of the vehicle (how fast the acceleration can change).
A lower value leads to smoother vehicle motions but also limited agility.</source>
      <translation>限制车辆的最大加加速度（加速度变化的速度）。较低的值会使车辆运动更平稳，但敏捷性也会受到限制。</translation>
    </message>
    <message>
      <source>Linear Wind Curve Coeff</source>
      <translation>线性风曲线系数</translation>
    </message>
    <message>
      <source>Load factor check (triggers if measurement is below stall speed)</source>
      <translation>负载系数检查（如果测量低于失速速度则触发）</translation>
    </message>
    <message>
      <source>Localhost-only</source>
      <translation>仅本地主机</translation>
    </message>
    <message>
      <source>Log GPS communication data</source>
      <translation>记录GPS通信数据</translation>
    </message>
    <message>
      <source>Log all instances</source>
      <translation>记录所有实例</translation>
    </message>
    <message>
      <source>Log raw communication between the driver and connected receivers. For example, "To receiver" will log all commands and corrections sent by the driver to the receiver.</source>
      <translation>记录驱动程序和连接的接收器之间的原始通信。例如，“至接收器”将记录驱动程序发送至接收器的所有命令和更正。</translation>
    </message>
    <message>
      <source>Logfile Encryption key exchange key</source>
      <translation>日志文件 加密密钥 交换密钥</translation>
    </message>
    <message>
      <source>Logging Mode</source>
      <translation>记录模式</translation>
    </message>
    <message>
      <source>Loglevel</source>
      <translation>日志级别</translation>
    </message>
    <message>
      <source>Loiter behaviour when moving to the target point</source>
      <translation>移动到目标点时的盘旋行为</translation>
    </message>
    <message>
      <source>Loiter in Auto Mode</source>
      <translation>自动模式下盘旋</translation>
    </message>
    <message>
      <source>Loiter maximum lean angle. Set to zero for 2/3 of PSC_ANGLE_MAX or ANGLE_MAX</source>
      <translation>盘旋最大倾斜角度。 PSC_ANGLE_MAX或ANGLE_MAX的2/3设置为零</translation>
    </message>
    <message>
      <source>Loiter or Hold</source>
      <translation>盘旋或停留</translation>
    </message>
    <message>
      <source>Loiter to QLand</source>
      <translation>盘旋到昆士兰</translation>
    </message>
    <message>
      <source>Loop rate</source>
      <translation>循环率</translation>
    </message>
    <message>
      <source>Low Speed</source>
      <translation>低速</translation>
    </message>
    <message>
      <source>Low position accuracy failsafe threshold</source>
      <translation>位置精度故障安全阈值低</translation>
    </message>
    <message>
      <source>Low-pass filter frequency applied to the target input (Hz)</source>
      <translation>应用于目标输入的低通滤波器频率 (Hz)</translation>
    </message>
    <message>
      <source>Lower value -&gt; More aggressive controller (beware overshoot/oscillations)</source>
      <translation>较低的值 -&gt; 更激进的控制器（注意过冲/振荡）</translation>
    </message>
    <message>
      <source>Löweheiser EFI system</source>
      <translation>罗威海塞尔 EFI 系统</translation>
    </message>
    <message>
      <source>MAC address 2nd byte</source>
      <translation>MAC 地址第 2 个字节</translation>
    </message>
    <message>
      <source>MAG1 Scaling factor</source>
      <translation>MAG1 比例因子</translation>
    </message>
    <message>
      <source>MAG3 Failure</source>
      <translation>MAG3故障</translation>
    </message>
    <message>
      <source>MAG8 Device ID</source>
      <translation>MAG8 设备 ID</translation>
    </message>
    <message>
      <source>MAIN 2</source>
      <translation>MAIN 2</translation>
    </message>
    <message>
      <source>MAIN 3 Failsafe Value</source>
      <translation>MAIN 3 故障安全值</translation>
    </message>
    <message>
      <source>MAIN 4 Minimum Value</source>
      <translation>MAIN 4 最小值</translation>
    </message>
    <message>
      <source>MAIN 6</source>
      <translation>MAIN 6</translation>
    </message>
    <message>
      <source>MAIN 7 Failsafe Value</source>
      <translation>MAIN 7 故障安全值</translation>
    </message>
    <message>
      <source>MAIN 8 Minimum Value</source>
      <translation>MAIN 8 最小值</translation>
    </message>
    <message>
      <source>MAVLINK_SYSTEM_TIME</source>
      <translation>MAVLINK_SYSTEM_TIME</translation>
    </message>
    <message>
      <source>MAVLink Mode for instance 0</source>
      <translation>MAVLink 模式（例如 0）</translation>
    </message>
    <message>
      <source>MAVLink Remote Port for instance 0</source>
      <translation>MAVLink 远程端口，例如 0</translation>
    </message>
    <message>
      <source>MAVLink Stream rate of AHRS, SYSTEM_TIME, WIND, RANGEFINDER, DISTANCE_SENSOR, TERRAIN_REQUEST, TERRAIN_REPORT, BATTERY_STATUS, GIMBAL_DEVICE_ATTITUDE_STATUS, OPTICAL_FLOW, MAG_CAL_REPORT, MAG_CAL_PROGRESS, EKF_STATUS_REPORT, VIBRATION, RPM, ESC TELEMETRY,GENERATOR_STATUS, and WINCH_STATUS</source>
      <translation>MAVLink AHRS、SYSTEM_TIME、WIND、RANGEFINDER、DISTANCE_SENSOR、TERRAIN_REQUEST、TERRAIN_REPORT、BATTERY_STATUS、GIMBAL_DEVICE_ATTITUDE_STATUS、OPTICAL_FLOW、MAG_CAL_REPORT、MAG_CAL_PROGRESS、EKF_STATUS_REPORT、VIBRATION、RPM、ESC 的 MAVLink 码流速率TELEMETRY、GENERATOR_STATUS 和 WINCH_STATUS</translation>
    </message>
    <message>
      <source>MAVLink Stream rate of SERVO_OUTPUT_RAW and RC_CHANNELS</source>
      <translation>SERVO_OUTPUT_RAW 和 RC_CHANNELS 的 MAVLink 流速率</translation>
    </message>
    <message>
      <source>MAVLink gimbal protocol v2</source>
      <translation>MAVLink云台协议v2</translation>
    </message>
    <message>
      <source>MAVLink system ID for the tether simulation, used to distinguish it from other systems on the network</source>
      <translation>MAVLink系统ID用于系链模拟，用于区分其与网络上的其他系统</translation>
    </message>
    <message>
      <source>MAVRadio</source>
      <translation>MVR电台</translation>
    </message>
    <message>
      <source>MC Altitude Acceptance Radius</source>
      <translation>MC 高度接受半径</translation>
    </message>
    <message>
      <source>MMC5XX3</source>
      <translation>MMC5XX3</translation>
    </message>
    <message>
      <source>MPEG_TS</source>
      <translation>MPEG_TS</translation>
    </message>
    <message>
      <source>MS5607</source>
      <translation>MS5607</translation>
    </message>
    <message>
      <source>MTK19</source>
      <translation>MTK19</translation>
    </message>
    <message>
      <source>Magnetic declination, in degrees</source>
      <translation>磁偏角，以度为单位</translation>
    </message>
    <message>
      <source>Magnetometer 0 Custom Euler Yaw Angle</source>
      <translation>磁力计 0 定制欧拉偏航角</translation>
    </message>
    <message>
      <source>Magnetometer 0 Y-axis scaling factor</source>
      <translation>磁力计 0 Y 轴比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 1 Custom Euler Pitch Angle</source>
      <translation>磁力计 1 定制欧拉俯仰角</translation>
    </message>
    <message>
      <source>Magnetometer 1 Y-axis off diagonal scale factor</source>
      <translation>磁力计 1 Y 轴偏离对角线比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 1 priority</source>
      <translation>磁力计1优先</translation>
    </message>
    <message>
      <source>Magnetometer 2 X-axis scaling factor</source>
      <translation>磁力计 2 X 轴比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 2 Z-axis scaling factor</source>
      <translation>磁力计 2 Z 轴比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 3 X-axis off diagonal scale factor</source>
      <translation>磁力计 3 X 轴偏离对角线比例因子</translation>
    </message>
    <message>
      <source>Magnetometer 3 Z-axis off diagonal scale factor</source>
      <translation>磁力计 3 Z 轴偏离对角线比例因子</translation>
    </message>
    <message>
      <source>Magnetometer calibration minimum temperature</source>
      <translation>磁力计校准最低温度</translation>
    </message>
    <message>
      <source>Magnetometer max rate</source>
      <translation>磁力计最大速率</translation>
    </message>
    <message>
      <source>Magnetometer offset injected into the simulation (Z-axis)</source>
      <translation>注入模拟的磁力计偏移（Z 轴）</translation>
    </message>
    <message>
      <source>Magnetometer offset temperature ^2 polynomial coefficient - Y axis</source>
      <translation>磁力计偏移温度^2多项式系数-Y轴</translation>
    </message>
    <message>
      <source>Magnetometer soft-iron off-diagonal X component</source>
      <translation>磁力计软铁非对角 X 分量</translation>
    </message>
    <message>
      <source>Main Rotor RPM Sensor </source>
      <translation>主旋翼 RPM 传感器 </translation>
    </message>
    <message>
      <source>MainOut3</source>
      <translation>主输出3</translation>
    </message>
    <message>
      <source>Mainsheet loose, angle between centerline and boom. For direct-control rotating masts, the rotation angle at SERVOx_MAX/_MIN; for rotating masts, this value can exceed 90 degrees if the linkages can physically rotate the mast past that angle.</source>
      <translation>主板松动，中心线与吊杆之间形成角度。对于直接控制旋转桅杆，旋转角度为 SERVOx_MAX/_MIN；对于旋转桅杆，如果连杆可以物理地将桅杆旋转超过该角度，则该值可能超过 90 度。</translation>
    </message>
    <message>
      <source>Manual</source>
      <translation>手动</translation>
    </message>
    <message>
      <source>Manual control input source configuration</source>
      <translation>手动控制输入源配置</translation>
    </message>
    <message>
      <source>Manual mode maximum force</source>
      <translation>手动模式最大力</translation>
    </message>
    <message>
      <source>Manual steering expo to allow faster steering when stick at edges</source>
      <translation>手动转向博览会可在粘在边缘时实现更快的转向</translation>
    </message>
    <message>
      <source>Mapped to center throttle stick in Stabilized mode (see MPC_THR_CURVE). Used for initialization of the hover thrust estimator (see MPC_USE_HTE). The estimated hover thrust is used as base for zero vertical acceleration in altitude control. The hover thrust is important for land detection to work correctly.</source>
      <translation>在稳定模式下映射到中心油门杆（参见 MPC_THR_CURVE）。用于初始化悬停推力估计器（参见 MPC_USE_HTE）。估计的悬停推力用作高度控制中零垂直加速度的基础。悬停推力对于陆地探测的正常工作非常重要。</translation>
    </message>
    <message>
      <source>MappyDot Sensor 4 Rotation</source>
      <translation>MappyDot 传感器 4 旋转</translation>
    </message>
    <message>
      <source>Margin that is added to the time estimate, after it has already been scaled Time with margin = RTL_TIME_FACTOR * time + RTL_TIME_MARGIN</source>
      <translation>缩放后添加到时间估计中的裕度 具有裕度的时间 = RTL_TIME_FACTOR * 时间 + RTL_TIME_MARGIN</translation>
    </message>
    <message>
      <source>Mask Channel 13 as a mode button</source>
      <translation>屏蔽通道 13 作为模式按钮</translation>
    </message>
    <message>
      <source>Mask Channel 4 as a mode button</source>
      <translation>屏蔽通道 4 作为模式按钮</translation>
    </message>
    <message>
      <source>Mask of R/C channels to pass directly to corresponding output channel when in MANUAL mode. When in any mode except MANUAL the channels selected with this option behave normally. This parameter is designed to allow for complex mixing strategies to be used for MANUAL flight using transmitter based mixing. Note that when this option is used you need to be very careful with pre-flight checks to ensure that the output is correct both in MANUAL and non-MANUAL modes.</source>
      <translation>在 MANUAL 模式下，R/C 通道的掩码直接传递到相应的输出通道。当处于除 MANUAL 之外的任何模式时，使用此选项选择的通道将正常运行。该参数旨在允许使用基于发射器的混合的 MANUAL 飞行使用复杂的混合策略。请注意，使用此选项时，您需要非常小心地进行飞行前检查，以确保 MANUAL 和非 MANUAL 模式下的输出正确。</translation>
    </message>
    <message>
      <source>Mask of channels which are reversible. This is used for ESCs which have been configured in '3D' mode, allowing for the motor to spin in either direction</source>
      <translation>可逆通道掩码。这用于已配置为“3D”模式的 ESC，允许电机沿任一方向旋转</translation>
    </message>
    <message>
      <source>MavLink2</source>
      <translation>MavLink2</translation>
    </message>
    <message>
      <source>Mavlink Update Rate</source>
      <translation>Mavlink 更新率</translation>
    </message>
    <message>
      <source>Max acro roll rate</source>
      <translation>最大特技横滚率</translation>
    </message>
    <message>
      <source>Max input value for RSSI reading</source>
      <translation>RSSI 读取的最大输入值</translation>
    </message>
    <message>
      <source>Max propeller torque</source>
      <translation>最大螺旋桨扭矩</translation>
    </message>
    <message>
      <source>MaxBotix MB12XX Sensor 0 Rotation</source>
      <translation>MaxBotix MB12XX 传感器 0 旋转</translation>
    </message>
    <message>
      <source>MaxBotix MB12XX Sensor 6 Rotation</source>
      <translation>MaxBotix MB12XX 传感器 6 旋转</translation>
    </message>
    <message>
      <source>MaxbotixSerial</source>
      <translation>Maxbotix系列</translation>
    </message>
    <message>
      <source>Maximum Airspeed (CAS)</source>
      <translation>最大空速 (CAS)</translation>
    </message>
    <message>
      <source>Maximum DataFlash File Backend buffer size (in kilobytes)</source>
      <translation>最大 DataFlash 文件后端缓冲区大小（以千字节为单位）</translation>
    </message>
    <message>
      <source>Maximum MAVLink sending rate for instance 2</source>
      <translation>实例 2 的最大 MAVLink 发送速率</translation>
    </message>
    <message>
      <source>Maximum RPM to report</source>
      <translation>报告的最大 RPM</translation>
    </message>
    <message>
      <source>Maximum acceleration in pitch axis</source>
      <translation>俯仰轴最大加速度</translation>
    </message>
    <message>
      <source>Maximum airspeed demanded in automatic throttle modes. Should be set slightly less than level flight speed at THR_MAX and also at least 50% above AIRSPEED_MIN to allow for accurate TECS altitude control.</source>
      <translation>自动油门模式下所需的最大空速。应设置为略低于 THR_MAX 的水平飞行速度，并且至少高于 AIRSPEED_MIN 的 50%，以实现精确的 TECS 高度控制。</translation>
    </message>
    <message>
      <source>Maximum allowed current</source>
      <translation>最大允许电流</translation>
    </message>
    <message>
      <source>Maximum altitude allowed before geofence triggers</source>
      <translation>地理围栏触发前允许的最大高度</translation>
    </message>
    <message>
      <source>Maximum ascent velocity</source>
      <translation>最大上升速度</translation>
    </message>
    <message>
      <source>Maximum collective blade pitch angle in deg that corresponds to the PWM set for maximum collective pitch (H_COL_MAX).</source>
      <translation>最大总距叶片桨距角（以度为单位），对应于 PWM 设置的最大总距桨距 (H_COL_MAX)。</translation>
    </message>
    <message>
      <source>Maximum data transmit rate to the modem in bytes/second. Use zero for unlimited</source>
      <translation>调制解调器的最大数据传输速率（以字节/秒为单位）。使用零表示无限</translation>
    </message>
    <message>
      <source>Maximum descent velocity</source>
      <translation>最大下降速度</translation>
    </message>
    <message>
      <source>Maximum duration of a back transition</source>
      <translation>向后过渡的最大持续时间</translation>
    </message>
    <message>
      <source>Maximum height above the ground (if available, otherwise above Home if available, otherwise above the local origin) where triggering a quad-chute is possible. At high altitudes there is a big risk to deplete the battery and therefore crash if quad-chuting there.</source>
      <translation>可以触发四滑道的离地最大高度（如果可用，否则高于 Home（如果可用），否则高于本地原点）。在高海拔地区，如果在高海拔地区进行四轮降落，电池耗尽的风险很大，因此会发生坠机。</translation>
    </message>
    <message>
      <source>Maximum horizontal velocity</source>
      <translation>最大水平速度</translation>
    </message>
    <message>
      <source>Maximum integrator value</source>
      <translation>最大积分值</translation>
    </message>
    <message>
      <source>Maximum lean angle in all VTOL flight modes</source>
      <translation>VTOL所有飞行模式下的最大倾斜角</translation>
    </message>
    <message>
      <source>Maximum lookahead distance for the pure pursuit controller</source>
      <translation>纯追踪控制器的最大前视距离</translation>
    </message>
    <message>
      <source>Maximum number of GPS loss events</source>
      <translation>GPS丢失事件最大数量</translation>
    </message>
    <message>
      <source>Maximum number of obstacles to track</source>
      <translation>可追踪的最大障碍物数量</translation>
    </message>
    <message>
      <source>Maximum pan angle</source>
      <translation>最大平移角度</translation>
    </message>
    <message>
      <source>Maximum pitch down angle commanded in modes with stabilized limits</source>
      <translation>在具有稳定限制的模式下命令的最大俯仰角</translation>
    </message>
    <message>
      <source>Maximum power altitude, relative to the home location</source>
      <translation>最大功率高度，相对于家乡位置</translation>
    </message>
    <message>
      <source>Maximum rate when releasing line</source>
      <translation>放线时最大速率</translation>
    </message>
    <message>
      <source>Maximum sideways velocity in Position mode</source>
      <translation>位置模式下的最大侧向速度</translation>
    </message>
    <message>
      <source>Maximum starting temperature for thermal calibration</source>
      <translation>热校准的最高起始温度</translation>
    </message>
    <message>
      <source>Maximum tilt angle</source>
      <translation>最大倾斜角度</translation>
    </message>
    <message>
      <source>Maximum transmit rate for Notify State Message</source>
      <translation>通知状态消息的最大传输速率</translation>
    </message>
    <message>
      <source>Maximum value for yaw D gain</source>
      <translation>偏航 D 增益的最大值</translation>
    </message>
    <message>
      <source>Maximum vertical acceleration used to correct speed or height errors.</source>
      <translation>用于校正速度或高度误差的最大垂直加速度。</translation>
    </message>
    <message>
      <source>Maximum voltage supplied by analog wind vane. When using pin 103, the maximum value of the parameter is 3.3V.</source>
      <translation>模拟风向标提供的最大电压。当使用103 引脚时，该参数的最大值为3.3V。</translation>
    </message>
    <message>
      <source>Maxmimum output value (when not disarmed).</source>
      <translation>最大输出值（未上锁时）。</translation>
    </message>
    <message>
      <source>Measurement noise for GNSS velocity</source>
      <translation>GNSS 速度的测量噪声</translation>
    </message>
    <message>
      <source>Measurement noise for magnetic heading fusion</source>
      <translation>磁航向融合的测量噪声</translation>
    </message>
    <message>
      <source>Measurement noise for vision velocity measurements</source>
      <translation>视觉速度测量的测量噪声</translation>
    </message>
    <message>
      <source>Message display duration in seconds</source>
      <translation>消息显示持续时间（以秒为单位）</translation>
    </message>
    <message>
      <source>MicroStrain External Magnetometer Orientation (Pitch)</source>
      <translation>MicroStrain 外部磁力计方向（俯仰）</translation>
    </message>
    <message>
      <source>MicroStrain GNSS lever arm offset 2 (X)</source>
      <translation>MicroStrain GNSS 杠杆臂偏移 2 (X)</translation>
    </message>
    <message>
      <source>MicroStrain barometer data rate</source>
      <translation>MicroStrain 气压计数据速率</translation>
    </message>
    <message>
      <source>MicroStrain optical flow offset (Z)</source>
      <translation>MicroStrain 光流偏移 (Z)</translation>
    </message>
    <message>
      <source>Min</source>
      <translation>最小</translation>
    </message>
    <message>
      <source>MinChannelCW</source>
      <translation>最小频道CW</translation>
    </message>
    <message>
      <source>Minimum GPS fix type required for GPS usage.</source>
      <translation>使用 GPS 所需的最低 GPS 修复类型。</translation>
    </message>
    <message>
      <source>Minimum Output Value for DShot in percent. The value depends on the ESC. Make sure to set this high enough so that the motors are always spinning while armed.</source>
      <translation>DShot 的最小输出值（以百分比表示）。该值取决于 ESC。确保将其设置得足够高，以便电机在解锁时始终旋转。</translation>
    </message>
    <message>
      <source>Minimum RPM to report</source>
      <translation>报告的最低 RPM</translation>
    </message>
    <message>
      <source>Minimum airspeed demanded in automatic throttle modes. Should be set to 20% higher than level flight stall speed.</source>
      <translation>自动油门模式下所需的最小空速。应将 20% 设置为高于平飞失速速度。</translation>
    </message>
    <message>
      <source>Minimum altitude below which proximity should not work.</source>
      <translation>最低高度，低于该高度则接近不起作用。</translation>
    </message>
    <message>
      <source>Minimum amount of time in seconds to move in pitch</source>
      <translation>俯仰移动的最短时间（以秒为单位）</translation>
    </message>
    <message>
      <source>Minimum collective blade pitch angle when landed in degrees for non-manual collective modes (i.e. modes that use altitude hold).</source>
      <translation>非手动总距模式（即使用高度保持的模式）着陆时的最小总距叶片桨距角（以度为单位）。</translation>
    </message>
    <message>
      <source>Minimum distance in centimeters that rangefinder can reliably read</source>
      <translation>测距仪可以可靠读取的最小距离（以厘米为单位）</translation>
    </message>
    <message>
      <source>Minimum expected range for Proximity Sensor. Setting this to 0 will set value to manufacturer reported range.</source>
      <translation>接近传感器的最小预期范围。将其设置为 0 会将值设置为制造商报告的范围。</translation>
    </message>
    <message>
      <source>Minimum groundspeed</source>
      <translation>最小行驶速度</translation>
    </message>
    <message>
      <source>Minimum lookahead distance for the pure pursuit controller</source>
      <translation>纯追踪控制器的最小前视距离</translation>
    </message>
    <message>
      <source>Minimum physical pan (yaw) angular position of mount.</source>
      <translation>安装座的最小物理平移（偏航）角度位置。</translation>
    </message>
    <message>
      <source>Minimum pitch in auto flight</source>
      <translation>自动飞行时的最小俯仰角</translation>
    </message>
    <message>
      <source>Minimum signal strength</source>
      <translation>最小信号强度</translation>
    </message>
    <message>
      <source>Minimum tailsitter gain scaling</source>
      <translation>最小尾随增益缩放</translation>
    </message>
    <message>
      <source>Minimum time to apply a yaw reversal</source>
      <translation>应用偏航反转的最短时间</translation>
    </message>
    <message>
      <source>Minimum voltage supplied by analog wind vane</source>
      <translation>模拟风向标提供的最小电压</translation>
    </message>
    <message>
      <source>Mission Restart when entering Auto mode</source>
      <translation>进入自动模式时任务重新启动</translation>
    </message>
    <message>
      <source>Mixer Yaw</source>
      <translation>混控器偏航</translation>
    </message>
    <message>
      <source>Mode slot 3</source>
      <translation>模式插槽 3</translation>
    </message>
    <message>
      <source>Mode1</source>
      <translation>模式1</translation>
    </message>
    <message>
      <source>Mode1+2+3+5+6</source>
      <translation>型号1+2+3+5+6</translation>
    </message>
    <message>
      <source>Mode1+2+6</source>
      <translation>模式1+2+6</translation>
    </message>
    <message>
      <source>Mode1+3+6</source>
      <translation>模式1+3+6</translation>
    </message>
    <message>
      <source>Mode2</source>
      <translation>模式2</translation>
    </message>
    <message>
      <source>Mode2+3+6</source>
      <translation>模式2+3+6</translation>
    </message>
    <message>
      <source>Mode3</source>
      <translation>模式3</translation>
    </message>
    <message>
      <source>Mode4</source>
      <translation>模式4</translation>
    </message>
    <message>
      <source>ModeAcro</source>
      <translation>模式特技</translation>
    </message>
    <message>
      <source>ModeGuided</source>
      <translation>模式引导</translation>
    </message>
    <message>
      <source>ModeStabilize</source>
      <translation>模式稳定</translation>
    </message>
    <message>
      <source>Models differ in range and FoV.</source>
      <translation>型号在范围和 FoV 方面有所不同。</translation>
    </message>
    <message>
      <source>Module ID 12</source>
      <translation>模块 ID 12</translation>
    </message>
    <message>
      <source>Module ID 2</source>
      <translation>模块 ID 2</translation>
    </message>
    <message>
      <source>Module ID 27</source>
      <translation>模块 ID 27</translation>
    </message>
    <message>
      <source>Module ID 34</source>
      <translation>模块 ID 34</translation>
    </message>
    <message>
      <source>Module ID 41</source>
      <translation>模块 ID 41</translation>
    </message>
    <message>
      <source>Module ID 49</source>
      <translation>模块 ID 49</translation>
    </message>
    <message>
      <source>Module ID 56</source>
      <translation>模块 ID 56</translation>
    </message>
    <message>
      <source>Module ID 7</source>
      <translation>模块 ID 7</translation>
    </message>
    <message>
      <source>Module Param - If the flight controller uses 2D or 3D communication</source>
      <translation>模块参数 - 如果飞控使用 2D 或 3D 通信</translation>
    </message>
    <message>
      <source>Module Param - The module's Throttle Control Value Index</source>
      <translation>Module Param - 模块的油门控制值索引</translation>
    </message>
    <message>
      <source>Moment coefficient of rotor 4</source>
      <translation>转子力矩系数4</translation>
    </message>
    <message>
      <source>Motion Capture</source>
      <translation>动作捕捉</translation>
    </message>
    <message>
      <source>Motor 11 slew rate limit</source>
      <translation>电机 11 转换速率限制</translation>
    </message>
    <message>
      <source>Motor 5</source>
      <translation>电机5</translation>
    </message>
    <message>
      <source>Motor 9</source>
      <translation>电机9</translation>
    </message>
    <message>
      <source>Motor Failure Hysteresis Time</source>
      <translation>电机故障迟滞时间</translation>
    </message>
    <message>
      <source>Motor Spin armed</source>
      <translation>电机旋转解锁</translation>
    </message>
    <message>
      <source>Motor failure only triggers after current thresholds are exceeded for this time.</source>
      <translation>仅在超过本次电流阈值后才会触发电机故障。</translation>
    </message>
    <message>
      <source>Motor interference compensation for body frame Y axis</source>
      <translation>机身Y轴电机干涉补偿</translation>
    </message>
    <message>
      <source>Motor interference compensation type</source>
      <translation>电机干扰补偿型</translation>
    </message>
    <message>
      <source>Motor speed scaling base speed</source>
      <translation>电机速度缩放基速</translation>
    </message>
    <message>
      <source>Motor11</source>
      <translation>电机11</translation>
    </message>
    <message>
      <source>Motor19</source>
      <translation>电机19</translation>
    </message>
    <message>
      <source>Motor26</source>
      <translation>电机26</translation>
    </message>
    <message>
      <source>Motor4</source>
      <translation>电机4</translation>
    </message>
    <message>
      <source>MotorTilt</source>
      <translation>电机倾斜</translation>
    </message>
    <message>
      <source>Mount LRF enable</source>
      <translation>挂载LRF使能</translation>
    </message>
    <message>
      <source>Mount Roll angle maximum</source>
      <translation>云台横滚角最大</translation>
    </message>
    <message>
      <source>Mount Yaw angle maximum</source>
      <translation>安装偏航角最大</translation>
    </message>
    <message>
      <source>Mount options</source>
      <translation>安装选项</translation>
    </message>
    <message>
      <source>Mount roll angle when in retracted position</source>
      <translation>处于缩回位置时云台横滚角度</translation>
    </message>
    <message>
      <source>Mount1 Yaw</source>
      <translation>挂载1偏航</translation>
    </message>
    <message>
      <source>Mount2 Yaw</source>
      <translation>挂载2偏航</translation>
    </message>
    <message>
      <source>Mount2's Roll stabilization lead time</source>
      <translation>Mount2的侧倾稳定前置时间</translation>
    </message>
    <message>
      <source>Mount2's minimum physical roll angular position</source>
      <translation>Mount2的最小物理横滚角位置</translation>
    </message>
    <message>
      <source>Mount2Pan</source>
      <translation>平移安装</translation>
    </message>
    <message>
      <source>MountRoll</source>
      <translation>罗尔山</translation>
    </message>
    <message>
      <source>Moving Base (Moving Base UART1 Connected to Autopilot Or Can Node At 921600)</source>
      <translation>移动底座（移动底座UART1连接飞控或者可以节点在921600）</translation>
    </message>
    <message>
      <source>Multi GPS Blending Control Mask</source>
      <translation>多 GPS 混合控制面膜</translation>
    </message>
    <message>
      <source>MultiCopterInput</source>
      <translation>多旋翼输入</translation>
    </message>
    <message>
      <source>Multiplicative correction factor for the feedforward mapping of the yaw rate controller.
Increase this value (x &gt; 1) if the measured yaw rate is lower than the setpoint, decrease (0 &lt; x &lt; 1) otherwise.
Note: Tuning this is particularly useful for skid-steered rovers, or rovers with misaligned wheels/steering axes
that cause a lot of friction when turning.</source>
      <translation>偏航率控制器前馈映射的乘法校正因子。如果测得的偏航率低于设定值，则增大该值 (x &gt; 1)，否则减小该值 (0 &lt; x &lt; 1)。注意：调整此项对于滑移转向流动站或车轮/转向轴未对准的流动站特别有用，这些流动站在转弯时会产生大量摩擦。</translation>
    </message>
    <message>
      <source>Multiplied by the current throttle and added to compass2's z-axis values to compensate for motor interference</source>
      <translation>乘以当前油门并添加到compass2的z轴值以补偿电机干扰</translation>
    </message>
    <message>
      <source>Multiplied by the current throttle and added to the compass's x-axis values to compensate for motor interference</source>
      <translation>乘以当前油门并添加到罗盘的 x 轴值以补偿电机干扰</translation>
    </message>
    <message>
      <source>Multiplied by the track error boundary to determine when the aircraft switches
to the next waypoint and/or path segment. Should be less than 1.</source>
      <translation>乘以航迹误差边界以确定飞机何时切换到下一个航路点和/或路径段。应小于 1。</translation>
    </message>
    <message>
      <source>Murata SCH16T IMU (external SPI)</source>
      <translation>村田SCH16T IMU（外部SPI）</translation>
    </message>
    <message>
      <source>NED anomaly vector at ground level</source>
      <translation>NED 地面异常矢量</translation>
    </message>
    <message>
      <source>NMEA 2000 CAN driver. Use 1 for first CAN scripting driver, 2 for 2nd driver</source>
      <translation>NMEA 2000 CAN 驱动程序。使用 1 作为第一个 CAN 脚本驱动程序，使用 2 作为第二个驱动程序</translation>
    </message>
    <message>
      <source>NOTE: max(FW_LND_FLALT, FW_LND_FL_TIME * |z-velocity|) is taken as the flare altitude</source>
      <translation>NOTE: max(FW_LND_FLALT, FW_LND_FL_TIME * |z-velocity|) 取为耀斑高度</translation>
    </message>
    <message>
      <source>NSats</source>
      <translation>卫星</translation>
    </message>
    <message>
      <source>Navigation Tuning</source>
      <translation>导航调整</translation>
    </message>
    <message>
      <source>Navigator Leak1</source>
      <translation>导航器泄漏1</translation>
    </message>
    <message>
      <source>NeoPixel2</source>
      <translation>NeoPixel2</translation>
    </message>
    <message>
      <source>Neutral</source>
      <translation>中性</translation>
    </message>
    <message>
      <source>Nmumber of signals per rotation of actuator</source>
      <translation>执行器每转一圈的信号数量</translation>
    </message>
    <message>
      <source>No Scripts to run message if all scripts have stopped</source>
      <translation>如果所有脚本都已停止，则没有可运行的脚本消息</translation>
    </message>
    <message>
      <source>No precision landing</source>
      <translation>无法精确着陆</translation>
    </message>
    <message>
      <source>NoChange</source>
      <translation>无变化</translation>
    </message>
    <message>
      <source>Nominal RTD resistance</source>
      <translation>标称 RTD 电阻</translation>
    </message>
    <message>
      <source>NoopLoop_TOFSense_CAN</source>
      <translation>NoopLoop_TOFSense_CAN</translation>
    </message>
    <message>
      <source>North-East-Down</source>
      <translation>东北向下</translation>
    </message>
    <message>
      <source>Notch Filter</source>
      <translation>陷波滤波器</translation>
    </message>
    <message>
      <source>Notch bandwidth in Hz</source>
      <translation>陷波带宽（Hz）</translation>
    </message>
    <message>
      <source>Note: actuator failure needs to be enabled and configured via FD_ACT_*
parameters.</source>
      <translation>注意：执行器故障需要通过 FD_ACT_* 参数启用和配置。</translation>
    </message>
    <message>
      <source>Notify the user that on failsafe a QuadPlan will land. 3 or less to prevent arming. -1 to disable.</source>
      <translation>通知用户在故障安全时，QuadPlan 将着陆。 3 或以下以防止解锁。 -1 禁用。</translation>
    </message>
    <message>
      <source>Number attempts for starting the engine</source>
      <translation>尝试启动发动机的次数</translation>
    </message>
    <message>
      <source>Number of amps that a 1V reading on the current sensor corresponds to.</source>
      <translation>电流传感器上的 1V 读数对应的安培数。</translation>
    </message>
    <message>
      <source>Number of cycles to run servo test on boot-up</source>
      <translation>启动时运行舵机测试的周期数</translation>
    </message>
    <message>
      <source>Number of motor poles for eRPM scaling</source>
      <translation>用于 eRPM 缩放的电机极数</translation>
    </message>
    <message>
      <source>Number of satellites GPS 1 has in view</source>
      <translation>GPS 1 所看到的卫星数量</translation>
    </message>
    <message>
      <source>Number of standard deviations applied to the re-enable EKF consistency check that is used when ARSPD_OPTIONS bit position 3 is set. Larger values will make the re-enabling of the airspeed sensor faster, but increase the likelihood of re-enabling a degraded sensor. The value can be tuned by using the ARSP.TR log message by setting ARSP_WIND_GATE to a value that is higher than the value for ARSP.TR observed with a healthy airspeed sensor. Occasional transients in ARSP.TR above the value set by ARSP_WIND_GATE can be tolerated provided they are less than 5 seconds in duration and less than 10% duty cycle.</source>
      <translation>当设置 ARSPD_OPTIONS 位位置 3 时，应用于重新启用 EKF 一致性检查的标准偏差数。较大的值将使空速传感器的重新启用速度更快，但会增加重新启用退化传感器的可能性。可以使用 ARSP.TR 日志消息调整该值，方法是将 ARSP_WIND_GATE 设置为高于使用正常空速传感器观察到的 ARSP.TR 值。 ARSP.TR 中偶尔出现高于 ARSP_WIND_GATE 设置值的瞬变是可以容忍的，前提是它们的持续时间小于 5 秒且小于 10% 占空比。</translation>
    </message>
    <message>
      <source>OADatabase Minimum  radius</source>
      <translation>OA数据库最小半径</translation>
    </message>
    <message>
      <source>OADatabase output level to configure which database objects are sent to the ground station. All data is always available internally for avoidance algorithms.</source>
      <translation>OADatabase 输出级别配置哪些数据库对象发送到地面站。所有数据始终可在内部用于规避算法。</translation>
    </message>
    <message>
      <source>ODI_X in the magnetometer soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>磁力计软铁校准矩阵中的ODI_X：[[DIA_X、ODI_X、ODI_Y]、[ODI_X、DIA_Y、ODI_Z]、[ODI_Y、ODI_Z、DIA_Z]]</translation>
    </message>
    <message>
      <source>ODI_Z in the magnetometer soft-iron calibration matrix: [[DIA_X, ODI_X, ODI_Y], [ODI_X, DIA_Y, ODI_Z], [ODI_Y, ODI_Z, DIA_Z]]</source>
      <translation>磁力计软铁校准矩阵中的ODI_Z：[[DIA_X、ODI_X、ODI_Y]、[ODI_X、DIA_Y、ODI_Z]、[ODI_Y、ODI_Z、DIA_Z]]</translation>
    </message>
    <message>
      <source>OSD Dwell Time (ms)</source>
      <translation>OSD 停留时间（毫秒）</translation>
    </message>
    <message>
      <source>OSD type</source>
      <translation>OSD型</translation>
    </message>
    <message>
      <source>Object Avoidance wide margin distance</source>
      <translation>物体回避宽裕度距离</translation>
    </message>
    <message>
      <source>Octorotor</source>
      <translation>八旋翼</translation>
    </message>
    <message>
      <source>Offset (m) in the Y direction if an Optical Flow sensor is connected. Requires MS_OPT_FLOW_EN to be enabled to be used.</source>
      <translation>如果连接了光流传感器，则 Y 方向的偏移量 (m)。需要启用 MS_OPT_FLOW_EN 才能使用。</translation>
    </message>
    <message>
      <source>Offset for roll channel output in degrees</source>
      <translation>横滚通道输出的偏移量（以度为单位）</translation>
    </message>
    <message>
      <source>Offset in volts for zero distance for analog rangefinders. Offset added to distance in centimeters for PWM and I2C Lidars</source>
      <translation>模拟测距仪零距离偏移（以伏特为单位）。 PWM 和 I2C 激光雷达的距离添加偏移（以厘米为单位）</translation>
    </message>
    <message>
      <source>Offset to be added to compass3's x-axis values to compensate for metal in the frame</source>
      <translation>添加到 compass3 的 x 轴值的偏移量以补偿框架中的金属</translation>
    </message>
    <message>
      <source>Omni3Mecanum</source>
      <translation>Omni3麦克纳姆</translation>
    </message>
    <message>
      <source>On in all Q modes except QAUTOTUNE and QACRO</source>
      <translation>除 QAUTOTUNE 和 QACRO 外，所有 Q 模式均开启</translation>
    </message>
    <message>
      <source>Oneshot output mask</source>
      <translation>一次性输出掩码</translation>
    </message>
    <message>
      <source>Only for H3 swashplate.  If pitching the swash forward induces a roll, this can be correct the problem</source>
      <translation>仅适用于 H3 斜盘。  如果向前倾斜斜盘会引起横滚，这可以解决问题</translation>
    </message>
    <message>
      <source>Only used in Position mode.</source>
      <translation>仅在位置模式下使用。</translation>
    </message>
    <message>
      <source>Opflow Delay</source>
      <translation>操作流延迟</translation>
    </message>
    <message>
      <source>Optflow may use terrain alt</source>
      <translation>Optflow 可能会使用地形替代</translation>
    </message>
    <message>
      <source>Optical flow aiding</source>
      <translation>光流辅助</translation>
    </message>
    <message>
      <source>Optical flow max rate</source>
      <translation>光流最大速率</translation>
    </message>
    <message>
      <source>Optical flow options. Bit 0 should be set if the sensor is stabilised (e.g. mounted on a gimbal)</source>
      <translation>光流选项。如果传感器稳定（例如安装在万向节上），则应设置位 0</translation>
    </message>
    <message>
      <source>Optical flow z offset from center</source>
      <translation>光流 z 相对中心的偏移</translation>
    </message>
    <message>
      <source>Optional EKF behaviour</source>
      <translation>可选的 EKF 行为</translation>
    </message>
    <message>
      <source>Options for Pin 1. PWM input detects PWM above or below 1800/1200us instead of logic level. If PWM is not detected or is less than 800us or above 2200us the button will interpreted as low. Invert changes HIGH state to be logic low voltage on pin, or below 1200us, if PWM input.</source>
      <translation>引脚 1 的选项。 PWM 输入检测 PWM 高于或低于 1800/1200us，而不是逻辑电平。如果未检测到 PWM 或小于 800us 或高于 2200us，则按钮将被解释为低电平。反转将 HIGH 状态更改为引脚上的逻辑低电压，或低于 1200us（如果 PWM 输入）。</translation>
    </message>
    <message>
      <source>Options for emergency failsafe codes and device capabilities</source>
      <translation>紧急故障安全代码和设备功能选项</translation>
    </message>
    <message>
      <source>Options that can be applied to change RTL mode behaviour</source>
      <translation>可用于更改 RTL 模式行为的选项</translation>
    </message>
    <message>
      <source>Options to retract or deploy landing gear in Auto, Takeoff and Autoland modes</source>
      <translation>在自动、起飞和自动着陆模式下收起或展开起落架的选项</translation>
    </message>
    <message>
      <source>Orientation of rangefinder</source>
      <translation>测距仪方向</translation>
    </message>
    <message>
      <source>Output 1</source>
      <translation>输出1</translation>
    </message>
    <message>
      <source>Output 17</source>
      <translation>输出17</translation>
    </message>
    <message>
      <source>Output 24</source>
      <translation>输出24</translation>
    </message>
    <message>
      <source>Output 31</source>
      <translation>输出31</translation>
    </message>
    <message>
      <source>Output PWM type</source>
      <translation>输出PWM型</translation>
    </message>
    <message>
      <source>Output channels to be transmitted as ESC over DroneCAN</source>
      <translation>通过 DroneCAN 作为 ESC 传输的输出通道</translation>
    </message>
    <message>
      <source>Output rate of ESC command messages</source>
      <translation>ESC命令消息的输出速率</translation>
    </message>
    <message>
      <source>Output13</source>
      <translation>输出13</translation>
    </message>
    <message>
      <source>Output20</source>
      <translation>输出20</translation>
    </message>
    <message>
      <source>Output28</source>
      <translation>输出28</translation>
    </message>
    <message>
      <source>Output6</source>
      <translation>输出6</translation>
    </message>
    <message>
      <source>Overall Vertical Velocity Limit</source>
      <translation>总垂直速度限制</translation>
    </message>
    <message>
      <source>Overrides LIM_PITCH_MAX in automatic throttle modes to reduce climb rates. Uses LIM_PITCH_MAX if set to 0. For proper TECS tuning, set to the angle that the aircraft can climb at TRIM_ARSPD_CM and THR_MAX.</source>
      <translation>在自动油门模式下覆盖 LIM_PITCH_MAX 以降低爬升率。如果设置为 0，则使用 LIM_PITCH_MAX。为了正确调整 TECS，请设置为飞机可以在 TRIM_ARSPD_CM 和 THR_MAX 爬升的角度。</translation>
    </message>
    <message>
      <source>P gain</source>
      <translation>磷增益</translation>
    </message>
    <message>
      <source>PAA3905 Optical Flow</source>
      <translation>PAA3905光流</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 1 Disarmed Value</source>
      <translation>PCA9685 输出通道 1 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 10 Maximum Value</source>
      <translation>PCA9685 输出通道 10 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 11 Output Function</source>
      <translation>PCA9685 输出通道 11 输出功能</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 13 Disarmed Value</source>
      <translation>PCA9685 输出通道 13 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 14 Maximum Value</source>
      <translation>PCA9685 输出通道 14 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 15 Output Function</source>
      <translation>PCA9685 输出通道 15 输出功能</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 2 Disarmed Value</source>
      <translation>PCA9685 输出通道 2 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 3 Maximum Value</source>
      <translation>PCA9685 输出通道 3 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 4 Output Function</source>
      <translation>PCA9685 输出通道 4 输出功能</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 6 Disarmed Value</source>
      <translation>PCA9685 输出通道 6 上锁值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 7 Maximum Value</source>
      <translation>PCA9685 输出通道 7 最大值</translation>
    </message>
    <message>
      <source>PCA9685 Output Channel 8 Output Function</source>
      <translation>PCA9685 输出通道 8 输出功能</translation>
    </message>
    <message>
      <source>PCF8583 rotorfreq (i2c) pool interval</source>
      <translation>PCF8583 转子频率 (i2c) 池间隔</translation>
    </message>
    <message>
      <source>PID Derivative FeedForward Gain</source>
      <translation>PID 微分前馈增益</translation>
    </message>
    <message>
      <source>PID Proportional Gain</source>
      <translation>PID 比例增益</translation>
    </message>
    <message>
      <source>PLUSCODE_X</source>
      <translation>PLUSCODE_X</translation>
    </message>
    <message>
      <source>POWER_X</source>
      <translation>POWER_X</translation>
    </message>
    <message>
      <source>PWM 100 Hz</source>
      <translation>PWM 100 Hz</translation>
    </message>
    <message>
      <source>PWM Aux 1 Maximum Value</source>
      <translation>PWM 辅助 1 最大值</translation>
    </message>
    <message>
      <source>PWM Aux 2 Minimum Value</source>
      <translation>PWM 辅助 2 最小值</translation>
    </message>
    <message>
      <source>PWM Aux 3 Output Function</source>
      <translation>PWM 辅助 3 输出功能</translation>
    </message>
    <message>
      <source>PWM Aux 5</source>
      <translation>PWM 辅助 5</translation>
    </message>
    <message>
      <source>PWM Aux 6 Center Value</source>
      <translation>PWM 辅助 6 中心值</translation>
    </message>
    <message>
      <source>PWM Aux 7 Disarmed Value</source>
      <translation>PWM 辅助 7 上锁值</translation>
    </message>
    <message>
      <source>PWM Aux 8 Failsafe Value</source>
      <translation>PWM 辅助 8 故障安全值</translation>
    </message>
    <message>
      <source>PWM Capture 1 Maximum Value</source>
      <translation>PWM 捕获 1 最大值</translation>
    </message>
    <message>
      <source>PWM Capture 2 Minimum Value</source>
      <translation>PWM 捕获 2 最小值</translation>
    </message>
    <message>
      <source>PWM Capture 3 Output Function</source>
      <translation>PWM 捕捉 3 输出功能</translation>
    </message>
    <message>
      <source>PWM in microseconds sent to external gyro on an servo/output whose function is Motor7 when tail type is Servo w/ ExtGyro</source>
      <translation>PWM（以微秒为单位）发送到舵机/输出上的外部陀螺仪，当尾部类型为带有 ExtGyro 的舵机时，其功能为 Motor7</translation>
    </message>
    <message>
      <source>PWM output minimum</source>
      <translation>PWM 输出最小值</translation>
    </message>
    <message>
      <source>PWM value for starter off</source>
      <translation>PWM 启动器关闭值</translation>
    </message>
    <message>
      <source>PWM value sent to Gripper to release the cargo</source>
      <translation>PWM 值发送至 Gripper 以释放货物</translation>
    </message>
    <message>
      <source>PWMRange</source>
      <translation>脉宽调制范围</translation>
    </message>
    <message>
      <source>PX4 FMU Relay1</source>
      <translation>PX4 FMU 继电器1</translation>
    </message>
    <message>
      <source>PX4-airspeed port</source>
      <translation>PX4-空速口</translation>
    </message>
    <message>
      <source>Packet rate channel</source>
      <translation>包速率通道</translation>
    </message>
    <message>
      <source>Parachute Servo PWM value in microseconds when parachute is released</source>
      <translation>降落伞释放时降落伞舵机 PWM 值（以微秒为单位）</translation>
    </message>
    <message>
      <source>Parachute release delay</source>
      <translation>降落伞释放延迟</translation>
    </message>
    <message>
      <source>Parameter increment</source>
      <translation>参数增量</translation>
    </message>
    <message>
      <source>Parameter type</source>
      <translation>参数类型</translation>
    </message>
    <message>
      <source>Passthrough</source>
      <translation>直通</translation>
    </message>
    <message>
      <source>Payload Place minimum range finder altitude</source>
      <translation>有效负载 放置最小测距仪高度</translation>
    </message>
    <message>
      <source>Percentage exponential for pitch input in MANUAL, ACRO and TRAINING modes</source>
      <translation>MANUAL、ACRO 和 TRAINING 模式下俯仰输入的百分比指数</translation>
    </message>
    <message>
      <source>Performance</source>
      <translation>表现</translation>
    </message>
    <message>
      <source>Peripheral via Actuator Set 3</source>
      <translation>通过执行器套件 3 的外围设备</translation>
    </message>
    <message>
      <source>Physical coefficient representing the friction with air particules during rotations.
The greater this value, the slower the quad will rotate.
Aerodynamic moment function of body rate: Ma=-KDW*W_B.
This value can be set to 0 if unknown.</source>
      <translation>表示旋转过程中与空气颗粒的摩擦力的物理系数。该值越大，四边形旋转的速度越慢。机体速率的气动力矩函数：Ma=-KDW*W_B。如果未知，可以将该值设置为 0。</translation>
    </message>
    <message>
      <source>Pilot controlled yaw expo</source>
      <translation>先导偏航博览会</translation>
    </message>
    <message>
      <source>Pilot maximum vertical ascending speed</source>
      <translation>飞行员最大垂直上升速度</translation>
    </message>
    <message>
      <source>Pilot takeoff altitude</source>
      <translation>飞行员起飞高度</translation>
    </message>
    <message>
      <source>Pin number indicating location of analog airspeed sensors. Pixhawk/Cube if set to 15. </source>
      <translation>指示模拟空速传感器位置的引脚号。 Pixhawk/Cube（如果设置为 15）。 </translation>
    </message>
    <message>
      <source>Pin used to read the RSSI voltage or PWM value. Analog Airspeed ports can be used for Analog inputs (some autopilots provide others also), Non-IOMCU Servo/MotorOutputs can be used for PWM input when configured as "GPIOs". Values for some autopilots are given as examples. Search wiki for "Analog pins" for analog pin or "GPIOs", if PWM input type, to determine pin number.</source>
      <translation>用于读取 RSSI 电压或 PWM 值的引脚。模拟空速端口可用于模拟输入（某些自动驾驶仪也提供其他端口），当配置为“GPIO”时，非 IOMCU 舵机/电机输出可用于 PWM 输入。一些自动驾驶仪的值作为示例给出。在 wiki 中搜索“模拟引脚”以获取模拟引脚，如果是 PWM 输入类型，则在 wiki 中搜索“GPIO”以确定引脚编号。</translation>
    </message>
    <message>
      <source>Pitch 270°</source>
      <translation>节距270°</translation>
    </message>
    <message>
      <source>Pitch Target notch filter index</source>
      <translation>俯仰目标陷波滤波器指数</translation>
    </message>
    <message>
      <source>Pitch angle offset</source>
      <translation>俯仰角偏移</translation>
    </message>
    <message>
      <source>Pitch axis controller I gain</source>
      <translation>俯仰轴控制器 I 增益</translation>
    </message>
    <message>
      <source>Pitch axis controller derivative frequency in Hz</source>
      <translation>俯仰轴控制器导数频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Pitch axis rate controller I gain</source>
      <translation>俯仰轴速率控制器 I 增益</translation>
    </message>
    <message>
      <source>Pitch axis rate controller P gain.  Converts the difference between desired pitch rate and actual pitch rate into a motor speed output</source>
      <translation>俯仰轴速率控制器 P 增益。  将所需变桨速率与实际变桨速率之间的差值转换为电机速度输出</translation>
    </message>
    <message>
      <source>Pitch axis rate controller error frequency in Hz</source>
      <translation>俯仰轴速率控制器错误频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Pitch control Derivative FeedForward Gain</source>
      <translation>变桨控制 微分前馈增益</translation>
    </message>
    <message>
      <source>Pitch control I gain maximum</source>
      <translation>桨距控制 I 增益最大</translation>
    </message>
    <message>
      <source>Pitch control Target notch filter index, zero disables</source>
      <translation>俯仰控制 目标陷波滤波器指数，零禁用</translation>
    </message>
    <message>
      <source>Pitch control limit throttle threshold.  Pitch angle will be limited if throttle crosses this threshold (from 0 to 1)</source>
      <translation>变桨控制限制油门阈值。  如果油门超过此阈值（从 0 到 1），俯仰角将受到限制</translation>
    </message>
    <message>
      <source>Pitch differential thrust factor in forward flight</source>
      <translation>向前飞行时的俯仰差推力因数</translation>
    </message>
    <message>
      <source>Pitch input</source>
      <translation>俯仰输入</translation>
    </message>
    <message>
      <source>Pitch proportional gain, i.e. desired angular speed in rad/s for error 1 rad.</source>
      <translation>俯仰比例增益，即误差 1 rad 所需的角速度（以 rad/s 为单位）。</translation>
    </message>
    <message>
      <source>Pitch rate feed forward</source>
      <translation>俯仰率前馈</translation>
    </message>
    <message>
      <source>Pitch rate proportional gain, i.e. control output for angular speed error 1 rad/s.</source>
      <translation>俯仰率比例增益，即角速度误差1 rad/s的控制输出。</translation>
    </message>
    <message>
      <source>Pitch trim increment at minimum airspeed</source>
      <translation>最小空速时的俯仰配平增量</translation>
    </message>
    <message>
      <source>Pitch180Yaw270</source>
      <translation>俯仰180偏航270</translation>
    </message>
    <message>
      <source>Pitch90</source>
      <translation>节距90</translation>
    </message>
    <message>
      <source>Pivot Delay</source>
      <translation>枢轴延迟</translation>
    </message>
    <message>
      <source>Pixhawk</source>
      <translation>皮克斯霍克</translation>
    </message>
    <message>
      <source>Pixhawk AUXOUT2</source>
      <translation>皮克斯霍克 AUXOUT2</translation>
    </message>
    <message>
      <source>Pixhawk Aux4</source>
      <translation>Pixhawk Aux4</translation>
    </message>
    <message>
      <source>Pixhawk TriggerPin53</source>
      <translation>Pixhawk TriggerPin53</translation>
    </message>
    <message>
      <source>Pixhawk2Slim</source>
      <translation>Pixhawk2Slim</translation>
    </message>
    <message>
      <source>PixhawkAUX6</source>
      <translation>PixhawkAUX6</translation>
    </message>
    <message>
      <source>Plane Follow Scripting Overshoot Angle</source>
      <translation>平面跟随脚本过冲角度</translation>
    </message>
    <message>
      <source>Plane Follow Scripting speed P gain</source>
      <translation>平面跟随脚本速度 P 增益</translation>
    </message>
    <message>
      <source>Plane Follow exit mode</source>
      <translation>平面跟随退出模式</translation>
    </message>
    <message>
      <source>Point at which the thrust starts expressed as a number from 0 to 1 in the entire output range.</source>
      <translation>在整个输出范围内，推力开始点用从 0 到 1 的数字表示。</translation>
    </message>
    <message>
      <source>Port number</source>
      <translation>端口号</translation>
    </message>
    <message>
      <source>PosHold flight mode's max lean angle during braking in degrees</source>
      <translation>PosHold 飞行模式制动时的最大倾斜角度（以度为单位）</translation>
    </message>
    <message>
      <source>Position (vertical) controller P gain.  Converts the difference between the desired altitude and actual altitude into a climb or descent rate which is passed to the throttle rate controller</source>
      <translation>位置（垂直）控制器 P 增益。  将所需高度与实际高度之间的差异转换为爬升或下降速率，并将其传递给油门速率控制器</translation>
    </message>
    <message>
      <source>Position Vertical Source</source>
      <translation>位置垂直源</translation>
    </message>
    <message>
      <source>Position minimum at servo min value. This should be within the position control range of the servos, normally 0 to 4095</source>
      <translation>位置最小值位于舵机最小值处。这应该在舵机的位置控制范围内，通常为 0 至 4095</translation>
    </message>
    <message>
      <source>Position of rotor 10 along X body axis relative to center of gravity</source>
      <translation>转子 10 沿 X 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 2 along Z body axis relative to center of gravity</source>
      <translation>转子 2 沿 Z 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 5 along Y body axis relative to center of gravity</source>
      <translation>转子 5 沿 Y 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position of rotor 8 along X body axis relative to center of gravity</source>
      <translation>转子 8 沿 X 体轴相对于重心的位置</translation>
    </message>
    <message>
      <source>Position stream rate to ground station</source>
      <translation>定位到地面站的流速率</translation>
    </message>
    <message>
      <source>Post filter</source>
      <translation>后置过滤器</translation>
    </message>
    <message>
      <source>Powersave range</source>
      <translation>省电范围</translation>
    </message>
    <message>
      <source>PrecLand minimum alt for retry</source>
      <translation>PrecLand 重试最低 alt</translation>
    </message>
    <message>
      <source>Precision Land Type</source>
      <translation>精密焊盘类型</translation>
    </message>
    <message>
      <source>Precland device alt range</source>
      <translation>Preclad 设备 alt 系列</translation>
    </message>
    <message>
      <source>Precland device maximum lateral range</source>
      <translation>Precland 装置最大横向范围</translation>
    </message>
    <message>
      <source>Precland device radiance type: it can be a cylinder, a cone, or a sphere.</source>
      <translation>Precland装置辐射类型：可以是圆柱体、圆锥体或球体。</translation>
    </message>
    <message>
      <source>Pressure error coefficient in negative X direction (backwards)</source>
      <translation>X负方向（向后）压力误差系数</translation>
    </message>
    <message>
      <source>Prevents downforce from pitching the body down when facing wind.
Uses puller/pusher (standard VTOL), or forward-tilt (tiltrotor VTOL) to accelerate forward instead.
Only active if demanded pitch  is below VT_PITCH_MIN.
Use VT_FWD_THRUST_SC to tune it.
Descend mode is treated as Landing too.
Only active (if enabled) in height-rate controlled modes.</source>
      <translation>迎风时防止下压力使机体向下倾斜。使用拉/推器（标准 VTOL）或前倾（倾转旋翼机 VTOL）来向前加速。仅当所需螺距低于 VT_PITCH_MIN 时才有效。使用VT_FWD_THRUST_SC进行调整。下降模式也被视为着陆。仅在高度速率控制模式下有效（如果启用）。</translation>
    </message>
    <message>
      <source>Primary height sensor used by the EKF. If the selected option cannot be used, baro is used. 1 uses the range finder and only with optical flow navigation (EK2_GPS_TYPE = 3), Do not use "1" for terrain following. NOTE: the EK2_RNG_USE_HGT parameter can be used to switch to range-finder when close to the ground.</source>
      <translation>EKF 使用的主要高度传感器。如果无法使用所选选项，则使用 baro。 1 使用测距仪且仅具有光流导航（EK2_GPS_TYPE = 3），请勿使用“1”进行地形跟踪。 NOTE：EK2_RNG_USE_HGT参数可用于接近地面时切换到测距仪。</translation>
    </message>
    <message>
      <source>Probablility a sonar glitch would happen</source>
      <translation>发生声纳故障的可能性</translation>
    </message>
    <message>
      <source>Process noise standard deviation for the airspeed rate</source>
      <translation>空速率的过程噪声标准偏差</translation>
    </message>
    <message>
      <source>ProfiLED2</source>
      <translation>轮廓LED2</translation>
    </message>
    <message>
      <source>Propeller momentum drag coefficient for multi-rotor wind estimation</source>
      <translation>用于多旋翼风力估计的螺旋桨动量阻力系数</translation>
    </message>
    <message>
      <source>Proportional gain for position error</source>
      <translation>位置误差比例增益</translation>
    </message>
    <message>
      <source>Protocol for Main GPS</source>
      <translation>主 GPS 协议</translation>
    </message>
    <message>
      <source>Proximity minimum range</source>
      <translation>接近最小范围</translation>
    </message>
    <message>
      <source>Proximity sensor ignore width 1</source>
      <translation>接近传感器忽略宽度 1</translation>
    </message>
    <message>
      <source>Proximity sensor yaw correction</source>
      <translation>接近传感器偏航校正</translation>
    </message>
    <message>
      <source>Pullup target pitch</source>
      <translation>上拉目标俯仰</translation>
    </message>
    <message>
      <source>Put CH11 to Duty-Cycle mode</source>
      <translation>将 CH11 置于占空比模式</translation>
    </message>
    <message>
      <source>Put CH4 to Duty-Cycle mode</source>
      <translation>将 CH4 置于占空比模式</translation>
    </message>
    <message>
      <source>Q RTL pause time.</source>
      <translation>Q RTL 暂停时间。</translation>
    </message>
    <message>
      <source>QACRO mode roll rate</source>
      <translation>QACRO模式横滚率</translation>
    </message>
    <message>
      <source>QMC5883</source>
      <translation>QMC5883</translation>
    </message>
    <message>
      <source>QRTL return altitude</source>
      <translation>QRTL返回高度</translation>
    </message>
    <message>
      <source>Qloiter</source>
      <translation>克洛伊特</translation>
    </message>
    <message>
      <source>Quadcopter</source>
      <translation>四轴飞行器</translation>
    </message>
    <message>
      <source>Quadplane fixed wing landing approach radius</source>
      <translation>四翼固定翼着陆进近半径</translation>
    </message>
    <message>
      <source>Quicktune Yaw D max</source>
      <translation>Quicktune 偏航 D 最大</translation>
    </message>
    <message>
      <source>Quicktune gain margin</source>
      <translation>快速调谐增益裕度</translation>
    </message>
    <message>
      <source>RC</source>
      <translation>遥控信号</translation>
    </message>
    <message>
      <source>RC Channel stream rate</source>
      <translation>RC 通道码流速率</translation>
    </message>
    <message>
      <source>RC Feel</source>
      <translation>RC 手感</translation>
    </message>
    <message>
      <source>RC Out channels to be transmitted as servo over CAN bus</source>
      <translation>RC 输出通道作为舵机通过 CAN 总线传输</translation>
    </message>
    <message>
      <source>RC Yaw</source>
      <translation>RC 偏航</translation>
    </message>
    <message>
      <source>RC channel 10 trim</source>
      <translation>RC 通道 10 微调</translation>
    </message>
    <message>
      <source>RC channel 12 trim</source>
      <translation>RC 通道 12 微调</translation>
    </message>
    <message>
      <source>RC channel 14 trim</source>
      <translation>RC 通道 14 微调</translation>
    </message>
    <message>
      <source>RC channel 16 trim</source>
      <translation>RC 通道 16 微调</translation>
    </message>
    <message>
      <source>RC channel 18 trim</source>
      <translation>RC 通道 18 微调</translation>
    </message>
    <message>
      <source>RC channel 3 trim</source>
      <translation>RC 通道 3 微调</translation>
    </message>
    <message>
      <source>RC channel 5 trim</source>
      <translation>RC 通道 5 微调</translation>
    </message>
    <message>
      <source>RC channel 7 trim</source>
      <translation>RC 通道 7 微调</translation>
    </message>
    <message>
      <source>RC channel 9 trim</source>
      <translation>RC 通道 9 微调</translation>
    </message>
    <message>
      <source>RC feel for roll/pitch which controls vehicle response to user input with 0 being extremely soft and 100 being crisp</source>
      <translation>RC 横滚/俯仰感觉，控制车辆对用户输入的响应，0 极其柔软，100 清脆</translation>
    </message>
    <message>
      <source>RC lock state from previous mode</source>
      <translation>RC 锁定先前模式的状态</translation>
    </message>
    <message>
      <source>RC options</source>
      <translation>RC 选项</translation>
    </message>
    <message>
      <source>RC reverse</source>
      <translation>RC反向</translation>
    </message>
    <message>
      <source>RC5</source>
      <translation>RC5</translation>
    </message>
    <message>
      <source>RCIN1/Pitch</source>
      <translation>RCIN1/节距</translation>
    </message>
    <message>
      <source>RCIN13Scaled</source>
      <translation>RCIN13缩放</translation>
    </message>
    <message>
      <source>RCIN2</source>
      <translation>RCIN2</translation>
    </message>
    <message>
      <source>RCIN4Scaled</source>
      <translation>RCIN4缩放</translation>
    </message>
    <message>
      <source>RCIN7/CameraPan</source>
      <translation>RCIN7/云台</translation>
    </message>
    <message>
      <source>RCPassThru</source>
      <translation>RC直通</translation>
    </message>
    <message>
      <source>RC_PWR_EN</source>
      <translation>RC_PWR_EN</translation>
    </message>
    <message>
      <source>RCn_OPTION number to used to trigger parameter reversion</source>
      <translation>RCn_OPTION 用于触发参数恢复的编号</translation>
    </message>
    <message>
      <source>RF capabilities</source>
      <translation>RF 功能</translation>
    </message>
    <message>
      <source>ROLL_X</source>
      <translation>ROLL_X</translation>
    </message>
    <message>
      <source>ROTATION_NONE</source>
      <translation>ROTATION_NONE</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_180</source>
      <translation>ROTATION_ROLL_180</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_180_YAW_90</source>
      <translation>ROTATION_ROLL_180_YAW_90</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_90</source>
      <translation>ROTATION_ROLL_90</translation>
    </message>
    <message>
      <source>ROTATION_ROLL_90_YAW_270</source>
      <translation>ROTATION_ROLL_90_YAW_270</translation>
    </message>
    <message>
      <source>ROTATION_YAW_315</source>
      <translation>ROTATION_YAW_315</translation>
    </message>
    <message>
      <source>RPM instance channel to use</source>
      <translation>RPM实例通道使用</translation>
    </message>
    <message>
      <source>RPM2</source>
      <translation>RPM2</translation>
    </message>
    <message>
      <source>RSSI pin's highest voltage</source>
      <translation>RSSI引脚最高电压</translation>
    </message>
    <message>
      <source>RSSI warn level (in %)</source>
      <translation>RSSI 警告级别（%）</translation>
    </message>
    <message>
      <source>RTCM code differential</source>
      <translation>RTCM代码差分</translation>
    </message>
    <message>
      <source>RTL altitude</source>
      <translation>RTL海拔高度</translation>
    </message>
    <message>
      <source>RTL minimum climb</source>
      <translation>RTL 最小爬升</translation>
    </message>
    <message>
      <source>RTL speed</source>
      <translation>RTL速度</translation>
    </message>
    <message>
      <source>RTL_ALTITUDE should be a valid value</source>
      <translation>RTL_ALTITUDE 应该是一个有效值</translation>
    </message>
    <message>
      <source>RX rate timeout configuration</source>
      <translation>RX速率超时配置</translation>
    </message>
    <message>
      <source>Radius of the GPS horizontal position error in meters</source>
      <translation>GPS 水平位置误差的半径（以米为单位）</translation>
    </message>
    <message>
      <source>Rallypoint</source>
      <translation>集结点</translation>
    </message>
    <message>
      <source>Range finder can be used as the primary height source when below this percentage of its maximum range (see RNGFND_MAX_CM). Set to -1 when EK3_ALT_SOURCE is not set to range finder.  This is not for terrain following.</source>
      <translation>当测距仪低于其最大范围的这个百分比时，可以用作主要高度源（参见 RNGFND_MAX_CM）。当 EK3_ALT_SOURCE 未设置为测距仪时，设置为 -1。  这不适用于地形跟踪。</translation>
    </message>
    <message>
      <source>Range finder range dependent noise scaler</source>
      <translation>测距仪测距相关噪声缩放器</translation>
    </message>
    <message>
      <source>Range of yaw channel output in degrees (only in AUX output mode)</source>
      <translation>偏航通道输出范围，单位为度（仅在 AUX 输出模式下）</translation>
    </message>
    <message>
      <source>RangeFinder Gain</source>
      <translation>测距仪增益</translation>
    </message>
    <message>
      <source>Rangefinder filter to smooth distance.  Set to zero to disable filtering</source>
      <translation>测距仪滤镜可平滑距离。  设置为零以禁用过滤</translation>
    </message>
    <message>
      <source>Rangefinder scaling</source>
      <translation>测距仪缩放</translation>
    </message>
    <message>
      <source>Rate Pitch</source>
      <translation>利率推介</translation>
    </message>
    <message>
      <source>Rate Roll kI</source>
      <translation>横滚率 kI</translation>
    </message>
    <message>
      <source>Rate Yaw kD</source>
      <translation>速率偏航 kD</translation>
    </message>
    <message>
      <source>Rate of change of pitch in degrees/second for on/off servos</source>
      <translation>开/关舵机系统的俯仰变化率（以度/秒为单位）</translation>
    </message>
    <message>
      <source>RatePitchPI</source>
      <translation>速率音调PI</translation>
    </message>
    <message>
      <source>RateYaw</source>
      <translation>偏航率</translation>
    </message>
    <message>
      <source>Ratio between measured response and CRUISE_THROTTLE value. Raise this to get a higher CRUISE_THROTTLE value</source>
      <translation>测量响应与 CRUISE_THROTTLE 值之间的比率。提高此值以获得更高的 CRUISE_THROTTLE 值</translation>
    </message>
    <message>
      <source>Ratio of mass to drag coefficient measured along the X body axis. This parameter enables estimation of wind drift for vehicles with bluff bodies and without propulsion forces in the X and Y direction (eg multicopters). The drag produced by this effect scales with speed squared.  Set to a postive value &gt; 1.0 to enable. A starting value is the mass in Kg divided by the frontal area. The predicted drag from the rotors is specified separately by the EK3_DRAG_MCOEF parameter.</source>
      <translation>沿 X 体轴测量的质量与阻力系数之比。该参数能够估计具有钝体且在 X 和 Y 方向上没有推进力的车辆（例如多旋翼飞行器）的风漂移。这种效应产生的阻力与速度的平方成正比。  设置为正值 &gt; 1.0 即可启用。起始值是质量（公斤）除以额面积。转子的预测阻力由 EK3_DRAG_MCOEF 参数单独指定。</translation>
    </message>
    <message>
      <source>Raw Control stream rate</source>
      <translation>原始控制码流速率</translation>
    </message>
    <message>
      <source>Raw logging options</source>
      <translation>原始日志记录选项</translation>
    </message>
    <message>
      <source>Read and write accelerometer data to/from files</source>
      <translation>从文件读取和写入加速度计数据</translation>
    </message>
    <message>
      <source>Receiver RSSI channel number</source>
      <translation>接收器 RSSI 通道号</translation>
    </message>
    <message>
      <source>Recovery behaviour after a fail event</source>
      <translation>失败事件后的恢复行为</translation>
    </message>
    <message>
      <source>Reduction in gain after oscillation detected. Raise this number to get a more conservative tune</source>
      <translation>检测到振荡后增益降低。提高此数字以获得更保守的调音</translation>
    </message>
    <message>
      <source>Relative to lead vehicle heading</source>
      <translation>相对于引导车辆航向</translation>
    </message>
    <message>
      <source>Relay invert output signal</source>
      <translation>继电器反相输出信号</translation>
    </message>
    <message>
      <source>Relay6</source>
      <translation>继电器6</translation>
    </message>
    <message>
      <source>Remove the PTCH_TRIM_DEG on the GCS horizon</source>
      <translation>移除 GCS 地平线上的 PTCH_TRIM_DEG</translation>
    </message>
    <message>
      <source>Report send time</source>
      <translation>报告发送时间</translation>
    </message>
    <message>
      <source>Require Location</source>
      <translation>需要位置</translation>
    </message>
    <message>
      <source>Require valid mission to arm</source>
      <translation>需要有效的任务来解锁</translation>
    </message>
    <message>
      <source>Required arming remaining capacity</source>
      <translation>所需解锁剩余容量</translation>
    </message>
    <message>
      <source>Required satellite count to use GPS</source>
      <translation>使用 GPS 所需的卫星数量</translation>
    </message>
    <message>
      <source>Required velocity xy standard deviation to publish position</source>
      <translation>发布位置所需的速度 xy 标准差</translation>
    </message>
    <message>
      <source>Reset Mission Channel</source>
      <translation>重置任务频道</translation>
    </message>
    <message>
      <source>Responsiveness to target movement in Target Estimator</source>
      <translation>目标估计器中对目标移动的响应能力</translation>
    </message>
    <message>
      <source>Retract Mount2</source>
      <translation>缩回安装座2</translation>
    </message>
    <message>
      <source>Return directly to safe landing point (do not consider mission landing and Home)</source>
      <translation>直接返回安全着陆点（不考虑任务着陆和返航）</translation>
    </message>
    <message>
      <source>Return to closest safe point (home or rally point) via direct path.</source>
      <translation>通过直接路径返回最近的安全点（家乡或集结点）。</translation>
    </message>
    <message>
      <source>Return-to-Launch speed default.  If zero use WP_SPEED or CRUISE_SPEED.</source>
      <translation>返回默认启动速度。  如果为零，则使用 WP_SPEED 或 CRUISE_SPEED。</translation>
    </message>
    <message>
      <source>Reverse Output Range for SIM_GZ</source>
      <translation>SIM_GZ 的反向输出范围</translation>
    </message>
    <message>
      <source>Reverse channel input. Set to 0 for normal operation. Set to 1 to reverse this input channel.</source>
      <translation>反向通道输入。设置为 0 即可正常运行。设置为 1 可反转该输入通道。</translation>
    </message>
    <message>
      <source>Reverse the raw measurements of all differential pressure sensors. This can be enabled if the sensors have static and dynamic ports swapped.</source>
      <translation>反转所有差压传感器的原始测量值。如果传感器交换了静态和动态端口，则可以启用此功能。</translation>
    </message>
    <message>
      <source>Right Aileron</source>
      <translation>右副翼</translation>
    </message>
    <message>
      <source>Right4m</source>
      <translation>右4m</translation>
    </message>
    <message>
      <source>Roboclaw Driver Channel 1 Failsafe Value</source>
      <translation>Roboclaw 驱动程序通道 1 故障安全值</translation>
    </message>
    <message>
      <source>Roboclaw Driver Channel 2 Minimum Value</source>
      <translation>Roboclaw 驱动程序通道 2 最小值</translation>
    </message>
    <message>
      <source>Rockblock booted feedback</source>
      <translation>Rockblock 启动反馈</translation>
    </message>
    <message>
      <source>Roll 180°, Yaw 270°</source>
      <translation>横滚 180°、偏航 270°</translation>
    </message>
    <message>
      <source>Roll 270°, Yaw 135°</source>
      <translation>横滚 270°、偏航 135°</translation>
    </message>
    <message>
      <source>Roll 90°, Pitch 68°, Yaw 293°</source>
      <translation>横滚 90°、俯仰 68°、偏航 293°</translation>
    </message>
    <message>
      <source>Roll Error notch filter index, zero disables</source>
      <translation>横滚误差陷波滤波器索引，零禁用</translation>
    </message>
    <message>
      <source>Roll and Yaw</source>
      <translation>横滚和偏航</translation>
    </message>
    <message>
      <source>Roll axis rate controller D gain</source>
      <translation>横滚轴速率控制器 D 增益</translation>
    </message>
    <message>
      <source>Roll axis rate controller P gain</source>
      <translation>横滚轴速率控制器 P 增益</translation>
    </message>
    <message>
      <source>Roll axis rate controller feed forward</source>
      <translation>横滚轴速率控制器前馈</translation>
    </message>
    <message>
      <source>Roll control time constant</source>
      <translation>横滚控制时间常数</translation>
    </message>
    <message>
      <source>Roll gain multiplier active when landed. A factor of 1.0 means no reduction in gain while landed. Reduce this factor to reduce ground oscitation in the roll axis.</source>
      <translation>着陆时横滚增益倍增器处于活动状态。 1.0 系数意味着着陆时增益不会减少。减小该系数可减少横滚轴上的地面振荡。</translation>
    </message>
    <message>
      <source>Roll rate I gain</source>
      <translation>我获得的横滚率</translation>
    </message>
    <message>
      <source>Roll rate integrator gain</source>
      <translation>横滚率积分器增益</translation>
    </message>
    <message>
      <source>Roll trim</source>
      <translation>横滚微调</translation>
    </message>
    <message>
      <source>Roll180Pitch270</source>
      <translation>滚动180俯仰270</translation>
    </message>
    <message>
      <source>Roll270</source>
      <translation>滚动270</translation>
    </message>
    <message>
      <source>Roll315</source>
      <translation>滚动315</translation>
    </message>
    <message>
      <source>Roll90Yaw135</source>
      <translation>横滚90偏航135</translation>
    </message>
    <message>
      <source>Rotation from flight controller board to vehicle body frame.
This parameter gets set during the "level horizon" calibration or can be
set manually.</source>
      <translation>从飞控板到机体框架的旋转。该参数在“水平水平”校准期间设置，也可以手动设置。</translation>
    </message>
    <message>
      <source>Rotational acceleration of twist x axis</source>
      <translation>扭转x轴旋转加速度</translation>
    </message>
    <message>
      <source>Rotor 2 tilt assignment</source>
      <translation>转子 2 倾斜分配</translation>
    </message>
    <message>
      <source>Rotor Governor Setpoint</source>
      <translation>转子调速器设定值</translation>
    </message>
    <message>
      <source>Rover (Differential)</source>
      <translation>流浪者（差速器）</translation>
    </message>
    <message>
      <source>Rover Quicktune Steering FF to I ratio</source>
      <translation>流动站 Quicktune 转向系统 FF 与 I 的比率</translation>
    </message>
    <message>
      <source>Rover with Static Base on UART2 (similar to Default, except coming in on UART2)</source>
      <translation>UART2 上带有静态底座的流动站（与默认类似，但在 UART2 上出现）</translation>
    </message>
    <message>
      <source>RunCam</source>
      <translation>跑摄</translation>
    </message>
    <message>
      <source>RunCam boot delay before allowing updates</source>
      <translation>允许更新之前 RunCam 启动延迟</translation>
    </message>
    <message>
      <source>Runcam 2 4k</source>
      <translation>润凯2 4k</translation>
    </message>
    <message>
      <source>Rx UAT only</source>
      <translation>仅接收 UAT</translation>
    </message>
    <message>
      <source>SAVE_X</source>
      <translation>SAVE_X</translation>
    </message>
    <message>
      <source>SBP</source>
      <translation>SBP</translation>
    </message>
    <message>
      <source>SCR_SDEV1</source>
      <translation>SCR_SDEV1</translation>
    </message>
    <message>
      <source>SERIAL1</source>
      <translation>SERIAL1</translation>
    </message>
    <message>
      <source>SERIAL9</source>
      <translation>SERIAL9</translation>
    </message>
    <message>
      <source>SERVO5</source>
      <translation>SERVO5</translation>
    </message>
    <message>
      <source>SERVOn_TRIM</source>
      <translation>SERVOn_TRIM</translation>
    </message>
    <message>
      <source>SF11/c</source>
      <translation>SF11/c</translation>
    </message>
    <message>
      <source>SIDEBARS_Y</source>
      <translation>SIDEBARS_Y</translation>
    </message>
    <message>
      <source>SIM Channel 12</source>
      <translation>SIM 通道 12</translation>
    </message>
    <message>
      <source>SIM Channel 16</source>
      <translation>SIM 通道 16</translation>
    </message>
    <message>
      <source>SIM Channel 5</source>
      <translation>SIM 通道 5</translation>
    </message>
    <message>
      <source>SIM Channel 9</source>
      <translation>SIM 通道 9</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 1 Maximum Value</source>
      <translation>SIM_GZ ESC 1 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 2 Output Function</source>
      <translation>SIM_GZ ESC 2 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 4 Disarmed Value</source>
      <translation>SIM_GZ ESC 4 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 5 Maximum Value</source>
      <translation>SIM_GZ ESC 5 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 6 Output Function</source>
      <translation>SIM_GZ ESC 6 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ ESC 8 Disarmed Value</source>
      <translation>SIM_GZ ESC 8 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 1 Maximum Value</source>
      <translation>SIM_GZ 舵机 1 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 2 Output Function</source>
      <translation>SIM_GZ 舵机 2 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 4 Disarmed Value</source>
      <translation>SIM_GZ 舵机 4 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 5 Maximum Value</source>
      <translation>SIM_GZ 舵机 5 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 6 Output Function</source>
      <translation>SIM_GZ 舵机 6 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ Servo 8 Disarmed Value</source>
      <translation>SIM_GZ 舵机 8 上锁值</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 1 Maximum Value</source>
      <translation>SIM_GZ 轮子 1 最大值</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 2 Output Function</source>
      <translation>SIM_GZ 轮子 2 输出功能</translation>
    </message>
    <message>
      <source>SIM_GZ Wheels 4 Disarmed Value</source>
      <translation>SIM_GZ 轮子 4 解除值</translation>
    </message>
    <message>
      <source>SITL GPIO emulation</source>
      <translation>SITL GPIO 仿真</translation>
    </message>
    <message>
      <source>SITL set this simulated pin to true if vehicle is on ground</source>
      <translation>如果车辆在地面上，SITL 将此模拟引脚设置为真</translation>
    </message>
    <message>
      <source>SITL vicon position glitch North</source>
      <translation>SITL 维康位置故障北</translation>
    </message>
    <message>
      <source>SITL vicon velocity glitch East</source>
      <translation>SITL 维康速度毛刺东</translation>
    </message>
    <message>
      <source>SLCAN Route</source>
      <translation>SLCAN路线</translation>
    </message>
    <message>
      <source>SMBUS-SUI6</source>
      <translation>SMBUS-SUI6</translation>
    </message>
    <message>
      <source>SPL06</source>
      <translation>SPL06</translation>
    </message>
    <message>
      <source>STABILIZE Mode</source>
      <translation>STABILIZE模式</translation>
    </message>
    <message>
      <source>SToRM32 MAVLink</source>
      <translation>SToRM32 MAVLink</translation>
    </message>
    <message>
      <source>Safety factor that is used to scale the actual RTL time estimate. Time with margin = RTL_TIME_FACTOR * time + RTL_TIME_MARGIN</source>
      <translation>用于缩放实际 RTL 时间估计的安全系数。有余量的时间 = RTL_TIME_FACTOR * 时间 + RTL_TIME_MARGIN</translation>
    </message>
    <message>
      <source>Sagetech MXS use External Config</source>
      <translation>Sagetech MXS使用外部配置</translation>
    </message>
    <message>
      <source>Sail Heel Target notch filter index, zero disables</source>
      <translation>帆跟目标陷波滤波器指数，零禁用</translation>
    </message>
    <message>
      <source>Sail Heel control P gain for sailboats.  Converts the error between the desired heel angle (in radians) and actual heel to a main sail output (in the range -1 to +1)</source>
      <translation>帆跟控制帆船的 P 增益。  将所需横倾角（以弧度为单位）与实际横倾角之间的误差转换为主帆输出（范围为 -1 至 +1）</translation>
    </message>
    <message>
      <source>Sail max angle</source>
      <translation>帆最大角度</translation>
    </message>
    <message>
      <source>Sailboat simulation sail type</source>
      <translation>帆船仿真帆型</translation>
    </message>
    <message>
      <source>Same as previous when landed, in-air require landing only if no valid VTOL approach is present</source>
      <translation>与之前着陆时相同，仅当不存在有效的 VTOL 进近时才需要在空中着陆</translation>
    </message>
    <message>
      <source>Save CRC of current scripts to loaded and running checksum parameters enabling pre-arm</source>
      <translation>将当前脚本的 CRC 保存到加载并运行的校验和参数中，以启用预准备</translation>
    </message>
    <message>
      <source>SaveWaypoint</source>
      <translation>保存航点</translation>
    </message>
    <message>
      <source>Scale factor applied to the desired pitch actuator command in full manual mode. This parameter allows
to adjust the throws of the control surfaces.</source>
      <translation>在全手动模式下应用于所需桨距执行器命令的比例因子。该参数允许调整控制面的投掷。</translation>
    </message>
    <message>
      <source>Scale factor for sensor measurements in sensor y axis</source>
      <translation>传感器 y 轴传感器测量的比例因子</translation>
    </message>
    <message>
      <source>Scale the compass 2 to simulate sensor scale factor errors</source>
      <translation>缩放罗盘 2 以模拟传感器比例因子误差</translation>
    </message>
    <message>
      <source>Scaling factor between sensor reading and RPM.</source>
      <translation>传感器读数和 RPM 之间的比例因子。</translation>
    </message>
    <message>
      <source>Scenario for thermalling simulation, for soaring</source>
      <translation>热力模拟场景，用于翱翔</translation>
    </message>
    <message>
      <source>Screen to be shown on failsafe event. Zero to disable the feature.</source>
      <translation>发生故障安全事件时显示的屏幕。为零则禁用该功能。</translation>
    </message>
    <message>
      <source>Script 16</source>
      <translation>脚本16</translation>
    </message>
    <message>
      <source>Script 9</source>
      <translation>脚本9</translation>
    </message>
    <message>
      <source>Script16</source>
      <translation>脚本16</translation>
    </message>
    <message>
      <source>Script9</source>
      <translation>脚本9</translation>
    </message>
    <message>
      <source>Scripting User Parameter1</source>
      <translation>编写用户参数1脚本</translation>
    </message>
    <message>
      <source>Scripting serial port number</source>
      <translation>脚本串口号</translation>
    </message>
    <message>
      <source>Scripting15</source>
      <translation>脚本15</translation>
    </message>
    <message>
      <source>Scripting8</source>
      <translation>脚本8</translation>
    </message>
    <message>
      <source>Second 4 characters of CALLSIGN</source>
      <translation>CALLSIGN 的第二个 4 字符</translation>
    </message>
    <message>
      <source>Second Gyro bias (Z-axis)</source>
      <translation>第二陀螺仪偏置（Z 轴）</translation>
    </message>
    <message>
      <source>Second RPM Sensor</source>
      <translation>第二个 RPM 传感器</translation>
    </message>
    <message>
      <source>Second WheelEncoder type</source>
      <translation>第二轮编码器类型</translation>
    </message>
    <message>
      <source>Second octet of the server IP address to connect to</source>
      <translation>要连接的服务器 IP 地址的第二个八位字节</translation>
    </message>
    <message>
      <source>Secondary protocol with 11 bit CAN addressing</source>
      <translation>具有 11 位 CAN 寻址的辅助协议</translation>
    </message>
    <message>
      <source>See description for each bitmask bit description</source>
      <translation>请参阅每个位掩码位描述的描述</translation>
    </message>
    <message>
      <source>Select the RGB LED brightness level. When USB is connected brightness will never be higher than low regardless of the setting.</source>
      <translation>选择 RGB LED 亮度级别。当连接 USB 时，无论设置如何，亮度都不会高于低值。</translation>
    </message>
    <message>
      <source>Select what should be output on MAIN 1. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 MAIN 1 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on MAIN 5. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 MAIN 5 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 1. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 1 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 13. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 13 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 2. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PCA9685 Output Channel 6. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PCA9685 输出通道 6 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Aux 1. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PWM Aux 1 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Aux 5. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PWM Aux 5 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on PWM Capture 1. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 PWM 捕获 1 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Roboclaw Driver Channel 2. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Roboclaw 驱动程序通道 2 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 12. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 12 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 16. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 16 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 5. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 5 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM Channel 9. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM 通道 9 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ ESC 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ ESC 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ ESC 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ ESC 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Servo 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 舵机 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Servo 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 舵机 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on SIM_GZ Wheels 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 SIM_GZ 轮子 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on TAP ESC Output ESC 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 TAP ESC 输出 ESC 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on TAP ESC Output ESC 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 TAP ESC 输出 ESC 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN ESC 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN ESC 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN ESC 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN ESC 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN Servo 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN 舵机 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCAN Servo 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCAN 舵机 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 12. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 12 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 16. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 16 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 5. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 5 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on UAVCANv1 ESC 9. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 UAVCANv1 ESC 9 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL ESC Output ESC 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 VOXL ESC 输出 ESC 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL2 IO Output PWM Channel 4. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 VOXL2 IO 输出 PWM 通道 4 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on VOXL2 IO Output PWM Channel 8. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 VOXL2 IO 输出 PWM 通道 8 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 11. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 11 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 15. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 15 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 5. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 5 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select what should be output on Vertiq IO CVI 9. The default failsafe value is set according to the selected function: - 'Min' for ConstantMin - 'Max' for ConstantMax - 'Max' for Parachute - ('Max'+'Min')/2 for Servos - 'Disarmed' for the rest</source>
      <translation>选择 Vertiq IO CVI 9 上应输出的内容。默认故障安全值根据所选功能设置： - ConstantMin 为“Min” - ConstantMax 为“Max” - 降落伞为“Max” - 舵机为 ('Max'+'Min')/2 - 其余为“Disarmed”</translation>
    </message>
    <message>
      <source>Select which Output Protocol to use for outputs PWM Aux 1-4. Custom PWM rates can be used by directly setting any value &gt;0.</source>
      <translation>选择用于输出 PWM Aux 1-4 的输出协议。可以通过直接设置&gt;0的任何值来使用自定义PWM速率。</translation>
    </message>
    <message>
      <source>Select which function if performed when CH11 is above 1800 pwm</source>
      <translation>选择当 CH11 高于 1800 pwm 时执行的功能</translation>
    </message>
    <message>
      <source>Select which function is performed when CH7 is above 1800 pwm</source>
      <translation>选择当 CH7 高于 1800 pwm 时执行哪个功能</translation>
    </message>
    <message>
      <source>Selector angular threshold</source>
      <translation>选择器角度阈值</translation>
    </message>
    <message>
      <source>Selects the Looprate of Test methods</source>
      <translation>选择测试方法的循环率</translation>
    </message>
    <message>
      <source>Selects what type of UART ESC, if any, is being used.</source>
      <translation>选择正在使用的 UART ESC 类型（如果有）。</translation>
    </message>
    <message>
      <source>Send only HIGH importance items</source>
      <translation>只发送HIGH重要项目</translation>
    </message>
    <message>
      <source>Sensirion SDP3X differential pressure sensor (external I2C)</source>
      <translation>Sensirion SDP3X 差压传感器（外部 I2C）</translation>
    </message>
    <message>
      <source>Sensor Mode</source>
      <translation>传感器模式</translation>
    </message>
    <message>
      <source>Sensor mode publishes raw IMU data to be used by EKF2. INS data from the device is published to the external INS topics.
INS mode publishes the INS data to the vehicle topics to be used for navigation.</source>
      <translation>传感器模式发布原始 IMU 数据以供 EKF2 使用。来自设备的 INS 数据发布到外部 INS 主题。 INS模式将INS数据发布到车辆主题以用于导航。</translation>
    </message>
    <message>
      <source>Sensors Only (default)</source>
      <translation>仅传感器（默认）</translation>
    </message>
    <message>
      <source>Serial 2 flow control</source>
      <translation>串行2流量控制</translation>
    </message>
    <message>
      <source>Serial 6 Baud Rate</source>
      <translation>串口 6 波特率</translation>
    </message>
    <message>
      <source>Serial CAN remote node number</source>
      <translation>串行 CAN 远程节点号</translation>
    </message>
    <message>
      <source>Serial Configuration for GHST RC Input Driver</source>
      <translation>GHST RC 输入驱动器的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for Lightware SF45 Rangefinder (serial)</source>
      <translation>Lightware SF45 测距仪的串行配置（串行）</translation>
    </message>
    <message>
      <source>Serial Configuration for Roboclaw Driver</source>
      <translation>Roboclaw 驱动程序的串行配置</translation>
    </message>
    <message>
      <source>Serial Configuration for VectorNav (VN-100, VN-200, VN-300)</source>
      <translation>VectorNav 的串行配置（VN-100、VN-200、VN-300）</translation>
    </message>
    <message>
      <source>Serial baud rate on remote CAN node</source>
      <translation>远程 CAN 节点上的串行波特率</translation>
    </message>
    <message>
      <source>Serial protocol of DroneCAN serial port</source>
      <translation>DroneCAN串口协议</translation>
    </message>
    <message>
      <source>Serial3</source>
      <translation>串口3</translation>
    </message>
    <message>
      <source>Serial6</source>
      <translation>串口6</translation>
    </message>
    <message>
      <source>Serial9 protocol selection</source>
      <translation>Serial9协议选择</translation>
    </message>
    <message>
      <source>ServiceSurface</source>
      <translation>服务界面</translation>
    </message>
    <message>
      <source>Servo 11</source>
      <translation>舵机11</translation>
    </message>
    <message>
      <source>Servo 19</source>
      <translation>舵机19</translation>
    </message>
    <message>
      <source>Servo 23</source>
      <translation>舵机23</translation>
    </message>
    <message>
      <source>Servo 3 Angle at Maximum</source>
      <translation>舵机 3 最大角度</translation>
    </message>
    <message>
      <source>Servo 4 Angle at Minimum</source>
      <translation>舵机 4 最小角度</translation>
    </message>
    <message>
      <source>Servo 6 Angle at Minimum</source>
      <translation>舵机 6 最小角度</translation>
    </message>
    <message>
      <source>Servo 8 Angle at Minimum</source>
      <translation>舵机 8 最小角度</translation>
    </message>
    <message>
      <source>Servo PCB</source>
      <translation>舵机PCB</translation>
    </message>
    <message>
      <source>Servo command output rate</source>
      <translation>舵机指令输出率</translation>
    </message>
    <message>
      <source>Servo output rate</source>
      <translation>舵机输出率</translation>
    </message>
    <message>
      <source>Set 0 if sensor is to be used as a 1-D rangefinder (minimum of all distances will be sent, typically used for height detection). Set 1 if it should be used as a 3-D proximity device (Eg. Obstacle Avoidance)</source>
      <translation>设为 0 时，将传感器作为一维测距仪使用：发送所有距离中的最小值，通常用于高度检测。设为 1 时，将其作为三维近距传感器使用，例如用于避障。</translation>
    </message>
    <message>
      <source>Set Predefined Velocity Setpoint</source>
      <translation>设置预定义速度设定点</translation>
    </message>
    <message>
      <source>Set bits in the following positions to enable: 0 : Horizontal position fusion 1 : Vertical position fusion 2 : 3D velocity fusion 3 : Yaw</source>
      <translation>设置以下位置中的位以启用： 0：水平位置融合 1：垂直位置融合 2：3D 速度融合 3：偏航</translation>
    </message>
    <message>
      <source>Set level at which AVGCELLV item will flash</source>
      <translation>设置 AVGCELLV 项目闪烁的级别</translation>
    </message>
    <message>
      <source>Set level below which TER_HGT item will flash. -1 disables.</source>
      <translation>设置低于该值 TER_HGT 项目将闪烁的级别。 -1 禁用。</translation>
    </message>
    <message>
      <source>Set telemetry transmit power. This is the power level (from 1 to 8) for telemetry packets sent from the RX to the TX</source>
      <translation>设置遥测发射功率。这是从 RX 发送到 TX 的遥测数据包的功率级别（从 1 到 8）</translation>
    </message>
    <message>
      <source>Set this parameter according to your vehicle/motor configuration</source>
      <translation>根据您的车辆/电机配置设置此参数</translation>
    </message>
    <message>
      <source>Set to -1 to disable.
For mecanum rovers this limit is used for longitudinal and lateral acceleration.</source>
      <translation>设置为 -1 以禁用。对于麦克纳姆漫游车，此限制​​用于纵向和横向加速度。</translation>
    </message>
    <message>
      <source>Set to -1 to reverse channel.</source>
      <translation>设置为 -1 为反向通道。</translation>
    </message>
    <message>
      <source>Set to 1 to reset parameters on next system startup (setting defaults). Platform-specific values are used if available. RC* parameters are preserved.</source>
      <translation>设置为 1 以在下次系统启动时重置参数（设置默认值）。如果可用，则使用特定于平台的值。 RC* 参数被保留。</translation>
    </message>
    <message>
      <source>Set to zero to enable automatic compensation from measurement timestamps</source>
      <translation>设置为零以启用测量时间戳的自动补偿</translation>
    </message>
    <message>
      <source>Set_AccelZ</source>
      <translation>设置_AccelZ</translation>
    </message>
    <message>
      <source>Set_RateYawDP</source>
      <translation>Set_RateYawDP</translation>
    </message>
    <message>
      <source>Sets a fraction of full flaps during take-off. Also applies to flaperons if enabled in the mixer/allocation.</source>
      <translation>在起飞期间设置全襟翼的一小部分。如果在混合器/分配中启用，也适用于襟副翼。</translation>
    </message>
    <message>
      <source>Sets second 4 characters of a total of 8. Valid characters are A-Z, 0-9, " " only. Example "TEST" -&gt; 1413829460
For CALLSIGN shorter than 8 characters use the null terminator at the end '\0'.</source>
      <translation>设置总共 8 中的第二个 4 字符。有效字符仅限 A-Z、0-9、“ ”。示例 "TEST" -&gt; 1413829460 对于短于 8 字符的 CALLSIGN，请在末尾使用空终止符 '\0'。</translation>
    </message>
    <message>
      <source>Sets the analog input pin that should be used for voltage monitoring.</source>
      <translation>设置用于电压监控的模拟输入引脚。</translation>
    </message>
    <message>
      <source>Sets the maximum number of times the EKF2 will be allowed to reset it's yaw to the estimate from the EKF-GSF yaw estimator. No resets will be allowed unless the use of the EKF-GSF yaw estimate is enabled via the EK2_GSF_USE parameter.</source>
      <translation>设置允许 EKF2 将其偏航重置为 EKF-GSF 偏航估计器估计值的最大次数。除非通过 EK2_GSF_USE 参数启用 EKF-GSF 偏航估计，否则不允许重置。</translation>
    </message>
    <message>
      <source>Sets the number of motor poles to calculate the correct RPM value</source>
      <translation>设置电机极数以计算正确的 RPM 值</translation>
    </message>
    <message>
      <source>Sets the repetition frequency of the ranging operation in Hertz. Upon entering the desired frequency the system will calculate the nearest frequency that it can handle according to the resolution of internal timers.</source>
      <translation>设置测距操作的重复频率（以赫兹为单位）。输入所需频率后，系统将根据内部定时器的分辨率计算它可以处理的最接近的频率。</translation>
    </message>
    <message>
      <source>Sets the throttle output (HeliRSC servo) in percent for the throttle curve at 25% of full collective travel where he 0 percent collective is defined by H_COL_MIN and 100 percent collective is defined by H_COL_MAX.  Example: if the setup has -2 degree to +10 degree collective pitch setup, the total range is 12 degrees. 25% of 12 degrees is 3 degrees, so this setting would correspond to +1 degree of pitch.</source>
      <translation>设置全总距行程 25% 油门曲线的油门输出（HeliRSC 舵机）百分比，其中 0 总距百分比由 H_COL_MIN 定义，100 总距百分比由 H_COL_MAX 定义。  示例：如果设置有 -2 度到 +10 度总距桨距设置，则总范围为 12 度。 12 度的 25% 是 3 度，因此此设置将对应于 +1 桨距。</translation>
    </message>
    <message>
      <source>Sets the type of simulation used for GPS 2</source>
      <translation>设置用于 GPS 2 的模拟类型</translation>
    </message>
    <message>
      <source>Setting an RC channel's _OPTION to this value will use it for Plane Follow enable/disable</source>
      <translation>将 RC 通道的 _OPTION 设置为此值将使用它来启用/禁用平面跟随</translation>
    </message>
    <message>
      <source>Setting this parameter to 0 disables the filter</source>
      <translation>将此参数设置为 0 将禁用滤波器</translation>
    </message>
    <message>
      <source>Setting this parameter to 22027 will disable IO safety. WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK</source>
      <translation>将此参数设置为 22027 将禁用 IO 安全。 WARNING： ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK</translation>
    </message>
    <message>
      <source>Setting this to 0 ~ 13 will enable battery voltage sensing on pins A0 ~ A13. For the 3DR power brick on APM2.5 it should be set to 13. On the PX4 it should be set to 100. On the Pixhawk powered from the PM connector it should be set to 2.</source>
      <translation>设置为 0 ~ 13 将启用引脚 A0 ~ A13 上的电池电压检测。对于 APM2.5 上的 3DR 电源块，应设置为 13。在 PX4 上，应设置为 100。在由 PM 连接器供电的 Pixhawk 上，应将其设置为 2。</translation>
    </message>
    <message>
      <source>Settings this parameter to one triggers an automatic follow offset calculation based on current position of the vehicle and the landing target. NOTE: This parameter will auto-reset to zero once the offset has been calculated.</source>
      <translation>将此参数设置为 1 会触发基于飞行器当前位置和着陆目标的自动跟随偏移计算。 NOTE：一旦计算出偏移量，该参数将自动重置为零。</translation>
    </message>
    <message>
      <source>Shallow (45deg)</source>
      <translation>浅 (45deg)</translation>
    </message>
    <message>
      <source>Short failsafe action</source>
      <translation>短故障保护动作</translation>
    </message>
    <message>
      <source>SiRF</source>
      <translation>SiRF</translation>
    </message>
    <message>
      <source>Sideways distance in ZigZag auto</source>
      <translation>ZigZag 自动中的横向距离</translation>
    </message>
    <message>
      <source>Simple</source>
      <translation>简单</translation>
    </message>
    <message>
      <source>Simulate Aux Global Position (AGP)</source>
      <translation>模拟辅助全球位置 (AGP)</translation>
    </message>
    <message>
      <source>Simulated OSD number of text rows</source>
      <translation>模拟 OSD 文本行数</translation>
    </message>
    <message>
      <source>Simulated airspeed sensor ratio</source>
      <translation>模拟空速传感器比率</translation>
    </message>
    <message>
      <source>Simulated battery resting voltage</source>
      <translation>模拟电池静息电压</translation>
    </message>
    <message>
      <source>Simulated failure of ACCEL5</source>
      <translation>ACCEL5的模拟故障</translation>
    </message>
    <message>
      <source>Simulated temperature calibration min</source>
      <translation>模拟温度校准最小值</translation>
    </message>
    <message>
      <source>Simulates localized magnetic field distortions at ground level that decays with altitude. (Z-axis)</source>
      <translation>模拟地面上随高度衰减的局部磁场扭曲。 （Z轴）</translation>
    </message>
    <message>
      <source>Single channel flight mode selection</source>
      <translation>单通道飞行模式选择</translation>
    </message>
    <message>
      <source>SixthEKF</source>
      <translation>第六届EKF</translation>
    </message>
    <message>
      <source>Siyi ZT30 Zoom</source>
      <translation>四艺ZT30变焦</translation>
    </message>
    <message>
      <source>Skip Baro</source>
      <translation>跳过巴罗</translation>
    </message>
    <message>
      <source>Skip airspeed calibration on startup</source>
      <translation>启动时跳过空速校准</translation>
    </message>
    <message>
      <source>SkyPower EFI allow start disarmed</source>
      <translation>SkyPower EFI 允许启动上锁</translation>
    </message>
    <message>
      <source>SkyPower EFI minimum RPM. This is the RPM below which the engine is considered to be stopped</source>
      <translation>SkyPower EFI 最低 RPM。这是 RPM，低于该值发动机被视为停止</translation>
    </message>
    <message>
      <source>SkyPower EFI throttle function. This sets which SERVOn_FUNCTION to use for the target throttle. This should be 70 for fixed wing aircraft and 31 for helicopter rotor speed control</source>
      <translation>SkyPower EFI油门功能。这设置了将哪个 SERVOn_FUNCTION 用于目标限制。对于固定翼飞机应该是70，对于直升机旋翼速度控制应该是31</translation>
    </message>
    <message>
      <source>Slow</source>
      <translation>慢</translation>
    </message>
    <message>
      <source>Slung Payload drag coefficient.  Higher values increase drag and slow the payload more quickly</source>
      <translation>悬挂有效载荷阻力系数。  较高的值会增加阻力并更快地减慢有效负载</translation>
    </message>
    <message>
      <source>Slung Payload return to WP position P gain</source>
      <translation>悬挂的有效负载返回到 WP 位置 P 增益</translation>
    </message>
    <message>
      <source>SmartRTL accuracy. The minimum distance between points.</source>
      <translation>SmartRTL 精度。点之间的最小距离。</translation>
    </message>
    <message>
      <source>SmartRTL or RTL or Land</source>
      <translation>SmartRTL 或 RTL 或 Land</translation>
    </message>
    <message>
      <source>Some UBX modules have a FLASH that allows to store persistent configuration that will be loaded on start.
PX4 does override all configuration parameters it needs in RAM, which takes precedence over the values in FLASH.
However, configuration parameters that are not overriden by PX4 can still cause unexpected problems during flight.
To avoid these kind of problems a clean config can be reached by wiping the FLASH on boot.
Note: Currently only supported on UBX.</source>
      <translation>一些 UBX 模块具有 FLASH，允许存储将在启动时加载的持久配置。 PX4 确实会覆盖 RAM 中所需的所有配置参数，这优先于 FLASH 中的值。然而，未被PX4覆盖的配置参数仍然可能在飞行过程中引起意外问题。为了避免此类问题，可以通过在启动时擦除 FLASH 来实现干净的配置。注意：目前仅在 UBX 上支持。</translation>
    </message>
    <message>
      <source>Sonar noise factor</source>
      <translation>声纳噪声系数</translation>
    </message>
    <message>
      <source>Spacecraft 3D</source>
      <translation>宇宙飞船3D</translation>
    </message>
    <message>
      <source>Specific total energy rate first order filter time constant</source>
      <translation>比总能量率一阶滤波器时间常数</translation>
    </message>
    <message>
      <source>Specifies how the accel cal routine determines the trims</source>
      <translation>指定加速校准例程如何确定微调</translation>
    </message>
    <message>
      <source>Specifies the estimation method to be used</source>
      <translation>指定要使用的估计方法</translation>
    </message>
    <message>
      <source>Specifies vehicle's startup altitude (AMSL)</source>
      <translation>指定车辆的启动高度（AMSL）</translation>
    </message>
    <message>
      <source>Specify modes in which ground control station connection loss is ignored and no failsafe action is triggered. See also COM_RCL_EXCEPT.</source>
      <translation>指定忽略地面控制站连接丢失且不触发故障安全操作的模式。另请参见 COM_RCL_EXCEPT。</translation>
    </message>
    <message>
      <source>Speed &lt;--&gt; Altitude weight</source>
      <translation>速度 &lt;--&gt; 海拔高度 重量</translation>
    </message>
    <message>
      <source>Speed at which to rotate the yaw axis in scan mode</source>
      <translation>扫描模式下偏航轴旋转速度</translation>
    </message>
    <message>
      <source>Speed control Error notch filter index, zero disables</source>
      <translation>速度控制 误差陷波滤波器指数，零禁用</translation>
    </message>
    <message>
      <source>Speed control PD sum maximum</source>
      <translation>速度控制 PD 和最大</translation>
    </message>
    <message>
      <source>Speed control brake enable/disable</source>
      <translation>速度控制制动器启用/禁用</translation>
    </message>
    <message>
      <source>Speed control stop speed.  Motor outputs to zero once vehicle speed falls below this value</source>
      <translation>速度控制停止速度。  一旦车速低于该值，电机输出为零</translation>
    </message>
    <message>
      <source>Speed minimum at which we will begin spraying</source>
      <translation>我们开始喷涂的最低速度</translation>
    </message>
    <message>
      <source>Spoiler landing setting</source>
      <translation>扰流板着陆设置</translation>
    </message>
    <message>
      <source>Sprayer Sim enable/disable</source>
      <translation>喷雾器 Sim 启用/禁用</translation>
    </message>
    <message>
      <source>Squawk 7400 on RC failsafe</source>
      <translation>RC 故障保护上的 Squawk 7400</translation>
    </message>
    <message>
      <source>Stabilize Collective Mid-High</source>
      <translation>稳定总距中高</translation>
    </message>
    <message>
      <source>Stabilize mount2 pan/yaw angle</source>
      <translation>稳定支架2 平移/偏航角</translation>
    </message>
    <message>
      <source>Stall turn throttle</source>
      <translation>失速转动油门</translation>
    </message>
    <message>
      <source>Standby</source>
      <translation>待机</translation>
    </message>
    <message>
      <source>Start temperature</source>
      <translation>起始温度</translation>
    </message>
    <message>
      <source>Starter</source>
      <translation>入门者</translation>
    </message>
    <message>
      <source>Steep</source>
      <translation>陡峭</translation>
    </message>
    <message>
      <source>Steering control Derivative term filter frequency in Hz</source>
      <translation>转向控制 微分项滤波器频率（Hz）</translation>
    </message>
    <message>
      <source>Steering control I gain.  Corrects long term error between the desired turn rate (in rad/s) and actual</source>
      <translation>我获得了转向控制。  纠正所需转速（以 rad/s 为单位）与实际转速之间的长期误差</translation>
    </message>
    <message>
      <source>Steering control angular acceleration maximum</source>
      <translation>转向控制角加速度最大</translation>
    </message>
    <message>
      <source>Steering control rate P gain.  Converts the turn rate error (in radians/sec) to a steering control output (in the range -1 to +1)</source>
      <translation>转向控制率P增益。  将转弯速率误差（以弧度/秒为单位）转换为转向控制输出（范围为 -1 至 +1）</translation>
    </message>
    <message>
      <source>Step</source>
      <translation>步骤</translation>
    </message>
    <message>
      <source>Stillness threshold for detecting if we are moving</source>
      <translation>用于检测我们是否在移动的静止阈值</translation>
    </message>
    <message>
      <source>StopAtFence</source>
      <translation>停在栅栏</translation>
    </message>
    <message>
      <source>Stream rate of ATTITUDE and SIMSTATE (SITL only) to ground station</source>
      <translation>ATTITUDE和SIMSTATE（仅限SITL）到地面站的码流速率</translation>
    </message>
    <message>
      <source>Stream rate of RC_CHANNELS_SCALED (HIL only) to ground station</source>
      <translation>RC_CHANNELS_SCALED（仅限 HIL）至地面站的码流速率</translation>
    </message>
    <message>
      <source>Strict</source>
      <translation>严格</translation>
    </message>
    <message>
      <source>Super Simple Mode</source>
      <translation>超级简单模式</translation>
    </message>
    <message>
      <source>Suppress CRSF mode/rate message for ELRS systems</source>
      <translation>抑制 ELRS 系统的 CRSF 模式/速率消息</translation>
    </message>
    <message>
      <source>Surface Maximum Throttle</source>
      <translation>水面最大油门</translation>
    </message>
    <message>
      <source>Surface vessel, boat, ship</source>
      <translation>水面舰艇、船、船</translation>
    </message>
    <message>
      <source>Swash 1 Collective Direction</source>
      <translation>斜盘 1 总距方向</translation>
    </message>
    <message>
      <source>Swash 2 Collective Direction</source>
      <translation>斜盘 2 总距方向</translation>
    </message>
    <message>
      <source>Swash 2 Phase Angle Compensation</source>
      <translation>斜盘 2 相位角补偿</translation>
    </message>
    <message>
      <source>Switch to Land mode from all modes</source>
      <translation>从所有模式切换到陆地模式</translation>
    </message>
    <message>
      <source>SwitchPos6</source>
      <translation>开关Pos6</translation>
    </message>
    <message>
      <source>System ID of the ship</source>
      <translation>船舶系统ID</translation>
    </message>
    <message>
      <source>System identification Total Sweep length</source>
      <translation>系统识别 总扫描长度</translation>
    </message>
    <message>
      <source>TAP ESC Output ESC 2</source>
      <translation>TAP ESC 输出 ESC 2</translation>
    </message>
    <message>
      <source>TAP ESC Output ESC 6</source>
      <translation>TAP ESC 输出 ESC 6</translation>
    </message>
    <message>
      <source>TCP echo test</source>
      <translation>TCP回波测试</translation>
    </message>
    <message>
      <source>TECS will attempt to control the aircraft to this sink rate via pitch angle (throttle killed during flare)</source>
      <translation>TECS 将尝试通过俯仰角控制飞机达到该下沉率（拉平期间油门被关闭）</translation>
    </message>
    <message>
      <source>TEMP_Y</source>
      <translation>TEMP_Y</translation>
    </message>
    <message>
      <source>TEST_D</source>
      <translation>TEST_D</translation>
    </message>
    <message>
      <source>TEST_LP</source>
      <translation>TEST_LP</translation>
    </message>
    <message>
      <source>TEST_RC_X</source>
      <translation>TEST_RC_X</translation>
    </message>
    <message>
      <source>THERMAL</source>
      <translation>THERMAL</translation>
    </message>
    <message>
      <source>TMP119</source>
      <translation>TMP119</translation>
    </message>
    <message>
      <source>TOFSENSE-M mode to be used</source>
      <translation>使用TOFSENSE-M模式</translation>
    </message>
    <message>
      <source>TREvo3m</source>
      <translation>特雷沃3m</translation>
    </message>
    <message>
      <source>TXONLY</source>
      <translation>TXONLY</translation>
    </message>
    <message>
      <source>Tail type selection.  Simpler yaw controller used if external gyro is selected. Direct Drive Variable Pitch is used for tails that have a motor that is governed at constant speed by an ESC.  Tail pitch is still accomplished with a servo.  Direct Drive Fixed Pitch (DDFP) CW is used for helicopters with a rotor that spins clockwise when viewed from above. Direct Drive Fixed Pitch (DDFP) CCW is used for helicopters with a rotor that spins counter clockwise when viewed from above. In both DDFP cases, no servo is used for the tail and the tail motor esc is controlled by the yaw axis.</source>
      <translation>尾型选择。  如果选择外部陀螺仪，则使用更简单的偏航控制器。直接驱动可变螺距用于具有由 ESC 恒速控制的电机的尾部。  尾部俯仰仍然是通过舵机器完成的。  直驱固定螺距 (DDFP) CW 用于旋翼从上方观察时顺时针旋转的直升机。直驱固定螺距 (DDFP) CCW 用于旋翼从上方观察时逆时针旋转的直升机。在这两种DDFP情况下，尾部均未使用舵机，尾部电机电调由偏航轴控制。</translation>
    </message>
    <message>
      <source>Tailsitter VTOL control surface roll gain</source>
      <translation>Tailsitter VTOL 控制表面横滚增益</translation>
    </message>
    <message>
      <source>Tailsitter gain scaling mask</source>
      <translation>Tailsitter 增益缩放掩模</translation>
    </message>
    <message>
      <source>Tailsitter vector thrust gain in hover</source>
      <translation>悬停时尾座矢量推力增益</translation>
    </message>
    <message>
      <source>Takeoff climb rate</source>
      <translation>起飞爬升率</translation>
    </message>
    <message>
      <source>Takeoff mode altitude</source>
      <translation>起飞模式高度</translation>
    </message>
    <message>
      <source>Takeoff rotation airspeed</source>
      <translation>起飞旋转空速</translation>
    </message>
    <message>
      <source>Takeoff throttle min acceleration</source>
      <translation>起飞油门最小加速度</translation>
    </message>
    <message>
      <source>Target Head Speed</source>
      <translation>目标头速度</translation>
    </message>
    <message>
      <source>Target altitude to descend during a drop, (must be positive). This allows for rapidly clearing surrounding obstacles.</source>
      <translation>下降过程中下降的目标高度（必须为正值）。这样可以快速清除周围的障碍物。</translation>
    </message>
    <message>
      <source>Target pitch angle during pullup</source>
      <translation>上拉期间的目标俯仰角</translation>
    </message>
    <message>
      <source>Telem2</source>
      <translation>电信2</translation>
    </message>
    <message>
      <source>Telemetry Transmit power</source>
      <translation>遥测发射功率</translation>
    </message>
    <message>
      <source>Temperature calibration max</source>
      <translation>最大温度校准</translation>
    </message>
    <message>
      <source>Temperature sensor analog 0th polynomial coefficient</source>
      <translation>温度传感器模拟第0多项式系数</translation>
    </message>
    <message>
      <source>Temperature sensor bus number, typically used to select from multiple I2C buses</source>
      <translation>温度传感器总线数量，通常用于从多个I2C总线中进行选择</translation>
    </message>
    <message>
      <source>Temperature that the gyroscope was calibrated at</source>
      <translation>陀螺仪校准温度</translation>
    </message>
    <message>
      <source>Terminate action</source>
      <translation>终止行动</translation>
    </message>
    <message>
      <source>Terrain avoidance will not be applied if the vehicle is less than this distance from home</source>
      <translation>如果车辆距家的距离小于此距离，则不会应用地形回避</translation>
    </message>
    <message>
      <source>Terrain lookahead</source>
      <translation>地形前瞻</translation>
    </message>
    <message>
      <source>TestChan1</source>
      <translation>测试Chan1</translation>
    </message>
    <message>
      <source>TestMotor1</source>
      <translation>测试电机1</translation>
    </message>
    <message>
      <source>Tether Damping Constant</source>
      <translation>系绳阻尼常数</translation>
    </message>
    <message>
      <source>The 1-sigma uncertainty (in Gauss) for all axes, which will remain constant across all aiding measurements. Requires MS_EXT_MAG_EN to be enabled to be used.</source>
      <translation>所有轴的 1-sigma 不确定度（以高斯为单位），在所有辅助测量中保持恒定。需要启用 MS_EXT_MAG_EN 才能使用。</translation>
    </message>
    <message>
      <source>The FFT harmonic peak target that should be returned by FTN1.PkAvg. The resulting value will be used by the harmonic notch if configured to track the FFT frequency. By default the appropriate peak is auto-detected based on the harmonic fit between peaks and the energy-weighted average frequency on roll on pitch is used. Setting this to 1 will always target the highest energy peak. Setting this to 2 will target the highest energy peak that is lower in frequency than the highest energy peak. Setting this to 3 will target the highest energy peak that is higher in frequency than the highest energy peak. Setting this to 4 will target the highest energy peak on the roll axis only and only the roll frequency will be used (some vehicles have a much more pronounced peak on roll). Setting this to 5 will target the highest energy peak on the pitch axis only and only the pitch frequency will be used (some vehicles have a much more pronounced peak on roll).</source>
      <translation>FTN1.PkAvg 应返回的 FFT 谐波峰值目标。如果配置为跟踪 FFT 频率，则谐波陷波将使用所得值。默认情况下，根据峰值之间的谐波拟合自动检测适当的峰值，并使用横滚和俯仰上的能量加权平均频率。将其设置为 1 将始终以最高能量峰值为目标。将其设置为 2 将瞄准频率低于最高能量峰值的最高能量峰值。将其设置为 3 将瞄准频率高于最高能量峰值的最高能量峰值。将其设置为 4 将仅针对侧倾轴上的最高能量峰值，并且仅使用侧倾频率（某些车辆在侧倾时具有更明显的峰值）。将其设置为 5 将仅针对俯仰轴上的最高能量峰值，并且仅使用俯仰频率（某些车辆在横滚时具有更明显的峰值）。</translation>
    </message>
    <message>
      <source>The MAVLink Mode defines the set of streamed messages (for example the vehicle's attitude) and their sending rates.</source>
      <translation>MAVLink 模式定义了一组流消息（例如车辆的姿态）及其发送速率。</translation>
    </message>
    <message>
      <source>The PWM level on the throttle channel at or below which throttle failsafe triggers.</source>
      <translation>油门通道上的 PWM 级别，达到或低于该级别时油门故障安全触发。</translation>
    </message>
    <message>
      <source>The SF45 sets the update rate in Hz to allow greater resolution</source>
      <translation>SF45 设置更新速率（以 Hz 为单位）以允许更高的分辨率</translation>
    </message>
    <message>
      <source>The action to take on a short (FS_SHORT_TIMEOUT) failsafe event. A short failsafe event can be triggered either by loss of RC control (see THR_FS_VALUE) or by loss of GCS control (see FS_GCS_ENABL). If in CIRCLE or RTL mode this parameter is ignored. A short failsafe event in stabilization and manual modes will cause a change to CIRCLE mode if FS_SHORT_ACTN is 0 or 1, a change to FBWA mode with zero throttle if FS_SHORT_ACTN is 2, and a change to FBWB mode if FS_SHORT_ACTN is 4. In all other modes (AUTO, GUIDED and LOITER) a short failsafe event will cause no mode change if FS_SHORT_ACTN is set to 0, will cause a change to CIRCLE mode if set to 1, will change to FBWA mode with zero throttle if set to 2, or will change to FBWB if set to 4. Please see the documentation for FS_LONG_ACTN for the behaviour after FS_LONG_TIMEOUT seconds of failsafe.</source>
      <translation>短时故障保护事件（FS_SHORT_TIMEOUT）的处理动作。失去遥控控制（见 THR_FS_VALUE）或地面站控制（见 FS_GCS_ENABL）均可触发短时故障保护。在 CIRCLE 或 RTL 模式下忽略此参数。在自稳和手动模式下，FS_SHORT_ACTN 为 0 或 1 时切换至 CIRCLE，为 2 时切换至零油门的 FBWA，为 4 时切换至 FBWB。在其他模式（AUTO、GUIDED 和 LOITER）下，设为 0 时保持当前模式，为 1 时切换至 CIRCLE，为 2 时切换至零油门的 FBWA，为 4 时切换至 FBWB。故障持续 FS_LONG_TIMEOUT 秒之后的处理方式详见 FS_LONG_ACTN。</translation>
    </message>
    <message>
      <source>The airspeed alternative derived from groundspeed and heading will be declared valid as soon and as long the horizontal wind uncertainty is below this value.</source>
      <translation>只要水平风的不确定性低于该值，从地速和航向得出的空速替代方案就会被宣布有效。</translation>
    </message>
    <message>
      <source>The amount of flaps (as a percentage) to apply in the landing approach and flare of an automatic landing</source>
      <translation>自动着陆进场和拉平时应用的襟翼数量（百分比）</translation>
    </message>
    <message>
      <source>The baud rate of the Telem2 port. Most stm32-based boards can support rates of up to 1500. If you setup a rate you cannot support and then can't connect to your board you should load a firmware from a different vehicle type. That will reset all your parameters to defaults.</source>
      <translation>Telem2 端口的波特率。大多数基于 stm32 的板卡可支持高达 1500 的速率。如果您设置了无法支持的速率，然后无法连接到您的主板，您应该从不同的车辆类型加载固件。这会将您的所有参数重置为默认值。</translation>
    </message>
    <message>
      <source>The baud rate used for Serial8. Most stm32-based boards can support rates of up to 1500. If you setup a rate you cannot support and then can't connect to your board you should load a firmware from a different vehicle type. That will reset all your parameters to defaults.</source>
      <translation>Serial8 使用的波特率。大多数基于 stm32 的板卡可支持高达 1500 的速率。如果您设置了无法支持的速率，然后无法连接到您的主板，您应该从不同的车辆类型加载固件。这会将您的所有参数重置为默认值。</translation>
    </message>
    <message>
      <source>The behavior triggered when the flight controller disarms. You have the option to trigger your motors' disarm behaviors, set all motors to coast,
or set a predefined throttle setpoint</source>
      <translation>当飞行控制器上锁时触发该行为。您可以选择触发电机的解除行为、将所有电机设置为惯性滑行或设置预定义的油门设定点</translation>
    </message>
    <message>
      <source>The calibrated airspeed setpoint during the takeoff climbout. If set &lt;= 0, FW_AIRSPD_MIN will be set by default.</source>
      <translation>起飞爬升期间校准的空速设定值。如果设置 &lt;= 0，则默认设置 FW_AIRSPD_MIN。</translation>
    </message>
    <message>
      <source>The center frequency for the 2nd order notch filter on the primary gyro. This filter can be enabled to avoid feedback amplification of structural resonances at a specific frequency. This only affects the signal sent to the controllers, not the estimators. Applies to both angular velocity and angular acceleration sent to the controllers. See "IMU_GYRO_NF1_BW" to set the bandwidth of the filter. A value of 0 disables the filter.</source>
      <translation>主陀螺仪上二阶陷波滤波器的中心频率。可以启用该滤波器以避免特定频率下结构共振的反馈放大。这仅影响发送到控制器的信号，而不影响估计器。适用于发送到控制器的角速度和角加速度。请参阅“IMU_GYRO_NF1_BW”来设置滤波器的带宽。 0 的值会禁用滤波器。</translation>
    </message>
    <message>
      <source>The channel index (starting from 1 for channel 1) indicates which channel should be used for reading roll inputs from. A value of zero indicates the switch is not assigned.</source>
      <translation>通道索引（通道 1 从 1 开始）指示应使用哪个通道读取横滚输入。值为零表示未分配开关。</translation>
    </message>
    <message>
      <source>The controller scales the acceptance radius based on the angle between
the previous, current and next waypoint.
Higher value -&gt; smoother trajectory at the cost of how close the rover gets
to the waypoint (Set to -1 to disable corner cutting).</source>
      <translation>控制器根据前一个、当前和下一个航路点之间的角度缩放接受半径。值越高 -&gt; 轨迹越平滑，但代价是流动站距离航路点的距离有多近（设置为 -1 以禁用切角）。</translation>
    </message>
    <message>
      <source>The cutoff frequency for the 2nd order butterworth filter on the primary gyro. This only affects the angular velocity sent to the controllers, not the estimators. It applies also to the angular acceleration (D-Term filter), see IMU_DGYRO_CUTOFF. A value of 0 disables the filter.</source>
      <translation>主陀螺仪上二阶巴特沃斯滤波器的截止频率。这仅影响发送到控制器的角速度，而不影响估计器。它也适用于角加速度（D 项滤波器），请参阅 IMU_DGYRO_CUTOFF。 0 的值会禁用滤波器。</translation>
    </message>
    <message>
      <source>The delay for zigzag waypoint</source>
      <translation>之字形航路点的延迟</translation>
    </message>
    <message>
      <source>The distance from target beyond which the target is ignored</source>
      <translation>距目标的距离，超出该距离则忽略目标</translation>
    </message>
    <message>
      <source>The duration of the front transition when there is no airspeed feedback available.
When airspeed is used, transition timeout is declared if airspeed does not
reach VT_ARSP_BLEND after this time.</source>
      <translation>没有可用空速反馈时前过渡的持续时间。当使用空速时，如果在此时间之后空速未达到 VT_ARSP_BLEND，则声明转换超时。</translation>
    </message>
    <message>
      <source>The expected value of COMPASS_DEV_ID3, used by arming checks. Setting this to -1 means "don't care."</source>
      <translation>COMPASS_DEV_ID3 的预期值，用于解锁检查。将其设置为 -1 意味着“不关心”。</translation>
    </message>
    <message>
      <source>The function the relay channel is mapped to.</source>
      <translation>中继通道映射到的功能。</translation>
    </message>
    <message>
      <source>The highest angle the pitch can reach</source>
      <translation>俯仰所能达到的最高角度</translation>
    </message>
    <message>
      <source>The inertia is a 3 by 3 symmetric matrix.
This value can be set to 0 for a quad symmetric about its center of mass.</source>
      <translation>惯量是 3 × 3 对称矩阵。对于关于其质心对称的四边形，可以将该值设置为 0。</translation>
    </message>
    <message>
      <source>The lowest input maps and is clamped to this rate.</source>
      <translation>最低输入映射并被限制在该速率。</translation>
    </message>
    <message>
      <source>The maximum adjustment of terrain altitude based on the assumption that the vehicle is on the ground when it is armed. When the vehicle is armed the location of the vehicle is recorded, and when terrain data is available for that location a height adjustment for terrain data is calculated that aligns the terrain height at that location with the altitude recorded at arming. This height adjustment is applied to all terrain data. This parameter clamps the amount of adjustment. A value of zero disables the use of terrain height adjustment.</source>
      <translation>地形高度的最大调整基于车辆解锁时位于地面的假设。当车辆解锁时，记录车辆的位置，并且当该位置的地形数据可用时，计算地形数据的高度调整，将该位置处的地形高度与解锁时记录的高度对齐。此高度调整适用于所有地形数据。该参数限制调整量。值为零将禁用地形高度调整。</translation>
    </message>
    <message>
      <source>The maximum internal temperature allowed before triggering failsafe. Failsafe action is determined by FS_TEMP_ENABLE parameter.</source>
      <translation>触发故障保护之前允许的最高内部温度。故障安全动作由 FS_TEMP_ENABLE 参数决定。</translation>
    </message>
    <message>
      <source>The maximum rate the gyro control data (vehicle_angular_velocity) will be
allowed to publish at. This is the loop rate for the rate controller and outputs.
Note: sensor data is always read and filtered at the full raw rate (eg commonly 8 kHz) regardless of this setting.</source>
      <translation>允许发布陀螺仪控制数据 (vehicle_angular_velocity) 的最大速率。这是速率控制器和输出的循环速率。注意：无论此设置如何，传感器数据始终以完整原始速率（例如通常为 8 kHz）读取和过滤。</translation>
    </message>
    <message>
      <source>The maximum value that will be applied to the parameter currently being tuned with the transmitter's channel 6 knob</source>
      <translation>将应用于当前使用发射机通道 6 旋钮调谐的参数的最大值</translation>
    </message>
    <message>
      <source>The maximum vertical velocity the pilot may request in cm/s</source>
      <translation>飞行员可能要求的最大垂直速度（以厘米/秒为单位）</translation>
    </message>
    <message>
      <source>The minimal airspeed (calibrated airspeed) the user is able to command.
Further, if the airspeed falls below this value, the TECS controller will try to
increase airspeed more aggressively.
Has to be set according to the vehicle's stall speed (which should be set in FW_AIRSPD_STALL),
with some margin between the stall speed and minimum airspeed.
This value corresponds to the desired minimum speed with the default load factor (level flight, default weight),
and is automatically adpated to the current load factor (calculated from roll setpoint and WEIGHT_GROSS/WEIGHT_BASE).</source>
      <translation>用户能够命令的最小空速（校准空速）。此外，如果空速低于该值，TECS 控制器将尝试更积极地增加空速。必须根据飞行器的失速速度（应在FW_AIRSPD_STALL中设置）进行设置，失速速度和最小空速之间有一定的余量。该值对应于默认负载系数（水平飞行、默认重量）所需的最小速度，并自动适应当前负载系数（根据横滚设定点和 WEIGHT_GROSS/WEIGHT_BASE 计算）。</translation>
    </message>
    <message>
      <source>The minimum altitude which the aircraft must be above to abort a deepstall landing</source>
      <translation>飞机中止深失速着陆必须达到的最低高度</translation>
    </message>
    <message>
      <source>The minimum value that will be applied to the parameter currently being tuned with the transmitter's channel 6 knob</source>
      <translation>将应用于当前使用发射机通道 6 旋钮调谐的参数的最小值</translation>
    </message>
    <message>
      <source>The number of 32x28 cache blocks to keep in memory. Each block uses about 1800 bytes of memory</source>
      <translation>要保留在内存中的 32x28 缓存块的数量。每个块使用大约 1800 字节的内存</translation>
    </message>
    <message>
      <source>The offset for the Vtail and elevon output mixers, as a percentage. This can be used in combination with MIXING_GAIN to configure how the control surfaces respond to input. The response to aileron or elevator input can be increased by setting this parameter to a positive or negative value. A common usage is to enter a positive value to increase the aileron response of the elevons of a flying wing. The default value of zero will leave the aileron-input response equal to the elevator-input response.</source>
      <translation>Vtail 和升降舵输出混合器的偏移量，以百分比表示。这可以与 MIXING_GAIN 结合使用来配置控制面如何响应输入。通过将此参数设置为正值或负值，可以增加对副翼或升降舵输入的响应。常见用法是输入正值以增加飞翼升降副翼的副翼响应。默认值零将使副翼输入响应等于升降舵输入响应。</translation>
    </message>
    <message>
      <source>The orientation of the device (Radians) with respect to the vehicle frame around the x axis.
Requires MS_SVT_EN to be enabled to be used.</source>
      <translation>设备相对于车架绕 x 轴的方向（弧度）。需要启用 MS_SVT_EN 才能使用。</translation>
    </message>
    <message>
      <source>The orientation of the device (Radians) with respect to the vehicle frame around the z axis.
Requires MS_EXT_MAG_EN to be enabled to be used.</source>
      <translation>设备相对于车架绕 z 轴的方向（弧度）。需要启用 MS_EXT_MAG_EN 才能使用。</translation>
    </message>
    <message>
      <source>The orientation of the second compass relative to the frame (if external) or autopilot board (if internal).</source>
      <translation>第二个罗盘相对于框架（如果外部）或自动驾驶仪板（如果内部）的方向。</translation>
    </message>
    <message>
      <source>The physical COM port on the connected device, currently only applies to SBF and GSOF GPS,Renamed in 4.6 and later to GPS1_COM_PORT.</source>
      <translation>连接设备上的物理COM端口，目前仅适用于SBF和GSOF GPS，在4.6及以后更名为GPS1_COM_PORT。</translation>
    </message>
    <message>
      <source>The pin number that the airspeed sensor is connected to for analog sensors. Values for some autopilots are given as examples. Search wiki for "Analog pins".</source>
      <translation>对于模拟传感器，空速传感器所连接的引脚号。一些自动驾驶仪的值作为示例给出。在 wiki 中搜索“模拟引脚”。</translation>
    </message>
    <message>
      <source>The previous mode will be restored if the horizontal distance to the thermalling start location exceeds this value. -1 to disable.</source>
      <translation>如果到热力起始位置的水平距离超过此值，将恢复之前的模式。 -1 禁用。</translation>
    </message>
    <message>
      <source>The rate at which raw IMU data is integrated to produce delta angles and delta velocities. Recommended to set this to a multiple of the estimator update period (currently 10 ms for ekf2).</source>
      <translation>对原始 IMU 数据进行积分以产生增量角度和增量速度的速率。建议将此设置为估计器更新周期的倍数（当前 ekf2 为 10 ms）。</translation>
    </message>
    <message>
      <source>The roll rate to use when entering a roll maneuver</source>
      <translation>进入横滚机动时使用的横滚速率</translation>
    </message>
    <message>
      <source>The speed in m/s which the aircraft will attempt to maintain horizontally while flying home. If this is set to zero, WP_SPD will be used instead.</source>
      <translation>飞机在返航时尝试保持水平的速度（以米/秒为单位）。如果设置为零，则将使用 WP_SPD。</translation>
    </message>
    <message>
      <source>The stream the autopilot sets up on the receiver to output the logging data. Set this to another value if the default stream is already used for another purpose.</source>
      <translation>自动驾驶仪在接收器上设置的流以输出记录数据。如果默认流已用于其他目的，请将此设置为另一个值。</translation>
    </message>
    <message>
      <source>The throttle failsafe allows you to configure a software RC failsafe activated by a setting on the throttle input channel. It also enables RC failsafe on absence of RC signals being recieved.</source>
      <translation>油门故障保护允许您配置由油门输入通道上的设置激活的软件 RC 故障保护。它还可以在没有接收到 RC 信号时启用 RC 故障保护。</translation>
    </message>
    <message>
      <source>The time (in seconds) of persistent data link loss before GCS failsafe occurs. </source>
      <translation>GCS 故障保护发生之前持续数据链路丢失的时间（以秒为单位）。 </translation>
    </message>
    <message>
      <source>The time the system should do open loop loiter and wait for GPS recovery
before it starts descending. Set to 0 to disable. Roll angle is set to FW_GPSF_R.
Does only apply for fixed-wing vehicles or VTOLs with NAV_FORCE_VT set to 0.</source>
      <translation>系统在开始下降之前应进行开环盘旋并等待 GPS 恢复的时间。设置为 0 以禁用。横滚角设置为 FW_GPSF_R。仅适用于 NAV_FORCE_VT 设置为 0 的固定翼车辆或 VTOL。</translation>
    </message>
    <message>
      <source>The trim CAS (calibrated airspeed) of the vehicle. If an airspeed controller is active, this is the default airspeed setpoint that the controller will try to achieve. This value corresponds to the trim airspeed with the default load factor (level flight, default weight).</source>
      <translation>车辆的配平 CAS（校准空速）。如果空速控制器处于活动状态，则这是控制器将尝试达到的默认空速设定值。该值对应于默认负载系数（水平飞行、默认重量）的配平空速。</translation>
    </message>
    <message>
      <source>The value is mapped to the lowest throttle stick position in Stabilized mode.
Too low collective thrust leads to loss of roll/pitch/yaw torque control authority.
Airmode is used to keep torque authority with zero thrust (see MC_AIRMODE).</source>
      <translation>该值映射到稳定模式下的最低油门摇杆位置。总距推力太低会导致横滚/俯仰/偏航扭矩控制权丧失。空气模式用于在零推力下保持扭矩权限（参见 MC_AIRMODE）。</translation>
    </message>
    <message>
      <source>The vehicle will climb this many m during the initial climb portion of the RTL. During this time the roll will be limited to LEVEL_ROLL_LIMIT degrees.</source>
      <translation>在 RTL 的初始爬升部分，车辆将爬升这么多米。在此期间，横滚将限制为 LEVEL_ROLL_LIMIT 度。</translation>
    </message>
    <message>
      <source>There will be a warning message if the current waypoint is more distant than MIS_DIST_1WP from Home.
Has no effect on mission validity.
Set a value of zero or less to disable.</source>
      <translation>如果当前航路点距 Home 的距离超过 MIS_DIST_1WP，将会出现警告消息。对任务有效性没有影响。设置为零或更小的值以禁用。</translation>
    </message>
    <message>
      <source>Thermal compensation for rate gyro sensors</source>
      <translation>速率陀螺仪传感器的热补偿</translation>
    </message>
    <message>
      <source>Third Gyro bias</source>
      <translation>第三陀螺仪偏置</translation>
    </message>
    <message>
      <source>Third Rangefinder type</source>
      <translation>第三测距仪类型</translation>
    </message>
    <message>
      <source>Third driver</source>
      <translation>第三司机</translation>
    </message>
    <message>
      <source>This adds a delay in milliseconds to boot to ensure peripherals initialise fully</source>
      <translation>这会增加启动延迟（以毫秒为单位），以确保外设完全初始化</translation>
    </message>
    <message>
      <source>This allows selection of LTE band. A value of -1 means no band setting change is made. A value of 0 sets all bands. Otherwise the specified band is set.</source>
      <translation>这允许选择 LTE 频段。 -1 值表示未进行频段设置更改。 0 的值设置所有频段。否则设置指定的频段。</translation>
    </message>
    <message>
      <source>This allows to add a proportional factor of the collective pitch command to the yaw command. A negative value is needed when positive thrust of the tail rotor rotates the vehicle opposite to the main rotor turn direction. tail_output += CA_HELI_YAW_CP_S * abs(collective_pitch - CA_HELI_YAW_CP_O)</source>
      <translation>这允许将总距俯仰命令的比例因子添加到偏航命令。当尾桨的正推力使飞行器与主旋翼旋转方向相反时，需要负值。 tail_output += CA_HELI_YAW_CP_S * abs(collective_pitch - CA_HELI_YAW_CP_O)</translation>
    </message>
    <message>
      <source>This can be set to indicate that drone behavior needs to be changed to match a custom setting</source>
      <translation>这可以设置为指示需要更改无人机行为以匹配自定义设置</translation>
    </message>
    <message>
      <source>This check detects if a traffic avoidance system (ADSB/FLARM transponder) is missing. Depending on the value of the parameter, the check can be disabled, warn only, or deny arming.</source>
      <translation>该检查可检测交通回避系统（ADSB/FLARM 应答器）是否缺失。根据参数的值，可以禁用检查、仅警告或拒绝解锁。</translation>
    </message>
    <message>
      <source>This check detects if there are hardfault files present on the SD card. If so, and the parameter is enabled, arming is prevented.</source>
      <translation>此检查检测 SD 卡上是否存在硬故障文件。如果是这样，并且该参数已启用，则会阻止解锁。</translation>
    </message>
    <message>
      <source>This configures the deadband that is tolerated before adjusting the idle setpoint</source>
      <translation>这将配置调整空闲设定点之前可容忍的死区</translation>
    </message>
    <message>
      <source>This controls how much extra throttle to add based on pitch ange. The value is for 90 degrees and is applied in proportion to pitch</source>
      <translation>这控制根据俯仰角添加多少额外油门。该值适用于 90 度，并与俯仰成比例应用</translation>
    </message>
    <message>
      <source>This controls how rapidly two aircraft are brought back into time sync</source>
      <translation>这控制两架飞机恢复时间同步的速度</translation>
    </message>
    <message>
      <source>This controls optional aspects of the scheduler.</source>
      <translation>这控制调度程序的可选方面。</translation>
    </message>
    <message>
      <source>This controls the fitness level required for a successful compass calibration. A lower value makes for a stricter fit (less likely to pass). This is the value used for the primary magnetometer. Other magnetometers get double the value.</source>
      <translation>这控制成功罗盘校准所需的健康水平。值越低，拟合越严格（通过的可能性越小）。这是用于主磁力计的值。其他磁力计的价值是其两倍。</translation>
    </message>
    <message>
      <source>This controls the maximum error in degrees on any axis before HIL will reset the DCM attitude to match the HIL_STATE attitude. This limit will prevent poor timing on HIL from causing a major attitude error. If the value is zero then no limit applies.</source>
      <translation>这控制了 HIL 重置 DCM 姿态以匹配 HIL_STATE 姿态之前任何轴上的最大角度误差。此限制将防止 HIL 的计时不当导致重大姿态错误。如果该值为零，则不适用任何限制。</translation>
    </message>
    <message>
      <source>This controls the rate of the main control loop in Hz. This should only be changed by developers. This only takes effect on restart</source>
      <translation>这控制主控制循环的速率（以 Hz 为单位）。这只能由开发人员更改。这只在重启时生效</translation>
    </message>
    <message>
      <source>This controls what channels have full manual control when hovering as a tailsitter and the Q_TAILSIT_MASKCH channel in high. This can be used to teach yourself to prop-hang a 3D plane by learning one or more channels at a time.</source>
      <translation>此选项控制当悬停为尾翼且 Q_TAILSIT_MASKCH 通道处于高电平时哪些通道具有完全手动控制。这可用于通过一次学习一个或多个通道来教自己支撑 3D 平面。</translation>
    </message>
    <message>
      <source>This controls whether stick input when hovering as a tailsitter follows the conventions for fixed wing hovering or multicopter hovering. When multicopter input is selected the roll stick will roll the aircraft in earth frame and yaw stick will yaw in earth frame. When using fixed wing input the roll and yaw sticks are swapped so that the roll stick controls earth-frame yaw and rudder controls earth-frame roll. When body-frame roll is selected, the yaw stick controls earth-frame yaw rate and the roll stick controls roll in the tailsitter's body frame.</source>
      <translation>这控制当尾翼悬停时摇杆输入是否遵循固定翼悬停或多旋翼悬停的惯例。当选择多旋翼输入时，横滚杆将使飞机在地球坐标系中横滚，偏航杆将在地球坐标系中偏航。当使用固定翼输入时，横滚杆和偏航杆会互换，以便横滚杆控制地球框架偏航，方向舵控制地球框架横滚。当选择机体框架横滚时，偏航杆控制地球框架偏航速率，横滚杆控制尾座机体框架的横滚。</translation>
    </message>
    <message>
      <source>This converts the target roll/pitch angle of the aircraft into the correcting (into wind) yaw rate. e.g. Gain = 2, roll = 30 deg, pitch = 0 deg, yaw rate = 60 deg/s.</source>
      <translation>这会将飞机的目标横滚/俯仰角转换为校正（风）偏航率。例如增益 = 2，横滚 = 30 deg，俯仰 = 0 deg，偏航率 = 60 deg/s。</translation>
    </message>
    <message>
      <source>This determines if moving the left stick to the lower right arms and to the lower left disarms the vehicle.</source>
      <translation>这决定了将左摇杆移动到右下臂和左下是否会解除车辆的解锁。</translation>
    </message>
    <message>
      <source>This enables EKF1 to be disabled when using alternative algorithms. When disabling it, the alternate EKF2 estimator must be enabled by setting EK2_ENABLED = 1 and flight control algorithms must be set to use the alternative estimator by setting AHRS_EKF_TYPE = 2.</source>
      <translation>这使得 EKF1 在使用替代算法时被禁用。禁用它时，必须通过设置 EK2_ENABLED = 1 来启用备用 EKF2 估计器，并且必须通过设置 AHRS_EKF_TYPE = 2 将飞行控制算法设置为使用备用估计器。</translation>
    </message>
    <message>
      <source>This enables a logic that automatically adjusts the output of the rate controller to take
into account the real torque produced by an aerodynamic control surface given
the current deviation from the trim airspeed (FW_AIRSPD_TRIM).
Enable when using aerodynamic control surfaces (e.g.: plane)
Disable when using rotor wings (e.g.: autogyro)</source>
      <translation>这使得逻辑能够自动调整速率控制器的输出，以考虑到给定当前与配平空速（FW_AIRSPD_TRIM）的偏差由空气动力控制表面产生的实际扭矩。使用空气动力学控制面（例如：飞机）时启用 使用旋翼（例如：旋翼机）时禁用</translation>
    </message>
    <message>
      <source>This enables custom rotations</source>
      <translation>这可以实现自定义旋转</translation>
    </message>
    <message>
      <source>This enables terrain following for RTL and LAND flight modes. To use this option TERRAIN_ENABLE must be 1 and the GCS must  support sending terrain data to the aircraft.  In RTL the RTL_ALT will be considered a height above the terrain.  In LAND mode the vehicle will slow to LAND_SPEED 10m above terrain (instead of 10m above home).  This parameter does not affect AUTO and Guided which use a per-command flag to determine if the height is above-home, absolute or above-terrain.</source>
      <translation>这使得 RTL 和 LAND 飞行模式能够跟随地形。要使用此选项，TERRAIN_ENABLE 必须是 1，并且 GCS 必须支持向飞机发送地形数据。  在 RTL 中，RTL_ALT 将被视为高于地形的高度。  在 LAND 模式下，车辆将在地形上方减速至 LAND_SPEED 10m（而不是在家上方的 10m）。  此参数不会影响 AUTO 和 Guided，它们使用每个命令标志来确定高度是高于起始高度、绝对高度还是高于地形。</translation>
    </message>
    <message>
      <source>This enables the use of synthetic airspeed in TECS for aircraft that don't have a real airspeed sensor. This is useful for development testing where the user is aware of the considerable limitations of the synthetic airspeed system, such as very poor estimates when a wind estimate is not accurate. Do not enable this option unless you fully understand the limitations of a synthetic airspeed estimate. This option has no effect if a healthy airspeed sensor is being used for airspeed measurements.</source>
      <translation>这使得没有真实空速传感器的飞机能够在 TECS 中使用合成空速。这对于开发测试非常有用，其中用户意识到合成空速系统的相当大的局限性，例如当风估计不准确时估计非常差。除非您完全了解合成空速估算的局限性，否则请勿启用此选项。如果使用健康的空速传感器进行空速测量，则此选项无效。</translation>
    </message>
    <message>
      <source>This filters the load test output. A value of 1 means no filter. 2 means values are repeated once. 3 means values are repeated 3 times, etc</source>
      <translation>这会过滤负载测试输出。 1 的值表示没有滤波器。 2 表示值重复一次。 3 表示值重复 3 次，等等</translation>
    </message>
    <message>
      <source>This increment is added to TRIM_ROLL when airspeed is FW_AIRSPD_MAX.</source>
      <translation>当空速为 FW_AIRSPD_MAX 时，该增量被添加到 TRIM_ROLL。</translation>
    </message>
    <message>
      <source>This is a 3D GPS speed threshold above which we will force arm the vehicle to start logging. WARNING: This should only be used on a vehicle with no propellers attached to the flight controller and when the flight controller is not in control of the vehicle.</source>
      <translation>这是 3D GPS 速度阈值，高于该阈值我们将强制车辆开始记录。 WARNING：仅应在飞行控制器未连接螺旋桨的飞行器上以及飞行控制器未控制飞行器时使用。</translation>
    </message>
    <message>
      <source>This is a minimum PWM value for engine start channel for an engine stop to be commanded. Setting this value will avoid RC input glitches with low PWM values from causing an unwanted engine stop. A value of zero means any PWM below 1300 triggers an engine stop.</source>
      <translation>这是用于命令发动机停止的发动机启动通道的最小 PWM 值。设置此值将避免 RC 输入毛刺与低 PWM 值导致不必要的发动机停止。值为零意味着任何低于 1300 的 PWM 都会触发发动机停止。</translation>
    </message>
    <message>
      <source>This is an RC input channel for requesting engine start. Engine will try to start when channel is at or above 1700. Engine will stop when channel is at or below 1300. Between 1301 and 1699 the engine will not change state unless a MAVLink command or mission item commands a state change, or the vehicle is disarmed. See ICE_STARTCHN_MIN parameter to change engine stop PWM value and/or to enable debouncing of the START_CH to avoid accidental engine kills due to noise on channel.</source>
      <translation>这是用于请求发动机启动的 RC 输入通道。当通道等于或高于 1700 时，发动机将尝试启动。当通道等于或低于 1300 时，发动机将停止。在 1301 和 1699 之间，发动机不会改变状态，除非 MAVLink 命令或任务项命令状态改变，或者车辆被上锁。请参阅 ICE_STARTCHN_MIN 参数以更改发动机停止 PWM 值和/或启用 START_CH 去抖以避免由于通道上的噪音而导致发动机意外停机。</translation>
    </message>
    <message>
      <source>This is maximum yaw acceleration to use</source>
      <translation>这是要使用的最大偏航加速度</translation>
    </message>
    <message>
      <source>This is the 2nd order temperature coefficient from a temperature calibration</source>
      <translation>这是温度校准的二阶温度系数</translation>
    </message>
    <message>
      <source>This is the RMS value of noise in the altitude measurement. Increasing it reduces the weighting of the baro measurement and will make the filter respond more slowly to baro measurement errors, but will make it more sensitive to GPS and accelerometer errors.</source>
      <translation>这是高度测量中噪声的 RMS 值。增加它会减少气压测量的权重，并使滤波器对气压测量误差的响应更慢，但会使其对 GPS 和加速度计误差更加敏感。</translation>
    </message>
    <message>
      <source>This is the action taken when the left action button is pressed</source>
      <translation>这是按下左侧操作按钮时执行的操作</translation>
    </message>
    <message>
      <source>This is the actual weight of the vehicle at any time. This value will differ from WEIGHT_BASE in case weight was added or removed from the base weight. Examples are the addition of payloads or larger batteries. A zero or negative value disables trim throttle and minimum airspeed compensation based on weight.</source>
      <translation>这是任何时候车辆的实际重量。如果在基本重量中添加或删除重量，则该值将与 WEIGHT_BASE 不同。例如添加有效负载或更大的电池。零或负值将禁用调整油门和基于重量的最小空速补偿。</translation>
    </message>
    <message>
      <source>This is the altitude the vehicle will move to as the final stage of Returning to Launch or after completing a mission.  Set to zero to land.</source>
      <translation>这是飞行器返回发射的最后阶段或完成任务后将移动到的高度。  设置为零以着陆。</translation>
    </message>
    <message>
      <source>This is the angular error in attitude beyond which the quadplane VTOL motors will provide stability assistance. This will only be used if Q_ASSIST_SPEED is also positive and non-zero. Assistance will be given if the attitude is outside the normal attitude limits by at least 5 degrees and the angular error in roll or pitch is greater than this angle for at least Q_ASSIST_DELAY seconds. Set to zero to disable angle assistance.</source>
      <translation>这是姿态角度误差，超过该角度误差四平面 VTOL 电机将提供稳定性辅助。仅当 Q_ASSIST_SPEED 也为正且非零时才会使用此值。如果姿态超出正常姿态限制至少 5 度，并且横滚或俯仰角度误差大于该角度至少 Q_ASSIST_DELAY 秒，则将提供帮助。设置为零以禁用角度辅助。</translation>
    </message>
    <message>
      <source>This is the applied simulated acceleration to the 3rd accelerometer (Y-axis)</source>
      <translation>这是对第三个加速度计（Y 轴）应用的模拟加速度</translation>
    </message>
    <message>
      <source>This is the applied simulated angular rate to the 3rd gyroscope (X-axis)</source>
      <translation>这是对第三个陀螺仪（X 轴）应用的模拟角速率</translation>
    </message>
    <message>
      <source>This is the battery voltage below which no output limiting is done</source>
      <translation>这是电池电压，低于该电压则不进行输出限制</translation>
    </message>
    <message>
      <source>This is the distance from the takeoff location where the plane will loiter. The loiter point will be in the direction of takeoff (the direction the plane is facing when the plane begins takeoff)</source>
      <translation>这是距飞机悬停的起飞地点的距离。盘旋点将位于起飞方向（飞机开始起飞时飞机所面向的方向）</translation>
    </message>
    <message>
      <source>This is the gain from the integral of steering angle. Increasing this gain causes the controller to trim out steady offsets due to an out of trim vehicle.</source>
      <translation>这是转向角积分的增益。增加该增益会导致控制器消除由于车辆不平衡而导致的稳定偏移。</translation>
    </message>
    <message>
      <source>This is the integrator gain on the control loop during takeoff. Increase to increase the rate at which speed and height offsets are trimmed out.</source>
      <translation>这是起飞期间控制环路上的积分器增益。增加以增加微调速度和高度偏移的速率。</translation>
    </message>
    <message>
      <source>This is the maximum acceptable altitude discrepancy between GPS altitude and barometric presssure altitude calculated against a standard atmosphere for arming checks to pass. If you are getting an arming error due to this parameter then you may have a faulty or substituted barometer. A common issue is vendors replacing a MS5611 in a "Pixhawk" with a MS5607. If you have that issue then please see BARO_OPTIONS parameter to force the MS5611 to be treated as a MS5607. This check is disabled if the value is zero.</source>
      <translation>这是 GPS 高度与根据标准大气计算的气压高度之间的最大可接受高度差异，以便通过解锁检查。如果您由于此参数而收到解锁错误，则您的气压计可能有故障或被替换。一个常见问题是供应商将“Pixhawk”中的 MS5611 替换为 MS5607。如果您遇到该问题，请参阅 BARO_OPTIONS 参数以强制将 MS5611 视为 MS5607。如果该值为零，则禁用此检查。</translation>
    </message>
    <message>
      <source>This is the maximum speed at which the motor angle will change for a tiltrotor when moving from hover to forward flight. When this is zero the Q_TILT_RATE_UP value is used.</source>
      <translation>这是倾转旋翼机从悬停移动到向前飞行时电机角度变化的最大速度。当该值为零时，使用 Q_TILT_RATE_UP 值。</translation>
    </message>
    <message>
      <source>This is the maximum yaw rate for pilot input on rudder stick in degrees/second and weathervaning is limited to half of this value</source>
      <translation>这是舵杆上飞行员输入的最大偏航率（以度/秒为单位），风标限制为该值的一半</translation>
    </message>
    <message>
      <source>This is the minimum assumed ground speed in meters/second for steering. Having a minimum speed prevents oscillations when the vehicle first starts moving. The vehicle can still drive slower than this limit, but the steering calculations will be done based on this minimum speed.</source>
      <translation>这是转向时假定的最小地面速度（以米/秒为单位）。具有最低速度可以防止车辆首次开始移动时出现振荡。车辆的行驶速度仍可以低于此限制，但转向计算将根据此最低速度进行。</translation>
    </message>
    <message>
      <source>This is the number of msec that the GPS position measurements lag behind the inertial measurements.</source>
      <translation>这是 GPS 位置测量滞后于惯性测量的毫秒数。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PCA9685_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PCA9685_FUNC2).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC2）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PWM_AUX_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_AUX_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see PWM_MAIN_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_MAIN_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see RBCLW_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 RBCLW_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_EC_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC7).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_SV_FUNC7）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC3).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_EC_FUNC3）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC3).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_SV_FUNC3）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC11).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC11）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC10).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC10）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode.
When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PCA9685_FUNC11).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC11）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PCA9685_FUNC4).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PCA9685_FUNC4）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PWM_AUX_FUNC3).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_AUX_FUNC3）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see PWM_MAIN_FUNC3).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 PWM_MAIN_FUNC3）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_EC_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_SV_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see SIM_GZ_WH_FUNC1).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 SIM_GZ_WH_FUNC1）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC5).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_EC_FUNC5）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC5).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UAVCAN_SV_FUNC5）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC13).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC13）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see UCAN1_ESC_FUNC6).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 UCAN1_ESC_FUNC6）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC12).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC12）。</translation>
    </message>
    <message>
      <source>This is the output value that is set when in failsafe mode. When set to -1 (default), the value depends on the function (see VTQ_IO_FUNC6).</source>
      <translation>这是在故障安全模式下设置的输出值。当设置为 -1（默认）时，该值取决于功能（参见 VTQ_IO_FUNC6）。</translation>
    </message>
    <message>
      <source>This is the pitch angle at which tailsitter aircraft will change from fixed wing control to VTOL control, if zero Q_TAILSIT_ANGLE will be used</source>
      <translation>如果使用零 Q_TAILSIT_ANGLE，则这是尾翼飞机从固定翼控制更改为 VTOL 控制的俯仰角</translation>
    </message>
    <message>
      <source>This is the ratio of static pressure error to dynamic pressure generated by a negative wind relative velocity along the Y body axis. If the baro height estimate rises during sideways flight to the left, then this should be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_BCOEF_X and EK3_BCOEF_Y parameters have been tuned.</source>
      <translation>这是沿 Y 体轴负风相对速度产生的静压误差与动压之比。如果在向左横向飞行期间巴罗高度估计值上升，则该值应该是负数。仅当使用 EKF3 且已调整 EK3_BCOEF_X 和 EK3_BCOEF_Y 参数时，多旋翼飞行器才能使用此功能。</translation>
    </message>
    <message>
      <source>This is the ratio of static pressure error to dynamic pressure generated by a positive wind relative velocity along the Z body axis. If the baro height estimate rises above truth height during climbing flight (or forward flight with a high forwards lean angle), then this should be a negative number. Multirotors can use this feature only if using EKF3 and if the EK3_DRAG_BCOEF_X and EK3_DRAG_BCOEF_Y parameters have been tuned.</source>
      <translation>这是静压误差与沿 Z 体轴正风相对速度产生的动压之比。如果在爬升飞行（或具有高前倾角度的向前飞行）期间气压高度估计值高于真实高度，则这应该是负数。仅当使用 EKF3 且已调整 EK3_DRAG_BCOEF_X 和 EK3_DRAG_BCOEF_Y 参数时，多旋翼飞行器才能使用此功能。</translation>
    </message>
    <message>
      <source>This is the scale IAS --&gt; CAS of the third airspeed sensor instance</source>
      <translation>这是第三个空速传感器实例的比例 IAS --&gt; CAS</translation>
    </message>
    <message>
      <source>This is the speed in Hertz that your ESCs will receive updates</source>
      <translation>这是您的 ESC 接收更新的速度（以赫兹为单位）</translation>
    </message>
    <message>
      <source>This is the time constant for correcting roll errors. A smaller value leads to faster roll corrections</source>
      <translation>这是校正横滚误差的时间常数。较小的值会导致更快的横滚校正</translation>
    </message>
    <message>
      <source>This is the type of tiltrotor when TILT_MASK is non-zero. A continuous tiltrotor can tilt the rotors to any angle on demand. A binary tiltrotor assumes a retract style servo where the servo is either fully forward or fully up. In both cases the servo can't move faster than Q_TILT_RATE. A vectored yaw tiltrotor will use the tilt of the motors to control yaw in hover, Bicopter tiltrottor must use the tailsitter frame class (10)</source>
      <translation>这是 TILT_MASK 非零时的倾转旋翼类型。连续倾转旋翼机可以根据需要将旋翼倾斜到任何角度。二元倾转旋翼机采用缩回式舵机系统，其中舵机系统要么完全向前，要么完全向上。在这两种情况下，舵机系统的移动速度都不会快于 Q_TILT_RATE。矢量偏航倾转旋翼机将使用电机的倾斜来控制悬停时的偏航，双旋翼倾转旋翼机必须使用尾座框架类（10）</translation>
    </message>
    <message>
      <source>This is the velocity at which pulsing is allowed. Any velocity between VELOCITY_CUTOFF and -VELOCITY_CUTOFF will not pulse.</source>
      <translation>这是允许脉冲的速度。 VELOCITY_CUTOFF 和 -VELOCITY_CUTOFF 之间的任何速度都不会产生脉冲。</translation>
    </message>
    <message>
      <source>This is used internally only: an airframe configuration might set an expected parameter version value via PARAM_DEFAULTS_VER. This is checked on bootup against SYS_PARAM_VER, and if they do not match, parameters are reset and reloaded from the airframe configuration.</source>
      <translation>这仅在内部使用：机身配置可以通过 PARAM_DEFAULTS_VER 设置预期的参数版本值。这是在启动时针对 SYS_PARAM_VER 进行检查的，如果它们不匹配，则会重置参数并从机身配置中重新加载。</translation>
    </message>
    <message>
      <source>This is used to force flow control on or off for the the mavlink instance. By default it is auto detected. Use when auto detection fails.</source>
      <translation>这用于强制打开或关闭 mavlink 实例的流量控制。默认情况下会自动检测到。自动检测失败时使用。</translation>
    </message>
    <message>
      <source>This limits the difference between the learned earth magnetic field and the earth field from the world magnetic model tables. A value of zero means to disable the use of the WMM tables.</source>
      <translation>这限制了学习的地球磁场与世界磁模型表中的地球磁场之间的差异。值为零意味着禁用 WMM 表。</translation>
    </message>
    <message>
      <source>This makes the position of the landing beacon follow the simulated ship from SIM_SHIP. The ship movement is controlled with the SIM_SHIP parameters</source>
      <translation>这使得着陆信标的位置跟随SIM_SHIP的模拟船。船舶运动由 SIM_SHIP 参数控制</translation>
    </message>
    <message>
      <source>This noise controls the growth of wind state error estimates. Increasing it makes wind estimation faster and noisier.</source>
      <translation>这种噪声控制着风状态误差估计的增长。增加它会使风估计更快、噪音更大。</translation>
    </message>
    <message>
      <source>This param controls how much headwind compensation is used when landing.  Headwind speed component multiplied by this parameter is added to TECS_LAND_ARSPD command.  Set to Zero to disable.  Note:  The target landing airspeed command is still limited to ARSPD_FBW_MAX.</source>
      <translation>该参数控制着陆时使用多少逆风补偿。  逆风速度分量乘以该参数被添加到 TECS_LAND_ARSPD 命令中。  设置为零以禁用。  注：目标着陆空速命令仍限于 ARSPD_FBW_MAX。</translation>
    </message>
    <message>
      <source>This parameter allows you to to set the PSI (pounds per square inch) range for your sensor. You should not change this unless you examine the datasheet for your device</source>
      <translation>此参数允许您设置传感器的 PSI（磅每平方英寸）范围。除非您检查设备的数据表，否则不应更改此设置</translation>
    </message>
    <message>
      <source>This parameter controls the prediction of drag produced by bluff body drag along the forward/reverse axis when flying a multi-copter which enables estimation of wind drift when enabled by the EKF2_DRAG_CTRL parameter. The drag produced by this effect scales with speed squared. The predicted drag from the rotors is specified separately by the EKF2_MCOEF parameter. Set this parameter to zero to turn off the bluff body drag model for this axis.</source>
      <translation>该参数控制飞行多旋翼飞行器时沿前向/反向轴钝体阻力产生的阻力的预测，当通过 EKF2_DRAG_CTRL 参数启用时，可以估计风漂移。这种效应产生的阻力与速度的平方成正比。转子的预测阻力由 EKF2_MCOEF 参数单独指定。将此参数设置为零可关闭该轴的钝体阻力模型。</translation>
    </message>
    <message>
      <source>This parameter controls the source of battery data. The value 'Power Module' means that measurements are expected to come from a power module. If the value is set to 'External' then the system expects to receive mavlink battery status messages. If the value is set to 'ESCs', the battery information are taken from the esc_status message. This requires the ESC to provide both voltage as well as current.</source>
      <translation>该参数控制电池数据的来源。值“电源模块”表示测量预计来自电源模块。如果该值设置为“外部”，则系统期望接收 mavlink 电池状态消息。如果该值设置为“ESCs”，则从 esc_status 消息中获取电池信息。这需要 ESC 提供电压和电流。</translation>
    </message>
    <message>
      <source>This parameter defines the maximum fuel capacity of the vehicle's fuel tank.</source>
      <translation>该参数定义了车辆油箱的最大燃油容量。</translation>
    </message>
    <message>
      <source>This parameter defines the minimum condition under which the system will command the RIGHT_OF_WAY lights on 0 - Always off 1 - When autopilot is armed 2 - When autopilot is prearmed 3 - Always on</source>
      <translation>该参数定义了系统命令 RIGHT_OF_WAY 灯亮起的最低条件 0 - 始终关闭 1 - 当自动驾驶仪解锁时 2 - 当自动驾驶仪预解锁时 3 - 始终打开</translation>
    </message>
    <message>
      <source>This parameter defines the rotation of the Mappydot sensor relative to the platform.</source>
      <translation>该参数定义Mappydot传感器相对于平台的旋转。</translation>
    </message>
    <message>
      <source>This parameter defines the yaw rotation of the optical flow relative to the vehicle body frame. Zero rotation is defined as X on flow board pointing towards front of vehicle.</source>
      <translation>该参数定义了光流相对于机体框架的偏航旋转。零旋转定义为流量板上的 X 指向车辆前部。</translation>
    </message>
    <message>
      <source>This parameter is not used by this vehicle. Always set to 0.</source>
      <translation>该车辆未使用该参数。始终设置为 0。</translation>
    </message>
    <message>
      <source>This parameter is used when using a rangefinder during landing for altitude correction from baro drift (RNGFND_LANDING=1) and the altitude correction indicates your actual altitude is higher than the intended slope path. Normally it would pitch down steeply but that can result in a crash with high airspeed so this allows remembering the baro offset and self-abort the landing and come around for another landing with the correct baro offset applied for a perfect slope. An auto-abort go-around will only happen once, next attempt will not auto-abort again. This operation happens entirely automatically in AUTO mode. This value is the delta degrees threshold to trigger the go-around compared to the original slope. Example: if set to 5 deg and the mission planned slope is 15 deg then if the new slope is 21 then it will go-around. Set to 0 to disable. Requires LAND_SLOPE_RCALC &gt; 0.</source>
      <translation>此参数用于在着陆期间使用测距仪进行气压漂移的高度校正 (RNGFND_LANDING=1)，并且高度校正表明您的实际高度高于预期的斜坡路径。通常情况下，它会急剧下降，但这可能会导致高空速坠毁，因此这可以记住气压偏移并自动中止着陆，并使用正确的气压偏移进行另一次着陆，以获得完美的坡度。自动中止复飞只会发生一次，下次尝试不会再次自动中止。该操作在 AUTO 模式下完全自动发生。该值是与原始坡度相比触发复飞的增量度阈值。示例：如果设置为 5 deg，并且任务计划坡度为 15 deg，则如果新坡度为 21，则它将复飞。设置为 0 以禁用。需要 LAND_SLOPE_RCALC &gt; 0。</translation>
    </message>
    <message>
      <source>This parameter sets the airspeed at which to stop holding the tail down and transition to rudder control of steering on the ground. When TKOFF_TDRAG_SPD1 is reached the pitch of the aircraft will be held level until TKOFF_ROTATE_SPD is reached, at which point the takeoff pitch specified in the mission will be used to "rotate" the pitch for takeoff climb. Set TKOFF_TDRAG_SPD1 to zero to go straight to rotation. This should be set to zero for hand launch and catapult launch. It should also be set to zero for tricycle undercarriages unless you are using the method above to gently hold the nose wheel down. For tail dragger aircraft it should be set just below the stall speed.</source>
      <translation>此参数设置停止保持机尾向下并过渡到地面转向的方向舵控制的空速。当达到 TKOFF_TDRAG_SPD1 时，飞机的俯仰角将保持水平，直到达到 TKOFF_ROTATE_SPD，此时任务中指定的起飞俯仰角将用于“旋转”起飞爬升的俯仰距。将 TKOFF_TDRAG_SPD1 设置为零以直接旋转。对于手动发射和弹射器发射，应将其设置为零。对于三轮车起落架，也应将其设置为零，除非您使用上述方法轻轻地按住前轮。对于尾拖式飞机，应将其设置为略低于失速速度。</translation>
    </message>
    <message>
      <source>This parameter sets the expected range measurement(in cm) that the second range finder should return when the vehicle is on the ground.</source>
      <translation>此参数设置当车辆在地面上时第二个测距仪应返回的预期距离测量值（以厘米为单位）。</translation>
    </message>
    <message>
      <source>This parameter sets the number of standard deviations applied to the GPS velocity measurement innovation consistency check. Decreasing it makes it more likely that good measurements willbe rejected. Increasing it makes it more likely that bad measurements will be accepted.</source>
      <translation>该参数设置应用于 GPS 速度测量创新一致性检查的标准偏差数。降低它会使好的测量更有可能被拒绝。增加它使得错误的测量更有可能被接受。</translation>
    </message>
    <message>
      <source>This parameter sets the size of the dead zone that is applied to negative baro height spikes that can occur when taking off or landing when a vehicle with lift rotors is operating in ground effect ground effect. Set to about 0.5m less than the amount of negative offset in baro height that occurs just prior to takeoff when lift motors are spooling up. Set to 0 if no ground effect is present.</source>
      <translation>此参数设置死区的大小，该死区应用于带有升力旋翼的飞行器在地面效应地面效应中运行时起飞或着陆时可能出现的负气压高度尖峰。设置为大约 0.5m，小于起飞前升力电机加速时发生的气压高度负偏移量。如果不存在地面效应，则设置为 0。</translation>
    </message>
    <message>
      <source>This parameter specifies the ADC channel used to monitor current of main power battery. A value of -1 means to use the board default.</source>
      <translation>该参数指定用于监控主动力电池电流的ADC通道。 -1 值表示使用板默认值。</translation>
    </message>
    <message>
      <source>This scales down the inner flaps so less than full downwards range can be used for differential spoiler and full span ailerons, 100 is use full range, upwards travel is unaffected</source>
      <translation>这会缩小内部襟翼，因此小于完整的向下范围可用于差速扰流板和全展副翼，100 使用全范围，向上行程不受影响</translation>
    </message>
    <message>
      <source>This selects the mode to start in on boot. This is useful for when you want to start in AUTO mode on boot without a receiver.</source>
      <translation>这将选择启动时启动的模式。当您想要在没有接收器的情况下启动时以 AUTO 模式启动时，这非常有用。</translation>
    </message>
    <message>
      <source>This selects which barometer will be the primary if multiple barometers are found</source>
      <translation>如果找到多个气压计，这将选择哪个气压计将成为主要气压计</translation>
    </message>
    <message>
      <source>This sets a digital output pin to set high when in manual mode.  See the Wiki's "GPIOs" page for how to determine the pin number for a given autopilot.</source>
      <translation>在手动模式下，这会将数字输出引脚设置为高电平。  请参阅 Wiki 的“GPIO”页面，了解如何确定给定自动驾驶仪的引脚号。</translation>
    </message>
    <message>
      <source>This sets one side of pass-through between two serial ports. Once both sides are set then all data received on either port will be passed to the other port</source>
      <translation>这设置了两个串行端口之间的直通的一侧。一旦双方都设置完毕，则任一端口接收到的所有数据都将传递到另一个端口</translation>
    </message>
    <message>
      <source>This sets the GPS horizontal position observation noise. Increasing it reduces the weighting of GPS horizontal position measurements.</source>
      <translation>这设置了 GPS 水平位置观测噪声。增加它会减少 GPS 水平位置测量的权重。</translation>
    </message>
    <message>
      <source>This sets the SBAS (satellite based augmentation system) mode if available on this GPS. If set to 2 then the SBAS mode is not changed in the GPS. Otherwise the GPS will be reconfigured to enable/disable SBAS. Disabling SBAS may be worthwhile in some parts of the world where an SBAS signal is available but the baseline is too long to be useful.</source>
      <translation>这将设置 SBAS（基于卫星的增强系统）模式（如果在此 GPS 上可用）。如果设置为 2，则 SBAS 模式在 GPS 中不会更改。否则，GPS 将被重新配置以启用/禁用 SBAS。在世界上某些有 SBAS 信号但基线太长而无用的地方，禁用 SBAS 可能是值得的。</translation>
    </message>
    <message>
      <source>This sets the amount of storage in kilobytes reserved on the microsd card in mission.stg for waypoint storage. Each waypoint uses 15 bytes.</source>
      <translation>这将设置在mission.stg 中的microsd 卡上为航路点存储保留的存储量（以千字节为单位）。每个航路点使用 15 字节。</translation>
    </message>
    <message>
      <source>This sets the compensation for the pitch angle trim difference between calibrated AHRS level and vertical flight pitch. NOTE! this is relative to calibrated AHRS trim, not forward flight trim which includes PTCH_TRIM_DEG. For tailsitters, this is relative to a baseline of 90 degrees in AHRS.</source>
      <translation>这设置了校准 AHRS 水平和垂直飞行俯仰之间的俯仰角配平差异的补偿。 NOTE！这是相对于校准的 AHRS 配平，而不是包括 PTCH_TRIM_DEG 的前向飞行配平。对于尾随者，这是相对于 AHRS 中 90 度数的基线。</translation>
    </message>
    <message>
      <source>This sets the inactivity timeout for the BLHeli protocol in seconds. If no packets are received in this time normal MAVLink operations are resumed. A value of 0 means no timeout</source>
      <translation>这设置了 BLHeli 协议的不活动超时（以秒为单位）。如果此时没有收到数据包，则恢复正常的 MAVLink 操作。 0 的值表示没有超时</translation>
    </message>
    <message>
      <source>This sets the maximum allowable difference between ground speed and airspeed. This allows the plane to cope with a failing airspeed sensor. A value of zero means to use the airspeed as is.</source>
      <translation>这设置了地速和空速之间的最大允许差异。这使得飞机能够应对空速传感器故障。值为零意味着按原样使用空速。</translation>
    </message>
    <message>
      <source>This sets the maximum number of log file that will be written on dataflash or sd card before starting to rotate log number. Limit is capped at 500 logs.</source>
      <translation>这设置了在开始轮换日志数量之前将写入数据闪存或 SD 卡上的日志文件的最大数量。限制上限为 500 日志。</translation>
    </message>
    <message>
      <source>This sets the min PWM output value in microseconds that will ever be output to the motors</source>
      <translation>这设置了将输出到电机的最小 PWM 输出值（以微秒为单位）</translation>
    </message>
    <message>
      <source>This sets the percentage number of standard deviations applied to the GPS position measurement innovation consistency check. Decreasing it makes it more likely that good measurements will be rejected. Increasing it makes it more likely that bad measurements will be accepted.</source>
      <translation>这设置了应用于 GPS 位置测量创新一致性检查的标准偏差的百分比数。降低它会使好的测量结果更有可能被拒绝。增加它使得错误的测量更有可能被接受。</translation>
    </message>
    <message>
      <source>This sets the percentage number of standard deviations applied to the magnetometer measurement innovation consistency check. Decreasing it makes it more likely that good measurements will be rejected. Increasing it makes it more likely that bad measurements will be accepted.</source>
      <translation>这设置了应用于磁力计测量创新一致性检查的标准偏差的百分比。降低它会使好的测量结果更有可能被拒绝。增加它使得错误的测量更有可能被接受。</translation>
    </message>
    <message>
      <source>This sets the range over which tuning will change a parameter. A value of 2 means the tuning parameter will go from 0.5 times the start value to 2x the start value over the range of the tuning channel</source>
      <translation>这设置了调整将更改参数的范围。 2 值表示在调谐通道范围内调谐参数将从 0.5 乘以起始值到 2 倍起始值</translation>
    </message>
    <message>
      <source>This sets the target IMU temperature for boards with controllable IMU heating units. A value of -1 disables heating.</source>
      <translation>这为具有可控 IMU 加热单元的电路板设置了目标 IMU 温度。 -1 的值会禁用加热。</translation>
    </message>
    <message>
      <source>This sets the time integral of airspeed innovation exceedance above ASPD_FS_INNOV required to trigger a failsafe.
Larger values make the check less sensitive, smaller positive values make it more sensitive.</source>
      <translation>这将空速创新超越的时间积分设置为触发故障保护所需的高于 ASPD_FS_INNOV。较大的值使检查不太敏感，较小的正值使其更敏感。</translation>
    </message>
    <message>
      <source>This sets up the board RGB LED for override by MAVLink. Normal notify LED control is disabled</source>
      <translation>这会将板 RGB LED 设置为由 MAVLink 覆盖。正常通知LED控制被禁用</translation>
    </message>
    <message>
      <source>This sets which types of external i2c barometer to look for. It is a bitmask of barometer types. The I2C buses to probe is based on BARO_EXT_BUS. If BARO_EXT_BUS is -1 then it will probe all external buses, otherwise it will probe just the bus number given in BARO_EXT_BUS.</source>
      <translation>这设置了要查找的外部 i2c 气压计类型。它是气压计类型的位掩码。用于探测的 I2C 总线基于 BARO_EXT_BUS。如果BARO_EXT_BUS是-1，那么它将探测所有外部总线，否则它将仅探测BARO_EXT_BUS中给出的总线号。</translation>
    </message>
    <message>
      <source>This state process noise controls the growth of earth magnetic field state error estimates. Increasing it makes earth magnetic field estimation faster and noisier.</source>
      <translation>这种状态过程噪声控制着地球磁场状态误差估计的增长。增加它会使地球磁场估计更快、噪音更大。</translation>
    </message>
    <message>
      <source>This value can be measured by weighting the quad on a scale.</source>
      <translation>该值可以通过在天平上对四边形进行加权来测量。</translation>
    </message>
    <message>
      <source>This value represents the Above Mean Sea Level (AMSL) altitude where the simulation begins. If using FlightGear as a visual animation, this value can be tweaked such that the vehicle lies on the ground at takeoff. LAT0, LON0, H0, MU_X, MU_Y, and MU_Z should ideally be consistent among each others to represent a physical ground location on Earth.</source>
      <translation>该值表示模拟开始处的平均海平面以上 (AMSL) 高度。如果使用 FlightGear 作为视觉动画，可以调整该值，使飞行器在起飞时位于地面上。理想情况下，LAT0、LON0、H0、MU_X、MU_Y 和 MU_Z 彼此之间应保持一致，以表示地球上的物理地面位置。</translation>
    </message>
    <message>
      <source>ThrLandControl-enable throttle stick control of landing rate</source>
      <translation>ThrLandControl-启用油门摇杆控制着陆速度</translation>
    </message>
    <message>
      <source>Threshold for oscillation detection. A lower value will lead to a more conservative tune.</source>
      <translation>振荡检测阈值。较低的值将导致更保守的调整。</translation>
    </message>
    <message>
      <source>Threshold for the arm switch</source>
      <translation>臂开关阈值</translation>
    </message>
    <message>
      <source>Throttle Accel kI</source>
      <translation>油门加速 kI</translation>
    </message>
    <message>
      <source>Throttle Failsafe Enable</source>
      <translation>油门故障安全启用</translation>
    </message>
    <message>
      <source>Throttle Slew Rate</source>
      <translation>油门转换率</translation>
    </message>
    <message>
      <source>Throttle acceleration controller P gain</source>
      <translation>油门加速控制器 P 增益</translation>
    </message>
    <message>
      <source>Throttle boost</source>
      <translation>油门助力</translation>
    </message>
    <message>
      <source>Throttle curve at position 1</source>
      <translation>1 位置油门曲线</translation>
    </message>
    <message>
      <source>Throttle feed forward from pitch</source>
      <translation>油门从俯仰前馈</translation>
    </message>
    <message>
      <source>Throttle limit max</source>
      <translation>油门极限最大</translation>
    </message>
    <message>
      <source>Throttle minimum</source>
      <translation>油门最小</translation>
    </message>
    <message>
      <source>Throttle percentage for engine idle</source>
      <translation>发动机怠速的油门百分比</translation>
    </message>
    <message>
      <source>Throttle used during FW-&gt;VTOL transition, -1 uses hover throttle</source>
      <translation>FW-&gt;VTOL过渡时使用油门，-1使用悬停油门</translation>
    </message>
    <message>
      <source>Throttle-gain boost</source>
      <translation>油门增益提升</translation>
    </message>
    <message>
      <source>Throw mode maximum altitude</source>
      <translation>投掷模式最大高度</translation>
    </message>
    <message>
      <source>Thrust Hover Value</source>
      <translation>推力悬停值</translation>
    </message>
    <message>
      <source>Thrust coefficient of rotor 4</source>
      <translation>转子推力系数4</translation>
    </message>
    <message>
      <source>Thrust compensation filter cutoff</source>
      <translation>推力补偿滤波器截止</translation>
    </message>
    <message>
      <source>Tide speed</source>
      <translation>潮汐速度</translation>
    </message>
    <message>
      <source>Tilt 2 is used for control</source>
      <translation>倾斜2用于控制</translation>
    </message>
    <message>
      <source>Tilt Servo 1 Tilt Angle at Minimum</source>
      <translation>倾斜舵机 1 最小倾斜角度</translation>
    </message>
    <message>
      <source>Tilt minimum angle for vectored yaw</source>
      <translation>矢量偏航的倾斜最小角度</translation>
    </message>
    <message>
      <source>TiltMotorsRear</source>
      <translation>倾斜电机后</translation>
    </message>
    <message>
      <source>Tiltrotor upwards tilt rate</source>
      <translation>倾转旋翼机向上倾斜率</translation>
    </message>
    <message>
      <source>Time based, always on</source>
      <translation>以时间为基础，永远在</translation>
    </message>
    <message>
      <source>Time constant of the TECS control algorithm. Small values make faster altitude corrections but can cause overshoot and aggressive behavior.</source>
      <translation>TECS控制算法的时间常数。小值可以更快地进行高度校正，但可能会导致超调和攻击行为。</translation>
    </message>
    <message>
      <source>Time for which vehicle continues descend even if target is lost. After this time period, vehicle will attempt a landing retry depending on PLND_STRICT parameter.</source>
      <translation>即使目标丢失，车辆仍持续下降的时间。在此时间段之后，飞行器将根据 PLND_STRICT 参数尝试着陆重试。</translation>
    </message>
    <message>
      <source>Time in seconds that EPM gripper will regrab the cargo to ensure grip has not weakened; 0 to disable</source>
      <translation>EPM 夹具重新抓取货物以确保抓力不会减弱的时间（以秒为单位）； 0 禁用</translation>
    </message>
    <message>
      <source>Time in seconds to spool up the motors from zero to min throttle. </source>
      <translation>将电机从零油门加速到最小油门所需的时间（以秒为单位）。 </translation>
    </message>
    <message>
      <source>Time taken to disable and enable the motor PWM output when disarmed and armed.</source>
      <translation>上锁和设防时禁用和启用电机 PWM 输出所需的时间。</translation>
    </message>
    <message>
      <source>Time to look ahead in the path to calculate rudder correction for bank angle</source>
      <translation>是时候向前看路径以计算倾斜角的舵修正了</translation>
    </message>
    <message>
      <source>Time-out for auto disarm after landing</source>
      <translation>落地后自动上锁超时</translation>
    </message>
    <message>
      <source>Timeout for activation of pilot input failsafe</source>
      <translation>激活先导输入故障保护超时</translation>
    </message>
    <message>
      <source>Timeout to allow the payload to execute the mission command</source>
      <translation>允许负载执行任务命令的超时</translation>
    </message>
    <message>
      <source>Tmode action 1</source>
      <translation>T模式动作 1</translation>
    </message>
    <message>
      <source>Tmode left action</source>
      <translation>T模式左动作</translation>
    </message>
    <message>
      <source>ToggleSimpleMode</source>
      <translation>切换简单模式</translation>
    </message>
    <message>
      <source>Toggles internal heading as an aiding measurement.
If dual antennas are supported (CV7-GNSS/INS). The filter will be configured to use dual antenna heading as an aiding measurement.</source>
      <translation>切换内部航向作为辅助测量。如果支持双天线（CV7-GNSS/INS）。滤波器将配置为使用双天线航向作为辅助测量。</translation>
    </message>
    <message>
      <source>Too low thrust leads to loss of roll/pitch/yaw torque control authority. With airmode enabled this parameters can be set to 0 while still keeping torque authority (see MC_AIRMODE).</source>
      <translation>推力过低会导致横滚/俯仰/偏航扭矩控制权丧失。启用空气模式后，可以将此参数设置为 0，同时仍保持扭矩权限（请参阅 MC_AIRMODE）。</translation>
    </message>
    <message>
      <source>Torqeedo Motor Power</source>
      <translation>扭矩电机功率</translation>
    </message>
    <message>
      <source>Torqeedo connection type</source>
      <translation>Torqeedo 连接类型</translation>
    </message>
    <message>
      <source>Total FlightTime (seconds)</source>
      <translation>总飞行时间（秒）</translation>
    </message>
    <message>
      <source>Total number of ADSB simulated aircraft</source>
      <translation>ADSB模拟飞机总数</translation>
    </message>
    <message>
      <source>Total runtime</source>
      <translation>总运行时间</translation>
    </message>
    <message>
      <source>Tracker will track targets at least this distance away</source>
      <translation>跟踪器将跟踪至少此距离外的目标</translation>
    </message>
    <message>
      <source>Transceiever disable</source>
      <translation>收发器禁用</translation>
    </message>
    <message>
      <source>Transition is also declared over if the groundspeed drops below MPC_XY_CRUISE.</source>
      <translation>如果地速降至 MPC_XY_CRUISE 以下，则也宣告转换结束。</translation>
    </message>
    <message>
      <source>Transmitter Tuning minum value. The parameter being tuned will have its value set to this minimum value when the tuning knob is at its lowest position</source>
      <translation>发射机调谐最小值。当调谐旋钮处于最低位置时，正在调谐的参数的值将设置为该最小值</translation>
    </message>
    <message>
      <source>Transmitter tuning parameter or set of parameters</source>
      <translation>发射机调谐参数或参数组</translation>
    </message>
    <message>
      <source>Trigger</source>
      <translation>触发源</translation>
    </message>
    <message>
      <source>Trik Count</source>
      <translation>技巧计数</translation>
    </message>
    <message>
      <source>Trim applied to simulated accelerometer (Z-axis)</source>
      <translation>应用于模拟加速度计（Z 轴）的微调</translation>
    </message>
    <message>
      <source>True airspeed measurement noise of the internal wind estimator(s) of the airspeed selector.</source>
      <translation>空速选择器内部风估计器的真实空速测量噪声。</translation>
    </message>
    <message>
      <source>Tuning Parameter for TRANSMITTER_TUNE2</source>
      <translation>TRANSMITTER_TUNE2 的调整参数</translation>
    </message>
    <message>
      <source>Turn radius of vehicle</source>
      <translation>车辆转弯半径</translation>
    </message>
    <message>
      <source>Twist y</source>
      <translation>扭动</translation>
    </message>
    <message>
      <source>Type of ADS-B hardware for ADSB-in and ADSB-out configuration and operation. If any type is selected then MAVLink based ADSB-in messages will always be enabled</source>
      <translation>用于 ADSB-in 和 ADSB-out 配置和操作的 ADS-B 硬件类型。如果选择任何类型，则将始终启用基于 MAVLink 的 ADSB-in 消息</translation>
    </message>
    <message>
      <source>Type of camera (0: None, 1: BMMCC)</source>
      <translation>相机类型（0：无，1：BMMCC）</translation>
    </message>
    <message>
      <source>Type of the parameter to be displayed and modified</source>
      <translation>显示和修改的参数类型</translation>
    </message>
    <message>
      <source>UART ESC ID 3 Spin Direction Flag</source>
      <translation>UART ESC ID 3 自旋方向标志</translation>
    </message>
    <message>
      <source>UART ESC Turtle Mode Crash Flip Motor Deadband</source>
      <translation>UART ESC 海龟模式碰撞翻转电机死区</translation>
    </message>
    <message>
      <source>UART byte loss percentage</source>
      <translation>UART 字节丢失百分比</translation>
    </message>
    <message>
      <source>UAVCAN CAN bus bitrate</source>
      <translation>UAVCAN CAN 总线比特率</translation>
    </message>
    <message>
      <source>UAVCAN ESC 2 Maximum Value</source>
      <translation>UAVCAN ESC 2 最大值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 4</source>
      <translation>UAVCAN ESC 4</translation>
    </message>
    <message>
      <source>UAVCAN ESC 5 Minimum Value</source>
      <translation>UAVCAN ESC 5 最小值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 7 Failsafe Value</source>
      <translation>UAVCAN ESC 7 故障安全值</translation>
    </message>
    <message>
      <source>UAVCAN ESC 8 Output Function</source>
      <translation>UAVCAN ESC 8 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN Servo 1 Maximum Value</source>
      <translation>UAVCAN 舵机 1 最大值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 2 Output Function</source>
      <translation>UAVCAN 舵机 2 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN Servo 4 Disarmed Value</source>
      <translation>UAVCAN 舵机 4 上锁值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 5 Maximum Value</source>
      <translation>UAVCAN 舵机 5 最大值</translation>
    </message>
    <message>
      <source>UAVCAN Servo 6 Output Function</source>
      <translation>UAVCAN 舵机 6 输出功能</translation>
    </message>
    <message>
      <source>UAVCAN Servo 8 Disarmed Value</source>
      <translation>UAVCAN 舵机 8 上锁值</translation>
    </message>
    <message>
      <source>UAVCAN node should be set implicitly</source>
      <translation>UAVCAN 节点应隐式设置</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 1</source>
      <translation>UAVCANv1 ESC 1</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 10 Minimum Value</source>
      <translation>UAVCANv1 ESC 10 最小值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 12 Failsafe Value</source>
      <translation>UAVCANv1 ESC 12 故障保护值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 13 Output Function</source>
      <translation>UAVCANv1 ESC 13 输出功能</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 15 Maximum Value</source>
      <translation>UAVCANv1 ESC 15 最大值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 2</source>
      <translation>UAVCANv1 ESC 2</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 3 Minimum Value</source>
      <translation>UAVCANv1 ESC 3 最小值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 5 Failsafe Value</source>
      <translation>UAVCANv1 ESC 5 故障安全值</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 6 Output Function</source>
      <translation>UAVCANv1 ESC 6 输出功能</translation>
    </message>
    <message>
      <source>UAVCANv1 ESC 8 Maximum Value</source>
      <translation>UAVCANv1 ESC 8 最大值</translation>
    </message>
    <message>
      <source>UBlox</source>
      <translation>UBlox</translation>
    </message>
    <message>
      <source>UDRAL battery status subscription port ID</source>
      <translation>UDRAL 电池状态订阅端口 ID</translation>
    </message>
    <message>
      <source>UUV Torque setpoint Saturation</source>
      <translation>UUV 扭矩设定值 饱和</translation>
    </message>
    <message>
      <source>UltraLight</source>
      <translation>超轻</translation>
    </message>
    <message>
      <source>UnicoreNMEA</source>
      <translation>联芯NMEA</translation>
    </message>
    <message>
      <source>Update at loop rate</source>
      <translation>以循环速率更新</translation>
    </message>
    <message>
      <source>Upper limit of random jitter in loop time</source>
      <translation>循环时间随机抖动上限</translation>
    </message>
    <message>
      <source>Use 420kbaud for ELRS protocol</source>
      <translation>对 ELRS 协议使用 420kbaud</translation>
    </message>
    <message>
      <source>Use Governor</source>
      <translation>使用调速器</translation>
    </message>
    <message>
      <source>Use RC_MAP_FAILSAFE to specify which channel is used to indicate RC loss via this threshold.
By default this is the throttle channel.
Set to a PWM value slightly above the PWM value for the channel (e.g. throttle) in a failsafe event,
but below the minimum PWM value for the channel during normal operation.
Note: The default value of 0 disables the feature (it is below the expected range).</source>
      <translation>使用 RC_MAP_FAILSAFE 指定使用哪个通道通过该阈值指示 RC 丢失。默认情况下，这是油门通道。设置为略高于故障安全事件中通道的 PWM 值（例如油门）的 PWM 值，但低于正常操作期间通道的最小 PWM 值。注意：0 的默认值禁用该功能（低于预期范围）。</translation>
    </message>
    <message>
      <source>Use a fixed wing approach for VTOL landings</source>
      <translation>使用固定翼进场进行 VTOL 着陆</translation>
    </message>
    <message>
      <source>Use dedicated CAN port b/w GPSes for moving baseline</source>
      <translation>使用专用 CAN 端口 b/w GPS 来移动基线</translation>
    </message>
    <message>
      <source>Use for vehicles where the measured body Z magnetic field is subject to strong magnetic interference. For magnetic heading fusion the magnetometer Z measurement will be replaced by a synthetic value calculated using the knowledge of the 3D magnetic field vector at the location of the drone. Therefore, this parameter will only have an effect if the global position of the drone is known. For 3D mag fusion the magnetometer Z measurement will simply be ignored instead of fusing the synthetic value.</source>
      <translation>用于被测体Z磁场受到强磁干扰的车辆。对于磁航向融合，磁力计 Z 测量将被使用无人机所在位置的 3D 磁场矢量知识计算出的合成值所取代。因此，只有在已知无人机的全球位置的情况下，该参数才会产生影响。对于 3D 磁融合，磁力计 Z 测量将被简单地忽略，而不是融合合成值。</translation>
    </message>
    <message>
      <source>Use it to arm/disarm via switch instead of default throttle stick. If this is assigned, arming and disarming via stick is disabled.</source>
      <translation>使用它通过开关而不是默认的油门杆来解锁/上锁。如果指定了此项，则禁用通过摇杆解锁和上锁。</translation>
    </message>
    <message>
      <source>Use primary</source>
      <translation>使用初级</translation>
    </message>
    <message>
      <source>Use third IMU for attitude, velocity and position estimates</source>
      <translation>使用第三个 IMU 进行姿态、速度和位置估计</translation>
    </message>
    <message>
      <source>UseBest</source>
      <translation>使用最佳</translation>
    </message>
    <message>
      <source>UseProximitySensor</source>
      <translation>使用接近传感器</translation>
    </message>
    <message>
      <source>Used by THROW mode. Controls whether motors will run at the speed set by THR_MIN or will be stopped when armed and waiting for the throw.</source>
      <translation>由 THROW 模式使用。控制电机是否按照 THR_MIN 设置的速度运行，或者在准备就绪并等待投掷时停止。</translation>
    </message>
    <message>
      <source>Used in autoland to give the minimum pitch in the final stage of landing (after the flare). This parameter can be used to ensure that the final landing attitude is appropriate for the type of undercarriage on the aircraft. Note that it is a minimum pitch only - the landing code will control pitch above this value to try to achieve the configured landing sink rate.</source>
      <translation>用于自动着陆，以给出着陆最后阶段（拉平后）的最小俯仰角。该参数可用于确保最终着陆姿态适合飞机起落架类型。请注意，这只是最小俯仰角 - 着陆代码将控制俯仰角高于该值，以尝试实现配置的着陆下沉率。</translation>
    </message>
    <message>
      <source>Used to calibrate fuel flow for MS protocol (Offset). This can be used to correct for a non-zero offset in the fuel consumption calculation of EFI_COEF1</source>
      <translation>用于校准 MS 协议的燃油流量（偏移）。这可用于校正 EFI_COEF1 燃油消耗计算中的非零偏移</translation>
    </message>
    <message>
      <source>Used to cap yaw rate setpoints and map controller inputs to yaw rate setpoints
in Acro, Stabilized and Position mode.</source>
      <translation>用于限制偏航率设定值并将控制器输入映射到特技、稳定和位置模式下的偏航率设定值。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT4_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATT4_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATT8_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATT8_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATTC_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATTC_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to convert the voltage of the voltage sensing pin (BATTG_VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT).</source>
      <translation>用于将电压检测引脚（BATTG_VOLT_PIN）的电压转换为实际电池电压（pin_电压 * VOLT_MULT）。</translation>
    </message>
    <message>
      <source>Used to linearly map speeds [m/s] to throttle values [-1. 1].</source>
      <translation>用于将速度 [m/s] 线性映射到油门值 [-1. 1]。</translation>
    </message>
    <message>
      <source>User provided ambient ground temperature in degrees Celsius. This is used to improve the calculation of the altitude the vehicle is at. This parameter is not persistent and will be reset to 0 every time the vehicle is rebooted. A value of 0 means use the internal measurement ambient temperature.</source>
      <translation>用户提供了环境地面温度（摄氏度）。这用于改进车辆所在高度的计算。该参数不是永久性的，每次车辆重新启动时都会重置为 0。 0 的值表示使用内部测量环境温度。</translation>
    </message>
    <message>
      <source>V-Tail</source>
      <translation>V型尾翼</translation>
    </message>
    <message>
      <source>VISION_POSITION_ESTIMATE</source>
      <translation>VISION_POSITION_ESTIMATE</translation>
    </message>
    <message>
      <source>VOXL ESC Output ESC 2</source>
      <translation>VOXL ESC 输出 ESC 2</translation>
    </message>
    <message>
      <source>VOXL(ModalAI)</source>
      <translation>VOXL(模态AI)</translation>
    </message>
    <message>
      <source>VOXL2 IO Output PWM Channel 4 Output Function</source>
      <translation>VOXL2 IO 输出 PWM 通道 4 输出功能</translation>
    </message>
    <message>
      <source>VOXL2 IO Output PWM Channel 8 Output Function</source>
      <translation>VOXL2 IO 输出 PWM 通道 8 输出功能</translation>
    </message>
    <message>
      <source>VR Micro Brain 5.2</source>
      <translation>VR 微脑 5.2</translation>
    </message>
    <message>
      <source>VRX BRAIN54</source>
      <translation>VRX BRAIN54</translation>
    </message>
    <message>
      <source>VTOL Input Pitch Angle</source>
      <translation>VTOL 输入俯仰角</translation>
    </message>
    <message>
      <source>VTOL Quad-rotor Tailsitter</source>
      <translation>VTOL 四旋翼尾翼</translation>
    </message>
    <message>
      <source>VTOL Standard (separate fixed rotors for hover and cruise flight)</source>
      <translation>VTOL标准型（用于悬停和巡航飞行的独立固定旋翼）</translation>
    </message>
    <message>
      <source>VTOL approach</source>
      <translation>VTOL方法</translation>
    </message>
    <message>
      <source>VTX_PWR_EN</source>
      <translation>VTX_PWR_EN</translation>
    </message>
    <message>
      <source>Value 0 turns the functionality off</source>
      <translation>值 0 关闭该功能</translation>
    </message>
    <message>
      <source>Vector Nav use uncompensated values for accel gyro and mag.</source>
      <translation>矢量导航对加速陀螺仪和磁力使用未补偿的值。</translation>
    </message>
    <message>
      <source>Vectored_6DOF_90</source>
      <translation>Vectored_6DOF_90</translation>
    </message>
    <message>
      <source>Vehicle inertia about Y axis</source>
      <translation>车辆绕Y轴的惯量</translation>
    </message>
    <message>
      <source>Vehicle will circle the center at this distance</source>
      <translation>车辆将在此距离绕中心旋转</translation>
    </message>
    <message>
      <source>VelXYI</source>
      <translation>维尔西伊</translation>
    </message>
    <message>
      <source>Velocity (horizontal) I gain.  Corrects long-term difference between desired and actual velocity to a target acceleration</source>
      <translation>我获得速度（水平）。  将期望速度和实际速度之间的长期差异校正为目标加速度</translation>
    </message>
    <message>
      <source>Velocity (horizontal) P gain. Converts the difference between desired and actual velocity to a target acceleration. Previously _VELXY_P.</source>
      <translation>速度（水平）P 增益。将期望速度与实际速度之间的差值转换为目标加速度。以前_VELXY_P。</translation>
    </message>
    <message>
      <source>Velocity (horizontal) input filter.  This filter (in Hz) is applied to the input for P and I terms</source>
      <translation>速度（水平）输入滤波器。  该滤波器（以 Hz 为单位）应用于 P 和 I 项的输入</translation>
    </message>
    <message>
      <source>Velocity (vertical) P gain.  Converts the difference between desired vertical speed and actual speed into a desired acceleration that is passed to the throttle acceleration controller</source>
      <translation>速度（垂直）P 增益。  将所需垂直速度与实际速度之间的差值转换为传递至油门加速度控制器的所需加速度</translation>
    </message>
    <message>
      <source>Velocity (vertical) controller I gain maximum</source>
      <translation>速度（垂直）控制器 I 增益最大值</translation>
    </message>
    <message>
      <source>Velocity (vertical) error filter</source>
      <translation>速度（垂直）误差滤波器</translation>
    </message>
    <message>
      <source>Velocity Horizontal Source (Secondary)</source>
      <translation>水平速度源（二次）</translation>
    </message>
    <message>
      <source>Velocity below this threshold with accompanying throttle demand triggers crash detection. Zero disables velocity check.</source>
      <translation>低于此阈值的速度以及伴随的油门需求会触发碰撞检测。零禁用速度检查。</translation>
    </message>
    <message>
      <source>Velocity xy P gain used to hold position during Max Gain, Rate P, and Rate D frequency sweeps</source>
      <translation>速度 xy P 增益用于在最大增益、速率 P 和速率 D 频率扫描期间保持位置</translation>
    </message>
    <message>
      <source>Vertical acceleration in m/s/s used during missions</source>
      <translation>任务期间使用的垂直加速度（以 m/s/s 为单位）</translation>
    </message>
    <message>
      <source>Vertical position on screen (MSP OSD only)</source>
      <translation>屏幕上的垂直位置（仅限 MSP OSD）</translation>
    </message>
    <message>
      <source>Vertical velocity threshold for sensitivity reduction</source>
      <translation>灵敏度降低的垂直速度阈值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 0 Output Function</source>
      <translation>Vertiq IO CVI 0 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 10 Disarmed Value</source>
      <translation>Vertiq IO CVI 10 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 11 Maximum Value</source>
      <translation>Vertiq IO CVI 11 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 12 Output Function</source>
      <translation>Vertiq IO CVI 12 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 14 Disarmed Value</source>
      <translation>Vertiq IO CVI 14 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 15 Maximum Value</source>
      <translation>Vertiq IO CVI 15 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 2 Output Function</source>
      <translation>Vertiq IO CVI 2 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 4 Disarmed Value</source>
      <translation>Vertiq IO CVI 4 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 5 Maximum Value</source>
      <translation>Vertiq IO CVI 5 最大值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 6 Output Function</source>
      <translation>Vertiq IO CVI 6 输出功能</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 8 Disarmed Value</source>
      <translation>Vertiq IO CVI 8 上锁值</translation>
    </message>
    <message>
      <source>Vertiq IO CVI 9 Maximum Value</source>
      <translation>Vertiq IO CVI 9 最大值</translation>
    </message>
    <message>
      <source>Very Strict</source>
      <translation>非常严格</translation>
    </message>
    <message>
      <source>Video Stabilization</source>
      <translation>视频稳定</translation>
    </message>
    <message>
      <source>Video Transmitter Options. Pitmode puts the VTX in a low power state. Unlocked enables certain restricted frequencies and power levels. Do not enable the Unlocked option unless you have appropriate permissions in your jurisdiction to transmit at high power levels.</source>
      <translation>视频发射器选项。 Pitmode 将 VTX 置于低功耗状态。解锁可启用某些受限制的频率和功率级别。除非您在您的管辖范围内拥有以高功率级别进行传输的适当权限，否则请勿启用“解锁”选项。</translation>
    </message>
    <message>
      <source>Video stream IP Address third octet</source>
      <translation>视频流 IP 地址第三个八位字节</translation>
    </message>
    <message>
      <source>Video stream id</source>
      <translation>视频流id</translation>
    </message>
    <message>
      <source>ViewPro Zoom Speed</source>
      <translation>ViewPro 变焦速度</translation>
    </message>
    <message>
      <source>Vision delay compensation</source>
      <translation>视力延迟补偿</translation>
    </message>
    <message>
      <source>Visual odometry camera Z position offset</source>
      <translation>视觉里程计相机Z位置偏移</translation>
    </message>
    <message>
      <source>Visual odometry scaling factor</source>
      <translation>视觉里程计比例因子</translation>
    </message>
    <message>
      <source>Visual odometry yaw measurement noise minimum (radians), This value will be used if the sensor provides a lower noise value (or no noise value)</source>
      <translation>视觉里程计偏航测量噪声最小值（弧度），如果传感器提供较低噪声值（或无噪声值），将使用该值</translation>
    </message>
    <message>
      <source>Voltage offset</source>
      <translation>电压偏移</translation>
    </message>
    <message>
      <source>Volz override mask</source>
      <translation>Volz 覆盖掩码</translation>
    </message>
    <message>
      <source>WARNING: Range finder measurements are less reliable and can experience unexpected errors. For these reasons, if accurate control of height relative to ground is required, it is recommended to use the MPC_ALT_MODE parameter instead, unless baro errors are severe enough to cause problems with landing and takeoff. If this parameter is enabled then the estimator will make use of the range finder measurements to estimate its height in addition to other height sources (if activated). Range sensor aiding can be enabled (i.e.: always use) or set in "conditional" mode. Conditional mode: This enables the range finder to be used during low speed (&lt; EKF2_RNG_A_VMAX) and low altitude (&lt; EKF2_RNG_A_HMAX) operation, eg takeoff and landing, where baro interference from rotor wash is excessive and can corrupt EKF state estimates. It is intended to be used where a vertical takeoff and landing is performed, and horizontal flight does not occur until above EKF2_RNG_A_HMAX.</source>
      <translation>WARNING：测距仪测量不太可靠，并且可能会出现意外错误。由于这些原因，如果需要精确控制相对于地面的高度，建议改用 MPC_ALT_MODE 参数，除非气压误差严重到足以导致着陆和起飞问题。如果启用此参数，则除了其他高度源（如果激活）之外，估计器还将利用测距仪测量来估计其高度。可以启用范围传感器辅助（即：始终使用）或设置为“条件”模式。条件模式：这使得测距仪能够在低速（&lt; EKF2_RNG_A_VMAX）和低高度（&lt; EKF2_RNG_A_HMAX）操作期间使用，例如起飞和着陆，其中旋翼清洗造成的气压干扰过大，可能会破坏 EKF 状态估计。它旨在用于进行垂直起飞和着陆，并且直到EKF2_RNG_A_HMAX以上才发生水平飞行的地方。</translation>
    </message>
    <message>
      <source>WAYPOINT_EN</source>
      <translation>WAYPOINT_EN</translation>
    </message>
    <message>
      <source>WP Speed (m/s)</source>
      <translation>WP 速度（米/秒）</translation>
    </message>
    <message>
      <source>Warn Only</source>
      <translation>仅警告</translation>
    </message>
    <message>
      <source>Wave direction</source>
      <translation>波向</translation>
    </message>
    <message>
      <source>Waypoint Acceleration </source>
      <translation>航点加速 </translation>
    </message>
    <message>
      <source>Waypoint Pivot Angle</source>
      <translation>航路点枢轴角度</translation>
    </message>
    <message>
      <source>Waypoint jerk</source>
      <translation>航路点加加速度</translation>
    </message>
    <message>
      <source>Waypoint overshoot maximum in meters.  The vehicle will attempt to stay within this many meters of the track as it completes one waypoint and moves to the next.</source>
      <translation>航路点超调最大值（以米为单位）。  当车辆完成一个航路点并移动到下一个航路点时，它将尝试保持在距离轨道这么多米的范围内。</translation>
    </message>
    <message>
      <source>Weathervaning max vertical speed</source>
      <translation>风标最大垂直速度</translation>
    </message>
    <message>
      <source>Weighting applied to speed control</source>
      <translation>加权应用于速度控制</translation>
    </message>
    <message>
      <source>What to do on a failsafe event</source>
      <translation>发生故障安全事件时该怎么做</translation>
    </message>
    <message>
      <source>What type of rangefinder device that is connected</source>
      <translation>连接什么类型的测距仪设备</translation>
    </message>
    <message>
      <source>Wheel rate Target notch filter index</source>
      <translation>轮速 目标陷波滤波器指数</translation>
    </message>
    <message>
      <source>Wheel rate control P gain</source>
      <translation>车轮速率控制 P 增益</translation>
    </message>
    <message>
      <source>Wheel rate control feed forward gain.  Desired rate (in radians/sec) is multiplied by this constant and output to output (in the range -1 to +1)</source>
      <translation>轮速率控制前馈增益。  所需速率（以弧度/秒为单位）乘以该常数并输出到输出（在 -1 至 +1 范围内）</translation>
    </message>
    <message>
      <source>Wheel steering rate proportional gain</source>
      <translation>车轮转向速率比例增益</translation>
    </message>
    <message>
      <source>Wheel2's radius</source>
      <translation>车轮2的半径</translation>
    </message>
    <message>
      <source>When a rangefinder reading differs from the previous by more than this, it will be considered a glitch and will not be used. A value of zero disables this check.</source>
      <translation>当测距仪读数与之前的读数相差超过此值时，它将被视为故障并且不会被使用。值为零会禁用此检查。</translation>
    </message>
    <message>
      <source>When enabled the PID filter settings are automatically set based on INS_GYRO_FILTER</source>
      <translation>启用后，PID 滤波器设置将根据 INS_GYRO_FILTER 自动设置</translation>
    </message>
    <message>
      <source>When enabled, moving the sticks more than COM_RC_STICK_OV immediately gives control back to the pilot by switching to Position mode and if position is unavailable Altitude mode. Note: Only has an effect on multicopters, and VTOLs in multicopter mode.</source>
      <translation>启用后，将操纵杆移动超过 COM_RC_STICK_OV 会立即通过切换到位置模式将控制权交还给飞行员，如果位置不可用则返回高度模式。注意：仅对多旋翼飞行器和多旋翼飞行器模式下的 VTOL 产生影响。</translation>
    </message>
    <message>
      <source>When enabled, this uses the throttle input in auto-throttle modes to 'nudge' the throttle or airspeed to higher or lower values. When you have an airspeed sensor the nudge affects the target airspeed, so that throttle inputs above 50% will increase the target airspeed from AIRSPEED_CRUISE up to a maximum of AIRSPEED_MAX. When no airspeed sensor is enabled the throttle nudge will push up the target throttle for throttle inputs above 50%.</source>
      <translation>启用后，这会使用自动油门模式下的油门输入将油门或空速“微移”到更高或更低的值。当您有空速传感器时，微移会影响目标空速，因此高于 50% 的油门输入会将目标空速从 AIRSPEED_CRUISE 增加到最大 AIRSPEED_MAX。当没有启用空速传感器时，油门微移将推高目标油门，使油门输入高于 50%。</translation>
    </message>
    <message>
      <source>When height above ground is below this, the sink rate will be held at TECS_LAND_SINK. Use this to perform a hold-of manoeuvre when combined with small values for TECS_LAND_SINK.</source>
      <translation>当离地高度低于此值时，下沉率将保持在 TECS_LAND_SINK。当与 TECS_LAND_SINK 的小值结合使用时，使用它来执行保持机动。</translation>
    </message>
    <message>
      <source>When no blending is active, this defines the preferred GPS receiver instance.
The GPS selection logic waits until the primary receiver is available to
send data to the EKF even if a secondary instance is already available.
The secondary instance is then only used if the primary one times out.
To have an equal priority of all the instances, set this parameter to -1 and
the best receiver will be used.
This parameter has no effect if blending is active.</source>
      <translation>当没有混合处于活动状态时，这定义了首选 GPS 接收器实例。即使辅助实例已经可用，GPS 选择逻辑也会等待，直到主接收器可用才能将数据发送到 EKF。仅当主实例超时时才使用辅助实例。为了使所有实例具有相同的优先级，请将此参数设置为 -1，并将使用最好的接收器。如果混合处于活动状态，则此参数无效。</translation>
    </message>
    <message>
      <source>When nudging is enabled (see MPC_LAND_RC_HELP), this controls the maximum allowed horizontal displacement from the original landing point.</source>
      <translation>当启用微移时（参见 MPC_LAND_RC_HELP），这控制着距原始着陆点的最大允许水平位移。</translation>
    </message>
    <message>
      <source>When piloting manually, this parameter is only used in MPC_POS_MODE 4.</source>
      <translation>手动驾驶时，该参数仅在 MPC_POS_MODE 4 中使用。</translation>
    </message>
    <message>
      <source>When set to 1, geofence automatically enables after an auto takeoff and automatically disables at the beginning of an auto landing.  When on the ground before takeoff the fence is disabled.  When set to 2, the fence autoenables after an auto takeoff, but only disables the fence floor during landing. It is highly recommended to not use this option for line of sight flying and use a fence enable channel instead. When set to 3 the fence auto-enables when the vehicle is armed and disables when disarmed and arming will fail if the fence cannot be enabled or is outside the fence. Option 3 cannot be used with a non-zero FENCE_MINALT</source>
      <translation>当设置为 1 时，地理围栏在自动起飞后自动启用，并在自动着陆开始时自动禁用。  起飞前在地面上时，围栏将被禁用。  当设置为 2 时，围栏在自动起飞后自动启用，但仅在着陆期间禁用围栏地板。强烈建议不要使用此选项进行视线飞行，而使用栅栏启用通道。当设置为 3 时，围栏在车辆解锁时自动启用，在上锁时禁用，如果围栏无法启用或位于围栏外，则解锁将失败。选项 3 不能与非零 FENCE_MINALT 一起使用</translation>
    </message>
    <message>
      <source>When the aircraft is within this altitude of the home waypoint, while disarmed it will automatically update the home position. Set to 0 to continously reset it.</source>
      <translation>当飞行器处于返航点的该高度范围内时，在上锁状态下，它将自动更新返航点位置。设置为0可连续复位。</translation>
    </message>
    <message>
      <source>When this many consecutive samples are considered a glitch, we give up and accept the new reading. A value of zero disables this behaviour and glitched values are never accepted.</source>
      <translation>当如此多的连续样本被认为是一个故障时，我们放弃并接受新的读数。零值会禁用此行为，并且永远不会接受有故障的值。</translation>
    </message>
    <message>
      <source>Whether to overwrite or add to existing logging</source>
      <translation>是否覆盖或添加到现有日志记录</translation>
    </message>
    <message>
      <source>Which pin to use. Only used on type = PWM.</source>
      <translation>使用哪个引脚。仅用于类型 = PWM。</translation>
    </message>
    <message>
      <source>Winch Clutch</source>
      <translation>绞车离合器</translation>
    </message>
    <message>
      <source>Winch control I gain maximum</source>
      <translation>绞车控制我获得最大</translation>
    </message>
    <message>
      <source>Winch deploy or retract rate maximum</source>
      <translation>绞盘展开或缩回速率最大</translation>
    </message>
    <message>
      <source>Wind Vane Type</source>
      <translation>风向标类型</translation>
    </message>
    <message>
      <source>Wind coefficient up</source>
      <translation>风系数上升</translation>
    </message>
    <message>
      <source>Wind speed analog speed input pin for Modern Devices Wind Sensor rev. p</source>
      <translation>Modern Devices 风传感器版本的风速模拟速度输入引脚。 p</translation>
    </message>
    <message>
      <source>Wind speed threshold above which an automatic failsafe action is triggered. Failsafe action can be specified with COM_WIND_MAX_ACT.</source>
      <translation>风速阈值，高于该阈值将触发自动故障安全操作。故障保护动作可以通过 COM_WIND_MAX_ACT 指定。</translation>
    </message>
    <message>
      <source>Wind vane direction will be ignored when apparent wind speeds are below this value (if wind speed sensor is present).  If the apparent wind is consistently below this value the vane will not work</source>
      <translation>当表观风速低于该值时（如果有风速传感器），风向标方向将被忽略。  如果视风始终低于该值，风向标将不起作用</translation>
    </message>
    <message>
      <source>Wind velocity from east direction</source>
      <translation>东风风速</translation>
    </message>
    <message>
      <source>WingSailElevator</source>
      <translation>翼帆电梯</translation>
    </message>
    <message>
      <source>X acceleration in meters/second/second to use to trigger the motor start in auto mode. If set to zero then auto throttle starts immediately when the mode switch happens, otherwise the rover waits for the X acceleration to go above this value before it will start the motor</source>
      <translation>X 加速度（以米/秒/秒为单位）用于在自动模式下触发电机启动。如果设置为零，则在模式切换发生时自动油门立即启动，否则流动站会等待 X 加速度高于此值，然后才会启动电机</translation>
    </message>
    <message>
      <source>X position of the base (primary) GPS antenna in body frame from the position of the 2nd antenna. Positive X is forward of the 2nd antenna. Use antenna phase centroid location if provided by the manufacturer.</source>
      <translation>底座（主）GPS 天线在机身框架中的 X 位置（距第二天线的位置）。正 X 位于第二个天线的前方。如果制造商提供，请使用天线相位质心位置。</translation>
    </message>
    <message>
      <source>X position of the first IMU Accelerometer in body frame. Positive X is forward of the origin. Attention: The IMU should be located as close to the vehicle c.g. as practical so that the value of this parameter is minimised. Failure to do so can result in noisy navigation velocity measurements due to vibration and IMU gyro noise. If the IMU cannot be moved and velocity noise is a problem, a location closer to the IMU can be used as the body frame origin.</source>
      <translation>机体框架中第一个 IMU 加速度计的 X 位置。正 X 位于原点前方。注意：IMU 应尽可能靠近车辆 c.g.。根据实际情况，使该参数的值最小化。否则，可能会因振动和 IMU 陀螺仪噪声而导致导航速度测量产生噪声。如果 IMU 无法移动并且速度噪声成为问题，则可以使用更靠近 IMU 的位置作为机体框架原点。</translation>
    </message>
    <message>
      <source>X position of the second rangefinder in body frame. Positive X is forward of the origin. Use the zero range datum point if supplied.</source>
      <translation>机身框架中第二个测距仪的 X 位置。正 X 位于原点前方。使用零范围基准点（如果提供）。</translation>
    </message>
    <message>
      <source>XFRobot</source>
      <translation>XF机器人</translation>
    </message>
    <message>
      <source>XYZ position of the optical flow sensor focal point relative to the body frame origin (X-axis)</source>
      <translation>XYZ 光流传感器焦点相对于机身原点（X 轴）的位置</translation>
    </message>
    <message>
      <source>Y axis optical flow scale factor correction</source>
      <translation>Y轴光流比例因子校正</translation>
    </message>
    <message>
      <source>Y position of the base (primary) GPS antenna in body frame from the position of the 2nd antenna. Positive Y is to the right of the 2nd antenna. Use antenna phase centroid location if provided by the manufacturer.This is renamed in 4.6 and later to GPSx_MB_OFS_Y.</source>
      <translation>底座（主）GPS 天线在机身框架中相对于第二根天线位置的 Y 位置。正 Y 位于第二个天线的右侧。如果制造商提供，则使用天线相位质心位置。这在 4.6 中重命名，后来更名为 GPSx_MB_OFS_Y。</translation>
    </message>
    <message>
      <source>Y position of the first rangefinder in body frame. Positive Y is to the right of the origin. Use the zero range datum point if supplied.</source>
      <translation>机身框架中第一个测距仪的 Y 位置。正 Y 位于原点右侧。使用零范围基准点（如果提供）。</translation>
    </message>
    <message>
      <source>Y position of the third IMU accelerometer in body frame. Positive Y is to the right of the origin. Attention: The IMU should be located as close to the vehicle c.g. as practical so that the value of this parameter is minimised. Failure to do so can result in noisy navigation velocity measurements due to vibration and IMU gyro noise. If the IMU cannot be moved and velocity noise is a problem, a location closer to the IMU can be used as the body frame origin.</source>
      <translation>机体框架中第三个 IMU 加速度计的 Y 位置。正 Y 位于原点右侧。注意：IMU 应尽可能靠近车辆 c.g.。根据实际情况，使该参数的值最小化。否则，可能会因振动和 IMU 陀螺仪噪声而导致导航速度测量产生噪声。如果 IMU 无法移动并且速度噪声成为问题，则可以使用更靠近 IMU 的位置作为机体框架原点。</translation>
    </message>
    <message>
      <source>Yaw</source>
      <translation>偏航</translation>
    </message>
    <message>
      <source>Yaw Angle Range</source>
      <translation>偏航角范围</translation>
    </message>
    <message>
      <source>Yaw Source</source>
      <translation>偏航源</translation>
    </message>
    <message>
      <source>Yaw angle from body x-axis to sensor x-axis.</source>
      <translation>从身体 x 轴到传感器 x 轴的偏航角。</translation>
    </message>
    <message>
      <source>Yaw axis controller I gain.  Corrects long-term difference in desired yaw angle (heading) vs actual yaw angle</source>
      <translation>偏航轴控制器 I 增益。  纠正所需偏航角（航向）与实际偏航角的长期差异</translation>
    </message>
    <message>
      <source>Yaw axis controller target frequency in Hz</source>
      <translation>偏航轴控制器目标频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Yaw axis rate controller I gain.  Corrects long-term difference in desired yaw rate vs actual yaw rate</source>
      <translation>偏航轴速率控制器 I 增益。  纠正期望偏航率与实际偏航率的长期差异</translation>
    </message>
    <message>
      <source>Yaw axis rate controller error frequency in Hz</source>
      <translation>偏航轴速率控制器错误频率（以 Hz 为单位）</translation>
    </message>
    <message>
      <source>Yaw channel number. This is useful when you have a RC transmitter that can't change the channel order easily. Yaw (also known as rudder) is normally on channel 4, but you can move it to any channel with this parameter.  Reboot is required for changes to take effect.</source>
      <translation>偏航通道数。当您的 RC 发射机无法轻松更改通道顺序时，这非常有用。偏航（也称为方向舵）通常位于通道 4 上，但您可以使用此参数将其移动到任何通道。  需要重新启动才能使更改生效。</translation>
    </message>
    <message>
      <source>Yaw differential thrust factor in forward flight</source>
      <translation>向前飞行时的偏航差推力因数</translation>
    </message>
    <message>
      <source>Yaw measurement gate size</source>
      <translation>偏航测量门尺寸</translation>
    </message>
    <message>
      <source>Yaw rate controller gain</source>
      <translation>偏航率控制器增益</translation>
    </message>
    <message>
      <source>Yaw rate for on/off servos</source>
      <translation>开/关舵机系统的偏航率</translation>
    </message>
    <message>
      <source>Yaw rate proportional gain</source>
      <translation>偏航率比例增益</translation>
    </message>
    <message>
      <source>Yaw stick gain for Manual mode</source>
      <translation>手动模式的偏航杆增益</translation>
    </message>
    <message>
      <source>Yaw135Roll270</source>
      <translation>偏航135横滚270</translation>
    </message>
    <message>
      <source>Yaw270Roll90</source>
      <translation>偏航270横滚90</translation>
    </message>
    <message>
      <source>Yaw45Roll90</source>
      <translation>偏航45横滚90</translation>
    </message>
    <message>
      <source>Yawh proportional gain</source>
      <translation>偏航比例增益</translation>
    </message>
    <message>
      <source>Z position of optical flow focal point in body frame</source>
      <translation>光流焦点在机身框架中的Z位置</translation>
    </message>
    <message>
      <source>Z position of the first GPS antenna in body frame. Positive Z is down from the origin. Use antenna phase centroid location if provided by the manufacturer.</source>
      <translation>机身框架中第一根 GPS 天线的 Z 位置。正 Z 轴从原点向下。如果制造商提供，请使用天线相位质心位置。</translation>
    </message>
    <message>
      <source>Z position of the second GPS antenna in body frame. Positive Z is down from the origin. Use antenna phase centroid location if provided by the manufacturer.Renamed in 4.6 and later to GPS2_POS_Z.</source>
      <translation>机身框架中第二个 GPS 天线的 Z 位置。正 Z 轴从原点向下。如果制造商提供，请使用天线相位质心位置。在 4.6 中重命名，后来更名为 GPS2_POS_Z。</translation>
    </message>
    <message>
      <source>Zero thrust collective</source>
      <translation>零推力总距</translation>
    </message>
    <message>
      <source>a0 in polynomial of form temperature in deg = a0 + a1*voltage + a2*voltage^2 + a3*voltage^3 + a4*voltage^4</source>
      <translation>a0 以度为单位的温度多项式 = a0 + a1*电压 + a2*电压^2 + a3*电压^3 + a4*电压^4</translation>
    </message>
    <message>
      <source>a4 in polynomial of form temperature in deg = a0 + a1*voltage + a2*voltage^2 + a3*voltage^3 + a4*voltage^4</source>
      <translation>a4 以度为单位的温度多项式 = a0 + a1*电压 + a2*电压^2 + a3*电压^3 + a4*电压^4</translation>
    </message>
    <message>
      <source>actuator_1_max_momentary</source>
      <translation>Actuator_1_max_momentary</translation>
    </message>
    <message>
      <source>actuator_2_max</source>
      <translation>执行器_2_max</translation>
    </message>
    <message>
      <source>actuator_3_inc</source>
      <translation>执行器_3_inc</translation>
    </message>
    <message>
      <source>actuator_4_dec</source>
      <translation>执行器_4_dec</translation>
    </message>
    <message>
      <source>actuator_5_center</source>
      <translation>执行器_5_center</translation>
    </message>
    <message>
      <source>actuator_5_min_toggle</source>
      <translation>Actuator_5_min_toggle</translation>
    </message>
    <message>
      <source>actuator_6_min_momentary</source>
      <translation>Actuator_6_min_momentary</translation>
    </message>
    <message>
      <source>altitude AMSL of the drag reference test</source>
      <translation>阻力参考测试海拔AMSL</translation>
    </message>
    <message>
      <source>apparent Wind vane direction low pass filter frequency, a value of -1 disables filter</source>
      <translation>表观风向标方向低通滤波器频率，-1 的值禁用滤波器</translation>
    </message>
    <message>
      <source>away from home</source>
      <translation>远离家乡</translation>
    </message>
    <message>
      <source>battery current in the drag reference test</source>
      <translation>拖动参考测试中的电池电流</translation>
    </message>
    <message>
      <source>bluff body drag coefficient</source>
      <translation>钝体阻力系数</translation>
    </message>
    <message>
      <source>center of lift</source>
      <translation>电梯中心</translation>
    </message>
    <message>
      <source>custom_1</source>
      <translation>定制_1</translation>
    </message>
    <message>
      <source>debug level</source>
      <translation>调试级别</translation>
    </message>
    <message>
      <source>disabled</source>
      <translation>残疾人</translation>
    </message>
    <message>
      <source>drag in sidewards direction</source>
      <translation>向侧面方向拖动</translation>
    </message>
    <message>
      <source>enable battery info support</source>
      <translation>启用电池信息支持</translation>
    </message>
    <message>
      <source>enter_mode_2</source>
      <translation>输入模式_2</translation>
    </message>
    <message>
      <source>extvisionmin</source>
      <translation>最小视野</translation>
    </message>
    <message>
      <source>free lift rate</source>
      <translation>自由提升率</translation>
    </message>
    <message>
      <source>fuse land detector</source>
      <translation>熔断器接地探测器</translation>
    </message>
    <message>
      <source>gimbal</source>
      <translation>万向节</translation>
    </message>
    <message>
      <source>hover throttle</source>
      <translation>悬停油门</translation>
    </message>
    <message>
      <source>lanes have to be consistently better than the primary by at least this threshold to reduce their overall relativeCoreError, lowering this makes lane switching more sensitive to smaller error differences</source>
      <translation>通道必须始终优于主通道至少此阈值，以减少其总体相对核心错误，降低此值会使通道切换对较小的误差差异更加敏感</translation>
    </message>
    <message>
      <source>lights2_cycle</source>
      <translation>灯2_cycle</translation>
    </message>
    <message>
      <source>log runtime memory usage and execution time</source>
      <translation>记录运行时内存使用情况和执行时间</translation>
    </message>
    <message>
      <source>mask of servo output channels to override with values from Volz protocol.  Note these are indexed from 0 - so channel 3 (usually throttle) has value 4 in this bitmask (1&lt;&lt;2).</source>
      <translation>使用 Volz 协议中的值覆盖的舵机输出通道掩码。  请注意，这些是从 0 索引的 - 因此通道 3（通常是油门）在此位掩码中具有值 4 (1&lt;&lt;2)。</translation>
    </message>
    <message>
      <source>maximum I applied the crosstrack/heading PID controller</source>
      <translation>最大我应用了横移/航向 PID 控制器</translation>
    </message>
    <message>
      <source>maximum motor PWM in the reference test, matching MOT_PWM_MAX</source>
      <translation>参考测试中最大电机PWM，匹配MOT_PWM_MAX</translation>
    </message>
    <message>
      <source>minimum PWM</source>
      <translation>最小 PWM</translation>
    </message>
    <message>
      <source>mode_acro</source>
      <translation>模式_acro</translation>
    </message>
    <message>
      <source>mode_surftrak</source>
      <translation>模式_surftrak</translation>
    </message>
    <message>
      <source>momentum drag coefficient of the rotors. Defaults to zero on planes where the plane model handles drag</source>
      <translation>转子的动量阻力系数。在平面模型处理拖动的平面上默认为零</translation>
    </message>
    <message>
      <source>monitor 8</source>
      <translation>监控8</translation>
    </message>
    <message>
      <source>mount joystick speed</source>
      <translation>安装操纵杆速度</translation>
    </message>
    <message>
      <source>mount_pan_left</source>
      <translation>向左安装平移</translation>
    </message>
    <message>
      <source>normal</source>
      <translation>正常</translation>
    </message>
    <message>
      <source>pan (yaw) RC input channel</source>
      <translation>平移（偏航）RC 输入通道</translation>
    </message>
    <message>
      <source>pin number to use for save accurate camera feedback messages. If set to -1 then don't use a pin flag for this, otherwise this is a pin number which if held high after a picture trigger order, will save camera messages when camera really takes a picture. A universal camera hot shoe is needed. The pin should be held high for at least 2 milliseconds for reliable trigger detection. See also the CAM_FEEDBACK_POL option.</source>
      <translation>用于保存准确的相机反馈消息的引脚号。如果设置为 -1，则不要为此使用引脚标志，否则这是一个引脚编号，如果在图片触发命令后保持高电平，将在相机真正拍照时保存相机消息。需要一个通用相机热靴。该引脚应保持高电平至少 2 毫秒，以实现可靠的触发检测。另请参见 CAM_FEEDBACK_POL 选项。</translation>
    </message>
    <message>
      <source>pub agl as lpos down</source>
      <translation>pub agl 为 lpos down</translation>
    </message>
    <message>
      <source>rangefinder offset</source>
      <translation>测距仪偏移</translation>
    </message>
    <message>
      <source>reference speed</source>
      <translation>参考速度</translation>
    </message>
    <message>
      <source>relay_2_momentary</source>
      <translation>继电器_2_瞬时</translation>
    </message>
    <message>
      <source>relay_4_momentary</source>
      <translation>继电器_4_瞬时</translation>
    </message>
    <message>
      <source>roll RC input channel</source>
      <translation>横滚RC输入通道</translation>
    </message>
    <message>
      <source>sample count per batch</source>
      <translation>每批次样本数</translation>
    </message>
    <message>
      <source>scaling factors applied to simulated accelerometer (Y-axis)</source>
      <translation>应用于模拟加速度计的缩放因子（Y 轴）</translation>
    </message>
    <message>
      <source>script_3</source>
      <translation>脚本_3</translation>
    </message>
    <message>
      <source>sensor_gps uORB over Cyphal subscription port ID</source>
      <translation>通过 Cyphal 订阅端口 ID 的sensor_gps uORB</translation>
    </message>
    <message>
      <source>servo_1_max</source>
      <translation>舵机_1_max</translation>
    </message>
    <message>
      <source>servo_2_inc</source>
      <translation>舵机_2_inc</translation>
    </message>
    <message>
      <source>servo_3_dec</source>
      <translation>舵机_3_dec</translation>
    </message>
    <message>
      <source>set roll pitch and yaw trim to current servo and RC</source>
      <translation>将横滚俯仰和偏航微调设置为当前舵机和 RC</translation>
    </message>
    <message>
      <source>simulated barometer temperature offset</source>
      <translation>模拟气压计温度偏移</translation>
    </message>
    <message>
      <source>sphere</source>
      <translation>球体</translation>
    </message>
    <message>
      <source>subscription IMU</source>
      <translation>认购IMU</translation>
    </message>
    <message>
      <source>subscription hygrometer</source>
      <translation>订阅湿度计</translation>
    </message>
    <message>
      <source>temperature required for generator to return to idle after having run</source>
      <translation>发电机运行后返回怠速所需的温度</translation>
    </message>
    <message>
      <source>terrain brake home distance. The distance from home where the auto BRAKE will be enabled. When within this distance of home the script will not activate</source>
      <translation>地形制动距离。将启用自动 BRAKE 的距家的距离。当离家的距离在这个范围内时，脚本将不会激活</translation>
    </message>
    <message>
      <source>this controls the time over which wind changes take effect</source>
      <translation>这控制了风向变化生效的时间</translation>
    </message>
    <message>
      <source>throttle output (CTUN.ThO) at hover at the reference altitude</source>
      <translation>悬停在参考高度时的油门输出（CTUN.ThO）</translation>
    </message>
    <message>
      <source>tiltMotorRight</source>
      <translation>倾斜电机向右</translation>
    </message>
    <message>
      <source>towards waypoint</source>
      <translation>朝向航路点</translation>
    </message>
    <message>
      <source>trim_roll_inc</source>
      <translation>微调卷轴公司</translation>
    </message>
    <message>
      <source>u-blox GPS jamming detection high sensitivity mode</source>
      <translation>u-blox GPS 干扰检测高灵敏度模式</translation>
    </message>
    <message>
      <source>uAvionix-MAVLink</source>
      <translation>uAvionix-MAVLink</translation>
    </message>
    <message>
      <source>uXRCE-DDS domain ID</source>
      <translation>uXRCE-DDS 域 ID</translation>
    </message>
    <message>
      <source>use Custom Controller</source>
      <translation>使用自定义控制器</translation>
    </message>
    <message>
      <source>vert speed</source>
      <translation>垂直速度</translation>
    </message>
    <message>
      <source>when armed until disarm (default)</source>
      <translation>解锁至上锁（默认）</translation>
    </message>
  </context>
</TS>
