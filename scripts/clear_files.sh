# create a vairable called scheduler.log
LOG_FILE="./log/scheduler.log"
PERF_FILE="./log/scheduler.perf"
# check if file exists
if [ -f $LOG_FILE ]; then
    rm $LOG_FILE
fi
if [ -f $PERF_FILE ]; then
    rm $PERF_FILE
fi