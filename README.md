# Jgraph Raytracer

A minimalistic, multithreaded ray tracer written entirely in the C programming language. 

Here's an image of a black hole I rendered with it.
![A Really Cool Black Hole](https://github.com/EvanA4/C-Ray-Tracer/blob/main/schwarz.png?raw=true)

...and here's an image of a sphere.
![A Really Cool Sphere](https://github.com/EvanA4/C-Ray-Tracer/blob/main/sphere.png?raw=true)

To create the binary executable, just run `make` while in this repository's root directory. Running `bin/rayt` will give you information on how to run the program.

To render the schwarzschild black hole as seen above, run the following command:
```
bin/rayt schwarz -w1920 -h1080 -s512 -x1.6 -y.8 -z-8 -t-.1 -u-.1 -v1 -m myschwarz
```

To render the sphere image as seen above, run the following command:
```
bin/rayt sphere -w1920 -h1080 -x3 -y1 -z6 -t-1.3 -u-.2 -v-2 -m mysphere
```

ffmpeg -f image2 -framerate 2 -i mp4ex/%d.png -vcodec libx264 -crf 22 video.mp4
96 x 54
./jgraph -P example.jgr | ps2pdf - > example.pdf
./jgraph -P myschwarz.jgr | ps2pdf - > example.pdf
./jgraph -P myschwarz.jgr | ps2pdf - | convert -density 300 - -quality 100 example.png
bin/rayt schwarz -w96 -h54 -x3 -y1 -z6 -t-1.3 -u-.2 -v-2 -m myschwarz
ffmpeg -i myschwarz.tga myschwarz.png