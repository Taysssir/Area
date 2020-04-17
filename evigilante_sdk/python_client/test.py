from ctypes import *
import struct
import embisphere_pb2
import socket
import sys
import time
import numpy as np


client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('localhost', 55897))

VERSION         = 0
MAPINFO         = 1
CHANGE_SPEED_REQ= 2
CHANGE_SPEED_RES= 3
GET_PATROLS_REQ = 4
GET_PATROLS_RES = 5

START_PATROL_REQ= 8
START_PATROL_RES= 9

STOP_PATROL_REQ = 10
STOP_PATROL_RES = 11

ROBOT_STATE_RES = 15
PATROLLING_STATE= 16

def receive_msg():
    message = client.recv(4)
    if len(message) == 4:
        tmp = struct.unpack("i", message)
        size_of_packet = socket.htonl(tmp[0])
        packet = client.recv( size_of_packet )
        if len(packet) == size_of_packet:
            l = len(packet) - 4
            print "packet received :"
            print ":".join("{:02x}".format(ord(c)) for c in packet)
            index, msg = struct.unpack("i"+str(l)+"s", packet)
            print "index : ", socket.htonl(index)
        return (socket.htonl(index), msg)
    return (None, None)

def send_msg(msg, t):
    buf = msg.SerializeToString()
    l = len(buf) + 4
    l = socket.htonl(l)
    packet = struct.pack("ii"+str(len(buf))+"s", l, socket.htonl(t), buf)
    client.send(packet)
    #print len(buf)
    print "sent message :"
    print ":".join("{:02x}".format(ord(c)) for c in packet)

def send_request(messagetype):        
    if messagetype == 2:
        print " --- ChangeSpeed request---" # KO     
        instance = embisphere_pb2.ChangeSpeed()
        instance.timestamp = 100
        instance.speed = 2
     
    elif messagetype == 4 :
        print " --- GetPatrols request ---"  #OK      
        instance = embisphere_pb2.GetPatrols()
        instance.timestamp = 100

    elif messagetype == 6:
        print " --- TurnAround request---" #OK       
        instance = embisphere_pb2.TurnAround()
        instance.timestamp = 100
        instance.sens = 1

    elif messagetype == 8:
        print " --- StartPatrol request---" #OK
        instance = embisphere_pb2.StartPatrol()
        instance.name = "Patrol 1"
        instance.sens = 1
        instance.timestamp = 100 
 
    elif messagetype == 10:
        print " --- StopPatroling request---"#OK
        instance = embisphere_pb2.StopPatrolling()
        instance.timestamp = 100    
    
    elif messagetype == 12:
        print " --- GoBackDocking request---" #OK
        instance = embisphere_pb2.GoBackDocking()
        instance.timestamp = 100    

    send_msg(instance, messagetype)

