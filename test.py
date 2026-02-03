import os
from PIL import Image

# jgrs = [img for img in os.listdir(image_folder) if img.endswith(".jgr")]
# for jgr in jgrs:
#     os.remove(image_folder + '/' + jgr)

images = []

for k in range(60):
    fname = f"data/{k}.png"
    input_image = Image.open(fname)
    box = (554, 1450, 1997, 2263)
    if input_image.mode != "RGB":
        input_image = input_image.convert("RGB")
    if input_image.size != (1997-554, 2263-1450):
        input_image = input_image.crop(box)
    images.append(input_image)

images[0].save("video.gif", save_all=True, append_images=images[1:], optimize=False, duration=1000, loop=0)