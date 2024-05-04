# add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# add_library(definitionlib "${CMAKE_CURRENT_SOURCE_DIR}/definition.c" "${CMAKE_CURRENT_SOURCE_DIR}/definition.h")
# target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/variable")
# target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/mem")
# target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/expression")
# target_include_directories(definitionlib PUBLIC "${CMAKE_SOURCE_DIR}/linked_list")
from argparse import ArgumentParser
from os import makedirs as mkdir
from os.path import join

PROJECT_ROOT_DIR: str = ""


def generate_toplevel_subdir(subdir, files):
    mkdir(subdir)
    cmake_file = join(PROJECT_ROOT_DIR, subdir, "CMakeLists.txt")

    # TODO: create files in directory and add them to CMakeLists.txt

    with open(cmake_file, "w") as f:
        lines = [
            "add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)",
            "add_library(definitionlib)",
        ]


if __name__ == "__main__":
    args = ArgumentParser()
    args.add_argument("--root ", type=str, required=True)
    args.add_argument("--create-module", type=str)
    args.add_argument("--files", nargs="+", type=str)
    params = args.parse_args()

    PROJECT_ROOT_DIR = params.root

    if params.create_module is not None:
        generate_toplevel_subdir(params.create_module, params.files)
