for i in $(seq 0 $(($1-1)))
do
    ./jgraph -P data/$i.jgr | ps2pdf - | convert -density 300 - -quality 100 data/$i.png
    rm -f data/$i.jgr
done
python movie.py $1
rm -rf data