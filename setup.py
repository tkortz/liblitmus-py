from distutils.core import setup, Extension

liblitmus = Extension(
    "liblitmus",
    include_dirs = ["/home/tanya/liblitmus-ext-res/include", "/home/tanya/liblitmus-ext-res/arch/x86/include"],
    library_dirs = ["/home/tanya/liblitmus-ext-res/", "/home/tanya/liblitmus-ext-res/src"],
    libraries = ["litmus"],
    sources = ["liblitmus.c"]
)

description = "Python wrapper for Liblitmus."
setup(name="Liblitmus",
      version="1.0",
      description=description,
      ext_modules=[liblitmus])