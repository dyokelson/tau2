#!/bin/bash
set -x

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

export SOS_CMD_PORT=22501
export SOS_WORK=${DIR}
export SOS_EVPATH_MEETUP=${DIR}
export SOS_DISCOVERY_DIR=${DIR}

export TAU_PLUGINS=libTAU-sos-plugin.so
#export TAU_PLUGINS_PATH=../../x86_64/lib/shared-mpi-pthread-pdt-sos
export TAU_PLUGINS_PATH=../../ibm64linux/lib/shared-mpi-pthread-pdt-sos


start_sos_daemon()
{
    # start the SOS daemon

    echo "Work directory is: $SOS_WORK"
    rm -rf sosd.00000.* profile.* dump.*
    #export SOS_IN_MEMORY_DATABASE=1
    #daemon="sosd -l 0 -a 1 -k 0 -r aggregator -w ${SOS_WORK}"
    daemon="sosd -l 0 -a 1 -k 0 -r aggregator -w ${SOS_WORK}"
    echo ${daemon}
    ${daemon} >& sosd.log &
    sleep 1
}

stop_sos_daemon()
{
    # shut down the daemon.
    if pgrep -x "sosd" > /dev/null; then
        #sosd_stop
	sosd_stop >& /dev/null
    fi
    sleep 5
}


stop_sos_daemon
./clean.sh
start_sos_daemon

# to use periodic, enable this variable, and comment out the
# TAU_SOS_send_data() call in matmult.c.
unset TAU_VERBOSE
export TAU_SOS_PERIODIC=1



#export TAU_METRICS=TIME,LIKWID_L1D_REPLACEMENT:PMC0


#unset TAU_SOS_PERIODIC
mpirun -np 2 tau_exec -T pdt,mpi,sos,pthread -sos ./matmult 


echo "sleep 5"
sleep 5

echo "Launch PyCOOLR"
./pycoolr -tool=sos >& /dev/null

pkill mpirun


stop_sos_daemon
showdb

