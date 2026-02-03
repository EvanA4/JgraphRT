# Jgraph Raytracer

A minimalistic, multithreaded ray tracer written entirely in the C programming language. 

Here's a GIF travelling close to a black hole.
![A Really Cool Black Hole Pierce](https://github.com/EvanA4/JgraphRT/blob/main/pierce.gif?raw=true)

...and here's another panning the side of it.
![A Really Cool Black Hole Slide](https://github.com/EvanA4/JgraphRT/blob/main/silde.gif?raw=true)

To create the binary executable for the C ray tracer, just run `make` while in this repository's root directory. The makefile will automatically compile a GIF similar to the piercing GIF. Running `bin/rayt` without arguments will give you information on how to run the program:

```
Usage: bin/kerr [schwarz|sphere]
Flags:
        a/x:    x for start/ending pos of camera
        b/y:    y for start/ending pos of camera
        c/z:    z for start/ending pos of camera
        t:                   x for dir of camera
        u:                   y for dir of camera
        v:                   z for dir of camera
        f:                            camera fov
        q:                number of steps in GIF
        s:                             task size
        n:                     number of threads
```

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

> [!WARNING]  
> Make sure to run `video.sh` with the same `q` value as `rayt`. The default for `rayt` is 30. The shell script also requires a python environment with the `pillow` package.