def print_received_msg(index, msg) : 
    if index == 0:
        print " --- Version received---" #OK
        version = embisphere_pb2.Version()
        version.ParseFromString(msg)
        print "Version %d.%d.%d" % (version.MAJOR, version.MINOR, version.RELEASE)
    elif index == 1:
        print " --- MapInfo received---" #OK
        map_info = embisphere_pb2.MapInfo()
        map_info.ParseFromString(msg)
        print "resolution : %f, width %d, height : %d" %(map_info.resolution, map_info.width, map_info.height)
    elif index == 3:
        print " --- ChangeSpeed received---" #OK
        change_speed = embisphere_pb2.ChangeSpeedResponse()
        change_speed.ParseFromString(msg)
        print "speed : %f, " %(change_speed.result)
    elif index ==5:
        print " --- GetPatrols received---"   #OK     
        get_patrols = embisphere_pb2.GetPatrolsResponse()
        get_patrols.ParseFromString(msg)
        print "patrols : %s, " %(get_patrols.patrols)
    elif index == 7:
        print " --- TurnAround received---"      #OK  
        turn_around = embisphere_pb2.TurnAroundResponse()
        turn_around.ParseFromString(msg)
        #print "turn around : %d, " %(turn_around.sens)

    elif index ==9:
        print " --- StartPatrol received---" #OK
        start_patrol = embisphere_pb2.StartPatrolResponse()
        start_patrol.ParseFromString(msg)
        print "StartPatrol : %d %d" %(start_patrol.timestamp, start_patrol.result)

    elif index == 11:
        print " --- StopPatrol received---" #OK
        stop_patrolling = embisphere_pb2.StopPatrollingResponse()
        stop_patrolling.ParseFromString(msg)
        
    elif index == 13:
        print " --- GoBackDocking received---" #OK
        go_back_docking = embisphere_pb2.GoBackDocking()
        go_back_docking.ParseFromString(msg)
    elif index == 14:
        print " --- RobotPosition received---"
        robot_position = embisphere_pb2.RobotPosition()
        robot_position.ParseFromString(msg)
    elif index == 15:
        print " --- RobotState received---" #OK
        robot_state = embisphere_pb2.RobotState()
        robot_state.ParseFromString(msg)
        print "state %d, x %f, y %f, theta %f" % (robot_state.state, robot_state.position.x, robot_state.position.y, robot_state.position.theta)
    elif index == 16:
        print " --- PatrollingState received---" # KO
        patrolling_state = embisphere_pb2.PatrollingState()
        patrolling_state.ParseFromString(msg)
        print "patrolling_state : patrol_name %s , percent %f" % (patrolling_state.patrol_name, patrolling_state.percent_done)

    elif index == 17:
        print " --- RobotIssue received---" # KO              
        robot_issue = embisphere_pb2.RobotIssue()
        robot_issue.ParseFromString(msg) 
        print "Robot issue : issue %f, status %f" % (robot_issue.issue, robot_issue.status)

def wait_for(index_to_wait) :
    
    while True :  
        index, msg = receive_msg()
        print_received_msg(index,msg)
        if index_to_wait == index:
            return msg
            break


def wait_robot_state(state) : 
    while True :  
	msg = wait_for(ROBOT_STATE_RES)
	robot_state = embisphere_pb2.RobotState()
	robot_state.ParseFromString(msg)
	if robot_state.state == state :
            break;
   
    
def SCENARIO_WAIT_GET_PATROLS() : 
    send_request(4)
    msg = wait_for(5)

def SCENARIO_GET_PATROLS_AND_START_FIRST() : 
    send_request(4) 
    msg = wait_for(5)
    get_patrols = embisphere_pb2.GetPatrolsResponse()
    get_patrols.ParseFromString(msg)
    if 0 != len(get_patrols.patrols) :
        #get_patrols.patrols.reverse()
    	print " --- StartPatrol request--- %s"  % (get_patrols.patrols[0]) 
        instance = embisphere_pb2.StartPatrol()
	instance.name = get_patrols.patrols[0]
        instance.sens = 1
        instance.timestamp = 100
        send_msg(instance, 8)
         
    else : 
        print " --- NO Patrol PRESENT ---" 

def SCENARIO_MAP_INFO():
    wait_for(MAPINFO)
    

def SCENARIO_PATROLLING_GET_START_STOP() : 
    #index, msg = receive_msg()
    send_request(GET_PATROLS_REQ) 
    msg = wait_for(GET_PATROLS_RES)
    get_patrols = embisphere_pb2.GetPatrolsResponse()
    get_patrols.ParseFromString(msg)
    if 0 != len(get_patrols.patrols) :
        #get_patrols.patrols.reverse()
    	print " --- StartPatrol request--- %s"  % (get_patrols.patrols[0]) 
        instance = embisphere_pb2.StartPatrol()
	instance.name = get_patrols.patrols[0]
        instance.sens = 1
        instance.timestamp = 100
        send_msg(instance, START_PATROL_REQ)
        # RECEIVE START
        wait_for(START_PATROL_RES)
      	wait_robot_state(3) #PATROLLING       
	#SEND STOP PATROL
	time.sleep(60)
        print " " 

	send_request(STOP_PATROL_REQ)
        msg = wait_for(STOP_PATROL_RES)
       	wait_robot_state(0) #PATROLLING
    else : 
        print " --- NO Patrol PRESENT ---" 

