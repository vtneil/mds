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

entrypoint=$(docker inspect --format='{{.Config.Entrypoint}}' "$image")
entrypoint=${entrypoint:1:-1}
if [ "$entrypoint" == "null" ]; then
    entrypoint="[]"
fi

container_id=$(docker run -d -v "$bind1" -v "$bind2" --entrypoint "/bin/sh" "$image" -c "tail -f /dev/null")

if [ -z "$container_id" ]; then
    exit 2
fi

until [ "$(docker inspect --format '{{.State.Status}}' "$container_id")" == "running" ]; do
  :
done

/usr/bin/time -f "%e" -o temp_time.txt -- docker exec "$container_id" /bin/sh -c "cd $workdir; eval $entrypoint $args"
echo "$image,$(cat temp_time.txt)" >> "$out_file".times.csv
rm temp_time.txt

docker kill "$container_id" > /dev/null
docker rm "$container_id" > /dev/null
