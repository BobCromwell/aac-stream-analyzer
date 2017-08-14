cmake_minimum_required (VERSION 2.8)
project (aac-stream-analyzer)
aux_source_directory(. DIR_SRCS)
add_executable(aac-stream-analyzer  ${DIR_SRCS})
