default: build run-all

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build:
    make -j8 -C src/build

run-all: run-cornell run-donuts run-tabletop

run-cornell: run-cornell-low run-cornell-high

run-cornell-low:
    src/build/render "assignments/assignment_1/Question 2/CornellBox/scene_lo_poly.json" outputs/cornell_low.png

run-cornell-high:
    src/build/render "assignments/assignment_1/Question 2/CornellBox/scene_hi_poly.json" outputs/cornell_high.png

run-donuts:
    src/build/render "assignments/assignment_1/Question 2/Donuts/scene.json" outputs/donuts.png

run-tabletop:
    src/build/render "assignments/assignment_1/Question 2/TableTop/scene.json" outputs/tabletop.png

