br spp: build (run-all spp)

prep:
    rm -rf src/build
    mkdir src/build
    mkdir -p outputs
    cmake -S src -B src/build

build:
    make -j8 -C src/build

run-all spp: (run-q1 spp) (run-q2 spp) (run-q3 spp)

run-q1 spp:
    src/build/render "assignment_3/Question 1/directional_light.json" "outputs/q1.png" {{spp}} 0

run-q2 spp: build
    src/build/render "assignment_3/Question 2/scene1.json" "outputs/q2_1.png" {{spp}} 0
    src/build/render "assignment_3/Question 2/scene2.json" "outputs/q2_2.png" {{spp}} 0
    src/build/render "assignment_3/Question 2/scene3.json" "outputs/q2_3.png" {{spp}} 0
    src/build/render "assignment_3/Question 2/scene4.json" "outputs/q2_4.png" {{spp}} 0

run-q3 spp: build
    src/build/render "assignment_3/Question 3/big.json" "outputs/q3_big_{{spp}}.png" {{spp}} 0
    src/build/render "assignment_3/Question 3/many.json" "outputs/q3_many_{{spp}}.png" {{spp}} 0
    src/build/render "assignment_3/Question 3/med.json" "outputs/q3_med_{{spp}}.png" {{spp}} 0
    src/build/render "assignment_3/Question 3/small.json" "outputs/q3_small_{{spp}}.png" {{spp}} 0