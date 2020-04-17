eos e-vigilante SDK
===========

a framework design to communicate with our robot

#### 1. Install protobuf dependencies : 
```
protobuf 2.6
```
Try to just install libs like that :

```
sudo apt-get install libgflags-dev libgoogle-glog-dev
```
## Dictionary side 

### sdk data message

#### :heavy_check_mark: Protocol 

```
Protocol
```
- give default TCP communication port number 

#### :heavy_check_mark: Robot Info

```
RobotInfo
```
- give robot information
- contains URLs information for cameras and shared path  

#### :heavy_check_mark: Joystick Command

```
JoystickCommand
```
- Enable to send cartesian movement command to remote

#### :heavy_check_mark: turn Command

```
Turn
```
- Enable to send rotation command to remote

#### :heavy_check_mark: stop

```
Stop
```
- Command emmergency stop of remote

#### :heavy_check_mark: Dispel doubt 

```
MoveTo
```
- command given position dispel doubt to remote

#### :heavy_check_mark: Hooter 

```
Hooter
```
- command hooter activation or deactivation

#### :heavy_check_mark: Toggle charging 

```
ToggleCharging
```
- activate or deactivate remote docking state 


#### :heavy_check_mark: Toggle Power Supply 

```
ToggleComponentsPowerSupply
```
- activate or deactivate remote docking state 

#### :heavy_check_mark: Robot position 
```
RobotPosition
```
- update robot position values 

#### :heavy_check_mark: Robot position 
```
RobotPosition
```
- update robot position values

#### :heavy_check_mark: Hardware Issues 
```
HardwareIssue
```
- notify about hardware issues

#### :heavy_check_mark: Alarm
```
Alarm
```
- update a specific alarm type state

#### :heavy_check_mark: Battery Status
```
BatteryState
```
- update battery informations values

#### :heavy_check_mark: LaserShot
```
LaserShot
```
- data structure of lazer shot

#### :heavy_check_mark: Robot Monitoring
```
RobotMonitoring
```
- command robot scores values
- update lazer shot list management

#### :heavy_check_mark: Scores Threshold
```
ScoresThreshold
```
- command robot scores min/max values

#### :heavy_check_mark: Control Patrolling
```
ControlPatrolling
```
- command start and stop patrolling behaviours 
- contains patrolling name, time data values

#### :heavy_check_mark: Clear Dynamic Obstacle
```
ClearDynamicObstacle
```
- erase all defined dynamic obstacle  

#### :heavy_check_mark: Dynamic Obstacle
```
DynamicObstacle
```
- defines a particular Dynamic Obstacle 
- contains position, state and position  

#### :heavy_check_mark: Emergency Stop
```
EmergencyStop
```
- command Emergy stop  

#### :heavy_check_mark: Robot Sensor
```
RobotSensor
```
- Declares a particular sensors type
- Contains sensor location

#### :heavy_check_mark: Robot Sensors
```
RobotSensors
```
- Update sensor array list state

#### :heavy_check_mark: Map Availability
```
NewMapAvailable
```
- Notify client when new map is available

#### :heavy_check_mark: Robot Subscription
```
RobotDataSubscription
```
- Define client subscription data

#### :heavy_check_mark: Carto Order
```
CartoOrder
```

#### :heavy_check_mark: Carto State
```
CartoState
```

