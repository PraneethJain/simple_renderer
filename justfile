default: build (run-all "3")

alias br := build-run-all

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build-run-all variant: build (run-all variant)

build:
    make -j8 -C src/build

run-all variant: (run-cornell variant) (run-donuts variant) (run-tabletop variant)

run-cornell variant: (run-cornell-low variant) (run-cornell-high variant)

run-cornell-low variant:
    src/build/render "assignments/assignment_1/Question 2/CornellBox/scene_lo_poly.json" outputs/cornell_low.png {{variant}}

run-cornell-high variant:
    src/build/render "assignments/assignment_1/Question 2/CornellBox/scene_hi_poly.json" outputs/cornell_high.png {{variant}}

run-donuts variant:
    src/build/render "assignments/assignment_1/Question 2/Donuts/scene.json" outputs/donuts.png {{variant}}

run-tabletop variant:
    src/build/render "assignments/assignment_1/Question 2/TableTop/scene.json" outputs/tabletop.png {{variant}}

