br spp: build (run-all spp)

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build:
    make -j8 -C src/build

run-all spp: (run-q1 spp)

run-q1 spp:
    src/build/render "assignment_3/Question 1/directional_light.json" "outputs/q1.png" {{spp}} 0

run-q2 spp: build
    src/build/render "assignment_3/Question 2/scene1.json" "outputs/q2_1.png" {{spp}} 0
    src/build/render "assignment_3/Question 2/scene2.json" "outputs/q2_2.png" {{spp}} 0
    src/build/render "assignment_3/Question 2/scene3.json" "outputs/q2_3.png" {{spp}} 0
    src/build/render "assignment_3/Question 2/scene4.json" "outputs/q2_4.png" {{spp}} 0