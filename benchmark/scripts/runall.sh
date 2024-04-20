while IFS= read -r bench_name; do
  echo "Started benchmark $bench_name...";

  while IFS= read -r image; do
    echo "Measuring test $bench_name:$image";
    ./benchmark/scripts/better_bench.sh ./benchmark/output/"$bench_name" "$image" ./data/input/:/input/ ./data/docker/:/output/ /input/"$bench_name" /output/tmp.out ;
  done < ./benchmark/other/images

  echo "Done benchmark $bench_name...";
done < ./benchmark/other/inputstr
