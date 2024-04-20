#!/bin/bash
# better_bench.sh: More accurate benchmark, perhaps?

if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <output-file> <docker-image> <bind-1> <bind-2> <args...?>"
    exit 1
fi

out_file="$1"
shift

image="$1"
shift

bind1="$1"
shift

bind2="$1"
shift

args=""
for arg in "$@"; do
    args+="'$arg' "
done

workdir=$(docker inspect --format='{{.Config.WorkingDir}}' "$image")
if [ -z "$workdir" ]; then
    workdir="/"
fi

init_time=$(date +%s%3N)

container_id=$(docker run -d -v "$bind1" -v "$bind2" --entrypoint "/bin/sh" "$image" -c "tail -f /dev/null")

if [ -z "$container_id" ]; then
    exit 2
fi

entrypoint_str=$(docker inspect --format='{{json .Config.Entrypoint}}' "$image")
entrypoint=$(python -c "print( ' '.join( [e if '$' not in e else e.split(' ')[0] for e in $entrypoint_str if e not in ['/bin/sh', '-c']]) )")
command="cd $workdir; $entrypoint $args"

echo "ENTRYPOINT $entrypoint"
echo "COMMAND $command"

until [ "$(docker inspect --format '{{.State.Status}}' "$container_id")" == "running" ]; do
  :
done

start_time=$(date +%s%3N)
timeout 300 docker exec "$container_id" /bin/sh -c "$command"
end_time=$(date +%s%3N)

time_ms_1=$((end_time - start_time))

start_time=$(date +%s%3N)
timeout 300 docker exec "$container_id" /bin/sh -c "$command"
end_time=$(date +%s%3N)

time_ms_2=$((end_time - start_time))

start_time=$(date +%s%3N)
timeout 300 docker exec "$container_id" /bin/sh -c "$command"
end_time=$(date +%s%3N)

time_ms_3=$((end_time - start_time))

docker kill "$container_id" > /dev/null
docker rm "$container_id" > /dev/null

kill_time=$(date +%s%3N)

proc_time_ms=$((kill_time - init_time))

echo "$image,$proc_time_ms,$time_ms_1,$time_ms_2,$time_ms_3" | tee -a "$out_file".times.csv
