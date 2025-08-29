import zlib
import os

from PIL import Image

class PNGBIN():
    def __init__(self, signature: str = "HOWARDPNGBIN"):
        self.__SIGNATURE = bytes(signature, "utf-8")

    def to_bin(self, image_filepath: str, bin_filename:str|None = None, mode: str|None = None, compression: int = 0) -> None:
        """
        Write an image to a custom .bin format.
        Includes a signature for validation.
        Mode = L, RGB, RGBA
        """
        image = Image.open(image_filepath)
        height, width = image.height, image.width

        # determine mode
        if mode is None:
            if image.mode in ("L", "RGB", "RGBA"):
                mode = image.mode
            else:
                mode = "RGB" 

        # convert image into desired mode
        img = image.convert(mode)

        # encode mode as byte
        mode_map = {"L": 0, "RGB": 1, "RGBA": 2}
        mode_byte = mode_map[mode]

        # extract raw pixel data
        # img_raster = []
        # for i in range(height):
        #     for j in range(width):
        #         pix = img.getpixel((j, i))
        #         if isinstance(pix, int):  # grayscale
        #             img_raster.append(pix)
        #         else:
        #             img_raster.extend(pix)

        # data = bytearray(img_raster)
        data = img.tobytes()

        # compress if needed
        if compression == 1:
            data = zlib.compress(data, level=9)

        if bin_filename is None:
            bin_filename = os.path.splitext(image_filepath)[-2] + ".bin"

        with open(bin_filename, "wb") as f:
            f.write(self.__SIGNATURE)                          # 12-byte signature
            f.write(mode_byte.to_bytes(1, "big"))       # mode
            f.write(height.to_bytes(2, "big"))          # height
            f.write(width.to_bytes(2, "big"))           # width
            f.write(compression.to_bytes(1, "big"))     # compression flag
            f.write(data)                               # pixel data
        
    def __read_bin(self, filepath:str) -> Image.Image:
        """
        Read an image from the custom .bin format.
        Verifies signature and returns a PIL Image.
        """
        with open(filepath, "rb") as f:
            sig = f.read(len(self.__SIGNATURE))
            if sig != self.__SIGNATURE:
                raise ValueError(f"Invalid file format: missing {self.__SIGNATURE.decode(encoding='utf-8')} signature")

            mode_byte = int.from_bytes(f.read(1), "big")
            height = int.from_bytes(f.read(2), "big")
            width = int.from_bytes(f.read(2), "big")
            compression = int.from_bytes(f.read(1), "big")
            data = f.read()

        # decode mode
        mode_map = {0: "L", 1: "RGB", 2: "RGBA"}
        if mode_byte not in mode_map:
            raise ValueError(f"Unknown mode byte: {mode_byte}")
        mode = mode_map[mode_byte]

        # decompress if needed
        if compression == 1:
            data = zlib.decompress(data)

        # create new image
        # img = Image.new(mode, (width, height))
        # num_channels = len(mode)

        # for i in range(height):
        #     for j in range(width):
        #         start = num_channels * (i * width + j)
        #         pix = tuple(data[start:start+num_channels])
        #         if num_channels == 1:
        #             pix = pix[0]  # grayscale is single int
        #         img.putpixel((j, i), pix)

        img = Image.frombytes(mode, (width, height), data)
        return img
    
    def bin_to_png(self, bin_filepath:str, filename:str) -> None:
        extension = os.path.splitext(bin_filepath)[-1].lower() 
        if extension != ".bin":
            raise ValueError(f"Invalid file path {extension}, was expecting bin")
        img = self.__read_bin(bin_filepath)
        img.save(filename)

    def show(self, bin_filepath:str) -> None: 
        extension = os.path.splitext(bin_filepath)[-1].lower() 
        if extension != ".bin":
            raise ValueError(f"Invalid file path {extension}, was expecting bin")
        img = self.__read_bin(bin_filepath)
        img.show()

# Example usage:
if __name__ == "__main__":
    converter = PNGBIN()
    converter.to_bin("/Users/Brody Howard/Documents/GitHub/ImageCompressor/RECEIPT.png", mode = "L")
    converter.show("RECEIPT.bin")