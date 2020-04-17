libeosconfiguration
===================

libeosconfiguration - A library to load/create/update eos configuration file.

## Load a sentinel configuration

Handle models to parametize the robots : 
 - Patrols
 - Zones
 - Users
 - Settings
 - Sensors
 - Tasks Scheduled
 - Map (base_map, static_map, display_map)


#### 1. Install dependencies: 
```
sudo apt-get install libtinyxml-dev
```

the zlib lib is located direclty in this repository.


#### 2. Get the project and compile it
```
cd <your_workspace>
git clone 
cd libeosconfiguration
mkdir build
cd build
cmake ..
make
sudo make install
```

#### 2. Some example
```
// Get Patrols
eos::Configuration config;
config.open("test.eos");
eos::Patrols patrols = *config.patrols();
for(auto patrol : patrols)
    std::cout<<patrol.name()<<std::endl;
```
