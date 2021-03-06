from setuptools import setup
from Cython.Build import cythonize
from distutils.extension import Extension
import os

CPP_PATH = os.path.join("..", "..", "cpp", "Chess")


def find_cpp_files(folder_path: str, exceptions: list[str] = []) -> list[str]:
    files = []
    for file_name in os.listdir(folder_path):
        if file_name.endswith(".cpp") and file_name not in exceptions:
            files.append(os.path.join(folder_path, file_name))
    return files


ext_modules = [
    Extension(
        name="chesspy",
        sources=["chesspy/wrapper.pyx",
                 *find_cpp_files(CPP_PATH, ["Chess.cpp", "wrapper.cpp"])],
        include_dirs=[CPP_PATH],
        language="c++"
    )
]


ext_modules = cythonize(ext_modules)

setup(
    name="chesspy",
    ext_modules=ext_modules
)
