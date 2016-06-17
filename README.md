libcorvis
=========

This is a library that implements monocular visual-inertial filter. We build
off on a number of excellent libraries such as [FAST](https://github.com/uzh-rpg/fast.git),
[Sophus](https://github.com/strasdat/Sophus.git) and [SVO](https://github.com/uzh-rpg/rpg_svo).
We use [LCM](https://github.com/lcm-proj/lcm) for message passing and [libbot](https://github.com/RobotLocomotion/libbot)
for visualization.

### Setup
------------

1. To begin, download the project as

    ```
    git clone --recursive https://github.com/pratikac/libcorvis
    ```

2. Dependencies (for Debian-based systems) are installed by executing in the ``install`` folder.

    ```
    ./setup.sh
    ```
    
This will add a few lines to your .bashrc and you should see an environment variable named ``CORVIS``.

### Build
-------------

1. We use the [PODS](http://sourceforge.net/projects/pods) software policies to build this project. Simply call

    ```
    make -jx
    ```

to build ``libcorvis`` using CMake.

2. For debugging, use

    ```
    make BUILD_TYPE=Debug
    ```

for the entire codebase. This is not recommended in general. To build individual modules / files in debug mode,
simply add ``add_definitions(-g)`` to their respective CMakeLists.txt.

### Run
---------

1. We use the excellent utility ``bot-procman-sheriff`` to use ``libcorvis``. Excecute

    ```
    bot-procman-sheriff -l ${CORVIS}/config/corvis.procman
    ```

to start a ``sheriff-deputy`` pair on your computer.