def SCENARIO_CHANGE_SPEED() : 
    # REQUEST and RECEIVE ALL robot patrols 
    send_request(GET_PATROLS_REQ) 
    msg = wait_for(GET_PATROLS_RES)
    get_patrols = embisphere_pb2.GetPatrolsResponse()
    get_patrols.ParseFromString(msg)
    if 0 != len(get_patrols.patrols) :
        #get_patrols.patrols.reverse()
        # START FIRST PATROL 
    	print " --- StartPatrol request--- %s"  % (get_patrols.patrols[0]) 
        instance = embisphere_pb2.StartPatrol()
	instance.name = get_patrols.patrols[0]
        instance.sens = 1
        instance.timestamp = 100
        send_msg(instance, START_PATROL_REQ)
        wait_for(START_PATROL_RES)# RECEIVE START
      	wait_robot_state(3) #PATROLLING       
	#SEND STOP PATROL
	time.sleep(60)
	
	#change Robot speed from 0 to 100 step 10 by ten := {0, 10, 20 .. 100}
        for i in np.arange(11)*10 :
            changeSpeed = embisphere_pb2.ChangeSpeed()
            changeSpeed.timestamp = 101
            changeSpeed.speed = i      
            send_msg(changeSpeed, CHANGE_SPEED_REQ)
            wait_for(CHANGE_SPEED_RES)
            time.sleep(2)
     

	send_request(STOP_PATROL_REQ)
        msg = wait_for(STOP_PATROL_RES)
       	wait_robot_state(0) #PATROLLING
    else : 
        print " --- NO Patrol PRESENT ---" 

def SCENARIO_CHANGE_SPEED_AND_PATROL_SENS(sens) : 
    # REQUEST and RECEIVE ALL robot patrols 
    send_request(GET_PATROLS_REQ) 
    msg = wait_for(GET_PATROLS_RES)
    get_patrols = embisphere_pb2.GetPatrolsResponse()
    get_patrols.ParseFromString(msg)
    if 0 != len(get_patrols.patrols) :
        #get_patrols.patrols.reverse()
        # START FIRST PATROL 
    	print " --- StartPatrol request--- %s"  % (get_patrols.patrols[0]) 
        instance = embisphere_pb2.StartPatrol()
	instance.name = get_patrols.patrols[len(get_patrols.patrols) - 1]
        instance.sens = sens
        instance.timestamp = 100
        send_msg(instance, START_PATROL_REQ)
        wait_for(START_PATROL_RES)# RECEIVE START
      	wait_robot_state(3) #PATROLLING       
	#SEND STOP PATROL
	#time.sleep(60)
	for i in np.arange(60) :
	    wait_for(PATROLLING_STATE)# RECEIVE START
      	
	
	#change Robot speed from 0 to 100 step 10 by ten := {0, 10, 20 .. 100}
        for i in np.arange(11)*10 :
            changeSpeed = embisphere_pb2.ChangeSpeed()
            changeSpeed.timestamp = 101
            changeSpeed.speed = i      
            send_msg(changeSpeed, CHANGE_SPEED_REQ)
            wait_for(CHANGE_SPEED_RES)
            time.sleep(2)
     

	send_request(STOP_PATROL_REQ)
        msg = wait_for(STOP_PATROL_RES)
       	wait_robot_state(0) #PATROLLING
    else : 
        print " --- NO Patrol PRESENT ---" 


# wait_for(VERSION)
# wait_for(MAPINFO)
sens = 0


send_request(START_PATROL_REQ)
while 1:
    
 #   SCENARIO_CHANGE_SPEED_AND_PATROL_SENS(sens%2)
    sens = sens+1
    
    #index, msg = receive_msg()
    #print_received_msg(index,msg)

    #scenario patrolling
    #send_request(2)
    #send_request(4)
    #send_request(6)
    #send_request(8)
    #send_request(10)
    #send_request(12) 

        
    
    #index, msg = receive_msg()
    #time.sleep(10)
    #print "LOOPING " 
        
print "Close"
client.close()

