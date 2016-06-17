#!/bin/bash

set -e  # exit on error
set -u  # exit on unset error

sudo apt-get update

p="libeigen3-dev libopencv-dev"

echo -e "[Install]" ${p}
sudo apt-get install ${p}
