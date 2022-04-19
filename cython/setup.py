from setuptools import setup
from Cython.Build import cythonize
from distutils.extension import Extension
import os

CPP_PATH = os.path.join("..", "cpp", "Chess")


def find_cpp_files(folder_path: str) -> list[str]:
    files = []
    for path in os.listdir(folder_path):
        if path.endswith(".cpp"):
            files.append(os.path.join(folder_path, path))
    return files


print(find_cpp_files(CPP_PATH))

ext_modules = [
    Extension(
        name="chess",
        sources=["wrapper.pyx", *find_cpp_files(CPP_PATH)],
        include_dirs=[CPP_PATH],
        language="c++"
    )
]


ext_modules = cythonize(ext_modules)

setup(
    name="Chess",
    ext_modules=ext_modules
)
