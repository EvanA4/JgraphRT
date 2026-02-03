# Jgraph Raytracer

A minimalistic, multithreaded ray tracer written entirely in the C programming language. 

Here's a GIF travelling close to a black hole.
![A Really Cool Black Hole Pierce](https://github.com/EvanA4/JgraphRT/blob/main/pierce.gif?raw=true)

...and here's another panning the side of it.
![A Really Cool Black Hole Slide](https://github.com/EvanA4/JgraphRT/blob/main/slide.gif?raw=true)

To create the binary executable for the C ray tracer, just run `make` while in this repository's root directory. Running `bin/rayt` will give you information on how to run the program.

To render the schwarzschild black hole as seen above, run the following command:
```
bin/rayt schwarz -q60
video.sh 60
```

To render the panning shot, run the following commands:
```
bin/rayt schwarz -a-5 -b.5 -c8 -x5 -y.5 -z8 -t0 -u0 -v-1 -q60
video.sh 60
```