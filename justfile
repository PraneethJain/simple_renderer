br spp: build (run-all spp)

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build:
    make -j8 -C src/build

run-all spp: 
    src/build/render "assignment_3/Question 1/directional_light.json" temp.png {{spp}} 0
    imv temp.png
    rm temp.png