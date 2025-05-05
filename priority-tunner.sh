#!/bin/sh
#
# Change scheduling priority for processes or interrupts (BusyBox compatible)
#

PROC=`ps -eLo pid,cmd`
VERBOSE=0

print_help() {
	echo "Usage: $0 [OPTIONS] COMMAND [ARGS...]"
	echo
	echo "Options:"
	echo "  -v, --verbose         Enable verbose logging"
	echo "  -h, --help            Show this help message"
	echo
	echo "Commands:"
	echo "  process <prio> <name>     Set real-time priority for a process"
	echo "  interrupt <prio> <irq>    Set real-time priority for an interrupt handler"
	echo
	echo "Examples:"
	echo "  $0 -v process 80 my_process"
	echo "  $0 interrupt 70 eth0"
}

for arg in "$@"; do
	case "$arg" in
		-v|--verbose) VERBOSE=1 ;;
		-h|--help) print_help; exit 0 ;;
	esac
done

log() {
	[ "$VERBOSE" -ne 0 ] && echo "$@"
}

setprio_process()
{
	# for verbose debugging use -v in the chrt command
	PIDs=`echo $PROC | grep -o -e "[0-9]* [\[]*$2" | grep -o -e "[0-9]* "`
	
	if [ -z "${PIDs}" ]; then
		echo "ERROR! $2 not found! Can't change priority."
	else

		SCHED="f"
		if [ "$1" == "0" ]; then
			SCHED="o";
		fi
		
		for pid in $(echo ${PIDs}); do
			log "Changing $2 pid $pid to prio $1 sched $SCHED"
			chrt -p -$SCHED $1 $pid
		done
	fi
}

setprio_interrupt()
{
	prio=$1
	name=$2

	matches=`ps -eLo comm | grep -w "irq" | grep "${name}"`
	
	if [ "$matches" == "" ]; then
		echo "no match for $name interrupt"
	else
	
		for entry in $matches; do
			log -n "int $name => irq $entry, "
			setprio_process $prio $entry
		done
	fi
}

if [ $# -eq 0 ]; then
    print_help
    exit 0
fi

while [ $# -gt 0 ]; do
	case "$1" in
		-v|--verbose) shift ;;
        -h|--help) shift ;;
		process)
			shift
			[ $# -lt 2 ] && echo "Usage: $0 process <priority> <process_name>" && exit 1
            log "Changing process \"$1\" to priority $2"
			setprio_process "$1" "$2"
			shift 2
			;;
		interrupt)
			shift
			[ $# -lt 2 ] && echo "Usage: $0 interrupt <priority> <interrupt_name>" && exit 1
            log "Changing interrupt \"$1\" to priority $2"
			setprio_interrupt "$1" "$2"
			shift 2
			;;
		*)
			echo "Unknown argument: $1"
			print_help
			exit 1
			;;
	esac
done

