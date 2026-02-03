for i in {0..59}
do
    ./jgraph -P data/$i.jgr | ps2pdf - | convert -density 300 - -quality 100 data/$i.png
    rm -f data/$i.jgr
done