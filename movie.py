import os
from PIL import Image
from sys import argv

images = []

NUM_STEPS = int(argv[1])
for k in range(NUM_STEPS):
    fname = f"data/{k}.png"
    input_image = Image.open(fname)
    box = (554, 1450, 1997, 2263)
    if input_image.mode != "RGB":
        input_image = input_image.convert("RGB")
    if input_image.size != (1997-554, 2263-1450):
        input_image = input_image.crop(box)
    images.append(input_image)

images[0].save("video.gif", save_all=True, append_images=images[1:], optimize=False, duration=1000/NUM_STEPS, loop=0)