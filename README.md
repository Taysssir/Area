V2/Sentinel-Admin is the graphical user interface for administrator to interact with V2 evigilante robots.

#### Compile V2/sentinel-admin
```
git clone git@192.168.100.102:V2/sentinel-admin.git
cd sentinel-admin
#git submodule update --init --recursive
git submodule init && git submodule update --remote 
```
then compile it with qtcreator

## Package dependencies for interaction with a V2 evigilante
To have sentinel-admin run and interact with a robot (for example in localhost) you will need evigilante_remote. To get the source and compile it:
```
cd eos_ws/src
git clone git@192.168.100.102:V2/evigilante_remote.git
git clone git@192.168.100.102:V2/evigilante_sdk.git
git clone git@192.168.100.102:V2/evigilante_configuration.git
cd ..
catkin_make
```

## Run V2/Senstinel-Admin and V2 evigilante
In a terminal, run evigilante_remote to start V2 service
```
roslaunch evigilante_remote evigilante_remote.launch
```
The execute Sentinel-Admin with the ip address of the robot in the configuration box. Now you should be able to download/upload configuration with your V2 evigilante.
